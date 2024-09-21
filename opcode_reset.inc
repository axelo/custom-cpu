static inline uint16_t opcode_reset(uint8_t s, uint8_t m7) {
    // copy byte, inc m and test zero
    switch (s) {
    case 0x0: return OE_T   |                  LD_CSEL(A_BOOT  | M);
    case 0x1: return OE_ALU | LD_MEM | INC_M | LD_CSEL(C_ML    | C);
    case 0x2: return OE_ALU | LD_MEM         | LD_CSEL(A_ADD_F | C);
    case 0x3: return OE_ALU | LD_ML          | LD_CSEL(C_ML    | C); // MLL = flags
    default:  break;
    }

    if (s == 0x4 && m7 == 0) {
        // keep copying by restoring ML and restarting S
        return OE_MEM | LD_ML | LD_S;

    } else {
        // copy done, store 0xff at C_FF, 0x01 at C_01 then set O to NOP
        switch (s) {
        case 0x04: return OE_MEM | LD_ML  | LD_MH | LD_CSEL(A_NAND | M); // assume mem[C_ML] is 0x00
        case 0x05: return OE_ALU | LD_T           | LD_CSEL(C_FF | C);   // T = 0xff
        case 0x06: return OE_T   | LD_MEM | INC_M | LD_CSEL(C_ML | C);   // mem[C_FF] = T, M++ (=> 0x01)
        case 0x07: return OE_ALU | LD_T           | LD_CSEL(C_01 | C);   // T = 0x01
        case 0x08: return OE_T   | LD_MEM         | LD_CSEL(C_ML | C);   // mem[C_01] = T
        case 0x09: return OE_MEM | LD_ML          | LD_CSEL(C_FF | C);   // M = 0x0000
        case 0x0a: return OE_MEM | LD_O           | LD_CSEL(M);          // O = 0xff (NOP), sel C_M
        default:  return 0;
        }
    }

    assert(0 && "invalid s");
}
