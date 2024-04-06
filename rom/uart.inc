; UART - Bit banged
;
;    Baud Rate: 115200 / 230400 / 374000
;    Data bits: 8
;       Parity: None
;    Stop bits: 1
; Flow control: RTS/CTS
;

#include "gpio.inc"

#ruledef combined {
  call_uart_delay => asm {
    ; 375-399 / 387 - unstable

    ;nop
        nop2 ; 353-374 / 363
        ;nop2 ; 333-352
        ;nop2 ; 316-332
        ;nop2 ; 300-315
        ;nop2 ; 273-299
        ;nop2 ; 261-272
        ;nop2 ; 250-260
        ;nop2 ; 240-249

    ;nop2 ; 231-239 ; 230400

    ;nop
        ;nop2 ; 222-230
        ;nop2 ; 207-221
        ;nop2 ; 200-206
        ;nop2 ; 194-199
        ;nop2 ; 188-193
        ;nop2 ; 182-187
        ;nop2 ; 172-181
        ;nop2 ; 167-171

    ;nop
        ;nop2 ; 162-166
        ;nop2 ; 158-161
        ;nop2 ; 154-157
        ;nop2 ; 147-153
        ;nop2 ; 143-146
        ;nop2 ; 140-142
        ;nop2 ; 137-139
        ;nop2 ; 134-136

    ;nop2 ; 128-135
    ;nop2 ; 125-127
    ;nop2 ; 123-124
    ;nop2 ; 120-122
    ;nop2 ; 118-119
    ;nop2 ; 113-117  ; 115200
  }
}


uart_init:
; modifies: GPO

    out PORT_GPO, GPO_BIT1_CTS_DISABLED | GPO_BIT0_TX_STOP_BIT
    ret


uart_wait_until_rts:
; modifies: flags, a

    in a, PORT_GPI
    and a, GPI_MASK_BIT6_RTS
    jnz uart_wait_until_rts

    call delay_1000ms

    ret


uart_flush:
; modifies: GPO, flags, a, b

  call uart_non_blocking_read_u8
  jz uart_flush
  ret


uart_non_blocking_read_u8:
;   modifies: GPO, flags, a, b
;    returns:
;       zf: set if read, cleared otherwise
;        b: read u8 if zf set

    out PORT_GPO, GPO_BIT1_CTS_ENABLED | GPO_BIT0_TX_STOP_BIT

    nop     ; 16 clocks
    nop     ; 16 clocks
    nop     ; 16 clocks

    nop2    ; 2 clocks
    nop2    ; 2 clocks
    nop2    ; 2 clocks
    nop2    ; 2 clocks
            ; = 48 + 8 = 56 clocks

    out PORT_GPO, GPO_BIT1_CTS_DISABLED | GPO_BIT0_TX_STOP_BIT

    in a, PORT_GPI        ; 6 clocks
    shl a                 ; 11 clocks
    jnc .got_start_bit    ; 4 clocks on fall through

    in a, PORT_GPI
    shl a                 ; 11 clocks
    jnc .got_start_bit    ; 4 clocks on fall through

    in a, PORT_GPI
    shl a                 ; 11 clocks
    jnc .got_start_bit    ; 4 clocks on fall through

    in a, PORT_GPI
    shl a                 ; 11 clocks
    jnc .got_start_bit    ; 4 clocks on fall through

    in a, PORT_GPI
    shl a                 ; 11 clocks
    jnc .got_start_bit    ; 4 clocks on fall through

    in a, PORT_GPI
    shl a                 ; 11 clocks
    jnc .got_start_bit    ; 4 clocks on fall through

    in a, PORT_GPI
    shl a                 ; 11 clocks
    jnc .got_start_bit_n7 ; 4 clocks on fall through

    in a, PORT_GPI
    shl a                 ; 11 clocks
    jnc .got_start_bit_n8 ; 4 clocks on fall through

    ; clear zf
    ld b, 0
    inc b
    ret

.got_start_bit_n7:
.got_start_bit_n8:
    ; should not happen
    ld b, "?"
    call uart_write_u8
    jmp $

.got_start_bit:
    call_uart_delay

    nop2
    nop2
    nop2

.bit0:
    ; bit 0
    in a, PORT_GPI

    ld b[0], a[GPI_BIT_RX]  ; 20 clocks

    call_uart_delay         ; D clocks
                            ; = D + 20 clocks

    ; bit 1
    in a, PORT_GPI

    ld b[1], a[GPI_BIT_RX]  ; 20 clocks

    call_uart_delay         ; = D + 20 clocks

    ; bit 2
    in a, PORT_GPI

    ld b[2], a[GPI_BIT_RX]  ; 20 clocks

    call_uart_delay         ; = D + 20 clocks

    ; bit 3
    in a, PORT_GPI

    ld b[3], a[GPI_BIT_RX]  ; 20 clocks

    call_uart_delay         ; = D + 20 clocks

    ; bit 4
    in a, PORT_GPI

    ld b[4], a[GPI_BIT_RX]  ; 20 clocks

    call_uart_delay         ; = D + 20 clocks

    ; bit 5
    in a, PORT_GPI

    ld b[5], a[GPI_BIT_RX]  ; 20 clocks

    call_uart_delay         ; = D + 20 clocks

    ; bit 6
    in a, PORT_GPI

    ld b[6], a[GPI_BIT_RX]  ; 20 clocks

    call_uart_delay         ; = D + 20 clocks

    ; bit 7
    in a, PORT_GPI

    ld b[7], a[GPI_BIT_RX]  ; 20 clocks

    call_uart_delay         ; = D + 20 clocks

    ; stop bit
    in a, PORT_GPI

    call_uart_delay         ; D clocks

    ; set zf
    and a, 0                ; 14 clocks

    ret                     ; 16 clocks
                            ; = D + 16 + 14 clocks


uart_blocking_read_u8:
;   modifies: GPO, flags, a, b
;    returns:
;       b: read u8

    out PORT_GPO, GPO_BIT1_CTS_ENABLED | GPO_BIT0_TX_STOP_BIT

.wait_on_start_bit:
    in a, PORT_GPI
    shl a                       ; 11 clocks
    jc .wait_on_start_bit       ; 4 clocks on fall through

    out PORT_GPO, GPO_BIT1_CTS_DISABLED | GPO_BIT0_TX_STOP_BIT ; 5 clocks

    call_uart_delay       ; D clocks
                          ; = D + 20 clocks

    nop2
    nop2

    ; bit 0
    in a, PORT_GPI

    ld b[0], a[GPI_BIT_RX]  ; 20 clocks

    call_uart_delay         ; D clocks
                            ; = D + 20 clocks

    ; bit 1
    in a, PORT_GPI

    ld b[1], a[GPI_BIT_RX]  ; 20 clocks

    call_uart_delay         ; = D + 20 clocks

    ; bit 2
    in a, PORT_GPI

    ld b[2], a[GPI_BIT_RX]  ; 20 clocks

    call_uart_delay         ; = D + 20 clocks

    ; bit 3
    in a, PORT_GPI

    ld b[3], a[GPI_BIT_RX]  ; 20 clocks

    call_uart_delay         ; = D + 20 clocks

    ; bit 4
    in a, PORT_GPI

    ld b[4], a[GPI_BIT_RX]  ; 20 clocks

    call_uart_delay         ; = D + 20 clocks

    ; bit 5
    in a, PORT_GPI

    ld b[5], a[GPI_BIT_RX]  ; 20 clocks

    call_uart_delay         ; = D + 20 clocks

    ; bit 6
    in a, PORT_GPI

    ld b[6], a[GPI_BIT_RX]  ; 20 clocks

    call_uart_delay         ; = D + 20 clocks

    ; bit 7
    in a, PORT_GPI

    ld b[7], a[GPI_BIT_RX]  ; 20 clocks

    call_uart_delay         ; = D + 20 clocks

    ; stop bit
    in a, PORT_GPI          ; only for sync

    call_uart_delay         ; D clocks

    ret                     ; 16 clocks
                            ; = D + 16 clocks


uart_write_u8:
;   modifies: GPO, a
;     params:
;       b: u8 to send

    ld a, GPO_BIT1_CTS_DISABLED | GPO_BIT0_TX_START_BIT;

    out PORT_GPO, a         ; start bit

    ld a[GPO_BIT_TX], b[0]  ; 20 clocks
    call_uart_delay         ; D clocks
                            ; = D + 20 clocks

    out PORT_GPO, a         ; bit 0

    ld a[GPO_BIT_TX], b[1]  ; 20 clocks
    call_uart_delay         ; D clocks

    out PORT_GPO, a         ; bit 1

    ld a[GPO_BIT_TX], b[2]  ; 20 clocks
    call_uart_delay

    out PORT_GPO, a         ; bit 2

    ld a[GPO_BIT_TX], b[3]  ; 20 clocks
    call_uart_delay

    out PORT_GPO, a         ; bit 3

    ld a[GPO_BIT_TX], b[4]  ; 20 clocks
    call_uart_delay

    out PORT_GPO, a         ; bit 4

    ld a[GPO_BIT_TX], b[5]  ; 20 clocks
    call_uart_delay

    out PORT_GPO, a         ; bit 5

    ld a[GPO_BIT_TX], b[6]  ; 20 clocks
    call_uart_delay

    out PORT_GPO, a         ; bit 6

    ld a[GPO_BIT_TX], b[7]  ; 20 clocks
    call_uart_delay

    out PORT_GPO, a         ; bit 7

    ld a[GPO_BIT_TX], b[7]  ; 20 clocks
    call_uart_delay         ; D clocks

    ; stop bit
    nop2                    ; 2 clocks
    out PORT_GPO, GPO_BIT1_CTS_DISABLED | GPO_BIT0_TX_STOP_BIT

    call_uart_delay         ; D clocks

    ret               ; 16 clocks
                      ; = D + 16 clocks


uart_write_n_string:
;   modifies: GPO, a, de
;     params:
;       b: u8 to send
;       de: n string start address

    ld c, [de++] ; n u8 to write, 0 means 256.
.next_u8:
    ld b, [de++]
    call uart_write_u8

    dec c
    jnz .next_u8

    ret
