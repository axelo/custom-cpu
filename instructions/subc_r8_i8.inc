static inline uint16_t instruction_subc_r8_i8(Operands o, uint8_t s, uint8_t tf) {
    switch (s) {
    case 0x0: return S0_FETCH;
    case 0x1: return OE_MEM | LD_T | INC_M   | LD_CSEL(C_MH | C);
    case 0x2: return OE_ALU | LD_MEM         | LD_CSEL(C_ML | C);
    case 0x3: return OE_ALU | LD_MEM         | LD_CSEL(C_F | C);
    case 0x4: return OE_MEM | SEL_C_LD_TF;
    case 0x5: return OE_T   | LD_ML | LD_MH  | LD_CSEL(A_NAND);
    case 0x6: return OE_ALU | LD_ML          | LD_CSEL(o.dst.r8 | C); // ML = ~i8 (ones complement)
    case 0x7: return OE_MEM | LD_MH | (IS_TF_C_SET(tf)
                                    ? INC_M  | LD_CSEL(A_ADD_F_CF)  // ML += cf (ensure MH unchanged)
                                    :          LD_CSEL(A_ADD_F));
    case 0x9: return OE_ALU | LD_T           | LD_CSEL(C_F | C);
    case 0xa: return OE_T   | LD_MEM         | LD_CSEL(A_ADD | C);
    case 0xb: return OE_ALU | LD_T           | LD_CSEL(o.dst.r8 | C);
    case 0xc: return OE_T   | LD_MEM         | LD_CSEL(C_MH | C);
    case 0xd: return OE_MEM | LD_MH          | LD_CSEL(C_ML | C);
    case 0xe: return OE_MEM | LD_ML | LD_S   | LD_CSEL(M);
    }
    return OE_T;
}

/*static inline uint16_t instruction_subc_r8_i8_v1(uint8_t dst_r8, uint8_t tf, uint8_t s) {
    switch (s) {
    case 0x0: return S0_FETCH;
    case 0x1: return OE_MEM | LD_T | INC_M   | LD_CSEL(C_MH | C);
    case 0x2: return OE_ALU | LD_MEM         | LD_CSEL(C_ML | C);
    case 0x3: return OE_ALU | LD_MEM         | LD_CSEL(C_F | C);
    case 0x4: return OE_MEM | SEL_C_LD_TF;
    case 0x5: return OE_T   | LD_ML | LD_MH  | LD_CSEL(A_NAND);
    case 0x6: return OE_ALU | LD_ML          | LD_CSEL(dst_r8 | C); // ML = ~i8
    case 0x7: return OE_MEM | LD_MH | INC_M  | LD_CSEL(C_MH | C);   // ML = ~i8 + 1 (two's complement), MH = dst_r8
    case 0x8: return OE_ALU | LD_MH | (tf & F_C
                                    ? INC_M  | LD_CSEL(A_ADD_F_CF)  // ML += cf (ensure MH unchanged)
                                    :          LD_CSEL(A_ADD_F));
    case 0x9: return OE_ALU | LD_T           | LD_CSEL(C_F | C);
    case 0xa: return OE_T   | LD_MEM         | LD_CSEL(A_ADD | C);
    case 0xb: return OE_ALU | LD_T           | LD_CSEL(dst_r8 | C);
    case 0xc: return OE_T   | LD_MEM         | LD_CSEL(C_MH | C);
    case 0xd: return OE_MEM | LD_MH          | LD_CSEL(C_ML | C);
    case 0xe: return OE_MEM | LD_ML | LD_S   | LD_CSEL(M);
    }
    return OE_T;
}
*/
