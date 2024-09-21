static inline uint16_t opcode_ld_r16_i16(uint8_t dst_rh8, uint8_t dst_rl8, uint8_t s) {
    switch (s) {
    case 0:  return S0_FETCH;
    case 1:  return OE_MEM | LD_T   | INC_M | LD_CSEL(dst_rh8 | C);
    case 2:  return OE_T   | LD_MEM         | LD_CSEL(M);
    case 3:  return OE_MEM | LD_T   | INC_M | LD_CSEL(dst_rl8 | C);
    case 4:  return OE_T   | LD_MEM         | LD_CSEL(M)           | LD_S;
    default: return OE_T;
    }
}
