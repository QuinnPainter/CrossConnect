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