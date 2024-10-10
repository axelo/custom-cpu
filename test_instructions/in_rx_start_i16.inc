static bool test_instruction_in_rx_start_i16(int _data, int permutation, char** buffer_ptr, test_State* before, test_State* after) {
    uint16_t i16 = 0x1234;
    bool got_rx_start_bit = permutation > 0;

    if (after == NULL) {
        before->mem[test_state_M(before) + 1] = i16 >> 8;
        before->mem[test_state_M(before) + 2] = i16 & 0xff;

        before->in_rx = got_rx_start_bit ? 0 : 1;

        return permutation >= 1;
    }

    uint16_t actual_pc = test_state_M(after);

    if (got_rx_start_bit) {
        if (actual_pc != i16) {
            *buffer_ptr += sprintf(*buffer_ptr, "expected pc to be 0x%04x, is 0x%04x\n", i16, actual_pc);
            return true;
        }
    } else {
        if (test_instruction_expect_pc_step(3, buffer_ptr, before, after)) return true;
    }

    return false;
}
