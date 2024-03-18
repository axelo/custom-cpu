#include "../build/custom-cpu.inc"
#include "../build/rom/symbols.inc"

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
ld b, "!"
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

ld b, "A"
call uart_write_u8
ld b, "A"
call uart_write_u8
ld b, "a"
call uart_write_u8
ld b, "a"
call uart_write_u8
ld b, "Z"
call uart_write_u8
ld b, "Z"
call uart_write_u8
ld b, "X"
call uart_write_u8
ld b, "\n"
call uart_write_u8


ld b, "."
call uart_write_u8

call uart_flush

ld b, "!"
call uart_write_u8

main:
    ;call uart_blocking_read_u8
    ;call uart_write_u8

    call uart_non_blocking_read_u8
    jnz main
    call uart_write_u8

    ;ld b, c
    ;call uart_write_u8
    ;ld b, " "
    ;call uart_write_u8

    jmp main

