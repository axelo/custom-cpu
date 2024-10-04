static bool test_instruction_pop_r16(int r16_h, int permutation, char** buffer_ptr, test_State* before, test_State* after) {
    uint16_t i16 = (uint16_t)(0xab00 | permutation);

    if (after == NULL) {
        before->mem[0xfff0 | r16_h] = 0x00;
        before->mem[0xfff0 | (r16_h + 1)] = 0x00;

        before->mem[0xfff0 | C_SP] = (uint8_t)permutation + 2;
        before->mem[0xff00 | before->mem[0xfff0 | C_SP] - 2] = i16 >> 8;
        before->mem[0xff00 | before->mem[0xfff0 | C_SP] - 1] = i16 & 0xff;

        return permutation >= 0x78;
    }

    if (test_instruction_expect_pc_step(1, buffer_ptr, before, after)) return true;

    int sp_diff = after->mem[0xfff0 | C_SP] - before->mem[0xfff0 | C_SP];

    if (sp_diff != -2) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected sp to move %d step(s), actual moved %d step(s)\n", -2, sp_diff);
        return true;
    }

    uint16_t expected_popped_i16 = i16;
    uint16_t actual_popped_i16 =
        (uint16_t)(after->mem[0xfff0 | r16_h] << 8) | (after->mem[0xfff0 | (r16_h + 1)]);

    if (actual_popped_i16 != expected_popped_i16) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected 0x%04x to be popped from the stack, was 0x%04x\n", expected_popped_i16, actual_popped_i16);
        return true;
    }

    return false;
}
