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

static const char *start = ""
"#bankdef memory {\n"
"    #bits     8\n"
"    #addr     0\n"
"    #addr_end 0x20000\n" // TODO: ROM_SIZE_BOOT
"    #outp     0\n"
"}\n"
"\n"
"#ruledef instructions\n{\n";

static const char *end = "}\n";

static size_t customasm_ruledef(size_t size, char ruledef[size], const Instruction instructions[0x100]) {

    size_t n = 0;

    n += strlcat(ruledef + n, start, size);
    assert(n < size);

    for (int i = 0; i < 0x100; ++i) {
        if (instructions[i].customasm != NULL) {
            if (strstr(instructions[i].customasm, "{imm") != NULL)
                n = customasm_ruledef_imm(size, ruledef, n, i, instructions[i].customasm);
            else
                n =  customasm_ruledef_0(size, ruledef, n, i, instructions[i].customasm);
        }
    }

    n += strlcat(ruledef + n, end, size);
    assert(n < size);

    return n;
}
