#include "sdk/hardware.h"
#include "sdk/video.h"
#include "sdk/oam.h"
#include "sdk/assets.h"
#include "sdk/joypad.h"
#include "sdk/interrupt.h"
#include <string.h>

ASSET(tiles, "tiles.2bpp");

uint8_t cursor_x = 0;
uint8_t cursor_y = 0;


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
    shadow_oam[0].y = 0x20;
    shadow_oam[0].x = 0x20;
    shadow_oam[0].tile = 0x04;
    shadow_oam[0].attr = 0x00;
    shadow_oam[1].y = 0x24;
    shadow_oam[1].x = 0x24;
    shadow_oam[1].tile = 0x02;
    shadow_oam[1].attr = 0x00;

    //Make sure sprites and the background are drawn
    rLCDC = LCDC_ON | LCDC_OBJON | LCDC_BGON;

    //Setup the VBLANK interrupt, but we don't actually enable interrupt handling.
    // We only do this, so HALT waits for VBLANK.
    rIF = 0;
    rIE = IE_VBLANK;

    vram_memset(0x9800, 0x05, 32 * 18);
    for(uint8_t x=0; x<20; x++) {
        vram_set(0x9800 + x, 0x07);
        vram_set(0x9A20 + x, 0x07);
    }
    for(uint8_t y=1; y<18; y++) {
        vram_set(0x9800 + y * 0x20, 0x07);
        vram_set(0x9800 + 19 + y * 0x20, 0x07);
    }
    vram_set(0x9800 + 3 + 3 * 0x20, 0x01);

    while(1) {
        joypad_update();
        if (joypad_state & PAD_LEFT) cursor_x --;
        if (joypad_state & PAD_RIGHT) cursor_x ++;
        if (joypad_state & PAD_UP) cursor_y --;
        if (joypad_state & PAD_DOWN) cursor_y ++;
        if (joypad_state & (PAD_A | PAD_B)) {
            vram_set(0x9800 + cursor_x / 8 + cursor_y / 8 * 0x20, 0x03);
        }

        // Screen to OAM coordinates
        shadow_oam[0].y = cursor_y + 16;
        shadow_oam[0].x = cursor_x + 8;

        //Wait for VBLANK
        HALT();
        rIF = 0;    //As global interrupts are not enabled, we need to clear the interrupt flag.

        //Copy the sprites into OAM memory
        oam_dma_copy();
    }
}
