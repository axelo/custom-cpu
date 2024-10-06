static bool test_instruction_out_tx_start(int _data, int permutation, char **buffer_ptr, test_State* before, test_State* after) {
    uint8_t u8 = 'U';

    if (after == NULL) {
        return permutation >= 7;
    };

    if (test_instruction_expect_pc_step(1, buffer_ptr, before, after)) return true;

    uint8_t expected_gpo = 0xfe;
    uint8_t actual_gpo   = after->gpo;

    if (expected_gpo != actual_gpo) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected gpo to be 0x%02x, is 0x%02x\n", expected_gpo, actual_gpo);
        return true;
    }

    return false;
}
