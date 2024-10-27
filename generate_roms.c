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

#define ROM_SIZE_BOOT        (1 << 13) // 8 KB
#define ROM_SIZE_INSTRUCTION (1 << 17) // 128 KB
#define ROM_SIZE_ALU         (1 << 19) // 512 KB

// signals
#define LD_I   (1 << 0)
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

#define SEL_C_OE_GPI SEL_C1
#define SEL_C_LD_GPO SEL_C2
#define SEL_C_LD_TF  SEL_C3

#define SIGNALS_ACTIVE_LOW_MASK (LD_I | LD_C | LD_ML | LD_MH | LD_S | OE_MEM | OE_ALU | OE_T)

#define S0_FETCH (OE_MEM | LD_I | INC_M)

#define M (0 << 4)
#define C (1 << 4)

#define LD_CSEL(c) ((uint16_t)((c & 0x1f) << 11) | LD_C)

// constants
#define C_MH A_OE_MH
#define C_ML A_OE_ML
#define C_T  0x2
#define C_A  0x3
#define C_B  0x4
#define C_C  0x5
#define C_D  0x6
#define C_F  A_ADD_F_CF
#define C_IH 0x8
#define C_IL 0x9
#define C_JH 0xa
#define C_JL 0xb
#define C_KH 0xc
#define C_KL 0xd
#define C_SP 0xe
#define C_FF 0xf

// alu operations
typedef enum {
    A_OE_MH    = 0,
    A_OE_ML    = 1,
    A_BOOT     = 2,
    A_ADD      = 3,
    A_UNARY    = 4,
    A_NAND     = 5,
    A_ADD_F    = 6,
    A_ADD_F_CF = 7,
} A;

typedef enum {
    AU_SHR_F     = 0x00,
    AU_1S_LSB    = 0x01,
    AU_SHR       = 0x02,
    AU_SHR_OR_00 = 0xfe,
    AU_SHR_OR_80 = 0xff,
} AU;

// flags, computed by the ALU lookup table, active low
#define F_C (1 << 0) // carry
#define F_Z (1 << 1) // zero
#define F_O (1 << 2) // overflow
#define F_S (1 << 3) // sign

// flip F_O then invert so we think of flags as active high,
#define TF_TO_F(tf) ((~((tf) ^ 0x4)) & 0xf)

#define FLAG_MASK_ANY 0x10

typedef struct {
    union {
        uint8_t r8;

        struct {
            uint8_t rh8;
            uint8_t rl8;
        };

        struct {
            uint8_t flag_mask_set;
            uint8_t flag_mask_unset;
        };
    } src;

    union {
        uint8_t r8;

        struct {
            uint8_t rh8;
            uint8_t rl8;
        };

    } dst;
} Operands;

typedef struct {
    uint8_t i;
    uint8_t s; // 4 bit
    uint8_t ml;
    uint8_t mh;
    uint8_t c;
    uint8_t t;
    uint8_t tf; // 4 bit
    uint8_t gpo;
    uint8_t gpi;
    uint8_t in_rx; // 1 bit
    uint8_t in_miso; // 1 bit
    uint8_t mem[0x10000];
} test_State;

#define test_state_M(state) ((uint16_t)(((state)->mh << 8) | (state)->ml))
#define test_state_M_or_C(state) ((state)->c & 0x10)

typedef const struct {
    const char* customasm;

    Operands operands;

    union {
        uint16_t (*signals)(Operands o, uint8_t s, uint8_t tf);
        uint16_t (*signals_with_m13)(Operands o, uint8_t s, uint8_t tf, uint8_t m13);
    };

    bool (*test)(int permutation, Operands o, char** buffer, test_State* before, test_State* after);
} Instruction;

#include "alu.inc"

#include "instructions.inc"

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

static void generate_alu(const uint8_t rom_boot[ROM_SIZE_BOOT], uint8_t rom_alu[ROM_SIZE_ALU]) {
    for (int i = 0; i < ROM_SIZE_ALU; ++i) {
        uint8_t ml  = i & 0xff;
        uint8_t mh  = (i >> 8) & 0xff;
        A alu_op    = (i >> 8 >> 8) & 0x7;

        rom_alu[i] = signals_alu(rom_boot, ml, mh, alu_op);
    }
}

static void generate_instructions(uint8_t rom_instruction1[ROM_SIZE_INSTRUCTION], uint8_t rom_instruction2[ROM_SIZE_INSTRUCTION], const Instruction is[N_INSTRUCTIONS]) {
    for (int index = 0; index < ROM_SIZE_INSTRUCTION; ++index) {
        I_id    i   = index & 0xff;
        uint8_t s   = (index >> 8) & 0xf;
        uint8_t tf  = (index >> 8 >> 4) & 0xf;
        uint8_t m13 = (index >> 8 >> 4 >> 4) & 1;

        uint16_t signals = 0;

        if (tf == 0) {
            signals = instruction_reset_cold_start(s);

        } else if (i == RESET) {
            assert(is[i].signals_with_m13 && "missing RESET instruction");
            signals = is[i].signals_with_m13(is[i].operands, s, tf, m13);

        } else if (is[i].signals != NULL) {
            signals = is[i].signals(is[i].operands, s, tf);

        } else {
            signals = OE_T | LD_S;

        }

        signals ^= SIGNALS_ACTIVE_LOW_MASK;

        rom_instruction1[index] = signals & 0xff;
        rom_instruction2[index] = (signals >> 8) & 0xff;
    }
}

static size_t generate_customasm_ruledef(size_t size, char ruledef[size], const Instruction is[N_INSTRUCTIONS]) {
    static const char *customasm_ruledef_start = ""
    "#bankdef memory {\n"
    "    #bits     8\n"
    "    #addr     0\n"
    "    #addr_end 0x20000\n" // TODO: ROM_SIZE_BOOT
    "    #outp     0\n"
    "}\n"
    "\n"
    "#ruledef instructions\n{\n";

    static const char *customasm_ruledef_end = "}\n";

    size_t n = 0;

    n += strlcat(ruledef + n, customasm_ruledef_start, size);
    assert(n < size);

    for (int i = 0; i < 0x100; ++i) {
        if (is[i].customasm != NULL) {
            if (strstr(is[i].customasm, "{imm") != NULL) {
                n += (size_t) snprintf(ruledef + n, size, "%s%s%s%02x @ imm\n", "    ", is[i].customasm, " => 0x", i);
                assert(n < size);
            }
            else {
                n += (size_t) snprintf(ruledef + n, size, "%s%s%s%02x\n", "    ", is[i].customasm, " => 0x", i);
                assert(n < size);
            }
        }
    }

    n += strlcat(ruledef + n, customasm_ruledef_end, size);
    assert(n < size);

    return n;
}

int main(void) {
    // read boot rom
    uint8_t rom_boot[ROM_SIZE_BOOT];

    {
        int j = 0;
        int dir = 0;
        for (int i = 0; i < ROM_SIZE_BOOT; i += 2) {
            if (dir) {
                j = j << 1;
                if (j >= 0x80) {
                    j = 0x80;
                    dir = 0;
                }
            } else {
                j = j >> 1;
                if (j <= 1) {
                    j = 1;
                    dir = 1;
                }
            }

            rom_boot[i] = LD_A_I8;
            rom_boot[i + 1] = (uint8_t)j;
        }
    }

    // generate alu rom
    uint8_t rom_alu[ROM_SIZE_ALU];

    generate_alu(rom_boot, rom_alu);

    // test alu
    if (test_alu(rom_alu)) {
        fprintf(stderr, "alu tests failed\n");
        return 1;
    }

    // generate instruction roms
    uint8_t rom_instruction1[ROM_SIZE_INSTRUCTION];
    uint8_t rom_instruction2[ROM_SIZE_INSTRUCTION];

    generate_instructions(rom_instruction1, rom_instruction2, instructions);

    // test instructions
    int n_failed_instruction_tests = 0;
    if ((n_failed_instruction_tests = test_instructions(rom_alu, rom_instruction1, rom_instruction2, instructions))) {
        fprintf(stderr, "%d instruction test(s) failed\n", n_failed_instruction_tests);
        return 1;
    }

    // generate customasm ruledef
    char ruledef[8096];

    size_t ruledef_size = generate_customasm_ruledef(sizeof(ruledef), ruledef, instructions);

    // write outputs to files
    if (write_rom(ROM_SIZE_ALU, rom_alu, "rom_alu.bin")) return 1;
    if (write_rom(ROM_SIZE_INSTRUCTION, rom_instruction1, "rom_instruction1.bin")) return 1;
    if (write_rom(ROM_SIZE_INSTRUCTION, rom_instruction2, "rom_instruction2.bin")) return 1;
    if (write_rom(ruledef_size, (uint8_t*)ruledef, "ruledef.customasm")) return 1;
}
