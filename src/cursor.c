#include "sdk/oam.h"
#include "sdk/hardware.h"
#include "sdk/system.h"
#include "sdk/joypad.h"
#include "cursor.h"
#include "game.h"

#define CURSOR_ANIM_SPEED 8 // number of frames between anim frames

uint8_t cursorAnimCtr = CURSOR_ANIM_SPEED;
uint8_t cursorTargetX = 0;
uint8_t cursorTargetY = 0;
uint16_t cursorCurX = 0; // 8.8 fixed point
uint16_t cursorCurY = 0;

uint8_t cursorBoardPrevX = 1; // Old cursor board position
uint8_t cursorBoardPrevY = 1;
uint8_t cursorBoardX = 1; // Current cursor board position
uint8_t cursorBoardY = 1;
uint8_t cursorMoveDirection = 0; // Which direction the cursor moved. If 0, did not move.

// Delayed Auto-Shift variables
#define DAS_DELAY 20 // Delay after starting to hold the button before the auto-shift activates
#define DAS_SPEED 4 // Once activated, this is the delay between each simulated button press
enum dasStates {
    DAS_WAITING, // nothing held / button just started to be held, delaying for a while
    DAS_ACTIVE // active and repeatedly pressing the button
};
uint8_t das_state = DAS_WAITING;
uint8_t das_direction = 0;
uint8_t das_timer = 0;

void processDpadPress(uint8_t dpadState)
{
    if (dpadState & (PAD_LEFT | PAD_RIGHT))
    {
        cursorBoardPrevX = cursorBoardX; cursorBoardPrevY = cursorBoardY;
        if (dpadState & PAD_LEFT) {  cursorBoardX--; cursorMoveDirection = DIR_RIGHT; }
        else if (dpadState & PAD_RIGHT) { cursorBoardX++; cursorMoveDirection = DIR_LEFT; }
        processMove();
    }
    if (dpadState & (PAD_UP | PAD_DOWN))
    {
        cursorBoardPrevX = cursorBoardX; cursorBoardPrevY = cursorBoardY;
        if (dpadState & PAD_UP) { cursorBoardY--; cursorMoveDirection = DIR_DOWN; }
        else if (dpadState & PAD_DOWN) { cursorBoardY++; cursorMoveDirection = DIR_UP; }
        processMove();
    }
}

void updateCursorMovement()
{
    processDpadPress(joypad_pressed);
    if ((joypad_state & (PAD_LEFT | PAD_RIGHT | PAD_UP | PAD_DOWN)) != das_direction)
    {
        // input changed, so restart DAS
        das_state = DAS_WAITING;
        das_timer = DAS_DELAY;
        das_direction = joypad_state & (PAD_LEFT | PAD_RIGHT | PAD_UP | PAD_DOWN);
    }

    das_timer--;
    if (das_timer == 0)
    {
        das_timer = DAS_SPEED;
        if (das_state == DAS_WAITING)
        {
            das_state = DAS_ACTIVE;
        }
        else//if (das_state == DAS_ACTIVE)
        {
            processDpadPress(das_direction);
        }
    }
}

void updateCursorAnimation()
{
    cursorAnimCtr--;
    if (cursorAnimCtr == 0)
    {
        cursorAnimCtr = CURSOR_ANIM_SPEED;
        if (cpu_type == CPU_CGB)
        {
            // Loop through palettes 0-3 (also sets all other attributes to 0, may want to change)
            uint8_t c = (shadow_oam[0].attr + 1) & 3;
            shadow_oam[0].attr = c; // game cursor sprite
            shadow_oam[1].attr = c; // menu cursor sprite
            if (c == 3)
            {
                cursorAnimCtr = CURSOR_ANIM_SPEED * 3; // extend the solid colour frame
            }
        }
        else // DMG
        {
            // sdcc doesn't optimise this into a single rotation, even though it's supposed to. ugh.
            rOBP0 = ((rOBP0 >> 2) | (rOBP0 << 6)); // rotate right by 2
            if ((rOBP0 & 0b11000000) == 0b11000000)
            {
                cursorAnimCtr = CURSOR_ANIM_SPEED * 3; // extend the solid colour frame
            }
        }
    }

    // lerp the cursor position
    // int16_t is so the number is sign-extended in the / 2
    // + 0x7F is so it aims for the "middle subpixel", helping it move into the right position
    // this line produces some truly awful code. could optimise this in asm at some point
    cursorCurX -= (int16_t)(cursorCurX - (((uint16_t)cursorTargetX) << 8) + 0x7F) / 2;
    cursorCurY -= (int16_t)(cursorCurY - (((uint16_t)cursorTargetY) << 8) + 0x7F) / 2;

    // the lerped position is off by 1 for some reason.
    shadow_oam[0].y = (cursorCurY >> 8) + 1;
    shadow_oam[0].x = (cursorCurX >> 8) + 1;
}
