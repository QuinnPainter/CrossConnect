#include "sdk/hardware.h"
#include "sdk/video.h"
#include "sdk/oam.h"
#include "sdk/assets.h"
#include "sdk/joypad.h"
#include "sdk/interrupt.h"
#include "sdk/system.h"
#include "helpers.h"
#include "gameassets.h"
#include "levelmngr.h"
#include "cursor.h"
#include "game.h"
#include "ingamemenu.h"
#include "levelselect.h"

#define BOARD_VRAM 0x99CD

uint8_t nodeStyle;

uint8_t cursorXOffset;
uint8_t cursorYOffset;

uint8_t board[18][18];

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
    // Draw board array
    for (uint8_t y = 0; y < 18; y++)
    {
        for (uint8_t x = 0; x < 18; x++)
        {
            rVBK = 0; // switch to tilemap vram bank (only for CGB)
            uint8_t tileBottomBits = board[y][x] & 0x0F;
            uint8_t tileTopBits = board[y][x] & 0xF0;
            uint16_t mapAddr = BOARD_VRAM + (y * 0x20) + x;
            if (tileBottomBits == 0)
            {
                if (tileTopBits == (BOARD_TILE_EMPTY & 0xF0))
                {
                    vram_set(mapAddr, TILE_BGEMPTY);
                }
                else //if (tileTopBits == BOARD_TILE_FILLED & 0xF0)
                {
                    vram_set(mapAddr, TILE_BGFILLED);
                }
            }
            else if (tileBottomBits == BOARD_TILE_NODE)
            {
                if (nodeStyle == STYLE_NUMS)
                {
                    vram_set(mapAddr, TILE_NUMNODE1 + (tileTopBits >> 4));
                }
                else if (nodeStyle == STYLE_SHAPES)
                {
                    vram_set(mapAddr, TILE_SHAPENODE1 + (tileTopBits >> 4));
                }
                if (cpu_type == CPU_CGB)
                {
                    rVBK = 1; // switch to CGB colour attribute bank
                    vram_set(mapAddr, (tileTopBits >> 4) & 0b111); // set colour palette for tile
                }
            }
            else // Since we're just drawing the initial board state, it should be impossible for there to be connections.
            {
                BGB_BREAKPOINT();
            }
        }
    }
    // Draw border
    {
        uint16_t boardBottomVRAM = (((uint16_t)curLevelHeight + 1) << 5);
        vram_set(BOARD_VRAM, TILE_BGTOPLEFT);
        vram_set(BOARD_VRAM + curLevelWidth + 1, TILE_BGTOPRIGHT);
        vram_set(BOARD_VRAM + boardBottomVRAM, TILE_BGBOTTOMLEFT);
        vram_set(BOARD_VRAM + boardBottomVRAM + curLevelWidth + 1, TILE_BGBOTTOMRIGHT);
        for (uint16_t i = BOARD_VRAM + 1; i < BOARD_VRAM + curLevelWidth + 1; i++)
        {
            vram_set(i, TILE_BGTOP);
            vram_set(i + boardBottomVRAM, TILE_BGBOTTOM);
        }
        for (uint16_t i = BOARD_VRAM + 0x20; i < BOARD_VRAM + boardBottomVRAM; i += 0x20)
        {
            vram_set(i, TILE_BGLEFT);
            vram_set(i + curLevelWidth + 1, TILE_BGRIGHT);
        }
    }
}

void drawOneTile(uint8_t x, uint8_t y)
{
    rVBK = 0; // switch to tilemap vram bank (only for CGB)
    uint8_t tile = board[y][x];
    uint8_t nodeOffset;
    uint16_t mapAddr = BOARD_VRAM + (y * 0x20) + x;
    switch (tile & 0x0F)
    {
        case 0:
            if (tile == BOARD_TILE_FILLED)
            {
                vram_set(mapAddr, TILE_BGFILLED);
            }
            else
            {
                vram_set(mapAddr, TILE_BGEMPTY);
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
            nodeOffset = 0;
        NODE_WITH_OFFSET:
            if (nodeStyle == STYLE_NUMS)
            {
                vram_set(mapAddr, TILE_NUMNODE1 + (tile >> 4));
                break;
            }
            vram_set(mapAddr, TILE_SHAPENODE1 + (tile >> 4) + nodeOffset);
            break;
        default: // must be connection
        {
            uint8_t baseTile = TILE_CONNECT1;
            if (cpu_type == CPU_CGB && tile & 0b10000000)
            {
                baseTile = TILE_CONNECTALT;
            }
            vram_set(mapAddr, baseTile + (tile & 0xF));
            if (cpu_type == CPU_CGB)
            {
                rVBK = 1; // switch to CGB colour attribute bank
                vram_set(mapAddr, (tile >> 4) & 0b111); // set colour palette for tile
            }
            break;
        }
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
        if ((*boardPtr) == BOARD_TILE_EMPTY) { return false; }
        // check for unconnected nodes and dangling connections
        switch ((*boardPtr) & 0x0F)
        {
            case BOARD_TILE_NODE:
            case 0b1000:
            case 0b0100:
            case 0b0010:
            case 0b0001:
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
            // don't redraw walls (so it doesn't screw up the borders)
            if (board[y][x] == BOARD_TILE_FILLED) { return; }
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

void runGame()
{
    cursorState = CURSOR_STATE_INGAME;

    memset(board, BOARD_TILE_FILLED, sizeof(board));
    loadLevel(lvlSelected);

    // set scroll based on level size
    rSCY = (8*14) - (((16 - curLevelHeight) >> 1) * 8);
    rSCX = (8*12) - (((16 - curLevelWidth) >> 1) * 8);
    cursorYOffset = ((144 - (curLevelHeight * 8)) / 2) + OAM_Y_OFS - 8;
    cursorXOffset = ((160 - (curLevelWidth * 8)) / 2) + OAM_X_OFS - 8;
    // adjust scroll by half tile for odd-sized levels
    if (curLevelHeight & 1) { rSCY -= 4; }
    if (curLevelWidth & 1) { rSCX -= 4; }


    // fill tilemap with BGFILLED
    rVBK = 0; // make sure we're on the tilemap vram bank
    for (uint16_t i = 0x9800; i < 0x9C00; i++)
    {
        vram_set(i, TILE_BGFILLED);
    }
    // fill background with the right colour
    if (cpu_type == CPU_CGB)
    {
        rVBK = 1; // make sure we're on the attribute vram bank
        for (uint16_t i = 0x9800; i < 0x9C00; i++)
        {
            vram_set(i, 0x7); // palette BG 7
        }
    }
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
    // Screen to OAM coordinates
    cursorTargetY = (cursorBoardY * 8) + cursorYOffset;
    cursorTargetX = (cursorBoardX * 8) + cursorXOffset;

    while(1)
    {
        joypad_update();

        updateCursorMovement();
        if (joypad_state & PAD_B) // B held = Erase connections
        {
            // Erase the connections coming from the 4 bordering tiles
            eraseConnection(cursorBoardX+1, cursorBoardY, DIR_LEFT);
            eraseConnection(cursorBoardX-1, cursorBoardY, DIR_RIGHT);
            eraseConnection(cursorBoardX, cursorBoardY+1, DIR_UP);
            eraseConnection(cursorBoardX, cursorBoardY-1, DIR_DOWN);
            // Erase the current tile
            eraseTileConnections(cursorBoardX, cursorBoardY);
        }

        //Wait for VBLANK
        HALT();
    }
}

void processMove()
{
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
            if (isMoveValid(prevBoardTile, board[cursorBoardY][cursorBoardX]))
            {
                // set colour of new tile to be the same as colour of previous tile
                board[cursorBoardY][cursorBoardX] =
                        (board[cursorBoardY][cursorBoardX] & 0x0F)
                        | (board[cursorBoardPrevY][cursorBoardPrevX] & 0xF0);
                // paint new tile we just moved into
                paintConnection(cursorBoardX, cursorBoardY, cursorMoveDirection);
                // paint previous tile
                paintConnection(cursorBoardPrevX, cursorBoardPrevY, invertDirection(cursorMoveDirection));

                if (checkGameWon())
                {
                    //todo: game winning
                    BGB_BREAKPOINT();
                }
            }
            else // if paint is not valid, don't move the cursor
            {   // todo : sound effect for this?
                cursorBoardX = cursorBoardPrevX;
                cursorBoardY = cursorBoardPrevY;
                cursorMoveDirection = 0;
            }
        }
    }

    // Screen to OAM coordinates
    cursorTargetY = (cursorBoardY * 8) + cursorYOffset;
    cursorTargetX = (cursorBoardX * 8) + cursorXOffset;
}
