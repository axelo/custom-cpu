static inline uint16_t instruction_spi_next_slower(uint8_t tf, uint8_t s) {
    switch (s) {
    case 0x0: return S0_FETCH;                                     // SCLK 0 (assumed)
    case 0x1: return OE_T                    | LD_CSEL(C_T | C);   // SCLK 0, C = C_T
    case 0x2: return OE_MEM       | SEL_C_LD_TF;                   // SCLK 0, TF[3] = MOSI on next clock
    case 0x3: return OE_T;                                         // SCLK 0, TF[3] = MOSI
    case 0x4: return OE_T;                                         // SCLK 0, TF[3] = MOSI
    case 0x5: return OE_T                    | LD_CSEL(0x8 | C);   // SCLK 0, MISO latched into GPI[1] on next clock
    case 0x6: return SEL_C_OE_GPI | SEL_C_LD_TF;                   // SCLK 1,TF[1] = MISO (MOSI lost but should have been latched by sub)
    case 0x7: return OE_T                    | LD_CSEL(C_ML | C);  // SCLK 1

    case 0x8: return OE_ALU | LD_MEM         | LD_CSEL(C_MH | C);  // SCLK 0
    case 0x9: return OE_ALU | LD_MEM         | LD_CSEL(C_T  | C);  // SCLK 0
    case 0xa: return OE_MEM | LD_ML;                               // SCLK 0, ML = mem[C_T]
    case 0xb: return OE_MEM | LD_MH | ((tf & 2)
                                        ? INC_M
                                        : 0) | LD_CSEL(A_ADD | C); // SCLK 0, MH = mem[C_T], ML + MISO (0/1)
    case 0xc: return OE_ALU | LD_T           | LD_CSEL(C_T | C);   // SCLK 0, T = mem[C_T] << 1 | MISO
    case 0xd: return OE_T   | LD_MEM         | LD_CSEL(C_MH | C);  // SCLK 0
    case 0xe: return OE_MEM | LD_MH          | LD_CSEL(C_ML | C);  // SCLK 0
    case 0xf: return OE_MEM | LD_ML | LD_S   | LD_CSEL(M);         // SCLK 0
    }
    return OE_T;
}

/*static inline uint16_t instruction_spi_next(uint8_t tf, uint8_t s) {
    switch (s) {
    case 0x0: return S0_FETCH;                                     // SCLK 0 (assumed)
    case 0x1: return OE_T                    | LD_CSEL(C_T | C);   // SCLK 0, C = C_T
    case 0x2: return OE_MEM       | SEL_C_LD_TF;                   // SCLK 0, TF[3] = MOSI on next clock
    case 0x3: return OE_T                    | LD_CSEL(0x8 | C_ML | C); // SCLK 1, MISO latched into GPI[1] on next clock
    case 0x4: return SEL_C_OE_GPI | SEL_C_LD_TF;                        // SCLK 1,TF[1] = MISO (MOSI lost unless re-wired to GPI but should have been latched by sub)

    case 0x5: return OE_ALU | LD_MEM         | LD_CSEL(C_MH | C);  // SCLK 0
    case 0x6: return OE_ALU | LD_MEM         | LD_CSEL(C_T  | C);  // SCLK 0
    case 0x7: return OE_MEM | LD_ML;                               // SCLK 0, ML = mem[C_T]
    case 0x8: return OE_MEM | LD_MH | ((tf & 2)
                                        ? INC_M
                                        : 0) | LD_CSEL(A_ADD | C); // SCLK 0, MH = mem[C_T], ML + MISO (0/1)
    case 0x9: return OE_ALU | LD_T           | LD_CSEL(C_T | C);   // SCLK 0, T = mem[C_T] << 1 | MISO
    case 0xa: return OE_T   | LD_MEM         | LD_CSEL(C_MH | C);  // SCLK 0
    case 0xb: return OE_MEM | LD_MH          | LD_CSEL(C_ML | C);  // SCLK 0
    case 0xc: return OE_MEM | LD_ML | LD_S   | LD_CSEL(M);         // SCLK 0
    }
    return OE_T;
}



static inline uint16_t instruction_spi_clock(uint8_t tf, uint8_t s) {
    switch (s) {
    case 0x0: return S0_FETCH;                                       // SCLK 0 (assumed)
    case 0x1: return OE_T                       | LD_CSEL(C_T | C);  // SCLK 0, C = C_T
    case 0x2: return OE_MEM       | SEL_C_LD_TF;                     // SCLK 0, TF[3] = MOSI
    case 0x3: return OE_T                       | LD_CSEL(0x4  | M); // SCLK 1, MISO latched into GPI[1]
    case 0x4: return SEL_C_OE_GPI | SEL_C_LD_TF | LD_S;              // SCLK 1,TF[1] = MISO (MOSI lost but should have been latched by sub)
    }
    return OE_T;
}

static inline uint16_t instruction_spi_next_bit(uint8_t tf, uint8_t s) {
    switch (s) {
    case 0x0: return S0_FETCH;                                     // SCLK x
    case 0x1: return OE_T                    | LD_CSEL(C_ML | C);  // SCLK 0
    case 0x2: return OE_ALU | LD_MEM         | LD_CSEL(C_MH | C);  // SCLK 0
    case 0x3: return OE_ALU | LD_MEM         | LD_CSEL(C_T  | C);  // SCLK 0
    case 0x4: return OE_MEM | LD_ML;                               // SCLK 0, ML = mem[C_T]
    case 0x5: return OE_MEM | LD_MH | ((tf & 2)
                                        ? INC_M
                                        : 0) | LD_CSEL(A_ADD | C); // SCLK 0, MH = mem[C_T], ML + MISO (0/1)
    case 0x6: return OE_ALU | LD_T           | LD_CSEL(C_T | C);   // SCLK 0, T = mem[C_T] << 1 | MISO
    case 0x7: return OE_T   | LD_MEM         | LD_CSEL(C_MH | C);  // SCLK 0
    case 0x8: return OE_MEM | LD_MH          | LD_CSEL(C_ML | C);  // SCLK 0
    case 0x9: return OE_MEM | LD_ML | LD_S   | LD_CSEL(M);         // SCLK 0
    }
    return OE_T;
}





static inline uint16_t instruction_spi_shift(uint8_t s) {
    switch (s) {
    case 0x0: return S0_FETCH;
    case 0x1: return OE_T   |                 LD_CSEL(C_MH | C);
    case 0x2: return OE_ALU | LD_MEM        | LD_CSEL(C_ML | C);
    case 0x3: return OE_ALU | LD_MEM        | LD_CSEL(C_FF | C);
    case 0x4: return OE_MEM | LD_ML | LD_MH | LD_CSEL(A_ADD_F | C); // ML,MH = 0xff
    case 0x5: return OE_ALU | LD_ML         | LD_CSEL(C_T | C);     // ML = 0x09
    case 0x6: return OE_MEM | LD_MH         | LD_CSEL(A_UNARY | C); // MH = mem[C_T]
    case 0x7: return OE_ALU | SEL_C_LD_GPO;                         // GPO = SCLK 0 SCP 0, MOSI t[7]
    case 0x8: return OE_T                   | LD_CSEL(C_MH | C);
    case 0x9: return OE_MEM | LD_MH         | LD_CSEL(C_ML | C);
    case 0xa: return OE_MEM | LD_ML | LD_S  | LD_CSEL(M);
    }
    return OE_T;
}

static inline uint16_t instruction_spi_sample(uint8_t tf, uint8_t s) {
    switch (s) {
    case 0x0: return S0_FETCH;
    case 0x1: return OE_T   |                 LD_CSEL(C_MH | C);
    case 0x2: return OE_ALU | LD_MEM        | LD_CSEL(C_ML | C);
    case 0x3: return OE_ALU | LD_MEM        | LD_CSEL(C_FF | C);
    case 0x4: return OE_MEM | LD_ML | LD_MH | LD_CSEL(A_ADD_F | C); // ML,MH = 0xff
    case 0x5: return OE_ALU | LD_ML         | LD_CSEL(C_T | C);     // ML = 0x09
    case 0x6: return OE_MEM | LD_MH | INC_M | LD_CSEL(A_UNARY | C); // MH = mem[C_T], ML = 0x0a
    case 0x7: return OE_ALU | SEL_C_LD_GPO;                         // GPO = SCLK 1 SCS 0, MOSI t[7]
    case 0x8: return SEL_C_OE_GPI | SEL_C_LD_TF | INC_M;            // TF = GPI, ML = 0x0b
    case 0x9: return OE_T   | ((tf & 2)
                               ? INC_M                              // ML = 0x0b if MISO 0, 0x0c if MISO 1
                               : 0)         | LD_CSEL(C_T | C);     // C = C_T
    case 0xa: return OE_MEM | LD_MH         | LD_CSEL(A_UNARY | C); // MH = mem[C], ALU = UNARY
    case 0xb: return OE_ALU | LD_T          | LD_CSEL(C_T | C);     // T = MH << 1 | 0 or 1
    case 0xc: return OE_T   | LD_MEM        | LD_CSEL(C_MH | C);
    case 0xd: return OE_MEM | LD_MH         | LD_CSEL(C_ML | C);
    case 0xe: return OE_MEM | LD_ML | LD_S  | LD_CSEL(M);
    }
    return OE_T;
}






static inline uint16_t instruction_spi_shift_v2(uint8_t tf, uint8_t s) {
    switch (s) {
    case 0x0: return S0_FETCH;
    case 0x1: return OE_T   |                 LD_CSEL(C_MH | C);
    case 0x2: return OE_ALU | LD_MEM        | LD_CSEL(C_ML | C);
    case 0x3: return OE_ALU | LD_MEM        | LD_CSEL(C_FF | C);
    case 0x4: return OE_MEM | LD_ML | LD_MH | LD_CSEL(A_ADD_F | C); // ML,MH = 0xff
    case 0x5: return OE_ALU | LD_ML         | LD_CSEL(C_T | C);     // ML = 0x09
    case 0x6: return OE_MEM | LD_MH         | LD_CSEL(A_UNARY | C); // MH = mem[C_T]
    case 0x7: return OE_ALU | SEL_C_LD_TF;                         // TF = SCLK 0, MOSI t[7]
    case 0x8: return OE_T                   | LD_CSEL(C_MH | C);
    case 0x9: return OE_MEM | LD_MH         | LD_CSEL(C_ML | C);
    case 0xa: return OE_MEM | LD_ML | LD_S  | LD_CSEL(M);
    }
    return OE_T;
}

*/
