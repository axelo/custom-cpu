static inline uint16_t instruction_jal_r16_i16(Operands o, uint8_t s, uint8_t tf) {
    switch (s) {
    case 0x0: return S0_FETCH;
    case 0x1: return OE_MEM | LD_T | INC_M | LD_CSEL(C_T | C);   // T = hi addr, M++, C = C_T
    case 0x2: return OE_T   | LD_MEM       | LD_CSEL(M);         // mem[C] = T, C = M
    case 0x3: return OE_MEM | LD_T | INC_M | LD_CSEL(o.dst.rh8 | C); // T = lo addr, M++, C = r16_h
    case 0x4: return OE_T   | LD_MEM       | LD_CSEL(C_ML | C);  // mem[C] = T, C = C_ML
    case 0x5: return OE_ALU | LD_T         | LD_CSEL(o.dst.rl8 | C); // T = ML, C = r16_l
    case 0x6: return OE_T   | LD_MEM       | LD_CSEL(o.dst.rh8 | C); // mem[C] = T, C = r16_h
    case 0x7: return OE_MEM | LD_ML        | LD_CSEL(C_MH | C);  // ML = mem[C], C = C_MH
    case 0x8: return OE_ALU | LD_T         | LD_CSEL(o.dst.rh8 | C); // T = MH, C = r16_h
    case 0x9: return OE_T   | LD_MEM       | LD_CSEL(C_T | C);   // mem[C] = T, C = C_T
    case 0xa: return OE_MEM | LD_MH | LD_S | LD_CSEL(M);         // MH = mem[C]
    }
    return OE_T;
}

/*static inline uint16_t instruction_jal_r16_i16_v2(uint8_t r16_h, uint8_t r16_l, uint8_t s) {
    switch (s) {
    case 0x0: return S0_FETCH;
    case 0x1: return OE_T                  | LD_CSEL(C_MH | C);
    case 0x2: return OE_ALU | LD_T         | LD_CSEL(r16_h | C); // T = MH
    case 0x3: return OE_T   | LD_MEM       | LD_CSEL(C_ML | C);  // mem[C] = T
    case 0x4: return OE_ALU | LD_T         | LD_CSEL(r16_l | C); // T = ML
    case 0x5: return OE_T   | LD_MEM       | LD_CSEL(M);         // mem[C] = T
    case 0x6: return OE_MEM | LD_T | INC_M;                      // T = hi addr, M++
    case 0x7: return OE_MEM | LD_ML;                             // ML = lo addr
    case 0x8: return OE_T   | LD_MH | LD_S;                      // MH = T
    }
    return OE_T;
}*/

static bool test_instruction_jal_r16_i16(int permutation, Operands o, char** buffer_ptr, test_State* before, test_State* after) {
    srand((uint32_t)(permutation + 33));

    uint16_t i16 = (uint16_t)(rand() & 0xff00) | ((rand() >> 8) & 0xff);

    if (after == NULL) {
        before->mh = (uint8_t)permutation;
        before->ml = 0xcd;
        before->mem[test_state_M(before) + 1] = i16 >> 8;
        before->mem[test_state_M(before) + 2] = i16 & 0xff;
        return permutation >= 0xff;
    }

    uint16_t actual_pc = test_state_M(after);

    if (actual_pc != i16) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected pc to be 0x%04x, is 0x%04x\n", i16, actual_pc);
        return true;
    }

    uint16_t actual_r16 = (uint16_t)(after->mem[0xfff0 | o.dst.rh8] << 8) | after->mem[0xfff0 | o.dst.rl8];
    uint16_t expected_r16 = (uint16_t)(((before->mh << 8) | before->ml) + 3);

    if (actual_r16 != expected_r16) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected r16 (%d) to be 0x%04x, is 0x%04x\n", o.dst.rh8, expected_r16, actual_r16);
        return true;
    }

    return false;
}
