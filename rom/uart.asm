PORT_GPO = 3
PORT_GPI = 3

GPO_TX_STOP_BIT  = 1
GPO_TX_START_BIT = 0

GPO_CTS_DISABLED = 2
GPO_CTS_ENABLED  = 0


uart_init:
    ld a, GPO_CTS_DISABLED | GPO_TX_STOP_BIT
    out PORT_GPO, a

    ret


; 28000 baud rate
uart_delay:
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    ret


; input:
; output:
;  a: read u8
; uses:
;  a
;  b
uart_cts_blocking_read_u8:
    ld b, 0

    ld a, GPO_CTS_ENABLED | GPO_TX_STOP_BIT
    out PORT_GPO, a

.wait_on_start_bit:
    in a, PORT_GPI
    shl a                 ; 14 clocks
    jc .wait_on_start_bit ; 4 clocks on fall through

    ld a, GPO_CTS_DISABLED | GPO_TX_STOP_BIT ; 5 clocks
    out PORT_GPO, a                          ; 6 clocks

    ld a, GPO_CTS_DISABLED | GPO_TX_STOP_BIT ; 5 clocks
    out PORT_GPO, a                          ; 6 clocks

    call uart_delay       ; D clocks
                          ; = D + 40 clocks

    ; bit 0
    in a, PORT_GPI

    and a, 0x80           ; 14 clocks
    shr b                 ; 14 clocks
    add b, a              ; 12 clocks
    call uart_delay       ; D clocks
                          ; = D + 40 clocks

    ; bit 1
    in a, PORT_GPI

    and a, 0x80           ; 14 clocks
    shr b                 ; 14 clocks
    add b, a              ; 12 clocks
    call uart_delay       ; ? clocks

    ; bit 2
    in a, PORT_GPI

    and a, 0x80           ; 14 clocks
    shr b                 ; 14 clocks
    add b, a              ; 12 clocks
    call uart_delay       ; ? clocks

    ; bit 3
    in a, PORT_GPI

    and a, 0x80           ; 14 clocks
    shr b                 ; 14 clocks
    add b, a              ; 12 clocks
    call uart_delay       ; ? clocks

    ; bit 4
    in a, PORT_GPI

    and a, 0x80           ; 14 clocks
    shr b                 ; 14 clocks
    add b, a              ; 12 clocks
    call uart_delay       ; ? clocks

    ; bit 5
    in a, PORT_GPI

    and a, 0x80           ; 14 clocks
    shr b                 ; 14 clocks
    add b, a              ; 12 clocks
    call uart_delay       ; ? clocks

    ; bit 6
    in a, PORT_GPI

    and a, 0x80           ; 14 clocks
    shr b                 ; 14 clocks
    add b, a              ; 12 clocks
    call uart_delay       ; ? clocks

    ; bit 7
    in a, PORT_GPI

    and a, 0x80           ; 14 clocks
    shr b                 ; 14 clocks
    add b, a              ; 12 clocks
    call uart_delay       ; ? clocks

    ; stop bit
    in a, PORT_GPI       ; only for sync

    ld a, b               ; 7 clocks

    nop                   ; 16 clocks
    call uart_delay       ; D clocks
    ret                   ; 16 clocks
                          ; = D + 39 clocks


; uses:
;  a
;  b
uart_cts_flush:
  call uart_cts_non_blocking_read_u8
  jz uart_cts_flush
  ret


; input:
; output:
;  a: read u8
; zf: set if read, cleared otherwise
; uses:
;  a
;  b
uart_cts_non_blocking_read_u8:
    ld b, 0

    ld a, GPO_CTS_ENABLED | GPO_TX_STOP_BIT
    out PORT_GPO, a

; check_start_bit:
    in a, PORT_GPI
    shl a                 ; 14 clocks
    jc .missing_start_bit ; 4 clocks on fall through

    ld a, GPO_CTS_DISABLED | GPO_TX_STOP_BIT ; 5 clocks
    out PORT_GPO, a                          ; 6 clocks

    ld a, GPO_CTS_DISABLED | GPO_TX_STOP_BIT ; 5 clocks
    out PORT_GPO, a                          ; 6 clocks

    call uart_delay       ; D clocks
                          ; = D + 40 clocks

    ; bit 0
    in a, PORT_GPI

    and a, 0x80           ; 14 clocks
    shr b                 ; 14 clocks
    add b, a              ; 12 clocks
    call uart_delay       ; D clocks
                          ; = D + 40 clocks

    ; bit 1
    in a, PORT_GPI

    and a, 0x80           ; 14 clocks
    shr b                 ; 14 clocks
    add b, a              ; 12 clocks
    call uart_delay       ; ? clocks

    ; bit 2
    in a, PORT_GPI

    and a, 0x80           ; 14 clocks
    shr b                 ; 14 clocks
    add b, a              ; 12 clocks
    call uart_delay       ; ? clocks

    ; bit 3
    in a, PORT_GPI

    and a, 0x80           ; 14 clocks
    shr b                 ; 14 clocks
    add b, a              ; 12 clocks
    call uart_delay       ; ? clocks

    ; bit 4
    in a, PORT_GPI

    and a, 0x80           ; 14 clocks
    shr b                 ; 14 clocks
    add b, a              ; 12 clocks
    call uart_delay       ; ? clocks

    ; bit 5
    in a, PORT_GPI

    and a, 0x80           ; 14 clocks
    shr b                 ; 14 clocks
    add b, a              ; 12 clocks
    call uart_delay       ; ? clocks

    ; bit 6
    in a, PORT_GPI

    and a, 0x80           ; 14 clocks
    shr b                 ; 14 clocks
    add b, a              ; 12 clocks
    call uart_delay       ; ? clocks

    ; bit 7
    in a, PORT_GPI

    and a, 0x80           ; 14 clocks
    shr b                 ; 14 clocks
    add b, a              ; 12 clocks
    call uart_delay       ; ? clocks

    ; stop bit
    in a, PORT_GPI       ; only for sync

    ld a, b               ; 7 clocks

    call uart_delay       ; D clocks
    and a, 0              ; 14 clocks, set zf
    ret                   ; 16 clocks
                          ; = D + 37 clocks

.missing_start_bit:
    ld a, GPO_CTS_DISABLED | GPO_TX_STOP_BIT ; 5 clocks
    out PORT_GPO, a                          ; 6 clocks
    or a, 1               ; clear zf
    ret


; input:
;   b: u8 to send
; output:
; uses:
;   a
;   b
uart_cts_write_u8:
    ld a, GPO_CTS_DISABLED | GPO_TX_START_BIT
    out PORT_GPO, a  ; start bit

    call uart_delay   ; D clocks
    in a, PORT_GPI    ; 6 clocks, only for sync

    shr b                   ; 14 clocks, cf = next bit
    ld  a, cf               ;  7 clocks
    or  a, GPO_CTS_DISABLED ; 13 clocks
                      ; = D +  clocks

    out PORT_GPO, a  ; bit 0

    call uart_delay   ; D clocks
    in a, PORT_GPI    ; 6 clocks, only for sync

    shr b                   ; cf = next char bit, 14 clocks
    ld  a, cf               ; 7 clocks
    or  a, GPO_CTS_DISABLED ; 13 clocks

    out PORT_GPO, a  ; bit 1

    call uart_delay
    in a, PORT_GPI    ; 6 clocks, only for sync

    shr b             ; cf = next char bit
    ld  a, cf
    or  a, GPO_CTS_DISABLED

    out PORT_GPO, a  ; bit 2

    call uart_delay
    in a, PORT_GPI    ; 6 clocks, only for sync

    shr b             ; cf = next char bit
    ld  a, cf
    or  a, GPO_CTS_DISABLED

    out PORT_GPO, a  ; bit 3

    call uart_delay
    in a, PORT_GPI    ; 6 clocks, only for sync

    shr b             ; cf = next char bit
    ld  a, cf
    or  a, GPO_CTS_DISABLED

    out PORT_GPO, a  ; bit 4

    call uart_delay
    in a, PORT_GPI    ; 6 clocks, only for sync

    shr b             ; cf = next char bit
    ld  a, cf
    or a, GPO_CTS_DISABLED

    out PORT_GPO, a  ; bit 5

    call uart_delay
    in a, PORT_GPI    ; 6 clocks, only for sync

    shr b             ; cf = next char bit
    ld  a, cf
    or  a, GPO_CTS_DISABLED

    out PORT_GPO, a  ; bit 6

    call uart_delay
    in a, PORT_GPI    ; 6 clocks, only for sync

    shr b             ; cf = next char bit
    ld  a, cf
    or a, GPO_CTS_DISABLED

    out PORT_GPO, a  ; bit 7

    call uart_delay
    in a, PORT_GPI    ; 6 clocks, only for sync

    or  a, 0          ; 13 clocks, only for sync
    nop               ; 16 clocks, only for sync
    ld  a, GPO_CTS_DISABLED | GPO_TX_STOP_BIT ; 5 clocks
                      ; = D + 40 clocks

    out PORT_GPO, a  ; stop bit

    call uart_delay
    nop               ; 16 clocks
    nop               ; 16 clocks
    ret               ; 16 clocks
                      ; = D + 48 clocks
