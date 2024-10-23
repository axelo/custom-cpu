static inline uint16_t instruction_push_r8(Operands o, uint8_t s, uint8_t tf) {
    switch (s) {
    case 0x0: return S0_FETCH;
    case 0x1: return OE_T                    | LD_CSEL(C_ML | C);
    case 0x2: return OE_ALU | LD_MEM         | LD_CSEL(C_MH | C);
    case 0x3: return OE_ALU | LD_MEM         | LD_CSEL(C_FF | C);
    case 0x4: return OE_MEM | LD_MH          | LD_CSEL(C_SP | C);
    case 0x5: return OE_MEM | LD_ML          | LD_CSEL(o.src.r8 | C);
    case 0x6: return OE_MEM | LD_T           | LD_CSEL(M);
    case 0x7: return OE_T   | LD_MEM | INC_M | LD_CSEL(C_ML | C);
    case 0x8: return OE_ALU | LD_T           | LD_CSEL(C_SP | C);
    case 0x9: return OE_T   | LD_MEM         | LD_CSEL(C_ML | C);
    case 0xa: return OE_MEM | LD_ML          | LD_CSEL(C_MH | C);
    case 0xb: return OE_MEM | LD_MH | LD_S   | LD_CSEL(M);
    }
    return OE_T;
}
