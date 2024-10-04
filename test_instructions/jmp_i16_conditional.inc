static bool test_instruction_jmp_i16_conditional(int flags_0_1, int permutation, char** buffer_ptr, test_State* before, test_State* after) {
    srand((uint32_t)(permutation + 17));

    uint16_t i16 = (uint16_t)(rand() & 0xff00) | ((rand() >> 8) & 0xff);

    uint8_t flags = permutation ? (flags_0_1 & 0xf) : ((uint8_t)flags_0_1 >> 4);

    if (after == NULL) {
        before->mem[test_state_M(before) + 1] = i16 >> 8;
        before->mem[test_state_M(before) + 2] = i16 & 0xff;

        before->mem[0xfff0 | C_F] = flags;

        return permutation >= 1;
    }

    uint16_t actual_pc = test_state_M(after);

    if (permutation) {
        if (actual_pc != i16) {
            *buffer_ptr += sprintf(*buffer_ptr, "expected pc to be 0x%04x, is 0x%04x\n", i16, actual_pc);
            return true;
        }
    } else {
        if (test_instruction_expect_pc_step(3, buffer_ptr, before, after)) return true;
    }

    return false;
}
