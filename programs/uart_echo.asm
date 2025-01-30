#include "../ruledef.customasm"

main:
    nop
    
    ; Send an E to verify uart setup is correct
    ld a, "E"
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

enable_rts:
    out 0b1101
wait_until_rx_start_bit:
    in f
    jc wait_until_rx_start_bit
    out 0b1111

    rx; 11 clocks delta between clocking rx bit
    rx
    rx
    rx
    rx
    rx
    rx
    ld a, rx

    ; Echo received byte back over tx
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

    ; Wait again
    jp enable_rts
