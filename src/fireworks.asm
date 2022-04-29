INCLUDE "sdk/hardware.inc"

; use this for inspiration :)
; https://codepen.io/MillerTime/pen/XgpNwb

RSRESET
DEF FIREWORK_YPOS RB 2 ; 8.8 fixed point
DEF FIREWORK_YVEL RB 2 ; int part is first byte, fraction part second byte
DEF FIREWORK_XPOS RB 2
DEF FIREWORK_XVEL RB 2
DEF FIREWORK_SIZE RB 0

DEF FIREWORK_TILEINDEX EQU $02
DEF FIREWORK_FIRST_SPRITE EQU 1 ; Sprite index of the first firework sprite
DEF NUM_FIREWORK_SPRITES EQU 14
DEF AIR_RESISTANCE EQU $0011 ; Amount the firework X velocity decreases by every frame. 8.8 fixed
DEF GRAVITY EQU $0015 ; Amount the firework Y velocity decreases by each frame. 8.8 fixed

SECTION "FireworksRAM", WRAM0
wFireworkArray: DS FIREWORK_SIZE * NUM_FIREWORK_SPRITES

SECTION "FireworksHRAM", HRAM
hFireworkSpritePtr: DB

SECTION "FireworkVelocityTable", ROM0
; angles are measured 0 - 65536.0 in rgbasm
DEF YVEL_MULTIPLIER EQU 160000
DEF XVEL_MULTIPLIER EQU 200000
DEF NUM_DIRECTIONS EQU 14
ANGLE = 0.0
REPT NUM_DIRECTIONS
YVEL = MUL(SIN(ANGLE), YVEL_MULTIPLIER) ; 16.16 fixed point number
    db (YVEL >> 16) & $FF ; save integer part
    db (YVEL >> 8) & $FF ; save fractional part
XVEL = MUL(COS(ANGLE), XVEL_MULTIPLIER) ; 16.16 fixed point number
    db (XVEL >> 16) & $FF ; save integer part
    db (XVEL >> 8) & $FF ; save fractional part
ANGLE = ANGLE + ((65536 / NUM_DIRECTIONS) << 16) ; left shift to convert to fixed point
ENDR
PRINTLN ANGLE


SECTION "FireworksCode", ROM0
_startFireworks::
    ld hl, wFireworkArray
    ld bc, STARTOF("FireworkVelocityTable")
    ld d, NUM_FIREWORK_SPRITES
.setupArrayLp:
    ld a, $50   ;
    ld [hli], a ; ypos
    ld [hli], a ;
    ld a, [bc]      ;
    inc bc          ;
    ld [hli], a     ; yvel
    ld a, [bc]      ;
    inc bc          ;
    ld [hli], a     ;
    ld a, $50   ;
    ld [hli], a ; xpos
    ld [hli], a ;
    ld a, [bc]      ;
    inc bc          ;
    ld [hli], a     ; xvel
    ld a, [bc]      ;
    inc bc          ;
    ld [hli], a     ;
    dec d
    jr nz, .setupArrayLp

    ld hl, wShadowOAM + (FIREWORK_FIRST_SPRITE * sizeof_OAM_ATTRS) + OAMA_TILEID
    ld b, NUM_FIREWORK_SPRITES
.setupSpritesLp:
    ld a, FIREWORK_TILEINDEX
    ld [hli], a
    xor a ; set sprite attibutes to 0
    ld [hli], a
    inc hl
    inc hl
    dec b
    jr nz, .setupSpritesLp

    ret

_updateFireworks::
    ld hl, wFireworkArray
    ld a, FIREWORK_FIRST_SPRITE * sizeof_OAM_ATTRS
    ldh [hFireworkSpritePtr], a
.mainlp:
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

    ; Set sprite Y
    ldh a, [hFireworkSpritePtr]
    ld c, a
    ld a, b
    ld b, HIGH(wShadowOAM)
    ld [bc], a
    ld a, c
    inc a
    ldh [hFireworkSpritePtr], a

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

    ; Set sprite X
    ldh a, [hFireworkSpritePtr]
    ld c, a
    ld a, b
    ld b, HIGH(wShadowOAM)
    ld [bc], a
    ld a, c
    add 3 ; skip over tile and attribute, go to next sprite Y pos
    ldh [hFireworkSpritePtr], a

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

    ; Check if we've done all the particles yet
    ldh a, [hFireworkSpritePtr]
    cp (FIREWORK_FIRST_SPRITE + NUM_FIREWORK_SPRITES) * sizeof_OAM_ATTRS
    jr c, .mainlp
    ret
