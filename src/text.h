#ifndef TEXT_H
#define TEXT_H

#include <stdint.h>

// Copy a string into locked VRAM
void copyStringVRAM(uint8_t* src, uint8_t* dst);

// Copy a string that is multiple lines
void copyFullscreenString(uint8_t* src, uint8_t* dst);

extern const uint8_t PlayString[];
extern const uint8_t StyleString[];
extern const uint8_t AboutString[];

extern const uint8_t ShapesString[];
extern const uint8_t NumbersString[];
extern const uint8_t HowToString[];

extern const uint8_t AboutPageString[];
extern const uint8_t HowToPageString[];

#endif//TEXT_H