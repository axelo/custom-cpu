static bool test_instruction_ld_r8_i8(int r8, int permutation, char** buffer_ptr, test_State* before, test_State* after) {
    uint8_t i8 = permutation & 0xff;

    if (after == NULL) {
        before->mem[test_state_M(before) + 1] = i8;

        *buffer_ptr += sprintf(*buffer_ptr, "imm 0x%02x", i8);
        return permutation >= 0xff;
    };

    if (test_instruction_expect_pc_step(2, buffer_ptr, before, after)) return true;

    uint8_t r8_value = after->mem[0xfff0 | r8];

    if (r8_value != i8) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected r8 to be 0x%02x, is 0x%02x\n", i8, r8_value);
        return true;
    }

    return false;
}
