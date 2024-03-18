#include "../build/custom-cpu.inc"

; Program start
main:
    ld a, 0x1

.next:
    out 3, a

    ld b, 0
    ld c, 0
    ld d, 0
    ld e, 0
    .delay0: dec b
    jnz .delay0
    .delay1: dec b
    jnz .delay1
    .delay2: dec c
    jnz .delay2
    .delay3: dec d
    jnz .delay3
    .delay4: dec e
    jnz .delay4
    .delay5: dec b
    jnz .delay5
    .delay6: dec b
    jnz .delay6
    .delay7: dec c
    jnz .delay7
    .delay8: dec d
    jnz .delay8
    .delay9: dec e
    jnz .delay9
    .delay10: dec b
    jnz .delay10
    .delay11: dec b
    jnz .delay11
    .delay12: dec c
    jnz .delay12
    .delay13: dec d
    jnz .delay13
    .delay14: dec e
    jnz .delay14
    .delay15: dec b
    jnz .delay15
    .delay16: dec b
    jnz .delay16
    .delay17: dec c
    jnz .delay17
    .delay18: dec d
    jnz .delay18
    .delay19: dec e
    jnz .delay19
    .delay20: dec b
    jnz .delay20
    .delay21: dec b
    jnz .delay21
    .delay22: dec c
    jnz .delay22
    .delay23: dec d
    jnz .delay23
    .delay24: dec e
    jnz .delay24
    .delay25: dec b
    jnz .delay25
    .delay26: dec b
    jnz .delay26
    .delay27: dec c
    jnz .delay27
    .delay28: dec d
    jnz .delay28
    .delay29: dec e
    jnz .delay29
    .delay30: dec b
    jnz .delay30
    .delay31: dec b
    jnz .delay31
    .delay32: dec c
    jnz .delay32
    .delay33: dec d
    jnz .delay33
    .delay34: dec e
    jnz .delay34
    .delay35: dec b
    jnz .delay35
    .delay36: dec b
    jnz .delay36
    .delay37: dec c
    jnz .delay37
    .delay38: dec d
    jnz .delay38
    .delay39: dec e
    jnz .delay39
    .delay40: dec b
    jnz .delay40
    .delay41: dec b
    jnz .delay41
    .delay42: dec c
    jnz .delay42
    .delay43: dec d
    jnz .delay43
    .delay44: dec e
    jnz .delay44
    .delay45: dec b
    jnz .delay45
    .delay46: dec b
    jnz .delay46
    .delay47: dec c
    jnz .delay47
    .delay48: dec d
    jnz .delay48
    .delay49: dec e
    jnz .delay49
    .delay50: dec b
    jnz .delay50
    .delay51: dec b
    jnz .delay51
    .delay52: dec c
    jnz .delay52
    .delay53: dec d
    jnz .delay53
    .delay54: dec e
    jnz .delay54
    .delay55: dec b
    jnz .delay55
    .delay56: dec b
    jnz .delay56
    .delay57: dec c
    jnz .delay57
    .delay58: dec d
    jnz .delay58
    .delay59: dec e
    jnz .delay59
    .delay60: dec b
    jnz .delay60
    .delay61: dec b
    jnz .delay61
    .delay62: dec c
    jnz .delay62
    .delay63: dec d
    jnz .delay63
    .delay64: dec e
    jnz .delay64
    .delay65: dec b
    jnz .delay65
    .delay66: dec b
    jnz .delay66
    .delay67: dec c
    jnz .delay67
    .delay68: dec d
    jnz .delay68
    .delay69: dec e
    jnz .delay69
    .delay70: dec b
    jnz .delay70
    .delay71: dec b
    jnz .delay71
    .delay72: dec c
    jnz .delay72
    .delay73: dec d
    jnz .delay73
    .delay74: dec e
    jnz .delay74
    .delay75: dec b
    jnz .delay75
    .delay76: dec b
    jnz .delay76
    .delay77: dec c
    jnz .delay77
    .delay78: dec d
    jnz .delay78
    .delay79: dec e
    jnz .delay79
    .delay80: dec b
    jnz .delay80

    shl a
    jnc .next

    ld a, 0x80

.next_right:
    out 3, a

    ld b, 0
    ld c, 0
    ld d, 0
    ld e, 0
    .delayb0: dec b
    jnz .delayb0
    .delayb1: dec b
    jnz .delayb1
    .delayb2: dec c
    jnz .delayb2
    .delayb3: dec d
    jnz .delayb3
    .delayb4: dec e
    jnz .delayb4
    .delayb5: dec b
    jnz .delayb5
    .delayb6: dec b
    jnz .delayb6
    .delayb7: dec c
    jnz .delayb7
    .delayb8: dec d
    jnz .delayb8
    .delayb9: dec e
    jnz .delayb9
    .delayb10: dec b
    jnz .delayb10
    .delayb11: dec b
    jnz .delayb11
    .delayb12: dec c
    jnz .delayb12
    .delayb13: dec d
    jnz .delayb13
    .delayb14: dec e
    jnz .delayb14
    .delayb15: dec b
    jnz .delayb15
    .delayb16: dec b
    jnz .delayb16
    .delayb17: dec c
    jnz .delayb17
    .delayb18: dec d
    jnz .delayb18
    .delayb19: dec e
    jnz .delayb19
    .delayb20: dec b
    jnz .delayb20
    .delayb21: dec b
    jnz .delayb21
    .delayb22: dec c
    jnz .delayb22
    .delayb23: dec d
    jnz .delayb23
    .delayb24: dec e
    jnz .delayb24
    .delayb25: dec b
    jnz .delayb25
    .delayb26: dec b
    jnz .delayb26
    .delayb27: dec c
    jnz .delayb27
    .delayb28: dec d
    jnz .delayb28
    .delayb29: dec e
    jnz .delayb29
    .delayb30: dec b
    jnz .delayb30
    .delayb31: dec b
    jnz .delayb31
    .delayb32: dec c
    jnz .delayb32
    .delayb33: dec d
    jnz .delayb33
    .delayb34: dec e
    jnz .delayb34
    .delayb35: dec b
    jnz .delayb35
    .delayb36: dec b
    jnz .delayb36
    .delayb37: dec c
    jnz .delayb37
    .delayb38: dec d
    jnz .delayb38
    .delayb39: dec e
    jnz .delayb39
    .delayb40: dec b
    jnz .delayb40
    .delayb41: dec b
    jnz .delayb41
    .delayb42: dec c
    jnz .delayb42
    .delayb43: dec d
    jnz .delayb43
    .delayb44: dec e
    jnz .delayb44
    .delayb45: dec b
    jnz .delayb45
    .delayb46: dec b
    jnz .delayb46
    .delayb47: dec c
    jnz .delayb47
    .delayb48: dec d
    jnz .delayb48
    .delayb49: dec e
    jnz .delayb49
    .delayb50: dec b
    jnz .delayb50
    .delayb51: dec b
    jnz .delayb51
    .delayb52: dec c
    jnz .delayb52
    .delayb53: dec d
    jnz .delayb53
    .delayb54: dec e
    jnz .delayb54
    .delayb55: dec b
    jnz .delayb55
    .delayb56: dec b
    jnz .delayb56
    .delayb57: dec c
    jnz .delayb57
    .delayb58: dec d
    jnz .delayb58
    .delayb59: dec e
    jnz .delayb59
    .delayb60: dec b
    jnz .delayb60
    .delayb61: dec b
    jnz .delayb61
    .delayb62: dec c
    jnz .delayb62
    .delayb63: dec d
    jnz .delayb63
    .delayb64: dec e
    jnz .delayb64
    .delayb65: dec b
    jnz .delayb65
    .delayb66: dec b
    jnz .delayb66
    .delayb67: dec c
    jnz .delayb67
    .delayb68: dec d
    jnz .delayb68
    .delayb69: dec e
    jnz .delayb69
    .delayb70: dec b
    jnz .delayb70
    .delayb71: dec b
    jnz .delayb71
    .delayb72: dec c
    jnz .delayb72
    .delayb73: dec d
    jnz .delayb73
    .delayb74: dec e
    jnz .delayb74
    .delayb75: dec b
    jnz .delayb75
    .delayb76: dec b
    jnz .delayb76
    .delayb77: dec c
    jnz .delayb77
    .delayb78: dec d
    jnz .delayb78
    .delayb79: dec e
    jnz .delayb79
    .delayb80: dec b
    jnz .delayb80

    shr a
    jnc .next_right

    jmp main