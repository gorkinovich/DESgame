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
// States
//****************************************************************************************************

void GotoStateMenu() {
    // Change the current state & draw the screen:
    game_data.state = STATE_MENU;
    DrawMenu();
}

//----------------------------------------------------------------------------------------------------

void GotoStateNewGame() {
    if (game_data.hostPlayer == PLAYER_ONE) {
        // Change the current state, send the star signal to the player 2 & draw the screen:
        game_data.state = STATE_NEW_GAME;
        SendNewGameMessage();
        DrawNewGame();
    } else {
        // Send the start signal to the player 1 & go to the game state:
        SendNewGameMessage();
        GotoStateGame();
    }
}

//----------------------------------------------------------------------------------------------------

void GotoStateGame() {
    // Change the current state, initialize a new game & draw the screen:
    InitializeNewGame();
    game_data.state = STATE_GAME;
    DrawGame();
}

//----------------------------------------------------------------------------------------------------

void GotoStateGameOver() {
    // Set if there is a victory or not, add the score & reset the host player:
    game_data.victory = game_data.players[game_data.hostPlayer].lives > 0;
    if (game_data.victory) { AddScore(game_data.lastScore); }
    PlayerOneAsHost();
    // Change the current state & draw the screen:
    game_data.state = STATE_GAME_OVER;
    DrawGameOver();
}

//----------------------------------------------------------------------------------------------------

void GotoStateScores() {
    // Change the current state & draw the screen:
    game_data.state = STATE_SCORES;
    DrawScores();
}

//----------------------------------------------------------------------------------------------------

void GotoStateHelp() {
    // Change the current state & draw the screen:
    game_data.state = STATE_HELP;
    DrawHelp();
}
