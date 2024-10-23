static inline uint16_t instruction_jmp_i16_conditional(bool do_jmp, uint8_t s) {
    switch (s) {
    case 0: return S0_FETCH;
    case 1: return OE_MEM | LD_T | INC_M      | LD_CSEL(C_F | C); // T = hi addr, M++, C = C_F
    case 2: return OE_MEM | SEL_C_LD_TF | SEL_C;                  // tf = mem[C]
    }

    if (do_jmp) {
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

static inline uint16_t instruction_jc_i16(Operands o, uint8_t s, uint8_t tf) {
    return instruction_jmp_i16_conditional(IS_TF_C_SET(tf), s);
}

static inline uint16_t instruction_jnc_i16(Operands o, uint8_t s, uint8_t tf) {
    return instruction_jmp_i16_conditional(!IS_TF_C_SET(tf), s);
}

static inline uint16_t instruction_jz_i16(Operands o, uint8_t s, uint8_t tf) {
    return instruction_jmp_i16_conditional(IS_TF_Z_SET(tf), s);
}

static inline uint16_t instruction_jnz_i16(Operands o, uint8_t s, uint8_t tf) {
    return instruction_jmp_i16_conditional(!IS_TF_Z_SET(tf), s);
}

static inline uint16_t instruction_jo_i16(Operands o, uint8_t s, uint8_t tf) {
    return instruction_jmp_i16_conditional(IS_TF_O_SET(tf), s);
}
