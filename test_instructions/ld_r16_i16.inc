static bool test_instruction_ld_r16_i16(int r8_h, int permutation, char** buffer_ptr, test_State* before, test_State* after) {
    srand((uint32_t)(permutation + 42));

    uint16_t i16 = (rand() & 0xff00) | ((rand() >> 8) & 0xff);

    if (after == NULL) {
        before->mem[test_state_M(before) + 1] = i16 >> 8;
        before->mem[test_state_M(before) + 2] = i16 & 0xff;

        *buffer_ptr += sprintf(*buffer_ptr, "imm 0x%04x", i16);
        return permutation >= 0xff;
    };

    if (test_instruction_expect_pc_step(3, buffer_ptr, before, after)) return true;

    uint8_t r8_h_value = after->mem[0xfff0 | r8_h];
    uint8_t r8_l_value = after->mem[0xfff0 | (r8_h + 1)];

    uint16_t r16_value = (uint16_t)(r8_h_value << 8 | r8_l_value);

    if (r16_value != i16) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected r16 (%d) to be 0x%04x, is 0x%04x\n", r8_h, i16, r16_value);
        return true;
    }

    return false;
}
