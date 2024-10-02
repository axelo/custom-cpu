typedef struct {
    uint8_t i;
    uint8_t s; // 4 bit
    uint8_t ml;
    uint8_t mh;
    uint8_t c;
    uint8_t t;
    uint8_t tf; // 4 bit
    uint8_t mem[0x10000];
} test_State;

typedef struct {
    Instruction i;
    const char* name;
    int data;
    bool (*test)(int, int, char**, test_State*, test_State*);
} test_Test_case;

#define test_state_M(state) ((uint16_t)(((state)->mh << 8) | (state)->ml))
#define test_state_M_or_C(state) ((state)->c & 0x10)

static bool test_instruction_nop(int _data, int permutation, char **buffer_ptr, test_State* before, test_State* after) {
    if (after == NULL) return true;

    int pc_diff = test_state_M(after) - test_state_M(before);

    if (pc_diff != 1) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected pc to move %d step(s), actual moved %d step(s)\n", 1, pc_diff);
        return true;
    }

    return false;
}

static bool test_instruction_ld_r8_i8(int r8, int permutation, char** buffer_ptr, test_State* before, test_State* after) {
    uint8_t i8 = permutation & 0xff;

    if (after == NULL) {
        before->mem[test_state_M(before) + 1] = i8;

        *buffer_ptr += sprintf(*buffer_ptr, "imm 0x%02x", i8);
        return permutation >= 0xff;
    };

    int pc_diff = test_state_M(after) - test_state_M(before);

    if (pc_diff != 2) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected pc to move %d step(s), actual moved %d step(s)\n", 2, pc_diff);
        return true;
    }

    uint8_t r8_value = after->mem[0xfff0 | r8];

    if (r8_value != i8) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected r8 to be 0x%02x, is 0x%02x\n", i8, r8_value);
        return true;
    }

    return false;
}

static bool test_instruction_ld_r8_r8(int r8_r8, int permutation, char** buffer_ptr, test_State* before, test_State* after) {
    uint8_t dest_r8 = r8_r8 >> 4 & 0xf;
    uint8_t src_r8 = r8_r8 & 0xf;

    if (after == NULL) {
        before->mem[0xfff0 | src_r8] = (uint8_t) permutation;

        *buffer_ptr += sprintf(*buffer_ptr, "ld r%d, r%d (0x%02x)", dest_r8, src_r8, permutation);
        return permutation >= 0xff;
    };

    int pc_diff = test_state_M(after) - test_state_M(before);

    if (pc_diff != 1) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected pc to move %d step(s), actual moved %d step(s)\n", 1, pc_diff);
        return true;
    }

    uint8_t dest_r8_value = after->mem[0xfff0 | dest_r8];

    if (dest_r8_value != permutation) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected r%d to be 0x%02x, is 0x%02x\n", dest_r8, permutation, dest_r8_value);
        return true;
    }

    uint8_t src_r8_value = after->mem[0xfff0 | src_r8];

    if (src_r8_value != permutation) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected r%d to be 0x%02x, is 0x%02x\n", src_r8, permutation, src_r8_value);
        return true;
    }

    return false;
}

static bool test_instruction_ld_r16_i16(int r8_h, int permutation, char** buffer_ptr, test_State* before, test_State* after) {
    srand((uint32_t)(permutation + 42));

    uint16_t i16 = (rand() & 0xff00) | ((rand() >> 8) & 0xff);

    if (after == NULL) {
        before->mem[test_state_M(before) + 1] = i16 >> 8;
        before->mem[test_state_M(before) + 2] = i16 & 0xff;

        *buffer_ptr += sprintf(*buffer_ptr, "imm 0x%04x", i16);
        return permutation >= 0xff;
    };

    int pc_diff = test_state_M(after) - test_state_M(before);

    if (pc_diff != 3) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected pc to move %d step(s), actual moved %d step(s)\n", 3, pc_diff);
        return true;
    }

    uint8_t r8_h_value = after->mem[0xfff0 | r8_h];
    uint8_t r8_l_value = after->mem[0xfff0 | (r8_h + 1)];

    uint16_t r16_value = (uint16_t)(r8_h_value << 8 | r8_l_value);

    if (r16_value != i16) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected r16 (%d) to be 0x%04x, is 0x%04x\n", r8_h, i16, r16_value);
        return true;
    }

    return false;
}

static bool test_instruction_ld_r8_at_i16(int r8, int permutation, char** buffer_ptr, test_State* before, test_State* after) {
    srand((uint32_t)(permutation + 17));

    uint16_t i16 = (uint16_t)(rand() & 0xff00) | ((rand() >> 8) & 0xff);
    uint8_t at_i16 = rand() & 0xff;

    if (after == NULL) {
        before->mem[test_state_M(before) + 1] = i16 >> 8;
        before->mem[test_state_M(before) + 2] = i16 & 0xff;

        before->mem[i16] = at_i16;

        *buffer_ptr += sprintf(*buffer_ptr, "imm 0x%04x", i16);
        return permutation >= 0xff;
    }

    int pc_diff = test_state_M(after) - test_state_M(before);

    if (pc_diff != 3) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected pc to move %d step(s), actual moved %d step(s)\n", 3, pc_diff);
        return true;
    }

    uint8_t r8_value = after->mem[0xfff0 | r8];

    if (r8_value != at_i16) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected r8 (%d) to be 0x%02x, is 0x%02x\n", r8, at_i16, r8_value);
        return true;
    }

    return false;
}

static bool test_instruction_ld_r8_at_at_i16(int r8, int permutation, char** buffer_ptr, test_State* before, test_State* after) {
    srand((uint32_t)(permutation + 37));

    uint16_t i16 = (uint16_t)(rand() & 0xff00) | ((rand() >> 8) & 0xff);
    uint16_t at_i16 = (uint16_t)(rand() & 0xff00) | ((rand() >> 8) & 0xff);
    uint8_t i8_at_at_i16 = rand() & 0xff;

    if (after == NULL) {
        before->mem[test_state_M(before) + 1] = i16 >> 8;
        before->mem[test_state_M(before) + 2] = i16 & 0xff;

        before->mem[i16] = at_i16 >> 8;
        before->mem[i16 + 1] = at_i16 & 0xff;

        before->mem[at_i16] = i8_at_at_i16;

        *buffer_ptr += sprintf(*buffer_ptr, "imm 0x%04x", i16);
        return permutation >= 0xff;
    }

    int pc_diff = test_state_M(after) - test_state_M(before);

    if (pc_diff != 3) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected pc to move %d step(s), actual moved %d step(s)\n", 3, pc_diff);
        return true;
    }

    uint8_t r8_value = after->mem[0xfff0 | r8];

    if (r8_value != i8_at_at_i16) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected r8 (%d) to be 0x%02x, is 0x%02x\n", r8, i8_at_at_i16, r8_value);
        return true;
    }

    return false;
}

static bool test_instruction_ld_r8_at_r16(int r8_r16_h, int permutation, char** buffer_ptr, test_State* before, test_State* after) {
    srand((uint32_t)(permutation + 37));

    uint8_t r8 = (uint8_t)(r8_r16_h >> 4);
    uint8_t r16_h = r8_r16_h & 0xf;

    uint16_t i16 = (uint16_t)(rand() & 0xff00) | ((rand() >> 8) & 0xff);
    uint8_t at_i16 = rand() & 0xff;

    if (after == NULL) {
        before->mem[0xfff0 | r16_h] = i16 >> 8;
        before->mem[0xfff0 | (r16_h + 1)] = i16 & 0xff;

        before->mem[i16] = at_i16;

        *buffer_ptr += sprintf(*buffer_ptr, "i 0x%04x", i16);
        return permutation >= 0xff;
    }

    int pc_diff = test_state_M(after) - test_state_M(before);

    if (pc_diff != 1) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected pc to move %d step(s), actual moved %d step(s)\n", 1, pc_diff);
        return true;
    }

    uint8_t r8_value = after->mem[0xfff0 | r8];

    if (r8_value != at_i16) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected r8 (%d) to be 0x%02x, is 0x%02x\n", r8, at_i16, r8_value);
        return true;
    }

    return false;
}

static bool test_instruction_ld_r8_at_r16_inc(int r8_r16_h, int permutation, char** buffer_ptr, test_State* before, test_State* after) {
    srand((uint32_t)(permutation + 37));

    uint8_t r8 = (uint8_t)(r8_r16_h >> 4);
    uint8_t r16_h = r8_r16_h & 0xf;

    uint16_t i16 = (uint16_t)(rand() & 0xff00) | ((rand() >> 8) & 0xff);
    uint8_t at_i16 = rand() & 0xff;

    if (after == NULL) {
        before->mem[0xfff0 | r16_h] = i16 >> 8;
        before->mem[0xfff0 | (r16_h + 1)] = i16 & 0xff;

        before->mem[i16] = at_i16;

        *buffer_ptr += sprintf(*buffer_ptr, "i 0x%04x", i16);
        return permutation >= 0xff;
    }

    int pc_diff = test_state_M(after) - test_state_M(before);

    if (pc_diff != 1) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected pc to move %d step(s), actual moved %d step(s)\n", 1, pc_diff);
        return true;
    }

    uint8_t r8_value = after->mem[0xfff0 | r8];

    if (r8_value != at_i16) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected r8 (%d) to be 0x%02x, is 0x%02x\n", r8, at_i16, r8_value);
        return true;
    }

    uint16_t r16_before_value = (uint16_t)(before->mem[0xfff0 | r16_h] << 8) | before->mem[0xfff0 | (r16_h + 1)];

    uint16_t expected_r16_value = (uint16_t)(r16_before_value + 1);
    uint16_t actual_r16_value = (uint16_t)(after->mem[0xfff0 | r16_h] << 8) | after->mem[0xfff0 | (r16_h + 1)];

    if (expected_r16_value != actual_r16_value) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected r16 (%d) to be 0x%04x, is 0x%04x\n", r16_h, expected_r16_value, actual_r16_value);
        return true;
    }

    return false;
}

static bool test_instruction_ld_at_i16_r8(int r8, int permutation, char** buffer_ptr, test_State* before, test_State* after) {
    srand((uint32_t)(permutation + 17));

    uint16_t i16 = (uint16_t)(rand() & 0xff00) | ((rand() >> 8) & 0xff);
    uint8_t i8 = rand() & 0xff;

    if (after == NULL) {
        before->mem[test_state_M(before) + 1] = i16 >> 8;
        before->mem[test_state_M(before) + 2] = i16 & 0xff;

        before->mem[0xfff0 | r8] = i8;
        before->mem[i16] = (uint8_t)(i8 + 17);

        *buffer_ptr += sprintf(*buffer_ptr, "imm 0x%04x", i16);
        return permutation >= 0xff;
    }

    int pc_diff = test_state_M(after) - test_state_M(before);

    if (pc_diff != 3) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected pc to move %d step(s), actual moved %d step(s)\n", 3, pc_diff);
        return true;
    }

    uint8_t at_i16_value = after->mem[i16];

    if (at_i16_value != i8) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected [0x%04x] to be 0x%02x (%d), is 0x%02x\n", i16, i8, r8, at_i16_value);
        return true;
    }

    return false;
}

static bool test_instruction_add_r8_i8(int r8, int permutation, char** buffer_ptr, test_State* before, test_State* after) {
    uint8_t r8_value = (uint8_t)(permutation >> 8);
    uint8_t i8 = permutation & 0xff;

    if (after == NULL) {
        before->mem[test_state_M(before) + 1] = i8;
        before->mem[0xfff0 | r8] = r8_value;
        before->mem[0xfff0 | C_F] = 0;

        *buffer_ptr += sprintf(*buffer_ptr, "add 0x%02x (%d), imm 0x%02x", r8_value, r8, i8);
        return permutation >= 0xffff;
    };

    int pc_diff = test_state_M(after) - test_state_M(before);

    if (pc_diff != 2) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected pc to move %d step(s), actual moved %d step(s)\n", 2, pc_diff);
        return true;
    }

    uint8_t actual_r8_value = after->mem[0xfff0 | r8];
    uint8_t expected_r8_value = (uint8_t)(r8_value + i8);

    if (expected_r8_value != actual_r8_value) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected r8 (%d) to be 0x%02x, is 0x%02x\n", r8, expected_r8_value, actual_r8_value);
        return true;
    }

    uint8_t flags = after->mem[0xfff0 | C_F];

    if (r8_value + i8 >= 0x100 && !(flags & F_C)) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected cf\n");
        return true;
    } else if (r8_value + i8 < 0x100 && (flags & F_C)) {
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

static bool test_instruction_addc_r8_i8(int r8, int permutation, char** buffer_ptr, test_State* before, test_State* after) {
    uint8_t r8_value = (uint8_t)(permutation >> 8);
    uint8_t i8 = permutation & 0xff;
    uint8_t ingoing_cf = (uint8_t)(permutation >> 16);

    if (after == NULL) {
        before->mem[test_state_M(before) + 1] = i8;
        before->mem[0xfff0 | r8] = r8_value;

        uint8_t flags = ingoing_cf ? F_C : 0;

        before->mem[0xfff0 | C_F] = flags;

        *buffer_ptr += sprintf(*buffer_ptr, "addc 0x%02x (%d), imm 0x%02x, cf before %d", r8_value, r8, i8, ingoing_cf);
        return permutation >= 0x1ffff;
    };

    int pc_diff = test_state_M(after) - test_state_M(before);

    if (pc_diff != 2) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected pc to move %d step(s), actual moved %d step(s)\n", 2, pc_diff);
        return true;
    }

    uint8_t actual_r8_value = after->mem[0xfff0 | r8];
    uint8_t expected_r8_value = (uint8_t)(r8_value + i8 + ingoing_cf);

    if (expected_r8_value != actual_r8_value) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected r8 (%d) to be 0x%02x, is 0x%02x\n", r8, expected_r8_value, actual_r8_value);
        return true;
    }

    uint16_t q = r8_value + i8 + ingoing_cf;

    uint8_t flags = after->mem[0xfff0 | C_F];

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

static bool test_instruction_shl_r8(int r8, int permutation, char** buffer_ptr, test_State* before, test_State* after) {
    uint8_t r8_value = (uint8_t)permutation;

    if (after == NULL) {
        before->mem[0xfff0 | r8] = r8_value;

        *buffer_ptr += sprintf(*buffer_ptr, "shl 0x%02x (%d)", r8_value, r8);
        return permutation >= 0xff;
    };

    int pc_diff = test_state_M(after) - test_state_M(before);

    if (pc_diff != 1) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected pc to move %d step(s), actual moved %d step(s)\n", 1, pc_diff);
        return true;
    }

    uint8_t actual_r8_value = after->mem[0xfff0 | r8];
    uint8_t expected_r8_value = (uint8_t)(r8_value << 1);

    if (expected_r8_value != actual_r8_value) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected r8 (%d) to be 0x%02x, is 0x%02x\n", r8, expected_r8_value, actual_r8_value);
        return true;
    }

    uint8_t flags = after->mem[0xfff0 | C_F];

    uint8_t expected_cf = r8_value >> 7;
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

static bool test_instruction_shr_r8(int r8, int permutation, char** buffer_ptr, test_State* before, test_State* after) {
    uint8_t r8_value = (uint8_t)permutation;

    if (after == NULL) {
        before->mem[0xfff0 | r8] = r8_value;

        *buffer_ptr += sprintf(*buffer_ptr, "shr 0x%02x (%d)", r8_value, r8);
        return permutation >= 0xff;
    };

    int pc_diff = test_state_M(after) - test_state_M(before);

    if (pc_diff != 1) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected pc to move %d step(s), actual moved %d step(s)\n", 1, pc_diff);
        return true;
    }

    uint8_t actual_r8_value = after->mem[0xfff0 | r8];
    uint8_t expected_r8_value = (uint8_t)(r8_value >> 1);

    if (expected_r8_value != actual_r8_value) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected r8 (%d) to be 0x%02x, is 0x%02x\n", r8, expected_r8_value, actual_r8_value);
        return true;
    }

    uint8_t flags = after->mem[0xfff0 | C_F];

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

static bool test_instruction_jmp_i16(int data, int permutation, char** buffer_ptr, test_State* before, test_State* after) {
    srand((uint32_t)(permutation + 17));

    uint16_t i16 = (uint16_t)(rand() & 0xff00) | ((rand() >> 8) & 0xff);

    if (after == NULL) {
        before->mem[test_state_M(before) + 1] = i16 >> 8;
        before->mem[test_state_M(before) + 2] = i16 & 0xff;
        return permutation >= 0xff;
    }

    uint16_t actual_pc = test_state_M(after);

    if (actual_pc != i16) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected pc to be 0x%04x, is 0x%04x\n", i16, actual_pc);
        return true;
    }

    return false;
}

static bool test_instruction_jmp_r16(int r16_h, int permutation, char** buffer_ptr, test_State* before, test_State* after) {
    srand((uint32_t)(permutation + 19));

    uint16_t i16 = (uint16_t)(rand() & 0xff00) | ((rand() >> 8) & 0xff);

    if (after == NULL) {
        before->mem[0xfff0 | r16_h] = i16 >> 8;
        before->mem[0xfff0 | (r16_h + 1)] = i16 & 0xff;
        return permutation >= 0xff;
    }

    uint16_t actual_pc = test_state_M(after);

    if (actual_pc != i16) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected pc to be 0x%04x, is 0x%04x\n", i16, actual_pc);
        return true;
    }

    return false;
}

static bool test_instruction_jmp_i16_conditional(int flags_0_1, int permutation, char** buffer_ptr, test_State* before, test_State* after) {
    srand((uint32_t)(permutation + 17));

    uint16_t i16 = (uint16_t)(rand() & 0xff00) | ((rand() >> 8) & 0xff);

    uint8_t flags = permutation ? (flags_0_1 & 0xf) : ((uint8_t)flags_0_1 >> 4);

    if (after == NULL) {
        before->mem[test_state_M(before) + 1] = i16 >> 8;
        before->mem[test_state_M(before) + 2] = i16 & 0xff;

        before->mem[0xfff0 | C_F] = flags;

        return permutation >= 1;
    }

    uint16_t actual_pc = test_state_M(after);

    if (permutation) {
        if (actual_pc != i16) {
            *buffer_ptr += sprintf(*buffer_ptr, "expected pc to be 0x%04x, is 0x%04x\n", i16, actual_pc);
            return true;
        }
    } else {
        int pc_diff = test_state_M(after) - test_state_M(before);

        if (pc_diff != 3) {
            *buffer_ptr += sprintf(*buffer_ptr, "expected pc to move %d step(s), actual moved %d step(s)\n", 3, pc_diff);
            return true;
        }
    }

    return false;
}

static bool test_instruction_jal_r16_i16(int r16_h, int permutation, char** buffer_ptr, test_State* before, test_State* after) {
    srand((uint32_t)(permutation + 33));

    uint16_t i16 = (uint16_t)(rand() & 0xff00) | ((rand() >> 8) & 0xff);

    if (after == NULL) {
        before->mh = (uint8_t)permutation;
        before->ml = 0xcd;
        before->mem[test_state_M(before) + 1] = i16 >> 8;
        before->mem[test_state_M(before) + 2] = i16 & 0xff;
        return permutation >= 0xff;
    }

    uint16_t actual_pc = test_state_M(after);

    if (actual_pc != i16) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected pc to be 0x%04x, is 0x%04x\n", i16, actual_pc);
        return true;
    }

    uint16_t actual_r16 = (uint16_t)(after->mem[0xfff0 | r16_h] << 8) | after->mem[0xfff0 | (r16_h + 1)];
    uint16_t expected_r16 = (uint16_t)(((before->mh << 8) | before->ml) + 3);

    if (actual_r16 != expected_r16) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected r16 (%d) to be 0x%04x, is 0x%04x\n", r16_h, expected_r16, actual_r16);
        return true;
    }

    return false;
}

static bool test_instruction_push_r8(int r8, int permutation, char** buffer_ptr, test_State* before, test_State* after) {
    if (after == NULL) {
        before->mem[0xfff0 | C_SP] = (uint8_t)permutation + 1;
        before->mem[0xfff0 | r8] = (uint8_t)permutation;
        return permutation >= 0xf0;
    }

    int pc_diff = test_state_M(after) - test_state_M(before);

    if (pc_diff != 1) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected pc to move %d step(s), actual moved %d step(s)\n", 1, pc_diff);
        return true;
    }

    int sp_diff = after->mem[0xfff0 | C_SP] - before->mem[0xfff0 | C_SP];

    if (sp_diff != 1) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected sp to move %d step(s), actual moved %d step(s)\n", 1, sp_diff);
        return true;
    }

    uint8_t expected_pushed_i8 = before->mem[0xfff0 | r8];
    uint8_t actual_pushed_i8 = after->mem[0xff00 | before->mem[0xfff0 | C_SP]];

    if (actual_pushed_i8 != expected_pushed_i8) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected 0x%02x to be pushed onto stack, is 0x%02x\n", expected_pushed_i8, actual_pushed_i8);
        return true;
    }

    return false;
}

static bool test_instruction_push_r16(int r16_h, int permutation, char** buffer_ptr, test_State* before, test_State* after) {
    if (after == NULL) {
        srand((uint32_t)(permutation + 37));

        before->mem[0xfff0 | C_SP] = (uint8_t)(permutation * 2);
        before->mem[0xfff0 | r16_h] = (uint8_t)rand();
        before->mem[0xfff0 | (r16_h + 1)] = (uint8_t)rand();
        return permutation >= (0xf0 / 2);
    }

    int pc_diff = test_state_M(after) - test_state_M(before);

    if (pc_diff != 1) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected pc to move %d step(s), actual moved %d step(s)\n", 1, pc_diff);
        return true;
    }

    int sp_diff = after->mem[0xfff0 | C_SP] - before->mem[0xfff0 | C_SP];

    if (sp_diff != 2) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected sp to move %d step(s), actual moved %d step(s)\n", 2, sp_diff);
        return true;
    }

    uint8_t expected_pushed_i8_h = before->mem[0xfff0 | r16_h];
    uint8_t expected_pushed_i8_l = before->mem[0xfff0 | (r16_h + 1)];

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

static bool test_instruction_pop_r8(int r8, int permutation, char** buffer_ptr, test_State* before, test_State* after) {
    uint8_t i8 = (uint8_t)permutation;

    if (after == NULL) {
        before->mem[0xfff0 | r8] = 0x00;
        before->mem[0xfff0 | C_SP] = (uint8_t)permutation + 1;
        before->mem[0xff00 | (before->mem[0xfff0 | C_SP] - 1)] = i8;

        return permutation >= 0xef;
    }

    int pc_diff = test_state_M(after) - test_state_M(before);

    if (pc_diff != 1) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected pc to move %d step(s), actual moved %d step(s)\n", 1, pc_diff);
        return true;
    }

    int sp_diff = after->mem[0xfff0 | C_SP] - before->mem[0xfff0 | C_SP];

    if (sp_diff != -1) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected sp to move %d step(s), actual moved %d step(s)\n", -1, sp_diff);
        return true;
    }

    uint8_t expected_popped_i8 = i8;
    uint8_t actual_popped_i8 = after->mem[0xfff0 | r8];

    if (actual_popped_i8 != expected_popped_i8) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected 0x%02x to be popped from the stack, was 0x%02x\n", expected_popped_i8, actual_popped_i8);
        return true;
    }

    return false;
}

static bool test_instruction_pop_r16(int r16_h, int permutation, char** buffer_ptr, test_State* before, test_State* after) {
    uint16_t i16 = (uint16_t)(0xab00 | permutation);

    if (after == NULL) {
        before->mem[0xfff0 | r16_h] = 0x00;
        before->mem[0xfff0 | (r16_h + 1)] = 0x00;

        before->mem[0xfff0 | C_SP] = (uint8_t)permutation + 2;
        before->mem[0xff00 | before->mem[0xfff0 | C_SP] - 2] = i16 >> 8;
        before->mem[0xff00 | before->mem[0xfff0 | C_SP] - 1] = i16 & 0xff;

        return permutation >= 0x78;
    }

    int pc_diff = test_state_M(after) - test_state_M(before);

    if (pc_diff != 1) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected pc to move %d step(s), actual moved %d step(s)\n", 1, pc_diff);
        return true;
    }

    int sp_diff = after->mem[0xfff0 | C_SP] - before->mem[0xfff0 | C_SP];

    if (sp_diff != -2) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected sp to move %d step(s), actual moved %d step(s)\n", -2, sp_diff);
        return true;
    }

    uint16_t expected_popped_i16 = i16;
    uint16_t actual_popped_i16 =
        (uint16_t)(after->mem[0xfff0 | r16_h] << 8) | (after->mem[0xfff0 | (r16_h + 1)]);

    if (actual_popped_i16 != expected_popped_i16) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected 0x%04x to be popped from the stack, was 0x%04x\n", expected_popped_i16, actual_popped_i16);
        return true;
    }

    return false;
}

static bool test_instruction_ld_at_r16_inc_r8(int data, int permutation, char** buffer_ptr, test_State* before, test_State* after) {
    uint8_t r16_h = (uint8_t)data >> 4;
    uint8_t r8 = data & 0xf;

    if (after == NULL) {
        uint16_t i16 = rand() & 0xfff0;

        before->mem[0xfff0 | r8] = (uint8_t)permutation;
        before->mem[0xfff0 | r16_h] = i16 >> 8;
        before->mem[0xfff0 | (r16_h + 1)] = i16 & 0xff;

        return permutation >= 0xff;
    }

    int pc_diff = test_state_M(after) - test_state_M(before);

    if (pc_diff != 1) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected pc to move %d step(s), actual moved %d step(s)\n", 1, pc_diff);
        return true;
    }

    uint16_t before_r16 = (uint16_t)(before->mem[0xfff0 | r16_h] << 8) | before->mem[0xfff0 | (r16_h + 1)];

    uint8_t expected_stored_r8 = before->mem[0xfff0 | r8];
    uint8_t actual_stored_r8 = after->mem[before_r16];

    if (expected_stored_r8 != actual_stored_r8) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected 0x%02x to be stored, is 0x%02x\n", expected_stored_r8, actual_stored_r8);
        return true;
    }

    uint16_t expected_r16 = before_r16 + 1;

    uint16_t actual_r16 = (uint16_t)(after->mem[0xfff0 | r16_h] << 8) | after->mem[0xfff0 | (r16_h + 1)];

    if (expected_r16 != actual_r16) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected r16 (%d) to be 0x%04x, is 0x%04x\n", r16_h, expected_r16, actual_r16);
        return true;
    }

    return false;
}

static test_Test_case test_cases[] = {
    { .i = NOP, .name = MNEMONIC_NOP, .test = test_instruction_nop, .data = 0 },

    { .i = LD_A_I8, .name = MNEMONIC_LD_A_I8, .test = test_instruction_ld_r8_i8, .data = C_A },
    { .i = LD_B_I8, .name = MNEMONIC_LD_B_I8, .test = test_instruction_ld_r8_i8, .data = C_B },
    { .i = LD_C_I8, .name = MNEMONIC_LD_C_I8, .test = test_instruction_ld_r8_i8, .data = C_C },
    { .i = LD_D_I8, .name = MNEMONIC_LD_D_I8, .test = test_instruction_ld_r8_i8, .data = C_D },

    { .i = LD_A_B, .name = MNEMONIC_LD_A_B, .test = test_instruction_ld_r8_r8, .data = C_A << 4 | C_B },
    { .i = LD_A_C, .name = MNEMONIC_LD_A_C, .test = test_instruction_ld_r8_r8, .data = C_A << 4 | C_C },
    { .i = LD_A_D, .name = MNEMONIC_LD_A_D, .test = test_instruction_ld_r8_r8, .data = C_A << 4 | C_D },

    { .i = LD_B_A, .name = MNEMONIC_LD_B_A, .test = test_instruction_ld_r8_r8, .data = C_B << 4 | C_A },
    { .i = LD_B_C, .name = MNEMONIC_LD_B_C, .test = test_instruction_ld_r8_r8, .data = C_B << 4 | C_C },
    { .i = LD_B_D, .name = MNEMONIC_LD_B_D, .test = test_instruction_ld_r8_r8, .data = C_B << 4 | C_D },

    { .i = LD_C_A, .name = MNEMONIC_LD_C_A, .test = test_instruction_ld_r8_r8, .data = C_C << 4 | C_A },
    { .i = LD_C_B, .name = MNEMONIC_LD_C_B, .test = test_instruction_ld_r8_r8, .data = C_C << 4 | C_B },
    { .i = LD_C_D, .name = MNEMONIC_LD_C_D, .test = test_instruction_ld_r8_r8, .data = C_C << 4 | C_D },

    { .i = LD_D_A, .name = MNEMONIC_LD_D_A, .test = test_instruction_ld_r8_r8, .data = C_D << 4 | C_A },
    { .i = LD_D_B, .name = MNEMONIC_LD_D_B, .test = test_instruction_ld_r8_r8, .data = C_D << 4 | C_B },
    { .i = LD_D_C, .name = MNEMONIC_LD_D_C, .test = test_instruction_ld_r8_r8, .data = C_D << 4 | C_C },

    { .i = LD_I_I16, .name = MNEMONIC_LD_I_I16, .test = test_instruction_ld_r16_i16, .data = C_IH },
    { .i = LD_J_I16, .name = MNEMONIC_LD_J_I16, .test = test_instruction_ld_r16_i16, .data = C_JH },
    { .i = LD_K_I16, .name = MNEMONIC_LD_K_I16, .test = test_instruction_ld_r16_i16, .data = C_KH },

    { .i = LD_A_AT_I16, .name = MNEMONIC_LD_A_AT_I16, .test = test_instruction_ld_r8_at_i16, .data = C_A },
    { .i = LD_B_AT_I16, .name = MNEMONIC_LD_B_AT_I16, .test = test_instruction_ld_r8_at_i16, .data = C_B },
    { .i = LD_C_AT_I16, .name = MNEMONIC_LD_C_AT_I16, .test = test_instruction_ld_r8_at_i16, .data = C_C },
    { .i = LD_D_AT_I16, .name = MNEMONIC_LD_D_AT_I16, .test = test_instruction_ld_r8_at_i16, .data = C_D },

    { .i = LD_A_AT_AT_I16, .name = MNEMONIC_LD_A_AT_AT_I16, .test = test_instruction_ld_r8_at_at_i16, .data = C_A },
    { .i = LD_B_AT_AT_I16, .name = MNEMONIC_LD_B_AT_AT_I16, .test = test_instruction_ld_r8_at_at_i16, .data = C_B },
    { .i = LD_C_AT_AT_I16, .name = MNEMONIC_LD_C_AT_AT_I16, .test = test_instruction_ld_r8_at_at_i16, .data = C_C },
    { .i = LD_D_AT_AT_I16, .name = MNEMONIC_LD_D_AT_AT_I16, .test = test_instruction_ld_r8_at_at_i16, .data = C_D },

    { .i = LD_A_AT_I, .name = MNEMONIC_LD_A_AT_I, .test = test_instruction_ld_r8_at_r16, .data = C_A << 4 | C_IH },
    { .i = LD_B_AT_I, .name = MNEMONIC_LD_B_AT_I, .test = test_instruction_ld_r8_at_r16, .data = C_B << 4 | C_IH },
    { .i = LD_C_AT_I, .name = MNEMONIC_LD_C_AT_I, .test = test_instruction_ld_r8_at_r16, .data = C_C << 4 | C_IH },
    { .i = LD_D_AT_I, .name = MNEMONIC_LD_D_AT_I, .test = test_instruction_ld_r8_at_r16, .data = C_D << 4 | C_IH },

    { .i = LD_A_AT_J, .name = MNEMONIC_LD_A_AT_J, .test = test_instruction_ld_r8_at_r16, .data = C_A << 4 | C_JH },
    { .i = LD_B_AT_J, .name = MNEMONIC_LD_B_AT_J, .test = test_instruction_ld_r8_at_r16, .data = C_B << 4 | C_JH },
    { .i = LD_C_AT_J, .name = MNEMONIC_LD_C_AT_J, .test = test_instruction_ld_r8_at_r16, .data = C_C << 4 | C_JH },
    { .i = LD_D_AT_J, .name = MNEMONIC_LD_D_AT_J, .test = test_instruction_ld_r8_at_r16, .data = C_D << 4 | C_JH },

    { .i = LD_A_AT_K, .name = MNEMONIC_LD_A_AT_K, .test = test_instruction_ld_r8_at_r16, .data = C_A << 4 | C_KH },
    { .i = LD_B_AT_K, .name = MNEMONIC_LD_B_AT_K, .test = test_instruction_ld_r8_at_r16, .data = C_B << 4 | C_KH },
    { .i = LD_C_AT_K, .name = MNEMONIC_LD_C_AT_K, .test = test_instruction_ld_r8_at_r16, .data = C_C << 4 | C_KH },
    { .i = LD_D_AT_K, .name = MNEMONIC_LD_D_AT_K, .test = test_instruction_ld_r8_at_r16, .data = C_D << 4 | C_KH },

    { .i = LD_A_AT_I_INC, .name = MNEMONIC_LD_A_AT_I_INC, .test = test_instruction_ld_r8_at_r16_inc, .data = C_A << 4 | C_IH },
    { .i = LD_B_AT_I_INC, .name = MNEMONIC_LD_B_AT_I_INC, .test = test_instruction_ld_r8_at_r16_inc, .data = C_B << 4 | C_IH },
    { .i = LD_C_AT_I_INC, .name = MNEMONIC_LD_C_AT_I_INC, .test = test_instruction_ld_r8_at_r16_inc, .data = C_C << 4 | C_IH },
    { .i = LD_D_AT_I_INC, .name = MNEMONIC_LD_D_AT_I_INC, .test = test_instruction_ld_r8_at_r16_inc, .data = C_D << 4 | C_IH },

    { .i = LD_A_AT_J_INC, .name = MNEMONIC_LD_A_AT_J_INC, .test = test_instruction_ld_r8_at_r16_inc, .data = C_A << 4 | C_JH },
    { .i = LD_B_AT_J_INC, .name = MNEMONIC_LD_B_AT_J_INC, .test = test_instruction_ld_r8_at_r16_inc, .data = C_B << 4 | C_JH },
    { .i = LD_C_AT_J_INC, .name = MNEMONIC_LD_C_AT_J_INC, .test = test_instruction_ld_r8_at_r16_inc, .data = C_C << 4 | C_JH },
    { .i = LD_D_AT_J_INC, .name = MNEMONIC_LD_D_AT_J_INC, .test = test_instruction_ld_r8_at_r16_inc, .data = C_D << 4 | C_JH },

    { .i = LD_A_AT_K_INC, .name = MNEMONIC_LD_A_AT_K_INC, .test = test_instruction_ld_r8_at_r16_inc, .data = C_A << 4 | C_KH },
    { .i = LD_B_AT_K_INC, .name = MNEMONIC_LD_B_AT_K_INC, .test = test_instruction_ld_r8_at_r16_inc, .data = C_B << 4 | C_KH },
    { .i = LD_C_AT_K_INC, .name = MNEMONIC_LD_C_AT_K_INC, .test = test_instruction_ld_r8_at_r16_inc, .data = C_C << 4 | C_KH },
    { .i = LD_D_AT_K_INC, .name = MNEMONIC_LD_D_AT_K_INC, .test = test_instruction_ld_r8_at_r16_inc, .data = C_D << 4 | C_KH },

    { .i = LD_AT_I16_A, .name = MNEMONIC_LD_AT_I16_A, .test = test_instruction_ld_at_i16_r8, .data = C_A },
    { .i = LD_AT_I16_B, .name = MNEMONIC_LD_AT_I16_B, .test = test_instruction_ld_at_i16_r8, .data = C_B },
    { .i = LD_AT_I16_C, .name = MNEMONIC_LD_AT_I16_C, .test = test_instruction_ld_at_i16_r8, .data = C_C },
    { .i = LD_AT_I16_D, .name = MNEMONIC_LD_AT_I16_D, .test = test_instruction_ld_at_i16_r8, .data = C_D },

    { .i = LD_AT_I_INC_A, .name = MNEMONIC_LD_AT_I_INC_A, .test = test_instruction_ld_at_r16_inc_r8, .data = C_IH << 4 | C_A },

    { .i = SHL_A, .name = MNEMONIC_SHL_A, .test = test_instruction_shl_r8, .data = C_A },

    { .i = SHR_A, .name = MNEMONIC_SHR_A, .test = test_instruction_shr_r8, .data = C_A },

    { .i = ADD_A_I8, .name = MNEMONIC_ADD_A_I8, .test = test_instruction_add_r8_i8, .data = C_A },

    { .i = ADDC_A_I8, .name = MNEMONIC_ADDC_A_I8, .test = test_instruction_addc_r8_i8, .data = C_A },

    { .i = JMP_I16, .name = MNEMONIC_JMP_I16, .test = test_instruction_jmp_i16, .data = 0 },

    { .i = JMP_K, .name = MNEMONIC_JMP_K, .test = test_instruction_jmp_r16, .data = C_KH },

    { .i = JZ_I16,  .name = MNEMONIC_JZ_I16,  .test = test_instruction_jmp_i16_conditional, .data =   0 << 4 | F_Z },
    { .i = JNZ_I16, .name = MNEMONIC_JNZ_I16, .test = test_instruction_jmp_i16_conditional, .data = F_Z << 4 | 0 },
    { .i = JC_I16,  .name = MNEMONIC_JC_I16,  .test = test_instruction_jmp_i16_conditional, .data =   0 << 4 | F_C },
    { .i = JNC_I16, .name = MNEMONIC_JNC_I16, .test = test_instruction_jmp_i16_conditional, .data = F_C << 4 | 0 },

    { .i = JAL_K_I16, .name = MNEMONIC_JAL_K_I16, .test = test_instruction_jal_r16_i16, .data = C_KH },

    { .i = PUSH_A, .name = MNEMONIC_PUSH_A, .test = test_instruction_push_r8, .data = C_A },

    { .i = PUSH_K, .name = MNEMONIC_PUSH_K, .test = test_instruction_push_r16, .data = C_KH },

    { .i = POP_A, .name = MNEMONIC_POP_A, .test = test_instruction_pop_r8, .data = C_A },

    { .i = POP_K, .name = MNEMONIC_POP_K, .test = test_instruction_pop_r16, .data = C_KH },
};

static int test_step(uint8_t rom_alu[ROM_SIZE_ALU], uint8_t rom_instruction1[ROM_SIZE_INSTRUCTION], uint8_t rom_instruction2[ROM_SIZE_INSTRUCTION], char** buffer_ptr, test_State* state) {
    uint32_t index = (uint32_t)(
        (((state->mh >> 5) & 1) << 16) |
        ((state->tf & 0xf)      << 12) |
         (state->s              << 8)  |
          state->i);

    uint16_t signals = (uint16_t)((rom_instruction2[index] << 8) | rom_instruction1[index]) ^ SIGNALS_ACTIVE_LOW_MASK;

    char* buffer = *buffer_ptr;

    buffer += sprintf(buffer, "\n    0x%02x: index: 0x%05x signals: 0x%04x\n", state->s, index, signals);

    if (state->i != RESET && state->s == 0 && signals != S0_FETCH) {
        buffer += sprintf(buffer, "        expected signals at s 0 to be S0_FETCH\n");
        return 2;
    }

    bool use_c_address = test_state_M_or_C(state);

    uint8_t bus = 0;
    uint16_t mem_address = (uint16_t)(use_c_address ? (0xfff0 | (state->c & 0xf)) : test_state_M(state));

    if (signals & OE_MEM) {
        bus = state->mem[mem_address];

        buffer += sprintf(buffer, "        OE_MEM: bus = 0x%02x from mem[0x%04x] (%c)\n", bus, mem_address, test_state_M_or_C(state) ? 'C' : 'M');
    } else if (signals & OE_T) {
        bus = state->t;

        buffer += sprintf(buffer, "        OE_T: bus = 0x%02x\n", bus);
    } else if (signals & OE_ALU) {
        uint32_t alu_index = (uint32_t)(
            ((state->c & 0x7) << 16) |
             (state->mh       << 8)  |
              state->ml
            );

        bus = rom_alu[alu_index];

        buffer += sprintf(buffer, "        OE_ALU: bus = 0x%02x from alu[0x%05x]\n", bus, alu_index);
    } else {
        buffer +=  sprintf(buffer, "        nothing asserted on the bus\n");
        return 2;
    };

    if (signals & LD_I) {
        buffer +=  sprintf(buffer, "        LD_I\n");

        state->i = bus;
    }

    if (signals & LD_T) {
        buffer += sprintf(buffer, "        LD_T\n");

        state->t = bus;
    }

    if (signals & LD_MEM) {
        buffer += sprintf(buffer, "        LD_MEM to mem[0x%04x] (%c)\n", mem_address, test_state_M_or_C(state) ? 'C' : 'M');

        state->mem[mem_address] = bus;
    }

    if (signals & LD_C) {
        buffer += sprintf(buffer, "        LD_C: sel C=0x%x sel M/C=%d\n", (signals >> 11) & 0xf, signals >> 11 >> 4);

        state->c = signals >> 11;
    } else {
        if (signals & SEL_C_LD_TF) {
            buffer += sprintf(buffer, "        LD_TF\n");

            state->tf = bus & 0xf;
        }
    }

    if (signals & LD_ML) {
        buffer += sprintf(buffer, "        LD_ML\n");

        state->ml = bus;
    }

    if (signals & LD_MH) {
        buffer += sprintf(buffer, "        LD_MH\n");

        state->mh = bus;
    }

    if (signals & INC_M) {
        buffer += sprintf(buffer, "        INC_M: mh=0x%02x ml=0x%02x\n", state->mh, state->ml);

        if (!(signals & LD_ML)) {
            state->ml = (uint8_t)(state->ml + 1);

            if (state->ml == 0) {
                if (!(signals & LD_MH)) {
                    state->mh = (uint8_t)(state->mh + 1);
                }
            }
        }
     }

     if (signals & LD_S) {
        buffer += sprintf(buffer, "        LD_S\n");

        state->s = 0;
     } else {
         ++state->s;
         if (state->s == 0x10) state->s = 0;
     }

     *buffer_ptr = buffer;

     return state->s == 0 ? 1 : 0;
}

static int test_instructions(uint8_t rom_alu[ROM_SIZE_ALU], uint8_t rom_instruction1[ROM_SIZE_INSTRUCTION], uint8_t rom_instruction2[ROM_SIZE_INSTRUCTION]) {
    char buffer[0x2000];
    char* buffer_ptr = buffer;

    test_State state = {0};

    state.mem[0xfff0 | C_FF] = 0xab;
    state.mem[0xfff0 | C_SP] = 0xba;

    int steps = 0;

    state.i = RESET;

    for (;; ++steps) {
        buffer[0] = '\0';
        buffer_ptr = buffer;

        int error = test_step(rom_alu, rom_instruction1, rom_instruction2, &buffer_ptr, &state);

        if (error > 1) {
            fprintf(stderr, "reset failed: %s\n", buffer);
            return 1;
        }

        if (state.i != RESET && error == 1) break;

        if (state.mh >= 0x20 && state.ml >= 0x02) {
            fprintf(stderr, "reset failed, M too big 0x%02x%02x\n", state.mh, state.ml);
            return 1;
        }
    }

    printf("reset complete after %d steps\n", steps);

    int n_failed_tests = 0;
    int n_test_cases = sizeof(test_cases) / sizeof(test_cases[0]);

    for (int i = n_test_cases - 1; i >= 0; --i) {
        for (int permutation = 0;; ++permutation) {
            buffer[0] = '\0';
            buffer_ptr = buffer;

            if (state.mh == 0xff) state.mh = 0; // prevent overwriting mem registers

            test_Test_case test_case = test_cases[i];

            state.i = (uint8_t)test_case.i; // To prevent getting stuck in reset instruction

            bool last_permutation = test_case.test(test_case.data, permutation, &buffer_ptr, &state, NULL);

            buffer_ptr += sprintf(buffer_ptr, "\n");

            state.mem[test_state_M(&state)] = (uint8_t)test_case.i; // The value loaded into I on first step

            test_State orignal_state = state;

            bool test_failed = false;

            for (steps = 0; steps < 0x100; ++steps) {
                int error = test_step(rom_alu, rom_instruction1, rom_instruction2, &buffer_ptr, &state);

                if (error > 1) {
                    test_failed = true;
                    break;
                }

                if (error > 0) break;
            }

            if (!test_failed) {
                buffer_ptr += sprintf(buffer_ptr, "\nsteps taken: %d\n\n", steps + 1);

                if (steps > 0xf) {
                    buffer_ptr += sprintf(buffer_ptr, "instruction never finished\n");
                    test_failed = true;
                } else if (test_state_M_or_C(&state)) {
                    buffer_ptr += sprintf(buffer_ptr, "instruction never restored mem address to M\n");
                    test_failed = true;
                } else {
                    test_failed = test_case.test(test_case.data, permutation, &buffer_ptr, &orignal_state, &state);
                }
            }

            if (test_failed) {
                fprintf(stderr, "%s - permutation %d failed: %s\n", test_case.name, permutation, buffer);
                ++n_failed_tests;
                break;
            } else if (last_permutation) {
                printf("%s - passed (%d times)\n", test_case.name, permutation + 1);
                break;
            }
        }
    }

    return n_failed_tests;
}
