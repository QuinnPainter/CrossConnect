#include "sdk/interrupt.h"
#include "sdk/oam.h"
#include "sdk/video.h"
#include "sdk/system.h"
#include "sdk/joypad.h"
#include "sdk/hardware.h"
#include "game.h"
#include "cursor.h"
#include "gameassets.h"
#include "text.h"

#define BGCOLOUR PAL24(0xFFFFFF)
#define GRIDCOLOUR PAL24(0x999999)
const uint16_t bgpal[8*4] = {
    BGCOLOUR, GRIDCOLOUR, PAL24(0x6000D6), PAL24(0xFF0000), // purple & red
    BGCOLOUR, GRIDCOLOUR, PAL24(0xB30033), PAL24(0x00FF00), // reddish-pink & green
    BGCOLOUR, GRIDCOLOUR, PAL24(0x002861), PAL24(0x0000FF), // navy & blue
    BGCOLOUR, GRIDCOLOUR, PAL24(0x00692C), PAL24(0xFF7700), // dark green & orange
    BGCOLOUR, GRIDCOLOUR, PAL24(0x5EAD82), PAL24(0x5E5E5E), // dark blue-green & grey
    BGCOLOUR, GRIDCOLOUR, PAL24(0x704C25), PAL24(0xFF00FF), // brown & pink
    BGCOLOUR, GRIDCOLOUR, PAL24(0xA6BF00), PAL24(0x00FFFF), // yellow & cyan
    BGCOLOUR, GRIDCOLOUR, PAL24(0x610069), PAL24(0x000000), // dark purple & black
};

const uint16_t objpal[8*4] = {
    PAL24(0x000000), PAL24(0x000000), PAL24(0x000000), PAL24(0xFFFFFF),
    PAL24(0x000000), PAL24(0x000000), PAL24(0x000000), PAL24(0x999999),
    PAL24(0x000000), PAL24(0x000000), PAL24(0x000000), PAL24(0x444444),
    PAL24(0x000000), PAL24(0x000000), PAL24(0x000000), PAL24(0x000000),
    PAL24(0x000000), PAL24(0x000000), PAL24(0x000000), PAL24(0x000000),
    PAL24(0x000000), PAL24(0x000000), PAL24(0x000000), PAL24(0x000000),
    PAL24(0x000000), PAL24(0x000000), PAL24(0x000000), PAL24(0x000000),
    PAL24(0x000000), PAL24(0x000000), PAL24(0x000000), PAL24(0x000000),
};

#define MENUCURSOR_X_POS (OAM_X_OFS + (2*8))
#define MENUCURSOR_BASE_Y (OAM_Y_OFS + (12*8)) // Position of cursor when cursorSelection is 0
uint8_t cursorSelection = 0; // 0 = Play, 1 = Style, 2 = About

void genConnectedNodeTiles(uint8_t* startPtr, uint8_t* endPtr, bool useAltColour)
{
    if (useAltColour)
    {
        // change 01 to 10
        for (uint8_t* i = startPtr; i < endPtr; i += 16) // top
        {
            *(i) &= ~0b00011000;
            (*(i + 1)) |= 0b00011000;
        }
        for (uint8_t* i = startPtr + 0x010E; i < endPtr + 0x010E; i += 16) // bottom
        {
            (*i) &= ~0b00011000;
            (*(i + 1)) |= 0b00011000;
        }
        for (uint8_t* i = startPtr + 0x0206; i < endPtr + 0x0206; i += 16) // left
        {
            (*i) &= ~0b10000000;
            (*(i + 1)) |= 0b10000000;
            (*(i + 2)) &= ~0b10000000;
            (*(i + 3)) |= 0b10000000;
        }
        for (uint8_t* i = startPtr + 0x0306; i < endPtr + 0x0306; i += 16) // right
        {
            (*i) &= ~0b00000001;
            (*(i + 1)) |= 0b00000001;
            (*(i + 2)) &= ~0b00000001;
            (*(i + 3)) |= 0b00000001;
        }
    }
    else
    {
        // some of the writes aren't needed since there's already a grid box drawn around the node
        // it doesn't need to change 00 to 11, just 01 to 11
        for (uint8_t* i = startPtr; i < endPtr; i += 16) // top
        {
            //(*i) |= 0b00011000;
            (*(i + 1)) |= 0b00011000;
        }
        for (uint8_t* i = startPtr + 0x010E; i < endPtr + 0x010E; i += 16) // bottom
        {
            //(*i) |= 0b00011000;
            (*(i + 1)) |= 0b00011000;
        }
        for (uint8_t* i = startPtr + 0x0206; i < endPtr + 0x0206; i += 16) // left
        {
            //(*i) |= 0b10000000;
            (*(i + 1)) |= 0b10000000;
            //(*(i + 2)) |= 0b10000000;
            (*(i + 3)) |= 0b10000000;
        }
        for (uint8_t* i = startPtr + 0x0306; i < endPtr + 0x0306; i += 16) // right
        {
            //(*i) |= 0b00000001;
            (*(i + 1)) |= 0b00000001;
            //(*(i + 2)) |= 0b00000001;
            (*(i + 3)) |= 0b00000001;
        }
    }
}

// For CGB only. Sets the bg attributes for a 2x3 tile rect.
// Used to colour in the letters of the title on the main menu.
void setTitleLetterColour(uint16_t addr, uint8_t value)
{
    rVBK = 1; // make sure we're on the attribute vram bank
    vram_set(addr, value);
    vram_set(addr+1, value);
    vram_set(addr+0x20, value);
    vram_set(addr+0x21, value);
    vram_set(addr+0x40, value);
    vram_set(addr+0x41, value);
}

void main()
{
    lcd_off(); // Disable screen so we can copy to VRAM freely

    memcpy((void*)0x8000, spriteTiles, spriteTiles_end - spriteTiles);
    memcpy((void*)0x8800, backgroundTiles, backgroundTiles_end - backgroundTiles);
    memcpy((void*)0x8900, connectionTiles, connectionTiles_end - connectionTiles);
    memcpy((void*)0x8B00, nodeShapeTiles, nodeShapeTiles_end - nodeShapeTiles); // unconnected

    // Setup VRAM for CGB
    if (cpu_type == CPU_CGB)
    {
        cgb_background_palette(bgpal);
        cgb_object_palette(objpal);

        memcpy((void*)0x8A00, nodeNumberTilesCGB, nodeNumberTilesCGB_end - nodeNumberTilesCGB);

        // Copy in the tiles for the alternate colour connections
        memcpy((void*)0x9000, connectionTiles, connectionTiles_end - connectionTiles);

        // Convert the colours for the alternate colour connections
        for (uint8_t* i = (uint8_t*)0x9000; i < (uint8_t*)(0x9000 + (connectionTiles_end - connectionTiles)); i += 2)
        {
            for (uint8_t a = 1; a != 0; a <<= 1)
            {
                if (((*i) & a) && ((*(i + 1)) & a))
                {
                    (*i) &= ~a;
                }
            }
        }

        // Convert the colours for the alternate colour shape nodes
        for (uint8_t* i = (uint8_t*)0x8B80; i < (uint8_t*)(0x8B80 + 16 * 8); i += 2)
        {
            for (uint8_t a = 1; a != 0; a <<= 1)
            {
                if (((*i) & a) && ((*(i + 1)) & a))
                {
                    (*i) &= ~a;
                }
            }
        }

        memcpy((void*)0x8C00, (void*)0x8B00, nodeShapeTiles_end - nodeShapeTiles); // top
        memcpy((void*)0x8D00, (void*)0x8B00, nodeShapeTiles_end - nodeShapeTiles); // bottom
        memcpy((void*)0x8E00, (void*)0x8B00, nodeShapeTiles_end - nodeShapeTiles); // left
        memcpy((void*)0x8F00, (void*)0x8B00, nodeShapeTiles_end - nodeShapeTiles); // right

        // Generate the "connected" tiles for nodeShapes
        genConnectedNodeTiles((uint8_t*)0x8C00, (uint8_t*)0x8C80, false);
        genConnectedNodeTiles((uint8_t*)0x8C80, (uint8_t*)0x8D00, true);
    }
    else // DMG
    {
        rBGP = 0b11100100;
        rOBP0 = 0b11100100;
        rOBP1 = 0b11100100;

        memcpy((void*)0x8A00, nodeNumberTiles, nodeNumberTiles_end - nodeNumberTiles);

        memcpy((void*)0x8C00, (void*)0x8B00, nodeShapeTiles_end - nodeShapeTiles); // top
        memcpy((void*)0x8D00, (void*)0x8B00, nodeShapeTiles_end - nodeShapeTiles); // bottom
        memcpy((void*)0x8E00, (void*)0x8B00, nodeShapeTiles_end - nodeShapeTiles); // left
        memcpy((void*)0x8F00, (void*)0x8B00, nodeShapeTiles_end - nodeShapeTiles); // right

        // Generate the "connected" tiles for nodeShapes
        genConnectedNodeTiles((uint8_t*)0x8C00, (uint8_t*)0x8D00, false);
    }
    memcpy((void*)0x9200, fontTiles, fontTiles_end - fontTiles);

    // Setup main menu graphics
    memcpy((void*)0x9100, mainMenuTiles, mainMenuTiles_end - mainMenuTiles);
    uint8_t* dstPtr = (uint8_t*)0x9800;
    const uint8_t* srcPtr = mainMenuTilemap;
    rVBK = 0; // make sure we're on the tilemap vram bank
    for (uint8_t y = 0; y < 18; y++)
    {
        for (uint8_t x = 0; x < 20; x++)
        {
            if (cpu_type == CPU_CGB)
            {
                rVBK = 1; // make sure we're on the attribute vram bank
                *dstPtr = 0x7; // fill bg with BG palette 7
                rVBK = 0;
            }
            // 0x10 = tile offset from tilemap
            (*dstPtr++) = (*srcPtr++) + 0x10;
        }
        dstPtr += 12;
    }
    if (cpu_type == CPU_CGB)
    {
        setTitleLetterColour(0x9841, 0);
        setTitleLetterColour(0x9843, 1);
        setTitleLetterColour(0x9845, 2);
        setTitleLetterColour(0x9847, 3);
        setTitleLetterColour(0x9849, 5);
    }
    rSCY = 0;
    rSCX = 0;
    rVBK = 0; // switch to bg tilemap bank
    copyString(PlayString, (uint8_t*)0x9983);
    copyString(StyleString, (uint8_t*)0x99A3);
    copyString(AboutString, (uint8_t*)0x99C3);

    // Setup the OAM for sprite drawing
    oam_init();

    // Setup menu cursor sprite
    shadow_oam[1].y = MENUCURSOR_BASE_Y;
    shadow_oam[1].x = MENUCURSOR_X_POS;
    shadow_oam[1].tile = TILE_MENUCURSOR;
    shadow_oam[1].attr = 0x00;

    // Make sure sprites and the background are drawn (also turns the screen on)
    rLCDC = LCDC_ON | LCDC_OBJON | LCDC_BGON;

    // Setup the VBLANK interrupt.
    rIF = 0;
    rIE = IE_VBLANK;
    ENABLE_INTERRUPTS();

    // Init joypad state
    joypad_state = 0;

    while(1)
    {
        joypad_update();

        if (joypad_pressed & PAD_START)
        {
            runGame();
        }

        HALT();
    }
}

ISR_VBLANK()
{
    oam_dma_copy();
    updateCursorAnimation();
}
