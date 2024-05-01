#include "emulate.h"

static void test_instr_nop(uint8_t control[CONTROL_ROM_SIZE], uint8_t alu[ALU_ROM_SIZE]) {
    printf("%-16s", customasm_rule_from_opcode(O_NOP));

    State state = {0};
    state.f = F_I;

    uint16_t pc = ((uint16_t)(state.mh << 8) | state.ml);
    state.mem[pc] = O_NOP;

    for (uint16_t i = 0xfff0; i < 0xfff8; ++i)
        state.mem[i] = i & 0xff;

    uint16_t pc_expected  = pc + 1;
    uint8_t step_expected = 16;

    State org_state = state;

    int step;
    for (step = 1; step < 20; ++step)
        if (emulate_next_cycle(false, control, alu, &state)) break;

    uint16_t pc_after = (uint16_t)(state.mh << 8) | state.ml;

    if (step != step_expected) {
        printf("failed\n");
        fprintf(stderr, "expected %02x steps, got %02x\n", step_expected, step);
        exit(1);
    }

    if (pc_after != pc_expected) {
        printf("failed\n");
        fprintf(stderr, "unexpected pc, got %04x, expected %04x\n", pc_after, pc_expected);
        exit(1);
    }

    if (org_state.o != state.o) assert(false);
    if (org_state.f != state.f) assert(false);
    if (org_state.c != state.c) assert(false);
    if (org_state.t != state.t) assert(false);

    for (uint16_t i = 0xfff0; i < 0xfff8; ++i)
        if (org_state.mem[i] != state.mem[i]) assert(false && "unexpected mem change");

    printf("passed\n");
}

static void test_instr_inc_r8(uint8_t control[CONTROL_ROM_SIZE], uint8_t alu[ALU_ROM_SIZE], O inc_opcocde, C r8) {
    printf("%-16s", customasm_rule_from_opcode(inc_opcocde));

    State state = {0};
    state.f = F_I;

    for (int value = 0x00; value < 0x100; ++value) {
        uint16_t pc = ((uint16_t)(state.mh << 8) | state.ml);
        state.mem[pc] = (uint8_t)inc_opcocde;

        state.mem[0xfff0 | r8] = (uint8_t)value;

        uint16_t pc_expected  = pc + 1;
        uint8_t step_expected = 0xd;
        uint8_t c_r8_expected = (uint8_t)(value + 1);

        int step;
        for (step = 1; step < 20; ++step)
            if (emulate_next_cycle(false, control, alu, &state)) break;

        uint16_t pc_after = (uint16_t)(state.mh << 8) | state.ml;
        uint8_t c_r8_after = state.mem[0xfff0 | r8];

        if (step != step_expected) {
            printf("failed\n");
            fprintf(stderr, "expected %02x steps, got %02x\n", step_expected, step);
            exit(1);
        }

        if (pc_after != pc_expected) {
            printf("failed\n");
            fprintf(stderr, "unexpected pc, got %04x, expected %04x\n", pc_after, pc_expected);
            exit(1);
        }

        if (c_r8_after != c_r8_expected) {
            printf("failed\n");
            fprintf(stderr, "unexpected value, got %02x, expected %02x\n", c_r8_after, c_r8_expected);
            exit(1);
        }

        if (c_r8_after == 0 && !(state.f & F_Z)) {
            printf("failed\n");
            fprintf(stderr, "expected zero flag to be set\n");
            exit(1);
        } else if (c_r8_after != 0 && (state.f & F_Z)) {
            printf("failed\n");
            fprintf(stderr, "expected zero flag to be cleared\n");
            exit(1);
        }

        if (c_r8_after == 0 && !(state.f & F_C)) {
            printf("failed\n");
            fprintf(stderr, "expected carry flag to be set\n");
            exit(1);
        } else if (c_r8_after != 0 && (state.f & F_C)) {
            printf("failed\n");
            fprintf(stderr, "expected carry flag to be cleared\n");
            exit(1);
        }

        if ((c_r8_after & 0x80) && !(state.f & F_S)) {
            printf("failed\n");
            fprintf(stderr, "expected sign flag to be set\n");
            exit(1);
        } else if (!(c_r8_after & 0x80) && (state.f & F_S)) {
            printf("failed\n");
            fprintf(stderr, "expected sign flag to be cleared\n");
            exit(1);
        }
    }

    printf("passed\n");
}

static void test_instr_out_p3_i8(uint8_t control[CONTROL_ROM_SIZE], uint8_t alu[ALU_ROM_SIZE], O out_opcode) {
    printf("%-32s", customasm_rule_from_opcode(O_OUT_0_A));

    State state = {0};
    state.f = F_I;

    uint16_t pc = ((uint16_t)(state.mh << 8) | state.ml);
    state.mem[pc] = (uint8_t)out_opcode;

    uint8_t value = 0xba;
    state.mem[pc + 1] = value;

    int step;
    for (step = 1; step < 20; ++step)
        if (emulate_next_cycle(false, control, alu, &state)) break;

    uint16_t pc_after = (uint16_t)(state.mh << 8) | state.ml;
    uint16_t pc_expected  = pc + 2;

    if (pc_after != pc_expected) {
        printf("failed\n");
        fprintf(stderr, "unexpected pc, got %04x, expected %04x\n", pc_after, pc_expected);
        exit(1);
    }

    if (state.gpo != value) {
        printf("failed\n");
        fprintf(stderr, "unexpected gpo, got %02x, expected %02x\n", state.gpo, value);
        exit(1);
    }

    printf("passed\n");
}

static void test_instr_addc_r8_i8(uint8_t control[CONTROL_ROM_SIZE], uint8_t alu[ALU_ROM_SIZE], O addc_opcode, C r8) {
    printf("%-32s", customasm_rule_from_opcode(addc_opcode));

    State state = {0};
    state.f = F_I;

    uint16_t pc = ((uint16_t)(state.mh << 8) | state.ml);
    state.mem[pc] = (uint8_t)addc_opcode;

    uint8_t value = (uint8_t)-10;
    state.mem[pc + 1] = value;

    state.mem[0xfff0 | r8] = 13;

    int step;
    for (step = 1; step < 20; ++step)
        if (emulate_next_cycle(false, control, alu, &state)) break;

    uint16_t pc_after = (uint16_t)(state.mh << 8) | state.ml;
    uint16_t pc_expected  = pc + 2;

    if (pc_after != pc_expected) {
        printf("failed\n");
        fprintf(stderr, "unexpected pc, got %04x, expected %04x\n", pc_after, pc_expected);
        exit(1);
    }

    uint8_t c_r8_after = state.mem[0xfff0 | r8];
    uint8_t c_r8_expected = (uint8_t)(13 + value);

    if (c_r8_after != c_r8_expected) {
        printf("failed\n");
        fprintf(stderr, "unexpected value, got %02x, expected %02x\n", c_r8_after, c_r8_expected);
        exit(1);
    }

    if (!(state.f & F_C)) {
        printf("failed\n");
        fprintf(stderr, "expected carry flag to be set\n");
        exit(1);
    }

    printf("passed\n");
}

static void test_instr_ld_r8_flags(uint8_t control[CONTROL_ROM_SIZE], uint8_t alu[ALU_ROM_SIZE], O opcode, C r8) {
    printf("%-32s", customasm_rule_from_opcode(opcode));

    State state = {0};

    for (uint8_t flags = F_I; flags < 0x10; ++flags) {
        state.f = flags;

        uint16_t pc = ((uint16_t)(state.mh << 8) | state.ml);
        state.mem[pc] = (uint8_t)opcode;

        state.mem[0xfff0 | r8] = 0xaa;

        int step;
        for (step = 1; step < 20; ++step)
            if (emulate_next_cycle(false, control, alu, &state)) break;

        uint16_t pc_after = (uint16_t)(state.mh << 8) | state.ml;
        uint16_t pc_expected  = pc + 1;

        if (pc_after != pc_expected) {
            printf("failed\n");
            fprintf(stderr, "unexpected pc, got %04x, expected %04x\n", pc_after, pc_expected);
            exit(1);
        }

        if (state.f != flags) {
            printf("failed\n");
            fprintf(stderr, "unexpected flags, got %02x, expected %02x\n", state.f, flags);
            exit(1);
        }
    }

    printf("passed\n");
}

static void test_instr_ld_flags_r8(uint8_t control[CONTROL_ROM_SIZE], uint8_t alu[ALU_ROM_SIZE], O opcode, C r8) {
    printf("%-32s", customasm_rule_from_opcode(opcode));

    State state = {0};

    for (int value = 0; value < 0x100; ++value) {
        state.f = F_I;

        uint16_t pc = ((uint16_t)(state.mh << 8) | state.ml);
        state.mem[pc] = (uint8_t)opcode;

        uint8_t flags_expected = (uint8_t)(value & 0x0f) | F_I;

        state.mem[0xfff0 | r8] = flags_expected;

        int step;
        for (step = 1; step < 20; ++step)
            if (emulate_next_cycle(false, control, alu, &state)) break;

        uint16_t pc_after = (uint16_t)(state.mh << 8) | state.ml;
        uint16_t pc_expected  = pc + 1;

        if (pc_after != pc_expected) {
            printf("failed\n");
            fprintf(stderr, "unexpected pc, got %04x, expected %04x\n", pc_after, pc_expected);
            exit(1);
        }

        if (state.f != flags_expected) {
            printf("failed\n");
            fprintf(stderr, "unexpected flags, got %02x, expected %02x\n", state.f, flags_expected);
            exit(1);
        }
    }

    printf("passed\n");
}

static void test_instr_shlc_r8(uint8_t control[CONTROL_ROM_SIZE], uint8_t alu[ALU_ROM_SIZE], O opcode, C r8) {
    printf("%-32s", customasm_rule_from_opcode(opcode));

    State state = {0};

    for (int value = 0; value < 0x100; ++value) {
        for (int cf = 0; cf < 2; ++cf) {
            state.f = F_I | (cf == 1 ? F_C : 0);

            uint16_t pc = ((uint16_t)(state.mh << 8) | state.ml);
            state.mem[pc] = (uint8_t)opcode;

            state.mem[0xfff0 | r8] = (uint8_t)value;
            uint8_t value_expected = (uint8_t)((value << 1) | cf);

            int step;
            for (step = 1; step < 20; ++step)
                if (emulate_next_cycle(false, control, alu, &state)) break;

            uint16_t pc_after = (uint16_t)(state.mh << 8) | state.ml;
            uint16_t pc_expected  = pc + 1;

            if (pc_after != pc_expected) {
                printf("failed\n");
                fprintf(stderr, "unexpected pc, got %04x, expected %04x\n", pc_after, pc_expected);
                exit(1);
            }

            uint8_t value_actual = state.mem[0xfff0 | r8];

            if (value_actual != value_expected) {
                printf("failed\n");
                fprintf(stderr, "unexpected value, got %02x, expected %02x\n", value_actual, value_expected);
                exit(1);
            }
        }
    }

    printf("passed\n");
}

static void test_instr_or_r8_r8(uint8_t control[CONTROL_ROM_SIZE], uint8_t alu[ALU_ROM_SIZE], O opcocde, C r8_dest, C r8_src) {
    printf("%-32s", customasm_rule_from_opcode(opcocde));

    State state = {0};
    state.f = F_I;

    for (int value_dest = 0x00; value_dest < 0x100; ++value_dest) {
    for (int value_src = 0x00; value_src < 0x100; ++value_src) {
        uint16_t pc = ((uint16_t)(state.mh << 8) | state.ml);
        state.mem[pc] = (uint8_t)opcocde;

        state.mem[0xfff0 | r8_dest] = (uint8_t)value_dest;
        state.mem[0xfff0 | r8_src] = (uint8_t)value_src;

        uint16_t pc_expected  = (uint16_t)(pc + 1);
        uint8_t dest_r8_expected = (uint8_t)(value_dest | value_src);

        for (int step = 1; step < 20; ++step)
            if (emulate_next_cycle(false, control, alu, &state)) break;

        uint16_t pc_after = (uint16_t)(state.mh << 8) | state.ml;
        uint8_t dest_r8_actual = state.mem[0xfff0 | r8_dest];
        uint8_t src_r8_actual = state.mem[0xfff0 | r8_src];

        if (pc_after != pc_expected) {
            printf("failed\n");
            fprintf(stderr, "unexpected pc, got %04x, expected %04x\n", pc_after, pc_expected);
            exit(1);
        }

        if (src_r8_actual != value_src) {
            printf("failed\n");
            fprintf(stderr, "unexpected src value, got %02x, expected %02x\n", src_r8_actual, value_src);
            exit(1);
        }

        if (dest_r8_actual != dest_r8_expected) {
            printf("failed\n");
            fprintf(stderr, "unexpected dest value, got %02x, expected %02x\n", dest_r8_actual, dest_r8_expected);
            exit(1);
        }

        if (dest_r8_actual == 0 && !(state.f & F_Z)) {
            printf("failed\n");
            fprintf(stderr, "expected zero flag to be set\n");
            exit(1);
        } else if (dest_r8_actual != 0 && (state.f & F_Z)) {
            printf("failed\n");
            fprintf(stderr, "expected zero flag to be cleared\n");
            exit(1);
        }

        if (state.f & F_C) {
            printf("failed\n");
            fprintf(stderr, "expected carry flag to be cleared\n");
            exit(1);
        }

        if ((dest_r8_actual & 0x80) && !(state.f & F_S)) {
            printf("failed\n");
            fprintf(stderr, "expected sign flag to be set\n");
            exit(1);
        } else if (!(dest_r8_actual & 0x80) && (state.f & F_S)) {
            printf("failed\n");
            fprintf(stderr, "expected sign flag to be cleared\n");
            exit(1);
        }
    }
    }

    printf("passed\n");
}

static void test_instr_testz_r8(uint8_t control[CONTROL_ROM_SIZE], uint8_t alu[ALU_ROM_SIZE], O opcocde, C r8) {
    printf("%-32s", customasm_rule_from_opcode(opcocde));

    State state = {0};
    state.f = F_I;

    for (int value_src = 0x00; value_src < 0x100; ++value_src) {
        uint16_t pc = ((uint16_t)(state.mh << 8) | state.ml);
        state.mem[pc] = (uint8_t)opcocde;

        state.mem[0xfff0 | r8] = (uint8_t)value_src;

        uint16_t pc_expected  = (uint16_t)(pc + 1);

        for (int step = 1; step < 20; ++step)
            if (emulate_next_cycle(false, control, alu, &state)) break;

        uint16_t pc_after = (uint16_t)(state.mh << 8) | state.ml;
        uint8_t r8_actual = state.mem[0xfff0 | r8];

        if (pc_after != pc_expected) {
            printf("failed\n");
            fprintf(stderr, "unexpected pc, got %04x, expected %04x\n", pc_after, pc_expected);
            exit(1);
        }

        if (r8_actual != value_src) {
            printf("failed\n");
            fprintf(stderr, "unexpected src value, got %02x, expected %02x\n", r8_actual, value_src);
            exit(1);
        }

        if (r8_actual == 0 && !(state.f & F_Z)) {
            printf("failed\n");
            fprintf(stderr, "expected zero flag to be set\n");
            exit(1);
        } else if (r8_actual != 0 && (state.f & F_Z)) {
            printf("failed\n");
            fprintf(stderr, "expected zero flag to be cleared\n");
            exit(1);
        }

        if (state.f & F_C) {
            printf("failed\n");
            fprintf(stderr, "expected carry flag to be cleared\n");
            exit(1);
        }

        if ((r8_actual & 0x80) && !(state.f & F_S)) {
            printf("failed\n");
            fprintf(stderr, "expected sign flag to be set\n");
            exit(1);
        } else if (!(r8_actual & 0x80) && (state.f & F_S)) {
            printf("failed\n");
            fprintf(stderr, "expected sign flag to be cleared\n");
            exit(1);
        }
    }

    printf("passed\n");
}

static void test_instructions(uint8_t control[CONTROL_ROM_SIZE], uint8_t alu[ALU_ROM_SIZE]) {
    test_instr_nop(control, alu);
    test_instr_inc_r8(control, alu, O_INC_A, C_A);
    test_instr_inc_r8(control, alu, O_INC_B, C_B);
    test_instr_inc_r8(control, alu, O_INC_C, C_C);
    test_instr_inc_r8(control, alu, O_INC_D, C_D);
    test_instr_out_p3_i8(control, alu, O_OUT_3_I8);
    test_instr_addc_r8_i8(control, alu, O_ADDC_B_I8, C_B);
    test_instr_ld_r8_flags(control, alu, O_LD_A_FLAGS, C_A);
    test_instr_ld_flags_r8(control, alu, O_LD_FLAGS_A, C_A);
    test_instr_shlc_r8(control, alu, O_SHLC_B, C_B);
    test_instr_shlc_r8(control, alu, O_SHLC_C, C_C);
    test_instr_shlc_r8(control, alu, O_SHLC_D, C_D);
    test_instr_shlc_r8(control, alu, O_SHLC_E, C_E);
    test_instr_or_r8_r8(control, alu, O_OR_A_B, C_A, C_B);
    test_instr_testz_r8(control, alu, O_TESTZ_B, C_B);
}
