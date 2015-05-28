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
// Render
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
    char * score = IntToString(game_data.lastScore);
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
                IntToString10(game_data.scores[i]));
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

//----------------------------------------------------------------------------------------------------

void DrawGame() {
    UINT8 i, j;
    for (i = 0; i < MAX_ROWS; ++i) {
        for (j = 0; j < MAX_COLS; ++j) {
            DrawWorlCell(i, j);
        }
    }
    PutStringLCD(0, 224, LCD_COLOR_BLACK, "Score:");
    PutStringLCD(240, 224, LCD_COLOR_BLACK, "Lives:");
    DrawGameScoreAndLives();
}

//----------------------------------------------------------------------------------------------------

void DrawGameScoreAndLives() {
    PutStringLCD(56, 224, LCD_COLOR_BLACK, IntToString10(game_data.lastScore));
    PutStringLCD(296, 224, LCD_COLOR_BLACK, IntToString3(game_data.players[game_data.hostPlayer].lives));
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
