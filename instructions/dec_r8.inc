static inline uint16_t instruction_dec_r8(Operands o, uint8_t s, uint8_t tf) {
    switch (s) {
    case 0x0: return S0_FETCH;
    case 0x1: return OE_MEM |                LD_CSEL(C_MH | C);
    case 0x2: return OE_ALU | LD_MEM       | LD_CSEL(C_ML | C);
    case 0x3: return OE_ALU | LD_MEM       | LD_CSEL(o.dst.r8 | C);
    case 0x4: return OE_MEM | LD_ML        | LD_CSEL(C_FF | C);
    case 0x5: return OE_MEM | LD_MH        | LD_CSEL(A_ADD_F | C);
    case 0x6: return OE_ALU | LD_T         | LD_CSEL(C_F | C);
    case 0x7: return OE_T   | LD_MEM       | LD_CSEL(A_ADD | C);
    case 0x8: return OE_ALU | LD_T         | LD_CSEL(o.dst.r8 | C);
    case 0x9: return OE_T   | LD_MEM       | LD_CSEL(C_MH | C);
    case 0xa: return OE_MEM | LD_MH        | LD_CSEL(C_ML | C);
    case 0xb: return OE_MEM | LD_ML | LD_S | LD_CSEL(M);
    }
    return OE_T;
}

static bool test_instruction_dec_r8(int permutation, Operands o, char **buffer_ptr, test_State* before, test_State* after) {
    if (after == NULL) {
        before->mem[0xfff0 | o.dst.r8] = (uint8_t)permutation;
        return permutation >= 0xff;
    }

    if (test_instruction_expect_pc_step(1, buffer_ptr, before, after)) return true;

    uint8_t expected_r8 = (permutation - 1) & 0xff;
    uint8_t actual_r8 = after->mem[0xfff0 | o.dst.r8];

    if (expected_r8 != actual_r8) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected r8 (%d) to be 0x%02x, is 0x%02x\n", o.dst.r8, expected_r8, actual_r8);
        return true;
    }

    uint8_t flags = (uint8_t)~after->mem[0xfff0 | C_F]; // flags are active low

    if ((permutation + 0xff) >= 0x100 && !(flags & F_C)) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected cf\n");
        return true;
    } else if ((permutation + 0xff) < 0x100 && flags & F_C) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected cf cleared\n");
        return true;
    }

    if (expected_r8 == 0 && !(flags & F_Z)) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected zf\n");
        return true;
    } else if (expected_r8 != 0 && (flags & F_Z)) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected zf cleared\n");
        return true;
    }

    if (expected_r8 >> 7 && !(flags & F_S)) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected sf\n");
        return true;
    } else if (!(expected_r8 >> 7) && (flags & F_S)) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected sf cleared\n");
        return true;
    }

    return false;
}
