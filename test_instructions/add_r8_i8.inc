static bool test_instruction_add_r8_i8(int r8, int permutation, char** buffer_ptr, test_State* before, test_State* after) {
    uint8_t r8_value = (uint8_t)(permutation >> 8);
    uint8_t i8 = permutation & 0xff;

    if (after == NULL) {
        before->mem[test_state_M(before) + 1] = i8;
        before->mem[0xfff0 | r8] = r8_value;
        before->mem[0xfff0 | C_F] = 0;

        *buffer_ptr += sprintf(*buffer_ptr, "add 0x%02x (%d), imm 0x%02x", r8_value, r8, i8);
        return permutation >= 0xffff;
    };

    if (test_instruction_expect_pc_step(2, buffer_ptr, before, after)) return true;

    uint8_t actual_r8_value = after->mem[0xfff0 | r8];
    uint8_t expected_r8_value = (uint8_t)(r8_value + i8);

    if (expected_r8_value != actual_r8_value) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected r8 (%d) to be 0x%02x, is 0x%02x\n", r8, expected_r8_value, actual_r8_value);
        return true;
    }

    uint8_t flags = after->mem[0xfff0 | C_F]; // flags are active low

    if (r8_value + i8 >= 0x100 && !IS_TF_C_SET(flags)) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected cf\n");
        return true;
    } else if (r8_value + i8 < 0x100 && IS_TF_C_SET(flags)) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected cf cleared\n");
        return true;
    }

    if (expected_r8_value == 0 && !IS_TF_Z_SET(flags)) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected zf\n");
        return true;
    } else if (expected_r8_value != 0 && IS_TF_Z_SET(flags)) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected zf cleared\n");
        return true;
    }

    if (expected_r8_value >> 7 && !IS_TF_S_SET(flags)) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected sf\n");
        return true;
    } else if (!(expected_r8_value >> 7) && IS_TF_S_SET(flags)) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected sf cleared\n");
        return true;
    }

    return false;
}
