#ifndef HELPERS_H
#define HELPERS_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define BGB_BREAKPOINT() __asm__("ld b, b") // opcode used as breakpoint in BGB
#define CRASH_POINT() __asm__(" .db #0xDD") // invalid opcode, will lock up the system

// returns the 2's complement negative version of a number.
// since SDCC is stupid, adding a NEGATEd number is (sometimes) more efficient than subtracting.
#define NEGATE(x) ((~x)+1)

// Useful for 8.8 fixed point values. Taken from GBDK/types.h
typedef union _fixed {
  struct {
    uint8_t l;
    uint8_t h;
  } b;
  uint16_t w;
} fixed;

inline void mbc5_sram_enable() { *((uint8_t*)0x0000) = 0x0A; }
inline void mbc5_sram_disable() { *((uint8_t*)0x0000) = 0x00; }

void smoothSlide(uint16_t* srcCoord, uint8_t targetPos, uint8_t speed);
void clearScreenWithBorder();
void drawTilemap(uint8_t* dstPtr, uint8_t* srcPtr, uint8_t* srcEnd, uint8_t tileOffset);

#endif//HELPERS