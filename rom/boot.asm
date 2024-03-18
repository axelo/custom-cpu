#include "../build/custom-cpu.inc"

nop

call uart_init

call uart_wait_until_rts

call uart_flush

; Program start
boot_ready:
ld b, "="
call uart_write_u8

ld b, "?"
call uart_write_u8

; Read size into de
call uart_blocking_read_u8
ld a, b
ld d, a
call uart_blocking_read_u8
ld a, b
ld e, a


ld b, "#"
call uart_write_u8

; Read de bytes into bc (0x1000)
ld b, 0x10
ld c, 0x00

.next:
    dec e
    decc d
    jnc .done

    push b
    ld b, "."
    call uart_write_u8
    call uart_blocking_read_u8
    ld a, b
    pop b

    ld [bc], a

    inc c
    incc b

    jmp .next

.done:
    call uart_flush

    ld b, "!"
    call uart_write_u8

    call uart_blocking_read_u8

    jmp 0x1000

#include "delay.inc"

#include "uart.inc"

#addr 0x1000 - 1
.boot_end: #d8 0x00
