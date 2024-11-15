#include "../ruledef.customasm"

; TODO - echo atm
;transmit_u:

;ld i, str_u
;ld t, [i++]

;tx start
;tx
;tx
;tx
;tx
;tx
;tx
;tx
;tx
;tx stop

;jmp transmit_u

;str_u: #d "U\0"


main:
    ld i, str_version
    jal k, fn_uart_write_string

.read:
    ; read a character
    out 0xfd ; enable RTS
.wait:
    rx start, .got_rx_start_bit
    jmp .wait

.got_rx_start_bit:
    out 0xff ; disable RTS

    rx
    rx
    rx
    rx
    rx
    rx
    rx
    ld a, rx

    jal k, fn_uart_write_char

    jmp .read


fn_uart_write_char:
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

    jmp k


fn_uart_write_string:
    ld a, [i++]
    jcmpz k, a

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

    jmp fn_uart_write_string


str_version: #d "\nboot from uart\n\n\0"
