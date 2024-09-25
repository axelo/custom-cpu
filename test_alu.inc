static int test_alu(uint8_t rom_alu[ROM_SIZE_ALU]) {
    for (int operation = 0; operation < 8; ++operation) {
        int op_index = operation << 8 << 8;

        switch ((A)operation) {
        case A_BOOT: break;

        case A_UNARY: break; // TODO

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
                    uint8_t expected_output = (uint8_t) (ml + mh);

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

                    uint8_t expected_zf = ((uint8_t)(ml + mh) == 0 ? 1 : 00);
                    uint8_t expected_cf = ((ml + mh) > 0xff ? 1 : 00);
                    uint8_t expected_of = 0; // TODO
                    uint8_t expected_sf = (uint8_t)(ml + mh) >> 7;
                    uint8_t expected_output = (uint8_t)(
                        (expected_sf << 3) |
                        (expected_of << 2) |
                        (expected_cf << 1) |
                        (expected_zf << 0));

                    if (actual_output != expected_output) {
                        fprintf(stderr, "flags (0x%02x + 0x%02x) => expected: 0x%02x, actual: 0x%02x\n", ml, mh, expected_output, actual_output);
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

                    uint8_t expected_zf = ((uint8_t)q == 0 ? 1 : 00);
                    uint8_t expected_cf = (q > 0xff ? 1 : 00);
                    uint8_t expected_of = 0; // TODO
                    uint8_t expected_sf = (uint8_t)q >> 7;
                    uint8_t expected_output = (uint8_t)(
                        (expected_sf << 3) |
                        (expected_of << 2) |
                        (expected_cf << 1) |
                        (expected_zf << 0));

                    if (actual_output != expected_output) {
                        fprintf(stderr, "flags (0x%02x + 0x%02x + 1) => expected: 0x%02x, actual: 0x%02x\n", ml, mh, expected_output, actual_output);
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
