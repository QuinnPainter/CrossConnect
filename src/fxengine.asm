INCLUDE "sdk/hardware.inc"

; Sound array header:
; Byte 1 = Starting Sound Priority

; Format of sound array entries:
; Byte 1 = Command
; FF = Stop Sound
; FE = Change Priority
; FD = Change CH1 Pan
; FC = Change CH2 Pan
; FB = Change CH3 Pan
; FA = Change CH4 Pan
; Everything else = Set Register

; Byte 2 = Command Parameter
; for Change Priority, this is the priority number
; for Set Register, this is the value the reg gets set to

; Byte 3 = Frame Wait
; Number of frames to wait before next command

SECTION "FX Engine RAM", WRAM0
FXAddr: DS 2       ; Address of the current position in the command array
FXWaitTime: DS 1   ; Number of frames left to wait until next command
FXPlaying: DS 1    ; Is a sound effect playing? 0 or 1
FXPriority: DS 1   ; Current sound priority. Higher priority takes precedence

SECTION "Sound FX Code", ROM0

; Initialise when program starts
; Sets - A to 0
_initFXEngine::
    ; Init sound registers
    ld a, $FF
    ldh [rAUDENA], a ; Turn on sound controller
    ldh [rAUDTERM], a ; Enable all channels
    ld a, $77
    ldh [rAUDVOL], a ; Set master volume to max

    xor a
    ld [FXPlaying], a
    ret

; Start playing a sound
; Input - HL (ASM), DE (C) = Starting address of sound data
; Sets - A B H L to garbage
_playNewFX::
    ld h, d
    ld l, e
PlayNewFX::
    ld a, [FXPlaying]       ; \
    and a                   ; | skip priority check if no sound is playing
    jr z, .setFX            ; /

    ld a, [FXPriority]
    ld b, a
    ld a, [hl]
    cp b ; c unset if FXPriority <= new fx priority
    ret c ; new fx priority < FXPriority, so don't play anything

.setFX:
    ld a, [hli]
    ld [FXPriority], a   ; set new FXPriority
    ld a, l              ; \
    ld [FXAddr], a       ; | set new FXAddr
    ld a, h              ; |
    ld [FXAddr + 1], a   ; /
    ld a, 1
    ld [FXWaitTime], a  ; FXWaitTime = 1
    ld [FXPlaying], a   ; FXPlaying = 1
    ret

; Update the current sound effect
; Run this once every frame
; Sets - A C H L to garbage
_updateFXEngine::
    ; Check if a sound is playing
    ld a, [FXPlaying]
    and a
    ret z ; no fx playing
    ; Update remaining wait time
    ld hl, FXWaitTime
    dec [hl]
    ret nz ; if wait time is nonzero, don't play any effect
.processCommand:
    ld hl, FXAddr        ; \
    ld a, [hli]          ; | Load address stored in FXAddr
    ld h, [hl]           ; | into HL
    ld l, a              ; /
    ld a, [hli]          ; A = Command byte
    ld c, a
    inc c ; cp $FF
    jr z, .stopSound
    inc c ; cp $FE
    jr z, .changePriority
    inc c ; cp $FD
    jr z, .ch1Pan
    inc c ; cp $FC
    jr z, .ch2Pan
    inc c ; cp $FB
    jr z, .ch3Pan
    inc c ; cp $FA
    jr z, .ch4Pan
    ; Set Register command
    ld c, a     ; C = Pointer to sound register
    ld a, [hli] ; A = Command Parameter
    ld [$FF00+C], a ; Set register
    jr .doneProcessCommand
.stopSound:
    xor a
    ld [FXPlaying], a
    ret ; don't bother doing any more processing, sound is over
.changePriority:
    ld a, [hli] ; A = Command Parameter
    ld [FXPriority], a
    jr .doneProcessCommand
.ch1Pan:
    ldh a, [rNR51]
    and %11101110
    or [hl]
    inc hl
    ldh [rNR51], a
    jr .doneProcessCommand
.ch2Pan:
    ldh a, [rNR51]
    and %11011101
    or [hl]
    inc hl
    ldh [rNR51], a
    jr .doneProcessCommand
.ch3Pan:
    ldh a, [rNR51]
    and %10111011
    or [hl]
    inc hl
    ldh [rNR51], a
    jr .doneProcessCommand
.ch4Pan:
    ldh a, [rNR51]
    and %01110111
    or [hl]
    inc hl
    ldh [rNR51], a
.doneProcessCommand:
    ld a, [hli] ; A = Frames to wait
    and a
    ld [FXWaitTime], a
    ld a, l                 ; \
    ld [FXAddr], a          ; | Write back new FXAddr
    ld a, h                 ; |
    ld [FXAddr + 1], a      ; /
    jr z, .processCommand   ; If frames is 0, process another command
    ret
