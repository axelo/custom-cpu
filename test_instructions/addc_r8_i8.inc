static bool test_instruction_addc_r8_i8(int r8, int permutation, char** buffer_ptr, test_State* before, test_State* after) {
    uint8_t r8_value = (uint8_t)(permutation >> 8);
    uint8_t i8 = permutation & 0xff;
    uint8_t ingoing_cf = (uint8_t)(permutation >> 16);

    if (after == NULL) {
        before->mem[test_state_M(before) + 1] = i8;
        before->mem[0xfff0 | r8] = r8_value;

        uint8_t flags = ingoing_cf ? F_C : 0;

        before->mem[0xfff0 | C_F] = flags;

        *buffer_ptr += sprintf(*buffer_ptr, "addc 0x%02x (%d), imm 0x%02x, cf before %d", r8_value, r8, i8, ingoing_cf);
        return permutation >= 0x1ffff;
    };

    if (test_instruction_expect_pc_step(2, buffer_ptr, before, after)) return true;

    uint8_t actual_r8_value = after->mem[0xfff0 | r8];
    uint8_t expected_r8_value = (uint8_t)(r8_value + i8 + ingoing_cf);

    if (expected_r8_value != actual_r8_value) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected r8 (%d) to be 0x%02x, is 0x%02x\n", r8, expected_r8_value, actual_r8_value);
        return true;
    }

    uint16_t q = r8_value + i8 + ingoing_cf;

    uint8_t flags = after->mem[0xfff0 | C_F];

    if (q >= 0x100 && !(flags & F_C)) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected cf\n");
        return true;
    } else if (q < 0x100 && (flags & F_C)) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected cf cleared\n");
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
