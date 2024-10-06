typedef struct {
    uint8_t i;
    uint8_t s; // 4 bit
    uint8_t ml;
    uint8_t mh;
    uint8_t c;
    uint8_t t;
    uint8_t tf; // 4 bit
    uint8_t gpo;
    uint8_t gpi;
    uint8_t mem[0x10000];
} test_State;

typedef struct {
    Instruction i;
    const char* name;
    int data;
    bool (*test)(int, int, char**, test_State*, test_State*);
} test_case;

#define test_state_M(state) ((uint16_t)(((state)->mh << 8) | (state)->ml))
#define test_state_M_or_C(state) ((state)->c & 0x10)

static bool test_instruction_expect_pc_step(int pc_diff_expected, char **buffer_ptr, test_State* before, test_State* after) {
    int pc_diff_actual = test_state_M(after) - test_state_M(before);

    if (pc_diff_actual != pc_diff_expected) {
        *buffer_ptr += sprintf(*buffer_ptr, "expected pc to move %d step(s), actual moved %d step(s)\n", pc_diff_expected, pc_diff_actual);
        return true;
    }

    return false;
}

#include "test_instructions/nop.inc"
#include "test_instructions/ld_r8_i8.inc"
#include "test_instructions/ld_r8_r8.inc"
#include "test_instructions/ld_r16_i16.inc"
#include "test_instructions/ld_r8_at_i16.inc"
#include "test_instructions/ld_r8_at_at_i16.inc"
#include "test_instructions/ld_r8_at_r16.inc"
#include "test_instructions/ld_r8_at_r16_inc.inc"
#include "test_instructions/ld_at_i16_r8.inc"
#include "test_instructions/ld_at_r16_inc_r8.inc"
#include "test_instructions/add_r8_i8.inc"
#include "test_instructions/addc_r8_i8.inc"
#include "test_instructions/subc_r8_i8.inc"
#include "test_instructions/shl_r8.inc"
#include "test_instructions/shr_r8.inc"
#include "test_instructions/and_r8_i8.inc"
#include "test_instructions/jmp_i16.inc"
#include "test_instructions/jmp_r16.inc"
#include "test_instructions/jmp_i16_conditional.inc"
#include "test_instructions/jal_r16_i16.inc"
#include "test_instructions/push_r8.inc"
#include "test_instructions/push_r16.inc"
#include "test_instructions/pop_r8.inc"
#include "test_instructions/pop_r16.inc"
#include "test_instructions/out_i8.inc"
#include "test_instructions/out_tx_start.inc"
#include "test_instructions/out_tx.inc"
#include "test_instructions/out_tx_stop.inc"
#include "test_instructions/in_rx_start_i16.inc"
#include "test_instructions/in_rx.inc"

static test_case test_cases[] = {
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
    { .i = LD_AT_J_INC_A, .name = MNEMONIC_LD_AT_J_INC_A, .test = test_instruction_ld_at_r16_inc_r8, .data = C_JH << 4 | C_A },
    { .i = LD_AT_K_INC_A, .name = MNEMONIC_LD_AT_K_INC_A, .test = test_instruction_ld_at_r16_inc_r8, .data = C_KH << 4 | C_A },

    { .i = LD_FLAGS_I8, .name = MNEMONIC_LD_FLAGS_I8, .test = test_instruction_ld_r8_i8, .data = C_F },

    { .i = SHL_A, .name = MNEMONIC_SHL_A, .test = test_instruction_shl_r8, .data = C_A },

    { .i = SHR_A, .name = MNEMONIC_SHR_A, .test = test_instruction_shr_r8, .data = C_A },

    { .i = AND_A_I8, .name = MNEMONIC_AND_A_I8, .test = test_instruction_and_r8_i8, .data = C_A },

    { .i = ADD_A_I8, .name = MNEMONIC_ADD_A_I8, .test = test_instruction_add_r8_i8, .data = C_A },

    { .i = ADDC_A_I8, .name = MNEMONIC_ADDC_A_I8, .test = test_instruction_addc_r8_i8, .data = C_A },

    { .i = SUBC_A_I8, .name = MNEMONIC_SUBC_A_I8, .test = test_instruction_subc_r8_i8, .data = C_A },

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

    { .i = OUT_I8, .name = MNEMONIC_OUT_I8, .test = test_instruction_out_i8, .data = 0 },

    { .i = OUT_TX_START, .name = MNEMONIC_OUT_TX_START, .test = test_instruction_out_tx_start, .data = 0 },
    { .i = OUT_TX,       .name = MNEMONIC_OUT_TX,       .test = test_instruction_out_tx,       .data = 0 },
    { .i = OUT_TX_STOP,  .name = MNEMONIC_OUT_TX_STOP,  .test = test_instruction_out_tx_stop,  .data = 0 },

    { .i = IN_RX_START_I16, .name = MNEMONIC_IN_RX_START_I16, .test = test_instruction_in_rx_start_i16, .data = 0 },
    { .i = IN_RX,           .name = MNEMONIC_IN_RX,           .test = test_instruction_in_rx,           .data = 0 },
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

            test_case test_case = test_cases[i];

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
