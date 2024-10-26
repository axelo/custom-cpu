static inline uint16_t instruction_out_i8(Operands o, uint8_t s, uint8_t  tf) {
    switch (s) {
    case 0x0: return S0_FETCH;
    case 0x1: return OE_MEM | SEL_C_LD_GPO | LD_S | INC_M;
    }
    return OE_T;
}

static bool test_instruction_out_i8(int permutation, Operands o, char **buffer_ptr, test_State* before, test_State* after) {
    uint8_t i8 = permutation & 0xff;

    if (after == NULL) {
        before->mem[test_state_M(before) + 1] = i8;

        return true;
    }

    if (test_instruction_expect_pc_step(2, buffer_ptr, before, after)) return true;

    uint8_t actual_gpo = after->gpo;

    if (i8 != actual_gpo) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected gpo to be 0x%02x, is 0x%02x\n", i8, actual_gpo);
        return true;
    }

    return false;
}
