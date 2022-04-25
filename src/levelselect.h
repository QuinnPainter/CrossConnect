#ifndef LEVELSELECT_H
#define LEVELSELECT_H

#include "helpers.h"

extern uint8_t lvlSelected;

void levelSelectLoop();
void lvlSelectProcessMove();
bool getLevelSolved(uint8_t levelPack, uint8_t levelNum);
void setLevelSolved(uint8_t levelPack, uint8_t levelNum);

#define LVL_PACK_SAVE_SIZE 12

#endif
