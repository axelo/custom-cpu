#include "../ruledef.customasm"

mem_copy_check:
    ld a, 0x01
    ld a, 0x02
    ld a, 0x04
    ld a, 0x08
    ld a, 0x10
    ld a, 0x20
    ld a, 0x40
    ld a, 0x80
    ld a, 0x40
    ld a, 0x20
    ld a, 0x10
    ld a, 0x08
    ld a, 0x04
    ld a, 0x02
    ld a, 0x01
    ld a, 0x02
    ld a, 0x04
    ld a, 0x08
    ld a, 0x10
    ld a, 0x20
    ld a, 0x40
    ld a, 0x80
    ld a, 0x40
    ld a, 0x20
    ld a, 0x10
    ld a, 0x08
    ld a, 0x04
    ld a, 0x02
    ld a, 0x01

ld a, "U"
ld b, 0x10
ld c, 0x00

transmit_u:
    ld k, .next

    jcmpz c, .next_hi

.write:
    jmp fn_uart_write_char

.next:
    dec c
    jnz .write

.next_hi:
    dec b
    jc .write

;    dec c
;    decc b     ; if !CF sub 1 else if ZF do sub b, 0 else nothing
;    jnz .next

main:
    ld i, str_version
    jal k, fn_uart_write_string

; read a character
    ld k, .read ; return to .read

.read:
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

; echo read character, return to k (.read)
    jmp fn_uart_write_char


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
    jcmpz a, k

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
