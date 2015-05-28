//****************************************************************************************************
// Copyright (c) 2015 Gorka Suárez García, Dan Cristian Rotaru
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//****************************************************************************************************

#include "game.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

//****************************************************************************************************
// Data
//****************************************************************************************************

extern GameData game_data;

//****************************************************************************************************
// Move
//****************************************************************************************************

void ExecuteActionNorth(UINT8 player) {
    if (game_data.players[player].direction == DIR_NORTH) {
        UINT8 row1 = game_data.players[player].row;
        UINT8 col1 = game_data.players[player].col;
        UINT8 row2 = row1 - 1, col2 = col1;
        ExecuteActionMove(player, row1, col1, row2, col2);
    } else {
        game_data.players[player].direction = DIR_NORTH;
        RedrawWorldPlayer(player);
    }
}

//----------------------------------------------------------------------------------------------------

void ExecuteActionEast(UINT8 player) {
    if (game_data.players[player].direction == DIR_EAST) {
        UINT8 row1 = game_data.players[player].row;
        UINT8 col1 = game_data.players[player].col;
        UINT8 row2 = row1, col2 = col1 + 1;
        ExecuteActionMove(player, row1, col1, row2, col2);
    } else {
        game_data.players[player].direction = DIR_EAST;
        RedrawWorldPlayer(player);
    }
}

//----------------------------------------------------------------------------------------------------

void ExecuteActionSouth(UINT8 player) {
    if (game_data.players[player].direction == DIR_SOUTH) {
        UINT8 row1 = game_data.players[player].row;
        UINT8 col1 = game_data.players[player].col;
        UINT8 row2 = row1 + 1, col2 = col1;
        ExecuteActionMove(player, row1, col1, row2, col2);
    } else {
        game_data.players[player].direction = DIR_SOUTH;
        RedrawWorldPlayer(player);
    }
}

//----------------------------------------------------------------------------------------------------

void ExecuteActionWest(UINT8 player) {
    if (game_data.players[player].direction == DIR_WEST) {
        UINT8 row1 = game_data.players[player].row;
        UINT8 col1 = game_data.players[player].col;
        UINT8 row2 = row1, col2 = col1 - 1;
        ExecuteActionMove(player, row1, col1, row2, col2);
    } else {
        game_data.players[player].direction = DIR_WEST;
        RedrawWorldPlayer(player);
    }
}

//----------------------------------------------------------------------------------------------------

void ExecuteActionMove(UINT8 player, UINT8 row1, UINT8 col1, UINT8 row2, UINT8 col2) {
    BOOL move = FALSE;
    if (game_data.world[row2][col2] == W_EMPT) {
        // Move in an empty space:
        move = TRUE;
    } else if (game_data.world[row2][col2] == W_LIFE) {
        // Move inside a cell with a life inside of it:
        move = TRUE;
        // Update the player data:
        if (game_data.players[player].lives < MAX_LIVES_COUNT) {
            game_data.players[player].lives++;
        } else {
            PlayerAddScore(player, SCORE_LIFE);
        }
        // Update the entity count and the HUD:
        DecEntityCount();
        DrawGameScoreAndLives();
    }
    if (move) {
        // Move the player from the old cell to the new one:
        game_data.world[row2][col2] = game_data.world[row1][col1];
        game_data.world[row1][col1] = W_EMPT;
        DrawWorlCell(row1, col1);
        DrawWorlCell(row2, col2);
        game_data.players[player].row = row2;
        game_data.players[player].col = col2;
    }
}
