delay_1000ms:
; modifies: flags

    push d
    push e

    ld d, 0xff
    ld e, 0xff

.delay0:
    dec e
    decc d
    jc .delay0

.delay1:
    dec e
    decc d
    jc .delay1

.delay2:
    dec e
    decc d
    jc .delay2

.delay3:
    dec e
    decc d
    jc .delay3

.delay4:
    dec e
    decc d
    jc .delay4

.delay5:
    dec e
    decc d
    jc .delay5

.delay6:
    dec e
    decc d
    jc .delay6

    pop e
    pop d

    ret
