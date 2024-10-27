/*

dest + src + cf

0xff + 0x01 + 0x00 => cf set
0xff + 0x01 + 0x01 => cf set

0xfe + 0x01 + 0x00 => cf cleared
0xfe + 0x01 + 0x01 => cf set

*/

static inline uint16_t instruction_addc_r8_i8(Operands o, uint8_t s, uint8_t tf) {
    switch (s) {
    case 0x0: return S0_FETCH;
    case 0x1: return OE_MEM | LD_T | INC_M   | LD_CSEL(C_MH | C);
    case 0x2: return OE_ALU | LD_MEM         | LD_CSEL(C_ML | C);
    case 0x3: return OE_ALU | LD_MEM         | LD_CSEL(o.dst.r8 | C);
    case 0x4: return OE_MEM | LD_ML          | LD_CSEL(C_F | C);
    case 0x5: return OE_MEM | SEL_C_LD_TF;
    case 0x6: return OE_T   | LD_MH | ((TF_TO_F(tf) & F_C)
                                    ? INC_M  | LD_CSEL(A_ADD_F_CF)
                                    :          LD_CSEL(A_ADD_F));
    case 0x7: return OE_ALU | LD_T           | LD_CSEL(C_F | C);
    case 0x8: return OE_T   | LD_MEM         | LD_CSEL(A_ADD | C);
    case 0x9: return OE_ALU | LD_T           | LD_CSEL(o.dst.r8 | C);
    case 0xa: return OE_T   | LD_MEM         | LD_CSEL(C_MH | C);
    case 0xb: return OE_MEM | LD_MH          | LD_CSEL(C_ML | C);
    case 0xc: return OE_MEM | LD_ML | LD_S   | LD_CSEL(M);
    }
    return OE_T;
}

// static inline uint16_t instruction_addc_r8_i8(uint8_t dst_r8, uint8_t mhl, uint8_t s) {
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

// static inline uint16_t instruction_addc_r8_i8(uint8_t dst_r8, uint8_t mhl, uint8_t s) {
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

static bool test_instruction_addc_r8_i8(int permutation, Operands o, char** buffer_ptr, test_State* before, test_State* after) {
    uint8_t r8_value = (uint8_t)(permutation >> 8);
    uint8_t i8 = permutation & 0xff;
    uint8_t ingoing_cf = (uint8_t)(permutation >> 16);

    if (after == NULL) {
        before->mem[test_state_M(before) + 1] = i8;
        before->mem[0xfff0 | o.dst.r8] = r8_value;

        uint8_t flags = ingoing_cf ? F_C : 0;

        before->mem[0xfff0 | C_F] = (uint8_t)~flags; // flags are active low

        *buffer_ptr += sprintf(*buffer_ptr, "addc 0x%02x (%d), imm 0x%02x, cf before %d", r8_value, o.dst.r8, i8, ingoing_cf);
        return permutation >= 0x1ffff;
    };

    if (test_instruction_expect_pc_step(2, buffer_ptr, before, after)) return true;

    uint8_t actual_r8_value = after->mem[0xfff0 | o.dst.r8];
    uint8_t expected_r8_value = (uint8_t)(r8_value + i8 + ingoing_cf);

    if (expected_r8_value != actual_r8_value) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected r8 (%d) to be 0x%02x, is 0x%02x\n", o.dst.r8, expected_r8_value, actual_r8_value);
        return true;
    }

    uint16_t q = r8_value + i8 + ingoing_cf;

    uint8_t flags = (uint8_t)~after->mem[0xfff0 | C_F];

    if (q >= 0x100 && !(flags & F_C)) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected cf\n");
        return true;
    } else if (q < 0x100 && (flags & F_C)) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected cf cleared\n");
        return true;
    }

    if (expected_r8_value == 0 && !(flags & F_Z)) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected zf\n");
        return true;
    } else if (expected_r8_value != 0 && (flags & F_Z)) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected zf cleared\n");
        return true;
    }

    if (expected_r8_value >> 7 && !(flags & F_S)) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected sf\n");
        return true;
    } else if (!(expected_r8_value >> 7) && (flags & F_S)) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected sf cleared\n");
        return true;
    }

    return false;
}
