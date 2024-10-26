static inline uint16_t instruction_ld_at_r16_r8(Operands o, uint8_t s, uint8_t tf) {
    switch (s) {
    case 0x0: return S0_FETCH;
    case 0x1: return OE_T                    | LD_CSEL(C_MH | C);
    case 0x2: return OE_ALU | LD_MEM         | LD_CSEL(C_ML | C);
    case 0x3: return OE_ALU | LD_MEM         | LD_CSEL(o.src.r8 | C);
    case 0x4: return OE_MEM | LD_T           | LD_CSEL(o.dst.rh8 | C);
    case 0x5: return OE_MEM | LD_MH          | LD_CSEL(o.dst.rl8 | C);
    case 0x6: return OE_MEM | LD_ML          | LD_CSEL(M);
    case 0x7: return OE_T   | LD_MEM         | LD_CSEL(C_MH | C);
    case 0x8: return OE_MEM | LD_MH          | LD_CSEL(C_ML | C);
    case 0x9: return OE_MEM | LD_ML | LD_S   | LD_CSEL(M);
    }
    return OE_T;
}

static bool test_instruction_ld_at_r16_r8(int permutation, Operands o, char** buffer_ptr, test_State* before, test_State* after) {
    if (after == NULL) {
        uint16_t i16 = rand() & 0xfff0;

        before->mem[0xfff0 | o.src.r8] = (uint8_t)permutation;
        before->mem[0xfff0 | o.dst.rh8] = i16 >> 8;
        before->mem[0xfff0 | o.dst.rl8] = i16 & 0xff;

        return permutation >= 0xff;
    }

    if (test_instruction_expect_pc_step(1, buffer_ptr, before, after)) return true;

    uint16_t before_r16 = (uint16_t)(before->mem[0xfff0 | o.dst.rh8] << 8) | before->mem[0xfff0 | o.dst.rl8];

    uint8_t expected_stored_r8 = before->mem[0xfff0 | o.src.r8];
    uint8_t actual_stored_r8 = after->mem[before_r16];

    if (expected_stored_r8 != actual_stored_r8) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected 0x%02x to be stored, is 0x%02x\n", expected_stored_r8, actual_stored_r8);
        return true;
    }

    uint16_t expected_r16 = before_r16;

    uint16_t actual_r16 = (uint16_t)(after->mem[0xfff0 | o.dst.rh8] << 8) | after->mem[0xfff0 | o.dst.rl8];

    if (expected_r16 != actual_r16) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected r16 (%d) to be 0x%04x, is 0x%04x\n", o.dst.rh8, expected_r16, actual_r16);
        return true;
    }

    return false;
}
