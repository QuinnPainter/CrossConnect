// Experimenting with level generation.
// This isn't actually used in the game, just a test.

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>

#define BOARD_X 14
#define BOARD_Y 14

struct tile
{
	int colour;
	bool isNode;
	bool isWall;
	bool connectedUp;
	bool connectedDown;
	bool connectedLeft;
	bool connectedRight;
};

struct tile board[BOARD_Y][BOARD_X] = { 0 };
int curColour = 0;

// todo: change random to match random gen used on GB
void seedRand(int seed)
{
	srand(seed);
}

int genRand()
{
	return rand();
}

bool isSpotEmpty(int x, int y)
{
	struct tile spot = board[y][x];
	return !(spot.isWall || spot.isNode || spot.connectedUp || spot.connectedDown || spot.connectedLeft || spot.connectedRight);
}

bool isValidMove(int x, int y, int ignoreDir)
{
	if (!isSpotEmpty(x, y)) { return false; }

	if (ignoreDir & 0b1000)
	{
		if (board[y-1][x].colour == curColour && !board[y-1][x].connectedDown) { return false; }
	}
	if (ignoreDir & 0b0100)
	{
		if (board[y+1][x].colour == curColour && !board[y+1][x].connectedUp) { return false; }
	}
	if (ignoreDir & 0b0010)
	{
		if (board[y][x-1].colour == curColour && !board[y][x-1].connectedRight) { return false; }
	}
	if (ignoreDir & 0b0001)
	{
		if (board[y][x+1].colour == curColour && !board[y][x+1].connectedLeft) { return false; }
	}
	/*if (board[y-1][x].colour == curColour && !board[y-1][x].connectedDown) { return false; }
	if (board[y+1][x].colour == curColour && !board[y+1][x].connectedUp) { return false; }
	if (board[y][x-1].colour == curColour && !board[y][x-1].connectedRight) { return false; }
	if (board[y][x+1].colour == curColour && !board[y][x+1].connectedLeft) { return false; }*/
	/*if (board[y-1][x].colour == curColour) { return false; }
	if (board[y+1][x].colour == curColour) { return false; }
	if (board[y][x-1].colour == curColour) { return false; }
	if (board[y][x+1].colour == curColour) { return false; }*/

	return true;
}

bool drawOnePath()
{
	int curX = genRand() % BOARD_X;
	int curY = genRand() % BOARD_Y;
	int startX = curX;
	int startY = curY;
	int pathLength = 0;

	while (!isSpotEmpty(curX, curY))
	{
		curX++;
		if (curX == BOARD_X)
		{
			curX = 0;
			curY++;
			if (curY == BOARD_Y)
			{
				curY = 0;
			}
		}
		if (curX == startX && curY == startY)
		{
			return false; // board is full
		}
	}

	board[curY][curX].isNode = true;
	board[curY][curX].colour = curColour;

	while (true)
	{
		int dirOptions = 0;
		if (pathLength == 0)
		{
			if (isSpotEmpty(curX, curY-1)) { dirOptions |= 0b1000; }
			if (isSpotEmpty(curX, curY+1)) { dirOptions |= 0b0100; }
			if (isSpotEmpty(curX-1, curY)) { dirOptions |= 0b0010; }
			if (isSpotEmpty(curX+1, curY)) { dirOptions |= 0b0001; }
		}
		else
		{
			if (isValidMove(curX, curY-1, 0b1011)) { dirOptions |= 0b1000; }
			if (isValidMove(curX, curY+1, 0b0111)) { dirOptions |= 0b0100; }
			if (isValidMove(curX-1, curY, 0b1110)) { dirOptions |= 0b0010; }
			if (isValidMove(curX+1, curY, 0b1101)) { dirOptions |= 0b0001; }
		}

		if (dirOptions == 0)
		{
			if (pathLength == 0)
			{
				board[curY][curX].isNode = false;
				board[curY][curX].isWall = true;
			}
			else
			{
				board[curY][curX].isNode = true;
				board[curY][curX].colour = curColour;
				curColour++;
			}
			return true;
		}

		int chosenDirection = 1 << (genRand() % 4);
		while ((chosenDirection & dirOptions) == 0)
		{
			chosenDirection >>= 1;
			if ((chosenDirection & 0xF) == 0) { chosenDirection = 0b1000; } // wrap around
		}

		switch (chosenDirection)
		{
			case 0b1000: if(!board[curY][curX].isNode) { board[curY][curX].connectedUp = true; } curY--; board[curY][curX].connectedDown = true; break;
			case 0b0100: if(!board[curY][curX].isNode) { board[curY][curX].connectedDown = true; } curY++; board[curY][curX].connectedUp = true; break;
			case 0b0010: if(!board[curY][curX].isNode) { board[curY][curX].connectedLeft = true; } curX--; board[curY][curX].connectedRight = true; break;
			case 0b0001: if(!board[curY][curX].isNode) { board[curY][curX].connectedRight = true; } curX++; board[curY][curX].connectedLeft = true; break;
		}
		board[curY][curX].colour = curColour;
		pathLength++;
	}
}

void printBoard()
{
	for (int y = 0; y < BOARD_Y; y++)
	{
		for (int x = 0; x < BOARD_X; x++)
		{
			struct tile curTile = board[y][x];
			if (curTile.isWall)
			{
				printf("X");
			}
			else if (curTile.isNode)
			{
				printf("%c", (char)curTile.colour + 'A');
			}
			else
			{
				if (curTile.connectedUp && curTile.connectedDown)
				{
					printf("│");
				}
				else if (curTile.connectedLeft && curTile.connectedRight)
				{
					printf("─");
				}
				else if (curTile.connectedUp && curTile.connectedLeft)
				{
					printf("┘");
				}
				else if (curTile.connectedUp && curTile.connectedRight)
				{
					printf("└");
				}
				else if (curTile.connectedDown && curTile.connectedLeft)
				{
					printf("┐");
				}
				else if (curTile.connectedDown && curTile.connectedRight)
				{
					printf("┌");
				}
				else
				{
					printf(" ");
				}
			}
		}
		printf("\n");
	}
}

void genBoard()
{
	memset(board, 0, BOARD_X * BOARD_Y * sizeof(struct tile));
	// border walls to board
	for (int x = 0; x < BOARD_X; x++)
	{
		board[0][x].isWall = true;
		board[BOARD_Y - 1][x].isWall = true;
	}
	for (int y = 0; y < BOARD_Y; y++)
	{
		board[y][0].isWall = true;
		board[y][BOARD_X - 1].isWall = true;
	}
	curColour = 1; // can't start at zero, makes level gen wrong

	while(true)
	{
		if (!drawOnePath())
		{ break; }
	}
}

bool verifyBoard()
{
	for (int y = 1; y < BOARD_Y - 1; y++)
	{
		for (int x = 1; x < BOARD_X - 1; x++)
		{
			// check if there are any filled spaces on the board
			if (board[y][x].isWall) { return false; }
			// check if there are any node pairs directly touching
			if (board[y][x].isNode)
			{
				if (board[y][x+1].isNode && board[y][x+1].colour == board[y][x].colour) { return false; }
				if (board[y+1][x].isNode && board[y+1][x].colour == board[y][x].colour) { return false; }
			}
		}
	}
	// make sure there are at least 2 colours
	if (curColour < 2) { return false; }
	return true;
}

int main ()
{
	seedRand(time(NULL));
	
	//struct tile* board = malloc(BOARD_X * BOARD_Y * sizeof(struct tile));
	//memset(board, 0, BOARD_X * BOARD_Y * sizeof(struct tile));
	int numAttempts = 0;

	do
	{
		genBoard();
		numAttempts++;
		//if ((numAttempts % 1000) == 0) { printf("Attempts: %d\n", numAttempts); }
	} while (verifyBoard() == false);
	
	printBoard();
	printf("Attempts: %d\n", numAttempts);
	
	return 0;
}