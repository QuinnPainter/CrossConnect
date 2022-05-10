#ifndef FXENGINE_H
#define FXENGINE_H

#include "stdint.h"

void initFXEngine() __preserves_regs(b, c, d, e, h, l);
void playNewFX(uint8_t* fxPtr) __preserves_regs(c, d, e);
void updateFXEngine() __preserves_regs(b, d, e);

#endif
