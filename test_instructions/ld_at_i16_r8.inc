static bool test_instruction_ld_at_i16_r8(int r8, int permutation, char** buffer_ptr, test_State* before, test_State* after) {
    srand((uint32_t)(permutation + 22));

    uint16_t i16 = (uint16_t)(rand() & 0xff00 & ((uint16_t)~before->mh << 8)) | ((rand() >> 8) & 0xff);
    uint8_t i8 = rand() & 0xff;

    if (after == NULL) {
        before->mem[test_state_M(before) + 1] = i16 >> 8;
        before->mem[test_state_M(before) + 2] = i16 & 0xff;

        before->mem[0xfff0 | r8] = i8;
        before->mem[i16] = (uint8_t)(i8 + 17);

        *buffer_ptr += sprintf(*buffer_ptr, "imm 0x%04x", i16);
        return permutation >= 0xff;
    }

    if (test_instruction_expect_pc_step(3, buffer_ptr, before, after)) return true;

    uint8_t at_i16_value = after->mem[i16];

    if (at_i16_value != i8) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected [0x%04x] to be 0x%02x (%d), is 0x%02x\n", i16, i8, r8, at_i16_value);
        return true;
    }

    return false;
}
