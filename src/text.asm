include "sdk/hardware.inc"

NEWCHARMAP MainCharmap
CHARMAP "A", $20
CHARMAP "B", "A" + 1
CHARMAP "C", "B" + 1
CHARMAP "D", "C" + 1
CHARMAP "E", "D" + 1
CHARMAP "F", "E" + 1
CHARMAP "G", "F" + 1
CHARMAP "H", "G" + 1
CHARMAP "I", "H" + 1
CHARMAP "J", "I" + 1
CHARMAP "K", "J" + 1
CHARMAP "L", "K" + 1
CHARMAP "M", "L" + 1
CHARMAP "N", "M" + 1
CHARMAP "O", "N" + 1
CHARMAP "P", "O" + 1
CHARMAP "Q", "P" + 1
CHARMAP "R", "Q" + 1
CHARMAP "S", "R" + 1
CHARMAP "T", "S" + 1
CHARMAP "U", "T" + 1
CHARMAP "V", "U" + 1
CHARMAP "W", "V" + 1
CHARMAP "X", "W" + 1
CHARMAP "Y", "X" + 1
CHARMAP "Z", "Y" + 1
CHARMAP "0", "Z" + 1
CHARMAP "1", "0" + 1
CHARMAP "2", "1" + 1
CHARMAP "3", "2" + 1
CHARMAP "4", "3" + 1
CHARMAP "5", "4" + 1
CHARMAP "6", "5" + 1
CHARMAP "7", "6" + 1
CHARMAP "8", "7" + 1
CHARMAP "9", "8" + 1
CHARMAP "!", "9" + 1
CHARMAP ".", "!" + 1
CHARMAP " ", $97 ; Empty space
CHARMAP "^", $90 ; Grid square
CHARMAP "[0]", $50 ; Alternate numbers


SECTION "CopyStringVRAM", ROM0
_copyStringVRAM:: ; src is DE, dst is BC
    ld h, d
    ld l, e
.lp:
    ld a, [hli]
    or a
    ret z
    ld d, a
:   ldh a, [rSTAT]
    and a, STATF_BUSY
    jr nz, :-
    ld a, d
    ld [bc], a
    inc bc
    jr .lp

SECTION "CopyFullscreenString", ROM0
_copyFullscreenString:: ; src is DE, dst is BC
    call _copyStringVRAM
    ld a, $E0
    and c           ; return to beginning of line
    add $20         ; go to next line
    ld c, a
    ld a, b
    adc 0
    ld b, a

    ld d, h
    ld e, l

    ld a, [hl]
    cp 1
    jr nz, _copyFullscreenString
    ret

SECTION "StringLength", ROM0
_strlength:: ; src is DE
    ld h, d
    ld l, e
    ld d, 0
.lp:
    ld a, [hli]
    or a
    ld a, d
    ret z ; output in A
    inc d
    jr .lp

SECTION "DrawBCD8", ROM0
_drawBCD8:: ; input num is A, dst is DE
    ld h, d
    ld l, e
    cp a, $10
    jr c, .skipFirstDigit
    ld d, a
    swap a
    and $0F
    add "0"
    ld e, a
:   ldh a, [rSTAT]
    and a, STATF_BUSY
    jr nz, :-
    ld a, e
    ld [hli], a
    ld a, d 
.skipFirstDigit:
    and $0F
    add "0"
    ld e, a
:   ldh a, [rSTAT]
    and a, STATF_BUSY
    jr nz, :-
    ld [hl], e
    ret

; todo: could optimise by making the tile offset an input instead of having 2 functions
_drawBCD8Alt:: ; input num is A, dst is DE
    ld h, d
    ld l, e
    cp a, $10
    jr c, .skipFirstDigit
    ld d, a
    swap a
    and $0F
    add "[0]"
    ld e, a
:   ldh a, [rSTAT]
    and a, STATF_BUSY
    jr nz, :-
    ld a, e
    ld [hli], a
    ld a, d 
.skipFirstDigit:
    and $0F
    add "[0]"
    ld e, a
:   ldh a, [rSTAT]
    and a, STATF_BUSY
    jr nz, :-
    ld [hl], e
    ret


SECTION "PlayString", ROM0
_PlayString:: DB "PLAY", 0
SECTION "StyleString", ROM0
_StyleString:: DB "STYLE", 0
SECTION "AboutString", ROM0
_AboutString:: DB "CREDITS", 0

SECTION "ShapesString", ROM0
_ShapesString:: DB "SHAPES ", 0
SECTION "NumbersString", ROM0
_NumbersString:: DB "NUMBERS", 0
SECTION "HowToString", ROM0
_HowToString:: DB "HOW TO", 0

SECTION "AboutPageString", ROM0
_AboutPageString::
DB "^CROSSCONNECT V0.1", 0
DB "^BY QUINN PAINTER", 0
DB "^       2022", 0
DB 0
DB "^MADE WITH GBSDK", 0
DB 0
DB "^WITCHFONT8", 0
DB "^BY LAVENFURR", 0
DB 0
DB 0
DB "^CHECK OUT THE", 0
DB "^GBDEV COMMUNITY!", 0
DB "^AT GBDEV.IO", 0
DB 0
DB 0
DB "^PRESS B TO RETURN", 0, 1

SECTION "HowToPageString", ROM0
_HowToPageString::
DB "^EACH LEVEL IS A", 0
DB "^GRID WITH SEVERAL", 0
DB "^PAIRS OF NODES. ", 0
DB 0
DB "^YOUR GOAL IS TO", 0
DB "^CONNECT EACH PAIR", 0
DB "^AND FILL THE BOARD", 0
DB 0
DB "^HOLD A TO DRAW", 0
DB "^HOLD B TO DELETE", 0
DB 0
DB "^DISCONNECTED LINES", 0
DB "^ARE AUTOMATICALLY", 0
DB "^DELETED.", 0
DB 0
DB "^PRESS B TO RETURN", 0, 1

SECTION "PauseMenuOptionsString", ROM0
_PauseMenuOptionsString:: DB "RESET  SKIP  MENU", 0

SECTION "WinMenuOptionsString", ROM0
_WinMenuOptionsString:: DB "NEXT  MENU", 0

SECTION "GameWinString", ROM0
_GameWinString:: DB "SOLVED!", 0

SECTION "LevelpackNameStrings", ROM0
_TestlevelsName:: DB "TESTLEVELS", 0
_Testlevels2Name:: DB "TESTLEVELS2", 0
