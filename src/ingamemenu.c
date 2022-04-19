#include "sdk/video.h"
#include "sdk/system.h"
#include "sdk/hardware.h"
#include "sdk/joypad.h"
#include "sdk/interrupt.h"
#include "ingamemenu.h"
#include "gameassets.h"
#include "text.h"
#include "levelselect.h"
#include "bcd.h"
#include "cursor.h"

#define WINDOW_SHOWN_Y 104
#define WINDOW_HIDDEN_Y 146 // it's offscreen at 144, just looks more natural if it's a bit over

uint16_t winYPos;

void drawPauseMenu()
{
    drawTilemap((uint8_t*)0x9C00, ingameMenuTilemap, ingameMenuTilemap_end, 0x89);
    copyStringVRAM(PauseMenuOptionsString, (uint8_t*)0x9C62);
    const uint8_t* lvlpackName = lvlDescArr[(lvlSelectPack * 2) + 1];
    copyStringVRAM(lvlpackName, (uint8_t*)0x9C21);
    drawBCD8(bcd_conv(lvlSelected + 1), (uint8_t*)(0x9C21 + 1 + strlength(lvlpackName)));
}

void pauseMenuLoop()
{
    drawPauseMenu();
    winYPos = (uint16_t)WINDOW_HIDDEN_Y << 8;
    while (rWY != WINDOW_SHOWN_Y)
    {
        smoothSlide(&winYPos, WINDOW_SHOWN_Y, 2);
        rWY = winYPos >> 8;
        HALT();
    }
    cursorState = CURSOR_STATE_INGAMEMENU;
    while (1)
    {
        joypad_update();

        if (joypad_pressed & PAD_START)
        {
            break;
        }
        HALT();
    }
    while (rWY != WINDOW_HIDDEN_Y)
    {
        smoothSlide(&winYPos, WINDOW_HIDDEN_Y, 2);
        rWY = winYPos >> 8;
        HALT();
    }
}

void ingameMenuProcessMove()
{}
