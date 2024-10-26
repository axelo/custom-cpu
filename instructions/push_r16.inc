static inline uint16_t instruction_push_r16(Operands o, uint8_t s, uint8_t tf) {
    switch (s) {
    case 0x0: return S0_FETCH;
    case 0x1: return OE_T                    | LD_CSEL(C_ML | C);
    case 0x2: return OE_ALU | LD_MEM         | LD_CSEL(C_MH | C);
    case 0x3: return OE_ALU | LD_MEM         | LD_CSEL(C_FF | C);
    case 0x4: return OE_MEM | LD_MH          | LD_CSEL(C_SP | C);
    case 0x5: return OE_MEM | LD_ML          | LD_CSEL(o.src.rh8 | C);
    case 0x6: return OE_MEM | LD_T           | LD_CSEL(M);
    case 0x7: return OE_T   | LD_MEM | INC_M | LD_CSEL(o.src.rl8 | C);
    case 0x8: return OE_MEM | LD_T           | LD_CSEL(M);
    case 0x9: return OE_T   | LD_MEM | INC_M | LD_CSEL(C_ML | C);
    case 0xa: return OE_ALU | LD_T           | LD_CSEL(C_SP | C);
    case 0xb: return OE_T   | LD_MEM         | LD_CSEL(C_ML | C);
    case 0xc: return OE_MEM | LD_ML          | LD_CSEL(C_MH | C);
    case 0xd: return OE_MEM | LD_MH | LD_S   | LD_CSEL(M);
    }
    return OE_T;
}

static bool test_instruction_push_r16(int permutation, Operands o, char** buffer_ptr, test_State* before, test_State* after) {
    if (after == NULL) {
        srand((uint32_t)(permutation + 37));

        before->mem[0xfff0 | C_SP] = (uint8_t)(permutation * 2);
        before->mem[0xfff0 | o.src.rh8] = (uint8_t)rand();
        before->mem[0xfff0 | o.src.rl8] = (uint8_t)rand();
        return permutation >= (0xf0 / 2) - 1;
    }

    if (test_instruction_expect_pc_step(1, buffer_ptr, before, after)) return true;

    int sp_diff = after->mem[0xfff0 | C_SP] - before->mem[0xfff0 | C_SP];

    if (sp_diff != 2) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected sp to move %d step(s), actual moved %d step(s)\n", 2, sp_diff);
        return true;
    }

    uint8_t expected_pushed_i8_h = before->mem[0xfff0 | o.src.rh8];
    uint8_t expected_pushed_i8_l = before->mem[0xfff0 | o.src.rl8];

    uint8_t actual_pushed_i8_h = after->mem[0xff00 | before->mem[0xfff0 | C_SP]];
    uint8_t actual_pushed_i8_l = after->mem[0xff00 | (before->mem[0xfff0 | C_SP] + 1)];

    if (actual_pushed_i8_h != expected_pushed_i8_h) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected 0x%02x to be pushed onto stack, is 0x%02x\n", expected_pushed_i8_h, actual_pushed_i8_h);
        return true;
    }

    if (actual_pushed_i8_l != expected_pushed_i8_l) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected 0x%02x to be pushed onto stack, is 0x%02x\n", expected_pushed_i8_l, actual_pushed_i8_l);
        return true;
    }

    return false;
}
