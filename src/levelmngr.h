#ifndef LEVELMNGR_H
#define LEVELMNGR_H

#include <stdint.h>

// Address of the start of the level pack
extern const uint8_t* curLevelPackAddr;

extern uint8_t curLevelWidth;
extern uint8_t curLevelHeight;

// Load a level from the currently selected level pack into RAM
// Assumes that the board has already been cleared (filled with wall tiles)
void loadLevel(uint8_t level);

#endif//LEVELMNGR_H