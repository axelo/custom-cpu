#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "control_roms.h"
#include "opcodes.h"

#define C_LD_S S_C(2 /*LD_S*/)

#define S_C(c) ((uint16_t)((c & 0x7) << 12))
#define S_OE_IO_C(port) ((uint16_t)(((1 << port) & 0xf) << 12))
#define S_LD_IO_C(port) ((uint16_t)((~(1 << port) & 0xf) << 12))

#define S0_FETCH (OE_MEM | C_LD_S | INC_M)

typedef enum {
    A_BOOT   = 0,
    A_ADD    = 1,
    A_ADD_F  = 2,
    A_NAND   = 3,
    A_OR     = 4,
    A_UNARY  = 5, // Special unary operations, like shift right.
    A_LS     = 6,
    A_RS     = 7,
} A;

typedef enum {
    AU_SHR      = 0xfc,
    AU_SHR_F    = 0xfd,
    AU_FLAGS    = 0xff,
    AU_OR_BIT   = 0x40, // 0x40..0x7f
    AU_BOOT_F   = 0x10,
} AU;

typedef enum {
    C_A    = 0,
    C_B    = 1,
    C_C    = 2,
    C_D    = 3,
    C_E    = 4,
    C_T    = 5,
    C_T_ML = A_LS,
    C_T_MH = A_RS,
} C;

static uint8_t alu_signals(uint8_t ls, uint8_t rs, A op) {
    switch ((A)op) {

    case A_BOOT: return 0;

    case A_ADD:  return (uint8_t)(ls + rs);

    case A_ADD_F: {
        uint16_t q = ls + rs;
        uint8_t cf = q > 0xff;
        uint8_t zf = (q & 0xff) == 0;
        uint8_t sf = (q & 0xff) >> 7;
        uint8_t initf = 1;

        return (uint8_t)((initf << 3) | (sf << 2) | (cf << 1) | (zf << 0));
    }

    case A_NAND: return (uint8_t)~(ls & rs);

    case A_OR:   return (uint8_t)(ls | rs);

    case A_UNARY: {
        switch ((AU)rs) {
        case AU_SHR: return (uint8_t)(ls >> 1);

        case AU_SHR_F: {
            uint8_t q  = ls >> 1;
            uint8_t zf = q == 0;
            uint8_t cf = ls & 1;
            uint8_t sf = q >> 7;
            uint8_t initf = 1;

            return (uint8_t)((initf << 3) | (sf << 2) | (cf << 1) | (zf << 0));
        }

        case AU_FLAGS: {
            return (ls & 0x07) | F_I;
        }

        case AU_BOOT_F: {
            uint8_t zf = 1; // zf and sf at the same time should never make sense elsewhere.
            uint8_t cf = 0;
            uint8_t sf = 1;
            uint8_t initf = 0;

            return (uint8_t)((initf << 3) | (sf << 2) | (cf << 1) | (zf << 0));
        }

        case AU_OR_BIT:

        default: {

            if (rs >= AU_OR_BIT && rs < (AU_OR_BIT + 0x40)) {
                uint8_t write_pos = (rs >> 3) & 0x7;
                uint8_t read_pos  = rs & 0x7;
                uint8_t or_bit = (uint8_t)(((ls >> read_pos) & 1) << write_pos);

                return or_bit;
            } else {
                return 1;
            }
        }
        }
    }

    case A_LS: return ls;

    case A_RS: return rs;

    }
}

static void test_alu(uint8_t alu[ALU_ROM_SIZE]) {
    for (int ls = 0; ls < 0x100; ++ls) {
    for (int rs = 0; rs < 0x100; ++rs) {
    for (uint8_t op = 0; op < 0x8; ++op) {
        uint8_t q = alu[(op << 16) | (rs << 8) | ls];

        uint8_t zf = (q >> 0) & 1;
        uint8_t cf = (q >> 1) & 1;
        uint8_t sf = (q >> 2) & 1;
        uint8_t initf = (q >> 3) & 1;

        switch ((A)op) {

        case A_BOOT: break;

        case A_ADD:
            assert(q == (uint8_t)(ls + rs));
            break;

        case A_ADD_F:
            assert((ls + rs) > 0xff            ? cf : !cf);
            assert((((ls + rs) & 0xff) == 0)   ? zf : !zf);
            assert((((ls + rs) & 0xff) & 0x80) ? sf : !sf);
            assert(initf == 1);
            break;

        case A_NAND:
            assert(q == (uint8_t)(~(ls & rs)));
            break;

        case A_OR:
            assert(q == (uint8_t)(ls | rs));
            break;

        case A_UNARY: {
            switch ((AU)rs) {

            case AU_SHR:
                assert(q == (ls >> 1));
                break;

            case AU_SHR_F: {
                uint8_t expected_c = ls & 1;

                assert(cf == expected_c);
                assert(((ls >> 1) == 0)   ? zf : !zf);
                assert(((ls >> 1) & 0x80) ? sf : !sf);
                assert(initf == 1);
            } break;

            case AU_FLAGS: {
                 assert(q == ((ls & 0x07) | F_I));
             } break;

            case AU_BOOT_F: {
                assert(zf == 1);
                assert(cf == 0);
                assert(sf == 1);
                assert(initf == 0);
            } break;

            case AU_OR_BIT:

            default: {
                if (rs >= AU_OR_BIT && rs < (AU_OR_BIT + 0x40)) {
                    assert(q == (uint8_t)(((ls >> (rs & 0x7)) & 1) << ((rs >> 3) & 0x7)));
                } else {
                    assert(zf == 1);
                    assert(initf == 0);
                }
            } break;
            }
        } break;

        case A_LS:
            assert(q == ls);
            break;

        case A_RS:
            assert(q == rs);
            break;

        }
    }
    }
    }
}

static void fill_alu(uint8_t alu[ALU_ROM_SIZE]) {
    for (uint32_t i = 0; i < ALU_ROM_SIZE; ++i) {
        uint8_t ls = (i >> 0)  & 0xff;
        uint8_t rs = (i >> 8)  & 0xff;
        uint8_t op = (i >> 16) & 0x7;

        alu[i] = alu_signals(ls, rs, op);
    }
}

static void fill_alu_boot_rom(uint8_t alu[ALU_ROM_SIZE], uint8_t boot_rom[BOOT_ROM_SIZE]) {
    for (uint32_t i = 0; i < BOOT_ROM_SIZE; ++i) {
        // Assumes rs:ls (MH:ML) is the first 16 bits of the rom address.
        alu[(A_BOOT << 16) | i] = boot_rom[i];
    }
}

// Requires power on reset on F and S.
static uint16_t control_init_signals(uint8_t s, uint8_t f) {
    if (!(f & F_Z)) {
        switch (s) {
        case 0x0: return OE_C;
        case 0x1: return OE_C  | S_C(0) | SEL_M | LD_C;
        case 0x2: return OE_C  | LD_ML | LD_MH;
        case 0x3: return OE_C  | S_C(1) | LD_C;
        case 0x4: return OE_C  | LD_F  | C_LD_S;
        default:  return OE_C;
        }
    } else {
        switch (s) {
        case 0x0: return OE_C;
        case 0x1: return OE_C   |                  S_C(A_BOOT)  | LD_C;
        case 0x2: return OE_ALU | LD_MEM;
        case 0x3: return OE_MEM | INC_M;
        case 0x4: return OE_ALU | LD_MEM;
        case 0x5: return OE_MEM | INC_M;
        case 0x6: return OE_C   |                  S_C(A_UNARY) | LD_C;
        case 0x7: return OE_ALU | LD_F;
        case 0x8: return OE_C;
        default:  break;
        }

        if (f & F_S) { // A_UNARY defaults to F_Z. AU_BOOT_F (MH is at the expected page) also sets F_S.
            switch (s) {
            case 0x9: return OE_C   |                 S_C(7) | LD_C; // C = 0xff
            case 0xa: return OE_C   | LD_ML | LD_MH;                 // M = 0xffff,
            case 0xb: return OE_C   |                 S_C(0) | LD_C; // C = 0
            case 0xc: return OE_C   | LD_MEM;                        // mem[M] = 0 (SP)
            case 0xd: return OE_C   | S_C(1 /*LD_O*/) | INC_M;       // O = NOP, M = 0
            case 0xe: return OE_C   |                 S_C(A_ADD_F) | LD_C; // ALU = 0 + 0
            case 0xf: return OE_ALU | LD_F; // Set ~F_I, taking us out from init stage.

            // Remaining steps will continue from the NOP instruction.
            default: return OE_C;
            }
        } else return OE_C | C_LD_S;
    }
}

static uint16_t instr_ld_r8_i8(C r8, uint8_t s) {
    switch (s) {
    case 1: return OE_MEM | LD_T   | S_C(r8) | SEL_C | LD_C | INC_M;
    case 2: return OE_T   | LD_MEM |           SEL_M | LD_C;
    case 3: return OE_C   | C_LD_S;
    default: return OE_C;
    }
}

static uint16_t instr_ld_r16_i16(C hi_r8, C lo_r8, uint8_t s) {
    switch (s) {
    case 1: return OE_MEM | LD_T   | S_C(hi_r8) | SEL_C | LD_C | INC_M;
    case 2: return OE_T   | LD_MEM |              SEL_M | LD_C;
    case 3: return OE_MEM | LD_T   | S_C(lo_r8) | SEL_C | LD_C | INC_M;
    case 4: return OE_T   | LD_MEM |              SEL_M | LD_C;
    case 5: return OE_C   | C_LD_S;
    default: return OE_C;
    }
}

static uint16_t instr_ld_r8_flags(C r8, uint8_t f, uint8_t s) {
    switch (s) {
    case 0x1: return OE_C   |                   S_C(C_T_ML)  | SEL_C | LD_C;
    case 0x2: return OE_ALU | LD_MEM          | S_C(C_T_MH)  | SEL_C | LD_C;
    case 0x3: return OE_ALU | LD_MEM          | S_C(f)       | SEL_C | LD_C;
    case 0x4: return OE_C   | LD_ML           | S_C(AU_FLAGS)| SEL_C | LD_C;
    case 0x5: return OE_C   | LD_MH           | S_C(A_UNARY) | SEL_C | LD_C;
    case 0x6: return OE_ALU | LD_T            | S_C(r8)      | SEL_C | LD_C;
    case 0x7: return OE_T   | LD_MEM          | S_C(C_T_ML)  | SEL_C | LD_C;
    case 0x8: return OE_MEM | LD_ML           | S_C(C_T_MH)  | SEL_C | LD_C;
    case 0x9: return OE_MEM | LD_MH                          | SEL_M | LD_C;
    case 0xa: return OE_C   | C_LD_S;
    default:  return OE_C;
    }
}

static uint16_t instr_ld_flags_r8(C r8, uint8_t s) {
    switch (s) {
    case 1: return OE_C   |         S_C(r8) | SEL_C | LD_C;
    case 2: return OE_MEM | LD_F            | SEL_M | LD_C;
    case 3: return OE_C   | C_LD_S;
    default: return OE_C;
    }
}

static uint16_t instr_ld_r8_r8(C dst_r8, C src_r8, uint8_t s) {
    switch (s) {
    case 1: return OE_C   |       S_C(src_r8) | SEL_C | LD_C;
    case 2: return OE_MEM | LD_T;
    case 3: return OE_C   |       S_C(dst_r8) | SEL_C | LD_C;
    case 4: return OE_T   | LD_MEM;
    case 5: return OE_C   |                     SEL_M | LD_C;
    case 6: return OE_C   | C_LD_S;
    default: return OE_C;
    }
}

static uint16_t instr_ld_at_r16_r8(C hi_r8, C lo_r8, C r8, uint8_t s) {
    switch (s) {
    case 0x1: return OE_ALU |          S_C(C_T_ML)  | SEL_C | LD_C;
    case 0x2: return OE_ALU | LD_MEM | S_C(C_T_MH)  | SEL_C | LD_C;
    case 0x3: return OE_ALU | LD_MEM | S_C(hi_r8)   | SEL_C | LD_C;
    case 0x4: return OE_MEM | LD_MH  | S_C(lo_r8)   | SEL_C | LD_C;
    case 0x5: return OE_MEM | LD_ML  | S_C(r8)      | SEL_C | LD_C;
    case 0x6: return OE_MEM | LD_T                  | SEL_M | LD_C;
    case 0x7: return OE_T   | LD_MEM | S_C(C_T_ML)  | SEL_C | LD_C;
    case 0x8: return OE_MEM | LD_ML  | S_C(C_T_MH)  | SEL_C | LD_C;
    case 0x9: return OE_MEM | LD_MH                 | SEL_M | LD_C;
    case 0xa: return OE_C   | C_LD_S;
    default:  return OE_C;
    }
}

static uint16_t instr_ld_r8_at_r16_inc(C r8, C hi_r8, C lo_r8, uint8_t s) {
    switch (s) {
    case 0x1: return OE_ALU |          S_C(C_T_ML)  | SEL_C | LD_C;
    case 0x2: return OE_ALU | LD_MEM | S_C(C_T_MH)  | SEL_C | LD_C;
    case 0x3: return OE_ALU | LD_MEM | S_C(hi_r8)   | SEL_C | LD_C;
    case 0x4: return OE_MEM | LD_MH  | S_C(lo_r8)   | SEL_C | LD_C;         // MH = mem[hi]
    case 0x5: return OE_MEM | LD_ML                 | SEL_M | LD_C;         // ML = mem[lo]
    case 0x6: return OE_MEM | LD_T   | S_C(r8)      | SEL_C | LD_C | INC_M; //  T = mem[M], M++
    case 0x7: return OE_T   | LD_MEM | S_C(C_T_ML)  | SEL_C | LD_C;         // mem[r8] = T
    case 0x8: return OE_ALU | LD_T   | S_C(lo_r8)   | SEL_C | LD_C;         // T = ML
    case 0x9: return OE_T   | LD_MEM | S_C(C_T_MH)  | SEL_C | LD_C;         // M[lo] = T
    case 0xa: return OE_ALU | LD_T   | S_C(hi_r8)   | SEL_C | LD_C;         // T = MH
    case 0xb: return OE_T   | LD_MEM | S_C(C_T_ML)  | SEL_C | LD_C;         // M[hi] = T
    case 0xc: return OE_MEM | LD_ML  | S_C(C_T_MH)  | SEL_C | LD_C;
    case 0xd: return OE_MEM | LD_MH                 | SEL_M | LD_C;
    case 0xe: return OE_C   | C_LD_S;
    default:  return OE_C;
    }
}

static uint16_t instr_ld_at_i16_r8(C r8, uint8_t s) {
    switch (s) {
    case 0x1: return OE_MEM | LD_T   | S_C(C_T)     | SEL_C | LD_C | INC_M;
    case 0x2: return OE_T   | LD_MEM |                SEL_M | LD_C;
    case 0x3: return OE_MEM | LD_T   | S_C(C_T_ML)  | SEL_C | LD_C | INC_M;
    case 0x4: return OE_ALU | LD_MEM | S_C(C_T_MH)  | SEL_C | LD_C;
    case 0x5: return OE_ALU | LD_MEM | S_C(C_T)     | SEL_C | LD_C;
    case 0x6: return OE_MEM | LD_MH;                                        //     MH = mem[hi]
    case 0x7: return OE_T   | LD_ML  | S_C(r8)      | SEL_C | LD_C;         //     ML = mem[lo]
    case 0x8: return OE_MEM | LD_T   |                SEL_M | LD_C;         //      T = mem[r8]
    case 0x9: return OE_T   | LD_MEM | S_C(C_T_ML)  | SEL_C | LD_C;         // mem[M] = T
    case 0xa: return OE_MEM | LD_ML  | S_C(C_T_MH)  | SEL_C | LD_C;
    case 0xb: return OE_MEM | LD_MH                 | SEL_M | LD_C;
    case 0xc: return OE_C   | C_LD_S;
    default:  return OE_C;
    }
}

static uint16_t instr_ld_r8_at_i16(C r8, uint8_t s) {
    switch (s) {
    case 0x1: return OE_MEM | LD_T   | S_C(C_T)     | SEL_C | LD_C | INC_M;
    case 0x2: return OE_T   | LD_MEM |                SEL_M | LD_C;
    case 0x3: return OE_MEM | LD_T   | S_C(C_T_ML)  | SEL_C | LD_C | INC_M;
    case 0x4: return OE_ALU | LD_MEM | S_C(C_T_MH)  | SEL_C | LD_C;
    case 0x5: return OE_ALU | LD_MEM | S_C(C_T)     | SEL_C | LD_C;
    case 0x6: return OE_MEM | LD_MH;                                        //    MH = mem[hi]
    case 0x7: return OE_T   | LD_ML                 | SEL_M | LD_C;         //    ML = mem[lo]
    case 0x8: return OE_MEM | LD_T   | S_C(r8)      | SEL_C | LD_C;         //     T = mem[M]
    case 0x9: return OE_T   | LD_MEM | S_C(C_T_ML)  | SEL_C | LD_C;         // M[r8] = T
    case 0xa: return OE_MEM | LD_ML  | S_C(C_T_MH)  | SEL_C | LD_C;
    case 0xb: return OE_MEM | LD_MH                 | SEL_M | LD_C;
    case 0xc: return OE_C   | C_LD_S;
    default:  return OE_C;
    }
}

static uint16_t instr_ld_at_r16_inc_r8(C hi_r8, C lo_r8, C r8, uint8_t s) {
    switch (s) {
    case 0x1: return OE_ALU |          S_C(C_T_ML)  | SEL_C | LD_C;
    case 0x2: return OE_ALU | LD_MEM | S_C(C_T_MH)  | SEL_C | LD_C;
    case 0x3: return OE_ALU | LD_MEM | S_C(hi_r8)   | SEL_C | LD_C;
    case 0x4: return OE_MEM | LD_MH  | S_C(lo_r8)   | SEL_C | LD_C;         // MH = mem[hi]
    case 0x5: return OE_MEM | LD_ML  | S_C(r8)      | SEL_C | LD_C;         // ML = mem[lo]
    case 0x6: return OE_MEM | LD_T                  | SEL_M | LD_C;         //  T = mem[r8]
    case 0x7: return OE_T   | LD_MEM | S_C(C_T_ML)  | SEL_C | LD_C | INC_M; // mem[M] = T, M++
    case 0x8: return OE_ALU | LD_T   | S_C(lo_r8)   | SEL_C | LD_C;         // T = ML
    case 0x9: return OE_T   | LD_MEM | S_C(C_T_MH)  | SEL_C | LD_C;         // M[lo] = T
    case 0xa: return OE_ALU | LD_T   | S_C(hi_r8)   | SEL_C | LD_C;         // T = MH
    case 0xb: return OE_T   | LD_MEM | S_C(C_T_ML)  | SEL_C | LD_C;         // M[hi] = T
    case 0xc: return OE_MEM | LD_ML  | S_C(C_T_MH)  | SEL_C | LD_C;
    case 0xd: return OE_MEM | LD_MH                 | SEL_M | LD_C;
    case 0xe: return OE_C   | C_LD_S;
    default:  return OE_C;
    }
}

static uint16_t instr_ld_at_r16_inc_i8(C hi_r8, C lo_r8, uint8_t s) {
    switch (s) {
    case 0x1: return OE_MEM | LD_T   | S_C(C_T_ML)  | SEL_C | LD_C | INC_M; //  T = i8
    case 0x2: return OE_ALU | LD_MEM | S_C(C_T_MH)  | SEL_C | LD_C;
    case 0x3: return OE_ALU | LD_MEM | S_C(hi_r8)   | SEL_C | LD_C;
    case 0x4: return OE_MEM | LD_MH  | S_C(lo_r8)   | SEL_C | LD_C;         // MH = mem[hi]
    case 0x5: return OE_MEM | LD_ML                 | SEL_M | LD_C;         // ML = mem[lo]
    case 0x6: return OE_T   | LD_MEM | S_C(C_T_ML)  | SEL_C | LD_C | INC_M; // mem[M] = T, M++
    case 0x7: return OE_ALU | LD_T   | S_C(lo_r8)   | SEL_C | LD_C;         // T = ML
    case 0x8: return OE_T   | LD_MEM | S_C(C_T_MH)  | SEL_C | LD_C;         // M[lo] = T
    case 0x9: return OE_ALU | LD_T   | S_C(hi_r8)   | SEL_C | LD_C;         // T = MH
    case 0xa: return OE_T   | LD_MEM | S_C(C_T_ML)  | SEL_C | LD_C;         // M[hi] = T
    case 0xb: return OE_MEM | LD_ML  | S_C(C_T_MH)  | SEL_C | LD_C;
    case 0xc: return OE_MEM | LD_MH                 | SEL_M | LD_C;
    case 0xd: return OE_C   | C_LD_S;
    default:  return OE_C;
    }
}

static uint16_t instr_out_port_r8(uint8_t port, C r8, uint8_t s) {
    assert(port < 4);
    switch (s) {
    case 0x1: return OE_C   |        S_C(r8)   | SEL_C | LD_C; // C = r8, M -> C
    case 0x2: return OE_MEM | LD_T | S_LD_IO_C(port)   | LD_C; // T = mem[C], C = port
    case 0x3: return OE_T   | S_C(4)/*LD_IO*/;                 // io[C] = T
    case 0x4: return OE_C   |                    SEL_M | LD_C;
    case 0x5: return OE_C   | C_LD_S;
    default:  return OE_C;
    }
}

static uint16_t instr_out_port_i8(uint8_t port, uint8_t s) {
    assert(port < 4);
    switch (s) {
    case 0x1: return OE_MEM | LD_T           | S_LD_IO_C(port) | LD_C | INC_M; // T = mem[PC++], C = port
    case 0x2: return OE_T   | S_C(4 /*LD_IO*/);                                // io[C] = T
    case 0x3: return OE_C   |                            SEL_M | LD_C;
    case 0x4: return OE_C   | C_LD_S;
    default:  return OE_C;
    }
}

static uint16_t instr_in_r8_port(C r8, uint8_t port, uint8_t s) {
    assert(port < 4);
    switch (s) {
    case 0x1: return OE_C  |          S_OE_IO_C(port)   | LD_C; // C = port
    case 0x2: return OE_IO | LD_T   | S_C(r8)   | SEL_C | LD_C; // T = io[C], C = A, M -> C
    case 0x3: return OE_T  | LD_MEM;                            // TODO: Merge with step above
    case 0x4: return OE_C                       | SEL_M | LD_C; // mem[C] = T
    case 0x5: return OE_C  | C_LD_S;
    default:  return OE_C;
    }
}

static uint16_t instr_jmp_i16(bool jump, uint8_t s) {
    if (jump) {
        switch (s) {
        case 0x1: return OE_MEM | LD_T;
        case 0x2: return OE_C   | INC_M;
        case 0x3: return OE_MEM | LD_ML;
        case 0x4: return OE_T   | LD_MH;
        case 0x5: return OE_C   | C_LD_S;
        default:  return OE_C;
        }
    } else {
        switch (s) {
        case 0x1: return OE_C | INC_M;
        case 0x2: return OE_C | INC_M;
        case 0x3: return OE_C | C_LD_S;
        default:  return OE_C;
        }
    }
}

static uint16_t instr_dec_r8(C r8, uint8_t s) {
    switch (s) {
    case 0x1: return OE_ALU |          S_C(C_T_ML)  | SEL_C | LD_C;
    case 0x2: return OE_ALU | LD_MEM | S_C(C_T_MH)  | SEL_C | LD_C;
    case 0x3: return OE_ALU | LD_MEM | S_C(r8)      | SEL_C | LD_C;
    case 0x4: return OE_MEM | LD_ML  | S_C(7)       | SEL_C | LD_C;
    case 0x5: return OE_C   | LD_MH  | S_C(A_ADD_F) | SEL_C | LD_C;
    case 0x6: return OE_ALU | LD_F   | S_C(A_ADD)   | SEL_C | LD_C;
    case 0x7: return OE_ALU | LD_T   | S_C(r8)      | SEL_C | LD_C;
    case 0x8: return OE_T   | LD_MEM;
    case 0x9: return OE_C            | S_C(C_T_ML)  | SEL_C | LD_C;
    case 0xa: return OE_MEM | LD_ML  | S_C(C_T_MH)  | SEL_C | LD_C;
    case 0xb: return OE_MEM | LD_MH                 | SEL_M | LD_C;
    case 0xc: return OE_C   | C_LD_S;
    default:  return OE_C;
    }
}

static uint16_t instr_decc_r8(C r8, bool cf_set, uint8_t s) {
    if (!cf_set) {
        switch (s) {
        case 0x1: return OE_ALU |          S_C(C_T_ML)  | SEL_C | LD_C;
        case 0x2: return OE_ALU | LD_MEM | S_C(C_T_MH)  | SEL_C | LD_C;
        case 0x3: return OE_ALU | LD_MEM | S_C(r8)      | SEL_C | LD_C;
        case 0x4: return OE_MEM | LD_ML  | S_C(7)       | SEL_C | LD_C;
        case 0x5: return OE_C   | LD_MH  | S_C(A_ADD_F) | SEL_C | LD_C;
        default:  break;
        }
    } else {
        switch (s) {
        case 0x1: return OE_C   | C_LD_S;
        default:  break;
        }
    }

    switch (s) {
    case 0x6: return OE_ALU | LD_F   | S_C(A_ADD)   | SEL_C | LD_C;
    case 0x7: return OE_ALU | LD_T   | S_C(r8)      | SEL_C | LD_C;
    case 0x8: return OE_T   | LD_MEM | S_C(C_T_ML)  | SEL_C | LD_C;
    case 0x9: return OE_MEM | LD_ML  | S_C(C_T_MH)  | SEL_C | LD_C;
    case 0xa: return OE_MEM | LD_MH                 | SEL_M | LD_C;
    case 0xb: return OE_C   | C_LD_S;
    default:  return OE_C;
    }
}

static uint16_t instr_inc_r8(C r8, uint8_t s) {
    switch (s) {
    case 0x1: return OE_ALU |          S_C(C_T_ML)  | SEL_C | LD_C;
    case 0x2: return OE_ALU | LD_MEM | S_C(C_T_MH)  | SEL_C | LD_C;
    case 0x3: return OE_ALU | LD_MEM | S_C(r8)      | SEL_C | LD_C;
    case 0x4: return OE_MEM | LD_ML  | S_C(1)       | SEL_C | LD_C;
    case 0x5: return OE_C   | LD_MH  | S_C(A_ADD_F) | SEL_C | LD_C;
    case 0x6: return OE_ALU | LD_F   | S_C(A_ADD)   | SEL_C | LD_C;
    case 0x7: return OE_ALU | LD_T   | S_C(r8)      | SEL_C | LD_C;
    case 0x8: return OE_T   | LD_MEM;
    case 0x9: return OE_C            | S_C(C_T_ML)  | SEL_C | LD_C;
    case 0xa: return OE_MEM | LD_ML  | S_C(C_T_MH)  | SEL_C | LD_C;
    case 0xb: return OE_MEM | LD_MH                 | SEL_M | LD_C;
    case 0xc: return OE_C   | C_LD_S;
    default:  return OE_C;
    }
}

static uint16_t instr_incc_r8(C r8, bool cf_set, uint8_t s) {
    if (cf_set) {
        switch (s) {
        case 0x1: return OE_ALU |          S_C(C_T_ML)  | SEL_C | LD_C;
        case 0x2: return OE_ALU | LD_MEM | S_C(C_T_MH)  | SEL_C | LD_C;
        case 0x3: return OE_ALU | LD_MEM | S_C(r8)      | SEL_C | LD_C;
        case 0x4: return OE_MEM | LD_ML  | S_C(1)       | SEL_C | LD_C;
        case 0x5: return OE_C   | LD_MH  | S_C(A_ADD_F) | SEL_C | LD_C;
        default:  break;
        }
    } else {
        switch (s) {
        case 0x1: return OE_C   | C_LD_S;
        default:  break;
        }
    }

    switch (s) {
    case 0x6: return OE_ALU | LD_F   | S_C(A_ADD)   | SEL_C | LD_C;
    case 0x7: return OE_ALU | LD_T   | S_C(r8)      | SEL_C | LD_C;
    case 0x8: return OE_T   | LD_MEM | S_C(C_T_ML)  | SEL_C | LD_C;
    case 0x9: return OE_MEM | LD_ML  | S_C(C_T_MH)  | SEL_C | LD_C;
    case 0xa: return OE_MEM | LD_MH                 | SEL_M | LD_C;
    case 0xb: return OE_C   | C_LD_S;
    default:  return OE_C;
    }
}

// 11 clocks.
static uint16_t instr_shl_r8(C r8, uint8_t s) {
    switch (s) {
    case 0x1: return OE_ALU |                   S_C(C_T_ML)  | SEL_C | LD_C;
    case 0x2: return OE_ALU | LD_MEM          | S_C(C_T_MH)  | SEL_C | LD_C;
    case 0x3: return OE_ALU | LD_MEM          | S_C(r8)      | SEL_C | LD_C;
    case 0x4: return OE_MEM | (LD_ML | LD_MH) | S_C(A_ADD_F) | SEL_C | LD_C;
    case 0x5: return OE_ALU | LD_F            | S_C(A_ADD)   | SEL_C | LD_C;
    case 0x6: return OE_ALU | LD_T            | S_C(r8)      | SEL_C | LD_C;
    case 0x7: return OE_T   | LD_MEM          | S_C(C_T_ML)  | SEL_C | LD_C;
    case 0x8: return OE_MEM | LD_ML           | S_C(C_T_MH)  | SEL_C | LD_C;
    case 0x9: return OE_MEM | LD_MH                          | SEL_M | LD_C;
    case 0xa: return OE_C   | C_LD_S;
    default:  return OE_C;
    }
}

static uint16_t instr_shlc_r8(C r8, bool cf_set, uint8_t s) {
    switch (s) {
    case 0x1: return OE_C   |           S_C(cf_set ? 1 : 0)  | SEL_C | LD_C;
    case 0x2: return OE_C   | LD_T            | S_C(C_T_ML)  | SEL_C | LD_C; // T = carry flag
    case 0x3: return OE_ALU | LD_MEM          | S_C(C_T_MH)  | SEL_C | LD_C;
    case 0x4: return OE_ALU | LD_MEM          | S_C(r8)      | SEL_C | LD_C;
    case 0x5: return OE_MEM | (LD_ML | LD_MH) | S_C(A_ADD_F) | SEL_C | LD_C;
    case 0x6: return OE_ALU | LD_F            | S_C(A_ADD)   | SEL_C | LD_C;
    case 0x7: return OE_ALU | LD_ML           | S_C(A_OR)    | SEL_C | LD_C;
    case 0x8: return OE_T   | LD_MH;
    case 0x9: return OE_ALU | LD_T            | S_C(r8)      | SEL_C | LD_C;
    case 0xa: return OE_T   | LD_MEM          | S_C(C_T_ML)  | SEL_C | LD_C;
    case 0xb: return OE_MEM | LD_ML           | S_C(C_T_MH)  | SEL_C | LD_C;
    case 0xc: return OE_MEM | LD_MH                          | SEL_M | LD_C;
    case 0xd: return OE_C   | C_LD_S;
    default:  return OE_C;
    }
}

// 13 clocks.
static uint16_t instr_shr_r8(C r8, uint8_t s) {
    switch (s) {
    case 0x1: return OE_ALU |          S_C(C_T_ML)  | SEL_C | LD_C;
    case 0x2: return OE_ALU | LD_MEM | S_C(C_T_MH)  | SEL_C | LD_C;
    case 0x3: return OE_ALU | LD_MEM | S_C(r8)      | SEL_C | LD_C;
    case 0x4: return OE_MEM | LD_ML  | S_C(AU_SHR_F)| SEL_C | LD_C;
    case 0x5: return OE_C   | LD_MH  | S_C(A_UNARY) | SEL_C | LD_C;
    case 0x6: return OE_ALU | LD_F   | S_C(AU_SHR)  | SEL_C | LD_C;
    case 0x7: return OE_C   | LD_MH  | S_C(A_UNARY) | SEL_C | LD_C;
    case 0x8: return OE_ALU | LD_T   | S_C(r8)      | SEL_C | LD_C;
    case 0x9: return OE_T   | LD_MEM | S_C(C_T_ML)  | SEL_C | LD_C;
    case 0xa: return OE_MEM | LD_ML  | S_C(C_T_MH)  | SEL_C | LD_C;
    case 0xb: return OE_MEM | LD_MH                 | SEL_M | LD_C;
    case 0xc: return OE_C   | C_LD_S;
    default:  return OE_C;
    }
}

// // Assumes C_T has the AND-mask for dest_r8.
// ld a[1], b[7]
static uint16_t instr_ld_r8_wbit_r8_rbit_end(C dest_r8, C src_r8, uint8_t s) {
    switch (s) {
    case 0x1: return OE_MEM | LD_T            | S_C(C_T_ML)   | SEL_C | LD_C | INC_M; //  T = clear/set bit
    case 0x2: return OE_ALU | LD_MEM          | S_C(C_T_MH)   | SEL_C | LD_C;
    case 0x3: return OE_ALU | LD_MEM          | S_C(0)        | SEL_C | LD_C;
    case 0x4: return OE_T   | LD_MH           | S_C(src_r8)   | SEL_C | LD_C; // MH = AU_OR_BIT
    case 0x5: return OE_MEM | LD_ML           | S_C(A_UNARY)  | SEL_C | LD_C; // ML = src_r8
    case 0x6: return OE_ALU | LD_T            | S_C(C_T)      | SEL_C | LD_C; //  T = or mask
    case 0x7: return OE_MEM | LD_MH           | S_C(dest_r8)  | SEL_C | LD_C; // MH = clear bit mask
    case 0x8: return OE_MEM | LD_ML           | S_C(A_NAND)   | SEL_C | LD_C; // ML = dest_r8
    case 0x9: return OE_ALU | (LD_ML | LD_MH);
    case 0xa: return OE_ALU | LD_ML;                                          // ML = dest_r8 & and mask
    case 0xb: return OE_T   | LD_MH           | S_C(A_OR)     | SEL_C | LD_C; // MH = or mask
    case 0xc: return OE_ALU | LD_T            | S_C(dest_r8)  | SEL_C | LD_C; //  T = ML | or mask
    case 0xd: return OE_T   | LD_MEM          | S_C(C_T_ML)   | SEL_C | LD_C; // dest_r8 = T
    case 0xe: return OE_MEM | LD_ML           | S_C(C_T_MH)   | SEL_C | LD_C;
    case 0xf: return OE_MEM | LD_MH                           | SEL_M | LD_C;
    default:  return OE_C;
    }
}

static uint16_t instr_and_r8_i8(C r8, uint8_t s) {
    switch (s) {
    case 0x1: return OE_MEM | LD_T            | S_C(C_T_ML)  | SEL_C | LD_C | INC_M;
    case 0x2: return OE_ALU | LD_MEM          | S_C(C_T_MH)  | SEL_C | LD_C;
    case 0x3: return OE_ALU | LD_MEM          | S_C(r8)      | SEL_C | LD_C;
    case 0x4: return OE_MEM | LD_ML;
    case 0x5: return OE_T   | LD_MH           | S_C(A_NAND)  | SEL_C | LD_C;
    case 0x6: return OE_ALU | (LD_ML | LD_MH);                               //   ALU = ML nand MH
    case 0x7: return OE_ALU | (LD_ML | LD_T)  | S_C(0)       | SEL_C | LD_C; // ML, T = (ML nand MH) nand (ML nand MH) => ML and MH
    case 0x8: return OE_C   | LD_MH           | S_C(A_ADD_F) | SEL_C | LD_C;
    case 0x9: return OE_ALU | LD_F            | S_C(r8)      | SEL_C | LD_C;
    case 0xa: return OE_T   | LD_MEM          | S_C(C_T_ML)  | SEL_C | LD_C;
    case 0xb: return OE_MEM | LD_ML           | S_C(C_T_MH)  | SEL_C | LD_C;
    case 0xc: return OE_MEM | LD_MH                          | SEL_M | LD_C;
    case 0xd: return OE_C   | C_LD_S;
    default:  return OE_C;
    }
}

static uint16_t instr_or_r8_i8(C r8, uint8_t s) {
    switch (s) {
    case 0x1: return OE_MEM | LD_T            | S_C(C_T_ML)  | SEL_C | LD_C | INC_M;
    case 0x2: return OE_ALU | LD_MEM          | S_C(C_T_MH)  | SEL_C | LD_C;
    case 0x3: return OE_ALU | LD_MEM          | S_C(r8)      | SEL_C | LD_C;
    case 0x4: return OE_MEM | LD_ML;
    case 0x5: return OE_T   | LD_MH           | S_C(A_OR)    | SEL_C | LD_C;
    case 0x6: return OE_ALU | (LD_T | LD_ML)  | S_C(0)       | SEL_C | LD_C; // T, ML = ML or MH
    case 0x7: return OE_C   | LD_MH           | S_C(A_ADD_F) | SEL_C | LD_C; // MH = 0
    case 0x8: return OE_ALU | LD_F            | S_C(r8)      | SEL_C | LD_C; // ALU = (ML or MH) + 0 (flags)
    case 0x9: return OE_T   | LD_MEM          | S_C(C_T_ML)  | SEL_C | LD_C;
    case 0xa: return OE_MEM | LD_ML           | S_C(C_T_MH)  | SEL_C | LD_C;
    case 0xb: return OE_MEM | LD_MH                          | SEL_M | LD_C;
    case 0xc: return OE_C   | C_LD_S;
    default:  return OE_C;
    }
}

static uint16_t instr_or_r8_r8(C r8_dest, C r8_src, uint8_t s) {
    switch (s) {
    case 0x1: return OE_C                     | S_C(C_T_ML)  | SEL_C | LD_C;
    case 0x2: return OE_ALU | LD_MEM          | S_C(C_T_MH)  | SEL_C | LD_C;
    case 0x3: return OE_ALU | LD_MEM          | S_C(r8_src)  | SEL_C | LD_C;
    case 0x4: return OE_MEM | LD_ML           | S_C(r8_dest) | SEL_C | LD_C;
    case 0x5: return OE_MEM | LD_MH           | S_C(A_OR)    | SEL_C | LD_C;
    case 0x6: return OE_ALU | (LD_T | LD_ML)  | S_C(0)       | SEL_C | LD_C; // T, ML = ML or MH
    case 0x7: return OE_C   | LD_MH           | S_C(A_ADD_F) | SEL_C | LD_C; // MH = 0
    case 0x8: return OE_ALU | LD_F            | S_C(r8_dest) | SEL_C | LD_C; // ALU = (ML or MH) + 0 (flags)
    case 0x9: return OE_T   | LD_MEM          | S_C(C_T_ML)  | SEL_C | LD_C;
    case 0xa: return OE_MEM | LD_ML           | S_C(C_T_MH)  | SEL_C | LD_C;
    case 0xb: return OE_MEM | LD_MH                          | SEL_M | LD_C;
    case 0xc: return OE_C   | C_LD_S;
    default:  return OE_C;
    }
}

static uint16_t instr_or_r8_cf(C r8, bool cf_set, uint8_t s) {
    if (cf_set) {
        switch (s) {
        case 0x1: return OE_ALU |                   S_C(C_T_ML)  | SEL_C | LD_C;
        case 0x2: return OE_ALU | LD_MEM          | S_C(C_T_MH)  | SEL_C | LD_C;
        case 0x3: return OE_ALU | LD_MEM          | S_C(r8)      | SEL_C | LD_C;
        case 0x4: return OE_MEM | LD_ML           | S_C(1)       | SEL_C | LD_C;
        case 0x5: return OE_C   | LD_MH           | S_C(A_OR)    | SEL_C | LD_C;
        case 0x6: return OE_ALU | LD_T            | S_C(0)       | SEL_C | LD_C; // T = ML or MH
        case 0x7: return OE_C   | LD_MH           | S_C(A_ADD_F) | SEL_C | LD_C;
        case 0x8: return OE_ALU | LD_F            | S_C(r8)      | SEL_C | LD_C;
        case 0x9: return OE_T   | LD_MEM          | S_C(C_T_ML)  | SEL_C | LD_C;
        case 0xa: return OE_MEM | LD_ML           | S_C(C_T_MH)  | SEL_C | LD_C;
        case 0xb: return OE_MEM | LD_MH                          | SEL_M | LD_C;
        case 0xc: return OE_C   | C_LD_S;
        default:  return OE_C;
        }
    } else {
        switch (s) {
        case 0x1: return OE_ALU |                   S_C(C_T_ML)  | SEL_C | LD_C;
        case 0x2: return OE_ALU | LD_MEM          | S_C(C_T_MH)  | SEL_C | LD_C;
        case 0x3: return OE_ALU | LD_MEM          | S_C(r8)      | SEL_C | LD_C;
        case 0x4: return OE_MEM | LD_ML           | S_C(0)       | SEL_C | LD_C;
        case 0x5: return OE_C   | LD_MH           | S_C(A_OR)    | SEL_C | LD_C;
        case 0x6: return OE_ALU | LD_T            | S_C(0)       | SEL_C | LD_C; // T = ML or MH
        case 0x7: return OE_C   | LD_MH           | S_C(A_ADD_F) | SEL_C | LD_C;
        case 0x8: return OE_ALU | LD_F            | S_C(r8)      | SEL_C | LD_C;
        case 0x9: return OE_T   | LD_MEM          | S_C(C_T_ML)  | SEL_C | LD_C;
        case 0xa: return OE_MEM | LD_ML           | S_C(C_T_MH)  | SEL_C | LD_C;
        case 0xb: return OE_MEM | LD_MH                          | SEL_M | LD_C;
        case 0xc: return OE_C   | C_LD_S;
        default:  return OE_C;
        }
    }
}

static uint16_t instr_add_r8_r8(C dst_r8, C src_r8, uint8_t s) {
    switch (s) {
    case 0x1: return OE_ALU |                  S_C(C_T_ML)  | SEL_C | LD_C;
    case 0x2: return OE_ALU | LD_MEM         | S_C(C_T_MH)  | SEL_C | LD_C;
    case 0x3: return OE_ALU | LD_MEM         | S_C(dst_r8)  | SEL_C | LD_C;
    case 0x4: return OE_MEM | LD_ML          | S_C(src_r8)  | SEL_C | LD_C;
    case 0x5: return OE_MEM | LD_MH          | S_C(A_ADD_F) | SEL_C | LD_C;
    case 0x6: return OE_ALU | LD_F           | S_C(A_ADD)   | SEL_C | LD_C;
    case 0x7: return OE_ALU | LD_T           | S_C(dst_r8)  | SEL_C | LD_C;
    case 0x8: return OE_T   | LD_MEM         | S_C(C_T_ML)  | SEL_C | LD_C;
    case 0x9: return OE_MEM | LD_ML          | S_C(C_T_MH)  | SEL_C | LD_C;
    case 0xa: return OE_MEM | LD_MH                         | SEL_M | LD_C;
    case 0xb: return OE_C   | C_LD_S;
    default:  return OE_C;
    }
}

static uint16_t instr_add_r8_i8(C r8, uint8_t s) {
    switch (s) {
    case 0x1: return OE_MEM | LD_T           | S_C(C_T_ML)  | SEL_C | LD_C | INC_M;
    case 0x2: return OE_ALU | LD_MEM         | S_C(C_T_MH)  | SEL_C | LD_C;
    case 0x3: return OE_ALU | LD_MEM         | S_C(r8)      | SEL_C | LD_C;
    case 0x4: return OE_MEM | LD_ML;
    case 0x5: return OE_T   | LD_MH          | S_C(A_ADD_F) | SEL_C | LD_C;
    case 0x6: return OE_ALU | LD_F           | S_C(A_ADD)   | SEL_C | LD_C;
    case 0x7: return OE_ALU | LD_T           | S_C(r8)      | SEL_C | LD_C;
    case 0x8: return OE_T   | LD_MEM         | S_C(C_T_ML)  | SEL_C | LD_C;
    case 0x9: return OE_MEM | LD_ML          | S_C(C_T_MH)  | SEL_C | LD_C;
    case 0xa: return OE_MEM | LD_MH                         | SEL_M | LD_C;
    case 0xb: return OE_C   | C_LD_S;
    default:  return OE_C;
    }
}

static uint16_t instr_cmp_r8_i8(C r8, uint8_t s) {
    switch (s) {
    case 0x1: return OE_MEM | LD_T           | S_C(C_T_ML)  | SEL_C | LD_C | INC_M;
    case 0x2: return OE_ALU | LD_MEM         | S_C(C_T_MH)  | SEL_C | LD_C;
    case 0x3: return OE_ALU | LD_MEM         | S_C(r8)      | SEL_C | LD_C;
    case 0x4: return OE_MEM | LD_ML;
    case 0x5: return OE_T   | LD_MH          | S_C(A_ADD_F) | SEL_C | LD_C;
    case 0x6: return OE_ALU | LD_F           | S_C(C_T_ML)  | SEL_C | LD_C;
    case 0x7: return OE_MEM | LD_ML          | S_C(C_T_MH)  | SEL_C | LD_C;
    case 0x8: return OE_MEM | LD_MH                         | SEL_M | LD_C;
    case 0x9: return OE_C   | C_LD_S;
    default:  return OE_C;
    }
}

static uint16_t instr_addc_r8_i8(C r8, bool cf_set, uint8_t s) {
    switch (s) {
    case 0x1: return OE_MEM | LD_T           | S_C(C_T_ML)  | SEL_C | LD_C | INC_M;
    case 0x2: return OE_ALU | LD_MEM         | S_C(C_T_MH)  | SEL_C | LD_C;
    case 0x3: return OE_ALU | LD_MEM         | S_C(r8)      | SEL_C | LD_C;
    case 0x4: return OE_MEM | LD_ML;
    case 0x5: return OE_T   | LD_MH          | S_C(A_ADD_F) | SEL_C | LD_C | (cf_set ? INC_M : 0);
    case 0x6: return OE_ALU | LD_F           | S_C(A_ADD)   | SEL_C | LD_C;
    case 0x7: return OE_ALU | LD_T           | S_C(r8)      | SEL_C | LD_C;
    case 0x8: return OE_T   | LD_MEM         | S_C(C_T_ML)  | SEL_C | LD_C;
    case 0x9: return OE_MEM | LD_ML          | S_C(C_T_MH)  | SEL_C | LD_C;
    case 0xa: return OE_MEM | LD_MH                         | SEL_M | LD_C;
    case 0xb: return OE_C   | C_LD_S;
    default:  return OE_C;
    }
}

static uint16_t instr_testz_r8(C r8, uint8_t s) {
    switch (s) {
    case 0x1: return OE_C                     | S_C(C_T_ML)  | SEL_C | LD_C;
    case 0x2: return OE_ALU | LD_MEM          | S_C(C_T_MH)  | SEL_C | LD_C;
    case 0x3: return OE_ALU | LD_MEM          | S_C(r8)      | SEL_C | LD_C;
    case 0x4: return OE_MEM | (LD_ML | LD_MH) | S_C(A_OR)    | SEL_C | LD_C;
    case 0x5: return OE_ALU | (LD_T | LD_ML)  | S_C(0)       | SEL_C | LD_C; // T, ML = ML or MH
    case 0x6: return OE_C   | LD_MH           | S_C(A_ADD_F) | SEL_C | LD_C; // MH = 0
    case 0x7: return OE_ALU | LD_F            | S_C(C_T_ML)  | SEL_C | LD_C; // F  = 0 + (ML or MH)
    case 0x8: return OE_MEM | LD_ML           | S_C(C_T_MH)  | SEL_C | LD_C;
    case 0x9: return OE_MEM | LD_MH                          | SEL_M | LD_C;
    case 0xa: return OE_C   | C_LD_S;
    default:  return OE_C;
    }
}

static uint16_t instr_push_r8(C r8, uint8_t s) {
    switch (s) {
    case 0x1: return OE_ALU |          S_C(C_T_ML) | SEL_C | LD_C;
    case 0x2: return OE_ALU | LD_MEM | S_C(C_T_MH) | SEL_C | LD_C;
    case 0x3: return OE_ALU | LD_MEM | S_C(r8)     | SEL_C | LD_C;
    case 0x4: return OE_MEM | LD_T   | S_C(7)      | SEL_M | LD_C;  // T = r8, OE C = 0xff.
    case 0x5: return OE_C   | LD_ML | LD_MH;                        // M = 0xffff, SP location.
    case 0x6: return OE_MEM | LD_ML;                                // M = 0xff:[SP]
    case 0x7: return OE_T   | LD_MEM;                               // mem[[SP]] = T
    case 0x8: return OE_C   |          S_C(C_T_ML) | SEL_M | LD_C | INC_M; //  M++
    case 0x9: return OE_ALU | LD_T   | S_C(7)      | SEL_M | LD_C;  // T = ML, SP+1, OE C = 0xff.
    case 0xa: return OE_C   | LD_MH | LD_ML;                        // M = 0xffff, SP location.
    case 0xb: return OE_T   | LD_MEM;                               // mem[SP] = T
    case 0xc: return OE_C   |          S_C(C_T_ML)  | SEL_C | LD_C;
    case 0xd: return OE_MEM | LD_ML  | S_C(C_T_MH)  | SEL_C | LD_C;
    case 0xe: return OE_MEM | LD_MH                 | SEL_M | LD_C;
    default:  return OE_C;
    }
}

static uint16_t instr_call_i16_begin(uint8_t s) {
    switch (s) {
    case 0x1: return OE_MEM | LD_T            | S_C(C_T_MH) | SEL_C | LD_C;
    case 0x2: return OE_T   | LD_MEM          |               SEL_M | LD_C | INC_M;
    case 0x3: return OE_MEM | LD_T            | S_C(C_T_ML) | SEL_C | LD_C;
    case 0x4: return OE_T   | LD_MEM                        | SEL_M | LD_C | INC_M;
    case 0x5: return OE_C   | C_LD_S;
    default:  return OE_C;
    }
}

static uint16_t instr_call_i16_end(uint8_t s) {
    switch (s) {
    case 0x1: return OE_C   |                   S_C(C_T_ML) | SEL_C | LD_C;
    case 0x2: return OE_ALU | LD_T            | S_C(C_T)    | SEL_C | LD_C;
    case 0x3: return OE_T   | LD_MEM          | S_C(C_T_MH) | SEL_C | LD_C; // mem[C_T] = ret addr. low
    case 0x4: return OE_ALU | LD_T            | S_C(7)      | SEL_M | LD_C; // T = ret addr. high
    case 0x5: return OE_C   | (LD_MH | LD_ML);                              // M = 0xffff, SP location
    case 0x6: return OE_MEM | LD_ML;                                        // M = 0xff:[SP]
    case 0x7: return OE_T   | LD_MEM          | S_C(C_T)    | SEL_C | LD_C; // mem[[SP]] = T (ret. addr high)
    case 0x8: return OE_MEM | LD_T            |               SEL_M | LD_C | INC_M;
    case 0x9: return OE_T   | LD_MEM          | S_C(C_T_ML) | SEL_M | LD_C | INC_M;
    case 0xa: return OE_ALU | LD_T            | S_C(7)      | SEL_M | LD_C;
    case 0xb: return OE_C   | (LD_MH | LD_ML);                              // M = 0xffff, SP location
    case 0xc: return OE_T   | LD_MEM;                                       // mem[SP] = T
    case 0xd: return OE_C   |                   S_C(C_T_ML)  | SEL_C | LD_C;
    case 0xe: return OE_MEM | LD_ML           | S_C(C_T_MH)  | SEL_C | LD_C;
    case 0xf: return OE_MEM | LD_MH                          | SEL_M | LD_C;
    default:  return OE_C;
    }
}


static uint16_t instr_ret(uint8_t s) {
    switch (s) {
    case 0x1: return OE_C   |                    S_C(7) | SEL_M | LD_C;
    case 0x2: return OE_C   | (LD_ML | LD_MH);                               // M = 0xffff, SP location.
    case 0x3: return OE_MEM | LD_ML            | S_C(A_ADD)  | SEL_M | LD_C; // M = 0xff:[SP]
    case 0x4: return OE_ALU | LD_ML;                                         // ML = SP - 1
    case 0x5: return OE_MEM | LD_T             | S_C(C_T_ML) | SEL_C | LD_C; // T = mem[[SP - 1]]
    case 0x6: return OE_T   | LD_MEM           | S_C(A_ADD)  | SEL_M | LD_C; // mem[C_T_ML] = T
    case 0x7: return OE_ALU | LD_ML;                                         // ML = SP - 1
    case 0x8: return OE_MEM | LD_T             | S_C(C_T_MH) | SEL_C | LD_C; // T = mem[[SP - 1]]
    case 0x9: return OE_T   | LD_MEM           | S_C(C_T_ML) | SEL_M | LD_C; // mem[C_T_MH] = T
    case 0xa: return OE_ALU | LD_T             | S_C(7)      | SEL_M | LD_C; // T = ML, SP - 2, OE C = 0xff.
    case 0xb: return OE_C   | (LD_MH | LD_ML);                               // M = 0xffff, SP location.
    case 0xc: return OE_T   | LD_MEM           | S_C(C_T_ML) | SEL_C | LD_C; // mem[SP] = T
    case 0xd: return OE_MEM | LD_ML            | S_C(C_T_MH) | SEL_C | LD_C;
    case 0xe: return OE_MEM | LD_MH                          | SEL_M | LD_C;
    default:  return OE_C;
    }
}

static uint16_t instr_nop2(uint8_t s) {
    switch (s) {
    case 0x1: return OE_C | C_LD_S;
    default:  return OE_C;
    }
}

static uint16_t instr_pop_r8(C r8, uint8_t s) {
    switch (s) {
    case 0x1: return OE_ALU |          S_C(C_T_ML) | SEL_C | LD_C;
    case 0x2: return OE_ALU | LD_MEM | S_C(C_T_MH) | SEL_C | LD_C;
    case 0x3: return OE_ALU | LD_MEM | S_C(7)      | SEL_M | LD_C;  // OE C = 0xff.
    case 0x4: return OE_C   | (LD_ML | LD_MH);                      // M = 0xffff, SP location.
    case 0x5: return OE_MEM | LD_ML  | S_C(A_ADD)  | SEL_M | LD_C;  // M = 0xff:[SP]
    case 0x6: return OE_ALU | LD_ML;                                // ML = SP - 1
    case 0x7: return OE_MEM | LD_T   | S_C(r8)     | SEL_C | LD_C;  // T = mem[[SP - 1]]
    case 0x8: return OE_T   | LD_MEM;                               // mem[r8] = T
    case 0x9: return OE_C   |          S_C(C_T_ML) | SEL_M | LD_C;  //
    case 0xa: return OE_ALU | LD_T   | S_C(7)      | SEL_M | LD_C;  // T = ML, SP - 1, OE C = 0xff.
    case 0xb: return OE_C   | LD_MH | LD_ML;                        // M = 0xffff, SP location.
    case 0xc: return OE_T   | LD_MEM;                               // mem[SP] = T
    case 0xd: return OE_C   |          S_C(C_T_ML)  | SEL_C | LD_C;
    case 0xe: return OE_MEM | LD_ML  | S_C(C_T_MH)  | SEL_C | LD_C;
    case 0xf: return OE_MEM | LD_MH                 | SEL_M | LD_C;
    default:  return OE_C;
    }
}

static uint16_t control_signals(uint8_t s, uint8_t f, uint8_t o) {
    if (s == 0) return S0_FETCH;

    switch ((O)o) {

    case O_NOP: break;

    case O_IN_A_0: return instr_in_r8_port(C_A, 0, s);
    case O_IN_A_1: return instr_in_r8_port(C_A, 1, s);
    case O_IN_A_2: return instr_in_r8_port(C_A, 2, s);
    case O_IN_A_3: return instr_in_r8_port(C_A, 3, s);

    case O_OUT_0_A: return instr_out_port_r8(0, C_A, s);
    case O_OUT_1_A: return instr_out_port_r8(1, C_A, s);
    case O_OUT_2_A: return instr_out_port_r8(2, C_A, s);
    case O_OUT_3_A: return instr_out_port_r8(3, C_A, s);

    case O_OUT_0_I8: return instr_out_port_i8(0, s);
    case O_OUT_1_I8: return instr_out_port_i8(1, s);
    case O_OUT_2_I8: return instr_out_port_i8(2, s);
    case O_OUT_3_I8: return instr_out_port_i8(3, s);

    case O_LD_A_I8: return instr_ld_r8_i8(C_A, s);
    case O_LD_B_I8: return instr_ld_r8_i8(C_B, s);
    case O_LD_C_I8: return instr_ld_r8_i8(C_C, s);
    case O_LD_D_I8: return instr_ld_r8_i8(C_D, s);
    case O_LD_E_I8: return instr_ld_r8_i8(C_E, s);
    case O_LD_T_I8: return instr_ld_r8_i8(C_T, s);

    case O_LD_BC_I16: return instr_ld_r16_i16(C_B, C_C, s);
    case O_LD_DE_I16: return instr_ld_r16_i16(C_D, C_E, s);

    case O_LD_A_FLAGS: return instr_ld_r8_flags(C_A, f, s);
    case O_LD_FLAGS_A: return instr_ld_flags_r8(C_A, s);

    case O_LD_A_B: return instr_ld_r8_r8(C_A, C_B, s);
    case O_LD_A_C: return instr_ld_r8_r8(C_A, C_C, s);
    case O_LD_A_D: return instr_ld_r8_r8(C_A, C_D, s);
    case O_LD_A_E: return instr_ld_r8_r8(C_A, C_E, s);
    case O_LD_B_A: return instr_ld_r8_r8(C_B, C_A, s);
    case O_LD_B_E: return instr_ld_r8_r8(C_B, C_E, s);
    case O_LD_B_C: return instr_ld_r8_r8(C_B, C_C, s);
    case O_LD_C_A: return instr_ld_r8_r8(C_C, C_A, s);
    case O_LD_C_B: return instr_ld_r8_r8(C_C, C_B, s);
    case O_LD_D_A: return instr_ld_r8_r8(C_D, C_A, s);
    case O_LD_E_A: return instr_ld_r8_r8(C_E, C_A, s);
    case O_LD_E_B: return instr_ld_r8_r8(C_E, C_B, s);

    case O_LD_AT_BC_A: return instr_ld_at_r16_r8(C_B, C_C, C_A, s);
    case O_LD_AT_BC_E: return instr_ld_at_r16_r8(C_B, C_C, C_E, s);
    case O_LD_AT_DE_A: return instr_ld_at_r16_r8(C_D, C_E, C_A, s);

    case O_LD_AT_I16_A: return instr_ld_at_i16_r8(C_A, s);
    case O_LD_AT_I16_B: return instr_ld_at_i16_r8(C_B, s);
    case O_LD_AT_I16_C: return instr_ld_at_i16_r8(C_C, s);

    case O_LD_B_AT_DE_INC: return instr_ld_r8_at_r16_inc(C_B, C_D, C_E, s);
    case O_LD_C_AT_DE_INC: return instr_ld_r8_at_r16_inc(C_C, C_D, C_E, s);

    case O_LD_A_AT_I16: return instr_ld_r8_at_i16(C_A, s);
    case O_LD_B_AT_I16: return instr_ld_r8_at_i16(C_B, s);
    case O_LD_C_AT_I16: return instr_ld_r8_at_i16(C_C, s);

    case O_LD_AT_DE_INC_A:  return instr_ld_at_r16_inc_r8(C_D, C_E, C_A, s);

    case O_LD_AT_DE_INC_I8: return instr_ld_at_r16_inc_i8(C_D, C_E, s);

    case O_JMP_I16: return instr_jmp_i16(true, s);
    case O_JZ_I16:  return instr_jmp_i16(f & F_Z, s);
    case O_JNZ_I16: return instr_jmp_i16(!(f & F_Z), s);
    case O_JC_I16:  return instr_jmp_i16(f & F_C, s);
    case O_JNC_I16: return instr_jmp_i16(!(f & F_C), s);
    case O_JS_I16:  return instr_jmp_i16(f & F_S, s);
    case O_JBE_I16: return instr_jmp_i16((f & F_Z) || !(f & F_C), s);
    case O_JAE_I16: return instr_jmp_i16((f & F_Z) || (f & F_C), s);

    case O_DEC_A: return instr_dec_r8(C_A, s);
    case O_DEC_B: return instr_dec_r8(C_B, s);
    case O_DEC_C: return instr_dec_r8(C_C, s);
    case O_DEC_D: return instr_dec_r8(C_D, s);
    case O_DEC_E: return instr_dec_r8(C_E, s);
    case O_DEC_T: return instr_dec_r8(C_T, s);

    case O_DECC_D: return instr_decc_r8(C_D, f & F_C, s);

    case O_INC_A: return instr_inc_r8(C_A, s);
    case O_INC_B: return instr_inc_r8(C_B, s);
    case O_INC_C: return instr_inc_r8(C_C, s);
    case O_INC_D: return instr_inc_r8(C_D, s);

    case O_INCC_B: return instr_incc_r8(C_B, f & F_C, s);

    case O_SHL_A: return instr_shl_r8(C_A, s);
    case O_SHL_B: return instr_shl_r8(C_B, s);
    case O_SHL_E: return instr_shl_r8(C_E, s);

    case O_SHLC_B: return instr_shlc_r8(C_B, f & F_C, s);
    case O_SHLC_C: return instr_shlc_r8(C_C, f & F_C, s);
    case O_SHLC_D: return instr_shlc_r8(C_D, f & F_C, s);
    case O_SHLC_E: return instr_shlc_r8(C_E, f & F_C, s);

    case O_SHR_A: return instr_shr_r8(C_A, s);
    case O_SHR_B: return instr_shr_r8(C_B, s);

    case O_AND_A_I8: return instr_and_r8_i8(C_A, s);

    case O_OR_A_I8: return instr_or_r8_i8(C_A, s);

    case O_OR_A_B: return instr_or_r8_r8(C_A, C_B, s);

    case O_OR_A_CF: return instr_or_r8_cf(C_A, f & F_C, s);

    case O_ADD_A_I8: return instr_add_r8_i8(C_A, s);
    case O_ADD_B_I8: return instr_add_r8_i8(C_B, s);
    case O_ADD_C_I8: return instr_add_r8_i8(C_C, s);
    case O_ADD_E_I8: return instr_add_r8_i8(C_E, s);

    case O_CMP_A_I8: return instr_cmp_r8_i8(C_A, s);
    case O_CMP_B_I8: return instr_cmp_r8_i8(C_B, s);
    case O_CMP_C_I8: return instr_cmp_r8_i8(C_C, s);

    case O_ADD_A_B: return instr_add_r8_r8(C_A, C_B, s);
    case O_ADD_B_A: return instr_add_r8_r8(C_B, C_A, s);
    case O_ADD_B_C: return instr_add_r8_r8(C_B, C_C, s);
    case O_ADD_E_A: return instr_add_r8_r8(C_E, C_A, s);

    case O_ADDC_B_I8: return instr_addc_r8_i8(C_B, f & F_C, s);
    case O_ADDC_D_I8: return instr_addc_r8_i8(C_D, f & F_C, s);

    case O_TESTZ_B: return instr_testz_r8(C_B, s);

    case O_PUSH_A: return instr_push_r8(C_A, s);
    case O_PUSH_B: return instr_push_r8(C_B, s);
    case O_PUSH_C: return instr_push_r8(C_C, s);
    case O_PUSH_D: return instr_push_r8(C_D, s);
    case O_PUSH_E: return instr_push_r8(C_E, s);

    case O_POP_A: return instr_pop_r8(C_A, s);
    case O_POP_B: return instr_pop_r8(C_B, s);
    case O_POP_C: return instr_pop_r8(C_C, s);
    case O_POP_D: return instr_pop_r8(C_D, s);
    case O_POP_E: return instr_pop_r8(C_E, s);

    case O_CALL_I16_BEGIN: return instr_call_i16_begin(s);
    case O_CALL_I16_END:   return instr_call_i16_end(s);
    case O_RET:            return instr_ret(s);

    case O_NOP2: return instr_nop2(s);

    case O_LD_A_WBIT_B_RBIT_END: return instr_ld_r8_wbit_r8_rbit_end(C_A, C_B, s);
    case O_LD_B_WBIT_A_RBIT_END: return instr_ld_r8_wbit_r8_rbit_end(C_B, C_A, s);

    case O_DEBUG: return OE_C | C_LD_S;
    case O_DEBUG_I16_N: {
        switch (s) {
        case 1: return OE_C | INC_M;
        case 2: return OE_C | INC_M;
        case 3: return OE_C | INC_M;
        case 4: return OE_C | INC_M | C_LD_S;
        }
    };

    default: break;
    }

    return OE_C;
}

static void fill_control(uint8_t control[CONTROL_ROM_SIZE]) {
    for (int i = 0; i < CONTROL_ROM_SIZE; ++i) {
        uint8_t o = (i >> 0)  & 0xff;
        uint8_t s = (i >> 8)  & 0xf;
        uint8_t f = (i >> 12) & 0xf;

        uint16_t unmasked = !(f & F_I)
                          ? control_init_signals(s, f)
                          : control_signals(s, f, o);

        uint16_t signals = unmasked ^ S_ACTIVE_LOW_MASK;

        bool second_slice = (i >> 16) & 1;

        control[i] = second_slice
                   ? signals >> 8
                   : signals & 0xff;
    }
}

static const char* customasm_rule_from_opcode(O o) {
    switch (o) {

    case O_NOP: return "nop => ?";

    // Assumes O_IN_A_0..3
    case O_IN_A_0: return "in a, {p: u3} => (? + p)`8";
    case O_IN_A_1: return NULL;
    case O_IN_A_2: return NULL;
    case O_IN_A_3: return NULL;

    // Assumes O_OUT_0_A..O_OUT_3_A
    case O_OUT_0_A: return "out {p: u3}, a => (? + p)`8";
    case O_OUT_1_A: return NULL;
    case O_OUT_2_A: return NULL;
    case O_OUT_3_A: return NULL;

    case O_OUT_0_I8: return "out {p: u3}, {i:i8} => (? + p)`8 @ i";
    case O_OUT_1_I8: return NULL;
    case O_OUT_2_I8: return NULL;
    case O_OUT_3_I8: return NULL;

    case O_LD_A_I8: return "ld a, {i:i8} => ? @ i";
    case O_LD_B_I8: return "ld b, {i:i8} => ? @ i";
    case O_LD_C_I8: return "ld c, {i:i8} => ? @ i";
    case O_LD_D_I8: return "ld d, {i:i8} => ? @ i";
    case O_LD_E_I8: return "ld e, {i:i8} => ? @ i";
    case O_LD_T_I8: return "ld t, {i:i8} => ? @ i";

    case O_LD_BC_I16: return "ld bc, {i:i16} => ? @ i";
    case O_LD_DE_I16: return "ld de, {i:i16} => ? @ i";

    case O_LD_A_FLAGS: return "ld a, flags => ?";
    case O_LD_FLAGS_A: return "ld flags, a => ?";

    case O_LD_A_B: return "ld a, b => ?";
    case O_LD_A_C: return "ld a, c => ?";
    case O_LD_A_D: return "ld a, d => ?";
    case O_LD_A_E: return "ld a, e => ?";
    case O_LD_B_A: return "ld b, a => ?";
    case O_LD_B_C: return "ld b, c => ?";
    case O_LD_C_A: return "ld c, a => ?";
    case O_LD_C_B: return "ld c, b => ?";
    case O_LD_B_E: return "ld b, e => ?";
    case O_LD_D_A: return "ld d, a => ?";
    case O_LD_E_A: return "ld e, a => ?";
    case O_LD_E_B: return "ld e, b => ?";

    case O_LD_AT_BC_A: return "ld [bc], a => ?";
    case O_LD_AT_BC_E: return "ld [bc], e => ?";
    case O_LD_AT_DE_A: return "ld [de], a => ?";

    case O_LD_AT_I16_A: return "ld [{i:i16}], a => ? @ i";
    case O_LD_AT_I16_B: return "ld [{i:i16}], b => ? @ i";
    case O_LD_AT_I16_C: return "ld [{i:i16}], c => ? @ i";

    case O_LD_AT_DE_INC_A: return "ld [de++], a => ?";

    case O_LD_AT_DE_INC_I8: return "ld [de++], {i: i8} => ? @ i";

    case O_LD_B_AT_DE_INC: return "ld b, [de++] => ?";
    case O_LD_C_AT_DE_INC: return "ld c, [de++] => ?";

    case O_LD_A_AT_I16: return "ld a, [{i:i16}] => ? @ i";
    case O_LD_B_AT_I16: return "ld b, [{i:i16}] => ? @ i";
    case O_LD_C_AT_I16: return "ld c, [{i:i16}] => ? @ i";

    case O_JMP_I16:   return "jmp {i:i16} => ? @ i";
    case O_JZ_I16:    return "jz {i:i16} => ? @ i";
    case O_JNZ_I16:   return "jnz {i:i16} => ? @ i";
    case O_JC_I16:    return "jc {i:i16} => ? @ i";
    case O_JNC_I16:   return "jnc {i:i16} => ? @ i";
    case O_JS_I16:  return "js {i:i16} => ? @ i";
    case O_JBE_I16: return "jbe {i:i16} => ? @ i";
    case O_JAE_I16: return "jae {i:i16} => ? @ i";

    case O_DEC_A: return "dec a => ?";
    case O_DEC_B: return "dec b => ?";
    case O_DEC_C: return "dec c => ?";
    case O_DEC_D: return "dec d => ?";
    case O_DEC_E: return "dec e => ?";
    case O_DEC_T: return "dec t => ?";

    case O_DECC_D: return "decc d => ?";

    case O_INC_A: return "inc a => ?";
    case O_INC_B: return "inc b => ?";
    case O_INC_C: return "inc c => ?";
    case O_INC_D: return "inc d => ?";

    case O_INCC_B: return "incc b => ?";

    case O_SHL_A: return "shl a => ?";
    case O_SHL_B: return "shl b => ?";
    case O_SHL_E: return "shl e => ?";

    case O_SHLC_B: return "shlc b => ?";
    case O_SHLC_C: return "shlc c => ?";
    case O_SHLC_D: return "shlc d => ?";
    case O_SHLC_E: return "shlc e => ?";

    case O_SHR_A: return "shr a => ?";
    case O_SHR_B: return "shr b => ?";

    case O_AND_A_I8: return "and a, {i: i8} => ? @ i";

    case O_OR_A_I8: return "or a, {i: i8} => ? @ i";

    case O_OR_A_B: return "or a, b => ?";

    case O_OR_A_CF: return "or a, cf => ?";

    case O_ADD_A_I8: return "add a, {i:i8} => ? @ i";
    case O_ADD_B_I8: return "add b, {i:i8} => ? @ i";
    case O_ADD_C_I8: return "add c, {i:i8} => ? @ i";
    case O_ADD_E_I8: return "add e, {i:i8} => ? @ i";

    case O_CMP_A_I8: return "cmp a, {i:i8} => ? @ (0 - i)`8";
    case O_CMP_B_I8: return "cmp b, {i:i8} => ? @ (0 - i)`8";
    case O_CMP_C_I8: return "cmp c, {i:i8} => ? @ (0 - i)`8";

    case O_ADD_A_B: return "add a, b => ?";
    case O_ADD_B_A: return "add b, a => ?";
    case O_ADD_B_C: return "add b, c => ?";
    case O_ADD_E_A: return "add e, a => ?";

    case O_ADDC_B_I8: return "addc b, {i:i8} => ? @ i";
    case O_ADDC_D_I8: return "addc d, {i:i8} => ? @ i";

    case O_TESTZ_B: return "testz b => ?";

    case O_PUSH_A: return "push a => ?";
    case O_PUSH_B: return "push b => ?";
    case O_PUSH_C: return "push c => ?";
    case O_PUSH_D: return "push d => ?";
    case O_PUSH_E: return "push e => ?";

    case O_POP_A: return "pop a => ?";
    case O_POP_B: return "pop b => ?";
    case O_POP_C: return "pop c => ?";
    case O_POP_D: return "pop d => ?";
    case O_POP_E: return "pop e => ?";

    case O_CALL_I16_BEGIN: return NULL;
    case O_CALL_I16_END:   return NULL;
    case O_RET:            return "ret => ?";

    case O_NOP2: return "nop2 => ?";

    case O_LD_A_WBIT_B_RBIT_END: return NULL;
    case O_LD_B_WBIT_A_RBIT_END: return NULL;

    case O_DEBUG:       return "debug => ?";
    case O_DEBUG_I16_N: return "debug {i:i16}, {n:i16} => ? @ i @ n";

    default: return NULL;
    }
}

static void combined_customasm_rules(int n, char buffer[n]) {
    const char *customasm_combined_rules =
        "#ruledef combined_instructions {\n"
        "    call {i:i16} => 0x%02x @ i @ 0x%02x\n"
        "\n"
        "    ld a[{wbit: u3}], b[{rbit: u3}] => 0x%02x @ (!(1 << wbit))`8 @ 0x%02x @ (0x%02x + ((wbit << 3) | rbit))`8\n"
        "    ld b[{wbit: u3}], a[{rbit: u3}] => 0x%02x @ (!(1 << wbit))`8 @ 0x%02x @ (0x%02x + ((wbit << 3) | rbit))`8\n"
        "}\n";

    int slength = snprintf(buffer, n, customasm_combined_rules,
        O_CALL_I16_BEGIN, O_CALL_I16_END,
        O_LD_T_I8, O_LD_A_WBIT_B_RBIT_END, AU_OR_BIT,
        O_LD_T_I8, O_LD_B_WBIT_A_RBIT_END, AU_OR_BIT
    );

    if (slength >= n) {
        fprintf(stderr, "Failed to write combined customasm rules %s, reason: %s\n", customasm_combined_rules, "string buffer too small");
        exit(1);
    }
}

static void write_customasm(const char *filename_prefix, const char *link_path, const char *filename, bool link) {
    char contents[4096*2];

    const char *start = ""
"PROGRAM_START_ADDRESS = 0x1000\n" // TODO: Define 0x1000
"\n"
"#bankdef ram {\n"
"    #bits     8\n"
"    #addr     PROGRAM_START_ADDRESS\n"
"    #addr_end 0x10000\n"
"    #outp     0\n"
"}\n"
"\n"
"#ruledef instructions\n{\n";

    size_t content_len = strlcat(contents, start, sizeof(contents));

    char buffer[512];

    for (O i = 0; i < 0x100; ++i) {
        const char* rule = customasm_rule_from_opcode(i);

        if (rule != NULL) {
            const char *o_symbol = strchr(rule, '?');

            if (o_symbol == NULL) {
                fprintf(stderr, "Missing '?' in customasm rule '%s'\n", rule);
                exit(1);
            }

            int slength = snprintf(buffer, 512, "    %.*s0x%02x%s;\n", (int)(o_symbol - rule), rule, i, o_symbol + 1);

            if (slength >= 512) {
                fprintf(stderr, "Failed to write rule %s, reason: %s\n", rule, "string buffer too small");
                exit(1);
            }

            content_len = strlcat(contents, buffer, sizeof(contents));
        }
    }

    const char *end = "}\n\n";
    content_len = strlcat(contents, end, sizeof(contents));

    combined_customasm_rules(512, buffer);
    content_len = strlcat(contents, buffer, sizeof(contents));

    if (content_len >= sizeof(contents)) {
        fprintf(stderr, "customasm file too big");
        exit(1);
    }

    snprintf(buffer, 256, "%s.inc", filename_prefix);

    FILE *file = fopen(buffer, "w");

    if (file == NULL) {
        fprintf(stderr, "Failed to open %s, reason: %s\n", buffer, strerror(errno));
        exit(1);
    }

    size_t write_result = fwrite(contents, content_len, 1, file);

    if (write_result != 1) {
        fprintf(stderr, "Failed to write to file %s, reason: %s\n", buffer, strerror(errno));
        exit(1);
    }

    if (fclose(file) != 0) {
        fprintf(stderr, "Failed to close file %s, reason: %s\n", buffer, strerror(errno));
        exit(1);
    }

    if (link) {
        unlink(filename);

        snprintf(buffer, 256, "%s%s.inc", link_path, filename_prefix);

        if (symlink(buffer, filename) != 0) {
            fprintf(stderr, "Failed to symlink %s to %s, reason: %s\n", filename, buffer, strerror(errno));
            exit(1);
        }
    }
}

static void compile_boot_program(const char *program, const char *output_filename) {
    char buffer[256];

    snprintf(buffer, 256,
        "pushd .. && customasm -q -dPROGRAM_START_ADDRESS=0x0000 rom/%s -f binary -o build/%s -- -p",
        program,
        output_filename);

    int status = system(buffer);

    if (status != 0) {
        fprintf(stderr, "Failed to compile boot program, status code: %d\n", status);
        exit(1);
    }
}

static bool read_rom(size_t size, uint8_t rom[size], const char *filename) {
    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        return false;
    }

    size_t read_result = fread(rom, size, 1, file);

    if (read_result != 1) {
        fprintf(stderr, "Failed to read rom from file %s, reason: must be exactly %zu bytes\n", filename, size);
        exit(1);
    }

    if (fclose(file) != 0) {
        fprintf(stderr, "Failed to close file %s, reason: %s\n", filename, strerror(errno));
        exit(1);
    }

    return true;
}

static void write_rom(size_t size, uint8_t rom[size], const char *filename) {
    FILE *file = fopen(filename, "w");

    if (file == NULL) {
        fprintf(stderr, "Failed to open %s, reason: %s\n", filename, strerror(errno));
        exit(1);
    }

    size_t write_result = fwrite(rom, size, 1, file);

    if (write_result != 1) {
        fprintf(stderr, "Failed to write rom to file %s, reason: %s\n", filename, strerror(errno));
        exit(1);
    }

    if (fclose(file) != 0) {
        fprintf(stderr, "Failed to close file %s, reason: %s\n", filename, strerror(errno));
        exit(1);
    }
}

static bool is_alu_identical(uint8_t a[ALU_ROM_SIZE], uint8_t b[ALU_ROM_SIZE]) {
    for (int i = 0; i < ALU_ROM_SIZE; ++i)
        if (a[i] != b[i]) return false;

    return true;
}

static bool is_control0_identical(uint8_t a[CONTROL_ROM_SIZE], uint8_t b[CONTROL_ROM_SIZE]) {
    for (int i = 0; i < (CONTROL_ROM_SIZE >> 1); ++i)
        if (a[i] != b[i]) return false;

    return true;
}

static bool is_control1_identical(uint8_t a[CONTROL_ROM_SIZE], uint8_t b[CONTROL_ROM_SIZE]) {
    for (int i = 0; i < (CONTROL_ROM_SIZE >> 1); ++i)
        if (a[i | (1 << 16)] != b[i | (1 << 16)]) return false;

    return true;
}

#include "control_roms_test_instructions.h"

int main(void) {
    uint8_t alu[ALU_ROM_SIZE];

    fill_alu(alu);
    test_alu(alu);

    uint8_t boot_rom[0x1000] = {0};

    write_customasm("custom-cpu", "./build/", "../custom-cpu.inc", false);
    compile_boot_program("boot.asm", "rom/boot.bin");

    if (!read_rom(BOOT_ROM_SIZE, boot_rom, "rom/boot.bin")) {
        fprintf(stderr, "Failed to read boot rom, reason: %s\n", strerror(errno));
        exit(1);
    }

    fill_alu_boot_rom(alu, boot_rom);

    uint8_t control[CONTROL_ROM_SIZE];

    fill_control(control);

    test_instructions(control, alu);

    uint8_t burned_alu[ALU_ROM_SIZE];

    if (read_rom(ALU_ROM_SIZE, burned_alu, "custom-cpu_alu.bin.burned")) {
        if (!is_alu_identical(burned_alu, alu)) printf("ALU needs to re-burned.\n");
    }

    write_rom(ALU_ROM_SIZE, alu, "custom-cpu_alu.bin");

    uint8_t burned_control[CONTROL_ROM_SIZE];

    if (read_rom(CONTROL_ROM_SIZE, burned_control, "custom-cpu_control.bin.burned")) {
        if (!is_control0_identical(burned_control, control)) printf("CONTROL0 needs to re-burned.\n");
        if (!is_control1_identical(burned_control, control)) printf("CONTROL1 needs to re-burned.\n");
    }

    write_rom(CONTROL_ROM_SIZE, control, "custom-cpu_control.bin");

    return 0;
}
