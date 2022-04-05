#ifndef HELPERS_H
#define HELPERS_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define BGB_BREAKPOINT() __asm__("ld b, b") // opcode used as breakpoint in BGB
#define CRASH_POINT() __asm__(" .db #0xDD") // invalid opcode, will lock up the system

// Useful for 8.8 fixed point values. Taken from GBDK/types.h
typedef union _fixed {
  struct {
    uint8_t l;
    uint8_t h;
  } b;
  uint16_t w;
} fixed;

void smoothSlide(uint16_t* srcCoord, uint8_t targetPos);

#endif//HELPERS