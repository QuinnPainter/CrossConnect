; Similar to the ASSET system from GBSDK, but it puts each asset
; into a new section so the linker can pack them better.

; Using a macro for this would be ideal, but it causes the build to fail.
; It's to do with the .d files generated using RGBASM's -M flag.
; Not sure if this is a problem with the GBSDK makefile, or a bug in RGBASM.

;MACRO NEWASSET
;SECTION "\1", ROM0
;_\1::
;INCBIN \2
;_\1_end::
;ENDM

SECTION "GraphicAssets", ROM0

_fontTiles::
    INCBIN "_build/assets/font.gbcompress"
_fontTiles_end::
_backgroundTiles::
    INCBIN "_build/assets/background.gbcompress"
_backgroundTiles_end::
_spriteTiles::
    INCBIN "_build/assets/sprites.gbcompress"
_spriteTiles_end::
_nodeNumberTiles::
    INCBIN "_build/assets/nodeNumbers.gbcompress"
_nodeNumberTiles_end::
_nodeNumberTilesCGB::
    INCBIN "_build/assets/nodeNumbersCGB.gbcompress"
_nodeNumberTilesCGB_end::
_nodeShapeTiles::
    INCBIN "_build/assets/nodeShapes.gbcompress"
_nodeShapeTiles_end::
_connectionTiles::
    INCBIN "_build/assets/connections.gbcompress"
_connectionTiles_end::

_mainMenuTiles::
    INCBIN "_build/assets/mainmenu.gbcompress"
_mainMenuTiles_end::
_mainMenuTilemap::
    INCBIN "_build/assets/mainmenu.tilemap"
_mainMenuTilemap_end::

_ingameMenuTiles::
    INCBIN "_build/assets/ingamemenu.gbcompress"
_ingameMenuTiles_end::
_ingameMenuTilemap::
    INCBIN "_build/assets/ingamemenu.tilemap"
_ingameMenuTilemap_end::

SECTION "Levels", ROM0
lvlPack1:
    INCBIN "_build/levels/pack1.bin"
lvlPack2:
    INCBIN "_build/levels/pack2.bin"
lvlPack3:
    INCBIN "_build/levels/pack3.bin"
lvlPack4:
    INCBIN "_build/levels/pack4.bin"
lvlPack5:
    INCBIN "_build/levels/pack5.bin"
lvlPack6:
    INCBIN "_build/levels/pack6.bin"
lvlPack7:
    INCBIN "_build/levels/pack7.bin"
lvlPack8:
    INCBIN "_build/levels/pack8.bin"
lvlPack9:
    INCBIN "_build/levels/pack9.bin"
lvlPack10:
    INCBIN "_build/levels/pack10.bin"
lvlPackBonus1:
    INCBIN "_build/levels/bonus1.bin"
lvlPackBonus2:
    INCBIN "_build/levels/bonus2.bin"
lvlPackBonus3:
    INCBIN "_build/levels/bonus3.bin"

SECTION "LevelDescriptionArray", ROM0
_lvlDescArr::
DW lvlPack1
DW lvlPack1Name
DW lvlPack2
DW lvlPack2Name
DW lvlPack3
DW lvlPack3Name
DW lvlPack4
DW lvlPack4Name
DW lvlPack5
DW lvlPack5Name
DW lvlPack6
DW lvlPack6Name
DW lvlPack7
DW lvlPack7Name
DW lvlPack8
DW lvlPack8Name
DW lvlPack9
DW lvlPack9Name
DW lvlPack10
DW lvlPack10Name
DW lvlPackBonus1
DW lvlPackBonus1Name
DW lvlPackBonus2
DW lvlPackBonus2Name
DW lvlPackBonus3
DW lvlPackBonus3Name
_lvlDescArr_end::

; would like to simplify this, but...
; to add new levels:
; 1. add levelpack bin to SECTION "Levels" above
; 2. add name string to text.asm
; 3. create entry in LevelDescriptionArray above
; 4. increase NUM_LEVEL_PACKS in gameassets.h
; 5. increase NUM_LVL_PACKS in savegame.asm
