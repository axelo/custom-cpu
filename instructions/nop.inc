static inline uint16_t instruction_nop(Operands o, uint8_t s, uint8_t tf) {
    switch (s) {
    case 0x0: return S0_FETCH;
    }
    return OE_T;
}

static bool test_instruction_nop(int permutation, Operands o, char **buffer_ptr, test_State* before, test_State* after) {
    if (after == NULL) return true;

    if (test_instruction_expect_pc_step(1, buffer_ptr, before, after)) return true;

    return false;
}
