static inline uint16_t opcode_ld_r8_at_i16(uint8_t dst_r8, uint8_t s) {
    switch (s) {
    case 0x0: return S0_FETCH;
    case 0x1: return OE_MEM | LD_T | INC_M | LD_CSEL(C_T | C);    // T = address high, C = C_T
    case 0x2: return OE_T   | LD_MEM       | LD_CSEL(M);          // mem[C] = T, C = M
    case 0x3: return OE_MEM | LD_T | INC_M | LD_CSEL(C_MH | C);   // T = address low, C = C_MH
    case 0x4: return OE_ALU | LD_MEM       | LD_CSEL(C_ML | C);   // mem[C] = MH, C = C_ML
    case 0x5: return OE_ALU | LD_MEM;                             // mem[C] = ML
    case 0x6: return OE_T   | LD_ML        | LD_CSEL(C_T | C);    // ML = T, C = T
    case 0x7: return OE_MEM | LD_MH        | LD_CSEL(M);          // MH = mem[C]
    case 0x8: return OE_MEM | LD_T         | LD_CSEL(dst_r8 | C); // T = mem[M], C = dst_r8
    case 0x9: return OE_T   | LD_MEM       | LD_CSEL(C_MH | C);   // mem[dst_r8] = T, C = C_MH
    case 0xa: return OE_MEM | LD_MH        | LD_CSEL(C_ML | C);   // MH = mem[C], C = C_ML
    case 0xb: return OE_MEM | LD_ML | LD_S | LD_CSEL(M);          // ML = mem[C], C = M
    }
    return OE_T;
}
