static bool test_instruction_push_r16(int r16_h, int permutation, char** buffer_ptr, test_State* before, test_State* after) {
    if (after == NULL) {
        srand((uint32_t)(permutation + 37));

        before->mem[0xfff0 | C_SP] = (uint8_t)(permutation * 2);
        before->mem[0xfff0 | r16_h] = (uint8_t)rand();
        before->mem[0xfff0 | (r16_h + 1)] = (uint8_t)rand();
        return permutation >= (0xf0 / 2);
    }

    if (test_instruction_expect_pc_step(1, buffer_ptr, before, after)) return true;

    int sp_diff = after->mem[0xfff0 | C_SP] - before->mem[0xfff0 | C_SP];

    if (sp_diff != 2) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected sp to move %d step(s), actual moved %d step(s)\n", 2, sp_diff);
        return true;
    }

    uint8_t expected_pushed_i8_h = before->mem[0xfff0 | r16_h];
    uint8_t expected_pushed_i8_l = before->mem[0xfff0 | (r16_h + 1)];

    uint8_t actual_pushed_i8_h = after->mem[0xff00 | before->mem[0xfff0 | C_SP]];
    uint8_t actual_pushed_i8_l = after->mem[0xff00 | (before->mem[0xfff0 | C_SP] + 1)];

    if (actual_pushed_i8_h != expected_pushed_i8_h) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected 0x%02x to be pushed onto stack, is 0x%02x\n", expected_pushed_i8_h, actual_pushed_i8_h);
        return true;
    }

    if (actual_pushed_i8_l != expected_pushed_i8_l) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected 0x%02x to be pushed onto stack, is 0x%02x\n", expected_pushed_i8_l, actual_pushed_i8_l);
        return true;
    }

    return false;
}
