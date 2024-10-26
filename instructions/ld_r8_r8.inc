static inline uint16_t instruction_ld_r8_r8(Operands o, uint8_t s, uint8_t tf) {
    switch (s) {
    case 0x0: return S0_FETCH;
    case 0x1: return OE_T   |          LD_CSEL(o.src.r8 | C);
    case 0x2: return OE_MEM | LD_T   | LD_CSEL(o.dst.r8 | C);
    case 0x3: return OE_T   | LD_MEM | LD_CSEL(M)          | LD_S;
    }
    return OE_T;
}

static bool test_instruction_ld_r8_r8(int permutation, Operands o, char** buffer_ptr, test_State* before, test_State* after) {
    if (after == NULL) {
        before->mem[0xfff0 | o.src.r8] = (uint8_t) permutation;

        *buffer_ptr += sprintf(*buffer_ptr, "ld r%d, r%d (0x%02x)", o.dst.r8, o.src.r8, permutation);
        return permutation >= 0xff;
    };

    if (test_instruction_expect_pc_step(1, buffer_ptr, before, after)) return true;

    if (o.dst.r8 == o.src.r8) {
        *buffer_ptr += sprintf(*buffer_ptr, "same dst (%d) and src (%d) register", o.dst.r8, o.src.r8);
        return true;
    }

    uint8_t dest_r8_value = after->mem[0xfff0 | o.dst.r8];

    if (dest_r8_value != permutation) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected r%d to be 0x%02x, is 0x%02x\n", o.dst.r8, permutation, dest_r8_value);
        return true;
    }

    uint8_t src_r8_value = after->mem[0xfff0 | o.src.r8];

    if (src_r8_value != permutation) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected r%d to be 0x%02x, is 0x%02x\n", o.src.r8, permutation, src_r8_value);
        return true;
    }

    return false;
}
