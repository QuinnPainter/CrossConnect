#ifndef CURSOR_H
#define CURSOR_H

#include "helpers.h"

extern uint8_t cursorTargetX;
extern uint8_t cursorTargetY;
extern uint16_t cursorCurX;
extern uint16_t cursorCurY;

extern uint8_t cursorBoardPrevX;
extern uint8_t cursorBoardPrevY;
extern uint8_t cursorBoardX;
extern uint8_t cursorBoardY;
extern uint8_t cursorMoveDirection;

void updateCursorMovement();
void updateCursorAnimation();
void processDpadPress(uint8_t dpadState);

#endif//CURSOR_H
