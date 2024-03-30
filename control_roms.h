#ifndef CONTROL_ROMS_H
#define CONTROL_ROMS_H

typedef enum {
    LD_MH  = 1 << 0,
    LD_ML  = 1 << 1,
    INC_M  = 1 << 2,
    OE_MEM = 1 << 3,
    OE_T   = 1 << 4,
    OE_IO  = 1 << 5,
    LD_T   = 1 << 6,
    LD_MEM = 1 << 7,
    LD_F   = 1 << 8,
    OE_C   = 1 << 9,
    OE_ALU = 1 << 10,
    LD_C   = 1 << 11,
    S_C0   = 1 << 12,
    S_C1   = 1 << 13,
    S_C2   = 1 << 14,
    SEL_M  = 0 << 15,
    SEL_C  = 1 << 15,
} S;

#define S_ACTIVE_LOW_MASK (LD_C | LD_F | LD_MEM | LD_ML | LD_MH | OE_ALU | OE_C | OE_MEM | OE_T)

typedef enum {
    F_Z = 1 << 0, // Zero
    F_C = 1 << 1, // Carry
    F_S = 1 << 2, // Sign
    F_I = 1 << 3, // Init, active low
} F;

#define CONTROL_ROM_SIZE (1 << 17)
#define ALU_ROM_SIZE     (1 << 19)
#define BOOT_ROM_SIZE    0x1000

#endif
