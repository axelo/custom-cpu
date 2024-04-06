#include "../build/custom-cpu.inc"
#include "../build/rom/symbols.inc"

ld b, 0

loop:
    push b
    ld de, string_buffer
    call u8_to_n_string_zero_padded

    ld de, string_buffer
    call uart_write_n_string

    ld b, " "
    call uart_write_u8

    pop b
    inc b

    jnz loop

jmp 0

string_buffer: #res 16
