#include "../build/custom-cpu.inc"
#include "../build/rom/symbols.inc"

main:
    ld c, 5

.loop:
    ld b, "H"
    call uart_write_u8

    ld b, "e"
    call uart_write_u8

    ld b, "l"
    call uart_write_u8

    ld b, "l"
    call uart_write_u8

    ld b, "o"
    call uart_write_u8

    ld b, " "
    call uart_write_u8

    ld b, "W"
    call uart_write_u8

    ld b, "o"
    call uart_write_u8

    ld b, "r"
    call uart_write_u8

    ld b, "l"
    call uart_write_u8

    ld b, "d"
    call uart_write_u8

    ld b, "!"
    call uart_write_u8

    ld b, "\n"
    call uart_write_u8

    dec c
    jnz .loop

jmp boot_ready
