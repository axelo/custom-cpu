static inline uint16_t instruction_jal_r16_i16(uint8_t r16_h, uint8_t r16_l, uint8_t s) {
    switch (s) {
    case 0x0: return S0_FETCH;
    case 0x1: return OE_MEM | LD_T | INC_M | LD_CSEL(C_T | C);   // T = hi addr, M++, C = C_T
    case 0x2: return OE_T   | LD_MEM       | LD_CSEL(M);         // mem[C] = T, C = M
    case 0x3: return OE_MEM | LD_T | INC_M | LD_CSEL(r16_h | C); // T = lo addr, M++, C = r16_h
    case 0x4: return OE_T   | LD_MEM       | LD_CSEL(C_ML | C);  // mem[C] = T, C = C_ML
    case 0x5: return OE_ALU | LD_T         | LD_CSEL(r16_l | C); // T = ML, C = r16_l
    case 0x6: return OE_T   | LD_MEM       | LD_CSEL(r16_h | C); // mem[C] = T, C = r16_h
    case 0x7: return OE_MEM | LD_ML        | LD_CSEL(C_MH | C);  // ML = mem[C], C = C_MH
    case 0x8: return OE_ALU | LD_T         | LD_CSEL(r16_h | C); // T = MH, C = r16_h
    case 0x9: return OE_T   | LD_MEM       | LD_CSEL(C_T | C);   // mem[C] = T, C = C_T
    case 0xa: return OE_MEM | LD_MH | LD_S | LD_CSEL(M);         // MH = mem[C]
    }
    return OE_T;
}

/*static inline uint16_t opcode_jal_r16_i16_v2(uint8_t r16_h, uint8_t r16_l, uint8_t s) {
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
