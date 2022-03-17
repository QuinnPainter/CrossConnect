#ifndef HELPERS_H
#define HELPERS_H

#include <stdbool.h>
#include <string.h>

#define BGB_BREAKPOINT() __asm__("ld b, b") // opcode used as breakpoint in BGB
#define CRASH_POINT() __asm__(" .db #0xDD") // invalid opcode, will lock up the system

// Equivalent to GBSDK's "ASSET" in asset.h, but for the level path
#define LEVELPACK(var_name, filename) \
    void __ ## var_name ## __() __naked { \
        __asm__("_" #var_name "::"); \
        __asm__(".incbin \"_build/levels/" filename "\""); \
        __asm__("_" #var_name "_end::"); \
    } EXTERN_ASSET(var_name)

// Useful for 8.8 fixed point values. Taken from GBDK/types.h
typedef union _fixed {
  struct {
    uint8_t l;
    uint8_t h;
  } b;
  uint16_t w;
} fixed;

#endif//HELPERS