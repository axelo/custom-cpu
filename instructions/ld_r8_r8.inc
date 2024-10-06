static inline uint16_t instruction_ld_r8_r8(uint8_t dst_r8, uint8_t src_r8, uint8_t s) {
    switch (s) {
    case 0x0: return S0_FETCH;
    case 0x1: return OE_T   |          LD_CSEL(src_r8 | C);
    case 0x2: return OE_MEM | LD_T   | LD_CSEL(dst_r8 | C);
    case 0x3: return OE_T   | LD_MEM | LD_CSEL(M)          | LD_S;
    }
    return OE_T;
}
