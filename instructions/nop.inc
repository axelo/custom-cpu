static inline uint16_t instruction_nop(Operands o, uint8_t s, uint8_t tf) {
    switch (s) {
    case 0x0: return S0_FETCH;
    }
    return OE_T;
}
