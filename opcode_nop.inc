static inline uint16_t opcode_nop(uint8_t s) {
    switch (s) {
    case 0x0: return S0_FETCH;
    }
    return OE_T;
}
