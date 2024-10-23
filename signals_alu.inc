static inline uint8_t signals_alu(uint8_t rom_boot[ROM_SIZE_BOOT], uint8_t ml, uint8_t mh, A alu_op) {
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
