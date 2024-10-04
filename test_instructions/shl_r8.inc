static bool test_instruction_shl_r8(int r8, int permutation, char** buffer_ptr, test_State* before, test_State* after) {
    uint8_t r8_value = (uint8_t)permutation;

    if (after == NULL) {
        before->mem[0xfff0 | r8] = r8_value;

        *buffer_ptr += sprintf(*buffer_ptr, "shl 0x%02x (%d)", r8_value, r8);
        return permutation >= 0xff;
    };

    if (test_instruction_expect_pc_step(1, buffer_ptr, before, after)) return true;

    uint8_t actual_r8_value = after->mem[0xfff0 | r8];
    uint8_t expected_r8_value = (uint8_t)(r8_value << 1);

    if (expected_r8_value != actual_r8_value) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected r8 (%d) to be 0x%02x, is 0x%02x\n", r8, expected_r8_value, actual_r8_value);
        return true;
    }

    uint8_t flags = after->mem[0xfff0 | C_F];

    uint8_t expected_cf = r8_value >> 7;
    uint8_t actual_cf = flags & F_C;

    if (expected_cf != actual_cf) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected cf to be %d, is %d\n", expected_cf, actual_cf);
        return true;
    }

    if (expected_r8_value == 0 && !(flags & F_Z)) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected zf\n");
        return true;
    } else if (expected_r8_value != 0 && (flags & F_Z)) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected zf cleared\n");
        return true;
    }

    if (expected_r8_value >> 7 && !(flags & F_S)) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected sf\n");
        return true;
    } else if (!(expected_r8_value >> 7) && (flags & F_S)) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected sf cleared\n");
        return true;
    }

    return false;
}
