#include <stdbool.h>

#include "control_roms.h"

typedef struct {
    uint8_t o;
    uint8_t s;
    uint8_t f;
    uint8_t c;
    uint8_t t;
    uint8_t ml;
    uint8_t mh;
    uint8_t mem[0x10000];
} State;

#define IS_LD_O(signals)  (((signals) & S_C0) && !((signals) & LD_C))
#define IS_LD_S(signals)  (((signals) & S_C1) && !((signals) & LD_C))
#define IS_LD_IO(signals) (((signals) & S_C2) && !((signals) & LD_C))

static const char* EMULATE_C_REG_NAME[8] = {
    "A   ", // 0x0
    "B   ", // 0x1
    "C   ", // 0x2
    "D   ", // 0x3
    "E   ", // 0x4
    "T   ", // 0x5
    "T_ML", // 0x6
    "T_MH", // 0x7
};

static void emulate_print_control_signals(uint8_t s, uint16_t signals) {
    char buf[8 * 1024];
    buf[0] = 0;

    char* pbuf = buf;

    if (signals & OE_MEM)  pbuf += sprintf(pbuf, "| OE_MEM    ");
    if (signals & OE_T)    pbuf += sprintf(pbuf, "| OE_T      ");
    if (signals & OE_IO)   pbuf += sprintf(pbuf, "| OE_IO     ");
    if (signals & OE_C)    pbuf += sprintf(pbuf, "| OE_C      ");
    if (signals & OE_ALU)  pbuf += sprintf(pbuf, "| OE_ALU    ");

    if (IS_LD_O(signals))  pbuf += sprintf(pbuf, "| LD_O      ");
    if (IS_LD_S(signals))  pbuf += sprintf(pbuf, "| LD_S      ");
    if (IS_LD_IO(signals)) pbuf += sprintf(pbuf, "| LD_IO     ");
    if (signals & LD_MH)   pbuf += sprintf(pbuf, "| LD_MH     ");
    if (signals & LD_ML)   pbuf += sprintf(pbuf, "| LD_ML     ");
    if (signals & LD_T)    pbuf += sprintf(pbuf, "| LD_T      ");
    if (signals & LD_MEM)  pbuf += sprintf(pbuf, "| LD_MEM    ");
    if (signals & LD_F)    pbuf += sprintf(pbuf, "| LD_F      ");

    if (signals & LD_C) {
        uint8_t s_c =
            ((signals & S_C2) ? 4 : 0) |
            ((signals & S_C1) ? 2 : 0) |
            ((signals & S_C0) ? 1 : 0);

        pbuf += sprintf(pbuf, "| C_%s    ", EMULATE_C_REG_NAME[s_c]);

        pbuf += sprintf(pbuf, (signals & SEL_C)
                            ? "| SEL_C     "
                            : "| SEL_M     ");

        pbuf += sprintf(pbuf, "| LD_C      ");
    }

    if (signals & INC_M)  pbuf += sprintf(pbuf, "| INC_M     ");

    printf("0x%x: %s\n", s, buf);
}

static bool emulate_next_cycle(
    bool print_debug_info,
    uint8_t control[CONTROL_ROM_SIZE],
    uint8_t alu[ALU_ROM_SIZE],
    State *state) {

    if (print_debug_info) printf("opcode: %02x\n", state->o);

    uint16_t control_address = (uint16_t)((state->f << 12) | (state->s << 8) | state->o);

    uint16_t control_signals = (uint16_t)(
            (control[(1 << 16) | control_address] << 8) |
             control[control_address]
        ) ^ S_ACTIVE_LOW_MASK;

    if (print_debug_info) emulate_print_control_signals(state->s, control_signals);

    uint8_t data_bus = 0xab;

    uint16_t mem_bus =
        (state->c & 0x8)
            ? (0xfff0 | (state->c & 0x7))
            : (uint16_t)((state->mh << 8) | state->ml);

    uint32_t alu_bus = (uint32_t)(
        ((state->c & 0x7) << 16) |
               (state->mh << 8)  |
                state->ml);

    bool oe_mem = control_signals & OE_MEM;
    bool oe_t   = control_signals & OE_T;
    bool oe_io  = control_signals & OE_IO;
    bool oe_c   = control_signals & OE_C;
    bool oe_alu = control_signals & OE_ALU;

    if      (oe_mem) data_bus = state->mem[mem_bus];
    else if (oe_t)   data_bus = state->t;
    else if (oe_io)  data_bus = 0x00; // TODO
    else if (oe_c)   data_bus = (0xf8 * ((state->c >> 2) & 1)) | (state->c & 0x7);
    else if (oe_alu) data_bus = alu[alu_bus];

    int n_oe = (oe_mem ? 1 : 0)
             + (oe_t   ? 1 : 0)
             + (oe_io  ? 1 : 0)
             + (oe_c   ? 1 : 0)
             + (oe_alu ? 1 : 0);

    if (n_oe == 0) {
        fprintf(stderr, "no output enabled");
        exit(1);
    } else if (n_oe > 1) {
        fprintf(stderr, "more than one output enabled");
        exit(1);
    }

    bool ld_o   = IS_LD_O(control_signals);
    bool ld_io  = IS_LD_IO(control_signals);
    bool ld_s   = IS_LD_S(control_signals);
    bool ld_ml  = control_signals & LD_ML;
    bool ld_mh  = control_signals & LD_MH;
    bool ld_t   = control_signals & LD_T;
    bool ld_mem = control_signals & LD_MEM;
    bool ld_f   = control_signals & LD_F;
    bool ld_c   = control_signals & LD_C;

    if (ld_o)   state->o = data_bus;
    if (ld_io)  {}; // TODO
    if (ld_ml)  state->ml = data_bus;
    if (ld_mh)  state->mh = data_bus;
    if (ld_t)   state->t  = data_bus;
    if (ld_mem) state->mem[mem_bus] = data_bus;
    if (ld_f)   state->f  = data_bus & 0x0f;
    if (ld_c)   state->c  = ((control_signals & SEL_C) ? 8 : 0) |
                            ((control_signals & S_C2)  ? 4 : 0) |
                            ((control_signals & S_C1)  ? 2 : 0) |
                            ((control_signals & S_C0)  ? 1 : 0);

    bool inc_m = control_signals & INC_M;

    if (inc_m & !ld_ml) {
        state->ml = (uint8_t)(state->ml + 1);
        if ((state->ml == 0) && !ld_mh) state->mh = (uint8_t)(state->mh + 1);
    }

    if (ld_s) {
        state->s = 0;
        return true;
    } else {
        ++state->s;

        if (state->s > 15) {
            state->s = 0;
            return true;
        } else
            return false;
    }
}
