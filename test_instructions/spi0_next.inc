static bool test_instruction_spi0_next(int _data, int permutation, char **buffer_ptr, test_State* before, test_State* after) {
    uint8_t mo_u8 = (permutation >> 3) & 0x7f;  // 'U'; // 0x55
    uint8_t so_u8 = (uint8_t)((permutation >> 3) >> 7) & 0x7f;    // 'O'; // 0x4f
    uint8_t bit_index = permutation & 0x7;

    if (after == NULL) {
        if (bit_index == 0) {
            before->mem[0xfff0 | C_T] = mo_u8;
            before->c = 0;
        }

        before->in_miso = (so_u8 >> (7 - bit_index)) & 1; // msb first

        return permutation >= 8 * 0x80 * 0x80 - 1;
    }

    if (test_instruction_expect_pc_step(1, buffer_ptr, before, after)) return true;

    uint8_t expected_t = (uint8_t)(mo_u8 << (bit_index + 1)) | (uint8_t)(so_u8 >> (7 - bit_index));
    uint8_t actual_t   = after->mem[0xfff0 | C_T];

    if (expected_t != actual_t) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected t to be 0x%02x (%c), is 0x%02x (%c)\n", expected_t, expected_t, actual_t, actual_t);
        return true;
    }

    uint8_t expected_mosi = (mo_u8 >> (7 - bit_index)) & 1;
    uint8_t actual_mosi = after->tf >> 3;

    if (expected_mosi != actual_mosi) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected MOSI %d, is %d\n", expected_mosi, actual_mosi);
        return true;
    }

    if (after->c & 0x8) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected SCLK (C[3]) to be 0, is %d\n", (after->c >> 4) & 1);
        return true;
    }

    return false;
}

static bool test_instruction_spi2_begin(int _data, int permutation, char **buffer_ptr, test_State* before, test_State* after) {
    uint8_t i8 = 0xfb;

    if (after == NULL) {
        before->mem[test_state_M(before) + 1] = i8;

        before->mem[0xfff0 | C_T]  = 0xab;
        before->mem[0xfff0 | 0x8 | C_T]  = 0x12;
        before->mem[0xfff0 | 0x8 | C_ML] = 0x34;
        before->mem[0xfff0 | 0x8 | C_MH] = 0x56;
        return true;
    }

    if (test_instruction_expect_pc_step(2, buffer_ptr, before, after)) return true;

    uint8_t expected_t = before->mem[0xfff0 | 0x8 | C_T];
    uint8_t actual_t = after->mem[0xfff0 | C_T];

    if (actual_t != expected_t) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected C_T to be 0x%02x, is 0x%02x\n", expected_t, actual_t);
        return true;
    }

    uint8_t expected_08_t = before->mem[0xfff0 | C_T];
    uint8_t actual_08_t = after->mem[0xfff0 | 0x8 | C_T];

    if (actual_08_t != expected_08_t) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected 0x8 | C_T to be 0x%02x, is 0x%02x\n", expected_08_t, actual_08_t);
        return true;
    }

    uint8_t expected_ml = before->mem[0xfff0 | 0x8 | C_ML];
    uint8_t actual_ml = after->mem[0xfff0 | C_ML];

    if (actual_ml != expected_ml) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected C_ML to be 0x%02x, is 0x%02x\n", expected_ml, actual_ml);
        return true;
    }

    uint8_t expected_mh = before->mem[0xfff0 | 0x8 | C_MH];
    uint8_t actual_mh = after->mem[0xfff0 | C_MH];

    if (actual_mh != expected_mh) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected C_MH to be 0x%02x, is 0x%02x\n", expected_mh, actual_mh);
        return true;
    }

    if (!(after->c & 0x8)) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected SCLK (C[3]) to be 1, is %d\n", (after->c >> 4) & 1);
        return true;
    }

    return false;
}
