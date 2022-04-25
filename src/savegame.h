#ifndef SAVEGAME_H
#define SAVEGAME_H

#include "helpers.h"

extern uint8_t nodeStyle;
extern uint8_t lvlSelectPage;
extern uint8_t lvlSelectPack;
extern uint8_t solvedLevels[];

void saveGame();
void loadSaveGame();

#endif
