static bool test_instruction_expect_pc_step(int pc_diff_expected, char **buffer_ptr, const test_State* before, const test_State* after) {
    int pc_diff_actual = test_state_M(after) - test_state_M(before);

    if (pc_diff_actual != pc_diff_expected) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected pc to move %d step(s), actual moved %d step(s)\n", pc_diff_expected, pc_diff_actual);
        return true;
    }

    return false;
}

static inline uint16_t instruction_jcmpz_r8_r16(Operands o, uint8_t s, uint8_t tf) {
    switch (s) {
    case 0x0: return S0_FETCH;
    case 0x1: return OE_T                    | LD_CSEL(C_MH | C);
    case 0x2: return OE_ALU | LD_MEM         | LD_CSEL(C_ML | C);
    case 0x3: return OE_ALU | LD_MEM         | LD_CSEL(C_FF | C);
    case 0x4: return OE_MEM | LD_ML | LD_MH  | LD_CSEL(A_NAND);
    case 0x5: return OE_ALU | LD_ML          | LD_CSEL(o.src.r8 | C); // ML = 0
    case 0x6: return OE_MEM | LD_MH          | LD_CSEL(A_ADD_F);
    case 0x7: return OE_ALU | SEL_C_LD_TF;
    }

    if (TF_TO_F(tf) & F_Z) {
        // zero, do jump
        switch (s) {
        case 0x8: return OE_T                    | LD_CSEL(o.dst.rh8 | C);
        case 0x9: return OE_MEM | LD_MH          | LD_CSEL(o.dst.rl8 | C);
        case 0xa: return OE_MEM | LD_ML | LD_S   | LD_CSEL(M);
        }

    } else {
        // not zero, do not jump
        switch (s) {
        case 0x8: return OE_T                    | LD_CSEL(C_MH | C);
        case 0x9: return OE_MEM | LD_MH          | LD_CSEL(C_ML | C);
        case 0xa: return OE_MEM | LD_ML | LD_S   | LD_CSEL(M);
        }
    }

    return OE_T;
}

static bool test_instruction_jcmpz_r8_r16(int permutation, Operands o, char **buffer_ptr, test_State* before, test_State* after) {
    if (after == NULL) {
        uint16_t i16 = (uint16_t)(((~before->mh & 0xf0) << 8) | (rand() & 0xff));

        before->mem[0xfff0 | o.dst.rh8] = i16 >> 8;
        before->mem[0xfff0 | o.dst.rl8] = i16 & 0xff;

        before->mem[0xfff0 | o.src.r8] = (uint8_t)permutation;

        return permutation >= 0xff;
    }

    uint8_t actual_r8_value = before->mem[0xfff0 | o.src.r8];

    if (actual_r8_value == 0) {
        uint16_t actual_pc = test_state_M(after);
        uint16_t expected_pc = (uint16_t)(before->mem[0xfff0 | o.dst.rh8] << 8 | before->mem[0xfff0 | o.dst.rl8]);

        if (actual_pc != expected_pc) {
            *buffer_ptr += sprintf(*buffer_ptr, "expected pc to jump to 0x%04x, is 0x%04x - test r8 (%d): 0x%x\n", expected_pc, actual_pc, o.src.r8, actual_r8_value);
            return true;
        }
    } else {
        if (test_instruction_expect_pc_step(1, buffer_ptr, before, after)) {
            *buffer_ptr += sprintf(*buffer_ptr, "expected pc not to jump - test r8 (%d): 0x%x\n", o.src.r8, actual_r8_value);

            return true;
        }
    }

    return false;
}

#include "instructions/reset.inc"
#include "instructions/nop.inc"
#include "instructions/ld_r8_i8.inc"
#include "instructions/ld_r8_r8.inc"
#include "instructions/ld_r16_i16.inc"
#include "instructions/ld_r8_at_i16.inc"
#include "instructions/ld_r8_at_at_i16.inc"
#include "instructions/ld_r8_at_r16.inc"
#include "instructions/ld_r8_at_r16_inc.inc"
#include "instructions/ld_at_i16_r8.inc"
#include "instructions/ld_at_r16_r8.inc"
#include "instructions/ld_at_r16_inc_r8.inc"
#include "instructions/add_r8_i8.inc"
#include "instructions/addc_r8_i8.inc"
#include "instructions/subc_r8_i8.inc"
#include "instructions/shl_r8.inc"
#include "instructions/shr_r8.inc"
#include "instructions/and_r8_i8.inc"
#include "instructions/jmp_i16.inc"
#include "instructions/jmp_r16.inc"
#include "instructions/jmp_i16_conditional.inc"
#include "instructions/jal_r16_i16.inc"
#include "instructions/push_r8.inc"
#include "instructions/push_r16.inc"
#include "instructions/pop_r8.inc"
#include "instructions/pop_r16.inc"
#include "instructions/out_i8.inc"
#include "instructions/uart.inc"
#include "instructions/spi.inc"

// instruction id
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
    LD_T_AT_I_INC,

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

    LD_AT_I_A,

    LD_AT_I_INC_A,
    LD_AT_J_INC_A,
    LD_AT_K_INC_A,

    ADD_A_I8,

    ADDC_A_I8,

    SUBC_A_I8,

    SHL_A,

    SHR_A,

    AND_A_I8,

    JMP_I16,
    JMP_K,

    JZ_I16,
    JNZ_I16,
    JC_I16,
    JNC_I16,
    JO_I16,
    // http://www.unixwiz.net/techtips/x86-jumps.html

    JAL_K_I16,

    JCMPZ_A_K,

    PUSH_A,

    PUSH_K,

    POP_A,

    POP_K,

    OUT_I8,

    OUT_TX_START,
    OUT_TX,
    OUT_TX_STOP,

    IN_RX_START_I16,
    IN_RX,
    IN_RX_STOP_R8,

    SPI0_BEGIN,
    SPI0_NEXT,
    SPI0_END,

    SPI2_BEGIN,
} I_id;

#define N_INSTRUCTIONS 0x100

static const Instruction instructions[N_INSTRUCTIONS] = {
    [RESET] = {
        .customasm = "reset",
        .signals_with_m13 = instruction_reset,
    },

    [NOP] = {
        .customasm = "nop",
        .signals = instruction_nop,
        .test = test_instruction_nop,
    },

    [LD_A_I8] = {
        .customasm = "ld a, {imm:i8}",
        .signals = instruction_ld_r8_i8,
        .operands = { .dst = { .r8 = C_A } },
        .test = test_instruction_ld_r8_i8,
    },

    [LD_B_I8] = {
        .customasm = "ld b, {imm:i8}",
        .signals = instruction_ld_r8_i8,
        .operands = { .dst = { .r8 = C_B } },
        .test = test_instruction_ld_r8_i8,
    },

    [LD_C_I8] = {
        .customasm = "ld c, {imm:i8}",
        .signals = instruction_ld_r8_i8,
        .operands = { .dst = { .r8 = C_C } },
        .test = test_instruction_ld_r8_i8,
    },

    [LD_D_I8] = {
        .customasm = "ld d, {imm:i8}",
        .signals = instruction_ld_r8_i8,
        .operands = { .dst = { .r8 = C_D } },
        .test = test_instruction_ld_r8_i8,
    },

    [LD_A_B] = {
        .customasm = "ld a, b",
        .signals = instruction_ld_r8_r8,
        .operands = { .dst = { .r8 = C_A }, .src = { .r8 = C_B } },
        .test = test_instruction_ld_r8_r8,
    },

    [LD_A_C] = {
        .customasm = "ld a, c",
        .signals = instruction_ld_r8_r8,
        .operands = { .dst = { .r8 = C_A }, .src = { .r8 = C_C } },
        .test = test_instruction_ld_r8_r8,
    },

    [LD_A_D] = {
        .customasm = "ld a, d",
        .signals = instruction_ld_r8_r8,
        .operands = { .dst = { .r8 = C_A }, .src = { .r8 = C_D } },
        .test = test_instruction_ld_r8_r8,
    },

    [LD_B_A] = {
        .customasm = "ld b, a",
        .signals = instruction_ld_r8_r8,
        .operands = { .dst = { .r8 = C_B }, .src = { .r8 = C_A } },
        .test = test_instruction_ld_r8_r8,
    },

    [LD_B_C] = {
        .customasm = "ld b, c",
        .signals = instruction_ld_r8_r8,
        .operands = { .dst = { .r8 = C_B }, .src = { .r8 = C_C } },
        .test = test_instruction_ld_r8_r8,
    },

    [LD_B_D] = {
        .customasm = "ld b, d",
        .signals = instruction_ld_r8_r8,
        .operands = { .dst = { .r8 = C_B }, .src = { .r8 = C_D } },
        .test = test_instruction_ld_r8_r8,
    },

    [LD_C_A] = {
        .customasm = "ld c, a",
        .signals = instruction_ld_r8_r8,
        .operands = { .dst = { .r8 = C_C }, .src = { .r8 = C_A } },
        .test = test_instruction_ld_r8_r8,
    },

    [LD_C_B] = {
        .customasm = "ld c, b",
        .signals = instruction_ld_r8_r8,
        .operands = { .dst = { .r8 = C_C }, .src = { .r8 = C_B } },
        .test = test_instruction_ld_r8_r8,
    },

    [LD_C_D] = {
        .customasm = "ld c, d",
        .signals = instruction_ld_r8_r8,
        .operands = { .dst = { .r8 = C_C }, .src = { .r8 = C_D } },
        .test = test_instruction_ld_r8_r8,
    },

    [LD_D_A] = {
        .customasm = "ld d, a",
        .signals = instruction_ld_r8_r8,
        .operands = { .dst = { .r8 = C_D }, .src = { .r8 = C_A } },
        .test = test_instruction_ld_r8_r8,
    },

    [LD_D_B] = {
        .customasm = "ld d, b",
        .signals = instruction_ld_r8_r8,
        .operands = { .dst = { .r8 = C_D }, .src = { .r8 = C_B } },
        .test = test_instruction_ld_r8_r8,
    },

    [LD_D_C] = {
        .customasm = "ld d, c",
        .signals = instruction_ld_r8_r8,
        .operands = { .dst = { .r8 = C_D }, .src = { .r8 = C_C } },
        .test = test_instruction_ld_r8_r8,
    },

    [LD_I_I16] = {
        .customasm = "ld i, {imm:i16}",
        .signals = instruction_ld_r16_i16,
        .operands = { .dst = { .rh8 = C_IH, .rl8 = C_IL } },
        .test = test_instruction_ld_r16_i16,
    },

    [LD_J_I16] = {
        .customasm = "ld j, {imm:i16}",
        .signals = instruction_ld_r16_i16,
        .operands = { .dst = { .rh8 = C_JH, .rl8 = C_JL } },
        .test = test_instruction_ld_r16_i16,
    },

    [LD_K_I16] = {
        .customasm = "ld k, {imm:i16}",
        .signals = instruction_ld_r16_i16,
        .operands = { .dst = { .rh8 = C_KH, .rl8 = C_KL } },
        .test = test_instruction_ld_r16_i16,
    },

    [LD_A_AT_I16] = {
        .customasm = "ld a, [{imm:i16}]",
        .signals = instruction_ld_r8_at_i16,
        .operands = { .dst = { .r8 = C_A } },
        .test = test_instruction_ld_r8_at_i16,
    },

    [LD_B_AT_I16] = {
        .customasm = "ld b, [{imm:i16}]",
        .signals = instruction_ld_r8_at_i16,
        .operands = { .dst = { .r8 = C_B } },
        .test = test_instruction_ld_r8_at_i16,
    },

    [LD_C_AT_I16] = {
        .customasm = "ld c, [{imm:i16}]",
        .signals = instruction_ld_r8_at_i16,
        .operands = { .dst = { .r8 = C_C } },
        .test = test_instruction_ld_r8_at_i16,
    },

    [LD_D_AT_I16] = {
        .customasm = "ld d, [{imm:i16}]",
        .signals = instruction_ld_r8_at_i16,
        .operands = { .dst = { .r8 = C_D } },
        .test = test_instruction_ld_r8_at_i16,
    },

    [LD_A_AT_AT_I16] = {
        .customasm = "ld a, [[{imm:i16}]]",
        .signals = instruction_ld_r8_at_at_i16,
        .operands = { .dst = { .r8 = C_A } },
        .test = test_instruction_ld_r8_at_at_i16,
    },

    [LD_B_AT_AT_I16] = {
        .customasm = "ld b, [[{imm:i16}]]",
        .signals = instruction_ld_r8_at_at_i16,
        .operands = { .dst = { .r8 = C_B } },
        .test = test_instruction_ld_r8_at_at_i16,
    },

    [LD_C_AT_AT_I16] = {
        .customasm = "ld c, [[{imm:i16}]]",
        .signals = instruction_ld_r8_at_at_i16,
        .operands = { .dst = { .r8 = C_C } },
        .test = test_instruction_ld_r8_at_at_i16,
    },

    [LD_D_AT_AT_I16] = {
        .customasm = "ld d, [[{imm:i16}]]",
        .signals = instruction_ld_r8_at_at_i16,
        .operands = { .dst = { .r8 = C_D } },
        .test = test_instruction_ld_r8_at_at_i16,
    },

    [LD_A_AT_I] = {
        .customasm = "ld a, [i]",
        .signals = instruction_ld_r8_at_r16,
        .operands = { .dst = { .r8 = C_A }, .src = { .rh8 = C_IH, .rl8 = C_IL } },
        .test = test_instruction_ld_r8_at_r16,
    },

    [LD_B_AT_I] = {
        .customasm = "ld b, [i]",
        .signals = instruction_ld_r8_at_r16,
        .operands = { .dst = { .r8 = C_B }, .src = { .rh8 = C_IH, .rl8 = C_IL } },
        .test = test_instruction_ld_r8_at_r16,
    },

    [LD_C_AT_I] = {
        .customasm = "ld c, [i]",
        .signals = instruction_ld_r8_at_r16,
        .operands = { .dst = { .r8 = C_C }, .src = { .rh8 = C_IH, .rl8 = C_IL } },
        .test = test_instruction_ld_r8_at_r16,
    },

    [LD_D_AT_I] = {
        .customasm = "ld d, [i]",
        .signals = instruction_ld_r8_at_r16,
        .operands = { .dst = { .r8 = C_D }, .src = { .rh8 = C_IH, .rl8 = C_IL } },
        .test = test_instruction_ld_r8_at_r16,
    },

    [LD_A_AT_J] = {
        .customasm = "ld a, [j]",
        .signals = instruction_ld_r8_at_r16,
        .operands = { .dst = { .r8 = C_A }, .src = { .rh8 = C_JH, .rl8 = C_JL } },
        .test = test_instruction_ld_r8_at_r16,
    },

    [LD_B_AT_J] = {
        .customasm = "ld b, [j]",
        .signals = instruction_ld_r8_at_r16,
        .operands = { .dst = { .r8 = C_B }, .src = { .rh8 = C_JH, .rl8 = C_JL } },
        .test = test_instruction_ld_r8_at_r16,
    },

    [LD_C_AT_J] = {
        .customasm = "ld c, [j]",
        .signals = instruction_ld_r8_at_r16,
        .operands = { .dst = { .r8 = C_C }, .src = { .rh8 = C_JH, .rl8 = C_JL } },
        .test = test_instruction_ld_r8_at_r16,
    },

    [LD_D_AT_J] = {
        .customasm = "ld d, [j]",
        .signals = instruction_ld_r8_at_r16,
        .operands = { .dst = { .r8 = C_D }, .src = { .rh8 = C_JH, .rl8 = C_JL } },
        .test = test_instruction_ld_r8_at_r16,
    },

    [LD_A_AT_K] = {
        .customasm = "ld a, [k]",
        .signals = instruction_ld_r8_at_r16,
        .operands = { .dst = { .r8 = C_A }, .src = { .rh8 = C_KH, .rl8 = C_KL } },
        .test = test_instruction_ld_r8_at_r16,
    },

    [LD_B_AT_K] = {
        .customasm = "ld b, [k]",
        .signals = instruction_ld_r8_at_r16,
        .operands = { .dst = { .r8 = C_B }, .src = { .rh8 = C_KH, .rl8 = C_KL } },
        .test = test_instruction_ld_r8_at_r16,
    },

    [LD_C_AT_K] = {
        .customasm = "ld c, [k]",
        .signals = instruction_ld_r8_at_r16,
        .operands = { .dst = { .r8 = C_C }, .src = { .rh8 = C_KH, .rl8 = C_KL } },
        .test = test_instruction_ld_r8_at_r16,
    },

    [LD_D_AT_K] = {
        .customasm = "ld d, [k]",
        .signals = instruction_ld_r8_at_r16,
        .operands = { .dst = { .r8 = C_D }, .src = { .rh8 = C_KH, .rl8 = C_KL } },
        .test = test_instruction_ld_r8_at_r16,
    },

    [LD_A_AT_I_INC] = {
        .customasm = "ld a, [i++]",
        .signals = instruction_ld_r8_at_r16_inc,
        .operands = { .dst = { .r8 = C_A }, .src = { .rh8 = C_IH, .rl8 = C_IL } },
        .test = test_instruction_ld_r8_at_r16_inc,
    },

    [LD_B_AT_I_INC] = {
        .customasm = "ld b, [i++]",
        .signals = instruction_ld_r8_at_r16_inc,
        .operands = { .dst = { .r8 = C_B }, .src = { .rh8 = C_IH, .rl8 = C_IL } },
        .test = test_instruction_ld_r8_at_r16_inc,
    },

    [LD_C_AT_I_INC] = {
        .customasm = "ld c, [i++]",
        .signals = instruction_ld_r8_at_r16_inc,
        .operands = { .dst = { .r8 = C_C }, .src = { .rh8 = C_IH, .rl8 = C_IL } },
        .test = test_instruction_ld_r8_at_r16_inc,
    },

    [LD_D_AT_I_INC] = {
        .customasm = "ld d, [i++]",
        .signals = instruction_ld_r8_at_r16_inc,
        .operands = { .dst = { .r8 = C_D }, .src = { .rh8 = C_IH, .rl8 = C_IL } },
        .test = test_instruction_ld_r8_at_r16_inc,
    },

    [LD_T_AT_I_INC] = {
        .customasm = "ld t, [i++]",
        .signals = instruction_ld_r8_at_r16_inc,
        .operands = { .dst = { .r8 = C_T }, .src = { .rh8 = C_IH, .rl8 = C_IL } },
        .test = test_instruction_ld_r8_at_r16_inc,
    },

    [LD_A_AT_J_INC] = {
        .customasm = "ld a, [j++]",
        .signals = instruction_ld_r8_at_r16_inc,
        .operands = { .dst = { .r8 = C_A }, .src = { .rh8 = C_JH, .rl8 = C_JL } },
        .test = test_instruction_ld_r8_at_r16_inc,
    },

    [LD_B_AT_J_INC] = {
        .customasm = "ld b, [j++]",
        .signals = instruction_ld_r8_at_r16_inc,
        .operands = { .dst = { .r8 = C_B }, .src = { .rh8 = C_JH, .rl8 = C_JL } },
        .test = test_instruction_ld_r8_at_r16_inc,
    },

    [LD_C_AT_J_INC] = {
        .customasm = "ld c, [j++]",
        .signals = instruction_ld_r8_at_r16_inc,
        .operands = { .dst = { .r8 = C_C }, .src = { .rh8 = C_JH, .rl8 = C_JL } },
        .test = test_instruction_ld_r8_at_r16_inc,
    },

    [LD_D_AT_J_INC] = {
        .customasm = "ld d, [j++]",
        .signals = instruction_ld_r8_at_r16_inc,
        .operands = { .dst = { .r8 = C_D }, .src = { .rh8 = C_JH, .rl8 = C_JL } },
        .test = test_instruction_ld_r8_at_r16_inc,
    },

    [LD_A_AT_K_INC] = {
        .customasm = "ld a, [k++]",
        .signals = instruction_ld_r8_at_r16_inc,
        .operands = { .dst = { .r8 = C_A }, .src = { .rh8 = C_KH, .rl8 = C_KL } },
        .test = test_instruction_ld_r8_at_r16_inc,
    },

    [LD_B_AT_K_INC] = {
        .customasm = "ld b, [k++]",
        .signals = instruction_ld_r8_at_r16_inc,
        .operands = { .dst = { .r8 = C_B }, .src = { .rh8 = C_KH, .rl8 = C_KL } },
        .test = test_instruction_ld_r8_at_r16_inc,
    },

    [LD_C_AT_K_INC] = {
        .customasm = "ld c, [k++]",
        .signals = instruction_ld_r8_at_r16_inc,
        .operands = { .dst = { .r8 = C_C }, .src = { .rh8 = C_KH, .rl8 = C_KL } },
        .test = test_instruction_ld_r8_at_r16_inc,
    },

    [LD_D_AT_K_INC] = {
        .customasm = "ld d, [k++]",
        .signals = instruction_ld_r8_at_r16_inc,
        .operands = { .dst = { .r8 = C_D }, .src = { .rh8 = C_KH, .rl8 = C_KL } },
        .test = test_instruction_ld_r8_at_r16_inc,
    },

    [LD_AT_I16_A] = {
        .customasm = "ld [{imm:i16}], a",
        .signals = instruction_ld_at_i16_r8,
        .operands = { .src = { .r8 = C_A } },
        .test = test_instruction_ld_at_i16_r8,
    },

    [LD_AT_I16_B] = {
        .customasm = "ld [{imm:i16}], b",
        .signals = instruction_ld_at_i16_r8,
        .operands = { .src = { .r8 = C_B } },
        .test = test_instruction_ld_at_i16_r8,
    },

    [LD_AT_I16_C] = {
        .customasm = "ld [{imm:i16}], c",
        .signals = instruction_ld_at_i16_r8,
        .operands = { .src = { .r8 = C_C } },
        .test = test_instruction_ld_at_i16_r8,
    },

    [LD_AT_I16_D] = {
        .customasm = "ld [{imm:i16}], d",
        .signals = instruction_ld_at_i16_r8,
        .operands = { .src = { .r8 = C_D } },
        .test = test_instruction_ld_at_i16_r8,
    },

    [LD_AT_I_A] = {
        .customasm = "ld [i], a",
        .signals = instruction_ld_at_r16_r8,
        .operands = { .dst = { .rh8 = C_IH, .rl8 = C_IL }, .src = { .r8 = C_A } },
        .test = test_instruction_ld_at_r16_r8,
    },

    [LD_AT_I_INC_A] = {
        .customasm = "ld [i++], a",
        .signals = instruction_ld_at_r16_inc_r8,
        .operands = { .dst = { .rh8 = C_IH, .rl8 = C_IL }, .src = { .r8 = C_A } },
        .test = test_instruction_ld_at_r16_inc_r8,
    },

    [LD_AT_J_INC_A] = {
        .customasm = "ld [j++], a",
        .signals = instruction_ld_at_r16_inc_r8,
        .operands = { .dst = { .rh8 = C_JH, .rl8 = C_JL }, .src = { .r8 = C_A } },
        .test = test_instruction_ld_at_r16_inc_r8,
    },

    [LD_AT_K_INC_A] = {
        .customasm = "ld [k++], a",
        .signals = instruction_ld_at_r16_inc_r8,
        .operands = { .dst = { .rh8 = C_KH, .rl8 = C_KL }, .src = { .r8 = C_A } },
        .test = test_instruction_ld_at_r16_inc_r8,
    },

    [ADD_A_I8] = {
        .customasm = "add a, {imm:i8}",
        .signals = instruction_add_r8_i8,
        .operands = { .dst = { .r8 = C_A } },
        .test = test_instruction_add_r8_i8,
    },

    [ADDC_A_I8] = {
        .customasm = "addc a, {imm:i8}",
        .signals = instruction_addc_r8_i8,
        .operands = { .dst = { .r8 = C_A } },
        .test = test_instruction_addc_r8_i8,
    },

    [SUBC_A_I8] = {
        .customasm = "subc a, {imm:i8}",
        .signals = instruction_subc_r8_i8,
        .operands = { .dst = { .r8 = C_A } },
        .test = test_instruction_subc_r8_i8,
    },

    [SHL_A] = {
        .customasm = "shl a",
        .signals = instruction_shl_r8,
        .operands = { .dst = { .r8 = C_A } },
        .test = test_instruction_shl_r8,
    },

    [SHR_A] = {
        .customasm = "shr a",
        .signals = instruction_shr_r8,
        .operands = { .dst = { .r8 = C_A } },
        .test = test_instruction_shr_r8,
    },

    [AND_A_I8] = {
        .customasm = "and a, {imm:i8}",
        .signals = instruction_and_r8_i8,
        .operands = { .dst = { .r8 = C_A } },
        .test = test_instruction_and_r8_i8,
    },

    [JMP_I16] = {
        .customasm = "jmp {imm:i16}",
        .signals = instruction_jmp_i16,
        .test = test_instruction_jmp_i16,
    },

    [JMP_K] = {
        .customasm = "jmp k",
        .signals = instruction_jmp_r16,
        .operands = { .src = { .rh8 = C_KH, .rl8 = C_KL } },
        .test = test_instruction_jmp_r16,
    },

    [JC_I16] = {
        .customasm = "jc {imm:i16}",
        .signals = instruction_jmp_i16_conditional,
        .operands = { .src = { .flag_mask_set = F_C, .flag_mask_unset = FLAG_MASK_ANY } },
        .test = test_instruction_jmp_i16_conditional,
    },

    [JNC_I16] = {
        .customasm = "jnc {imm:i16}",
        .signals = instruction_jmp_i16_conditional,
        .operands = { .src = { .flag_mask_set = FLAG_MASK_ANY, .flag_mask_unset = F_C } },
        .test = test_instruction_jmp_i16_conditional,
    },

    [JZ_I16] = {
        .customasm = "jz {imm:i16}",
        .signals = instruction_jmp_i16_conditional,
        .operands = { .src = { .flag_mask_set = F_Z, .flag_mask_unset = FLAG_MASK_ANY } },
        .test = test_instruction_jmp_i16_conditional,
    },

    [JNZ_I16] = {
        .customasm = "jnz {imm:i16}",
        .signals = instruction_jmp_i16_conditional,
        .operands = { .src = { .flag_mask_set = FLAG_MASK_ANY, .flag_mask_unset = F_Z } },
        .test = test_instruction_jmp_i16_conditional,
    },

    [JO_I16] = {
        .customasm = "jo {imm:i16}",
        .signals = instruction_jmp_i16_conditional,
        .operands = { .src = { .flag_mask_set = F_O, .flag_mask_unset = FLAG_MASK_ANY } },
        .test = test_instruction_jmp_i16_conditional,
    },

    [JAL_K_I16] = {
        .customasm = "jal k, {imm:i16}",
        .signals = instruction_jal_r16_i16,
        .operands = { .dst = { .rh8 = C_KH, .rl8 = C_KL } },
        .test = test_instruction_jal_r16_i16,
    },

    [JCMPZ_A_K] = {
        .customasm = "jcmpz k, a",
        .signals = instruction_jcmpz_r8_r16,
        .operands = { .dst = { .rh8 = C_KH, .rl8 = C_KL }, .src = { .r8 = C_A } },
        .test = test_instruction_jcmpz_r8_r16
    },

    [PUSH_A] = {
        .customasm = "push a",
        .signals = instruction_push_r8,
        .operands = { .src = { .r8 = C_A } },
        .test = test_instruction_push_r8,
    },

    [PUSH_K] = {
        .customasm = "push k",
        .signals = instruction_push_r16,
        .operands = { .src = { .rh8 = C_KH, .rl8 = C_KL } },
        .test = test_instruction_push_r16,
    },

    [POP_A] = {
        .customasm = "pop a",
        .signals = instruction_pop_r8,
        .operands = { .dst = { .r8 = C_A } },
        .test = test_instruction_pop_r8,
    },

    [POP_K] = {
        .customasm = "pop k",
        .signals = instruction_pop_r16,
        .operands = { .dst = { .rh8 = C_KH, .rl8 = C_KL } },
        .test = test_instruction_pop_r16,
    },

    [OUT_I8] = {
        .customasm = "out {imm:i8}",
        .signals = instruction_out_i8,
        .test = test_instruction_out_i8,
    },

    [OUT_TX_START] = {
        .customasm = "ld tx, a",
        .signals = instruction_out_tx_start,
        .operands = { .src = { .r8 = C_A } },
        .test = test_instruction_out_tx_start,
    },

    [OUT_TX] = {
        .customasm = "tx",
        .signals = instruction_out_tx,
        .test = test_instruction_out_tx,
    },

    [OUT_TX_STOP] = {
        .customasm = "tx stop",
        .signals = instruction_out_tx_stop,
        .test = test_instruction_out_tx_stop,
    },

    [IN_RX_START_I16] = {
        .customasm = "rx start, {imm:i16}",
        .signals = instruction_in_rx_start_i16,
        .test = test_instruction_in_rx_start_i16,
    },

    [IN_RX] = {
        .customasm = "rx",
        .signals = instruction_in_rx,
        .test = test_instruction_in_rx,
    },

    [IN_RX_STOP_R8] = {
        .customasm = "ld a, rx",
        .signals = instruction_in_rx_stop_r8,
        .operands = { .dst = { .r8 = C_A } },
        .test = test_instruction_in_rx_stop_r8
    },

    [SPI0_BEGIN] = {
        .customasm = "spi0 begin, {imm:i8}",
        .signals = instruction_spi0_begin,
    },

    [SPI0_NEXT] = {
        .customasm = "spi0 next",
        .signals = instruction_spi0_next_slower,
        .test = test_instruction_spi0_next,
    },

    [SPI0_END] = {
        .customasm = "spi0 end",
        .signals = instruction_spi0_end,
    },

    [SPI2_BEGIN] = {
        .customasm = "spi2 begin, {imm:i8}",
        .signals = instruction_spi2_begin,
        .test = test_instruction_spi2_begin,
    },
};

static void instructions_create_roms(uint8_t rom_instruction1[ROM_SIZE_INSTRUCTION], uint8_t rom_instruction2[ROM_SIZE_INSTRUCTION], const Instruction is[N_INSTRUCTIONS]) {
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

static int instructions_test_step(const uint8_t rom_alu[ROM_SIZE_ALU], const uint8_t rom_instruction1[ROM_SIZE_INSTRUCTION], const uint8_t rom_instruction2[ROM_SIZE_INSTRUCTION], char** buffer_ptr, test_State* state) {
    uint32_t index = (uint32_t)(
        (((state->mh >> 5) & 1) << 16) |
        ((state->tf & 0xf)      << 12) |
         (state->s              << 8)  |
          state->i);

    uint16_t signals = (uint16_t)((rom_instruction2[index] << 8) | rom_instruction1[index]) ^ SIGNALS_ACTIVE_LOW_MASK;

    char* buffer = *buffer_ptr;

    buffer += sprintf(buffer, "\n    0x%02x: index: 0x%05x signals: 0x%04x\n", state->s, index, signals);

    if (state->tf != 0 && state->i != RESET && state->s == 0 && signals != S0_FETCH) {
        buffer += sprintf(buffer, "        expected signals at s 0 to be S0_FETCH, TF=0x%02x\n", state->tf);
        return 2;
    }

    bool use_c_address = test_state_M_or_C(state);

    uint8_t bus = 0;
    uint16_t mem_address = (uint16_t)(use_c_address ? (0xfff0 | (state->c & 0xf)) : test_state_M(state));

    if (signals & OE_MEM) {
        bus = state->mem[mem_address];

        buffer += sprintf(buffer, "        OE_MEM: bus = 0x%02x from mem[0x%04x] (%c)\n", bus, mem_address, test_state_M_or_C(state) ? 'C' : 'M');
    } else if (signals & OE_T) {
        bus = state->t;

        buffer += sprintf(buffer, "        OE_T: bus = 0x%02x\n", bus);
    } else if (signals & OE_ALU) {
        uint32_t alu_index = (uint32_t)(
            ((state->c & 0x7) << 16) |
             (state->mh       << 8)  |
              state->ml
            );

        bus = rom_alu[alu_index];

        buffer += sprintf(buffer, "        OE_ALU: bus = 0x%02x from alu[0x%05x]\n", bus, alu_index);
    } else if (!(signals & LD_C) && (signals & SEL_C_OE_GPI)) {
        bus = state->gpi;

        buffer += sprintf(buffer, "        OE_GPI: bus = 0x%02x\n", bus);
    } else {
        buffer +=  sprintf(buffer, "        nothing asserted on the bus\n");
        return 2;
    };

    if (signals & LD_I) {
        buffer +=  sprintf(buffer, "        LD_I\n");

        state->i = bus;
    }

    if (signals & LD_T) {
        buffer += sprintf(buffer, "        LD_T\n");

        state->t = bus;

        state->gpi = (uint8_t)(
            (((state->tf >> 3) & 1) << 7) | // this is probably unnecessary
            ((state->in_miso & 1)   << 1) |
            ((state->in_rx & 1)     << 0));
    }

    if (signals & LD_MEM) {
        buffer += sprintf(buffer, "        LD_MEM to mem[0x%04x] (%c)\n", mem_address, test_state_M_or_C(state) ? 'C' : 'M');

        state->mem[mem_address] = bus;
    }

    if (signals & LD_C) {
        buffer += sprintf(buffer, "        LD_C: sel C=0x%x sel M/C=%d sel SCLK=%d\n", (signals >> 11) & 0xf, signals >> 11 >> 4, (signals >> 11 >> 3) & 1);

        state->c = signals >> 11;
    } else {
        if (signals & SEL_C_LD_TF) {
            bool sel_c = signals & SEL_C;
            uint8_t tf2 = sel_c ? (((~bus) >> 2) & 1) : 1;
            uint8_t tf3 = (bus >> 7);

            state->tf = (uint8_t)(
                (tf3 << 3) |
                (tf2 << 2) |
                (bus & 0x3));

            buffer += sprintf(buffer, "        LD_TF: TF=0x%02x sel MOSI=%d sel C=%d\n", state->tf, bus >> 7, sel_c ? 1 : 0);
        }

        if (signals & SEL_C_LD_GPO) {
            buffer += sprintf(buffer, "        LD_GPO\n");
            state->gpo = bus;
        }
    }

    if (signals & LD_ML) {
        buffer += sprintf(buffer, "        LD_ML\n");

        state->ml = bus;
    }

    if (signals & LD_MH) {
        buffer += sprintf(buffer, "        LD_MH\n");

        state->mh = bus;
    }

    if (signals & INC_M) {
        buffer += sprintf(buffer, "        INC_M: mh=0x%02x ml=0x%02x\n", state->mh, state->ml);

        if (!(signals & LD_ML)) {
            state->ml = (uint8_t)(state->ml + 1);

            if (state->ml == 0) {
                if (!(signals & LD_MH)) {
                    state->mh = (uint8_t)(state->mh + 1);
                }
            }
        }
     }

     if (signals & LD_S) {
        buffer += sprintf(buffer, "        LD_S\n");

        state->s = 0;
     } else {
         ++state->s;
         if (state->s == 0x10) state->s = 0;
     }

     *buffer_ptr = buffer;

     return state->s == 0 ? 1 : 0;
}

static int instructions_test_roms(const uint8_t rom_alu[ROM_SIZE_ALU], const uint8_t rom_instruction1[ROM_SIZE_INSTRUCTION], const uint8_t rom_instruction2[ROM_SIZE_INSTRUCTION], const Instruction is[0x100]) {
    char buffer[0x2000];
    char* buffer_ptr = buffer;

    test_State state = {0};

    // fill mem with bogus data
    for (int i = 0; i < 0x10000; ++i) {
        state.mem[i] = i & 1 ? 0xab : 0xba;
    }

    // fill regs with bogus data
    state.i = 0xab;
    state.ml = 0xfa;
    state.mh = 0x43;
    state.t = 0x85;
    state.c = 0xf;
    state.gpo = 0xba;

    state.s  = 0; // reset hooked up
    state.tf = 0; // reset hooked up

    int steps = 0;

    for (;; ++steps) {
        if ((steps & 0x1f) == 0x1f) {
            // fprintf(stderr, "steps: %d\n%s\n", steps, buffer);
            buffer[0] = '\0';
            buffer_ptr = buffer;
        }

        int error = instructions_test_step(rom_alu, rom_instruction1, rom_instruction2, &buffer_ptr, &state);

        if (error > 1) {
            fprintf(stderr, "reset failed after %d steps: %s\n", steps, buffer);
            return 1;
        }

        if (state.i == NOP && error == 1) break;

        if (state.tf != 0 && state.mh >= 0x20 && state.ml >= 0x02) {
            fprintf(stderr, "reset failed, M too big 0x%02x%02x\n", state.mh, state.ml);
            return 1;
        }

        if (steps >= 300000) {
            fprintf(stderr, "reset aborted after %d steps\n", steps);
            return 1;
        }
    }

    printf("reset complete after %d steps\n", steps);

    if (state.mh != 0 || state.ml != 0) {
        fprintf(stderr, "reset failed, expected mh (0x%02x) and ml (0x%02x) to be zero\n", state.mh, state.ml);
        return 1;
    }

    if (state.tf != 0x4) {
        fprintf(stderr, "reset failed, expected tf (0x%02x) to be 0x4\n", state.tf);
        return 1;
    }

    if (state.gpo != 0xff) {
        fprintf(stderr, "reset failed, expected gpo (0x%02x) to be 0xff\n", state.gpo);
        return 1;
    }

    if (state.mem[0xfff0 | C_FF] != 0xff) {
        fprintf(stderr, "reset failed, expected mem[C_FF] (0x%02x) to be 0xff\n", state.mem[0xfff0 | C_FF]);
        return 1;
    }

    if (state.mem[0xfff0 | C_SP] != 0x00) {
        fprintf(stderr, "reset failed, expected mem[C_SP] (0x%02x) to be 0x00\n", state.mem[0xfff0 | C_SP]);
        return 1;
    }

    if (state.mem[0xfff0 | C_F] != 0xff) {
        fprintf(stderr, "reset failed, expected mem[C_F] (0x%02x) to be 0xff\n", state.mem[0xfff0 | C_F]);
        return 1;
    }

     if (test_state_M_or_C(&state)) {
        fprintf(stderr, "reset never restored mem address to M\n");
        return 1;
    }

    for (int i = 0; i < ROM_SIZE_BOOT; ++i) {
        uint8_t actual = state.mem[i];
        uint8_t expected = rom_alu[(A_BOOT << 16) | i];

        if (actual != expected) {
            fprintf(stderr, "reset failed to copy boot rom byte 0x%02x to mem[0x%04x] (0x%02x)\n", expected, i, actual);
            return 1;
        }
    }

    int n_failed_tests = 0;

    for (int i = 0 ; i < 0x100; ++i) {
        if (is[i].test == NULL) {
            if (is[i].customasm != NULL)
                printf("%s - test not yet implemented\n", is[i].customasm);

            continue;
        }

        for (int permutation = 0;; ++permutation) {
            buffer[0] = '\0';
            buffer_ptr = buffer;

            if (state.mh == 0xff) state.mh = 0; // prevent overwriting mem registers

            state.i = (uint8_t)i; // prevent getting stuck in reset instruction

            bool last_permutation =
                is[i].test(permutation, is[i].operands, &buffer_ptr, &state, NULL);

            buffer_ptr += sprintf(buffer_ptr, "\n");

            state.mem[test_state_M(&state)] = (uint8_t)i; // the value loaded into I on first step

            state.t = 0x0d; // "random" value

            test_State orignal_state = state;

            bool test_failed = false;

            for (steps = 0; steps < 0x100; ++steps) {
                int error = instructions_test_step(rom_alu, rom_instruction1, rom_instruction2, &buffer_ptr, &state);

                if (error > 1) {
                    test_failed = true;
                    break;
                }

                if (error > 0) break;
            }

            if (!test_failed) {
                buffer_ptr += sprintf(buffer_ptr, "\nsteps taken: %d\n\n", steps + 1);

                if (steps > 0xf) {
                    buffer_ptr += sprintf(buffer_ptr, "instruction never finished\n");
                    test_failed = true;
                } else if (test_state_M_or_C(&state)) {
                    buffer_ptr += sprintf(buffer_ptr, "instruction never restored mem address to M\n");
                    test_failed = true;
                } else if (state.tf == 0) {
                    buffer_ptr += sprintf(buffer_ptr, "TF is 0, should only be true at cold start\n");
                    test_failed = true;
                } else {
                    test_failed =
                        is[i].test(permutation, is[i].operands, &buffer_ptr, &orignal_state, &state);
                }
            }

            if (test_failed) {
                fprintf(stderr, "%s - permutation %d failed: %s\n", is[i].customasm, permutation, buffer);
                ++n_failed_tests;
                break;
            } else if (last_permutation) {
                printf("%s - passed (%d times)\n", is[i].customasm, permutation + 1);
                break;
            }
        }
    }

    return n_failed_tests;
}
