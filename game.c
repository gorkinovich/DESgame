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

GameData game_data;

//****************************************************************************************************
// General
//****************************************************************************************************

void InitializeGame() {
    //TODO: Complete this function...
    InitializeScores();
    InitializeNewGame();
    //...
}

//****************************************************************************************************
// Scores
//****************************************************************************************************

void InitializeScores() {
    INT8 i;
    for (i = 0; i < MAX_SCORES; ++i) {
        game_data.scores[i] = 0;
    }
    LoadScores();
}

//----------------------------------------------------------------------------------------------------

void AddScore(UINT32 score) {
    INT8 i, j;
    for (i = 0; i < MAX_SCORES; ++i) {
        if (score > game_data.scores[i]) {
            for (j = MAX_SCORES - 1; j > i; --j) {
                game_data.scores[j] = game_data.scores[j - 1];
            }
            game_data.scores[i] = score;
        }
    }
    SaveScores();
}

//----------------------------------------------------------------------------------------------------

char * ScoreToString(UINT32 score) {
    static char buffer[16];
    sprintf(buffer, "%d", score);
    return buffer;
}

//----------------------------------------------------------------------------------------------------

char * ScoreToString10(UINT32 score) {
    static char buffer[16];
    sprintf(buffer, "%10d", score);
    return buffer;
}

//----------------------------------------------------------------------------------------------------

void SaveScores() {
    //TODO: Complete this function...
}

//----------------------------------------------------------------------------------------------------

void LoadScores() {
    //TODO: Complete this function...
}

//****************************************************************************************************
// World
//****************************************************************************************************

#define CELL_WIDTH 16
#define CELL_HEIGHT 16

#define W_EMPT 0x00
#define W_PLA1 0x01
#define W_PLA2 0x02
#define W_PLA3 0x04
#define W_PLA4 0x08
#define W_SOL1 0x10
#define W_SOL2 0x20
#define W_LIFE 0x40
#define W_WALL 0x80

#define CL_WH LCD_COLOR_WHITE
#define CL_LG LCD_COLOR_LIGHTGRAY
#define CL_DG LCD_COLOR_DARKGRAY
#define CL_BL LCD_COLOR_BLACK

//----------------------------------------------------------------------------------------------------

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
    INT8 i, j;
    UINT16 k = 0;
    for (i = 0; i < MAX_ROWS; ++i) {
        for (j = 0; j < MAX_COLS; ++j, ++k) {
            game_data.world[i][j] = data[k];
        }
    }
}

//----------------------------------------------------------------------------------------------------

void DrawWorlCell(UINT8 row, UINT8 col) {
    UINT8 victim = game_data.world[row][col];
    switch (victim) {
    case W_PLA1: DrawWorldPlayer1(row, col); break;
    case W_PLA2: DrawWorldPlayer2(row, col); break;
    case W_SOL1: DrawWorldSoldier1(row, col); break;
    case W_LIFE: DrawWorldLife(row, col); break;
    case W_WALL: DrawWorldWall(row, col); break;
    default: DrawWorldEmpty(row, col); break;
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

//****************************************************************************************************
// Game Logic
//****************************************************************************************************

void InitializeNewGame() {
    //TODO: Complete this function...
    UINT8 i;
    InitializeWorld();
    for (i = 0; i < MAX_PLAYERS; ++i) {
        InitializePlayer(i);
    }
    game_data.lastScore = 0;
    game_data.victory = FALSE;
    //...
}

//----------------------------------------------------------------------------------------------------

void InitializePlayer(UINT8 player) {
    //TODO: Complete this function...
	//...
}

//----------------------------------------------------------------------------------------------------

void DrawGame() {
    UINT8 i, j;
    for (i = 0; i < MAX_ROWS; ++i) {
        for (j = 0; j < MAX_COLS; ++j) {
            DrawWorlCell(i, j);
        }
    }
    PutStringLCD(0, 224, LCD_COLOR_BLACK, "Score:");
    DrawGameScore();
}

//----------------------------------------------------------------------------------------------------

void DrawGameScore() {
    PutStringLCD(240, 224, LCD_COLOR_BLACK, ScoreToString10(game_data.lastScore));
}

//----------------------------------------------------------------------------------------------------

void DrawGameSprite(UINT16 x, UINT16 y, const UINT8 * data) {
    INT8 i, j;
    UINT16 k = 0;
    for (i = 0; i < CELL_HEIGHT; ++i) {
        for (j = 0; j < CELL_WIDTH; ++j, ++k) {
            PutPixelLCD(x + j, y + i, data[k]);
        }
    }
}

//----------------------------------------------------------------------------------------------------

void DrawGameSprite90(UINT16 x, UINT16 y, const UINT8 * data) {
    INT8 i, j;
    UINT16 k = 0;
    for (j = CELL_WIDTH - 1; j >= 0; --j) {
        for (i = 0; i < CELL_HEIGHT; ++i, ++k) {
            PutPixelLCD(x + j, y + i, data[k]);
        }
    }
}

//----------------------------------------------------------------------------------------------------

void DrawGameSprite180(UINT16 x, UINT16 y, const UINT8 * data) {
    INT8 i, j;
    UINT16 k = 0;
    for (i = CELL_HEIGHT - 1; i >= 0; --i) {
        for (j = CELL_WIDTH - 1; j >= 0; --j, ++k) {
            PutPixelLCD(x + j, y + i, data[k]);
        }
    }
}

//----------------------------------------------------------------------------------------------------

void DrawGameSprite270(UINT16 x, UINT16 y, const UINT8 * data) {
    INT8 i, j;
    UINT16 k = 0;
    for (j = 0; j < CELL_WIDTH; ++j) {
        for (i = CELL_HEIGHT - 1; i >= 0; --i, ++k) {
            PutPixelLCD(x + j, y + i, data[k]);
        }
    }
}

//----------------------------------------------------------------------------------------------------

void DrawWorldPlayer(UINT8 row, UINT8 col, UINT8 player, const UINT8 * data) {
    if (player < MAX_PLAYERS) {
        switch (game_data.players[player].direction) {
        case DIR_EAST:
            DrawGameSprite90(col * CELL_WIDTH, row * CELL_HEIGHT, data);
            break;
        case DIR_SOUTH:
            DrawGameSprite180(col * CELL_WIDTH, row * CELL_HEIGHT, data);
            break;
        case DIR_WEST:
            DrawGameSprite270(col * CELL_WIDTH, row * CELL_HEIGHT, data);
            break;
        default:
            DrawGameSprite(col * CELL_WIDTH, row * CELL_HEIGHT, data);
            break;
        }
    }
}

//****************************************************************************************************
// Menus
//****************************************************************************************************

void DrawMenu() {
    ClearLCD();
    PutString2LCD(48, 32, LCD_COLOR_BLACK, "TANK COMMANDER"); //14
    PutStringLCD(116, 96, LCD_COLOR_BLACK, "A) New Game"); //11
    PutStringLCD(116, 128, LCD_COLOR_BLACK, "B) Scores"); //9
    PutStringLCD(116, 160, LCD_COLOR_BLACK, "C) Help"); //7
}

//----------------------------------------------------------------------------------------------------

void DrawNewGame() {
    ClearLCD();
    PutString2LCD(96, 32, LCD_COLOR_BLACK, "NEW GAME"); //8
    PutStringLCD(84, 112, LCD_COLOR_BLACK, "Waiting player 2..."); //19
    PutStringLCD(116, 192, LCD_COLOR_BLACK, "P) Cancel"); //9
}

//----------------------------------------------------------------------------------------------------

void DrawGameOver() {
    ClearLCD();
    if (game_data.victory) {
        PutString2LCD(96, 32, LCD_COLOR_BLACK, "VICTORY!"); //8
    } else {
        PutString2LCD(88, 32, LCD_COLOR_BLACK, "GAME OVER"); //9
    }
    char * score = ScoreToString(game_data.lastScore);
    UINT16 x = (LCD_WIDTH - (FONT_WIDTH * strlen(score))) / 2;
    PutStringLCD(64, 96, LCD_COLOR_BLACK, "This is your last score:"); //24
    PutStringLCD(x, 128, LCD_COLOR_BLACK, score);
    PutStringLCD(48, 192, LCD_COLOR_BLACK, "Press any key to continue..."); //28
}

//----------------------------------------------------------------------------------------------------

void DrawScores() {
    static const UINT8 MAX_LINES = 8;
    static const char * lines[] = {
        "1) ----------", //13
        "2) ----------",
        "3) ----------",
        "4) ----------",
        "5) ----------",
        "6) ----------",
        "7) ----------",
        "8) ----------"
    };
    UINT8 i;
    ClearLCD();
    PutString2LCD(112, 32, LCD_COLOR_BLACK, "SCORES"); //6
    for (i = 0; i < MAX_LINES; ++i) {
        PutStringLCD(108, 80 + FONT_HEIGHT * i, LCD_COLOR_BLACK, lines[i]);
    }
    for (i = 0; i < MAX_SCORES; ++i) {
        if (game_data.scores[i]) {
            PutStringLCD(132, 80 + i * FONT_HEIGHT, LCD_COLOR_BLACK,
                ScoreToString10(game_data.scores[i]));
        }
    }
}

//----------------------------------------------------------------------------------------------------

void DrawHelp() {
    static const UINT8 MAX_LINES = 9;
    static const char * lines[] = {
        "|----------|-----------------------|",
        "| Keyboard | Move with the arrows: |",
        "|  A B C D |                       |",
        "|  E F G H |         UP:B          |",
        "|  I J K L | LEFT:E        RIGHT:G |",
        "|  M N O P |        DOWN:J         |",
        "|----------|                       |",
        "           | Fire with: F          |",
        "           |-----------------------|"
    };
    UINT8 i;
    ClearLCD();
    PutString2LCD(128, 32, LCD_COLOR_BLACK, "HELP"); //4
    for (i = 0; i < MAX_LINES; ++i) {
        PutStringLCD(16, 80 + FONT_HEIGHT * i, LCD_COLOR_BLACK, lines[i]);
    }
}
