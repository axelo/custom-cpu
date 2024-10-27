static inline uint16_t instruction_shr_r8(Operands o, uint8_t s, uint8_t tf) {
    switch (s) {
    case 0x0: return S0_FETCH;
    case 0x1: return OE_T   |                  LD_CSEL(C_MH | C);
    case 0x2: return OE_ALU | LD_MEM         | LD_CSEL(C_ML | C);
    case 0x3: return OE_ALU | LD_MEM         | LD_CSEL(C_FF | C);
    case 0x4: return OE_MEM | LD_ML          | LD_CSEL(o.dst.r8 | C);  // ML = 0xff
    case 0x5: return OE_MEM | LD_MH  | INC_M | LD_CSEL(A_UNARY | C); // MH = mem[r8], ML+1 (0), AU_SHR_F
    case 0x6: return OE_ALU | LD_T   | INC_M | LD_CSEL(C_F | C);
    case 0x7: return OE_T   | LD_MEM | INC_M | LD_CSEL(A_UNARY | C); // AU_SHR (2)
    case 0x8: return OE_ALU | LD_T           | LD_CSEL(o.dst.r8 | C);
    case 0x9: return OE_T   | LD_MEM         | LD_CSEL(C_MH | C);
    case 0xa: return OE_MEM | LD_MH          | LD_CSEL(C_ML | C);
    case 0xb: return OE_MEM | LD_ML | LD_S   | LD_CSEL(M);
    }
    return OE_T;
}

// static inline uint16_t instruction_shr_r8_passed(uint8_t dst_r8, uint8_t s) {
//     switch (s) {
//     case 0x0: return S0_FETCH;
//     case 0x1: return OE_T   |                  LD_CSEL(C_MH | C);
//     case 0x2: return OE_ALU | LD_MEM         | LD_CSEL(C_ML | C);
//     case 0x3: return OE_ALU | LD_MEM         | LD_CSEL(dst_r8 | C);
//     case 0x4: return OE_MEM | LD_MH          | LD_CSEL(C_FF | C);    // AU_SHR_F
//     case 0x5: return OE_MEM | LD_ML          | LD_CSEL(A_UNARY | C);
//     case 0x6: return OE_ALU | LD_T   | INC_M | LD_CSEL(C_F | C);
//     case 0x7: return OE_T   | LD_MEM | INC_M | LD_CSEL(A_UNARY | C); // AU_SHR
//     case 0x8: return OE_ALU | LD_T           | LD_CSEL(dst_r8 | C);
//     case 0x9: return OE_T   | LD_MEM         | LD_CSEL(C_MH | C);
//     case 0xa: return OE_MEM | LD_MH          | LD_CSEL(C_ML | C);
//     case 0xb: return OE_MEM | LD_ML | LD_S   | LD_CSEL(M);
//     }
//     return OE_T;
// }

static bool test_instruction_shr_r8(int permutation, Operands o, char** buffer_ptr, test_State* before, test_State* after) {
    uint8_t r8_value = (uint8_t)permutation;

    if (after == NULL) {
        before->mem[0xfff0 | o.dst.r8] = r8_value;

        *buffer_ptr += sprintf(*buffer_ptr, "shr 0x%02x (%d)", r8_value, o.dst.r8);
        return permutation >= 0xff;
    };

    if (test_instruction_expect_pc_step(1, buffer_ptr, before, after)) return true;

    uint8_t actual_r8_value = after->mem[0xfff0 | o.dst.r8];
    uint8_t expected_r8_value = (uint8_t)(r8_value >> 1);

    if (expected_r8_value != actual_r8_value) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected r8 (%d) to be 0x%02x, is 0x%02x\n", o.dst.r8, expected_r8_value, actual_r8_value);
        return true;
    }

    uint8_t flags = (uint8_t)~after->mem[0xfff0 | C_F]; // flags are active low

    uint8_t expected_cf = r8_value & 1;
    uint8_t actual_cf = flags & F_C;

    if (expected_cf != actual_cf) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected cf to be %d, is %d\n", expected_cf, actual_cf);
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
