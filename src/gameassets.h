#ifndef GAMEASSETS_H
#define GAMEASSETS_H

#include "sdk/assets.h"

EXTERN_ASSET(fontTiles);
EXTERN_ASSET(backgroundTiles);
EXTERN_ASSET(spriteTiles);
EXTERN_ASSET(nodeNumberTiles);
EXTERN_ASSET(nodeNumberTilesCGB);
EXTERN_ASSET(nodeShapeTiles);
EXTERN_ASSET(connectionTiles);
EXTERN_ASSET(testLevels);

EXTERN_ASSET(mainMenuTiles);
EXTERN_ASSET(mainMenuTilemap);

#define TILE_CURSOR 0x00
#define TILE_MENUCURSOR 0x01
#define TILE_CONNECT1 0x90
#define TILE_CONNECTALT 0x00
#define TILE_NUMNODE1 0xA0
#define TILE_SHAPENODE1 0xB0
#define TILE_BGEMPTY TILE_CONNECT1
#define TILE_BGFILLED 0x80
#define TILE_BGTOPLEFT 0x81   // I would stack these like TILE_BGFILLED + 1, but then sdcc gives
#define TILE_BGTOPRIGHT 0x82  // "overflow in implicit constant conversion" warnings, for some reason.
#define TILE_BGBOTTOMLEFT 0x83
#define TILE_BGBOTTOMRIGHT 0x84
#define TILE_BGTOP 0x85
#define TILE_BGBOTTOM 0x86
#define TILE_BGLEFT 0x87
#define TILE_BGRIGHT 0x88

#endif//GAMEASSETS_H
