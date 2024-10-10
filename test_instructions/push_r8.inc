static bool test_instruction_push_r8(int r8, int permutation, char** buffer_ptr, test_State* before, test_State* after) {
    if (after == NULL) {
        before->mem[0xfff0 | C_SP] = (uint8_t)permutation + 1;
        before->mem[0xfff0 | r8] = (uint8_t)permutation;
        return permutation >= 0xef;
    }

    if (test_instruction_expect_pc_step(1, buffer_ptr, before, after)) return true;

    int sp_diff = after->mem[0xfff0 | C_SP] - before->mem[0xfff0 | C_SP];

    if (sp_diff != 1) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected sp to move %d step(s), actual moved %d step(s)\n", 1, sp_diff);
        return true;
    }

    uint8_t expected_pushed_i8 = before->mem[0xfff0 | r8];
    uint8_t actual_pushed_i8 = after->mem[0xff00 | before->mem[0xfff0 | C_SP]];

    if (actual_pushed_i8 != expected_pushed_i8) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected 0x%02x to be pushed onto stack, is 0x%02x\n", expected_pushed_i8, actual_pushed_i8);
        return true;
    }

    return false;
}
