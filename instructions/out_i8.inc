static inline uint16_t instruction_out_i8(Operands o, uint8_t s, uint8_t  tf) {
    switch (s) {
    case 0x0: return S0_FETCH;
    case 0x1: return OE_MEM | SEL_C_LD_GPO | LD_S | INC_M;
    }
    return OE_T;
}
