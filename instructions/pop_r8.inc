static inline uint16_t instruction_pop_r8(Operands o, uint8_t s, uint8_t tf) {
    switch (s) {
    case 0x0: return S0_FETCH;
    case 0x1: return OE_T                    | LD_CSEL(C_ML | C);
    case 0x2: return OE_ALU | LD_MEM         | LD_CSEL(C_MH | C);
    case 0x3: return OE_ALU | LD_MEM         | LD_CSEL(C_FF | C);
    case 0x4: return OE_MEM | LD_MH          | LD_CSEL(C_SP | C);
    case 0x5: return OE_MEM | LD_ML          | LD_CSEL(A_ADD | C);
    case 0x6: return OE_ALU | LD_ML          | LD_CSEL(M);
    case 0x7: return OE_MEM | LD_T           | LD_CSEL(o.dst.r8 | C);
    case 0x8: return OE_T   | LD_MEM         | LD_CSEL(C_ML | C);
    case 0x9: return OE_ALU | LD_T           | LD_CSEL(C_SP | C);
    case 0xa: return OE_T   | LD_MEM         | LD_CSEL(C_ML | C);
    case 0xb: return OE_MEM | LD_ML          | LD_CSEL(C_MH | C);
    case 0xc: return OE_MEM | LD_MH | LD_S   | LD_CSEL(M);
    }
    return OE_T;
}

static bool test_instruction_pop_r8(int permutation, Operands o, char** buffer_ptr, test_State* before, test_State* after) {
    uint8_t i8 = (uint8_t)permutation;

    if (after == NULL) {
        before->mem[0xfff0 | o.dst.r8] = 0x00;
        before->mem[0xfff0 | C_SP] = (uint8_t)permutation + 1;
        before->mem[0xff00 | (before->mem[0xfff0 | C_SP] - 1)] = i8;

        return permutation >= 0xef;
    }

    if (test_instruction_expect_pc_step(1, buffer_ptr, before, after)) return true;

    int sp_diff = after->mem[0xfff0 | C_SP] - before->mem[0xfff0 | C_SP];

    if (sp_diff != -1) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected sp to move %d step(s), actual moved %d step(s)\n", -1, sp_diff);
        return true;
    }

    uint8_t expected_popped_i8 = i8;
    uint8_t actual_popped_i8 = after->mem[0xfff0 | o.dst.r8];

    if (actual_popped_i8 != expected_popped_i8) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected 0x%02x to be popped from the stack, was 0x%02x\n", expected_popped_i8, actual_popped_i8);
        return true;
    }

    return false;
}
