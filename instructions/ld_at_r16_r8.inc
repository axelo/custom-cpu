static inline uint16_t instruction_ld_at_r16_r8(Operands o, uint8_t s, uint8_t tf) {
    switch (s) {
    case 0x0: return S0_FETCH;
    case 0x1: return OE_T                    | LD_CSEL(C_MH | C);
    case 0x2: return OE_ALU | LD_MEM         | LD_CSEL(C_ML | C);
    case 0x3: return OE_ALU | LD_MEM         | LD_CSEL(o.src.r8 | C);
    case 0x4: return OE_MEM | LD_T           | LD_CSEL(o.dst.rh8 | C);
    case 0x5: return OE_MEM | LD_MH          | LD_CSEL(o.dst.rl8 | C);
    case 0x6: return OE_MEM | LD_ML          | LD_CSEL(M);
    case 0x7: return OE_T   | LD_MEM         | LD_CSEL(C_MH | C);
    case 0x8: return OE_MEM | LD_MH          | LD_CSEL(C_ML | C);
    case 0x9: return OE_MEM | LD_ML | LD_S   | LD_CSEL(M);
    }
    return OE_T;
}
