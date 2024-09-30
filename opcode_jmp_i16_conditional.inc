static inline uint16_t opcode_jmp_i16_conditional(uint8_t do_jump, uint8_t s) {
    switch (s) {
    case 0: return S0_FETCH;
    case 1: return OE_MEM | LD_T | INC_M      | LD_CSEL(C_F | C); // T = hi addr, M++, C = C_F
    case 2: return OE_MEM | SEL_C_LD_TF;                          // tf = mem[C]
    }

    if (do_jump) {
        switch (s) {
        case 3: return OE_T                   | LD_CSEL(M); // C = M
        case 4: return OE_MEM | LD_ML | INC_M;              // ML = mem[M] (lo addr)
        case 5: return OE_T   | LD_MH | LD_S;               // MH = hi addr
        }
    } else {
        switch (s) {
        case 3: return OE_T   | INC_M | LD_S  | LD_CSEL(M);
        }
    }
    return OE_T;
}
