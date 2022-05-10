#ifndef INGAMEMENU_H
#define INGAMEMENU_H

#include "helpers.h"

enum ingameMenuResults {
    INGAMEMENU_RESULT_NEXT = 0,
    INGAMEMENU_RESULT_MENU,
    INGAMEMENU_RESULT_DONOTHING,
}; 

extern bool isWinMenu;

void drawPauseMenu();
void drawWinMenu();
uint8_t ingameMenuLoop();
void ingameMenuProcessMove(uint8_t dpadState);

#endif
