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
// Constants
//****************************************************************************************************

#define CL_WH LCD_COLOR_WHITE
#define CL_LG LCD_COLOR_LIGHTGRAY
#define CL_DG LCD_COLOR_DARKGRAY
#define CL_BL LCD_COLOR_BLACK

//****************************************************************************************************
// Data
//****************************************************************************************************

extern GameData game_data;

//****************************************************************************************************
// World
//****************************************************************************************************

void InitializeWorld() {
    static const UINT8 data[] = {
        W_WALL, W_WALL, W_WALL, W_WALL, W_WALL, W_WALL, W_WALL, W_WALL, W_WALL, W_WALL, W_WALL, W_WALL, W_WALL, W_WALL, W_WALL, W_WALL, W_WALL, W_WALL, W_WALL, W_WALL,
        W_WALL, W_PLA1, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_WALL,
        W_WALL, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_WALL,
        W_WALL, W_EMPT, W_EMPT, W_WALL, W_WALL, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_WALL, W_WALL, W_EMPT, W_EMPT, W_WALL,
        W_WALL, W_EMPT, W_EMPT, W_WALL, W_WALL, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_WALL, W_WALL, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_WALL, W_WALL, W_EMPT, W_EMPT, W_WALL,
        W_WALL, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_WALL, W_EMPT, W_WALL, W_WALL, W_WALL, W_WALL, W_EMPT, W_WALL, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_WALL,
        W_WALL, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_WALL, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_WALL, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_WALL,
        W_WALL, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_WALL, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_WALL, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_WALL,
        W_WALL, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_WALL, W_EMPT, W_WALL, W_WALL, W_WALL, W_WALL, W_EMPT, W_WALL, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_WALL,
        W_WALL, W_EMPT, W_EMPT, W_WALL, W_WALL, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_WALL, W_WALL, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_WALL, W_WALL, W_EMPT, W_EMPT, W_WALL,
        W_WALL, W_EMPT, W_EMPT, W_WALL, W_WALL, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_WALL, W_WALL, W_EMPT, W_EMPT, W_WALL,
        W_WALL, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_WALL,
        W_WALL, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_PLA2, W_WALL,
        W_WALL, W_WALL, W_WALL, W_WALL, W_WALL, W_WALL, W_WALL, W_WALL, W_WALL, W_WALL, W_WALL, W_WALL, W_WALL, W_WALL, W_WALL, W_WALL, W_WALL, W_WALL, W_WALL, W_WALL,
        W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT, W_EMPT
    };
    UINT8 i, j;
    UINT16 k = 0;
    for (i = 0; i < MAX_ROWS; ++i) {
        for (j = 0; j < MAX_COLS; ++j, ++k) {
            game_data.world[i][j] = data[k];
        }
    }
}

//----------------------------------------------------------------------------------------------------

BOOL IsCellEmpty(UINT8 row, UINT8 col) {
    return game_data.world[row][col] == W_EMPT;
}

//----------------------------------------------------------------------------------------------------

BOOL IsCellEmptyOrPickable(UINT8 row, UINT8 col) {
    return game_data.world[row][col] == W_EMPT || game_data.world[row][col] == W_LIFE;
}

//----------------------------------------------------------------------------------------------------

void DrawWorlCell(UINT8 row, UINT8 col) {
    UINT8 victim = game_data.world[row][col];
    switch (victim) {
    case W_PLA1: DrawWorldPlayer1(row, col);  break;
    case W_PLA2: DrawWorldPlayer2(row, col);  break;
    case W_SHO1: DrawWorldShoot(row, col);    break;
    case W_SHO2: DrawWorldShoot(row, col);    break;
    case W_SOL1: DrawWorldSoldier1(row, col); break;
    case W_SOL2: DrawWorldSoldier1(row, col); break;
    case W_LIFE: DrawWorldLife(row, col);     break;
    case W_WALL: DrawWorldWall(row, col);     break;
    default:     DrawWorldEmpty(row, col);    break;
    }
}

//----------------------------------------------------------------------------------------------------

void DrawWorldPlayer1(UINT8 row, UINT8 col) {
    static const UINT8 data[] = {
        CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH,
        CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_BL, CL_BL, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH,
        CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_BL, CL_BL, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH,
        CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_BL, CL_BL, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH,
        CL_WH, CL_BL, CL_BL, CL_BL, CL_WH, CL_WH, CL_WH, CL_BL, CL_BL, CL_WH, CL_WH, CL_WH, CL_BL, CL_BL, CL_BL, CL_WH,
        CL_WH, CL_BL, CL_LG, CL_BL, CL_WH, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_WH, CL_BL, CL_LG, CL_BL, CL_WH,
        CL_WH, CL_BL, CL_LG, CL_BL, CL_BL, CL_LG, CL_LG, CL_BL, CL_BL, CL_LG, CL_LG, CL_BL, CL_BL, CL_LG, CL_BL, CL_WH,
        CL_WH, CL_BL, CL_LG, CL_BL, CL_LG, CL_LG, CL_BL, CL_LG, CL_LG, CL_BL, CL_LG, CL_LG, CL_BL, CL_LG, CL_BL, CL_WH,
        CL_WH, CL_BL, CL_LG, CL_BL, CL_LG, CL_LG, CL_BL, CL_LG, CL_LG, CL_BL, CL_LG, CL_LG, CL_BL, CL_LG, CL_BL, CL_WH,
        CL_WH, CL_BL, CL_LG, CL_BL, CL_LG, CL_BL, CL_LG, CL_LG, CL_LG, CL_LG, CL_BL, CL_LG, CL_BL, CL_LG, CL_BL, CL_WH,
        CL_WH, CL_BL, CL_LG, CL_BL, CL_LG, CL_BL, CL_LG, CL_LG, CL_LG, CL_LG, CL_BL, CL_LG, CL_BL, CL_LG, CL_BL, CL_WH,
        CL_WH, CL_BL, CL_LG, CL_BL, CL_LG, CL_LG, CL_BL, CL_BL, CL_BL, CL_BL, CL_LG, CL_LG, CL_BL, CL_LG, CL_BL, CL_WH,
        CL_WH, CL_BL, CL_LG, CL_BL, CL_BL, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_BL, CL_BL, CL_LG, CL_BL, CL_WH,
        CL_WH, CL_BL, CL_LG, CL_BL, CL_WH, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_WH, CL_BL, CL_LG, CL_BL, CL_WH,
        CL_WH, CL_BL, CL_BL, CL_BL, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_BL, CL_BL, CL_BL, CL_WH,
        CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH
    };
    DrawWorldPlayer(row, col, PLAYER_ONE, data);
}

//----------------------------------------------------------------------------------------------------

void DrawWorldPlayer2(UINT8 row, UINT8 col) {
    static const UINT8 data[] = {
        CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH,
        CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_BL, CL_BL, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH,
        CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_BL, CL_BL, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH,
        CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_BL, CL_BL, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH,
        CL_WH, CL_BL, CL_BL, CL_BL, CL_WH, CL_WH, CL_WH, CL_BL, CL_BL, CL_WH, CL_WH, CL_WH, CL_BL, CL_BL, CL_BL, CL_WH,
        CL_WH, CL_BL, CL_WH, CL_BL, CL_WH, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_WH, CL_BL, CL_WH, CL_BL, CL_WH,
        CL_WH, CL_BL, CL_WH, CL_BL, CL_BL, CL_WH, CL_WH, CL_BL, CL_BL, CL_WH, CL_WH, CL_BL, CL_BL, CL_WH, CL_BL, CL_WH,
        CL_WH, CL_BL, CL_WH, CL_BL, CL_WH, CL_WH, CL_BL, CL_WH, CL_WH, CL_BL, CL_WH, CL_WH, CL_BL, CL_WH, CL_BL, CL_WH,
        CL_WH, CL_BL, CL_WH, CL_BL, CL_WH, CL_WH, CL_BL, CL_WH, CL_WH, CL_BL, CL_WH, CL_WH, CL_BL, CL_WH, CL_BL, CL_WH,
        CL_WH, CL_BL, CL_WH, CL_BL, CL_WH, CL_BL, CL_WH, CL_WH, CL_WH, CL_WH, CL_BL, CL_WH, CL_BL, CL_WH, CL_BL, CL_WH,
        CL_WH, CL_BL, CL_WH, CL_BL, CL_WH, CL_BL, CL_WH, CL_WH, CL_WH, CL_WH, CL_BL, CL_WH, CL_BL, CL_WH, CL_BL, CL_WH,
        CL_WH, CL_BL, CL_WH, CL_BL, CL_WH, CL_WH, CL_BL, CL_BL, CL_BL, CL_BL, CL_WH, CL_WH, CL_BL, CL_WH, CL_BL, CL_WH,
        CL_WH, CL_BL, CL_WH, CL_BL, CL_BL, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_BL, CL_BL, CL_WH, CL_BL, CL_WH,
        CL_WH, CL_BL, CL_WH, CL_BL, CL_WH, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_WH, CL_BL, CL_WH, CL_BL, CL_WH,
        CL_WH, CL_BL, CL_BL, CL_BL, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_BL, CL_BL, CL_BL, CL_WH,
        CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH
    };
    DrawWorldPlayer(row, col, PLAYER_TWO, data);
}

//----------------------------------------------------------------------------------------------------

void DrawWorldSoldier1(UINT8 row, UINT8 col) {
    static const UINT8 data[] = {
        CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH,
        CL_WH, CL_WH, CL_WH, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH,
        CL_WH, CL_WH, CL_BL, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_BL, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_BL, CL_WH,
        CL_WH, CL_BL, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_WH,
        CL_WH, CL_BL, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_BL, CL_WH, CL_WH, CL_WH, CL_WH, CL_BL, CL_WH,
        CL_WH, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH,
        CL_WH, CL_BL, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_BL, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH,
        CL_WH, CL_BL, CL_LG, CL_LG, CL_BL, CL_LG, CL_LG, CL_LG, CL_BL, CL_LG, CL_LG, CL_BL, CL_WH, CL_WH, CL_WH, CL_WH,
        CL_WH, CL_BL, CL_LG, CL_BL, CL_BL, CL_BL, CL_LG, CL_BL, CL_BL, CL_BL, CL_LG, CL_BL, CL_WH, CL_WH, CL_WH, CL_WH,
        CL_WH, CL_BL, CL_LG, CL_LG, CL_BL, CL_LG, CL_LG, CL_LG, CL_BL, CL_LG, CL_LG, CL_LG, CL_BL, CL_WH, CL_WH, CL_WH,
        CL_WH, CL_BL, CL_LG, CL_LG, CL_LG, CL_BL, CL_LG, CL_LG, CL_LG, CL_BL, CL_LG, CL_LG, CL_BL, CL_WH, CL_WH, CL_WH,
        CL_WH, CL_BL, CL_LG, CL_LG, CL_BL, CL_BL, CL_BL, CL_LG, CL_BL, CL_BL, CL_BL, CL_LG, CL_LG, CL_BL, CL_WH, CL_WH,
        CL_WH, CL_BL, CL_LG, CL_LG, CL_LG, CL_BL, CL_LG, CL_LG, CL_LG, CL_BL, CL_LG, CL_LG, CL_LG, CL_BL, CL_WH, CL_WH,
        CL_WH, CL_BL, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_BL, CL_WH,
        CL_WH, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_WH,
        CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH
    };
    DrawGameSprite(col * CELL_WIDTH, row * CELL_HEIGHT, data);
}

//----------------------------------------------------------------------------------------------------

void DrawWorldLife(UINT8 row, UINT8 col) {
    static const UINT8 data[] = {
        CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH,
        CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH,
        CL_WH, CL_WH, CL_WH, CL_DG, CL_DG, CL_DG, CL_WH, CL_WH, CL_WH, CL_DG, CL_DG, CL_DG, CL_WH, CL_WH, CL_WH, CL_WH,
        CL_WH, CL_WH, CL_DG, CL_WH, CL_WH, CL_WH, CL_DG, CL_WH, CL_DG, CL_WH, CL_WH, CL_WH, CL_DG, CL_WH, CL_WH, CL_WH,
        CL_WH, CL_DG, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_DG, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_DG, CL_WH, CL_WH,
        CL_WH, CL_DG, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_DG, CL_WH, CL_WH,
        CL_WH, CL_DG, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_DG, CL_WH, CL_WH,
        CL_WH, CL_DG, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_DG, CL_WH, CL_WH,
        CL_WH, CL_WH, CL_DG, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_DG, CL_WH, CL_WH, CL_WH,
        CL_WH, CL_WH, CL_WH, CL_DG, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_DG, CL_WH, CL_WH, CL_WH, CL_WH,
        CL_WH, CL_WH, CL_WH, CL_WH, CL_DG, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_DG, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH,
        CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_DG, CL_WH, CL_WH, CL_WH, CL_DG, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH,
        CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_DG, CL_WH, CL_DG, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH,
        CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_DG, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH,
        CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH,
        CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH
    };
    DrawGameSprite(col * CELL_WIDTH, row * CELL_HEIGHT, data);
}

//----------------------------------------------------------------------------------------------------

void DrawWorldWall(UINT8 row, UINT8 col) {
    static const UINT8 data[] = {
        CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL,
        CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_BL, CL_BL, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG,
        CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_BL, CL_BL, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG,
        CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_BL, CL_BL, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG,
        CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_BL, CL_BL, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG,
        CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_BL, CL_BL, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG,
        CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_BL, CL_BL, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG,
        CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL,
        CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL,
        CL_BL, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_BL,
        CL_BL, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_BL,
        CL_BL, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_BL,
        CL_BL, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_BL,
        CL_BL, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_BL,
        CL_BL, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_LG, CL_BL,
        CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL, CL_BL
    };
    DrawGameSprite(col * CELL_WIDTH, row * CELL_HEIGHT, data);
}

//----------------------------------------------------------------------------------------------------

void DrawWorldEmpty(UINT8 row, UINT8 col) {
    static const UINT8 data[] = {
        CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH,
        CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH,
        CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH,
        CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH,
        CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH,
        CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH,
        CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH,
        CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH,
        CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH,
        CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH,
        CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH,
        CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH,
        CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH,
        CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH,
        CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH,
        CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH
    };
    DrawGameSprite(col * CELL_WIDTH, row * CELL_HEIGHT, data);
}

//----------------------------------------------------------------------------------------------------

void DrawWorldShoot(UINT8 row, UINT8 col) {
    static const UINT8 data[] = {
        CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH,
        CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH,
        CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH,
        CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH,
        CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH,
        CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH,
        CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_BL, CL_BL, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH,
        CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_BL, CL_BL, CL_BL, CL_BL, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH,
        CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_BL, CL_BL, CL_BL, CL_BL, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH,
        CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_BL, CL_BL, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH,
        CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH,
        CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH,
        CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH,
        CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH,
        CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH,
        CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH, CL_WH
    };
    DrawGameSprite(col * CELL_WIDTH, row * CELL_HEIGHT, data);
}

//----------------------------------------------------------------------------------------------------

void ClearWorldCell(UINT8 row, UINT8 col) {
    game_data.world[row][col] = W_EMPT;
    DrawWorlCell(row, col);
}

//----------------------------------------------------------------------------------------------------

void RedrawWorldPlayer(UINT8 player) {
    if (player == PLAYER_ONE) {
        DrawWorldPlayer1(game_data.players[player].row, game_data.players[player].col);
    } else {
        DrawWorldPlayer2(game_data.players[player].row, game_data.players[player].col);
    }
}
