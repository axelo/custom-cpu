#include "../build/custom-cpu.inc"
#include "../build/rom/symbols.inc"

ld b, "B"
call uart_write_u8

; Number of times to blink before returning to boot ready
ld c, 3

main:
    out PORT_GPO, GPO_MASK_BIT7_UNUSED | GPO_DEFAULT

    call delay_1000ms

    out PORT_GPO, 0x00 | GPO_DEFAULT

    call delay_1000ms

    dec c
    jnz main


jmp boot_ready
