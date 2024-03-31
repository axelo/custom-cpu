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
    printf("%-16s", customasm_rule_from_opcode(O_OUT_0_A));

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

static void test_instructions(uint8_t control[CONTROL_ROM_SIZE], uint8_t alu[ALU_ROM_SIZE]) {
    test_instr_nop(control, alu);
    test_instr_inc_r8(control, alu, O_INC_A, C_A);
    test_instr_inc_r8(control, alu, O_INC_B, C_B);
    test_instr_inc_r8(control, alu, O_INC_C, C_C);
    test_instr_inc_r8(control, alu, O_INC_D, C_D);
    test_instr_out_p3_i8(control, alu, O_OUT_3_I8);
}
