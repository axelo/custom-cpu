static inline uint16_t instruction_jmp_i16_conditional(Operands o, uint8_t s, uint8_t tf) {
    uint8_t flags = TF_TO_F(tf);

    bool do_jump  =
        ((flags  | FLAG_MASK_ANY) & o.src.flag_mask_set) &&
        ((~flags | FLAG_MASK_ANY) & o.src.flag_mask_unset);

    switch (s) {
    case 0: return S0_FETCH;
    case 1: return OE_MEM | LD_T | INC_M      | LD_CSEL(C_F | C); // T = hi addr, M++, C = C_F
    case 2: return OE_MEM | SEL_C_LD_TF | SEL_C;                  // tf = mem[C]
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

static bool test_instruction_jmp_i16_conditional(int permutation, Operands o, char** buffer_ptr, test_State* before, test_State* after) {
    srand((uint32_t)(permutation + 17));

    uint16_t i16 = (uint16_t)(rand() & 0xff00) | ((rand() >> 8) & 0xff);

    uint8_t flags = (uint8_t)permutation;

    if (after == NULL) {
        before->mem[test_state_M(before) + 1] = i16 >> 8;
        before->mem[test_state_M(before) + 2] = i16 & 0xff;

        before->mem[0xfff0 | C_F] = (uint8_t)~flags; // flags are active low

        return permutation >= 0xf;
    }

    uint16_t actual_pc = test_state_M(after);

    bool do_jump  =
        ((flags  | FLAG_MASK_ANY) & o.src.flag_mask_set) &&
        ((~flags | FLAG_MASK_ANY) & o.src.flag_mask_unset);

    if (do_jump) {
        if (actual_pc != i16) {
            *buffer_ptr += sprintf(*buffer_ptr, "expected pc to jump to 0x%04x, is 0x%04x - test flags: 0x%x\n", i16, actual_pc, flags);
            return true;
        }
    } else {
        if (test_instruction_expect_pc_step(3, buffer_ptr, before, after)) {
            *buffer_ptr += sprintf(*buffer_ptr, "expected pc not to jump - test flags: 0x%x\n", flags);

            return true;
        }
    }

    return false;
}
