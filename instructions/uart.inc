static inline uint16_t instruction_out_tx_start(uint8_t s) {
    switch (s) {
    case 0x0: return S0_FETCH;
    case 0x1: return OE_T   |                 LD_CSEL(C_MH | C);  // C = C_MH
    case 0x2: return OE_ALU | LD_MEM        | LD_CSEL(C_ML | C);  // mem[C] = MH, C = C_ML
    case 0x3: return OE_ALU | LD_MEM        | LD_CSEL(C_FF | C);
    case 0x4: return OE_MEM | LD_ML | LD_MH | LD_CSEL(A_ADD | C);
    case 0x5: return OE_ALU | LD_T;                               // T = 0xfe
    case 0x6: return OE_T;
    case 0x7: return OE_T   | SEL_C_LD_GPO;                       // GPO = T
    case 0x8: return OE_T;
    case 0x9: return OE_T;
    case 0xa: return OE_T                   | LD_CSEL(C_MH | C);
    case 0xb: return OE_MEM | LD_MH         | LD_CSEL(C_ML | C);
    case 0xc: return OE_MEM | LD_ML | LD_S  | LD_CSEL(M);
    }
    return OE_T;
}

static inline uint16_t instruction_out_tx_stop(uint8_t s) {
    switch (s) {
    case 0x0: return S0_FETCH;
    case 0x1: return OE_T   |                 LD_CSEL(C_FF | C);  // C = C_MH
    case 0x2: return OE_MEM | LD_T;                               // T = 0xff
    case 0x3: return OE_T;
    case 0x4: return OE_T;
    case 0x5: return OE_T;
    case 0x6: return OE_T;
    case 0x7: return OE_T   | SEL_C_LD_GPO;                       // GPO = T
    case 0x8: return OE_T;
    case 0x9: return OE_T;
    case 0xa: return OE_T;
    case 0xb: return OE_T;
    case 0xc: return OE_T   | LD_S          | LD_CSEL(M);
    }
    return OE_T;
}

static inline uint16_t instruction_out_tx(uint8_t s) {
    switch (s) {
    case 0x0: return S0_FETCH;
    case 0x1: return OE_T   |                 LD_CSEL(C_MH | C);    // C = C_MH
    case 0x2: return OE_ALU | LD_MEM        | LD_CSEL(C_ML | C);    // mem[C] = MH, C = C_ML
    case 0x3: return OE_ALU | LD_MEM        | LD_CSEL(C_FF | C);
    case 0x4: return OE_MEM | LD_ML         | LD_CSEL(C_T | C);     // ML = 0xff, C = C_T, assumes mem[C_T] contains byte which next bit to send
    case 0x5: return OE_MEM | LD_MH | INC_M | LD_CSEL(A_UNARY | C); // MH = byte to send, M++ (ML = 0), ALU = AU_SHR_F
    case 0x6: return OE_T   | INC_M;                                // ML = 1, ALU = AU_GPO_TX
    case 0x7: return OE_ALU | SEL_C_LD_GPO | INC_M;                 // GPO = 1111 111x where x is the left most bit of MH, M++ (ML = 2) ALU = AU_SHR
    case 0x8: return OE_ALU | LD_T          | LD_CSEL(C_T | C);     // T = MH shr 1
    case 0x9: return OE_T   | LD_MEM;                               // mem[C_T] = T
    case 0xa: return OE_T                   | LD_CSEL(C_MH | C);
    case 0xb: return OE_MEM | LD_MH         | LD_CSEL(C_ML | C);
    case 0xc: return OE_MEM | LD_ML | LD_S  | LD_CSEL(M);
    }
    return OE_T;
}

static inline uint16_t instruction_in_rx_start_i16(uint8_t tf, uint8_t s) {
    // assumes rts enbaled - GPO = 0xfe
    // jumps to i16 if start bit found
    switch (s) {
    case 0x0: return S0_FETCH;
    case 0x1: return OE_MEM       | LD_T | INC_M | LD_CSEL(C_T | C);
    case 0x2: return OE_T         | LD_MEM       | LD_CSEL(M);
    case 0x3: return OE_MEM       | LD_T | INC_M | LD_CSEL(M);
    case 0x4: return SEL_C_OE_GPI | SEL_C_LD_TF;
    case 0x5: return OE_T         | ((tf & 1)
                                    ?  LD_S
                                    : (LD_ML     | LD_CSEL(C_T | C)));
    case 0x6: return OE_MEM       | LD_MH | LD_S | LD_CSEL(M);
    }
    return OE_T;
}

static inline uint16_t instruction_in_rx(uint8_t tf, uint8_t s) {
    switch (s) {
    case 0x0: return S0_FETCH;
    case 0x1: return OE_T         |                          LD_CSEL(C_MH | C);    // C = C_MH
    case 0x2: return OE_ALU       | LD_MEM                 | LD_CSEL(C_ML | C);    // mem[C] = MH, C = C_ML
    case 0x3: return OE_ALU       | LD_MEM                 | LD_CSEL(C_FF | C);
    case 0x4: return OE_MEM       | LD_ML | LD_MH          | LD_CSEL(A_ADD | C);
    case 0x5: return OE_ALU       | LD_ML                  | LD_CSEL(C_T | C);     // ML = 0xfe
    case 0x6: return OE_MEM       | LD_MH | LD_T;                                  // MH = mem[C_T], LD_T = LD_GPI
    case 0x7: return SEL_C_OE_GPI | SEL_C_LD_TF;
    case 0x8: return OE_T         | ((tf & 1) ? INC_M : 0) | LD_CSEL(A_UNARY | C); // ML = 0xfe or 0xff
    case 0x9: return OE_ALU       | LD_T                   | LD_CSEL(C_T | C);
    case 0xa: return OE_T         | LD_MEM                 | LD_CSEL(C_MH | C);    // mem[C_T] = T
    case 0xb: return OE_MEM       | LD_MH                  | LD_CSEL(C_ML | C);
    case 0xc: return OE_MEM       | LD_ML | LD_S           | LD_CSEL(M);
    }
    return OE_T;
}

// static inline uint16_t instruction_in_uart_next_bit_old(uint8_t tf, uint8_t s) {
//     switch (s) {
//     case 0x0: return S0_FETCH;
//     case 0x1: return OE_T         |                          LD_CSEL(C_MH | C);    // C = C_MH
//     case 0x2: return OE_ALU       | LD_MEM                 | LD_CSEL(C_ML | C);    // mem[C] = MH, C = C_ML
//     case 0x3: return OE_ALU       | LD_MEM                 | LD_CSEL(C_FF | C);
//     case 0x4: return OE_MEM       | LD_ML                  | LD_CSEL(C_T | C);     // ML = 0xff
//     case 0x5: return OE_MEM       | LD_MH | INC_M;                                 // MH = mem[C_T], ML = 0
//     case 0x6: return OE_T         | INC_M;                                         // ML = 1
//     case 0x7: return SEL_C_OE_GPI | SEL_C_LD_TF | INC_M;                           // ML = 2
//     case 0x8: return OE_T         | INC_M;                                         // ML = 3
//     case 0x9: return OE_T         | ((tf & 1) ? INC_M : 0) | LD_CSEL(A_UNARY | C); // ML = 3 or 4
//     case 0xa: return OE_ALU       | LD_T                   | LD_CSEL(C_T | C);
//     case 0xb: return OE_T         | LD_MEM                 | LD_CSEL(C_MH | C);    // mem[C_T] = T
//     case 0xc: return OE_MEM       | LD_MH                  | LD_CSEL(C_ML | C);
//     case 0xd: return OE_MEM       | LD_ML | LD_S           | LD_CSEL(M);
//     }
//     return OE_T;
// }
