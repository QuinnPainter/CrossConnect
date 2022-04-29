SECTION "level_mngr_ram", WRAM0
_curLevelPackAddr:: ds 2
_curLevelWidth:: ds 1
_curLevelHeight:: ds 1

SECTION "level_mngr_hram", HRAM
hCurNodeIndex: ds 1 ; temp value used to store the current node colour

SECTION "level_mngr_functions", ROM0

_loadLevel:: ; level index is in A
    ld b, a ; save level index into B
    ld hl, _curLevelPackAddr ; Load level pack start address into HL
    ld a, [hli]
    ld h, [hl]
    ld l, a
    ld a, b ; restore level index into A

    or a
    jr z, .doneFindPuzzle ; special case for index 0, HL is already at puzzle

    ld b, 0
:   ld c, [hl] ; BC = first entry in puzzle definition = number of nodes / bytes
    inc c ; skip past num nodes byte and puzzle dimensions byte
    inc c
    add hl, bc ; go to next entry
    dec a
    jr nz, :-
.doneFindPuzzle:

    ; Now HL is at the beginning of the puzzle definition
    ld a, [hli] ; save num nodes
    ldh [hCurNodeIndex], a

    ld a, [hli] ; Save puzzle width and height
    ld b, a
    swap a
    and $0F
    inc a ; increment width and height, so the max size is 16x16 not 15x15
    ld [_curLevelWidth], a
    ld a, b
    and $0F
    inc a
    ld [_curLevelHeight], a

    push hl ; save puzzle definition pointer for later

    ; Clear empty area of the board based on the puzzle's dimensions
    ld hl, _board + 18 + 1
    ld c, a ; C = curLevelHeight
    ld a, [_curLevelWidth]
    ld b, a
    ld a, 18
    sub b
    ld e, a
    ld d, 0 ; DE = 18 - width
.clearHeightLoop:
    ld a, [_curLevelWidth]
    ld b, a
    xor a
.clearWidthLoop:
    ld [hli], a
    dec b
    jr nz, .clearWidthLoop
    add hl, de
    dec c
    jr nz, .clearHeightLoop

    pop bc ; move puzzle definition pointer into BC
.drawNodesLoop:
    ld hl, _board + 18 + 1 ; HL = board[1][1]
    ld a, [bc]
    and $0F ; A = node Y
    ld d, a ; save node Y for later
    add a ; A = Y * 2
    ld e, a ; E = Y * 2
    ld a, d ; restore node Y
    swap a ; A = Y * 16
    ld d, 0
    add e ; A = bottom(Y * 18)
    ld e, a
    rl d ; rotate carry into D, so now DE = Y * 18
    add hl, de ; HL = board[y + 1][1]
    ld a, [bc]
    swap a
    and $0F ; A = node X
    ld e, a
    ld d, 0
    add hl, de ; HL = board[y + 1][x + 1]
    ldh a, [hCurNodeIndex]
    dec a ; take away 1 so it starts at 0
    srl a ; divide by 2 to get colour value
    swap a ; move colour value into top bits
    or $0F ; tile type = node
    ld [hl], a ; board[y + 1][x + 1] = node
    inc bc ; progress puzzle definition pointer
    ldh a, [hCurNodeIndex]
    dec a
    ldh [hCurNodeIndex], a
    jr nz, .drawNodesLoop
    ret