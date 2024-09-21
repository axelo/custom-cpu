typedef struct {
    uint8_t o;
    uint8_t s; // 4 bit
    uint8_t ml;
    uint8_t mh;
    uint8_t c;
    uint8_t t;
    uint8_t mem[0x10000];
} test_opcodes_State;

typedef struct {
    O o;
    const char* name;
    int data;
    bool (*test)(int, int, char*, test_opcodes_State*, test_opcodes_State*);
} test_opcodes_Test_case;

#define test_state_M(state) ((uint16_t)(((state)->mh << 8) | (state)->ml))
#define test_state_M_or_C(state) ((state)->c & 0x10)

static bool test_opcodes_nop(int data, int permutation, char *buffer, test_opcodes_State *before, test_opcodes_State *after) {
    if (after == NULL) return true;

    int pc_diff = test_state_M(after) - test_state_M(before);

    if (pc_diff != 1) {
        sprintf(buffer + strlen(buffer), "expected pc to move %d step(s), actual moved %d step(s)\n", 1, pc_diff);
        return true;
    }

    return false;
}

static bool test_opcodes_ld_r8_i8(int r8, int permutation, char *buffer, test_opcodes_State *before, test_opcodes_State *after) {
    uint8_t i8 = permutation & 0xff;

    if (after == NULL) {
        before->mem[test_state_M(before) + 1] = i8;
        sprintf(buffer + strlen(buffer), "imm 0x%02x", i8);
        return permutation >= 0xff;
    };

    int pc_diff = test_state_M(after) - test_state_M(before);

    if (pc_diff != 2) {
        sprintf(buffer + strlen(buffer), "expected pc to move %d step(s), actual moved %d step(s)\n", 2, pc_diff);
        return true;
    }

    uint8_t r8_value = after->mem[0xfff0 | r8];

    if (r8_value != i8) {
        sprintf(buffer + strlen(buffer), "expected r8 to be 0x%02x, is 0x%02x\n", i8, r8_value);
        return true;
    }

    return false;
}

static bool test_opcodes_ld_r8_r8(int r8_r8, int permutation, char *buffer, test_opcodes_State *before, test_opcodes_State *after) {
    uint8_t dest_r8 = r8_r8 >> 4 & 0xf;
    uint8_t src_r8 = r8_r8 & 0xf;

    if (after == NULL) {
        before->mem[0xfff0 | src_r8] = (uint8_t) permutation;

        sprintf(buffer + strlen(buffer), "ld r%d, r%d (0x%02x)", dest_r8, src_r8, permutation);
        return permutation >= 0xff;
    };

    int pc_diff = test_state_M(after) - test_state_M(before);

    if (pc_diff != 1) {
        sprintf(buffer + strlen(buffer), "expected pc to move %d step(s), actual moved %d step(s)\n", 1, pc_diff);
        return true;
    }

    uint8_t dest_r8_value = after->mem[0xfff0 | dest_r8];

    if (dest_r8_value != permutation) {
        sprintf(buffer + strlen(buffer), "expected r%d to be 0x%02x, is 0x%02x\n", dest_r8, permutation, dest_r8_value);
        return true;
    }

    uint8_t src_r8_value = after->mem[0xfff0 | src_r8];

    if (src_r8_value != permutation) {
        sprintf(buffer + strlen(buffer), "expected r%d to be 0x%02x, is 0x%02x\n", src_r8, permutation, src_r8_value);
        return true;
    }

    return false;
}

static bool test_opcodes_ld_r16_i16(int r8_h, int permutation, char *buffer, test_opcodes_State *before, test_opcodes_State *after) {
    srand((uint32_t)(permutation + 42));

    uint16_t i16 = (rand() & 0xff00) | ((rand() >> 8) & 0xff);

    if (after == NULL) {
        before->mem[test_state_M(before) + 1] = i16 >> 8;
        before->mem[test_state_M(before) + 2] = i16 & 0xff;
        sprintf(buffer + strlen(buffer), "imm 0x%04x", i16);
        return permutation >= 0xff;
    };

    int pc_diff = test_state_M(after) - test_state_M(before);

    if (pc_diff != 3) {
        sprintf(buffer + strlen(buffer), "expected pc to move %d step(s), actual moved %d step(s)\n", 3, pc_diff);
        return true;
    }

    uint8_t r8_h_value = after->mem[0xfff0 | r8_h];
    uint8_t r8_l_value = after->mem[0xfff0 | (r8_h + 1)];

    uint16_t r16_value = (uint16_t)(r8_h_value << 8 | r8_l_value);

    if (r16_value != i16) {
        sprintf(buffer + strlen(buffer), "expected r16 (%d) to be 0x%04x, is 0x%04x\n", r8_h, i16, r16_value);
        return true;
    }

    return false;
}

static bool test_opcodes_ld_r8_at_i16(int r8, int permutation, char *buffer, test_opcodes_State *before, test_opcodes_State *after) {
    srand((uint32_t)(permutation + 17));

    uint16_t i16 = (uint16_t)(rand() & 0xff00) | ((rand() >> 8) & 0xff);
    uint8_t at_i16 = rand() & 0xff;

    if (after == NULL) {
        before->mem[test_state_M(before) + 1] = i16 >> 8;
        before->mem[test_state_M(before) + 2] = i16 & 0xff;

        before->mem[i16] = at_i16;

        sprintf(buffer + strlen(buffer), "imm 0x%04x", i16);
        return permutation >= 0xff;
    }

    int pc_diff = test_state_M(after) - test_state_M(before);

    if (pc_diff != 3) {
        sprintf(buffer + strlen(buffer), "expected pc to move %d step(s), actual moved %d step(s)\n", 3, pc_diff);
        return true;
    }

    uint8_t r8_value = after->mem[0xfff0 | r8];

    if (r8_value != at_i16) {
        sprintf(buffer + strlen(buffer), "expected r8 (%d) to be 0x%02x, is 0x%02x\n", r8, at_i16, r8_value);
        return true;
    }

    return false;
}

static bool test_opcodes_ld_r8_at_at_i16(int r8, int permutation, char *buffer, test_opcodes_State *before, test_opcodes_State *after) {
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

        sprintf(buffer + strlen(buffer), "imm 0x%04x", i16);
        return permutation >= 0xff;
    }

    int pc_diff = test_state_M(after) - test_state_M(before);

    if (pc_diff != 3) {
        sprintf(buffer + strlen(buffer), "expected pc to move %d step(s), actual moved %d step(s)\n", 3, pc_diff);
        return true;
    }

    uint8_t r8_value = after->mem[0xfff0 | r8];

    if (r8_value != i8_at_at_i16) {
        sprintf(buffer + strlen(buffer), "expected r8 (%d) to be 0x%02x, is 0x%02x\n", r8, i8_at_at_i16, r8_value);
        return true;
    }

    return false;
}

static bool test_opcodes_ld_r8_at_r16(int r8_r16_h, int permutation, char *buffer, test_opcodes_State *before, test_opcodes_State *after) {
    srand((uint32_t)(permutation + 19));

    uint8_t r8 = (uint8_t)(r8_r16_h >> 4);
    uint8_t r16_h = r8_r16_h & 0xf;

    uint16_t i16 = (uint16_t)(rand() & 0xff00) | ((rand() >> 8) & 0xff);
    uint8_t at_i16 = rand() & 0xff;

    if (after == NULL) {
        before->mem[0xfff0 | r16_h] = i16 >> 8;
        before->mem[0xfff0 | (r16_h + 1)] = i16 & 0xff;

        before->mem[i16] = at_i16;

        sprintf(buffer + strlen(buffer), "i 0x%04x", i16);
        return permutation >= 0xff;
    }

    int pc_diff = test_state_M(after) - test_state_M(before);

    if (pc_diff != 1) {
        sprintf(buffer + strlen(buffer), "expected pc to move %d step(s), actual moved %d step(s)\n", 1, pc_diff);
        return true;
    }

    uint8_t r8_value = after->mem[0xfff0 | r8];

    if (r8_value != at_i16) {
        sprintf(buffer + strlen(buffer), "expected r8 (%d) to be 0x%02x, is 0x%02x\n", r8, at_i16, r8_value);
        return true;
    }

    return false;
}

static bool test_opcodes_ld_r8_at_r16_inc(int r8_r16_h, int permutation, char *buffer, test_opcodes_State *before, test_opcodes_State *after) {
    srand((uint32_t)(permutation + 19));

    uint8_t r8 = (uint8_t)(r8_r16_h >> 4);
    uint8_t r16_h = r8_r16_h & 0xf;

    uint16_t i16 = (uint16_t)(rand() & 0xff00) | ((rand() >> 8) & 0xff);
    uint8_t at_i16 = rand() & 0xff;

    if (after == NULL) {
        before->mem[0xfff0 | r16_h] = i16 >> 8;
        before->mem[0xfff0 | (r16_h + 1)] = i16 & 0xff;

        before->mem[i16] = at_i16;

        sprintf(buffer + strlen(buffer), "i 0x%04x", i16);
        return permutation >= 0xff;
    }

    int pc_diff = test_state_M(after) - test_state_M(before);

    if (pc_diff != 1) {
        sprintf(buffer + strlen(buffer), "expected pc to move %d step(s), actual moved %d step(s)\n", 1, pc_diff);
        return true;
    }

    uint8_t r8_value = after->mem[0xfff0 | r8];

    if (r8_value != at_i16) {
        sprintf(buffer + strlen(buffer), "expected r8 (%d) to be 0x%02x, is 0x%02x\n", r8, at_i16, r8_value);
        return true;
    }

    uint16_t r16_before_value = (uint16_t)(before->mem[0xfff0 | r16_h] << 8) | before->mem[0xfff0 | (r16_h + 1)];

    uint16_t expected_r16_value = (uint16_t)(r16_before_value + 1);
    uint16_t actual_r16_value = (uint16_t)(after->mem[0xfff0 | r16_h] << 8) | after->mem[0xfff0 | (r16_h + 1)];

    if (expected_r16_value != actual_r16_value) {
        sprintf(buffer + strlen(buffer), "expected r16 (%d) to be 0x%04x, is 0x%04x\n", r16_h, expected_r16_value, actual_r16_value);
        return true;
    }

    return false;
}

static test_opcodes_Test_case test_cases[] = {
    { .o = NOP, .name = "nop", .test = test_opcodes_nop, .data = 0 },

    { .o = LD_A_I8, .name = "ld a, {imm:i8}", .test = test_opcodes_ld_r8_i8, .data = C_A },
    { .o = LD_B_I8, .name = "ld b, {imm:i8}", .test = test_opcodes_ld_r8_i8, .data = C_B },
    { .o = LD_C_I8, .name = "ld c, {imm:i8}", .test = test_opcodes_ld_r8_i8, .data = C_C },
    { .o = LD_D_I8, .name = "ld d, {imm:i8}", .test = test_opcodes_ld_r8_i8, .data = C_D },

    { .o = LD_A_B, .name = "ld a, b", .test = test_opcodes_ld_r8_r8, .data = C_A << 4 | C_B },
    { .o = LD_A_C, .name = "ld a, c", .test = test_opcodes_ld_r8_r8, .data = C_A << 4 | C_C },
    { .o = LD_A_D, .name = "ld a, d", .test = test_opcodes_ld_r8_r8, .data = C_A << 4 | C_D },

    { .o = LD_B_A, .name = "ld b, a", .test = test_opcodes_ld_r8_r8, .data = C_B << 4 | C_A },
    { .o = LD_B_C, .name = "ld b, c", .test = test_opcodes_ld_r8_r8, .data = C_B << 4 | C_C },
    { .o = LD_B_D, .name = "ld b, d", .test = test_opcodes_ld_r8_r8, .data = C_B << 4 | C_D },

    { .o = LD_C_A, .name = "ld c, a", .test = test_opcodes_ld_r8_r8, .data = C_C << 4 | C_A },
    { .o = LD_C_B, .name = "ld c, b", .test = test_opcodes_ld_r8_r8, .data = C_C << 4 | C_B },
    { .o = LD_C_D, .name = "ld c, d", .test = test_opcodes_ld_r8_r8, .data = C_C << 4 | C_D },

    { .o = LD_D_A, .name = "ld d, a", .test = test_opcodes_ld_r8_r8, .data = C_D << 4 | C_A },
    { .o = LD_D_B, .name = "ld d, b", .test = test_opcodes_ld_r8_r8, .data = C_D << 4 | C_B },
    { .o = LD_D_C, .name = "ld d, c", .test = test_opcodes_ld_r8_r8, .data = C_D << 4 | C_C },

    { .o = LD_I_I16, .name = "ld i, {imm:i16}", .test = test_opcodes_ld_r16_i16, .data = C_IH },
    { .o = LD_J_I16, .name = "ld j, {imm:i16}", .test = test_opcodes_ld_r16_i16, .data = C_JH },
    { .o = LD_K_I16, .name = "ld k, {imm:i16}", .test = test_opcodes_ld_r16_i16, .data = C_KH },

    { .o = LD_A_AT_I16, .name = "ld a, [{imm:i16}]", .test = test_opcodes_ld_r8_at_i16, .data = C_A },
    { .o = LD_B_AT_I16, .name = "ld b, [{imm:i16}]", .test = test_opcodes_ld_r8_at_i16, .data = C_B },
    { .o = LD_C_AT_I16, .name = "ld c, [{imm:i16}]", .test = test_opcodes_ld_r8_at_i16, .data = C_C },
    { .o = LD_D_AT_I16, .name = "ld d, [{imm:i16}]", .test = test_opcodes_ld_r8_at_i16, .data = C_D },

    { .o = LD_A_AT_AT_I16, .name = "ld a, [[{imm:i16}]]", .test = test_opcodes_ld_r8_at_at_i16, .data = C_A },
    { .o = LD_B_AT_AT_I16, .name = "ld b, [[{imm:i16}]]", .test = test_opcodes_ld_r8_at_at_i16, .data = C_B },
    { .o = LD_C_AT_AT_I16, .name = "ld c, [[{imm:i16}]]", .test = test_opcodes_ld_r8_at_at_i16, .data = C_C },
    { .o = LD_D_AT_AT_I16, .name = "ld d, [[{imm:i16}]]", .test = test_opcodes_ld_r8_at_at_i16, .data = C_D },

    { .o = LD_A_AT_I, .name = "ld a, [i]", .test = test_opcodes_ld_r8_at_r16, .data = C_A << 4 | C_IH },
    { .o = LD_B_AT_I, .name = "ld b, [i]", .test = test_opcodes_ld_r8_at_r16, .data = C_B << 4 | C_IH },
    { .o = LD_C_AT_I, .name = "ld c, [i]", .test = test_opcodes_ld_r8_at_r16, .data = C_C << 4 | C_IH },
    { .o = LD_D_AT_I, .name = "ld d, [i]", .test = test_opcodes_ld_r8_at_r16, .data = C_D << 4 | C_IH },

    { .o = LD_A_AT_J, .name = "ld a, [j]", .test = test_opcodes_ld_r8_at_r16, .data = C_A << 4 | C_JH },
    { .o = LD_B_AT_J, .name = "ld b, [j]", .test = test_opcodes_ld_r8_at_r16, .data = C_B << 4 | C_JH },
    { .o = LD_C_AT_J, .name = "ld c, [j]", .test = test_opcodes_ld_r8_at_r16, .data = C_C << 4 | C_JH },
    { .o = LD_D_AT_J, .name = "ld d, [j]", .test = test_opcodes_ld_r8_at_r16, .data = C_D << 4 | C_JH },

    { .o = LD_A_AT_K, .name = "ld a, [k]", .test = test_opcodes_ld_r8_at_r16, .data = C_A << 4 | C_KH },
    { .o = LD_B_AT_K, .name = "ld b, [k]", .test = test_opcodes_ld_r8_at_r16, .data = C_B << 4 | C_KH },
    { .o = LD_C_AT_K, .name = "ld c, [k]", .test = test_opcodes_ld_r8_at_r16, .data = C_C << 4 | C_KH },
    { .o = LD_D_AT_K, .name = "ld d, [k]", .test = test_opcodes_ld_r8_at_r16, .data = C_D << 4 | C_KH },

    { .o = LD_A_AT_I_INC, .name = "ld a, [i++]", .test = test_opcodes_ld_r8_at_r16_inc, .data = C_A << 4 | C_IH },
    { .o = LD_B_AT_I_INC, .name = "ld b, [i++]", .test = test_opcodes_ld_r8_at_r16_inc, .data = C_B << 4 | C_IH },
    { .o = LD_C_AT_I_INC, .name = "ld c, [i++]", .test = test_opcodes_ld_r8_at_r16_inc, .data = C_C << 4 | C_IH },
    { .o = LD_D_AT_I_INC, .name = "ld d, [i++]", .test = test_opcodes_ld_r8_at_r16_inc, .data = C_D << 4 | C_IH },

    { .o = LD_A_AT_J_INC, .name = "ld a, [j++]", .test = test_opcodes_ld_r8_at_r16_inc, .data = C_A << 4 | C_JH },
    { .o = LD_B_AT_J_INC, .name = "ld b, [j++]", .test = test_opcodes_ld_r8_at_r16_inc, .data = C_B << 4 | C_JH },
    { .o = LD_C_AT_J_INC, .name = "ld c, [j++]", .test = test_opcodes_ld_r8_at_r16_inc, .data = C_C << 4 | C_JH },
    { .o = LD_D_AT_J_INC, .name = "ld d, [j++]", .test = test_opcodes_ld_r8_at_r16_inc, .data = C_D << 4 | C_JH },

    { .o = LD_A_AT_K_INC, .name = "ld a, [k++]", .test = test_opcodes_ld_r8_at_r16_inc, .data = C_A << 4 | C_KH },
    { .o = LD_B_AT_K_INC, .name = "ld b, [k++]", .test = test_opcodes_ld_r8_at_r16_inc, .data = C_B << 4 | C_KH },
    { .o = LD_C_AT_K_INC, .name = "ld c, [k++]", .test = test_opcodes_ld_r8_at_r16_inc, .data = C_C << 4 | C_KH },
    { .o = LD_D_AT_K_INC, .name = "ld d, [k++]", .test = test_opcodes_ld_r8_at_r16_inc, .data = C_D << 4 | C_KH },

    // LD_AT_I16_A
};

static int test_opcodes_step(uint8_t rom_alu[ROM_SIZE_ALU], uint8_t rom_opcode1[ROM_SIZE_OPCODE], uint8_t rom_opcode2[ROM_SIZE_OPCODE], char buffer[8096], test_opcodes_State *state) {
    uint32_t index = (uint32_t)(
        ((state->mh >> 7)  << 16) |
        ((state->ml & 0xf) << 12) |
         (state->s         << 8)  |
          state->o);

    uint16_t signals = (uint16_t)((rom_opcode2[index] << 8) | rom_opcode1[index]) ^ SIGNALS_ACTIVE_LOW_MASK;

    sprintf(buffer + strlen(buffer), "\n0x%02x: index: 0x%05x signals: 0x%04x\n", state->s, index, signals);

    if (state->o != RESET && state->s == 0 && signals != S0_FETCH) {
        sprintf(buffer + strlen(buffer), "      expected signals at s 0 to be S0_FETCH\n");
        return 2;
    }

    bool use_c_address = test_state_M_or_C(state);

    uint8_t bus = 0;
    uint16_t mem_address = (uint16_t)(use_c_address ? (0xfff0 | (state->c & 0xf)) : test_state_M(state));

    if (signals & OE_MEM) {
        bus = state->mem[mem_address];

        sprintf(buffer + strlen(buffer), "      OE_MEM: bus = 0x%02x from mem[0x%04x] (%c)\n", bus, mem_address, test_state_M_or_C(state) ? 'C' : 'M');
    } else if (signals & OE_T) {
        bus = state->t;

        sprintf(buffer + strlen(buffer), "      OE_T: bus = 0x%02x\n", bus);
    } else if (signals & OE_ALU) {
        uint32_t alu_index = (uint32_t)(
            ((state->c & 0x7) << 16) |
             (state->mh       << 8)  |
              state->ml
            );

        bus = rom_alu[alu_index];

        sprintf(buffer + strlen(buffer), "      OE_ALU: bus = 0x%02x from alu[0x%05x]\n", bus, alu_index);
    } else {
        sprintf(buffer + strlen(buffer), "      nothing asserted on the bus\n");
        return 2;
    };

    if (signals & LD_O) {
        sprintf(buffer + strlen(buffer), "      LD_O\n");

        state->o = bus;
    }

    if (signals & LD_T) {
        sprintf(buffer + strlen(buffer), "      LD_T\n");

        state->t = bus;
    }

    if (signals & LD_MEM) {
        sprintf(buffer + strlen(buffer), "      LD_MEM to mem[0x%04x] (%c)\n", mem_address, test_state_M_or_C(state) ? 'C' : 'M');

        state->mem[mem_address] = bus;
    }

    if (signals & LD_C) {
        sprintf(buffer + strlen(buffer), "      LD_C: sel C=0x%x sel M/C=%d\n", (signals >> 11) & 0xf, signals >> 11 >> 4);

        state->c = signals >> 11;
    }

    if (signals & LD_ML) {
        sprintf(buffer + strlen(buffer), "      LD_ML\n");

        state->ml = bus;
    }

    if (signals & LD_MH) {
        sprintf(buffer + strlen(buffer), "      LD_MH\n");

        state->mh = bus;
    }

    if (signals & INC_M) {
        sprintf(buffer + strlen(buffer), "      INC_M: mh=0x%02x ml=0x%02x\n", state->mh, state->ml);

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
        sprintf(buffer + strlen(buffer), "      LD_S\n");

        state->s = 0;
     } else {
         ++state->s;
         if (state->s == 0x10) state->s = 0;
     }

     return state->s == 0 ? 1 : 0;
}

static int test_opcodes(uint8_t rom_alu[ROM_SIZE_ALU], uint8_t rom_opcode1[ROM_SIZE_OPCODE], uint8_t rom_opcode2[ROM_SIZE_OPCODE]) {
    char buffer[0x2000];
    char failed_permutation_buffer[0x100];

    int n_failed_tests = 0;

    for (int i = 0; i < sizeof(test_cases) / sizeof(test_cases[0]); ++i) {
        for (int permutation = 0;; ++permutation) {
            buffer[0] = '\0';
            failed_permutation_buffer[0] = '\0';

            test_opcodes_State state = {0};

            test_opcodes_Test_case test_case = test_cases[i];

            state.o = (uint8_t)test_case.o; // To prevent getting stuck in reset opcode

            state.mem[test_state_M(&state)] = (uint8_t)test_case.o; // The value loaded into O on first step

            bool last_permutation = test_case.test(test_case.data, permutation, failed_permutation_buffer, &state, NULL);

            test_opcodes_State orignal_state = state;

            bool test_failed = false;

            int steps = 0;

            for (; steps < 0x100; ++steps) {
                int error = test_opcodes_step(rom_alu, rom_opcode1, rom_opcode2, buffer, &state);

                if (error > 1) {
                    test_failed = true;
                    break;
                }

                if (error > 0) break;
            }

            if (!test_failed) {
                sprintf(buffer + strlen(buffer), "\nsteps taken: %d\n\n", steps + 1);

                if (steps > 0xf) {
                    sprintf(buffer + strlen(buffer), "opcode never finished\n");
                    test_failed = true;
                } else if (test_state_M_or_C(&state)) {
                    sprintf(buffer + strlen(buffer), "opcode never restored mem address to M\n");
                    test_failed = true;
                } else {
                    test_failed = test_case.test(test_case.data, permutation, buffer, &orignal_state, &state);
                }
            }

            if (test_failed) {
                fprintf(stderr, "%s - %s (permutation %d) failed:\n%s\n", test_case.name, failed_permutation_buffer, permutation, buffer);
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
