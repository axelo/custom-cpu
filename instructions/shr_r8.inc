static inline uint16_t instruction_shr_r8(Operands o, uint8_t s, uint8_t tf) {
    switch (s) {
    case 0x0: return S0_FETCH;
    case 0x1: return OE_T   |                  LD_CSEL(C_MH | C);
    case 0x2: return OE_ALU | LD_MEM         | LD_CSEL(C_ML | C);
    case 0x3: return OE_ALU | LD_MEM         | LD_CSEL(C_FF | C);
    case 0x4: return OE_MEM | LD_ML          | LD_CSEL(o.dst.r8 | C);  // ML = 0xff
    case 0x5: return OE_MEM | LD_MH  | INC_M | LD_CSEL(A_UNARY | C); // MH = mem[r8], ML+1 (0), AU_SHR_F
    case 0x6: return OE_ALU | LD_T   | INC_M | LD_CSEL(C_F | C);
    case 0x7: return OE_T   | LD_MEM | INC_M | LD_CSEL(A_UNARY | C); // AU_SHR (2)
    case 0x8: return OE_ALU | LD_T           | LD_CSEL(o.dst.r8 | C);
    case 0x9: return OE_T   | LD_MEM         | LD_CSEL(C_MH | C);
    case 0xa: return OE_MEM | LD_MH          | LD_CSEL(C_ML | C);
    case 0xb: return OE_MEM | LD_ML | LD_S   | LD_CSEL(M);
    }
    return OE_T;
}

// static inline uint16_t instruction_shr_r8_passed(uint8_t dst_r8, uint8_t s) {
//     switch (s) {
//     case 0x0: return S0_FETCH;
//     case 0x1: return OE_T   |                  LD_CSEL(C_MH | C);
//     case 0x2: return OE_ALU | LD_MEM         | LD_CSEL(C_ML | C);
//     case 0x3: return OE_ALU | LD_MEM         | LD_CSEL(dst_r8 | C);
//     case 0x4: return OE_MEM | LD_MH          | LD_CSEL(C_FF | C);    // AU_SHR_F
//     case 0x5: return OE_MEM | LD_ML          | LD_CSEL(A_UNARY | C);
//     case 0x6: return OE_ALU | LD_T   | INC_M | LD_CSEL(C_F | C);
//     case 0x7: return OE_T   | LD_MEM | INC_M | LD_CSEL(A_UNARY | C); // AU_SHR
//     case 0x8: return OE_ALU | LD_T           | LD_CSEL(dst_r8 | C);
//     case 0x9: return OE_T   | LD_MEM         | LD_CSEL(C_MH | C);
//     case 0xa: return OE_MEM | LD_MH          | LD_CSEL(C_ML | C);
//     case 0xb: return OE_MEM | LD_ML | LD_S   | LD_CSEL(M);
//     }
//     return OE_T;
// }
