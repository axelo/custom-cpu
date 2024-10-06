#define MNEMONIC_RESET          "reset"
#define MNEMONIC_NOP            "nop"
#define MNEMONIC_LD_A_I8        "ld a, {imm:i8}"
#define MNEMONIC_LD_B_I8        "ld b, {imm:i8}"
#define MNEMONIC_LD_C_I8        "ld c, {imm:i8}"
#define MNEMONIC_LD_D_I8        "ld d, {imm:i8}"
#define MNEMONIC_LD_A_B         "ld a, b"
#define MNEMONIC_LD_A_C         "ld a, c"
#define MNEMONIC_LD_A_D         "ld a, d"
#define MNEMONIC_LD_B_A         "ld b, a"
#define MNEMONIC_LD_B_C         "ld b, c"
#define MNEMONIC_LD_B_D         "ld b, d"
#define MNEMONIC_LD_C_A         "ld c, a"
#define MNEMONIC_LD_C_B         "ld c, b"
#define MNEMONIC_LD_C_D         "ld c, d"
#define MNEMONIC_LD_D_A         "ld d, a"
#define MNEMONIC_LD_D_B         "ld d, b"
#define MNEMONIC_LD_D_C         "ld d, c"
#define MNEMONIC_LD_I_I16       "ld i, {imm:i16}"
#define MNEMONIC_LD_J_I16       "ld j, {imm:i16}"
#define MNEMONIC_LD_K_I16       "ld k, {imm:i16}"
#define MNEMONIC_LD_A_AT_I16    "ld a, [{imm:i16}]"
#define MNEMONIC_LD_B_AT_I16    "ld b, [{imm:i16}]"
#define MNEMONIC_LD_C_AT_I16    "ld c, [{imm:i16}]"
#define MNEMONIC_LD_D_AT_I16    "ld d, [{imm:i16}]"
#define MNEMONIC_LD_A_AT_AT_I16 "ld a, [[{imm:i16}]]"
#define MNEMONIC_LD_B_AT_AT_I16 "ld b, [[{imm:i16}]]"
#define MNEMONIC_LD_C_AT_AT_I16 "ld c, [[{imm:i16}]]"
#define MNEMONIC_LD_D_AT_AT_I16 "ld d, [[{imm:i16}]]"
#define MNEMONIC_LD_A_AT_I      "ld a, [i]"
#define MNEMONIC_LD_B_AT_I      "ld b, [i]"
#define MNEMONIC_LD_C_AT_I      "ld c, [i]"
#define MNEMONIC_LD_D_AT_I      "ld d, [i]"
#define MNEMONIC_LD_A_AT_J      "ld a, [j]"
#define MNEMONIC_LD_B_AT_J      "ld b, [j]"
#define MNEMONIC_LD_C_AT_J      "ld c, [j]"
#define MNEMONIC_LD_D_AT_J      "ld d, [j]"
#define MNEMONIC_LD_A_AT_K      "ld a, [k]"
#define MNEMONIC_LD_B_AT_K      "ld b, [k]"
#define MNEMONIC_LD_C_AT_K      "ld c, [k]"
#define MNEMONIC_LD_D_AT_K      "ld d, [k]"
#define MNEMONIC_LD_A_AT_I_INC  "ld a, [i++]"
#define MNEMONIC_LD_B_AT_I_INC  "ld b, [i++]"
#define MNEMONIC_LD_C_AT_I_INC  "ld c, [i++]"
#define MNEMONIC_LD_D_AT_I_INC  "ld d, [i++]"
#define MNEMONIC_LD_A_AT_J_INC  "ld a, [j++]"
#define MNEMONIC_LD_B_AT_J_INC  "ld b, [j++]"
#define MNEMONIC_LD_C_AT_J_INC  "ld c, [j++]"
#define MNEMONIC_LD_D_AT_J_INC  "ld d, [j++]"
#define MNEMONIC_LD_A_AT_K_INC  "ld a, [k++]"
#define MNEMONIC_LD_B_AT_K_INC  "ld b, [k++]"
#define MNEMONIC_LD_C_AT_K_INC  "ld c, [k++]"
#define MNEMONIC_LD_D_AT_K_INC  "ld d, [k++]"
#define MNEMONIC_LD_AT_I16_A    "ld [{imm:i16}], a"
#define MNEMONIC_LD_AT_I16_B    "ld [{imm:i16}], b"
#define MNEMONIC_LD_AT_I16_C    "ld [{imm:i16}], c"
#define MNEMONIC_LD_AT_I16_D    "ld [{imm:i16}], d"
#define MNEMONIC_LD_AT_I_INC_A  "ld [i++], a"
#define MNEMONIC_LD_AT_J_INC_A  "ld [j++], a"
#define MNEMONIC_LD_AT_K_INC_A  "ld [k++], a"
#define MNEMONIC_LD_FLAGS_I8    "ld flags, {imm:i8}"
#define MNEMONIC_ADD_A_I8       "add a, {imm:i8}"
#define MNEMONIC_ADDC_A_I8      "addc a, {imm:i8}"
#define MNEMONIC_SUBC_A_I8      "subc a, {imm:i8}"
#define MNEMONIC_SHL_A          "shl a"
#define MNEMONIC_SHR_A          "shr a"
#define MNEMONIC_AND_A_I8       "and a, {imm:i8}"
#define MNEMONIC_JMP_I16        "jmp {imm:i16}"
#define MNEMONIC_JMP_K          "jmp k"
#define MNEMONIC_JZ_I16         "jz {imm:i16}"
#define MNEMONIC_JNZ_I16        "jnz {imm:i16}"
#define MNEMONIC_JC_I16         "jc {imm:i16}"
#define MNEMONIC_JNC_I16        "jnc {imm:i16}"
#define MNEMONIC_JAL_K_I16      "jal k, {imm:i16}"
#define MNEMONIC_PUSH_A         "push a"
#define MNEMONIC_PUSH_K         "push k"
#define MNEMONIC_POP_A          "pop a"
#define MNEMONIC_POP_K          "pop k"
#define MNEMONIC_OUT_I8         "out {imm:i8}"
#define MNEMONIC_OUT_TX_START   "out tx start"
#define MNEMONIC_OUT_TX         "out tx"
#define MNEMONIC_OUT_TX_STOP    "out tx stop"
#define MNEMONIC_IN_RX_START_I16 "in rx start, {imm:i16}"
#define MNEMONIC_IN_RX          "in rx"

static size_t inline customasm_ruledef_0(size_t size, char ruledef[size], size_t n, int i, const char *mnemonic) {
    n += (size_t) snprintf(ruledef + n, size, "%s%s%s%02x\n", "    ", mnemonic, " => 0x", i);
    assert(n < size);

    return n;
}

static size_t inline customasm_ruledef_imm(size_t size, char ruledef[size], size_t n, int i, const char *mnemonic) {
    n += (size_t) snprintf(ruledef + n, size, "%s%s%s%02x @ imm\n", "    ", mnemonic, " => 0x", i);
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

    for (int i = 0; i < 0x100; ++i) {
        switch ((Instruction)i) {
        case RESET: n = customasm_ruledef_0(size, ruledef, n, i, MNEMONIC_RESET); break;
        case NOP: n = customasm_ruledef_0(size, ruledef, n, i, MNEMONIC_NOP); break;

        case LD_A_I8: n = customasm_ruledef_imm(size, ruledef, n, i, MNEMONIC_LD_A_I8); break;
        case LD_B_I8: n = customasm_ruledef_imm(size, ruledef, n, i, MNEMONIC_LD_B_I8); break;
        case LD_C_I8: n = customasm_ruledef_imm(size, ruledef, n, i, MNEMONIC_LD_C_I8); break;
        case LD_D_I8: n = customasm_ruledef_imm(size, ruledef, n, i, MNEMONIC_LD_D_I8); break;

        case LD_A_B: n = customasm_ruledef_0(size, ruledef, n, i, MNEMONIC_LD_A_B); break;
        case LD_A_C: n = customasm_ruledef_0(size, ruledef, n, i, MNEMONIC_LD_A_C); break;
        case LD_A_D: n = customasm_ruledef_0(size, ruledef, n, i, MNEMONIC_LD_A_D); break;

        case LD_B_A: n = customasm_ruledef_0(size, ruledef, n, i, MNEMONIC_LD_B_A); break;
        case LD_B_C: n = customasm_ruledef_0(size, ruledef, n, i, MNEMONIC_LD_B_C); break;
        case LD_B_D: n = customasm_ruledef_0(size, ruledef, n, i, MNEMONIC_LD_B_D); break;

        case LD_C_A: n = customasm_ruledef_0(size, ruledef, n, i, MNEMONIC_LD_C_A); break;
        case LD_C_B: n = customasm_ruledef_0(size, ruledef, n, i, MNEMONIC_LD_C_B); break;
        case LD_C_D: n = customasm_ruledef_0(size, ruledef, n, i, MNEMONIC_LD_C_D); break;

        case LD_D_A: n = customasm_ruledef_0(size, ruledef, n, i, MNEMONIC_LD_D_A); break;
        case LD_D_B: n = customasm_ruledef_0(size, ruledef, n, i, MNEMONIC_LD_D_B); break;
        case LD_D_C: n = customasm_ruledef_0(size, ruledef, n, i, MNEMONIC_LD_D_C); break;

        case LD_I_I16: n = customasm_ruledef_imm(size, ruledef, n, i, MNEMONIC_LD_I_I16); break;
        case LD_J_I16: n = customasm_ruledef_imm(size, ruledef, n, i, MNEMONIC_LD_J_I16); break;
        case LD_K_I16: n = customasm_ruledef_imm(size, ruledef, n, i, MNEMONIC_LD_K_I16); break;

        case LD_A_AT_I16: n = customasm_ruledef_imm(size, ruledef, n, i, MNEMONIC_LD_A_AT_I16); break;
        case LD_B_AT_I16: n = customasm_ruledef_imm(size, ruledef, n, i, MNEMONIC_LD_B_AT_I16); break;
        case LD_C_AT_I16: n = customasm_ruledef_imm(size, ruledef, n, i, MNEMONIC_LD_C_AT_I16); break;
        case LD_D_AT_I16: n = customasm_ruledef_imm(size, ruledef, n, i, MNEMONIC_LD_D_AT_I16); break;

        case LD_A_AT_AT_I16: n = customasm_ruledef_imm(size, ruledef, n, i, MNEMONIC_LD_A_AT_AT_I16); break;
        case LD_B_AT_AT_I16: n = customasm_ruledef_imm(size, ruledef, n, i, MNEMONIC_LD_B_AT_AT_I16); break;
        case LD_C_AT_AT_I16: n = customasm_ruledef_imm(size, ruledef, n, i, MNEMONIC_LD_C_AT_AT_I16); break;
        case LD_D_AT_AT_I16: n = customasm_ruledef_imm(size, ruledef, n, i, MNEMONIC_LD_D_AT_AT_I16); break;

        case LD_A_AT_I: n = customasm_ruledef_0(size, ruledef, n, i, MNEMONIC_LD_A_AT_I); break;
        case LD_B_AT_I: n = customasm_ruledef_0(size, ruledef, n, i, MNEMONIC_LD_B_AT_I); break;
        case LD_C_AT_I: n = customasm_ruledef_0(size, ruledef, n, i, MNEMONIC_LD_C_AT_I); break;
        case LD_D_AT_I: n = customasm_ruledef_0(size, ruledef, n, i, MNEMONIC_LD_D_AT_I); break;

        case LD_A_AT_J: n = customasm_ruledef_0(size, ruledef, n, i, MNEMONIC_LD_A_AT_J); break;
        case LD_B_AT_J: n = customasm_ruledef_0(size, ruledef, n, i, MNEMONIC_LD_B_AT_J); break;
        case LD_C_AT_J: n = customasm_ruledef_0(size, ruledef, n, i, MNEMONIC_LD_C_AT_J); break;
        case LD_D_AT_J: n = customasm_ruledef_0(size, ruledef, n, i, MNEMONIC_LD_D_AT_J); break;

        case LD_A_AT_K: n = customasm_ruledef_0(size, ruledef, n, i, MNEMONIC_LD_A_AT_K); break;
        case LD_B_AT_K: n = customasm_ruledef_0(size, ruledef, n, i, MNEMONIC_LD_B_AT_K); break;
        case LD_C_AT_K: n = customasm_ruledef_0(size, ruledef, n, i, MNEMONIC_LD_C_AT_K); break;
        case LD_D_AT_K: n = customasm_ruledef_0(size, ruledef, n, i, MNEMONIC_LD_D_AT_K); break;

        case LD_A_AT_I_INC: n = customasm_ruledef_0(size, ruledef, n, i, MNEMONIC_LD_A_AT_I_INC); break;
        case LD_B_AT_I_INC: n = customasm_ruledef_0(size, ruledef, n, i, MNEMONIC_LD_B_AT_I_INC); break;
        case LD_C_AT_I_INC: n = customasm_ruledef_0(size, ruledef, n, i, MNEMONIC_LD_C_AT_I_INC); break;
        case LD_D_AT_I_INC: n = customasm_ruledef_0(size, ruledef, n, i, MNEMONIC_LD_D_AT_I_INC); break;

        case LD_A_AT_J_INC: n = customasm_ruledef_0(size, ruledef, n, i, MNEMONIC_LD_A_AT_J_INC); break;
        case LD_B_AT_J_INC: n = customasm_ruledef_0(size, ruledef, n, i, MNEMONIC_LD_B_AT_J_INC); break;
        case LD_C_AT_J_INC: n = customasm_ruledef_0(size, ruledef, n, i, MNEMONIC_LD_C_AT_J_INC); break;
        case LD_D_AT_J_INC: n = customasm_ruledef_0(size, ruledef, n, i, MNEMONIC_LD_D_AT_J_INC); break;

        case LD_A_AT_K_INC: n = customasm_ruledef_0(size, ruledef, n, i, MNEMONIC_LD_A_AT_K_INC); break;
        case LD_B_AT_K_INC: n = customasm_ruledef_0(size, ruledef, n, i, MNEMONIC_LD_B_AT_K_INC); break;
        case LD_C_AT_K_INC: n = customasm_ruledef_0(size, ruledef, n, i, MNEMONIC_LD_C_AT_K_INC); break;
        case LD_D_AT_K_INC: n = customasm_ruledef_0(size, ruledef, n, i, MNEMONIC_LD_D_AT_K_INC); break;

        case LD_AT_I16_A: n = customasm_ruledef_imm(size, ruledef, n, i, MNEMONIC_LD_AT_I16_A); break;
        case LD_AT_I16_B: n = customasm_ruledef_imm(size, ruledef, n, i, MNEMONIC_LD_AT_I16_B); break;
        case LD_AT_I16_C: n = customasm_ruledef_imm(size, ruledef, n, i, MNEMONIC_LD_AT_I16_C); break;
        case LD_AT_I16_D: n = customasm_ruledef_imm(size, ruledef, n, i, MNEMONIC_LD_AT_I16_D); break;

        case LD_AT_I_INC_A: n = customasm_ruledef_0(size, ruledef, n, i, MNEMONIC_LD_AT_I_INC_A); break;
        case LD_AT_J_INC_A: n = customasm_ruledef_0(size, ruledef, n, i, MNEMONIC_LD_AT_J_INC_A); break;
        case LD_AT_K_INC_A: n = customasm_ruledef_0(size, ruledef, n, i, MNEMONIC_LD_AT_K_INC_A); break;

        case LD_FLAGS_I8: n = customasm_ruledef_imm(size, ruledef, n, i, MNEMONIC_LD_FLAGS_I8); break;

        case ADD_A_I8: n = customasm_ruledef_imm(size, ruledef, n, i, MNEMONIC_ADD_A_I8); break;

        case ADDC_A_I8: n = customasm_ruledef_imm(size, ruledef, n, i, MNEMONIC_ADDC_A_I8); break;

        case SUBC_A_I8: n = customasm_ruledef_imm(size, ruledef, n, i, MNEMONIC_SUBC_A_I8); break;

        case SHL_A: n = customasm_ruledef_0(size, ruledef, n, i, MNEMONIC_SHL_A); break;

        case SHR_A: n = customasm_ruledef_0(size, ruledef, n, i, MNEMONIC_SHR_A); break;

        case AND_A_I8: n = customasm_ruledef_imm(size, ruledef, n, i, MNEMONIC_AND_A_I8); break;

        case JMP_I16: n = customasm_ruledef_imm(size, ruledef, n, i, MNEMONIC_JMP_I16); break;

        case JMP_K: n = customasm_ruledef_0(size, ruledef, n, i, MNEMONIC_JMP_K); break;

        case JZ_I16:  n = customasm_ruledef_imm(size, ruledef, n, i, MNEMONIC_JZ_I16);  break;
        case JNZ_I16: n = customasm_ruledef_imm(size, ruledef, n, i, MNEMONIC_JNZ_I16); break;
        case JC_I16:  n = customasm_ruledef_imm(size, ruledef, n, i, MNEMONIC_JC_I16);  break;
        case JNC_I16: n = customasm_ruledef_imm(size, ruledef, n, i, MNEMONIC_JNC_I16); break;

        case JAL_K_I16: n = customasm_ruledef_imm(size, ruledef, n, i, MNEMONIC_JAL_K_I16); break;

        case PUSH_A: n = customasm_ruledef_0(size, ruledef, n, i, MNEMONIC_PUSH_A); break;

        case PUSH_K: n = customasm_ruledef_0(size, ruledef, n, i, MNEMONIC_PUSH_K); break;

        case POP_A: n = customasm_ruledef_0(size, ruledef, n, i, MNEMONIC_POP_A); break;

        case POP_K: n = customasm_ruledef_0(size, ruledef, n, i, MNEMONIC_POP_K); break;

        case OUT_I8: n = customasm_ruledef_imm(size, ruledef, n, i, MNEMONIC_OUT_I8); break;

        case OUT_TX_START: n = customasm_ruledef_0(size, ruledef, n, i, MNEMONIC_OUT_TX_START); break;
        case OUT_TX:       n = customasm_ruledef_0(size, ruledef, n, i, MNEMONIC_OUT_TX); break;
        case OUT_TX_STOP:  n = customasm_ruledef_0(size, ruledef, n, i, MNEMONIC_OUT_TX_STOP); break;

        case IN_RX_START_I16: n = customasm_ruledef_imm(size, ruledef, n, i, MNEMONIC_IN_RX_START_I16); break;
        case IN_RX:           n = customasm_ruledef_0(size, ruledef, n, i, MNEMONIC_IN_RX); break;

        default: break;
        }
    }

    n += strlcat(ruledef + n, end, size);
    assert(n < size);

    return n;
}
