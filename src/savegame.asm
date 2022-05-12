INCLUDE "sdk/hardware.inc"

DEF NUM_LVL_PACKS EQU 13
; 90 levels per pack / 8 levels per byte. >> 16 to convert from fixed point to int.
; Defined for C in levelselect.h
DEF LVL_PACK_SAVE_SIZE EQU (CEIL(90.0 / 8) >> 16)

SECTION "SaveVerifyString", ROM0
DB "CROSSSAV1"

SECTION "SavedVariablesMirror", WRAM0
_nodeStyle:: DB
_lvlSelectPage:: DB ; Current page within the pack, goes from 0 to 2 (3 pages of 30 levels)
_lvlSelectPack:: DB ; Index of the current level pack (todo: save these)
_solvedLevels:: DS LVL_PACK_SAVE_SIZE * NUM_LVL_PACKS

SECTION "SRAMSave", SRAM
SaveVerifyStringSRAM: DS SIZEOF("SaveVerifyString")
saveData: DS SIZEOF("SavedVariablesMirror")

SECTION "SavingCode", ROM0
_saveGame::
    ld a, $0A
    ld [rRAMG], a ; Enable SRAM
    ld de, SIZEOF("SavedVariablesMirror")
    push de
    ld bc, STARTOF("SavedVariablesMirror")
    ld de, saveData
    call ___memcpy
    xor a
    ld [rRAMG], a ; Disable SRAM
    ret


_loadSaveGame::
    ld a, $0A
    ld [rRAMG], a ; Enable SRAM

    ld de, STARTOF("SaveVerifyString")
    ld hl, SaveVerifyStringSRAM
    ld c, SIZEOF("SaveVerifyString")
.checkVerifyStringLoop:
    ld a, [de]
    cp [hl]
    jr nz, .verifyStringInvalid
    inc hl
    inc de
    dec c
    jr nz, .checkVerifyStringLoop
    jr .verifyStringValid
.verifyStringInvalid:
    ; Copy in verification string
    ld de, SIZEOF("SaveVerifyString")
    push de
    ld bc, STARTOF("SaveVerifyString")
    ld de, SaveVerifyStringSRAM
    call ___memcpy
    ; Initialise Save RAM
    ld de, SIZEOF("SRAMSave") - SIZEOF("SaveVerifyString")
    push de
    ld de, saveData
    ld c, 0
    call _memset
.verifyStringValid:

    ld de, SIZEOF("SavedVariablesMirror")
    push de
    ld bc, saveData
    ld de, STARTOF("SavedVariablesMirror")
    call ___memcpy

    xor a
    ld [rRAMG], a ; Disable SRAM
    ret
