#define test_state_M(state) ((uint16_t)(((state)->mh << 8) | (state)->ml))
#define test_state_M_or_C(state) ((state)->c & 0x10)

static bool test_instruction_expect_pc_step(int pc_diff_expected, char **buffer_ptr, const test_State* before, const test_State* after) {
    int pc_diff_actual = test_state_M(after) - test_state_M(before);

    if (pc_diff_actual != pc_diff_expected) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected pc to move %d step(s), actual moved %d step(s)\n", pc_diff_expected, pc_diff_actual);
        return true;
    }

    return false;
}

static int test_step(uint8_t rom_alu[ROM_SIZE_ALU], uint8_t rom_instruction1[ROM_SIZE_INSTRUCTION], uint8_t rom_instruction2[ROM_SIZE_INSTRUCTION], char** buffer_ptr, test_State* state) {
    uint32_t index = (uint32_t)(
        (((state->mh >> 5) & 1) << 16) |
        ((state->tf & 0xf)      << 12) |
         (state->s              << 8)  |
          state->i);

    uint16_t signals = (uint16_t)((rom_instruction2[index] << 8) | rom_instruction1[index]) ^ SIGNALS_ACTIVE_LOW_MASK;

    char* buffer = *buffer_ptr;

    buffer += sprintf(buffer, "\n    0x%02x: index: 0x%05x signals: 0x%04x\n", state->s, index, signals);

    if (state->tf != 0 && state->i != RESET && state->s == 0 && signals != S0_FETCH) {
        buffer += sprintf(buffer, "        expected signals at s 0 to be S0_FETCH, TF=0x%02x\n", state->tf);
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
    } else if (!(signals & LD_C) && (signals & SEL_C_OE_GPI)) {
        bus = state->gpi;

        buffer += sprintf(buffer, "        OE_GPI: bus = 0x%02x\n", bus);
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

        state->gpi = (uint8_t)(
            (((state->tf >> 3) & 1) << 7) | // this is probably unnecessary
            ((state->in_miso & 1)   << 1) |
            ((state->in_rx & 1)     << 0));
    }

    if (signals & LD_MEM) {
        buffer += sprintf(buffer, "        LD_MEM to mem[0x%04x] (%c)\n", mem_address, test_state_M_or_C(state) ? 'C' : 'M');

        state->mem[mem_address] = bus;
    }

    if (signals & LD_C) {
        buffer += sprintf(buffer, "        LD_C: sel C=0x%x sel M/C=%d sel SCLK=%d\n", (signals >> 11) & 0xf, signals >> 11 >> 4, (signals >> 11 >> 3) & 1);

        state->c = signals >> 11;
    } else {
        if (signals & SEL_C_LD_TF) {
            bool sel_c = signals & SEL_C;
            uint8_t tf2 = sel_c ? (((~bus) >> 2) & 1) : 1;
            uint8_t tf3 = (bus >> 7);

            state->tf = (uint8_t)(
                (tf3 << 3) |
                (tf2 << 2) |
                (bus & 0x3));

            buffer += sprintf(buffer, "        LD_TF: TF=0x%02x sel MOSI=%d sel C=%d\n", state->tf, bus >> 7, sel_c ? 1 : 0);
        }

        if (signals & SEL_C_LD_GPO) {
            buffer += sprintf(buffer, "        LD_GPO\n");
            state->gpo = bus;
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

static int test_instructions(uint8_t rom_alu[ROM_SIZE_ALU], uint8_t rom_instruction1[ROM_SIZE_INSTRUCTION], uint8_t rom_instruction2[ROM_SIZE_INSTRUCTION], const Instruction instructions[0x100]) {
    char buffer[0x2000];
    char* buffer_ptr = buffer;

    test_State state = {0};

    // fill mem with bogus data
    for (int i = 0; i < 0x10000; ++i) {
        state.mem[i] = i & 1 ? 0xab : 0xba;
    }

    // fill regs with bogus data
    state.i = 0xab;
    state.ml = 0xfa;
    state.mh = 0x43;
    state.t = 0x85;
    state.c = 0xf;

    state.s  = 0; // reset hooked up
    state.tf = 0; // reset hooked up

    int steps = 0;

    for (;; ++steps) {
        if ((steps & 0x1f) == 0x1f) {
            // fprintf(stderr, "steps: %d\n%s\n", steps, buffer);
            buffer[0] = '\0';
            buffer_ptr = buffer;
        }

        int error = test_step(rom_alu, rom_instruction1, rom_instruction2, &buffer_ptr, &state);

        if (error > 1) {
            fprintf(stderr, "reset failed after %d steps: %s\n", steps, buffer);
            return 1;
        }

        if (state.i == NOP && error == 1) break;

        if (state.tf != 0 && state.mh >= 0x20 && state.ml >= 0x02) {
            fprintf(stderr, "reset failed, M too big 0x%02x%02x\n", state.mh, state.ml);
            return 1;
        }

        if (steps >= 300000) {
            fprintf(stderr, "reset aborted after %d steps\n", steps);
            return 1;
        }
    }

    printf("reset complete after %d steps\n", steps);

    if (state.mh != 0 || state.ml != 0) {
        fprintf(stderr, "reset failed, expected mh (0x%02x) and ml (0x%02x) to be zero\n", state.mh, state.ml);
        return 1;
    }

    if (state.tf != 0x4) {
        fprintf(stderr, "reset failed, expected tf (0x%02x) to be 0x4\n", state.tf);
        return 1;
    }

    if (state.mem[0xfff0 | C_FF] != 0xff) {
        fprintf(stderr, "reset failed, expected mem[C_FF] (0x%02x) to be 0xff\n", state.mem[0xfff0 | C_FF]);
        return 1;
    }

    if (state.mem[0xfff0 | C_SP] != 0x00) {
        fprintf(stderr, "reset failed, expected mem[C_SP] (0x%02x) to be 0x00\n", state.mem[0xfff0 | C_SP]);
        return 1;
    }

    if (state.mem[0xfff0 | C_F] != 0xff) {
        fprintf(stderr, "reset failed, expected mem[C_F] (0x%02x) to be 0xff\n", state.mem[0xfff0 | C_F]);
        return 1;
    }

     if (test_state_M_or_C(&state)) {
        fprintf(stderr, "reset never restored mem address to M\n");
        return 1;
    }

    for (int i = 0; i < ROM_SIZE_BOOT; ++i) {
        uint8_t actual = state.mem[i];
        uint8_t expected = rom_alu[(A_BOOT << 16) | i];

        if (actual != expected) {
            fprintf(stderr, "reset failed to copy boot rom byte 0x%02x to mem[0x%04x] (0x%02x)\n", expected, i, actual);
            return 1;
        }
    }

    int n_failed_tests = 0;

    for (int i = 0 ; i < 0x100; ++i) {
        if (instructions[i].test == NULL) {
            if (instructions[i].customasm != NULL)
                printf("%s - test not yet implemented\n", instructions[i].customasm);

            continue;
        }

        for (int permutation = 0;; ++permutation) {
            buffer[0] = '\0';
            buffer_ptr = buffer;

            if (state.mh == 0xff) state.mh = 0; // prevent overwriting mem registers

            state.i = (uint8_t)i; // prevent getting stuck in reset instruction

            bool last_permutation =
                instructions[i].test(permutation, instructions[i].operands, &buffer_ptr, &state, NULL);

            buffer_ptr += sprintf(buffer_ptr, "\n");

            state.mem[test_state_M(&state)] = (uint8_t)i; // the value loaded into I on first step

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
                } else if (state.tf == 0) {
                    buffer_ptr += sprintf(buffer_ptr, "TF is 0, should only be true at cold start\n");
                    test_failed = true;
                } else {
                    test_failed =
                        instructions[i].test(permutation, instructions[i].operands, &buffer_ptr, &orignal_state, &state);
                }
            }

            if (test_failed) {
                fprintf(stderr, "%s - permutation %d failed: %s\n", instructions[i].customasm, permutation, buffer);
                ++n_failed_tests;
                break;
            } else if (last_permutation) {
                printf("%s - passed (%d times)\n", instructions[i].customasm, permutation + 1);
                break;
            }
        }
    }

    return n_failed_tests;
}
