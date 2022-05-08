SECTION "RandomRAM", WRAM0
_randState::
wRandState:: DS 2

SECTION "RandomCode", ROM0

; Seeding the generator is accomplished by just setting wRandState.
; Make sure it isn't 0!

; Generate a pseudorandom number between 1 and FFFF
; Sets - HL = pseudorandom number
; Sets - A to L (lower byte of random number)
; http://www.retroprogramming.com/2017/07/xorshift-pseudorandom-numbers-in-z80.html
genRandom::
    ld hl, wRandState
    ld a, [hli]
    ld l, [hl]
    ld h, a
    rra
    ld a, l
    rra
    xor h
    ld h, a
    ld a, l
    rra
    ld a, h
    rra
    xor l
    ld l, a
    xor h
    ld h, a
    ld [wRandState], a
    ld a, l
    ld [wRandState + 1], a
    ret
