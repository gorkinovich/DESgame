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

//****************************************************************************************************
// General
//****************************************************************************************************

void InitializeGame() {
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
	//TODO: Complete this function...
    ClearLCD();
    PutString2LCD(96, 32, LCD_COLOR_BLACK, "NEW GAME"); //8
    PutStringLCD(0, 96, LCD_COLOR_BLACK, "Waiting player 2...");
    PutStringLCD(0, 96, LCD_COLOR_BLACK, "0)");
    //...
    //(320-(8*))/2
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
	//TODO: Complete this function...
    ClearLCD();
    PutString2LCD(96, 32, LCD_COLOR_BLACK, "VICTORY!"); //8
    PutString2LCD(88, 32, LCD_COLOR_BLACK, "GAME OVER"); //9
    //...
    //(320-(8*))/2
}

//----------------------------------------------------------------------------------------------------

void DrawScores() {
	//TODO: Complete this function...
    ClearLCD();
    PutString2LCD(112, 32, LCD_COLOR_BLACK, "SCORES"); //6
    PutStringLCD(0, 96, LCD_COLOR_BLACK, "");
    //...
    //(320-(8*))/2
}

//----------------------------------------------------------------------------------------------------

void DrawHelp() {
    ClearLCD();
    PutString2LCD(128, 32, LCD_COLOR_BLACK, "HELP"); //4
    PutStringLCD(16,  80, LCD_COLOR_BLACK, "|----------|-----------------------|");
    PutStringLCD(16,  96, LCD_COLOR_BLACK, "| Keyboard | Move with the arrows: |");
    PutStringLCD(16, 112, LCD_COLOR_BLACK, "|  A B C D |                       |");
    PutStringLCD(16, 128, LCD_COLOR_BLACK, "|  E F G H |         UP:B          |");
    PutStringLCD(16, 144, LCD_COLOR_BLACK, "|  I J K L | LEFT:E        RIGHT:G |");
    PutStringLCD(16, 160, LCD_COLOR_BLACK, "|  M N O P |        DOWN:J         |");
    PutStringLCD(16, 176, LCD_COLOR_BLACK, "|----------|                       |");
    PutStringLCD(16, 192, LCD_COLOR_BLACK, "           | Fire with: F          |");
    PutStringLCD(16, 208, LCD_COLOR_BLACK, "           |-----------------------|");
}
