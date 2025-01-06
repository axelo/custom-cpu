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

wait_until_rx_start_bit:
    rx start
    jc wait_until_rx_start_bit

    rx ; 11 clocks delta between clocking rx bit
    rx
    rx
    rx
    rx
    rx
    rx
    ld a, rx

    ; Echo recevied byte back over tx
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
    jp wait_until_rx_start_bit
