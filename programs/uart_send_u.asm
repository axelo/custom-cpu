#include "../ruledef.customasm"


main:
    nop
    ld a, "U"

loop:
    ld tx, a
    tx
    tx
    tx
    tx
    tx
    tx
    tx
    tx
    tx stop

    jp loop
