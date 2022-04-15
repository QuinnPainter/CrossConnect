#include "sdk/hardware.h"
#include "sdk/interrupt.h"
#include "sdk/oam.h"
#include "sdk/joypad.h"
#include "levelselect.h"
#include "bcd.h"
#include "text.h"
#include "cursor.h"

#define CURSOR_X_OFFSET ((8 * 2) + OAM_X_OFS)
#define CURSOR_Y_OFFSET ((8 * 5) + OAM_Y_OFS)

void levelSelectLoop()
{
    clearScreenWithBorder();

    bcd8 levelNum = 0;
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

    cursorState = CURSOR_STATE_LVLSELECT;
    cursorBoardX = 1; cursorBoardY = 1; // todo - save this
    lvlSelectProcessMove();

    while(1)
    {
        joypad_update();
        updateCursorMovement();
        HALT();
    }
}

void lvlSelectProcessMove()
{
    if (cursorBoardY > 6 || cursorBoardY < 1)
    {
        cursorBoardY = cursorBoardPrevY;
    }
    if (cursorBoardX > 5 || cursorBoardX < 1)
    {
        // todo - transition between pages if available
        cursorBoardX = cursorBoardPrevX;
    }

    cursorTargetY = ((cursorBoardY - 1) * (8 * 2)) + CURSOR_Y_OFFSET;
    cursorTargetX = ((cursorBoardX - 1) * (8 * 3)) + CURSOR_X_OFFSET;
}
