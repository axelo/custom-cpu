static bool test_instruction_jal_r16_i16(int r16_h, int permutation, char** buffer_ptr, test_State* before, test_State* after) {
    srand((uint32_t)(permutation + 33));

    uint16_t i16 = (uint16_t)(rand() & 0xff00) | ((rand() >> 8) & 0xff);

    if (after == NULL) {
        before->mh = (uint8_t)permutation;
        before->ml = 0xcd;
        before->mem[test_state_M(before) + 1] = i16 >> 8;
        before->mem[test_state_M(before) + 2] = i16 & 0xff;
        return permutation >= 0xff;
    }

    uint16_t actual_pc = test_state_M(after);

    if (actual_pc != i16) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected pc to be 0x%04x, is 0x%04x\n", i16, actual_pc);
        return true;
    }

    uint16_t actual_r16 = (uint16_t)(after->mem[0xfff0 | r16_h] << 8) | after->mem[0xfff0 | (r16_h + 1)];
    uint16_t expected_r16 = (uint16_t)(((before->mh << 8) | before->ml) + 3);

    if (actual_r16 != expected_r16) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected r16 (%d) to be 0x%04x, is 0x%04x\n", r16_h, expected_r16, actual_r16);
        return true;
    }

    return false;
}
