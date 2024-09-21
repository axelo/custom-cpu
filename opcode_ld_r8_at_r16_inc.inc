static inline uint16_t opcode_ld_r8_at_r16_inc(uint8_t dst_r8, uint8_t src_rh8, uint8_t src_rl8, uint8_t s) {
    switch (s) {
    case 0x0: return S0_FETCH;
    case 0x1: return OE_T   |                LD_CSEL(C_MH | C);    // C = C_MH
    case 0x2: return OE_ALU | LD_MEM       | LD_CSEL(C_ML | C);    // mem[C] = MH, C = C_ML
    case 0x3: return OE_ALU | LD_MEM       | LD_CSEL(src_rh8 | C); // mem[C] = ML, C = src_rh8
    case 0x4: return OE_MEM | LD_MH        | LD_CSEL(src_rl8 | C); // MH = mem[C], C = src_rl8
    case 0x5: return OE_MEM | LD_ML        | LD_CSEL(M);           // ML = mem[C], C = M
    case 0x6: return OE_MEM | LD_T | INC_M | LD_CSEL(dst_r8 | C);  // T = mem[M], M++, C = dst_r8
    case 0x7: return OE_T   | LD_MEM       | LD_CSEL(C_MH | C);    // mem[dst_r8] = T, C = C_MH
    case 0x8: return OE_ALU | LD_T         | LD_CSEL(src_rh8 | C); // T = MH, C = src_rh8
    case 0x9: return OE_T   | LD_MEM       | LD_CSEL(C_ML | C);    // mem[C] = T, C = C_ML
    case 0xa: return OE_ALU | LD_T         | LD_CSEL(src_rl8 | C); // T = ML, C = src_rl8
    case 0xb: return OE_T   | LD_MEM       | LD_CSEL(C_MH | C);    // mem[C] = T, C = C_MH
    case 0xc: return OE_MEM | LD_MH        | LD_CSEL(C_ML | C);    // MH = mem[C], C = C_ML
    case 0xd: return OE_MEM | LD_ML | LD_S | LD_CSEL(M);           // ML = mem[C], C = M
    }
    return OE_T;
}
