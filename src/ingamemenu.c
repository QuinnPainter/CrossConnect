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
#include "savegame.h"
#include "fireworks.h"
#include "fxengine.h"
#include "soundfx.h"

#define WINDOW_SHOWN_Y 104
#define WINDOW_HIDDEN_Y 146 // it's offscreen at 144, just looks more natural if it's a bit over

#define MENU_CURSOR_Y (OAM_Y_OFS + 128)

// why is a cast to uint8_t necessary here??? stupid sdcc and your "overflow in implicit constant conversion"
const uint8_t cursorXPositions[] = {OAM_X_OFS + 8, OAM_X_OFS + 64, (uint8_t)(OAM_X_OFS + 112), OAM_X_OFS + 32, OAM_X_OFS + 80};

enum ingameMenuOptions {
    PAUSEMENU_RESET = 1,
    PAUSEMENU_SKIP = 2,
    PAUSEMENU_MENU = 3,
    WINMENU_NEXT = 4,
    WINMENU_MENU = 5
};

bool isWinMenu;
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

uint8_t ingameMenuLoop()
{
    if (isWinMenu) { drawWinMenu(); startFireworks(); }
    else { drawPauseMenu(); }

    cursorState = CURSOR_STATE_INGAMEMENU;
    uint8_t prevCursorX = cursorBoardX;
    uint8_t prevCursorY = cursorBoardY;
    cursorTargetY = (uint8_t)MENU_CURSOR_Y;
    cursorBoardX = isWinMenu ? WINMENU_NEXT : PAUSEMENU_RESET;
    ingameMenuProcessMove(0);

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

        if ((joypad_pressed & (PAD_START | PAD_B)) && !isWinMenu)
        {
            break;
        }
        if (joypad_pressed & PAD_A)
        {
            clearFireworks();
            switch (cursorBoardX)
            {
                case PAUSEMENU_RESET: eraseAllConnections(); playNewFX(FX_Reset); goto DONE_LOOP;
                case PAUSEMENU_SKIP:
                case WINMENU_NEXT: rWY = WINDOW_HIDDEN_Y; return INGAMEMENU_RESULT_NEXT;
                case PAUSEMENU_MENU:
                case WINMENU_MENU: rWY = WINDOW_HIDDEN_Y; return INGAMEMENU_RESULT_MENU;
            }
        }
        if (isWinMenu) { updateFireworks(); }
        HALT();
    }
    playNewFX(FX_Unpause);
DONE_LOOP:
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
    return INGAMEMENU_RESULT_DONOTHING;
}

void ingameMenuProcessMove(uint8_t dpadState)
{
    if (isWinMenu)
    {
        if (cursorBoardX > WINMENU_MENU || cursorBoardX < WINMENU_NEXT)
        {
            // using a goto here is more efficient than just putting another "cursorBoardX = cursorBoardPrevX";
            goto RESET_X;
        }
        else if (dpadState & (PAD_LEFT | PAD_RIGHT)) { playNewFX(FX_MenuBip); }   
    }
    else
    {
        if (cursorBoardX > PAUSEMENU_MENU || cursorBoardX < PAUSEMENU_RESET)
        {
RESET_X:
            cursorBoardX = cursorBoardPrevX;
        }
        else if (dpadState & (PAD_LEFT | PAD_RIGHT)) { playNewFX(FX_MenuBip); }
    }
    cursorTargetX = cursorXPositions[cursorBoardX - 1];
}
