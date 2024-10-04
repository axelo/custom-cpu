#define S0_FETCH (OE_MEM | LD_I | INC_M)

#define M (0 << 4)
#define C (1 << 4)

#define LD_CSEL(c) ((uint16_t)((c & 0x1f) << 11) | LD_C)

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
#include "instructions/ld_at_r16_inc_r8.inc"
#include "instructions/add_r8_i8.inc"
#include "instructions/addc_r8_i8.inc"
#include "instructions/subc_r8_i8.inc"
#include "instructions/shl_r8.inc"
#include "instructions/shr_r8.inc"
#include "instructions/jmp_i16.inc"
#include "instructions/jmp_r16.inc"
#include "instructions/jmp_i16_conditional.inc"
#include "instructions/jal_r16_i16.inc"
#include "instructions/push_r8.inc"
#include "instructions/push_r16.inc"
#include "instructions/pop_r8.inc"
#include "instructions/pop_r16.inc"

static inline uint16_t signals_instruction(Instruction i, uint8_t s, uint8_t tf, uint8_t m13) {
    switch (i) {
    case RESET: return instruction_reset(s, m13);

    case NOP: return instruction_nop(s);

    case LD_A_I8: return instruction_ld_r8_i8(C_A, s);
    case LD_B_I8: return instruction_ld_r8_i8(C_B, s);
    case LD_C_I8: return instruction_ld_r8_i8(C_C, s);
    case LD_D_I8: return instruction_ld_r8_i8(C_D, s);

    case LD_A_B: return instruction_ld_r8_r8(C_A, C_B, s);
    case LD_A_C: return instruction_ld_r8_r8(C_A, C_C, s);
    case LD_A_D: return instruction_ld_r8_r8(C_A, C_D, s);

    case LD_B_A: return instruction_ld_r8_r8(C_B, C_A, s);
    case LD_B_C: return instruction_ld_r8_r8(C_B, C_C, s);
    case LD_B_D: return instruction_ld_r8_r8(C_B, C_D, s);

    case LD_C_A: return instruction_ld_r8_r8(C_C, C_A, s);
    case LD_C_B: return instruction_ld_r8_r8(C_C, C_B, s);
    case LD_C_D: return instruction_ld_r8_r8(C_C, C_D, s);

    case LD_D_A: return instruction_ld_r8_r8(C_D, C_A, s);
    case LD_D_B: return instruction_ld_r8_r8(C_D, C_B, s);
    case LD_D_C: return instruction_ld_r8_r8(C_D, C_C, s);

    case LD_I_I16: return instruction_ld_r16_i16(C_IH, C_IL, s);
    case LD_J_I16: return instruction_ld_r16_i16(C_JH, C_JL, s);
    case LD_K_I16: return instruction_ld_r16_i16(C_KH, C_KL, s);

    case LD_A_AT_I16: return instruction_ld_r8_at_i16(C_A, s);
    case LD_B_AT_I16: return instruction_ld_r8_at_i16(C_B, s);
    case LD_C_AT_I16: return instruction_ld_r8_at_i16(C_C, s);
    case LD_D_AT_I16: return instruction_ld_r8_at_i16(C_D, s);

    case LD_A_AT_AT_I16: return instruction_ld_r8_at_at_i16(C_A, s);
    case LD_B_AT_AT_I16: return instruction_ld_r8_at_at_i16(C_B, s);
    case LD_C_AT_AT_I16: return instruction_ld_r8_at_at_i16(C_C, s);
    case LD_D_AT_AT_I16: return instruction_ld_r8_at_at_i16(C_D, s);

    case LD_A_AT_I: return instruction_ld_r8_at_r16(C_A, C_IH, C_IL, s);
    case LD_B_AT_I: return instruction_ld_r8_at_r16(C_B, C_IH, C_IL, s);
    case LD_C_AT_I: return instruction_ld_r8_at_r16(C_C, C_IH, C_IL, s);
    case LD_D_AT_I: return instruction_ld_r8_at_r16(C_D, C_IH, C_IL, s);

    case LD_A_AT_J: return instruction_ld_r8_at_r16(C_A, C_JH, C_JL, s);
    case LD_B_AT_J: return instruction_ld_r8_at_r16(C_B, C_JH, C_JL, s);
    case LD_C_AT_J: return instruction_ld_r8_at_r16(C_C, C_JH, C_JL, s);
    case LD_D_AT_J: return instruction_ld_r8_at_r16(C_D, C_JH, C_JL, s);

    case LD_A_AT_K: return instruction_ld_r8_at_r16(C_A, C_KH, C_KL, s);
    case LD_B_AT_K: return instruction_ld_r8_at_r16(C_B, C_KH, C_KL, s);
    case LD_C_AT_K: return instruction_ld_r8_at_r16(C_C, C_KH, C_KL, s);
    case LD_D_AT_K: return instruction_ld_r8_at_r16(C_D, C_KH, C_KL, s);

    case LD_A_AT_I_INC: return instruction_ld_r8_at_r16_inc(C_A, C_IH, C_IL, s);
    case LD_B_AT_I_INC: return instruction_ld_r8_at_r16_inc(C_B, C_IH, C_IL, s);
    case LD_C_AT_I_INC: return instruction_ld_r8_at_r16_inc(C_C, C_IH, C_IL, s);
    case LD_D_AT_I_INC: return instruction_ld_r8_at_r16_inc(C_D, C_IH, C_IL, s);

    case LD_A_AT_J_INC: return instruction_ld_r8_at_r16_inc(C_A, C_JH, C_JL, s);
    case LD_B_AT_J_INC: return instruction_ld_r8_at_r16_inc(C_B, C_JH, C_JL, s);
    case LD_C_AT_J_INC: return instruction_ld_r8_at_r16_inc(C_C, C_JH, C_JL, s);
    case LD_D_AT_J_INC: return instruction_ld_r8_at_r16_inc(C_D, C_JH, C_JL, s);

    case LD_A_AT_K_INC: return instruction_ld_r8_at_r16_inc(C_A, C_KH, C_KL, s);
    case LD_B_AT_K_INC: return instruction_ld_r8_at_r16_inc(C_B, C_KH, C_KL, s);
    case LD_C_AT_K_INC: return instruction_ld_r8_at_r16_inc(C_C, C_KH, C_KL, s);
    case LD_D_AT_K_INC: return instruction_ld_r8_at_r16_inc(C_D, C_KH, C_KL, s);

    case LD_AT_I16_A: return instruction_ld_at_i16_r8(C_A, s);
    case LD_AT_I16_B: return instruction_ld_at_i16_r8(C_B, s);
    case LD_AT_I16_C: return instruction_ld_at_i16_r8(C_C, s);
    case LD_AT_I16_D: return instruction_ld_at_i16_r8(C_D, s);

    case LD_AT_I_INC_A: return instruction_ld_at_r16_inc_r8(C_IH, C_IL, C_A, s);
    case LD_AT_J_INC_A: return instruction_ld_at_r16_inc_r8(C_JH, C_JL, C_A, s);
    case LD_AT_K_INC_A: return instruction_ld_at_r16_inc_r8(C_KH, C_KL, C_A, s);

    case LD_FLAGS_I8: return instruction_ld_r8_i8(C_F, s);

    case ADD_A_I8: return instruction_add_r8_i8(C_A, s);

    case ADDC_A_I8: return instruction_addc_r8_i8(C_A, tf, s);

    case SUBC_A_I8: return instruction_subc_r8_i8(C_A, tf, s);

    case SHL_A: return instruction_shl_r8(C_A, s);

    case SHR_A: return instruction_shr_r8(C_A, s);

    case JMP_I16: return instruction_jmp_i16(s);

    case JMP_K: return instruction_jmp_r16(C_KH, C_KL, s);

    case JZ_I16:  return instruction_jmp_i16_conditional(tf & F_Z,    s);
    case JNZ_I16: return instruction_jmp_i16_conditional(!(tf & F_Z), s);
    case JC_I16:  return instruction_jmp_i16_conditional(tf & F_C,    s);
    case JNC_I16: return instruction_jmp_i16_conditional(!(tf & F_C), s);

    case JAL_K_I16: return instruction_jal_r16_i16(C_KH, C_KL, s);

    case PUSH_A: return instruction_push_r8(C_A, s);

    case PUSH_K: return instruction_push_r16(C_KH, C_KL, s);

    case POP_A: return instruction_pop_r8(C_A, s);

    case POP_K: return instruction_pop_r16(C_KH, C_KL, s);

    default: return OE_T | LD_S;
    }
}
