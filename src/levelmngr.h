#ifndef LEVELMNGR_H
#define LEVELMNGR_H

#include <stdint.h>

// Address of the start of the level pack
extern const uint8_t* curLevelPackAddr;

// Load a level from the currently selected level pack into RAM
void loadLevel(uint8_t level);

#endif//LEVELMNGR_H