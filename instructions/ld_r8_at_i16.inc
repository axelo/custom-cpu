static inline uint16_t instruction_ld_r8_at_i16(Operands o, uint8_t s, uint8_t tf) {
    switch (s) {
    case 0x0: return S0_FETCH;
    case 0x1: return OE_MEM | LD_T | INC_M | LD_CSEL(C_T | C);    // T = address high, C = C_T
    case 0x2: return OE_T   | LD_MEM       | LD_CSEL(M);          // mem[C] = T, C = M
    case 0x3: return OE_MEM | LD_T | INC_M | LD_CSEL(C_MH | C);   // T = address low, C = C_MH
    case 0x4: return OE_ALU | LD_MEM       | LD_CSEL(C_ML | C);   // mem[C] = MH, C = C_ML
    case 0x5: return OE_ALU | LD_MEM;                             // mem[C] = ML
    case 0x6: return OE_T   | LD_ML        | LD_CSEL(C_T | C);    // ML = T, C = T
    case 0x7: return OE_MEM | LD_MH        | LD_CSEL(M);          // MH = mem[C]
    case 0x8: return OE_MEM | LD_T         | LD_CSEL(o.dst.r8 | C); // T = mem[M], C = dst_r8
    case 0x9: return OE_T   | LD_MEM       | LD_CSEL(C_MH | C);   // mem[dst_r8] = T, C = C_MH
    case 0xa: return OE_MEM | LD_MH        | LD_CSEL(C_ML | C);   // MH = mem[C], C = C_ML
    case 0xb: return OE_MEM | LD_ML | LD_S | LD_CSEL(M);          // ML = mem[C], C = M
    }
    return OE_T;
}

static bool test_instruction_ld_r8_at_i16(int permutation, Operands o, char** buffer_ptr, test_State* before, test_State* after) {
    srand((uint32_t)(permutation + 17));

    uint16_t i16 = (uint16_t)(rand() & 0xff00 & ((uint16_t)~before->mh << 8)) | ((rand() >> 8) & 0xff);
    uint8_t at_i16 = rand() & 0xff;

    if (after == NULL) {
        before->mem[test_state_M(before) + 1] = i16 >> 8;
        before->mem[test_state_M(before) + 2] = i16 & 0xff;

        before->mem[i16] = at_i16;

        *buffer_ptr += sprintf(*buffer_ptr, "imm 0x%04x", i16);
        return permutation >= 0xff;
    }

    if (test_instruction_expect_pc_step(3, buffer_ptr, before, after)) return true;

    uint8_t r8_value = after->mem[0xfff0 | o.dst.r8];

    if (r8_value != at_i16) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected r8 (%d) to be 0x%02x, is 0x%02x\n", o.dst.r8, at_i16, r8_value);
        return true;
    }

    return false;
}
