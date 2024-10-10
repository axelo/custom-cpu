static bool test_instruction_spi_next(int _data, int permutation, char **buffer_ptr, test_State* before, test_State* after) {
    uint8_t mo_u8 = 'U'; // 0x55
    uint8_t so_u8 = 'O'; // 0x4f

    if (after == NULL) {
        if (permutation == 0) before->mem[0xfff0 | C_T] = mo_u8;

        before->in_miso = (so_u8 >> (7 - permutation)) & 1; // msb first

        return permutation >= 7;
    }

    if (test_instruction_expect_pc_step(1, buffer_ptr, before, after)) return true;

    uint8_t expected_t = (uint8_t)(mo_u8 << (permutation + 1)) | (uint8_t)(so_u8 >> (7 - permutation));
    uint8_t actual_t   = after->mem[0xfff0 | C_T];

    if (expected_t != actual_t) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected t to be 0x%02x (%c), is 0x%02x (%c)\n", expected_t, expected_t, actual_t, actual_t);
        return true;
    }

    uint8_t expected_mosi = (mo_u8 >> (7 - permutation)) & 1;
    uint8_t actual_mosi = after->tf >> 3;

    if (expected_mosi != actual_mosi) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected MOSI %d, is %d\n", expected_mosi, actual_mosi);
        return true;
    }

    return false;
}
