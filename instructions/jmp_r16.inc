static inline uint16_t instruction_jmp_r16(uint8_t r16_h, uint8_t r16_l, uint8_t s) {
    switch (s) {
    case 0: return S0_FETCH;
    case 1: return OE_T                  | LD_CSEL(r16_h | C);
    case 2: return OE_MEM | LD_MH        | LD_CSEL(r16_l | C);
    case 3: return OE_MEM | LD_ML | LD_S | LD_CSEL(M);
    }
    return OE_T;
}
