#ifndef BCD_H
#define BCD_H

#include "helpers.h"

// obviously functions the same as a uint8, just signals that it should be treated as BCD
typedef uint8_t bcd8;

// Increments a BCD8 number
bcd8 bcd_inc(bcd8 input) __preserves_regs(b, c, d, e, h, l);

// Converts a number from 0-99 to BCD
bcd8 bcd_conv(uint8_t input) __preserves_regs(b, c, d);

#endif//BCD_H
