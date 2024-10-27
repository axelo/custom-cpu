static inline uint8_t signals_alu(const uint8_t rom_boot[ROM_SIZE_BOOT], uint8_t ml, uint8_t mh, A alu_op) {
    switch (alu_op) {
    case A_BOOT: {
        uint16_t address = (uint16_t)((mh << 8) | ml);
        return address >= ROM_SIZE_BOOT ? 0x00 : rom_boot[address];
    }

    case A_UNARY: {
        switch ((AU)ml) {
        case AU_SHR_F: {
            uint16_t q = mh >> 1;
            uint8_t cf = mh & 1;
            uint8_t zf = (q & 0xff) == 0;
            uint8_t of = 0;
            uint8_t sf = (q >> 7) & 1;

            uint8_t flags = (uint8_t)~((sf << 7) | (sf << 3) | (of << 2) | (zf << 1) | (cf << 0));

            return flags;
        }

        case AU_1S_LSB: return 0xfe | (mh & 1);

        case AU_SHR: return (uint8_t)(mh >> 1);

        case AU_SHR_OR_00: return (uint8_t)(mh >> 1) | 0x00;
        case AU_SHR_OR_80: return (uint8_t)(mh >> 1) | 0x80;
        }
    } break;

    case A_NAND: return (uint8_t)~(ml & mh);

    case A_ADD:  return (uint8_t)(ml + mh);

    case A_ADD_F: {
        uint16_t q = ml + mh;
        uint8_t cf = q > 0xff;
        uint8_t zf = (q & 0xff) == 0;
        uint8_t of = (
            ((int8_t)ml >= 0 && (int8_t)mh >= 0 && (int8_t)q < 0) ||
            ((int8_t)ml < 0  && (int8_t)mh < 0  && (int8_t)q >= 0))
            ? 1 : 0;
        uint8_t sf = ((q & 0xff) >> 7) & 1;

        uint8_t flags = (uint8_t)~((sf << 7) | (sf << 3) | (of << 2) | (zf << 1) | (cf << 0));

        return flags;
    }

    case A_ADD_F_CF: {
        uint16_t q = ((ml - 1) & 0xff) + mh + 1;
        uint8_t cf = q > 0xff;
        uint8_t zf = (q & 0xff) == 0;
        uint8_t of = (
            ((int8_t)ml >= 0 && (int8_t)mh >= 0 && (int8_t)q < 0) ||
            ((int8_t)ml < 0  && (int8_t)mh < 0  && (int8_t)q >= 0))
            ? 1 : 0;
        uint8_t sf = ((q & 0xff) >> 7) & 1;

        uint8_t flags = (uint8_t)~((sf << 7) | (sf << 3) | (of << 2) | (zf << 1) | (cf << 0));

        return flags;
    }

    case A_OE_ML: return (uint8_t)ml;

    case A_OE_MH: return (uint8_t)mh;

    default: assert(0 && "invalid alu operation");
    }
}

static int test_alu(uint8_t rom_alu[ROM_SIZE_ALU]) {
    for (int operation = 0; operation < 8; ++operation) {
        int op_index = operation << 8 << 8;

        switch ((A)operation) {
        case A_BOOT: {
            for (int ml = 0; ml < 0x100; ++ml) {
                for (int mh = 0; mh < 0x100; ++mh) {
                    int m = (mh << 8) | ml;

                    if (m >= ROM_SIZE_BOOT) {
                        int rom_index = op_index | m;
                        uint8_t actual_output = rom_alu[rom_index];

                        if (actual_output != 0x00) {
                            fprintf(stderr, "AU_BOOT when M >= ROM_SIZE_BOOT (0x%04x) => expected: 0x00, actual: 0x%02x\n", m, actual_output);
                            return 1;
                        }
                    }
                }
            }
        } break;

        case A_UNARY: {
            for (int ml = 0; ml < 0x100; ++ml) {
                for (int mh = 0; mh < 0x100; ++mh) {
                    int rom_index = op_index | (mh << 8) | ml;

                    uint8_t actual_output = rom_alu[rom_index];

                    switch ((AU)ml) {
                    case AU_SHR_F: {
                        uint8_t expected_zf = ((uint8_t)(mh >> 1) == 0 ? 1 : 0);
                        uint8_t expected_cf = mh & 1;
                        uint8_t expected_of = 0;
                        uint8_t expected_sf = (uint8_t)(mh >> 1) >> 7;
                        uint8_t expected_output = (uint8_t)~(
                            (expected_sf << 7) |
                            (expected_sf << 3) |
                            (expected_of << 2) |
                            (expected_zf << 1) |
                            (expected_cf << 0));

                        if (actual_output != expected_output) {
                            fprintf(stderr, "flags (0x%02x shr 1) => expected: 0x%02x, actual: 0x%02x\n", mh, expected_output, actual_output);
                            return 1;
                        }

                        if (actual_output == 0) {
                            fprintf(stderr, "flags (0x%02x shr 1) should never be all zero\n", mh);
                            return 1;
                        }
                    } break;

                    case AU_1S_LSB: {
                        uint8_t expected_output = 0xfe | (mh & 1);

                        if (actual_output != expected_output) {
                            fprintf(stderr, "0x%02x 1s lsb => expected: 0x%02x, actual: 0x%02x\n", mh, expected_output, actual_output);
                            return 1;
                        }
                    } break;

                    case AU_SHR: {
                        uint8_t expected_output = (uint8_t)(mh >> 1);

                        if (actual_output != expected_output) {
                            fprintf(stderr, "0x%02x shr 1 => expected: 0x%02x, actual: 0x%02x\n", mh, expected_output, actual_output);
                            return 1;
                        }
                    } break;

                    case AU_SHR_OR_00: {
                        uint8_t expected_output = (uint8_t)(mh >> 1);

                        if (actual_output != expected_output) {
                            fprintf(stderr, "0x%02x shr or 0x00 => expected: 0x%02x, actual: 0x%02x\n", mh, expected_output, actual_output);
                            return 1;
                        }
                    } break;

                    case AU_SHR_OR_80: {
                        uint8_t expected_output = (uint8_t)(mh >> 1) | 0x80;

                        if (actual_output != expected_output) {
                            fprintf(stderr, "0x%02x shr or 0x80 => expected: 0x%02x, actual: 0x%02x\n", mh, expected_output, actual_output);
                            return 1;
                        }
                    } break;
                    }
                }
            }
        } break;

        case A_NAND: {
            for (int ml = 0; ml < 0x100; ++ml) {
                for (int mh = 0; mh < 0x100; ++mh) {
                    int rom_index = op_index | (mh << 8) | ml;

                    uint8_t actual_output = rom_alu[rom_index];
                    uint8_t expected_output = (uint8_t) ~(ml & mh);

                    if (actual_output != expected_output) {
                        fprintf(stderr, "0x%02x nand 0x%02x => expected: 0x%02x, actual: 0x%02x\n", ml, mh, expected_output, actual_output);
                        return 1;
                    }
                }
            }
        }
        break;

        case A_ADD: {
            for (int ml = 0; ml < 0x100; ++ml) {
                for (int mh = 0; mh < 0x100; ++mh) {
                    int rom_index = op_index | (mh << 8) | ml;

                    uint8_t actual_output = rom_alu[rom_index];
                    uint8_t expected_output = (uint8_t)(ml + mh);

                    if (actual_output != expected_output) {
                        fprintf(stderr, "0x%02x + 0x%02x => expected: 0x%02x, actual: 0x%02x\n", ml, mh, expected_output, actual_output);
                        return 1;
                    }
                }
            }
        }
        break;

        case A_ADD_F: {
            for (int ml = 0; ml < 0x100; ++ml) {
                for (int mh = 0; mh < 0x100; ++mh) {
                    int rom_index = op_index | (mh << 8) | ml;

                    uint8_t actual_output = rom_alu[rom_index];

                    uint16_t q = (uint16_t)(ml + mh);

                    uint8_t expected_cf = q > 0xff ? 1 : 0;

                    uint8_t expected_zf = (uint8_t)q == 0 ? 1 : 0;

                    // A negative sum of positive operands (or vice versa) is an overflow.
                    uint8_t expected_of = (
                        ((int8_t)ml >= 0 && (int8_t)mh >= 0 && (int8_t)q < 0) ||
                        ((int8_t)ml < 0  && (int8_t)mh < 0  && (int8_t)q >= 0))
                        ? 1 : 0;

                    uint8_t expected_sf = (q >> 7) & 1;

                    uint8_t expected_output = (uint8_t)~(
                        (expected_sf << 7) |
                        (expected_sf << 3) |
                        (expected_of << 2) |
                        (expected_zf << 1) |
                        (expected_cf << 0));

                    if (actual_output != expected_output) {
                        fprintf(stderr, "flags (0x%02x + 0x%02x) => expected: 0x%02x, actual: 0x%02x\n", ml, mh, expected_output, actual_output);
                        return 1;
                    }

                    if (actual_output == 0) {
                        fprintf(stderr, "flags should never be all zero\n");
                        return 1;
                    }
                }
            }
        }
        break;

        case A_ADD_F_CF: { // set flags based on incoming cf set and ml has already been inc by one
            for (int ml = 0; ml < 0x100; ++ml) {
                for (int mh = 0; mh < 0x100; ++mh) {
                    int rom_index = op_index | (mh << 8) | ml;

                    uint8_t actual_output = rom_alu[rom_index];

                    uint16_t q = (uint16_t)(((ml - 1) & 0xff) + mh + 1);

                    uint8_t expected_cf = q > 0xff ? 1 : 0;

                    uint8_t expected_zf = (uint8_t)q == 0 ? 1 : 0;

                    // A negative sum of positive operands (or vice versa) is an overflow.
                   uint8_t expected_of = (
                        ((int8_t)ml >= 0 && (int8_t)mh >= 0 && (int8_t)q < 0) ||
                        ((int8_t)ml < 0  && (int8_t)mh < 0  && (int8_t)q >= 0))
                        ? 1 : 0;

                    uint8_t expected_sf = (q >> 7) & 1;

                    uint8_t expected_output = (uint8_t)~(
                        (expected_sf << 7) |
                        (expected_sf << 3) |
                        (expected_of << 2) |
                        (expected_zf << 1) |
                        (expected_cf << 0));

                    if (actual_output != expected_output) {
                        fprintf(stderr, "flags (0x%02x + 0x%02x + 1) => expected: 0x%02x, actual: 0x%02x\n", ml, mh, expected_output, actual_output);
                        return 1;
                    }

                    if (actual_output == 0) {
                        fprintf(stderr, "flags should never be all zero\n");
                        return 1;
                    }
                }
            }
        }
        break;

        case A_OE_MH: {
            for (int ml = 0; ml < 0x100; ++ml) {
                for (int mh = 0; mh < 0x100; ++mh) {

                    int rom_index = op_index | (mh << 8) | ml;

                    uint8_t actual_output = rom_alu[rom_index];
                    uint8_t expected_output = (uint8_t) mh;

                    if (actual_output != expected_output) {
                        fprintf(stderr, "oe 0x%02x => expected: 0x%02x, actual: 0x%02x\n", mh, expected_output, actual_output);
                        return 1;
                    }

                }
            }
        }
        break;

        case A_OE_ML: {
            for (int ml = 0; ml < 0x100; ++ml) {
                for (int mh = 0; mh < 0x100; ++mh) {

                    int rom_index = op_index | (mh << 8) | ml;

                    uint8_t actual_output = rom_alu[rom_index];
                    uint8_t expected_output = (uint8_t) ml;

                    if (actual_output != expected_output) {
                        fprintf(stderr, "oe 0x%02x => expected: 0x%02x, actual: 0x%02x\n", ml, expected_output, actual_output);
                        return 1;
                    }
                }
            }
        }
        break;

        default: assert(0);
        }
    }

    return 0;
}
