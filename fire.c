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
// Fire
//****************************************************************************************************

void ExterminateAnnihilateDestroy(UINT8 player, UINT8 row, UINT8 col) {
    switch (game_data.world[row][col]) {
    case W_PLA1:
        if (player == PLAYER_TWO) {
            PlayerAddScore(player, SCORE_PLAYER);
            PlayerQuitOneLife(PLAYER_ONE);
        }
        break;
    case W_PLA2:
        if (player == PLAYER_ONE) {
            PlayerAddScore(player, SCORE_PLAYER);
            PlayerQuitOneLife(PLAYER_TWO);
        }
        break;
    case W_SHO1:
    case W_SHO2:
        game_data.players[PLAYER_ONE].shoot.alive = FALSE;
        game_data.players[PLAYER_TWO].shoot.alive = FALSE;
        ClearWorldCell(row, col);
        break;
    case W_SOL1:
        DecEntityCount();
        PlayerAddScore(player, SCORE_SOLDIER1);
        ClearWorldCell(row, col);
        break;
    case W_SOL2:
        DecEntityCount();
        PlayerAddScore(player, SCORE_SOLDIER2);
        ClearWorldCell(row, col);
        break;
    case W_LIFE:
        DecEntityCount();
        ClearWorldCell(row, col);
        break;
    }
}

//----------------------------------------------------------------------------------------------------

void ExecuteActionFire(UINT8 player) {
    if (!game_data.players[player].shoot.alive) {
        UINT8 row = game_data.players[player].row;
        UINT8 col = game_data.players[player].col;
        switch (game_data.players[player].direction) {
        case DIR_NORTH: --row; break;
        case DIR_EAST:  ++col; break;
        case DIR_SOUTH: ++row; break;
        case DIR_WEST:  --col; break;
        }
        if (IsCellEmpty(row, col)) {
            game_data.players[player].shoot.alive = TRUE;
            game_data.players[player].shoot.direction = game_data.players[player].direction;
            game_data.players[player].shoot.row = row;
            game_data.players[player].shoot.col = col;
            game_data.world[row][col] = W_SHO1;
            DrawWorldShoot(row, col);
        } else {
            ExterminateAnnihilateDestroy(player, row, col);
        }
    }
}

//----------------------------------------------------------------------------------------------------

void UpdatePlayerShoot(UINT8 player) {
    if (game_data.players[player].lives > 0 && game_data.players[player].shoot.alive) {
        UINT8 row1 = game_data.players[player].shoot.row;
        UINT8 col1 = game_data.players[player].shoot.col;
        UINT8 row2 = row1, col2 = col1;
        switch (game_data.players[player].shoot.direction) {
        case DIR_NORTH: --row2; break;
        case DIR_EAST:  ++col2; break;
        case DIR_SOUTH: ++row2; break;
        case DIR_WEST:  --col2; break;
        }
        if (IsCellEmpty(row2, col2)) {
            game_data.players[player].shoot.row = row2;
            game_data.players[player].shoot.col = col2;
            game_data.world[row1][col1] = W_EMPT;
            game_data.world[row2][col2] = W_SHO1;
            DrawWorldEmpty(row1, col1);
            DrawWorldShoot(row2, col2);
        } else {
            game_data.players[player].shoot.alive = FALSE;
            game_data.world[row1][col1] = W_EMPT;
            DrawWorldEmpty(row1, col1);
            ExterminateAnnihilateDestroy(player, row2, col2);
        }
    }
}
