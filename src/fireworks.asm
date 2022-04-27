INCLUDE "sdk/hardware.inc"

; use this for inspiration :)
; https://codepen.io/MillerTime/pen/XgpNwb

RSRESET
DEF FIREWORK_XPOS RB 2 ; 8.8 fixed point
DEF FIREWORK_XVEL RB 2 ; int part is first byte, fraction part second byte
DEF FIREWORK_YPOS RB 2
DEF FIREWORK_YVEL RB 2
DEF FIREWORK_SIZE RB 0

DEF FIREWORK_FIRST_SPRITE EQU 1 ; Sprite index of the first firework sprite
DEF NUM_FIREWORK_SPRITES EQU 1
DEF AIR_RESISTANCE EQU $0030 ; Amount the firework X velocity decreases by every frame. 8.8 fixed
DEF GRAVITY EQU $0030 ; Amount the firework Y velocity decreases by each frame. 8.8 fixed

SECTION "FireworksRAM", WRAM0
FireworkArray: DS FIREWORK_SIZE * NUM_FIREWORK_SPRITES

SECTION "FireworksCode", ROM0
_startFireworks::
    ret

_updateFireworks::
    ld hl, FireworkArray
.mainlp:
    push hl
    ld a, [hli] ; BC = FIREWORK_XPOS
    ld b, a
    ld a, [hli]
    ld c, a
    ld a, [hli] ; DE = FIREWORK_XVEL
    ld d, a
    ld a, [hli]
    ld e, a

    add c ; BC = FIREWORK_XPOS + FIREWORK_XVEL
    ld c, a
    ld a, d
    adc b
    ld b, a

    pop hl ; reset HL back to FIREWORK_XPOS
    ld a, b ; save new FIREWORK_XPOS
    ld [hli], a
    ld a, c
    ld [hli], a

    push hl ; Apply air resistance to FIREWORK_XVEL
    bit 7, d
    jr z, .xvelPositive
    ld hl, AIR_RESISTANCE
    jr :+
.xvelPositive:
    ld hl, -AIR_RESISTANCE
:   add hl, de
    ld d, h
    ld e, l
    pop hl
    ld a, d
    ld [hli], a
    ld a, e
    ld [hli], a

    ret
