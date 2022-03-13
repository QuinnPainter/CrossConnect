#include "sdk/hardware.h"
#include "sdk/video.h"
#include "sdk/oam.h"
#include "sdk/assets.h"
#include "sdk/joypad.h"
#include "sdk/interrupt.h"
#include "helpers.h"
#include "levelmngr.h"

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
    DIR_UP = 0b1000,
    DIR_DOWN = 0b0100,
    DIR_LEFT = 0b0010,
    DIR_RIGHT = 0b0001,
};

ASSET(backgroundTiles, "background.2bpp");
ASSET(cursorTiles, "cursor.2bpp");
ASSET(nodeNumberTiles, "nodeNumbers.2bpp");
ASSET(nodeShapeTiles, "nodeShapes.2bpp");
ASSET(connectionTiles, "connections.2bpp");
LEVELPACK(testLevels, "testlevels.bin");

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
uint8_t cursorMoveDirection = 0; // Which direction the cursor moved. If 0, did not move.

uint8_t board[18][18];

/*#define F BOARD_TILE_FILLED
#define E BOARD_TILE_EMPTY
#define N1 (0b00000000 + BOARD_TILE_NODE)
#define N2 (0b00010000 + BOARD_TILE_NODE)
const uint8_t initialBoard[18][18] = {
    {F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F},
    {F, N1, E, E, E, E, E, E, N1, N2, E, E, E, E, E, E, N2, F},
    {F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F},
    {F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F},
    {F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F},
    {F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F},
    {F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F},
    {F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F},
    {F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F},
    {F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F},
    {F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F},
    {F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F},
    {F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F},
    {F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F},
    {F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F},
    {F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F},
    {F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F},
    {F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F}
};
#undef E
#undef F
#undef N1
#undef N2*/

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
        for (uint8_t x = 0; x < 18; x++)
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
    uint8_t nodeOffset;
    switch (tile & 0x0F)
    {
        case 0:
            if (tile == BOARD_TILE_FILLED)
            {
                vram_set(0x9800 + (y * 0x20) + x, TILE_BGFILLED);
            }
            else
            {
                vram_set(0x9800 + (y * 0x20) + x, TILE_BGEMPTY);
            }
            break;
        case BOARD_TILE_NODE_RIGHT:
            nodeOffset = 0x40;
            goto NODE_WITH_OFFSET;
        case BOARD_TILE_NODE_LEFT:
            nodeOffset = 0x30;
            goto NODE_WITH_OFFSET;
        case BOARD_TILE_NODE_BOTTOM:
            nodeOffset = 0x20;
            goto NODE_WITH_OFFSET;
        case BOARD_TILE_NODE_TOP:
            nodeOffset = 0x10;
            goto NODE_WITH_OFFSET;
        case BOARD_TILE_NODE:
            if (nodeStyle == STYLE_NUMS)
            {
                vram_set(0x9800 + (y * 0x20) + x, TILE_NUMNODE1 + (tile >> 4));
                break;
            }
            nodeOffset = 0;
        NODE_WITH_OFFSET:
            vram_set(0x9800 + (y * 0x20) + x, TILE_SHAPENODE1 + (tile >> 4) + nodeOffset);
            break;
        default: // must be connection
            vram_set(0x9800 + (y * 0x20) + x, TILE_CONNECT1 + (tile & 0xF));
            break;
    }
}

// true = ends at node, false = ends with connection
bool followPath(uint8_t startX, uint8_t startY, bool deletePath)
{
    uint8_t curX = startX;
    uint8_t curY = startY;
    uint8_t curDir = 0;
    while(1)
    {
        uint8_t curTile = board[curY][curX];
        switch (curTile & 0xF) // ignore colour
        {
            case BOARD_TILE_NODE_RIGHT:
            case BOARD_TILE_NODE_LEFT:
            case BOARD_TILE_NODE_BOTTOM:
            case BOARD_TILE_NODE_TOP:
                return true;
            case BOARD_TILE_NODE: // should be impossible
                BGB_BREAKPOINT();
            case 0b1000: // end of connection
            case 0b0100:
            case 0b0010:
            case 0b0001:
                if (curX != startX || curY != startY) // make sure we're not actually at the start of the connection
                {
                    if (deletePath)
                    {
                        board[curY][curX] = BOARD_TILE_EMPTY;
                        drawOneTile(curX, curY);
                    }
                    return false;
                }
            default: // must be a mid connection, time to follow it
                if (deletePath)
                {
                    board[curY][curX] = BOARD_TILE_EMPTY;
                    drawOneTile(curX, curY);
                }
                // remove the way we came from, to get a single direction
                curDir = ((curTile & 0xF) & (~invertDirection(curDir)));
                switch (curDir)
                {
                    case DIR_UP: curY--; break;
                    case DIR_DOWN: curY++; break;
                    case DIR_LEFT: curX--; break;
                    case DIR_RIGHT: curX++; break;
                    default: BGB_BREAKPOINT();
                }
        }
    }
}

inline bool isMoveValid(uint8_t prevTile, uint8_t curTile)
{
    // prevTile has already been confirmed to be not empty, and it can't be a wall,
    // so it must be a node or connection

    switch (prevTile & 0x0F)
    {
        case BOARD_TILE_NODE_RIGHT: // If prev tile is connected node
        case BOARD_TILE_NODE_LEFT:  // or a connection already connected in 2 directions
        case BOARD_TILE_NODE_BOTTOM:// move is not valid.
        case BOARD_TILE_NODE_TOP:
        case 0b1100:
        case 0b0011:
        case 0b1001:
        case 0b1010:
        case 0b0101:
        case 0b0110:
            return false;
    }

    switch (curTile & 0x0F)
    {
        case 0: // Moves into empty tiles are always valid
            return true;
        case 0b0001:
        case 0b0010:
        case 0b0100: // Moves into single connections or unconnected nodes
        case 0b1000: // are only valid if the colours match
        case BOARD_TILE_NODE:
            if ((curTile & 0xF0) == (prevTile & 0xF0)) { return true; }
    }
    // new tile must be a connected node or a 2-connected connection
    return false;
}

inline bool checkGameWon()
{
    for (uint8_t* boardPtr = (uint8_t*)board; boardPtr < (uint8_t*)board + sizeof(board); boardPtr++)
    {
        if (((*boardPtr) & 0x0F) == BOARD_TILE_NODE || (*boardPtr) == BOARD_TILE_EMPTY)
        {
            return false;
        }
    }
    return true;
}

void paintConnection(uint8_t x, uint8_t y, uint8_t direction)
{
    if ((board[y][x] & 0x0F) == BOARD_TILE_NODE)
    {
        board[y][x] &= (direction ^ 0xFF);
    }
    else
    {
        board[y][x] |= direction;
    }
    drawOneTile(x, y);
}

// Erase a single direction connection on a given tile
void eraseConnection(uint8_t x, uint8_t y, uint8_t direction)
{
    switch (board[y][x] & 0x0F)
    {
        case BOARD_TILE_NODE_RIGHT:
        case BOARD_TILE_NODE_LEFT:
        case BOARD_TILE_NODE_BOTTOM:
        case BOARD_TILE_NODE_TOP:
            // If direction matches the node direction, it will become an unconnected node
            // otherwise, nothing will change
            board[y][x] |= direction;
        case BOARD_TILE_NODE: // do nothing
            break;
        default: // must be connection (or empty / wall)
            if (board[y][x] & direction) // check if it's connected in that direction
            {
                board[y][x] &= ~direction;
                if ((board[y][x] & 0x0F) != 0)
                {
                    if (followPath(x, y, false) == false)
                    {
                        followPath(x, y, true);
                    }
                }
                else
                {
                    // If the last connection was removed, we must also remove colour
                    // so the tile ends up as empty instead of a wall
                    board[y][x] = BOARD_TILE_EMPTY;
                }
            }
    }
    drawOneTile(x, y);
}

// Erase all connections for a given tile
inline void eraseTileConnections(uint8_t x, uint8_t y)
{
    switch (board[y][x] & 0x0F)
    {
        case BOARD_TILE_NODE_RIGHT:
        case BOARD_TILE_NODE_LEFT:
        case BOARD_TILE_NODE_BOTTOM:
        case BOARD_TILE_NODE_TOP:
            board[y][x] |= 0x0F;
        case BOARD_TILE_NODE:
            break;
        default: // must be connection (or empty)
            board[y][x] = BOARD_TILE_EMPTY;
    }
    drawOneTile(x, y);
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

    //memcpy(board, initialBoard, sizeof(board));

    memset(board, BOARD_TILE_FILLED, sizeof(board));
    curLevelPackAddr = testLevels;
    loadLevel(2);

    drawInitialBoard();

    // Put the cursor in the top-leftmost open space
    for (cursorBoardY = 1;; cursorBoardY++)
    {
        for (cursorBoardX = 1; cursorBoardX < sizeof(board[0]); cursorBoardX++)
        {
            if (board[cursorBoardY][cursorBoardX] != BOARD_TILE_FILLED) { goto DONE_PLACE_CURSOR; }
        }
    }
    DONE_PLACE_CURSOR:

    while(1)
    {
        joypad_update();

        // Update cursor position
        cursorBoardPrevX = cursorBoardX;
        cursorBoardPrevY = cursorBoardY;
        cursorMoveDirection = 0;
        if (joypad_pressed & PAD_LEFT) { cursorBoardX --; cursorMoveDirection = DIR_RIGHT; }
        if (joypad_pressed & PAD_RIGHT) { cursorBoardX ++; cursorMoveDirection = DIR_LEFT; }
        // Don't allow moving diagonal in a single frame, so connection painting doesn't get messed up
        if ((joypad_pressed & PAD_UP) && cursorMoveDirection == 0) { cursorBoardY --; cursorMoveDirection = DIR_DOWN; }
        if ((joypad_pressed & PAD_DOWN) && cursorMoveDirection == 0) { cursorBoardY ++; cursorMoveDirection = DIR_UP; }
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
                if (isMoveValid(prevBoardTile, curBoardTile))
                {
                    // paint new tile we just moved into
                    paintConnection(cursorBoardX, cursorBoardY, cursorMoveDirection);
                    // paint previous tile
                    paintConnection(cursorBoardPrevX, cursorBoardPrevY, invertDirection(cursorMoveDirection));
                    // set colour of new tile to be the same as colour of previous tile
                    board[cursorBoardY][cursorBoardX] =
                            (board[cursorBoardY][cursorBoardX] & 0x0F)
                            | (board[cursorBoardPrevY][cursorBoardPrevX] & 0xF0);
                }

                if (checkGameWon())
                {
                    //todo: game winning
                    BGB_BREAKPOINT();
                }
            }
        }
        else if (joypad_state & PAD_B) // B held = Erase connections
        {
            // Erase the connections coming from the 4 bordering tiles
            eraseConnection(cursorBoardX+1, cursorBoardY, DIR_LEFT);
            eraseConnection(cursorBoardX-1, cursorBoardY, DIR_RIGHT);
            eraseConnection(cursorBoardX, cursorBoardY+1, DIR_UP);
            eraseConnection(cursorBoardX, cursorBoardY-1, DIR_DOWN);

            // Erase the current tile
            eraseTileConnections(cursorBoardX, cursorBoardY);
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
