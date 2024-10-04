static bool test_instruction_nop(int _data, int permutation, char **buffer_ptr, test_State* before, test_State* after) {
    if (after == NULL) return true;

    if (test_instruction_expect_pc_step(1, buffer_ptr, before, after)) return true;

    return false;
}
