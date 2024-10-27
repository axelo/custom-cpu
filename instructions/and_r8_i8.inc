static inline uint16_t instruction_and_r8_i8(Operands o, uint8_t s, uint8_t tf) {
    switch (s) {
    case 0x0: return S0_FETCH;
    case 0x1: return OE_MEM | LD_T  | INC_M | LD_CSEL(C_MH | C);   // T = i8, M++
    case 0x2: return OE_ALU | LD_MEM        | LD_CSEL(C_ML | C);
    case 0x3: return OE_ALU | LD_MEM        | LD_CSEL(o.dst.r8 | C);
    case 0x4: return OE_MEM | LD_ML;                               // ML = mem[dst_r8]
    case 0x5: return OE_T   | LD_MH         | LD_CSEL(A_NAND | C); // MH = T
    case 0x6: return OE_ALU | LD_ML | LD_MH;                       // ML,MH = (r8 nand i8)
    case 0x7: return OE_ALU | LD_T  | LD_ML | LD_CSEL(o.dst.r8 | C); // (r8 nand i8) nand (r8 nand i8) <=> r8 and i8
    case 0x8: return OE_T   | LD_MEM        | LD_CSEL(C_FF | C);
    case 0x9: return OE_MEM | LD_MH | INC_M | LD_CSEL(A_ADD_F_CF | C); // MH = 0xff, ML++, C = C_F (A_ADD_F_CF)
    case 0xa: return OE_ALU | LD_MEM        | LD_CSEL(C_MH | C);
    case 0xb: return OE_MEM | LD_MH         | LD_CSEL(C_ML | C);
    case 0xc: return OE_MEM | LD_ML | LD_S  | LD_CSEL(M);
    }
    return OE_T;
}

static bool test_instruction_and_r8_i8(int permutation, Operands o, char **buffer_ptr, test_State* before, test_State* after) {
    uint8_t r8_value = (uint8_t)(permutation >> 8);
    uint8_t i8 = permutation & 0xff;

    if (after == NULL) {
        before->mem[test_state_M(before) + 1] = i8;
        before->mem[0xfff0 | o.dst.r8] = r8_value;
        before->mem[0xfff0 | C_F] = 0;

        *buffer_ptr += sprintf(*buffer_ptr, "and 0x%02x (%d), imm 0x%02x", r8_value, o.dst.r8, i8);
        return permutation >= 0xffff;
    };

    if (test_instruction_expect_pc_step(2, buffer_ptr, before, after)) return true;

    uint8_t actual_r8_value = after->mem[0xfff0 | o.dst.r8];
    uint8_t expected_r8_value = r8_value & i8;

    if (expected_r8_value != actual_r8_value) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected r8 (%d) to be 0x%02x, is 0x%02x\n", o.dst.r8, expected_r8_value, actual_r8_value);
        return true;
    }

    uint8_t flags = (uint8_t)~after->mem[0xfff0 | C_F];

    if (!(flags & F_C)) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected cf due to adding with 0xff\n");
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
