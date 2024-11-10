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
    rx

    ld i, 0xfff2
    ld a, [i];    ; ld a, t

    jal k, fn_uart_write_char

    jmp .read


fn_uart_write_char:
    ld [i], a

    tx start
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
    ld t, [i++]
    jcmpz t, k

    tx start
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
