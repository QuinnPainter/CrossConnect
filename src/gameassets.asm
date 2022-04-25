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
_testLevels::
    INCBIN "_build/levels/testlevels.bin"
_testLevels_end::
_testLevels2::
    INCBIN "_build/levels/testlevels2.bin"
_testLevels2_end::

SECTION "LevelDescriptionArray", ROM0
_lvlDescArr::
DW _testLevels
DW _TestlevelsName
DW _testLevels2
DW _Testlevels2Name
_lvlDescArr_end::

; would like to simplify this, but...
; to add new levels:
; 1. add levelpack bin to SECTION "Levels" above
; 2. add name string to text.asm
; 3. create entry in LevelDescriptionArray above
; 4. increase NUM_LEVEL_PACKS in gameassets.h
; 5. increase NUM_LVL_PACKS in savegame.asm
