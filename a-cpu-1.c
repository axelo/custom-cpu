#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// Instructions
#define NOP           0x00
#define LD_A_I8       0x01
#define LD_AT_I16_A   0x02
#define LD_AT_I16_AB  0x03
#define LD_AT_FFI8_A  0x04
#define LD_AT_FFI8_AB 0x05
#define TX_START_A    0x0f
#define TX_START_I8   0x10
#define TX            0x11
#define TX_STOP       0x12
#define RX_RTS        0x20
#define RX_TEST       0x21
#define RX_FIRST      0x22
#define RX            0x23
#define RX_LAST       0x24
#define INC_A         0x70
#define CMP_A_I8      0x71
#define JP_I16        0x80
#define JC_I16        0x81
#define JNZ_I16       0x84

// Rom sizes
#define ROM_SIZE_SIGNALS  (1 << 17) // 128 KB
#define ROM_SIZE_ALU      (1 << 19) // 512 KB
#define ROM_SIZE_PROGRAM  (1 << 13) // 8 KB (part of ALU)

// Signals
#define SEL_C(cn) ((cn) & 0xf) // Constant is 4 bits.
#define LD_GPO (1 << 4)
#define LD_C_  (1 << 5)
#define LD_TF  (1 << 6)
#define LD_S   (1 << 7)
#define LD_RH  (1 << 8)
#define OE_MEM (1 << 9)
#define OE_T   (1 << 10)
#define LD_T   (1 << 11)
#define LD_MEM (1 << 12)
#define LD_RL  (1 << 13)
#define INC_R  (1 << 14)
#define OE_ALU (1 << 15)

#define SEL_C_LD_I   SEL_C(1) // Assumes LD_C_ not asserted
#define SEL_C_OE_GPI SEL_C(2) // Assumes LD_C_ not asserted

#define CN (1 << 3)
#define M  (0 << 3)
#define LD_C(c) (SEL_C(c) | LD_C_)

#define S0_FETCH (OE_MEM | SEL_C_LD_I | INC_R)

#define ACTIVE_LOW_MASK (LD_GPO | LD_S | LD_C_ | LD_RL | LD_RH | LD_TF | OE_T | OE_MEM | OE_ALU)

// ALU operations
#define A_OE_PROG  (0)
#define A_NAND     (1)
#define A_ADD      (2)
#define A_ADD_F    (3)
#define A_FF       (4)
#define A_UNARY    (5)
#define A_OE_RH    (6)
#define A_OE_RL    (7)

// Constants
#define F  (0x0)
#define E  (0x1)
#define D  (0x2)
#define C  (0x3)
#define B  (0x4)
#define A  A_UNARY
#define RH A_OE_RH
#define RL A_OE_RL

static uint16_t signals_intruction(uint8_t i, uint8_t s, uint8_t tf, uint8_t end_of_copy) {
    // Copy program into mem state
    if ((tf & 8) == 0) {
        if (end_of_copy) {
            switch (s) {
            case 0:
                // Deassert all outputs as all are supposed to be active low
                return OE_ALU | LD_C(0b1111) | LD_GPO;

            case 1:
                return OE_ALU | LD_C(A_FF);

            case 2:
                // R = 0xff (will preserve end_of_copy asserted)
                    return OE_ALU | LD_RH | LD_RL;

            case 3:
                // R++ => R = 0x00
                // Load TF will transition from copy to running state (bus values doesn't matter)
                return OE_ALU | LD_TF | INC_R | LD_S;
            }

            // Should never get here in the real world
            return OE_T | LD_T;
        }

        // Copy from ALU into mem[R++], assumes C is A_OE_PROG on startup
        return OE_ALU | LD_MEM | INC_R | LD_S;
    }

    // Running state
    switch (i) {
    case NOP:
        switch (s) {
        case 0x0: return S0_FETCH;
        case 0x1:
        case 0x2:
        case 0x3:
        case 0x4:
        case 0x5:
        case 0x6:
        case 0x7:
        case 0x8:
        case 0x9:
        case 0xa:
        case 0xb:
        case 0xc:
        case 0xd:
        case 0xe:
        case 0xf: return OE_T;
        }
        assert(false);

    // case PUSH_A:
    //     switch (s) {
    //     case 0x0: return S0_FETCH;               // SEL_C(A_OE_RL) | SEL_CN | LD_C
    //     case 0x1: return OE_ALU | LD_MEM          | SEL_C(A_OE_RH) | SEL_CN | LD_C;
    //     case 0x2: return OE_ALU | LD_MEM          | SEL_C(A_FF)             | LD_C;
    //     case 0x3: return OE_ALU | (LD_RH | LD_RL);
    //     case 0x4: return OE_MEM | LD_RL           | SEL_C(C_A)     | SEL_CN | LD_C;
    //     case 0x5: return OE_MEM | LD_T                                      | LD_C | SEL_C_INC_R;
    //     case 0x6: return OE_T   | LD_MEM          | SEL_C(A_OE_RL)          | LD_C;
    //     case 0x7: return OE_ALU | LD_T            | SEL_C(A_FF)             | LD_C;
    //     case 0x8: return OE_ALU | LD_RL;
    //     case 0x9: return OE_T   | LD_MEM          | SEL_C(A_OE_RL) | SEL_CN | LD_C;
    //     case 0xa: return OE_MEM | LD_RL           | SEL_C(A_OE_RH) | SEL_CN | LD_C;
    //     case 0xb: return OE_MEM | LD_RH                                     | LD_C | LD_S;
    //     }
    //     break;

    // case PUSH_AB:
    //     switch (s) {
    //     case 0x0: return S0_FETCH;               // SEL_C(A_OE_RL) | SEL_CN | LD_C
    //     case 0x1: return OE_ALU | LD_MEM          | SEL_C(A_OE_RH) | SEL_CN | LD_C;
    //     case 0x2: return OE_ALU | LD_MEM          | SEL_C(A_FF)             | LD_C;
    //     case 0x3: return OE_ALU | (LD_RH | LD_RL);
    //     case 0x4: return OE_MEM | LD_RL           | SEL_C(C_A)     | SEL_CN | LD_C;
    //     case 0x5: return OE_MEM | LD_T                                      | LD_C | SEL_C_INC_R;
    //     case 0x6: return OE_T   | LD_MEM          | SEL_C(C_B)     | SEL_CN | LD_C;
    //     case 0x7: return OE_MEM | LD_T                                      | LD_C | SEL_C_INC_R;
    //     case 0x8: return OE_T   | LD_MEM          | SEL_C(A_OE_RL)          | LD_C;
    //     case 0x9: return OE_ALU | LD_T            | SEL_C(A_FF)             | LD_C;
    //     case 0xa: return OE_ALU | LD_RL;
    //     case 0xb: return OE_T   | LD_MEM          | SEL_C(A_OE_RL) | SEL_CN | LD_C;
    //     case 0xc: return OE_MEM | LD_RL           | SEL_C(A_OE_RH) | SEL_CN | LD_C;
    //     case 0xd: return OE_MEM | LD_RH                                     | LD_C | LD_S;
    //     }
    //     break;

    // case LD_AT_FFI8_A: // ld [0xff'i8], a
    //     switch (s) {
    //     case 0x0: return S0_FETCH;
    //     case 0x1: return OE_MEM | LD_T            | SEL_C(A_OE_RL) | SEL_CN | LD_C
    //     case 0x2: return OE_ALU | LD_MEM          | SEL_C(A_OE_RH) | SEL_CN | LD_C;
    //     case 0x3: return OE_ALU | LD_MEM;
    //     case 0x4: return OE_T   | LD_RL;          | SEL_C(A_FF)             | LD_C;
    //     case 0x5: return OE_ALU | LD_RH           | SEL_C(A_C)     | SEL_CN | LD_C;
    //     case 0x6: return OE_MEM | LD_T;                                     | LD_C;
    //     case 0x7: return OE_T   | LD_MEM          | SEL_C(A_OE_RL) | SEL_CN | LD_C;
    //     case 0x8: return OE_MEM | LD_RL           | SEL_C(A_OE_RH) | SEL_CN | LD_C;
    //     case 0x9: return OE_MEM | LD_RH                                     | LD_C | LD_S;
    //     }
    //     break;

    case LD_A_I8: // ld a, {imm:i8}
        switch (s) {
        case 0x0: return S0_FETCH;
        case 0x1: return OE_MEM | LD_T   | INC_R | LD_C(A | CN);
        case 0x2: return OE_T   | LD_MEM         | LD_C(M)      | LD_S;
        }
        break;

    case LD_AT_I16_A: // ld [i16], a => OP HH LL
        switch (s) {
        case 0x0: return S0_FETCH;
        case 0x1: return OE_MEM | LD_T   | INC_R | LD_C(RL | CN);
        case 0x2: return OE_ALU | LD_MEM         | LD_C(RH | CN);
        case 0x3: return OE_ALU | LD_MEM         | LD_C(M);
        case 0x4: return OE_MEM | LD_RL;
        case 0x5: return OE_T   | LD_RH          | LD_C(A | CN);
        case 0x6: return OE_MEM | LD_T           | LD_C(M);
        case 0x7: return OE_T   | LD_MEM         | LD_C(RL | CN);
        case 0x8: return OE_MEM | LD_RL          | LD_C(RH | CN);
        case 0x9: return OE_MEM | LD_RH          | LD_C(M);
        case 0xa: return OE_T   | INC_R | LD_S;
        }
        break;

    case LD_AT_I16_AB: // ld [i16], ab => OP HH LL
        switch (s) {
        case 0x0: return S0_FETCH;
        case 0x1: return OE_MEM | LD_T   | INC_R | LD_C(RL | CN);
        case 0x2: return OE_ALU | LD_MEM         | LD_C(RH | CN);
        case 0x3: return OE_ALU | LD_MEM         | LD_C(M);
        case 0x4: return OE_MEM | LD_RL;
        case 0x5: return OE_T   | LD_RH          | LD_C(A | CN);
        case 0x6: return OE_MEM | LD_T           | LD_C(M);
        case 0x7: return OE_T   | LD_MEM         | LD_C(B | CN);
        case 0x8: return OE_MEM | LD_T   | INC_R | LD_C(M);
        case 0x9: return OE_T   | LD_MEM         | LD_C(RL | CN);
        case 0xa: return OE_MEM | LD_RL          | LD_C(RH | CN);
        case 0xb: return OE_MEM | LD_RH          | LD_C(M);
        case 0xc: return OE_T   | INC_R | LD_S;
        }
        break;

    case LD_AT_FFI8_A:
        switch (s) {
        case 0x0: return S0_FETCH;
        case 0x1: return OE_MEM | LD_T   | INC_R | LD_C(RL | CN);
        case 0x2: return OE_ALU | LD_MEM         | LD_C(RH | CN);
        case 0x3: return OE_ALU | LD_MEM;
        case 0x4: return OE_T   | LD_RL          | LD_C(A_FF);
        case 0x5: return OE_ALU | LD_RH          | LD_C(A | CN);
        case 0x6: return OE_MEM | LD_T           | LD_C(M);
        case 0x7: return OE_T   | LD_MEM         | LD_C(RL | CN);
        case 0x8: return OE_MEM | LD_RL          | LD_C(RH | CN);
        case 0x9: return OE_MEM | LD_RH          | LD_C(M) | LD_S;
        }
        break;

    case LD_AT_FFI8_AB:
        switch (s) {
        case 0x0: return S0_FETCH;
        case 0x1: return OE_MEM | LD_T   | INC_R | LD_C(RL | CN);
        case 0x2: return OE_ALU | LD_MEM         | LD_C(RH | CN);
        case 0x3: return OE_ALU | LD_MEM;
        case 0x4: return OE_T   | LD_RL          | LD_C(A_FF);
        case 0x5: return OE_ALU | LD_RH          | LD_C(A | CN);
        case 0x6: return OE_MEM | LD_T           | LD_C(M);
        case 0x7: return OE_T   | LD_MEM         | LD_C(B | CN);
        case 0x8: return OE_MEM | LD_T           | LD_C(M);
        case 0x9: return OE_T   | LD_MEM         | LD_C(RL | CN);
        case 0xa: return OE_MEM | LD_RL          | LD_C(RH | CN);
        case 0xb: return OE_MEM | LD_RH          | LD_C(M) | LD_S;
        }
        break;

    case TX_START_A: // ld tx, a
        switch (s) {
        case 0x0: return S0_FETCH;
        case 0x1: return OE_MEM         | LD_C(A | CN);
        case 0x2: return OE_MEM | LD_T;
        case 0x3: return OE_ALU;
        case 0x4: return OE_ALU;
        case 0x5: return OE_ALU;
        case 0x6: return OE_ALU;
        case 0x7: return OE_ALU | LD_GPO | LD_C(0b1110); // Assert TX (start bit)
        case 0x8: return OE_ALU          | LD_C(M);
        case 0x9: return OE_ALU;
        case 0xa: return OE_ALU | LD_S;
        }
        break;

    case TX_START_I8: // ld tx, i8
        switch (s) {
        case 0x0: return S0_FETCH;
        case 0x1: return OE_MEM | LD_T | INC_R;
        case 0x2: return OE_ALU;
        case 0x3: return OE_ALU;
        case 0x4: return OE_ALU;
        case 0x5: return OE_ALU;
        case 0x6: return OE_ALU;
        case 0x7: return OE_ALU | LD_C(0b1110) | LD_GPO; // Assert TX (start bit)
        case 0x8: return OE_ALU | LD_C(M);
        case 0x9: return OE_ALU;
        case 0xa: return OE_ALU | LD_S;
        }
        break;

    case TX:
        switch (s) {
        case 0x0: return S0_FETCH;

        case 0x1: return OE_ALU |                  LD_C(RL | CN);
        case 0x2: return OE_ALU | LD_MEM         | LD_C(RH | CN);
        case 0x3: return OE_ALU | LD_MEM         | LD_C(A_FF);

        case 0x4: return OE_ALU | LD_RL          | LD_C(A_UNARY);
        case 0x5: return OE_T   | LD_RH | INC_R;  // 0x00 = SHR_F

        case 0x6: return OE_ALU | LD_TF | INC_R;  // 0x01 = SHR
        case 0x7: return OE_ALU | LD_T  | LD_C(0b1110 | (tf & 1)) | LD_GPO;

        case 0x8: return OE_ALU                  | LD_C(RL | CN);
        case 0x9: return OE_MEM | LD_RL          | LD_C(RH | CN);
        case 0xa: return OE_MEM | LD_RH          | LD_C(M) | LD_S;
        }
        break;

    case TX_STOP:
        switch (s) {
        case 0x0: return S0_FETCH;

        case 0x1: return OE_ALU;
        case 0x2: return OE_ALU;
        case 0x3: return OE_ALU;

        case 0x4: return OE_ALU;
        case 0x5: return OE_ALU;

        case 0x6: return OE_ALU;
        case 0x7: return OE_ALU | LD_C(0b1111) | LD_GPO; // Deassert TX

        case 0x8: return OE_ALU | LD_C(M);
        case 0x9: return OE_ALU;
        case 0xa: return OE_ALU | LD_S;
        }
        break;

    case RX_RTS:
        switch (s) {
        case 0x0: return S0_FETCH;

        case 0x1: return OE_ALU | LD_C(0b1101) | LD_GPO; // Assert RTS
        case 0x2: return OE_ALU;
        case 0x3: return OE_ALU;
        case 0x4: return OE_ALU | LD_C(0b1111) | LD_GPO; // Deassert RTS

        case 0x5: return OE_ALU | LD_C(M) | LD_S;
        }
        break;

    case RX_TEST:
        switch (s) {
        case 0x0: return S0_FETCH;

        case 0x1: return SEL_C_OE_GPI | LD_TF; // Store inputs into TF

        case 0x2: return (tf & 1)
                    ? SEL_C_OE_GPI | LD_TF
                    : OE_ALU | LD_C(M) | LD_S; // start bit received

        case 0x3: return (tf & 1)
                    ? SEL_C_OE_GPI | LD_TF
                    : OE_ALU | LD_C(M) | LD_S; // start bit received

        case 0x4: return (tf & 1)
                    ? SEL_C_OE_GPI | LD_TF
                    : OE_ALU | LD_C(M) | LD_S; // start bit received

        case 0x5: return (tf & 1)
                    ? SEL_C_OE_GPI | LD_TF
                    : OE_ALU | LD_C(M) | LD_S; // start bit received

        case 0x6: return (tf & 1)
                    ? SEL_C_OE_GPI | LD_TF
                    : OE_ALU | LD_C(M) | LD_S; // start bit received

        case 0x7: return (tf & 1)
                    ? SEL_C_OE_GPI | LD_TF
                    : OE_ALU | LD_C(M) | LD_S; // start bit received

        case 0x8: return (tf & 1)
                    ? SEL_C_OE_GPI | LD_TF
                    : OE_ALU | LD_C(M) | LD_S; // start bit received

        case 0x9: return (tf & 1)
                    ? SEL_C_OE_GPI | LD_TF
                    : OE_ALU | LD_C(M) | LD_S; // start bit received

        case 0xa: return (tf & 1)
                    ? SEL_C_OE_GPI | LD_TF
                    : OE_ALU | LD_C(M) | LD_S; // start bit received

        case 0xb: return (tf & 1)
                    ? SEL_C_OE_GPI | LD_TF
                    : OE_ALU | LD_C(M) | LD_S; // start bit received

        case 0xc: return (tf & 1)
                    ? SEL_C_OE_GPI | LD_TF
                    : OE_ALU | LD_C(M) | LD_S; // start bit received

        case 0xd: return (tf & 1)
                    ? SEL_C_OE_GPI | LD_TF
                    : OE_ALU | LD_C(M) | LD_S; // start bit received

        case 0xe: return (tf & 1)
                    ? SEL_C_OE_GPI | LD_TF
                    : OE_ALU | LD_C(M) | LD_S; // start bit received

        case 0xf: return OE_ALU | LD_C(M) | LD_S;
        }
        break;

    case RX_FIRST:
        switch (s) {
        case 0x0: return S0_FETCH;

        case 0x1: return OE_ALU;

        case 0x2: return OE_ALU |                  LD_C(RL | CN);
        case 0x3: return OE_ALU | LD_MEM         | LD_C(RH | CN);
        case 0x4: return OE_ALU | LD_MEM;

        case 0x5: return OE_T   | LD_RL | LD_RH  | LD_C(A_ADD);
        case 0x6: return OE_ALU | LD_T  | LD_RH  | LD_C(A_FF);    // T, RH = shl t, 1

        case 0x7: return OE_ALU | LD_RL          | LD_C(A_UNARY); // RL = 0xff (inc rh, 1)

        case 0x8: return SEL_C_OE_GPI | LD_TF;                    // Store inputs into TF
        case 0x9: return ((tf & 1) ? (OE_ALU | LD_T) : OE_T) | LD_C(RL | CN);
        case 0xa: return OE_MEM | LD_RL          | LD_C(RH | CN);
        case 0xb: return OE_MEM | LD_RH          | LD_C(M) | LD_S;
        }
        break;

    case RX:
        switch (s) {
        case 0x0: return S0_FETCH;

        case 0x1: return OE_ALU |                  LD_C(RL | CN);
        case 0x2: return OE_ALU | LD_MEM         | LD_C(RH | CN);
        case 0x3: return OE_ALU | LD_MEM;

        case 0x4: return OE_T   | LD_RL | LD_RH  | LD_C(A_ADD);
        case 0x5: return OE_ALU | LD_T  | LD_RH  | LD_C(A_FF);    // T, RH = shl t, 1

        case 0x6: return OE_ALU | LD_RL          | LD_C(A_UNARY); // RL = 0xff (inc rh, 1)

        case 0x7: return SEL_C_OE_GPI | LD_TF;                    // Store inputs into TF
        case 0x8: return ((tf & 1) ? (OE_ALU | LD_T) : OE_T) | LD_C(RL | CN);
        case 0x9: return OE_MEM | LD_RL          | LD_C(RH | CN);
        case 0xa: return OE_MEM | LD_RH          | LD_C(M) | LD_S;
        }
        break;

    case RX_LAST: // ld a, rx
        switch (s) {
        case 0x0: return S0_FETCH;
        case 0x1: return OE_ALU                  | LD_C(RL | CN);
        case 0x2: return OE_ALU | LD_MEM         | LD_C(RH | CN);
        case 0x3: return OE_ALU | LD_MEM;

        case 0x4: return OE_T   | LD_RL | LD_RH  | LD_C(A_ADD);
        case 0x5: return OE_ALU | LD_T  | LD_RH  | LD_C(A_FF);    // T, RH = shl t, 1

        case 0x6: return OE_ALU | LD_RL          | LD_C(A_UNARY); // RL = 0xff (inc rh, 1)

        case 0x7: return SEL_C_OE_GPI | LD_TF;                    // Store inputs into TF
        case 0x8: return ((tf & 1) ? (OE_ALU | LD_T) : OE_T) | LD_C(A_FF);

        case 0x9: return OE_ALU | LD_RL | LD_RH  | LD_C(A_ADD);
        case 0xa: return OE_ALU | LD_RL;                          // RL = 0xfe (reverse bits rh)
        case 0xb: return OE_T   | LD_RH          | LD_C(A  | CN); // RH = T, assumes A = A_UNARY
        case 0xc: return OE_ALU | LD_MEM         | LD_C(RL | CN); // mem[A] = revers bits of T
        case 0xd: return OE_MEM | LD_RL          | LD_C(RH | CN);
        case 0xe: return OE_MEM | LD_RH          | LD_C(M) | LD_S;
        }
        break;

    case JP_I16:
        switch (s) {
        case 0x0: return S0_FETCH;
        case 0x1: return OE_MEM | LD_T  | INC_R;
        case 0x2: return OE_MEM | LD_RL | INC_R;
        case 0x3: return OE_T   | LD_RH | LD_S;
        }
        break;

    case JC_I16: {
        if (tf & 1) {
            switch (s) {
            case 0x0: return S0_FETCH;
            case 0x1: return OE_MEM | LD_T  | INC_R;
            case 0x2: return OE_MEM | LD_RL | INC_R;
            case 0x3: return OE_T   | LD_RH | LD_S;
            }
        } else {
            switch (s) {
            case 0x0: return S0_FETCH;
            case 0x1: return OE_T | INC_R;
            case 0x2: return OE_T | INC_R | LD_S;
            }
        }
    } break;

    case JNZ_I16: {
        if (!(tf & 2)) {
            switch (s) {
            case 0x0: return S0_FETCH;
            case 0x1: return OE_MEM | LD_T  | INC_R;
            case 0x2: return OE_MEM | LD_RL | INC_R;
            case 0x3: return OE_T   | LD_RH | LD_S;
            }
        } else {
            switch (s) {
            case 0x0: return S0_FETCH;
            case 0x1: return OE_T | INC_R;
            case 0x2: return OE_T | INC_R | LD_S;
            }
        }
    } break;

    case INC_A:
        switch (s) {
        case 0x0: return S0_FETCH;
        case 0x1: return OE_ALU                  | LD_C(RL | CN);
        case 0x2: return OE_ALU | LD_MEM         | LD_C(A  | CN);
        case 0x3: return OE_MEM | LD_RL;
        case 0x4: return OE_ALU | INC_R          | LD_C(A_OE_RL);
        case 0x5: return OE_ALU | LD_T           | LD_C(A  | CN);
        case 0x6: return OE_T   | LD_MEM         | LD_C(RL | CN);
        case 0x7: return OE_MEM | LD_RL          | LD_C(M) | LD_S;
        }
        break;

    case CMP_A_I8:
        switch (s) {
        case 0x0: return S0_FETCH;
        case 0x1: return OE_MEM | LD_T  | INC_R  | LD_C(RL | CN);
        case 0x2: return OE_ALU | LD_MEM         | LD_C(RH | CN);
        case 0x3: return OE_ALU | LD_MEM;
        case 0x4: return OE_T   | LD_RL | LD_RH  | LD_C(A_NAND);
        case 0x5: return OE_ALU | LD_RL          | LD_C(A  | CN); // RL = ~I8
        case 0x6: return OE_MEM | LD_RH | INC_R  | LD_C(A_ADD_F); // RH = A, RL = ~I8 + 1, 2's of I8
        case 0x7: return OE_ALU | LD_TF          | LD_C(RL | CN); // Store flags in TF
        case 0x8: return OE_MEM | LD_RL          | LD_C(RH | CN);
        case 0x9: return OE_MEM | LD_RH          | LD_C(M) | LD_S;
        }
        break;

    }

    return s == 0
        ? S0_FETCH
        : OE_T | LD_T;

    /*switch (s) {
    case 0x0: return S0_FETCH;
    case 0x1: return OE_T;
    case 0x2: return OE_T;
    case 0x3: return OE_T;
    case 0x4: return OE_ALU;
    case 0x5: return OE_ALU;
    case 0x6: return OE_ALU;
    case 0x7: return OE_MEM;
    case 0x8: return OE_MEM;
    case 0x9: return OE_MEM;
    case 0xa: return SEL_C_OE_GPI;
    case 0xb: return SEL_C_OE_GPI;
    case 0xc: return SEL_C_OE_GPI;
    case 0xd: return 0;
    case 0xe: return 0;
    case 0xf: return LD_S;
    }
    return OE_T | LD_T;*/
}

static uint8_t signals_alu(uint8_t rom_program[ROM_SIZE_PROGRAM], uint8_t rl, uint8_t rh, uint8_t op) {
    switch (op) {
    case A_OE_PROG:
        return rom_program[(rh << 8 | rl) & (ROM_SIZE_PROGRAM - 1)];

    case A_NAND:
        return (~(rl & rh)) & 0xff;

    case A_ADD:
        return (rl + rh) & 0xff;

    case A_ADD_F: {
        bool carry = (rl + rh) > 0xff;
        bool zero  = ((rl + rh) & 0xff) == 0;

        return (zero ? 2 : 0)  | (carry ? 1 : 0);
    }

    case A_FF:
        return 0xff;

    case A_UNARY:
        switch (rl) {
        case 0x00:{
            // carry and zero flags of: shr rh, 1
            bool carry = rh & 1;
            bool zero  = ((rh >> 1) == 0);

            return (zero ? 2 : 0)  | (carry ? 1 : 0);
        }

        case 0x01:
            // shr rh, 1
            return rh >> 1;

        case 0xfe:
            // flip rh
            return (uint8_t)(
                (((rh >> 0) & 1) << 7) |
                (((rh >> 1) & 1) << 6) |
                (((rh >> 2) & 1) << 5) |
                (((rh >> 3) & 1) << 4) |
                (((rh >> 4) & 1) << 3) |
                (((rh >> 5) & 1) << 2) |
                (((rh >> 6) & 1) << 1) |
                (((rh >> 7) & 1) << 0));

        case 0xff:
            // inc rh
            return (rh + 1) & 0xff;
        }

        return 0xab;

    case A_OE_RH:
        return rh;

    case A_OE_RL:
        return rl;
    }

    assert(false);
}

static bool write_rom(size_t size, uint8_t rom[size], const char *filename) {
    FILE *file = fopen(filename, "w");

    if (file == NULL) {
        fprintf(stderr, "Failed to open %s for writing\n", filename);
        return false;
    }

    size_t write_result = fwrite(rom, size, 1, file);

    if (write_result != 1) {
        fprintf(stderr, "Failed to write rom to file %s\n", filename);
        return false;
    }

    if (fclose(file) != 0) {
        fprintf(stderr, "Failed to close file %s\n", filename);
        return false;
    }

    return true;
}

int main(void) {
    uint8_t rom_program[ROM_SIZE_PROGRAM] = {
    /* 0x00 */ 0x00, 0x01, 0x45, 0x0f, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x12, 0x20, 0x21, 0x81,
    /* 0x10 */ 0x00, 0x0d, 0x22, 0x23, 0x23, 0x23, 0x23, 0x23, 0x23, 0x24, 0x0f, 0x11, 0x11, 0x11, 0x11, 0x11,
    /* 0x20 */ 0x11, 0x11, 0x11, 0x12, 0x80, 0x00, 0x0d
    };

    // for (uint16_t i = 0; i < ROM_SIZE_PROGRAM; ++i) {
    //     rom_program[i] = (i + 1) & 0xff;
    // }

    // bool going_left = false;
    // for (uint16_t i = 0; i < ROM_SIZE_PROGRAM; ++i) {
    //     if ((i & 7) == 0) going_left = !going_left;
    //     rom_program[i] = going_left ? (uint8_t)(1 << (i & 7)) : (uint8_t)(1 << (7 - (i & 7)));
    // }

    uint8_t rom_alu[ROM_SIZE_ALU];

    for (uint32_t input = 0; input < ROM_SIZE_ALU; ++input) {
        uint8_t rl = input & 0xff;
        uint8_t rh = (input >> 8) & 0xff;
        uint8_t op = (input >> 16) & 0x7;

        rom_alu[input] = signals_alu(rom_program, rl, rh, op);
    }

    uint8_t rom_signals0[ROM_SIZE_SIGNALS];
    uint8_t rom_signals1[ROM_SIZE_SIGNALS];

    for (uint32_t input = 0; input < ROM_SIZE_SIGNALS; ++input) {
        uint8_t i  = input & 0xff;
        uint8_t s  = (input >> 8) & 0xf;
        uint8_t tf = (input >> 12) & 0xf;
        uint8_t end_of_copy = (input >> 16) & 1;

        uint16_t signals = signals_intruction(i, s, tf, end_of_copy) ^ ACTIVE_LOW_MASK;

        rom_signals0[input] = signals & 0xff;
        rom_signals1[input] = (signals >> 8) & 0xff;
    }

    assert(write_rom(ROM_SIZE_ALU,     rom_alu,      "rom_alu.bin"));
    assert(write_rom(ROM_SIZE_SIGNALS, rom_signals0, "rom_signals0.bin"));
    assert(write_rom(ROM_SIZE_SIGNALS, rom_signals1, "rom_signals1.bin"));

    return 0;
}
