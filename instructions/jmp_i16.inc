static inline uint16_t instruction_jmp_i16(Operands o, uint8_t s, uint8_t tf) {
    switch (s) {
    case 0: return S0_FETCH;
    case 1: return OE_MEM | LD_T  | INC_M;
    case 2: return OE_MEM | LD_ML;
    case 3: return OE_T   | LD_MH | LD_S;
    }
    return OE_T;
}

static bool test_instruction_jmp_i16(int permutation, Operands o, char** buffer_ptr, test_State* before, test_State* after) {
    srand((uint32_t)(permutation + 17));

    uint16_t i16 = (uint16_t)(rand() & 0xff00) | ((rand() >> 8) & 0xff);

    if (after == NULL) {
        before->mem[test_state_M(before) + 1] = i16 >> 8;
        before->mem[test_state_M(before) + 2] = i16 & 0xff;
        return permutation >= 0xff;
    }

    uint16_t actual_pc = test_state_M(after);

    if (actual_pc != i16) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected pc to be 0x%04x, is 0x%04x\n", i16, actual_pc);
        return true;
    }

    return false;
}
