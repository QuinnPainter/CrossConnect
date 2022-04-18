#ifndef GAME_H
#define GAME_H

enum tileTypes {
    BOARD_TILE_EMPTY = 0b00000000,
    BOARD_TILE_FILLED = 0b00010000,
    BOARD_TILE_NODE = 0b1111, // Top 4 bits = Colour
    BOARD_TILE_NODE_RIGHT = 0b1110,
    BOARD_TILE_NODE_LEFT = 0b1101,
    BOARD_TILE_NODE_BOTTOM = 0b1011,
    BOARD_TILE_NODE_TOP = 0b0111,
    // All other configurations of the bottom 4 bits are connections,
    // with each bit representing a direction and with the top 4 bits being colour.
    DIR_UP = 0b1000,
    DIR_DOWN = 0b0100,
    DIR_LEFT = 0b0010,
    DIR_RIGHT = 0b0001,
};

enum nodeStyles {
    STYLE_NUMS = 0,
    STYLE_SHAPES = 1
};

extern uint8_t nodeStyle;

void runGame(uint8_t levelNum);
void processMove();

#endif//GAME_H
