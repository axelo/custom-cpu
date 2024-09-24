/*

cc  -Werror -Wall -Wpedantic -Wconversion -Wswitch-enum \
    -fsanitize=undefined,integer,nullability -std=c17 \
    --debug generate_roms.c -o generate_roms \
    && ./generate_roms


-Wsign-compare
-Wno-gnu-binary-literal
-Wunused-variable
-Wunused-function
-Wunused-but-set-variable
-Wunused-parameter
-Wduplicate-enum

https://en.wikipedia.org/wiki/NAND_logic
https://en.wikipedia.org/wiki/NOR_logic

*/

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ROM_SIZE_BOOT    (1 << 8)
#define ROM_SIZE_OPCODE  (1 << 17)
#define ROM_SIZE_ALU     (1 << 19)

// signals
#define LD_O   (1 << 0)
#define LD_C   (1 << 1)
#define LD_ML  (1 << 2)
#define LD_MH  (1 << 3)
#define LD_T   (1 << 4)
#define LD_MEM (1 << 5)
#define LD_S   (1 << 6)
#define INC_M  (1 << 7)
#define OE_MEM (1 << 8)
#define OE_ALU (1 << 9)
#define OE_T   (1 << 10)
#define SEL_C0 (1 << 11)
#define SEL_C1 (1 << 12)
#define SEL_C2 (1 << 13)
#define SEL_C3 (1 << 14)
#define SEL_C  (1 << 15)

#define SIGNALS_ACTIVE_LOW_MASK (LD_C | LD_ML | LD_MH | LD_S | OE_MEM | OE_ALU | OE_T)

// alu operations
typedef enum {
    A_BOOT  = 0,
    A_UNARY = 1,
    A_NAND  = 2,
    A_ADD   = 3,
    A_OP_5  = 4,
    A_ADD_F = 5,
    A_OE_MH = 6,
    A_OE_ML = 7,
} A;

typedef enum {
    AU_SHR   = 0x01,
    AU_SHR_F = 0xff,
} AU;

// constants
#define C_A  0x0
#define C_B  0x1
#define C_C  0x2
#define C_D  0x3
#define C_T  0x4
#define C_F  A_ADD_F
#define C_MH A_OE_MH
#define C_ML A_OE_ML
#define C_IH 0x8
#define C_IL 0x9
#define C_JH 0xa
#define C_JL 0xb
#define C_KH 0xc
#define C_KL 0xd
#define C_01 0xe
#define C_FF 0xf

// flags
#define F_Z (1 << 0) // zero
#define F_C (1 << 1) // carry
#define F_O (1 << 2) // overflow
#define F_S (1 << 3) // sign
#define F_B (1 << 7) // boot size hit

// opcodes
typedef enum {
    RESET = 0x00,

    NOP = 0xff,

    LD_A_I8 = 0x01,
    LD_B_I8,
    LD_C_I8,
    LD_D_I8,

    LD_A_B,
    LD_A_C,
    LD_A_D,

    LD_B_A,
    LD_B_C,
    LD_B_D,

    LD_C_A,
    LD_C_B,
    LD_C_D,

    LD_D_A,
    LD_D_B,
    LD_D_C,

    LD_I_I16,
    LD_J_I16,
    LD_K_I16,

    LD_A_AT_I16,
    LD_B_AT_I16,
    LD_C_AT_I16,
    LD_D_AT_I16,

    LD_A_AT_AT_I16,
    LD_B_AT_AT_I16,
    LD_C_AT_AT_I16,
    LD_D_AT_AT_I16,

    LD_A_AT_I,
    LD_B_AT_I,
    LD_C_AT_I,
    LD_D_AT_I,

    LD_A_AT_J,
    LD_B_AT_J,
    LD_C_AT_J,
    LD_D_AT_J,

    LD_A_AT_K,
    LD_B_AT_K,
    LD_C_AT_K,
    LD_D_AT_K,

    LD_A_AT_I_INC,
    LD_B_AT_I_INC,
    LD_C_AT_I_INC,
    LD_D_AT_I_INC,

    LD_A_AT_J_INC,
    LD_B_AT_J_INC,
    LD_C_AT_J_INC,
    LD_D_AT_J_INC,

    LD_A_AT_K_INC,
    LD_B_AT_K_INC,
    LD_C_AT_K_INC,
    LD_D_AT_K_INC,

    LD_AT_I16_A,
    LD_AT_I16_B,
    LD_AT_I16_C,
    LD_AT_I16_D,

    ADD_A_I8,
} O;

#include "./signals_alu.inc"
#include "./signals_opcode.inc"
#include "./customasm_ruledef.inc"

#include "./test_alu.inc"
#include "./test_opcodes.inc"

static int write_rom(size_t size, uint8_t rom[size], const char *filename) {
    FILE *file = fopen(filename, "w");

    if (file == NULL) {
        fprintf(stderr, "Failed to open %s\n", filename);
        return 1;
    }

    size_t write_result = fwrite(rom, size, 1, file);

    if (write_result != 1) {
        fprintf(stderr, "Failed to write rom to file %s\n", filename);
        return 1;
    }

    if (fclose(file) != 0) {
        fprintf(stderr, "Failed to close file %s\n", filename);
        return 1;
    }

    return 0;
}

int main(void) {
    // read boot rom
    uint8_t rom_boot[ROM_SIZE_BOOT] = {
        LD_A_I8, 0xaa,
        LD_AT_I16_A, 0xff, 0xe0,

        LD_I_I16, 0xff, 0xff,
        LD_A_AT_I_INC,
        LD_A_AT_I_INC,
        LD_A_AT_I_INC,
        LD_A_AT_I_INC,

        LD_A_AT_AT_I16, 0x00, 0x07,

        LD_A_AT_I16, 0x00, 16,
        LD_B_AT_I16, 0x00, 17,
        LD_C_AT_I16, 0x00, 19,
        LD_D_AT_I16, 0x00, 20,

        LD_I_I16, 0x00, 0x08,
        LD_J_I16, 0x56, 0x78,
        LD_K_I16, 0xab, 0xcd,

        LD_A_I8, 0x0a,
        LD_B_I8, 0x0b,
        LD_C_I8, 0x0c,
        LD_D_I8, 0x0d,

        LD_A_B,
        LD_A_C,
        LD_A_D,
        LD_A_I8, 0x0a,

        LD_B_A,
        LD_B_C,
        LD_B_D,
        LD_B_I8, 0x0b,

        LD_C_A,
        LD_C_B,
        LD_C_D,
        LD_C_I8, 0x0c,

        LD_D_A,
        LD_D_B,
        LD_D_C,
        LD_D_I8, 0x0d,

        NOP,
    };

    {
        int i = 0;
        for (; i < ROM_SIZE_BOOT; ++i) {
            if (i > 5 && rom_boot[i] == NOP) break;
        }

        for (; i < ROM_SIZE_BOOT; ++i) {
            rom_boot[i] = NOP;
        }
    }

    // generate alu rom
    uint8_t rom_alu[ROM_SIZE_ALU];

    for (int i = 0; i < ROM_SIZE_ALU; ++i) {
        uint8_t ml  = i & 0xff;
        uint8_t mh  = (i >> 8) & 0xff;
        A alu_op    = (i >> 8 >> 8) & 0x7;

        rom_alu[i] = signals_alu(rom_boot, ml, mh, alu_op);
    }

    // generate opcode roms
    uint8_t rom_opcode1[ROM_SIZE_OPCODE];
    uint8_t rom_opcode2[ROM_SIZE_OPCODE];

    for (int i = 0; i < ROM_SIZE_OPCODE; ++i) {
        O o         = i & 0xff;
        uint8_t s   = (i >> 8) & 0xf;
        uint8_t mll = (i >> 8 >> 4) & 0xf;
        uint8_t m7  = (i >> 8 >> 4 >> 4) & 1;

        uint16_t signals = signals_opcode(o, s, mll, m7) ^ SIGNALS_ACTIVE_LOW_MASK;

        rom_opcode1[i] = signals & 0xff;
        rom_opcode2[i] = (signals >> 8) & 0xff;
    }

    // generate customasm ruledef
    char ruledef[8096];

    size_t ruledef_size = customasm_ruledef(sizeof(ruledef), ruledef);

    // test roms
    if (test_alu(rom_alu)) {
        fprintf(stderr, "test_alu failed\n");
        return 1;
    }

    if (test_opcodes(rom_alu, rom_opcode1, rom_opcode2)) {
        fprintf(stderr, "test_opcodes failed\n");
        return 1;
    }

    // write outputs to files
    if (write_rom(ROM_SIZE_ALU, rom_alu, "rom_alu.bin")) return 1;
    if (write_rom(ROM_SIZE_OPCODE, rom_opcode1, "rom_opcode1.bin")) return 1;
    if (write_rom(ROM_SIZE_OPCODE, rom_opcode2, "rom_opcode2.bin")) return 1;
    if (write_rom(ruledef_size, (uint8_t*)ruledef, "ruledef.customasm")) return 1;
}
