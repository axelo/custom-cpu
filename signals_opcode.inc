#define S0_FETCH (OE_MEM | LD_O | INC_M)

#define M (0 << 4)
#define C (1 << 4)

#define LD_CSEL(c) ((uint16_t)((c & 0x1f) << 11) | LD_C)

#include "./opcode_reset.inc"
#include "./opcode_nop.inc"
#include "./opcode_ld_r8_i8.inc"
#include "./opcode_ld_r8_r8.inc"
#include "./opcode_ld_r16_i16.inc"
#include "./opcode_ld_r8_at_i16.inc"
#include "./opcode_ld_r8_at_at_i16.inc"
#include "./opcode_ld_r8_at_r16.inc"
#include "./opcode_ld_r8_at_r16_inc.inc"
#include "./opcode_ld_at_i16_r8.inc"

static inline uint16_t signals_opcode(O o, uint8_t s, uint8_t mll, uint8_t m7) {
    (void)mll; // TODO: use

    switch (o) {
    case RESET: return opcode_reset(s, m7);

    case NOP: return opcode_nop(s);

    case LD_A_I8: return opcode_ld_r8_i8(C_A, s);
    case LD_B_I8: return opcode_ld_r8_i8(C_B, s);
    case LD_C_I8: return opcode_ld_r8_i8(C_C, s);
    case LD_D_I8: return opcode_ld_r8_i8(C_D, s);

    case LD_A_B: return opcode_ld_r8_r8(C_A, C_B, s);
    case LD_A_C: return opcode_ld_r8_r8(C_A, C_C, s);
    case LD_A_D: return opcode_ld_r8_r8(C_A, C_D, s);

    case LD_B_A: return opcode_ld_r8_r8(C_B, C_A, s);
    case LD_B_C: return opcode_ld_r8_r8(C_B, C_C, s);
    case LD_B_D: return opcode_ld_r8_r8(C_B, C_D, s);

    case LD_C_A: return opcode_ld_r8_r8(C_C, C_A, s);
    case LD_C_B: return opcode_ld_r8_r8(C_C, C_B, s);
    case LD_C_D: return opcode_ld_r8_r8(C_C, C_D, s);

    case LD_D_A: return opcode_ld_r8_r8(C_D, C_A, s);
    case LD_D_B: return opcode_ld_r8_r8(C_D, C_B, s);
    case LD_D_C: return opcode_ld_r8_r8(C_D, C_C, s);

    case LD_I_I16: return opcode_ld_r16_i16(C_IH, C_IL, s);
    case LD_J_I16: return opcode_ld_r16_i16(C_JH, C_JL, s);
    case LD_K_I16: return opcode_ld_r16_i16(C_KH, C_KL, s);

    case LD_A_AT_I16: return opcode_ld_r8_at_i16(C_A, s);
    case LD_B_AT_I16: return opcode_ld_r8_at_i16(C_B, s);
    case LD_C_AT_I16: return opcode_ld_r8_at_i16(C_C, s);
    case LD_D_AT_I16: return opcode_ld_r8_at_i16(C_D, s);

    case LD_A_AT_AT_I16: return opcode_ld_r8_at_at_i16(C_A, s);
    case LD_B_AT_AT_I16: return opcode_ld_r8_at_at_i16(C_B, s);
    case LD_C_AT_AT_I16: return opcode_ld_r8_at_at_i16(C_C, s);
    case LD_D_AT_AT_I16: return opcode_ld_r8_at_at_i16(C_D, s);

    case LD_A_AT_I: return opcode_ld_r8_at_r16(C_A, C_IH, C_IL, s);
    case LD_B_AT_I: return opcode_ld_r8_at_r16(C_B, C_IH, C_IL, s);
    case LD_C_AT_I: return opcode_ld_r8_at_r16(C_C, C_IH, C_IL, s);
    case LD_D_AT_I: return opcode_ld_r8_at_r16(C_D, C_IH, C_IL, s);

    case LD_A_AT_J: return opcode_ld_r8_at_r16(C_A, C_JH, C_JL, s);
    case LD_B_AT_J: return opcode_ld_r8_at_r16(C_B, C_JH, C_JL, s);
    case LD_C_AT_J: return opcode_ld_r8_at_r16(C_C, C_JH, C_JL, s);
    case LD_D_AT_J: return opcode_ld_r8_at_r16(C_D, C_JH, C_JL, s);

    case LD_A_AT_K: return opcode_ld_r8_at_r16(C_A, C_KH, C_KL, s);
    case LD_B_AT_K: return opcode_ld_r8_at_r16(C_B, C_KH, C_KL, s);
    case LD_C_AT_K: return opcode_ld_r8_at_r16(C_C, C_KH, C_KL, s);
    case LD_D_AT_K: return opcode_ld_r8_at_r16(C_D, C_KH, C_KL, s);

    case LD_A_AT_I_INC: return opcode_ld_r8_at_r16_inc(C_A, C_IH, C_IL, s);
    case LD_B_AT_I_INC: return opcode_ld_r8_at_r16_inc(C_B, C_IH, C_IL, s);
    case LD_C_AT_I_INC: return opcode_ld_r8_at_r16_inc(C_C, C_IH, C_IL, s);
    case LD_D_AT_I_INC: return opcode_ld_r8_at_r16_inc(C_D, C_IH, C_IL, s);

    case LD_A_AT_J_INC: return opcode_ld_r8_at_r16_inc(C_A, C_JH, C_JL, s);
    case LD_B_AT_J_INC: return opcode_ld_r8_at_r16_inc(C_B, C_JH, C_JL, s);
    case LD_C_AT_J_INC: return opcode_ld_r8_at_r16_inc(C_C, C_JH, C_JL, s);
    case LD_D_AT_J_INC: return opcode_ld_r8_at_r16_inc(C_D, C_JH, C_JL, s);

    case LD_A_AT_K_INC: return opcode_ld_r8_at_r16_inc(C_A, C_KH, C_KL, s);
    case LD_B_AT_K_INC: return opcode_ld_r8_at_r16_inc(C_B, C_KH, C_KL, s);
    case LD_C_AT_K_INC: return opcode_ld_r8_at_r16_inc(C_C, C_KH, C_KL, s);
    case LD_D_AT_K_INC: return opcode_ld_r8_at_r16_inc(C_D, C_KH, C_KL, s);

    case LD_AT_I16_A: return opcode_ld_at_i16_r8(C_A, s);
    case LD_AT_I16_B: return opcode_ld_at_i16_r8(C_B, s);
    case LD_AT_I16_C: return opcode_ld_at_i16_r8(C_C, s);
    case LD_AT_I16_D: return opcode_ld_at_i16_r8(C_D, s);

    default: return OE_T | LD_S;
    }
}
