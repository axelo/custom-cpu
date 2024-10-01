static inline uint16_t instruction_reset(uint8_t s, uint8_t m13) {
    // copy byte, inc m and test zero
    switch (s) {
    case 0x0: return OE_T   |                 LD_CSEL(A_BOOT | M);
    case 0x1: return OE_ALU | LD_MEM | INC_M;
    default:  break;
    }

    if (s == 0x2 && m13 == 0) {
        // keep copying by restarting S
        return OE_T | LD_S;

    } else {
        // copy done, store 0xff at C_FF, 0x00 at C_SP then set O to NOP
        switch (s) {
        case 0x02: return OE_MEM | LD_ML  | LD_MH | LD_CSEL(A_NAND | M); // assume mem[C_ML] is 0x00, M = 0x0000
        case 0x03: return OE_ALU | LD_T           | LD_CSEL(C_FF | C);   // T = 0xff
        case 0x04: return OE_T   | LD_MEM         | LD_CSEL(C_ML | C);   // mem[C_FF] = T
        case 0x05: return OE_ALU | LD_T           | LD_CSEL(C_SP | C);   // T = 0x00
        case 0x06: return OE_T   | LD_MEM         | LD_CSEL(C_FF | C);   // M = 0x0000
        case 0x07: return OE_MEM | LD_O           | LD_CSEL(M);          // O = 0xff (NOP), sel C_M
        default:   return OE_T;
        }
    }

    assert(0 && "invalid s");
}
