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

static const Instruction instructions[0x100] = {
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
        .customasm = "tx start",
        .signals = instruction_out_tx_start,
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
