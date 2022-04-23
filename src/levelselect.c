#include "sdk/hardware.h"
#include "sdk/interrupt.h"
#include "sdk/oam.h"
#include "sdk/joypad.h"
#include "sdk/video.h"
#include "levelselect.h"
#include "bcd.h"
#include "text.h"
#include "cursor.h"
#include "game.h"
#include "gameassets.h"
#include "levelmngr.h"

#define CURSOR_X_OFFSET ((8 * 2) + OAM_X_OFS)
#define CURSOR_Y_OFFSET ((8 * 5) + OAM_Y_OFS)
#define RIGHTARROW_POS 0x9952
#define LEFTARROW_POS 0x9941
#define LVLPACKNAME_POS 0x9843

uint8_t lvlSelectPage = 0; // Current page within the pack, goes from 0 to 2 (3 pages of 30 levels)
uint8_t lvlSelectPack = 0; // Index of the current level pack (todo: save these)
uint8_t lvlSelected;

inline bool atLeftmostPage()
{
    return (lvlSelectPack == 0) && (lvlSelectPage == 0);
}

inline bool atRightmostPage()
{
    return (lvlSelectPack == NUM_LEVEL_PACKS - 1) && (lvlSelectPage == 2);
}

void drawLevelSelect()
{
    clearScreenWithBorder();

    bcd8 levelNum = lvlSelectPage * 0x30;
    uint8_t* curTilemapAddr = (uint8_t*)0x98A3;
    for (uint8_t y = 6; y > 0; y--)
    {
        for (uint8_t x = 5; x > 0; x--)
        {
            levelNum = bcd_inc(levelNum);
            drawBCD8(levelNum, curTilemapAddr);
            curTilemapAddr += 3;
        }
        // 17 = proceed to next line, 32 = go down another line
        curTilemapAddr += 17 + 32;
    }

    vram_set(RIGHTARROW_POS, atRightmostPage() ? TILE_BLANK : TILE_RIGHTARROW);
    vram_set(LEFTARROW_POS, atLeftmostPage() ? TILE_BLANK : TILE_LEFTARROW);

    copyStringVRAM(lvlDescArr[(lvlSelectPack * 2) + 1], (uint8_t*)LVLPACKNAME_POS);

    cursorState = CURSOR_STATE_LVLSELECT;
}

void levelSelectLoop()
{
    drawLevelSelect();
    cursorBoardX = 1; cursorBoardY = 1; // todo - save this
    lvlSelectProcessMove();
    while(1)
    {
        joypad_update();
        updateCursorMovement();

        if (joypad_pressed & PAD_B) { return; }
        if (joypad_pressed & (PAD_START | PAD_A))
        {
            curLevelPackAddr = lvlDescArr[lvlSelectPack * 2];
            lvlSelected = ((cursorBoardY - 1) * 5) + (cursorBoardX - 1) + (lvlSelectPage * 30);
            runGame();
            return;
        }

        HALT();
    }
}

void lvlSelectProcessMove()
{
    if (cursorBoardY > 6 || cursorBoardY < 1)
    {
        cursorBoardY = cursorBoardPrevY;
    }
    if (cursorBoardX > 5)
    {
        if (atRightmostPage())
        {
            // at the right end, so cancel cursor move
            cursorBoardX = cursorBoardPrevX;
        }
        else
        {
            // switch pages right
            lvlSelectPage++;
            if (lvlSelectPage > 2)
            {
                lvlSelectPage = 0;
                lvlSelectPack++;
            }
            drawLevelSelect();
            cursorBoardX = 1;
            lvlSelectProcessMove();
        }
    }
    if (cursorBoardX < 1)
    {
        if (atLeftmostPage())
        {
            // at the left end, so cancel cursor move
            cursorBoardX = cursorBoardPrevX;
        }
        else
        {
            // switch pages left
            lvlSelectPage--;
            if ((int8_t)lvlSelectPage < 0)
            {
                lvlSelectPage = 2;
                lvlSelectPack--;
            }
            drawLevelSelect();
            cursorBoardX = 5;
            lvlSelectProcessMove();
        }
    }

    cursorTargetY = ((cursorBoardY - 1) * (8 * 2)) + CURSOR_Y_OFFSET;
    cursorTargetX = ((cursorBoardX - 1) * (8 * 3)) + CURSOR_X_OFFSET;
}
