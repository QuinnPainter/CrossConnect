INCLUDE "sdk/hardware.inc"

; use this for inspiration :)
; https://codepen.io/MillerTime/pen/XgpNwb

RSRESET
DEF FIREWORK_YPOS RB 2 ; 8.8 fixed point
DEF FIREWORK_YVEL RB 2 ; int part is first byte, fraction part second byte
DEF FIREWORK_XPOS RB 2
DEF FIREWORK_XVEL RB 2
DEF FIREWORK_SIZE RB 0

DEF FIREWORK_STARTY EQU 150 + OAM_Y_OFS ; Y position it starts at before shooting up
DEF FIREWORK_ALIVETIME EQU 17 ; Number of frames after exploding before firework fades out
DEF FIREWORK_FADETIME EQU 6 ; Number of frames between each fade state
DEF FIREWORK_START_YVEL EQU -$0440 ; Starting Y velocity of the first particle that shoots up. 8.8 fixed point
DEF FIREWORK_NUM_FADE_TILES EQU 4 ; Number of animation frames in the fadeout
DEF FIREWORK_TILEINDEX EQU $02
DEF FIREWORK_FIRST_SPRITE EQU 1 ; Sprite index of the first firework sprite
DEF NUM_FIREWORK_SPRITES EQU 30
DEF AIR_RESISTANCE EQU $0002 ; Amount the firework X velocity decreases by every frame. 8.8 fixed
DEF GRAVITY EQU $0014 ; Amount the firework Y velocity decreases by each frame. 8.8 fixed

SECTION "FireworksRAM", WRAM0
wFireworkArray: DS FIREWORK_SIZE * NUM_FIREWORK_SPRITES
wFireworkState: DS 1 ; 0 = Idle, 1 = Shooting up, 2 = Exploding, 3 = Fading out
wFireworkTimer: DS 1 ; Holds frame counts for various states
wFireworkCurTile: DS 1 ; Current sprite tile, used for fading.
wFireworkCurPalette: DS 1 ; Cycles through 4 palettes. Only on CGB

SECTION "FireworksHRAM", HRAM
hFireworkSpritePtr: DB

SECTION "FireworkVelocityTable", ROM0
; angles are measured 0 - 65536.0 in rgbasm
DEF VEL_MULTIPLIER1 EQU 120000
DEF VEL_MULTIPLIER2 EQU 70000
DEF NUM_DIRECTIONS EQU 30
ANGLE = 0.0
REPT NUM_DIRECTIONS / 2
YVEL = MUL(SIN(ANGLE), VEL_MULTIPLIER1) ; 16.16 fixed point number
    db (YVEL >> 16) & $FF ; save integer part
    db (YVEL >> 8) & $FF ; save fractional part
XVEL = MUL(COS(ANGLE), VEL_MULTIPLIER1) ; 16.16 fixed point number
    db (XVEL >> 16) & $FF ; save integer part
    db (XVEL >> 8) & $FF ; save fractional part
ANGLE = ANGLE + ((65536 / (NUM_DIRECTIONS / 2)) << 16) ; left shift to convert to fixed point
ENDR
ANGLE = 0.0
REPT NUM_DIRECTIONS / 2
YVEL = MUL(SIN(ANGLE), VEL_MULTIPLIER2) ; 16.16 fixed point number
    db (YVEL >> 16) & $FF ; save integer part
    db (YVEL >> 8) & $FF ; save fractional part
XVEL = MUL(COS(ANGLE), VEL_MULTIPLIER2) ; 16.16 fixed point number
    db (XVEL >> 16) & $FF ; save integer part
    db (XVEL >> 8) & $FF ; save fractional part
ANGLE = ANGLE + ((65536 / (NUM_DIRECTIONS / 2)) << 16) ; left shift to convert to fixed point
ENDR


SECTION "FireworksCode", ROM0
_startFireworks::
    xor a
    ld [wFireworkCurPalette], a ; initialise the palette
    ld [wFireworkState], a ; go to idle state
    inc a
    ld [wFireworkTimer], a ; fire immediately
    ret

_updateFireworks::
    ld a, [wFireworkState]
    and a
    jr z, .idleState
    dec a
    jr z, .flyingState
    dec a
    jp z, .explodingState
    ; Fading State
    ld hl, wFireworkTimer
    dec [hl]
    jp nz, .explodingStateFade
    ld a, FIREWORK_FADETIME
    ld [hl], a ; reset timer

    ld hl, wFireworkCurTile
    inc [hl]
    ld a, [hl]
    cp a, FIREWORK_TILEINDEX + FIREWORK_NUM_FADE_TILES
    jr z, .doneFade
    ld b, NUM_FIREWORK_SPRITES
    ld hl, wShadowOAM + (FIREWORK_FIRST_SPRITE * sizeof_OAM_ATTRS) + OAMA_TILEID
.setFadeSprites: ; set new fade tiles
    ld [hli], a
    inc hl
    inc hl
    inc hl
    dec b
    jr nz, .setFadeSprites
    jp .explodingStateFade
.doneFade:
    ld a, $01 ; basically no delay, is a delay even necessary? todo: remove delay?
    ld [wFireworkTimer], a ; delay before shooting another firework
    xor a
    ld [wFireworkState], a ; go to idle state
    jp .explodingStateFade

.idleState:
    ld hl, wFireworkTimer
    dec [hl]
    ret nz
    ; setup shooting up state
    ld a, [wFireworkCurPalette]
    inc a
    and %11
    or %100
    ld [wFireworkCurPalette], a ; go to next palette
    ld hl, wShadowOAM + (FIREWORK_FIRST_SPRITE * sizeof_OAM_ATTRS)
    ld a, FIREWORK_STARTY   ; starting Y pos
    ld [hli], a             ; set sprite Y
    push hl
     call genRandom         ; generate random X position
     ld a, h
    pop hl
    and $7F                 ; 0 - 127
    add OAM_X_OFS + 15      ; 15 - 142 (+ OAM_X_OFS)
    ld [hli], a             ; set sprite X
    ld c, a                 ; save X for later
    ld a, FIREWORK_TILEINDEX
    ld [hli], a             ; set sprite tile
    ld a, [wFireworkCurPalette] ;
    or OAMF_PAL1                ; use OBJ PAL 1 on DMG, FireworkCurPalette on CGB
    ld [hli], a             ; set sprite attributes
    ld hl, wFireworkArray       ; use first array entry to store Y data for initial particle
    ld a, FIREWORK_STARTY
    ld [hli], a                 ; set start Y
    ld [hli], a                 ; set subpixel the same as pixel, because why not
    ld a, HIGH(FIREWORK_START_YVEL)
    ld [hli], a                 ; set start Y velocity
    ld a, LOW(FIREWORK_START_YVEL)
    ld [hli], a                 ; set velocity fraction
    ld a, c
    ld [hli], a                 ; set X
    ld a, 1
    ld [wFireworkState], a  ; move to shooting up state
    ret

.flyingState:
    ld hl, wFireworkArray
    ld a, FIREWORK_FIRST_SPRITE * sizeof_OAM_ATTRS
    ldh [hFireworkSpritePtr], a
    call updateParticleY
    ld a, d
    cp $FF ; continue going if particle has velocity < -1
    ret nz
    ; initialise exploding state
    ld hl, wFireworkArray
    ld bc, STARTOF("FireworkVelocityTable")
    ld d, NUM_FIREWORK_SPRITES
.setupArrayLp:
    ld a, [wFireworkArray]  ;
    ld [hli], a             ; load ypos from shooting up particle
    ld [hli], a             ;
    ld a, [bc]      ;
    inc bc          ;
    ld [hli], a     ; yvel
    ld a, [bc]      ;
    inc bc          ;
    ld [hli], a     ;
    ld a, [wFireworkArray + 4]  ;
    ld [hli], a                 ; load xpos from shooting up particle
    ld [hli], a                 ;
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
    ld a, [wFireworkCurPalette] ;
    or OAMF_PAL1                ; use OBJ PAL 1 on DMG, FireworkCurPalette on CGB
    ld [hli], a ; sprite attributes
    inc hl
    inc hl
    dec b
    jr nz, .setupSpritesLp
    ld a, FIREWORK_ALIVETIME
    ld [wFireworkTimer], a
    ld a, 2
    ld [wFireworkState], a ; move to exploding state
    ; fall into explode state
    
.explodingState:
    ld hl, wFireworkTimer
    dec [hl]
    jr nz, .explodingStateFade
    ld a, FIREWORK_FADETIME
    ld [wFireworkTimer], a
    ld a, FIREWORK_TILEINDEX
    ld [wFireworkCurTile], a
    ld a, 3
    ld [wFireworkState], a ; move to fading state
.explodingStateFade:
    ld hl, wFireworkArray
    ld a, FIREWORK_FIRST_SPRITE * sizeof_OAM_ATTRS
    ldh [hFireworkSpritePtr], a
.mainlp:
    call updateParticleY

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

    ; go to next entry
    inc hl
    inc hl

    ; Check if we've done all the particles yet
    ldh a, [hFireworkSpritePtr]
    cp (FIREWORK_FIRST_SPRITE + NUM_FIREWORK_SPRITES) * sizeof_OAM_ATTRS
    jr c, .mainlp
    ret

; Used in both the flying state and exploding state
updateParticleY:
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

    ; Apply gravity to Y Velocity
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
