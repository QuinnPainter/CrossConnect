#ifndef TEXT_H
#define TEXT_H

#include <stdint.h>

// Copy a string into RAM (or unlocked VRAM)
void copyString(uint8_t* src, uint8_t* dst);

// Copy a string into locked VRAM
void copyStringVRAM(uint8_t* src, uint8_t* dst);

extern const uint8_t PlayString[];
extern const uint8_t StyleString[];
extern const uint8_t AboutString[];

extern const uint8_t ShapesString[];
extern const uint8_t NumbersString[];

#endif//TEXT_H