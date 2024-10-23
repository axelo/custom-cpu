static inline uint16_t instruction_reset_cold_start(uint8_t s) {
    switch (s) {
    case 0x0: return OE_T                           | LD_CSEL(A_OE_MH);
    case 0x1: return OE_ALU | LD_ML                 | LD_CSEL(A_NAND);     // ML = MH
    case 0x2: return OE_ALU | LD_ML                 | LD_CSEL(A_OE_MH);    // ML = ~MH
    case 0x3: return OE_ALU | LD_MH  | INC_M        | LD_CSEL(A_ADD);      // ML = 2s complement
    case 0x4: return OE_ALU | LD_ML  | LD_MH | LD_T | LD_CSEL(C_F | C);    // ML, MH, T = MH - MH = 0
    case 0x5: return OE_T   | LD_MEM | LD_I         | LD_CSEL(A_BOOT | M); // mem[C_F] = 0, I = RESET (0)
    case 0x6: return OE_T   | SEL_C_LD_TF | LD_S;                          // this will take us out from reset cold start
                                                                           // and set all flags (which is a weird state on purpose)
    }
    return OE_T;
}

static inline uint16_t instruction_reset(Operands o, uint8_t s, uint8_t tf, uint8_t m13) {
    if (IS_TF_C_SET(tf) && IS_TF_Z_SET(tf) && IS_TF_S_SET(tf)) { // TF = 0x04
        switch (s) {
        // assumes LD_CSEL(A_BOOT | M);
        case 0x0: return OE_ALU | LD_MEM | INC_M | (m13 == 0
                                                    ? LD_S // keep copying by restarting S
                                                    : 0);  // copy done, go to next S
        // store 0xff at C_FF and C_F, 0x00 at C_SP then set I to NOP (0xff)
        // assumes that the next byte after end of boot rom is 0x00
        case 0x1: return OE_ALU | LD_ML  | LD_MH | LD_CSEL(A_NAND | M); // assume ALU is 0x00, M = 0x0000
        case 0x2: return OE_ALU | LD_T           | LD_CSEL(C_FF | C);   // T = 0xff
        case 0x3: return OE_T   | LD_MEM         | LD_CSEL(C_F | C);    // mem[C_FF] = T
        case 0x4: return OE_T   | LD_MEM         | LD_CSEL(C_ML | C);   // mem[C_F] = T
        case 0x5: return OE_ALU | LD_T           | LD_CSEL(C_SP | C);   // T = 0x00
        case 0x6: return OE_T   | LD_MEM         | LD_CSEL(C_FF | C);
        case 0x7: return OE_MEM | LD_I           | LD_CSEL(M);          // I = 0xff (NOP), sel C_M
                                                                        // we skip LD_S to prevent S0 that would increase PC,
                                                                        // we aren't actually running NOP from memory
        }

        return OE_T;
    } else {
        // TODO: untested, if we get here we're already booted - this clears M, sets all flags and restarts the instruction,
        // we could also just hang or jump to 0x0000
        switch (s) {
        case 0x0: return OE_T                           | LD_CSEL(A_OE_MH);
        case 0x1: return OE_ALU | LD_ML                 | LD_CSEL(A_NAND);     // ML = MH
        case 0x2: return OE_ALU | LD_ML                 | LD_CSEL(A_OE_MH);    // ML = ~MH
        case 0x3: return OE_ALU | LD_MH  | INC_M        | LD_CSEL(A_ADD);      // ML = 2s complement
        case 0x4: return OE_ALU | LD_ML  | LD_MH | LD_T | LD_CSEL(C_F);        // ML, MH, T = MH - MH = 0
        case 0x5: return OE_T   | LD_MEM                | LD_CSEL(A_BOOT | M); // mem[C_F] = 0x00
        case 0x6: return OE_T   | SEL_C_LD_TF | LD_S;                          // TF = 0x04
        }

        return OE_T;
    }
}
