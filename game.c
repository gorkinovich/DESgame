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
	//...
}

//****************************************************************************************************
// Scores
//****************************************************************************************************

void InitializeScores() {
    UINT8 i;
    for (i = 0; i < MAX_SCORES; ++i) {
        game_data.scores[i] = 0;
    }
    LoadScores();
}

//----------------------------------------------------------------------------------------------------

void AddScore(UINT32 score) {
    UINT8 i, j;
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

void DrawGame() {
	//TODO: Complete this function...
    ClearLCD();
    //...
    //(320-(16*))/2
    //(320-(8*))/2
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
