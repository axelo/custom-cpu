/*

dest + src + cf

0xff + 0x01 + 0x00 => cf set
0xff + 0x01 + 0x01 => cf set

0xfe + 0x01 + 0x00 => cf cleared
0xfe + 0x01 + 0x01 => cf set

*/

static inline uint16_t opcode_addc_r8_i8(uint8_t dst_r8, uint8_t tf, uint8_t s) {
    switch (s) {
    case 0x0: return S0_FETCH;
    case 0x1: return OE_MEM | LD_T | INC_M   | LD_CSEL(C_MH | C);
    case 0x2: return OE_ALU | LD_MEM         | LD_CSEL(C_ML | C);
    case 0x3: return OE_ALU | LD_MEM         | LD_CSEL(dst_r8 | C);
    case 0x4: return OE_MEM | LD_ML          | LD_CSEL(C_F | C);
    case 0x5: return OE_MEM | SEL_C_LD_TF;
    case 0x6: return OE_T   | LD_MH | (tf & F_C
                                    ? INC_M  | LD_CSEL(A_ADD_F_CF)
                                    :          LD_CSEL(A_ADD_F));
    case 0x7: return OE_ALU | LD_T           | LD_CSEL(C_F | C);
    case 0x8: return OE_T   | LD_MEM         | LD_CSEL(A_ADD | C);
    case 0x9: return OE_ALU | LD_T           | LD_CSEL(dst_r8 | C);
    case 0xa: return OE_T   | LD_MEM         | LD_CSEL(C_MH | C);
    case 0xb: return OE_MEM | LD_MH          | LD_CSEL(C_ML | C);
    case 0xc: return OE_MEM | LD_ML | LD_S   | LD_CSEL(M);
    }
    return OE_T;
}

// static inline uint16_t opcode_addc_r8_i8(uint8_t dst_r8, uint8_t mhl, uint8_t s) {
//     switch (s) {
//     case 0x0: return S0_FETCH;
//     case 0x1: return OE_MEM | LD_T | INC_M | LD_CSEL(C_MH | C);        // T = i8, M++, C = C_MH
//     case 0x2: return OE_ALU | LD_MEM       | LD_CSEL(C_ML | C);        // mem[C] = MH, C = C_ML
//     case 0x3: return OE_ALU | LD_MEM       | LD_CSEL(C_F | C);         // mem[C] = ML, C = C_F
//     case 0x4: return OE_MEM | LD_MH;                                   // mhl = flags
//     case 0x5: return OE_T   | LD_ML;                                   // ML = i8
//     case 0x6: return OE_T   | (mhl & F_C) ? INC_M : 0 | LD_CSEL(C_MH); // M++ if CF set (ML + cf), C = C_MH
//     case 0x7: return OE_ALU | LD_T         | LD_CSEL(dst_r8 | C);      // T = intermediate CF (in M8), C = dst_r8
//     case 0x8: return OE_MEM | LD_MH        | LD_CSEL(A_ADD_F | C);     // MH = r8, C = add flags
//     case 0x9: return OE_ALU | LD_MEM       | LD_CSEL(A_ADD | C);       // mem[F] = add flags (int. cf lost in F), C = add
//     case 0xa: return OE_ALU | LD_MEM;                                  // mem[T] = i8 + cf + r8
//     case 0xb: return OE_T   | LD_MH        | LD_CSEL(C_F | C);         // MH = T (int. cf), C = C_F
//     case 0xc: return OE_MEM | LD_ML;                                   // ML = flags
//     case 0xd: return OE_T   | (mhl & F_C) ? INC_M : 0 | LD_CSEL(C_ML); // M++ if int. CF set, C = C_ML
//     case 0xe: return OE_ALU | LD_T         | LD_CSEL(C_F | C);         // T = flags with int. cf, C = C_F
//     case 0xf: return OE_T   | LD_MEM       | LD_CSEL(C_T | C);         // mem[F] = T, C = C_T
//     case 0x10:return OE_MEM | LD_T         | LD_CSEL(dst_r8 | C);      // T = mem[C], C = dst_r8
//     case 0x11:return OE_T   | LD_MEM       | LD_CSEL(C_MH | C);        // mem[C] = T, C = C_MH
//     case 0x12:return OE_MEM | LD_MH        | LD_CSEL(C_ML | C);        // MH = mem[C], C = C_ML
//     case 0x13:return OE_MEM | LD_ML | LD_S | LD_CSEL(M);               // ML = mem[C], C = M
//     }
//     return OE_T;
// }

// static inline uint16_t opcode_addc_r8_i8(uint8_t dst_r8, uint8_t mhl, uint8_t s) {
//     switch (s) {
//     case 0x0: return S0_FETCH;
//     case 0x1: return OE_MEM | LD_T | INC_M | LD_CSEL(C_MH | C);
//     case 0x2: return OE_ALU | LD_MEM       | LD_CSEL(C_ML | C);
//     case 0x3: return OE_ALU | LD_MEM       | LD_CSEL(dst_r8 | C);
//     case 0x4: return OE_MEM | LD_ML;
//     case 0x5: return OE_T   | LD_MH        | LD_CSEL(A_ADD_F | C);
//     case 0x6: return OE_ALU | LD_T         | LD_CSEL(C_T | C);
//     case 0x7: return OE_T   | LD_MEM       | LD_CSEL(A_ADD | C); // mem|C_T] = flags (r8 + i8), C = ADD
//     case 0x8: return OE_ALU | LD_ML        | LD_CSEL(C_F | C); // ML = r8 + i8, C = C_F
//     case 0x9: return OE_MEM | LD_MH; // mhl = prev. flags
//     case 0xa: return OE_T   | (mhl & F_C) ? INC_M | LD_CSEL(C_ML | C); // ml+1 if CF, m8 (mh0) if new cf
//     case 0xb: return OE_ALU | LD_T         | LD_CSEL(dst_r8 | C);
//     case 0xc: return OE_T   | LD_MEM       | LD_CSEL(C_T | C); // mem[C] = r8 + i8 + cf
//     case 0xd: return OE_MEM | LD_ML        | LD_CSEL(A_PROP_CF | C); // ml = new flags
//     case 0xe: return OE_ALU | LD_T         | LD_CSEL(C_F | C);
//     case 0xf: return OE_T   | LD_MEM       | LD_CSEL(C_MH | C);
//     case 0x10:return OE_MEM | LD_MH        | LD_CSEL(C_ML | C);
//     case 0x11:return OE_MEM | LD_ML | LD_S | LD_CSEL(M);
//     }
//     return OE_T;
// }
