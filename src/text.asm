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
CHARMAP " ", $07

SECTION "StringRoutines", ROM0

_copyString::
    pop de ; DE = Return address
    pop hl ; HL = Source address
    pop bc ; BC = Dest address
    push bc
    push hl
    push de
.lp:
    ld a, [hli]
    or a
    ret z
    ld [bc], a
    inc bc
    jr .lp

SECTION "PlayString", ROM0
_PlayString:: DB "PLAY", 0
SECTION "StyleString", ROM0
_StyleString:: DB "STYLE", 0
SECTION "AboutString", ROM0
_AboutString:: DB "ABOUT", 0