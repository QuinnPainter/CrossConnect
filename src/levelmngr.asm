SECTION "level_mngr_ram", WRAM0
_curLevelPackAddr:: ds 2

SECTION "level_mngr_functions", ROM0

_loadLevel::
    ld b, b
    ld hl, sp+2
    ld b, [hl] ; Load level index into B
    ld hl, _curLevelPackAddr ; Load level pack start address into HL
    ld a, [hli]
    ld h, [hl]
    ld l, a
    ret