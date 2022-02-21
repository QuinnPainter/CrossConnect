#include "sdk/hardware.h"
#include "sdk/video.h"
#include "sdk/oam.h"
#include "sdk/assets.h"
#include "sdk/joypad.h"
#include "sdk/interrupt.h"
#include <string.h>
#include "helpers.h"

enum tileTypes {
    TILE_EMPTY,
    TILE_FILLED
};

ASSET(tiles, "tiles.2bpp");

uint8_t cursor_board_prev_x = 0;
uint8_t cursor_board_prev_y = 0;
uint8_t cursor_board_x = 1;
uint8_t cursor_board_y = 1;

uint8_t board[18][20];

#define F TILE_FILLED
#define E TILE_EMPTY
const uint8_t initialBoard[18][20] = {
    {F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F},
    {F, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, F},
    {F, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, F},
    {F, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, F},
    {F, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, F},
    {F, E, E, E, E, E, E, E, F, F, E, E, E, E, E, E, E, E, E, F},
    {F, E, E, E, E, E, E, E, F, F, E, E, E, E, E, E, E, E, E, F},
    {F, E, E, E, E, E, E, E, E, F, E, E, E, E, E, E, E, E, E, F},
    {F, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, F},
    {F, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, F},
    {F, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, F},
    {F, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, F},
    {F, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, F},
    {F, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, F},
    {F, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, F},
    {F, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, F},
    {F, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, F},
    {F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F}
};
#undef E
#undef F

void main() {
    //Load our initial vram (this is slow, but always works, even outside of vblank)
    vram_memcpy(0x8000, tiles, tiles_end - tiles);
    vram_memcpy(0x9000, tiles, tiles_end - tiles);
    //Setup the OAM for sprite drawing
    oam_init();

    //Set some default DMG palette data
    rBGP = 0b11100100;
    rOBP0 = 0b11100100;
    rOBP1 = 0b11100100;

    //Put some sprites on the screen
    shadow_oam[0].y = 0x00;
    shadow_oam[0].x = 0x00;
    shadow_oam[0].tile = 0x04;
    shadow_oam[0].attr = 0x00;

    //Make sure sprites and the background are drawn
    rLCDC = LCDC_ON | LCDC_OBJON | LCDC_BGON;

    //Setup the VBLANK interrupt, but we don't actually enable interrupt handling.
    // We only do this, so HALT waits for VBLANK.
    rIF = 0;
    rIE = IE_VBLANK;

    // Init joypad state
    joypad_state = 0;

    //memset(board, 0, sizeof(board));
    memcpy(board, initialBoard, sizeof(board));

    for (uint8_t y = 0; y < 18; y++)
    {
        for (uint8_t x = 0; x < 20; x++)
        {
            switch(board[y][x])
            {
                case TILE_FILLED:
                    vram_set(0x9800 + (y * 0x20) + x, 0x07);
                    break;
                case TILE_EMPTY:
                    vram_set(0x9800 + (y * 0x20) + x, 0x05);
                    break;
                default: BGB_BREAKPOINT();
            }
        }
    }

    while(1) {
        joypad_update();

        // Update cursor position
        cursor_board_prev_x = cursor_board_x;
        cursor_board_prev_y = cursor_board_y;
        if (joypad_pressed & PAD_LEFT) cursor_board_x --;
        if (joypad_pressed & PAD_RIGHT) cursor_board_x ++;
        if (joypad_pressed & PAD_UP) cursor_board_y --;
        if (joypad_pressed & PAD_DOWN) cursor_board_y ++;
        if (board[cursor_board_y][cursor_board_x] == TILE_FILLED)
        {
            // Can't move into filled space, so move back to previous position
            cursor_board_x = cursor_board_prev_x;
            cursor_board_y = cursor_board_prev_y;
        }

        // Screen to OAM coordinates
        shadow_oam[0].y = (cursor_board_y * 8) + 16;
        shadow_oam[0].x = (cursor_board_x * 8) + 8;

        //Wait for VBLANK
        HALT();
        rIF = 0;    //As global interrupts are not enabled, we need to clear the interrupt flag.

        //Copy the sprites into OAM memory
        oam_dma_copy();
    }
}
