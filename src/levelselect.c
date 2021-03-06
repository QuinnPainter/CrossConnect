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
#include "savegame.h"
#include "fxengine.h"
#include "soundfx.h"

#define CURSOR_X_OFFSET ((8 * 2) + OAM_X_OFS)
#define CURSOR_Y_OFFSET ((8 * 5) + OAM_Y_OFS)
#define RIGHTARROW_POS 0x9952
#define LEFTARROW_POS 0x9941
#define LVLPACKNAME_POS 0x9843

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

    bcd8 levelNumBCD = lvlSelectPage * 0x30;
    uint8_t levelNum = lvlSelectPage * 30;
    uint8_t* curTilemapAddr = (uint8_t*)0x98A3;
    for (uint8_t y = 6; y > 0; y--)
    {
        for (uint8_t x = 5; x > 0; x--)
        {
            levelNumBCD = bcd_inc(levelNumBCD);
            if (getLevelSolved(lvlSelectPack, levelNum))
            {
                drawBCD8Alt(levelNumBCD, curTilemapAddr);
            }
            else
            {
                drawBCD8(levelNumBCD, curTilemapAddr);
            }
            levelNum++;
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
    lvlSelectProcessMove(0);
    while(1)
    {
        joypad_update();
        updateCursorMovement();

        if (joypad_pressed & PAD_B) { return; }
        if (joypad_pressed & (PAD_START | PAD_A))
        {
            curLevelPackAddr = lvlDescArr[lvlSelectPack * 2];
            lvlSelected = ((cursorBoardY - 1) * 5) + (cursorBoardX - 1) + (lvlSelectPage * 30);
            saveGame();
            runGame();
            return;
        }

        HALT();
    }
}

void lvlSelectProcessMove(uint8_t dpadState)
{
    if (cursorBoardY > 6 || cursorBoardY < 1)
    {
        cursorBoardY = cursorBoardPrevY;
    }
    else if (dpadState & (PAD_UP | PAD_DOWN))
    { playNewFX(FX_MenuBip); }

    bool fastMove = joypad_state & PAD_SELECT; // move by pages at a time if Select is held
    if (cursorBoardX > 5 || cursorBoardX < 1 || fastMove)
    {
        if (cursorBoardX > 5 || (fastMove && (dpadState & PAD_RIGHT)))
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
                playNewFX(FX_MenuBip);
            }
        }
        else //if (cursorBoardX < 1)
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
                playNewFX(FX_MenuBip);
            }
        }
    }
    else if (dpadState & (PAD_LEFT | PAD_RIGHT))
    { playNewFX(FX_MenuBip); }

    cursorTargetY = ((cursorBoardY - 1) * (8 * 2)) + CURSOR_Y_OFFSET;
    cursorTargetX = ((cursorBoardX - 1) * (8 * 3)) + CURSOR_X_OFFSET;
}

bool getLevelSolved(uint8_t levelPack, uint8_t levelNum)
{
    uint8_t solvedLvlByte = solvedLevels[(levelPack * LVL_PACK_SAVE_SIZE) + (levelNum / 8)];
    return (solvedLvlByte >> (levelNum & 0x7)) & 0x1;
}

void setLevelSolved(uint8_t levelPack, uint8_t levelNum)
{
    uint8_t* lvlBytePtr = solvedLevels + (levelPack * LVL_PACK_SAVE_SIZE) + (levelNum / 8);
    *lvlBytePtr |= 1 << (levelNum & 0x7);
    saveGame();
}
