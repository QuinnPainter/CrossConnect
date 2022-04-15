SECTION "BCD Functions", ROM0

_bcd_inc:: ; input is in A
    add a, 1
    daa
    ret ; output is in A
