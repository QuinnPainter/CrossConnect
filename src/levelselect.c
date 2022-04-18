#include "sdk/hardware.h"
#include "sdk/interrupt.h"
#include "sdk/oam.h"
#include "sdk/joypad.h"
#include "levelselect.h"
#include "bcd.h"
#include "text.h"
#include "cursor.h"
#include "game.h"
#include "gameassets.h"
#include "levelmngr.h"

#define CURSOR_X_OFFSET ((8 * 2) + OAM_X_OFS)
#define CURSOR_Y_OFFSET ((8 * 5) + OAM_Y_OFS)

uint8_t lvlSelectPage = 0; // Current page within the pack, goes from 0 to 2 (3 pages of 30 levels)
uint8_t lvlSelectPack = 0; // Index of the current level pack (todo: save these)

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

    copyStringVRAM(lvlDescArr[(lvlSelectPack * 2) + 1], (uint8_t*)0x9843);

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
            runGame(((cursorBoardY - 1) * 5) + (cursorBoardX - 1) + (lvlSelectPage * 30));
            drawLevelSelect();
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
        if ((lvlSelectPack == NUM_LEVEL_PACKS - 1) && (lvlSelectPage == 2))
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
        if ((lvlSelectPack == 0) && (lvlSelectPage == 0))
        {
            // at the left end, so cancel cursor move
            cursorBoardX = cursorBoardPrevX;
        }
        else
        {
            // switch pages left
            lvlSelectPage--;
            if (lvlSelectPage == 0xFF) // wrapped from 0
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
