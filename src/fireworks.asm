INCLUDE "sdk/hardware.inc"

; use this for inspiration :)
; https://codepen.io/MillerTime/pen/XgpNwb

RSRESET
DEF FIREWORK_XPOS RB 2 ; 8.8 fixed point
DEF FIREWORK_XVEL RB 2 ; int part is first byte, fraction part second byte
DEF FIREWORK_YPOS RB 2
DEF FIREWORK_YVEL RB 2
DEF FIREWORK_SIZE RB 0

DEF FIREWORK_TILEINDEX EQU $02
DEF FIREWORK_FIRST_SPRITE EQU 1 ; Sprite index of the first firework sprite
DEF NUM_FIREWORK_SPRITES EQU 1
DEF AIR_RESISTANCE EQU $0019 ; Amount the firework X velocity decreases by every frame. 8.8 fixed
DEF GRAVITY EQU $0023 ; Amount the firework Y velocity decreases by each frame. 8.8 fixed

SECTION "FireworksRAM", WRAM0
FireworkArray: DS FIREWORK_SIZE * NUM_FIREWORK_SPRITES

SECTION "FireworksCode", ROM0
_startFireworks::
    ld hl, FireworkArray
    ld a, $50
    ld [hli], a
    ld [hli], a ; xpos
    ld a, $04
    ld [hli], a
    ld [hli], a ; xvel

    ld a, $50
    ld [hli], a
    ld [hli], a ; ypos
    ld a, $04
    ld [hli], a
    ld [hli], a ; yvel

    ld a, FIREWORK_TILEINDEX
    ld [wShadowOAM + (FIREWORK_FIRST_SPRITE * sizeof_OAM_ATTRS) + OAMA_TILEID], a
    xor a
    ld [wShadowOAM + (FIREWORK_FIRST_SPRITE * sizeof_OAM_ATTRS) + OAMA_FLAGS], a

    ret

_updateFireworks::
    ld hl, FireworkArray
.mainlp:
    ; Update X position
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

    ld a, b
    ld [wShadowOAM + (FIREWORK_FIRST_SPRITE * sizeof_OAM_ATTRS) + OAMA_X], a

    ; Apply air resistance to X velocity
    push hl
     ld a, e ; Set X velocity to 0 if it's under a certain threshold
     and %11000000
     or d
     ld h, d ; only matters when d is 0, so basically just HL = 0
     ld l, d
     jr z, .setXvel

     bit 7, d
     jr z, .xvelPositive
     ld hl, AIR_RESISTANCE
     jr :+
.xvelPositive:
     ld hl, -AIR_RESISTANCE
:    add hl, de
.setXvel:
     ld d, h
     ld e, l
    pop hl
    ld a, d
    ld [hli], a
    ld a, e
    ld [hli], a

    ; Update Y Position
    push hl
     ld a, [hli] ; BC = FIREWORK_YPOS
     ld b, a
     ld a, [hli]
     ld c, a
     ld a, [hli] ; DE = FIREWORK_YVEL
     ld d, a
     ld a, [hli]
     ld e, a

     add c ; BC = FIREWORK_YPOS + FIREWORK_YVEL
     ld c, a
     ld a, d
     adc b
     ld b, a

    pop hl ; reset HL back to FIREWORK_YPOS
    ld a, b ; save new FIREWORK_YPOS
    ld [hli], a
    ld a, c
    ld [hli], a

    ld a, b
    ld [wShadowOAM + (FIREWORK_FIRST_SPRITE * sizeof_OAM_ATTRS) + OAMA_Y], a

    ; Apply gravity to Y Position
    push hl
     ld hl, GRAVITY
     add hl, de
     ld d, h
     ld e, l
    pop hl
    ld a, d
    ld [hli], a
    ld a, e
    ld [hli], a
    ret
