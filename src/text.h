#ifndef TEXT_H
#define TEXT_H

#include <stdint.h>
#include "bcd.h"

// Copy a string into locked VRAM
void copyStringVRAM(const uint8_t* src, uint8_t* dst);

// Copy a string that is multiple lines
void copyFullscreenString(const uint8_t* src, uint8_t* dst);

// Draws a BCD8 number
void drawBCD8(bcd8 input, uint8_t* dst) __preserves_regs(b, c);

extern const uint8_t PlayString[];
extern const uint8_t StyleString[];
extern const uint8_t AboutString[];

extern const uint8_t ShapesString[];
extern const uint8_t NumbersString[];
extern const uint8_t HowToString[];

extern const uint8_t AboutPageString[];
extern const uint8_t HowToPageString[];

extern const uint8_t Testlevels2Name[];
extern const uint8_t Testlevels2Name[];

#endif//TEXT_H