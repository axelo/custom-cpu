#include "../build/custom-cpu.inc"

nop

call uart_init

; Program start
ld b, "="
call uart_cts_write_u8

call uart_cts_flush

ld b, "?"
call uart_cts_write_u8


; Read size into de
call uart_cts_blocking_read_u8
ld d, a
call uart_cts_blocking_read_u8
ld e, a

ld b, "#"
call uart_cts_write_u8

; Read de bytes into bc (0x1000)
ld b, 0x10
ld c, 0x00

.next:
    dec e
    decc d
    jnc .done

    push b
    ld b, "."
    call uart_cts_write_u8
    call uart_cts_blocking_read_u8
    pop b

    ld [bc], a

    inc c
    incc b

    jmp .next

.done:
    ld b, "!"
    call uart_cts_write_u8

    call uart_cts_blocking_read_u8
    call uart_cts_flush

    jmp 0x1000


#include "uart.asm"

#addr 0x1000 - 1
boot_end: #d8 0x00
