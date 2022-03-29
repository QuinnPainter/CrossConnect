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
    INCBIN "_build/assets/font.2bpp"
_fontTiles_end::
_backgroundTiles::
    INCBIN "_build/assets/background.2bpp"
_backgroundTiles_end::
_cursorTiles::
    INCBIN "_build/assets/cursor.2bpp"
_cursorTiles_end::
_nodeNumberTiles::
    INCBIN "_build/assets/nodeNumbers.2bpp"
_nodeNumberTiles_end::
_nodeNumberTilesCGB::
    INCBIN "_build/assets/nodeNumbersCGB.2bpp"
_nodeNumberTilesCGB_end::
_nodeShapeTiles::
    INCBIN "_build/assets/nodeShapes.2bpp"
_nodeShapeTiles_end::
_connectionTiles::
    INCBIN "_build/assets/connections.2bpp"
_connectionTiles_end::

_mainMenuTiles::
    INCBIN "_build/assets/mainmenu.2bpp"
_mainMenuTiles_end::
_mainMenuTilemap::
    INCBIN "_build/assets/mainmenu.tilemap"
_mainMenuTilemap_end::

SECTION "Testlevels", ROM0
_testLevels::
    INCBIN "_build/levels/testlevels.bin"
_testLevels_end::

;ASSET(backgroundTiles, "background.2bpp");
;ASSET(cursorTiles, "cursor.2bpp");
;ASSET(nodeNumberTiles, "nodeNumbers.2bpp");
;ASSET(nodeNumberTilesCGB, "nodeNumbersCGB.2bpp");
;ASSET(nodeShapeTiles, "nodeShapes.2bpp");
;ASSET(connectionTiles, "connections.2bpp");
;LEVELPACK(testLevels, "testlevels.bin");