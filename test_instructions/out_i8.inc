static bool test_instruction_out_i8(int _data, int permutation, char **buffer_ptr, test_State* before, test_State* after) {
    uint8_t i8 = permutation & 0xff;

    if (after == NULL) {
        before->mem[test_state_M(before) + 1] = i8;

        return true;
    }

    if (test_instruction_expect_pc_step(2, buffer_ptr, before, after)) return true;

    uint8_t actual_gpo = after->gpo;

    if (i8 != actual_gpo) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected gpo to be 0x%02x, is 0x%02x\n", i8, actual_gpo);
        return true;
    }

    return false;
}
