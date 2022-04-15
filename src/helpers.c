#include "sdk/video.h"
#include "sdk/system.h"
#include "helpers.h"

/**
 * Moves a coordinate such that it halves it's distance to the target on every call.
 * So it starts at max speed, then slows down as it gets closer to the target.
 * I think this would be considered a logarithmic function?
 * @param srcCoord Pointer to 8.8 fixed point value
 * @param targetPos 8 bit value
 */
void smoothSlide(uint16_t* srcCoord, uint8_t targetPos)
{
    // int16_t is so the number is sign-extended in the / 2
    // - 0x7F is so it aims for the "middle subpixel", helping it move into the right position
    // this line produces some truly awful code. could optimise this in asm at some point
    *srcCoord -= ((int16_t)(*srcCoord - (((uint16_t)targetPos) << 8) + NEGATE(0x7F)) / 2);
}

// Clears background and draws grid tiles on the outer border. Used for the info pages and level select
void clearScreenWithBorder()
{
    uint16_t dstPtr = 0x9800;
    for (uint8_t y = 0; y < 18; y++)
    {
        for (uint8_t x = 0; x < 20; x++)
        {
            // this check might not actually be necessary?
            // since DMG will ignore the rVBK write, 0x7 will get written to tilemap
            // and immediately get overwritten by the next vram_set
            if (cpu_type == CPU_CGB)
            {
                rVBK = 1; // make sure we're on the attribute vram bank
                vram_set(dstPtr, 0x7); // fill bg with BG palette 7
                rVBK = 0;
            }
            // 0x97 = empty tile
            vram_set(dstPtr++, 0x97);
        }
        dstPtr += 12;
    }
    // top and bottom borders
    for (uint16_t i = 0x9800; i < 0x9814; i++)
    {
        vram_set(i, 0x10); // 0x10 = grid tile
        vram_set(i + 0x220, 0x10);
    }
    // left and right borders
    for (uint16_t i = 0x9820; i < 0x9A20; i += 0x20)
    {
        vram_set(i, 0x10); // 0x10 = grid tile
        vram_set(i + 0x13, 0x10);
    }
}
