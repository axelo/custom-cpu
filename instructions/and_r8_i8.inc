static inline uint16_t instruction_and_r8_i8(Operands o, uint8_t s, uint8_t tf) {
    switch (s) {
    case 0x0: return S0_FETCH;
    case 0x1: return OE_MEM | LD_T  | INC_M | LD_CSEL(C_MH | C);   // T = i8, M++
    case 0x2: return OE_ALU | LD_MEM        | LD_CSEL(C_ML | C);
    case 0x3: return OE_ALU | LD_MEM        | LD_CSEL(o.dst.r8 | C);
    case 0x4: return OE_MEM | LD_ML;                               // ML = mem[dst_r8]
    case 0x5: return OE_T   | LD_MH         | LD_CSEL(A_NAND | C); // MH = T
    case 0x6: return OE_ALU | LD_ML | LD_MH;                       // ML,MH = (r8 nand i8)
    case 0x7: return OE_ALU | LD_T  | LD_ML | LD_CSEL(o.dst.r8 | C); // (r8 nand i8) nand (r8 nand i8) <=> r8 and i8
    case 0x8: return OE_T   | LD_MEM        | LD_CSEL(C_FF | C);
    case 0x9: return OE_MEM | LD_MH | INC_M | LD_CSEL(A_ADD_F_CF | C); // MH = 0xff, ML++, C = C_F (A_ADD_F_CF)
    case 0xa: return OE_ALU | LD_MEM        | LD_CSEL(C_MH | C);
    case 0xb: return OE_MEM | LD_MH         | LD_CSEL(C_ML | C);
    case 0xc: return OE_MEM | LD_ML | LD_S  | LD_CSEL(M);
    }
    return OE_T;
}
