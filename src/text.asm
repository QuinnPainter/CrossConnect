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


SECTION "PlayString", ROM0
_PlayString:: DB "PLAY", 0
SECTION "StyleString", ROM0
_StyleString:: DB "STYLE", 0
SECTION "AboutString", ROM0
_AboutString:: DB "ABOUT", 0

SECTION "ShapesString", ROM0
_ShapesString:: DB "SHAPES ", 0
SECTION "NumbersString", ROM0
_NumbersString:: DB "NUMBERS", 0

SECTION "AboutPageString", ROM0
_AboutPageString::
DB "^CROSSCONNECT V0.1", 0
DB "^BY QUINN PAINTER", 0
DB 0
DB "^MADE WITH GBSDK", 0
DB 0
DB "^WITCHFONT8", 0
DB "^  BY LAVENFURR", 0
DB 0
DB 0
DB 0
DB 0
DB 0
DB 0
DB 0
DB 0
DB "^PRESS B TO RETURN", 0, 1
