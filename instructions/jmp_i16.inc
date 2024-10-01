static inline uint16_t instruction_jmp_i16(uint8_t s) {
    switch (s) {
    case 0: return S0_FETCH;
    case 1: return OE_MEM | LD_T  | INC_M;
    case 2: return OE_MEM | LD_ML;
    case 3: return OE_T   | LD_MH | LD_S;
    }
    return OE_T;
}
