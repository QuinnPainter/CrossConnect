#include "sdk/video.h"
#include "sdk/system.h"
#include "sdk/hardware.h"
#include "sdk/joypad.h"
#include "sdk/interrupt.h"
#include "sdk/oam.h"
#include "ingamemenu.h"
#include "gameassets.h"
#include "text.h"
#include "levelselect.h"
#include "bcd.h"
#include "cursor.h"
#include "game.h"

#define WINDOW_SHOWN_Y 104
#define WINDOW_HIDDEN_Y 146 // it's offscreen at 144, just looks more natural if it's a bit over

#define MENU_CURSOR_Y (OAM_Y_OFS + 128)

uint16_t winYPos;

void drawPauseMenu()
{
    drawTilemap((uint8_t*)0x9C00, ingameMenuTilemap, ingameMenuTilemap_end, 0x89);
    copyStringVRAM(PauseMenuOptionsString, (uint8_t*)0x9C62);
    const uint8_t* lvlpackName = lvlDescArr[(lvlSelectPack * 2) + 1];
    copyStringVRAM(lvlpackName, (uint8_t*)0x9C21);
    drawBCD8(bcd_conv(lvlSelected + 1), (uint8_t*)(0x9C21 + 1 + strlength(lvlpackName)));
}

void drawWinMenu()
{
    drawTilemap((uint8_t*)0x9C00, ingameMenuTilemap, ingameMenuTilemap_end, 0x89);
    copyStringVRAM(GameWinString, (uint8_t*)0x9C26);
    copyStringVRAM(WinMenuOptionsString, (uint8_t*)0x9C65);
}

void ingameMenuLoop(bool isWinMenu)
{
    if (isWinMenu) { drawWinMenu(); }
    else { drawPauseMenu(); }

    cursorState = CURSOR_STATE_INGAMEMENU;
    uint8_t prevCursorX = cursorBoardX;
    uint8_t prevCursorY = cursorBoardY;
    cursorTargetY = (uint8_t)MENU_CURSOR_Y;

    winYPos = (uint16_t)WINDOW_HIDDEN_Y << 8;
    while (rWY != WINDOW_SHOWN_Y)
    {
        smoothSlide(&winYPos, WINDOW_SHOWN_Y, 2);
        rWY = winYPos >> 8;
        HALT();
    }
    while (1)
    {
        joypad_update();
        updateCursorMovement();

        if (joypad_pressed & PAD_START)
        {
            break;
        }
        HALT();
    }
    cursorBoardX = prevCursorX;
    cursorBoardY = prevCursorY;
    ingameUpdateCursorPosition();
    cursorState = CURSOR_STATE_INGAME;
    while (rWY != WINDOW_HIDDEN_Y)
    {
        smoothSlide(&winYPos, WINDOW_HIDDEN_Y, 2);
        rWY = winYPos >> 8;
        HALT();
    }
}

void ingameMenuProcessMove()
{}
