#include "sdk/oam.h"
#include "sdk/hardware.h"
#include "sdk/system.h"
#include "cursor.h"

#define CURSOR_ANIM_SPEED 8 // number of frames between anim frames

uint8_t cursorAnimCtr = CURSOR_ANIM_SPEED;
uint8_t cursorTargetX = 0;
uint8_t cursorTargetY = 0;
uint16_t cursorCurX = 0; // 8.8 fixed point
uint16_t cursorCurY = 0;

void updateCursorAnimation()
{
    cursorAnimCtr--;
    if (cursorAnimCtr == 0)
    {
        cursorAnimCtr = CURSOR_ANIM_SPEED;
        if (cpu_type == CPU_CGB)
        {
            // Loop through palettes 0-3 (also sets all other attributes to 0, may want to change)
            shadow_oam[0].attr = (shadow_oam[0].attr + 1) & 3;
            if (shadow_oam[0].attr == 3)
            {
                cursorAnimCtr = CURSOR_ANIM_SPEED * 3; // extend the solid colour frame
            }
        }
        else // DMG
        {
            rOBP0 = (rOBP0 >> 2) | (rOBP0 << 6); // rotate right by 2
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
