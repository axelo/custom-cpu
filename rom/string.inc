u8_to_n_string_zero_padded:
;   modifies: a, b, de
;     params:
;       b: u8
;       de: n string start address

    ld a, 3
    ld [de++], a

u8_to_append_string_zero_padded:
    ld a, 0xff
.fits100:
    inc a
    add b, -100
    jc .fits100

    add a, "0"
    ld [de++], a

    add b, 100
    ld a, 0xff
.fits10:
    inc a
    add b, -10
    jc .fits10

    add a, "0"
    ld [de++], a

    add b, 10 + "0"
    ld a, b
    ld [de++], a

    ret
