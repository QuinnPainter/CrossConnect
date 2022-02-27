#include "sdk/hardware.h"
#include "sdk/video.h"
#include "sdk/oam.h"
#include "sdk/assets.h"
#include "sdk/joypad.h"
#include "sdk/interrupt.h"
#include <string.h>
#include "helpers.h"

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
    // 1000 = Top
    // 0100 = Bottom
    // 0010 = Left
    // 0001 = Right
};

ASSET(backgroundTiles, "background.2bpp");
ASSET(cursorTiles, "cursor.2bpp");
ASSET(nodeNumberTiles, "nodeNumbers.2bpp");
ASSET(nodeShapeTiles, "nodeShapes.2bpp");
ASSET(connectionTiles, "connections.2bpp");

#define TILE_CURSOR1 0x00
#define TILE_CURSOR2 0x01
#define TILE_BGEMPTY 0x80
#define TILE_BGFILLED 0x81
#define TILE_CONNECT1 0x90
#define TILE_NUMNODE1 0xA0
#define TILE_SHAPENODE1 0xB0

#define STYLE_NUMS 0
#define STYLE_SHAPES 1

uint8_t nodeStyle = 1; // 0 = numbers, 1 = shapes
uint8_t cursorBoardPrevX = 0; // Old cursor board position
uint8_t cursorBoardPrevY = 0;
uint8_t cursorBoardX = 1; // Current cursor board position
uint8_t cursorBoardY = 1;
uint8_t cursorMoveDirection = false; // Which direction the cursor moved. If 0, did not move.

uint8_t board[18][20];

#define F BOARD_TILE_FILLED
#define E BOARD_TILE_EMPTY
#define N1 (0b00000000 + BOARD_TILE_NODE)
#define N2 (0b00010000 + BOARD_TILE_NODE)
const uint8_t initialBoard[18][20] = {
    {F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F},
    {F, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, F},
    {F, N1, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, F},
    {F, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, F},
    {F, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, F},
    {F, E, E, E, E, E, E, E, F, F, E, E, E, E, E, E, E, E, E, F},
    {F, E, E, E, E, E, E, E, F, F, E, E, E, E, E, N2, E, E, E, F},
    {F, E, E, E, E, E, E, E, E, F, E, E, E, E, E, E, E, E, E, F},
    {F, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, F},
    {F, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, F},
    {F, E, E, E, E, E, E, E, E, E, E, N2, E, E, E, E, E, E, E, F},
    {F, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, F},
    {F, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, F},
    {F, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, F},
    {F, E, E, E, E, E, E, E, N1, E, E, E, E, E, E, E, E, E, E, F},
    {F, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, F},
    {F, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, E, F},
    {F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F}
};
#undef E
#undef F
#undef N1
#undef N2

bool isTileConnection(uint8_t tile)
{
    switch(tile)
    {
        case 0:
        case BOARD_TILE_NODE:
        case BOARD_TILE_NODE_RIGHT:
        case BOARD_TILE_NODE_LEFT:
        case BOARD_TILE_NODE_BOTTOM:
        case BOARD_TILE_NODE_TOP:
            return false;
        default: // must be connection
            return true;
    }
}

// Inverts a direction, while maintaining the colour value.
uint8_t invertDirection(uint8_t dir)
{
    uint8_t inverseDir = dir;
    if (inverseDir & 0b1100) // invert up & down
    {
        inverseDir ^= 0b1100;
    }
    else if (inverseDir & 0b0011) // invert left & right
    {
        inverseDir ^= 0b0011;
    }
    return inverseDir;
}

void drawInitialBoard()
{
    for (uint8_t y = 0; y < 18; y++)
    {
        for (uint8_t x = 0; x < 20; x++)
        {
            uint8_t tileBottomBits = board[y][x] & 0x0F;
            uint8_t tileTopBits = board[y][x] & 0xF0;
            if (tileBottomBits == 0)
            {
                if (tileTopBits == (BOARD_TILE_EMPTY & 0xF0))
                {
                    vram_set(0x9800 + (y * 0x20) + x, TILE_BGEMPTY);
                }
                else //if (tileTopBits == BOARD_TILE_FILLED & 0xF0)
                {
                    vram_set(0x9800 + (y * 0x20) + x, TILE_BGFILLED);
                }
            }
            else if (tileBottomBits == BOARD_TILE_NODE)
            {
                if (nodeStyle == STYLE_NUMS)
                {
                    vram_set(0x9800 + (y * 0x20) + x, TILE_NUMNODE1 + (board[y][x] >> 4));
                }
                else if (nodeStyle == STYLE_SHAPES)
                {
                    vram_set(0x9800 + (y * 0x20) + x, TILE_SHAPENODE1 + (board[y][x] >> 4));
                }
            }
            else // Since we're just drawing the initial board state, it should be impossible for there to be connections.
            {
                BGB_BREAKPOINT();
            }
        }
    }
}

void drawOneTile(uint8_t x, uint8_t y)
{
    uint8_t tile = board[y][x];
    switch (tile & 0x0F)
    {
        case 0: // Can't be wall, so must be empty
            vram_set(0x9800 + (y * 0x20) + x, TILE_BGEMPTY);
            break;
        case BOARD_TILE_NODE:
            if (nodeStyle == STYLE_NUMS)
            {
                vram_set(0x9800 + (y * 0x20) + x, TILE_NUMNODE1 + (tile >> 4));
            }
            else if (nodeStyle == STYLE_SHAPES)
            {
                vram_set(0x9800 + (y * 0x20) + x, TILE_SHAPENODE1 + (tile >> 4));
            }
            break;
        case BOARD_TILE_NODE_RIGHT:
        case BOARD_TILE_NODE_LEFT:
        case BOARD_TILE_NODE_BOTTOM:
        case BOARD_TILE_NODE_TOP:
            break; // todo
        default: // must be connection
            vram_set(0x9800 + (y * 0x20) + x, TILE_CONNECT1 + (tile & 0xF));
            break;
    }
}

// true = ends at node, false = ends with connection
bool followPath(uint8_t startX, uint8_t startY, uint8_t startDir, bool deletePath)
{
    uint8_t curX = startX;
    uint8_t curY = startY;
    uint8_t curDir = startDir;
    while(1)
    {
        //BGB_BREAKPOINT();
        uint8_t curTile = board[curY][curX];
        switch (curTile & 0xF) // ignore colour
        {
            case BOARD_TILE_NODE_RIGHT:
            case BOARD_TILE_NODE_LEFT:
            case BOARD_TILE_NODE_BOTTOM:
            case BOARD_TILE_NODE_TOP:
            case BOARD_TILE_NODE: // temp
                return true;
            case 0b1000: // end of connection
            case 0b0100:
            case 0b0010:
            case 0b0001:
                return false;
            default: // must be a mid connection, time to follow it
                if (deletePath)
                {
                    board[curY][curX] = BOARD_TILE_EMPTY;
                    drawOneTile(curX, curY);
                }
                // remove the way we came from, to get a single direction
                //curDir++; curDir--; BGB_BREAKPOINT();
                curDir = ((curTile & 0xF) & (~invertDirection(curDir)));
                switch (curDir)
                {
                    case 0b1000: curY--; break;
                    case 0b0100: curY++; break;
                    case 0b0010: curX--; break;
                    case 0b0001: curX++; break;
                    default: BGB_BREAKPOINT();
                }
        }
    }
}

void main() {
    lcd_off(); // Disable screen so we can copy to VRAM freely

    memcpy((void*)0x8000, cursorTiles, cursorTiles_end - cursorTiles);
    memcpy((void*)0x8800, backgroundTiles, backgroundTiles_end - backgroundTiles);
    memcpy((void*)0x8900, connectionTiles, connectionTiles_end - connectionTiles);
    memcpy((void*)0x8A00, nodeNumberTiles, nodeNumberTiles_end - nodeNumberTiles);
    memcpy((void*)0x8B00, nodeShapeTiles, nodeShapeTiles_end - nodeShapeTiles); // unconnected
    memcpy((void*)0x8C00, nodeShapeTiles, nodeShapeTiles_end - nodeShapeTiles); // top
    memcpy((void*)0x8D00, nodeShapeTiles, nodeShapeTiles_end - nodeShapeTiles); // bottom
    memcpy((void*)0x8E00, nodeShapeTiles, nodeShapeTiles_end - nodeShapeTiles); // left
    memcpy((void*)0x8F00, nodeShapeTiles, nodeShapeTiles_end - nodeShapeTiles); // right
    // Generate the "connected" tiles for nodeShapes
    for (uint8_t* i = (uint8_t*)0x8C00; i < (uint8_t*)0x8D00; i += 16) // top
    {
        (*i) |= 0b00011000;
        (*(i + 1)) |= 0b00011000;
    }
    for (uint8_t* i = (uint8_t*)0x8D0E; i < (uint8_t*)0x8E0E; i += 16) // bottom
    {
        (*i) |= 0b00011000;
        (*(i + 1)) |= 0b00011000;
    }
    for (uint8_t* i = (uint8_t*)0x8E06; i < (uint8_t*)0x8F06; i += 16) // left
    {
        (*i) |= 0b10000000;
        (*(i + 1)) |= 0b10000000;
        (*(i + 2)) |= 0b10000000;
        (*(i + 3)) |= 0b10000000;
    }
    for (uint8_t* i = (uint8_t*)0x8F06; i < (uint8_t*)0x9006; i += 16) // right
    {
        (*i) |= 0b00000001;
        (*(i + 1)) |= 0b00000001;
        (*(i + 2)) |= 0b00000001;
        (*(i + 3)) |= 0b00000001;
    }
    //Setup the OAM for sprite drawing
    oam_init();

    //Set some default DMG palette data
    rBGP = 0b11100100;
    rOBP0 = 0b11100100;
    rOBP1 = 0b11100100;

    //Put some sprites on the screen
    shadow_oam[0].y = 0x00;
    shadow_oam[0].x = 0x00;
    shadow_oam[0].tile = TILE_CURSOR1;
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

    drawInitialBoard();

    while(1) {
        joypad_update();

        // Update cursor position
        cursorBoardPrevX = cursorBoardX;
        cursorBoardPrevY = cursorBoardY;
        cursorMoveDirection = 0;
        if (joypad_pressed & PAD_LEFT) { cursorBoardX --; cursorMoveDirection = 0b0001; }
        if (joypad_pressed & PAD_RIGHT) { cursorBoardX ++; cursorMoveDirection = 0b0010; }
        // Don't allow moving diagonal in a single frame, so connection painting doesn't get messed up
        if (joypad_pressed & PAD_UP && cursorMoveDirection == 0) { cursorBoardY --; cursorMoveDirection = 0b0100; }
        if (joypad_pressed & PAD_DOWN && cursorMoveDirection == 0) { cursorBoardY ++; cursorMoveDirection = 0b1000; }
        if (board[cursorBoardY][cursorBoardX] == BOARD_TILE_FILLED)
        {
            // Can't move into filled space, so move back to previous position
            cursorBoardX = cursorBoardPrevX;
            cursorBoardY = cursorBoardPrevY;
            cursorMoveDirection = 0;
        }
        else if ((joypad_state & PAD_A) && cursorMoveDirection) // If A is held, may want to paint a connection
        {
            uint8_t prevBoardTile = board[cursorBoardPrevY][cursorBoardPrevX];
            if (prevBoardTile != BOARD_TILE_EMPTY) // must be node or connection if not empty
            {
                uint8_t curBoardTile = board[cursorBoardY][cursorBoardX];
                // checks are done - now it's time to paint the new connection
                if (curBoardTile == BOARD_TILE_EMPTY)
                {
                    // paint new tile we just moved into
                    board[cursorBoardY][cursorBoardX] |= cursorMoveDirection;
                    drawOneTile(cursorBoardX, cursorBoardY);
                    // paint previous tile
                    board[cursorBoardPrevY][cursorBoardPrevX] |= invertDirection(cursorMoveDirection);
                    drawOneTile(cursorBoardPrevX, cursorBoardPrevY);
                }
                else
                {
                    for (uint8_t i = 0b1000; i != 0; i >>= 1)
                    {
                        if ((curBoardTile & i) != 0)
                        {
                            if (followPath(cursorBoardX, cursorBoardY, invertDirection(curBoardTile & i), false) == false)
                            {
                                followPath(cursorBoardX, cursorBoardY, invertDirection(curBoardTile & i), true);
                                break;
                            }
                        }
                    }
                    // delete previous tile
                    board[cursorBoardPrevY][cursorBoardPrevX] = BOARD_TILE_EMPTY;
                    drawOneTile(cursorBoardPrevX, cursorBoardPrevY);
                    // recalculate the direction the new tile connection should be
                    board[cursorBoardY][cursorBoardX] &= 0xF0;
                    if (isTileConnection(board[cursorBoardY+1][cursorBoardX]) && (board[cursorBoardY+1][cursorBoardX] & 0b1000)) // check lower tile
                    {
                        board[cursorBoardY][cursorBoardX] |= 0b0100;
                    }
                    else if (isTileConnection(board[cursorBoardY-1][cursorBoardX]) && (board[cursorBoardY-1][cursorBoardX] & 0b0100)) // check upper tile
                    {
                        board[cursorBoardY][cursorBoardX] |= 0b1000;
                    }
                    else if (isTileConnection(board[cursorBoardY][cursorBoardX-1]) && (board[cursorBoardY][cursorBoardX-1] & 0b0001)) // check left tile
                    {
                        board[cursorBoardY][cursorBoardX] |= 0b0010;
                    }
                    else if (isTileConnection(board[cursorBoardY][cursorBoardX+1]) && (board[cursorBoardY][cursorBoardX+1] & 0b0010)) // check right tile
                    {
                        board[cursorBoardY][cursorBoardX] |= 0b0001;
                    }
                    drawOneTile(cursorBoardX, cursorBoardY);
                }
            }
        }

        // Screen to OAM coordinates
        shadow_oam[0].y = (cursorBoardY * 8) + 16;
        shadow_oam[0].x = (cursorBoardX * 8) + 8;

        //Wait for VBLANK
        HALT();
        rIF = 0;    //As global interrupts are not enabled, we need to clear the interrupt flag.

        //Copy the sprites into OAM memory
        oam_dma_copy();
    }
}
