#ifndef CURSOR_H
#define CURSOR_H

#include "helpers.h"

extern uint8_t cursorTargetX;
extern uint8_t cursorTargetY;
extern uint16_t cursorCurX;
extern uint16_t cursorCurY;

void updateCursorAnimation();

#endif//CURSOR_H
