static bool test_instruction_ld_r8_r8(int r8_r8, int permutation, char** buffer_ptr, test_State* before, test_State* after) {
    uint8_t dest_r8 = r8_r8 >> 4 & 0xf;
    uint8_t src_r8 = r8_r8 & 0xf;

    if (after == NULL) {
        before->mem[0xfff0 | src_r8] = (uint8_t) permutation;

        *buffer_ptr += sprintf(*buffer_ptr, "ld r%d, r%d (0x%02x)", dest_r8, src_r8, permutation);
        return permutation >= 0xff;
    };

    if (test_instruction_expect_pc_step(1, buffer_ptr, before, after)) return true;

    uint8_t dest_r8_value = after->mem[0xfff0 | dest_r8];

    if (dest_r8_value != permutation) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected r%d to be 0x%02x, is 0x%02x\n", dest_r8, permutation, dest_r8_value);
        return true;
    }

    uint8_t src_r8_value = after->mem[0xfff0 | src_r8];

    if (src_r8_value != permutation) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected r%d to be 0x%02x, is 0x%02x\n", src_r8, permutation, src_r8_value);
        return true;
    }

    return false;
}
