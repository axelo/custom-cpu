static bool test_instruction_in_rx(int _data, int permutation, char **buffer_ptr, test_State* before, test_State* after) {
    uint8_t u8 = 'U'; // 0x55 0101 0101

    if (after == NULL) {
        if (permutation == 0) before->mem[0xfff0 | C_T] = 'X'; // 0x58 0101 1000

        before->in_rx = (u8 >> permutation) & 1;

        return permutation >= 7;
    };

    if (test_instruction_expect_pc_step(1, buffer_ptr, before, after)) return true;

    uint8_t expected_t = (uint8_t)('X' >> (permutation + 1)) | (uint8_t)(u8 << (7 - permutation));
    uint8_t actual_t   = after->mem[0xfff0 | C_T];

    if (expected_t != actual_t) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected t to be 0x%02x (%c), is 0x%02x (%c)\n", expected_t, expected_t, actual_t, actual_t);
        return true;
    }

    return false;
}
