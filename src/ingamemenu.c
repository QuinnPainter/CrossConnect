#include "sdk/video.h"
#include "sdk/system.h"
#include "ingamemenu.h"
#include "gameassets.h"
#include "text.h"
#include "levelselect.h"
#include "bcd.h"

void drawPauseMenu()
{
    drawTilemap((uint8_t*)0x9C00, ingameMenuTilemap, ingameMenuTilemap_end, 0x89);
    copyStringVRAM(PauseMenuOptionsString, (uint8_t*)0x9C62);
    const uint8_t* lvlpackName = lvlDescArr[(lvlSelectPack * 2) + 1];
    copyStringVRAM(lvlpackName, (uint8_t*)0x9C21);
    drawBCD8(bcd_conv(lvlSelected + 1), (uint8_t*)(0x9C21 + 1 + strlength(lvlpackName)));
}
