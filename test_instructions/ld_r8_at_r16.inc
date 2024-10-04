static bool test_instruction_ld_r8_at_r16(int r8_r16_h, int permutation, char** buffer_ptr, test_State* before, test_State* after) {
    srand((uint32_t)(permutation + 37));

    uint8_t r8 = (uint8_t)(r8_r16_h >> 4);
    uint8_t r16_h = r8_r16_h & 0xf;

    uint16_t i16 = (uint16_t)(rand() & 0xff00) | ((rand() >> 8) & 0xff);
    uint8_t at_i16 = rand() & 0xff;

    if (after == NULL) {
        before->mem[0xfff0 | r16_h] = i16 >> 8;
        before->mem[0xfff0 | (r16_h + 1)] = i16 & 0xff;

        before->mem[i16] = at_i16;

        *buffer_ptr += sprintf(*buffer_ptr, "i 0x%04x", i16);
        return permutation >= 0xff;
    }

    if (test_instruction_expect_pc_step(1, buffer_ptr, before, after)) return true;

    uint8_t r8_value = after->mem[0xfff0 | r8];

    if (r8_value != at_i16) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected r8 (%d) to be 0x%02x, is 0x%02x\n", r8, at_i16, r8_value);
        return true;
    }

    return false;
}
