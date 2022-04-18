SECTION "BCD Functions", ROM0

_bcd_inc:: ; input is in A
    add a, 1
    daa
    ret ; output is in A

; uses the "double-dabble" algorithm
; https://my.eng.utah.edu/~nmcdonal/Tutorials/BCDTutorial/BCDConversion.html
_bcd_conv:: ; input is in A
    ld h, a
    xor a
    ld e, 8
.lp:
    ld l, a
    and $0F
    cp 5
    ld a, l
    jr c, .noAdd1
    add 3
.noAdd1:
    ld l, a
    and $F0
    cp $50
    ld a, l
    jr c, .noAdd2
    add $30
.noAdd2:
    sla h
    rla
    dec e
    jr nz, .lp
    ret ; output is in A
