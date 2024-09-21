static size_t inline customasm_ruledef_0(size_t size, char ruledef[size], size_t n, int o, const char *mnemonic) {
    n += (size_t) snprintf(ruledef + n, size, "%s%s%s%02x\n", "    ", mnemonic, " => 0x", o);
    assert(n < size);

    return n;
}

// static size_t inline customasm_ruledef_i8(size_t size, char ruledef[size], size_t n, int o, const char *mnemonic) {
//     n += (size_t) snprintf(ruledef + n, size, "%s%s, {i:i8}%s%02x @ i\n", "    ", mnemonic, " => 0x", o);
//     assert(n < size);

//     return n;
// }

// static size_t inline customasm_ruledef_i16(size_t size, char ruledef[size], size_t n, int o, const char *mnemonic, const char *around_l, const char *around_r) {
//     n += (size_t) snprintf(ruledef + n, size, "%s%s, %s{i:i16}%s%s%02x @ i\n", "    ", mnemonic, around_l, around_r, " => 0x", o);
//     assert(n < size);

//     return n;
// }

static size_t inline customasm_ruledef_imm(size_t size, char ruledef[size], size_t n, int o, const char *mnemonic) {
    n += (size_t) snprintf(ruledef + n, size, "%s%s%s%02x @ imm\n", "    ", mnemonic, " => 0x", o);
    assert(n < size);

    return n;
}

static size_t customasm_ruledef(size_t size, char ruledef[size]) {
    static const char *start = ""
"#bankdef memory {\n"
"    #bits     8\n"
"    #addr     0\n"
"    #addr_end 0x10000\n"
"    #outp     0\n"
"}\n"
"\n"
"#ruledef instructions\n{\n";

    static const char *end = "}\n";

    size_t n = 0;

    n += strlcat(ruledef + n, start, size);
    assert(n < size);

    for (int o = 0; o < 0x100; ++o) {
        switch ((O)o) {
        case RESET: n = customasm_ruledef_0(size, ruledef, n, o, "reset"); break;
        case NOP: n = customasm_ruledef_0(size, ruledef, n, o, "nop"); break;

        case LD_A_I8: n = customasm_ruledef_imm(size, ruledef, n, o, "ld a, {imm:i8}"); break;
        case LD_B_I8: n = customasm_ruledef_imm(size, ruledef, n, o, "ld b, {imm:i8}"); break;
        case LD_C_I8: n = customasm_ruledef_imm(size, ruledef, n, o, "ld c, {imm:i8}"); break;
        case LD_D_I8: n = customasm_ruledef_imm(size, ruledef, n, o, "ld d, {imm:i8}"); break;

        case LD_A_B: n = customasm_ruledef_0(size, ruledef, n, o, "ld a, b"); break;
        case LD_A_C: n = customasm_ruledef_0(size, ruledef, n, o, "ld a, c"); break;
        case LD_A_D: n = customasm_ruledef_0(size, ruledef, n, o, "ld a, d"); break;

        case LD_B_A: n = customasm_ruledef_0(size, ruledef, n, o, "ld b, a"); break;
        case LD_B_C: n = customasm_ruledef_0(size, ruledef, n, o, "ld b, c"); break;
        case LD_B_D: n = customasm_ruledef_0(size, ruledef, n, o, "ld b, d"); break;

        case LD_C_A: n = customasm_ruledef_0(size, ruledef, n, o, "ld c, a"); break;
        case LD_C_B: n = customasm_ruledef_0(size, ruledef, n, o, "ld c, b"); break;
        case LD_C_D: n = customasm_ruledef_0(size, ruledef, n, o, "ld c, d"); break;

        case LD_D_A: n = customasm_ruledef_0(size, ruledef, n, o, "ld d, a"); break;
        case LD_D_B: n = customasm_ruledef_0(size, ruledef, n, o, "ld d, b"); break;
        case LD_D_C: n = customasm_ruledef_0(size, ruledef, n, o, "ld d, c"); break;

        case LD_I_I16: n = customasm_ruledef_imm(size, ruledef, n, o, "ld i, {imm:i16}"); break;
        case LD_J_I16: n = customasm_ruledef_imm(size, ruledef, n, o, "ld j, {imm:i16}"); break;
        case LD_K_I16: n = customasm_ruledef_imm(size, ruledef, n, o, "ld k, {imm:i16}"); break;

        case LD_A_AT_I16: n = customasm_ruledef_imm(size, ruledef, n, o, "ld a, [{imm:i16}]"); break;
        case LD_B_AT_I16: n = customasm_ruledef_imm(size, ruledef, n, o, "ld b, [{imm:i16}]"); break;
        case LD_C_AT_I16: n = customasm_ruledef_imm(size, ruledef, n, o, "ld c, [{imm:i16}]"); break;
        case LD_D_AT_I16: n = customasm_ruledef_imm(size, ruledef, n, o, "ld d, [{imm:i16}]"); break;

        case LD_A_AT_AT_I16: n = customasm_ruledef_imm(size, ruledef, n, o, "ld a, [[{imm:i16}]]"); break;
        case LD_B_AT_AT_I16: n = customasm_ruledef_imm(size, ruledef, n, o, "ld b, [[{imm:i16}]]"); break;
        case LD_C_AT_AT_I16: n = customasm_ruledef_imm(size, ruledef, n, o, "ld c, [[{imm:i16}]]"); break;
        case LD_D_AT_AT_I16: n = customasm_ruledef_imm(size, ruledef, n, o, "ld d, [[{imm:i16}]]"); break;

        case LD_A_AT_I: n = customasm_ruledef_0(size, ruledef, n, o, "ld a, [i]"); break;
        case LD_B_AT_I: n = customasm_ruledef_0(size, ruledef, n, o, "ld b, [i]"); break;
        case LD_C_AT_I: n = customasm_ruledef_0(size, ruledef, n, o, "ld c, [i]"); break;
        case LD_D_AT_I: n = customasm_ruledef_0(size, ruledef, n, o, "ld d, [i]"); break;

        case LD_A_AT_J: n = customasm_ruledef_0(size, ruledef, n, o, "ld a, [j]"); break;
        case LD_B_AT_J: n = customasm_ruledef_0(size, ruledef, n, o, "ld b, [j]"); break;
        case LD_C_AT_J: n = customasm_ruledef_0(size, ruledef, n, o, "ld c, [j]"); break;
        case LD_D_AT_J: n = customasm_ruledef_0(size, ruledef, n, o, "ld d, [j]"); break;

        case LD_A_AT_K: n = customasm_ruledef_0(size, ruledef, n, o, "ld a, [k]"); break;
        case LD_B_AT_K: n = customasm_ruledef_0(size, ruledef, n, o, "ld b, [k]"); break;
        case LD_C_AT_K: n = customasm_ruledef_0(size, ruledef, n, o, "ld c, [k]"); break;
        case LD_D_AT_K: n = customasm_ruledef_0(size, ruledef, n, o, "ld d, [k]"); break;

        case LD_A_AT_I_INC: n = customasm_ruledef_0(size, ruledef, n, o, "ld a, [i++]"); break;
        case LD_B_AT_I_INC: n = customasm_ruledef_0(size, ruledef, n, o, "ld b, [i++]"); break;
        case LD_C_AT_I_INC: n = customasm_ruledef_0(size, ruledef, n, o, "ld c, [i++]"); break;
        case LD_D_AT_I_INC: n = customasm_ruledef_0(size, ruledef, n, o, "ld d, [i++]"); break;

        case LD_A_AT_J_INC: n = customasm_ruledef_0(size, ruledef, n, o, "ld a, [j++]"); break;
        case LD_B_AT_J_INC: n = customasm_ruledef_0(size, ruledef, n, o, "ld b, [j++]"); break;
        case LD_C_AT_J_INC: n = customasm_ruledef_0(size, ruledef, n, o, "ld c, [j++]"); break;
        case LD_D_AT_J_INC: n = customasm_ruledef_0(size, ruledef, n, o, "ld d, [j++]"); break;

        case LD_A_AT_K_INC: n = customasm_ruledef_0(size, ruledef, n, o, "ld a, [k++]"); break;
        case LD_B_AT_K_INC: n = customasm_ruledef_0(size, ruledef, n, o, "ld b, [k++]"); break;
        case LD_C_AT_K_INC: n = customasm_ruledef_0(size, ruledef, n, o, "ld c, [k++]"); break;
        case LD_D_AT_K_INC: n = customasm_ruledef_0(size, ruledef, n, o, "ld d, [k++]"); break;

        case LD_AT_I16_A: n = customasm_ruledef_imm(size, ruledef, n, o, "ld [{imm:i16}], a"); break;
        case LD_AT_I16_B: n = customasm_ruledef_imm(size, ruledef, n, o, "ld [{imm:i16}], b"); break;
        case LD_AT_I16_C: n = customasm_ruledef_imm(size, ruledef, n, o, "ld [{imm:i16}], c"); break;
        case LD_AT_I16_D: n = customasm_ruledef_imm(size, ruledef, n, o, "ld [{imm:i16}], d"); break;

        default: break;
        }
    }

    n += strlcat(ruledef + n, end, size);
    assert(n < size);

    return n;
}
