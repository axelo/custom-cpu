static inline uint16_t instruction_jmp_r16(Operands o, uint8_t s, uint8_t tf) {
    switch (s) {
    case 0: return S0_FETCH;
    case 1: return OE_T                  | LD_CSEL(o.src.rh8 | C);
    case 2: return OE_MEM | LD_MH        | LD_CSEL(o.src.rl8 | C);
    case 3: return OE_MEM | LD_ML | LD_S | LD_CSEL(M);
    }
    return OE_T;
}
