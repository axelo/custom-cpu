static bool test_instruction_ld_r8_at_at_i16(int r8, int permutation, char** buffer_ptr, test_State* before, test_State* after) {
    srand((uint32_t)(permutation + 42));

    uint16_t i16 = (uint16_t)(rand() & 0xff00 & ((uint16_t)~before->mh << 8)) | ((rand() >> 8) & 0xff);
    uint16_t at_i16 = (uint16_t)(rand() & 0xff00 & ((uint16_t)~(before->ml) << 8)) | ((rand() >> 8) & 0xff);
    uint8_t i8_at_at_i16 = rand() & 0xff;

    if (after == NULL) {
        before->mem[test_state_M(before) + 1] = i16 >> 8;
        before->mem[test_state_M(before) + 2] = i16 & 0xff;

        before->mem[i16] = at_i16 >> 8;
        before->mem[i16 + 1] = at_i16 & 0xff;

        before->mem[at_i16] = i8_at_at_i16;

        *buffer_ptr += sprintf(*buffer_ptr, "imm 0x%04x", i16);
        return permutation >= 0xff;
    }

    if (test_instruction_expect_pc_step(3, buffer_ptr, before, after)) return true;

    uint8_t r8_value = after->mem[0xfff0 | r8];

    if (r8_value != i8_at_at_i16) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected r8 (%d) to be 0x%02x, is 0x%02x\n", r8, i8_at_at_i16, r8_value);
        return true;
    }

    return false;
}
