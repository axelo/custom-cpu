static inline uint16_t instruction_ld_r16_i16(uint8_t dst_rh8, uint8_t dst_rl8, uint8_t s) {
    switch (s) {
    case 0x0: return S0_FETCH;
    case 0x1: return OE_MEM | LD_T   | INC_M | LD_CSEL(dst_rh8 | C);
    case 0x2: return OE_T   | LD_MEM         | LD_CSEL(M);
    case 0x3: return OE_MEM | LD_T   | INC_M | LD_CSEL(dst_rl8 | C);
    case 0x4: return OE_T   | LD_MEM         | LD_CSEL(M)           | LD_S;
    }
    return OE_T;
}
