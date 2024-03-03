#include "../build/custom-cpu.inc"
#include "../build/rom/symbols.inc"

main:
    ld c, 3

.loop:
    ld b, "H"
    call uart_cts_write_u8

    ld b, "e"
    call uart_cts_write_u8

    ld b, "l"
    call uart_cts_write_u8

    ld b, "l"
    call uart_cts_write_u8

    ld b, "o"
    call uart_cts_write_u8

    ld b, " "
    call uart_cts_write_u8

    ld b, "W"
    call uart_cts_write_u8

    ld b, "o"
    call uart_cts_write_u8

    ld b, "r"
    call uart_cts_write_u8

    ld b, "l"
    call uart_cts_write_u8

    ld b, "d"
    call uart_cts_write_u8

    ld b, "!"
    call uart_cts_write_u8

    ld b, "\n"
    call uart_cts_write_u8

    dec c
    jnz .loop

jmp 0
