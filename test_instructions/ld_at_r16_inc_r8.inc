static bool test_instruction_ld_at_r16_inc_r8(int data, int permutation, char** buffer_ptr, test_State* before, test_State* after) {
    uint8_t r16_h = (uint8_t)data >> 4;
    uint8_t r8 = data & 0xf;

    if (after == NULL) {
        uint16_t i16 = rand() & 0xfff0;

        before->mem[0xfff0 | r8] = (uint8_t)permutation;
        before->mem[0xfff0 | r16_h] = i16 >> 8;
        before->mem[0xfff0 | (r16_h + 1)] = i16 & 0xff;

        return permutation >= 0xff;
    }

    if (test_instruction_expect_pc_step(1, buffer_ptr, before, after)) return true;

    uint16_t before_r16 = (uint16_t)(before->mem[0xfff0 | r16_h] << 8) | before->mem[0xfff0 | (r16_h + 1)];

    uint8_t expected_stored_r8 = before->mem[0xfff0 | r8];
    uint8_t actual_stored_r8 = after->mem[before_r16];

    if (expected_stored_r8 != actual_stored_r8) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected 0x%02x to be stored, is 0x%02x\n", expected_stored_r8, actual_stored_r8);
        return true;
    }

    uint16_t expected_r16 = before_r16 + 1;

    uint16_t actual_r16 = (uint16_t)(after->mem[0xfff0 | r16_h] << 8) | after->mem[0xfff0 | (r16_h + 1)];

    if (expected_r16 != actual_r16) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected r16 (%d) to be 0x%04x, is 0x%04x\n", r16_h, expected_r16, actual_r16);
        return true;
    }

    return false;
}
