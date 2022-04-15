#ifndef BCD_H
#define BCD_H

#include "helpers.h"

// obviously functions the same as a uint8, just signals that it should be treated as BCD
typedef uint8_t bcd8;

// Increments a BCD8 number
bcd8 bcd_inc(bcd8 input) __preserves_regs(b, c, d, e, h, l);

#endif//BCD_H
