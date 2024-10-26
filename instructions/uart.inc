static inline uint16_t instruction_out_tx_start(Operands o, uint8_t s, uint8_t tf) {
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

static inline uint16_t instruction_out_tx_stop(Operands o, uint8_t s, uint8_t tf) {
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

static inline uint16_t instruction_out_tx(Operands o, uint8_t s, uint8_t tf) {
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

static inline uint16_t instruction_in_rx_start_i16(Operands o, uint8_t s, uint8_t tf) {
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

static inline uint16_t instruction_in_rx(Operands o, uint8_t s, uint8_t tf) {
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

static bool test_instruction_out_tx_start(int permutation, Operands o, char **buffer_ptr, test_State* before, test_State* after) {
    uint8_t u8 = 'U';

    if (after == NULL) {
        return permutation >= 7;
    };

    if (test_instruction_expect_pc_step(1, buffer_ptr, before, after)) return true;

    uint8_t expected_gpo = 0xfe;
    uint8_t actual_gpo   = after->gpo;

    if (expected_gpo != actual_gpo) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected gpo to be 0x%02x, is 0x%02x\n", expected_gpo, actual_gpo);
        return true;
    }

    return false;
}

static bool test_instruction_out_tx(int permutation, Operands o, char **buffer_ptr, test_State* before, test_State* after) {
    uint8_t u8 = 'U';

    if (after == NULL) {
        if (permutation == 0) before->mem[0xfff0 | C_T] = u8;

        return permutation >= 7;
    };

    if (test_instruction_expect_pc_step(1, buffer_ptr, before, after)) return true;

    uint8_t expected_gpo = 0xfe | ((u8 >> permutation) & 1);
    uint8_t actual_gpo   = after->gpo;

    if (expected_gpo != actual_gpo) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected gpo to be 0x%02x, is 0x%02x\n", expected_gpo, actual_gpo);
        return true;
    }

    uint8_t expected_t = u8 >> (permutation + 1);
    uint8_t actual_t   = after->mem[0xfff0 | C_T];

    if (expected_t != actual_t) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected t to be 0x%02x, is 0x%02x\n", expected_t, actual_t);
        return true;
    }

    return false;
}

static bool test_instruction_out_tx_stop(int permutation, Operands o, char **buffer_ptr, test_State* before, test_State* after) {
    uint8_t u8 = 'U';

    if (after == NULL) {
        return permutation >= 7;
    };

    if (test_instruction_expect_pc_step(1, buffer_ptr, before, after)) return true;

    uint8_t expected_gpo = 0xff;
    uint8_t actual_gpo   = after->gpo;

    if (expected_gpo != actual_gpo) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected gpo to be 0x%02x, is 0x%02x\n", expected_gpo, actual_gpo);
        return true;
    }

    return false;
}

static bool test_instruction_in_rx_start_i16(int permutation, Operands o, char** buffer_ptr, test_State* before, test_State* after) {
    uint16_t i16 = 0x1234;
    bool got_rx_start_bit = permutation > 0;

    if (after == NULL) {
        before->mem[test_state_M(before) + 1] = i16 >> 8;
        before->mem[test_state_M(before) + 2] = i16 & 0xff;

        before->in_rx = got_rx_start_bit ? 0 : 1;

        return permutation >= 1;
    }

    uint16_t actual_pc = test_state_M(after);

    if (got_rx_start_bit) {
        if (actual_pc != i16) {
            *buffer_ptr += sprintf(*buffer_ptr, "expected pc to be 0x%04x, is 0x%04x\n", i16, actual_pc);
            return true;
        }
    } else {
        if (test_instruction_expect_pc_step(3, buffer_ptr, before, after)) return true;
    }

    return false;
}

static bool test_instruction_in_rx(int permutation, Operands o, char **buffer_ptr, test_State* before, test_State* after) {
    uint8_t u8 = 'U'; // 0x55 0101 0101

    if (after == NULL) {
        if (permutation == 0) before->mem[0xfff0 | C_T] = 'X'; // 0x58 0101 1000

        before->in_rx = (u8 >> permutation) & 1;

        return permutation >= 7;
    };

    if (test_instruction_expect_pc_step(1, buffer_ptr, before, after)) return true;

    uint8_t expected_t = (uint8_t)('X' >> (permutation + 1)) | (uint8_t)(u8 << (7 - permutation));
    uint8_t actual_t   = after->mem[0xfff0 | C_T];

    if (expected_t != actual_t) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected t to be 0x%02x (%c), is 0x%02x (%c)\n", expected_t, expected_t, actual_t, actual_t);
        return true;
    }

    return false;
}
