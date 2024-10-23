static inline uint16_t instruction_ld_r8_i8(Operands o, uint8_t s, uint8_t tf) {
    switch (s) {
    case 0:  return S0_FETCH;
    case 1:  return OE_MEM | LD_T | INC_M | LD_CSEL(o.dst.r8 | C);
    case 2:  return OE_T   | LD_MEM       | LD_CSEL(M)          | LD_S;
    default: return OE_T;
    }
}
