#ifndef INGAMEMENU_H
#define INGAMEMENU_H

#include "helpers.h"

extern bool isWinMenu;

void drawPauseMenu();
void drawWinMenu();
void ingameMenuLoop();
void ingameMenuProcessMove();

#endif
