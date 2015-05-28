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
static unsigned int game_seed = 0;

//****************************************************************************************************
// Game
//****************************************************************************************************

void InitializeGame() {
    InitializeScores();
    InitializeNewGame();
    PlayerOneAsHost();
    GotoStateMenu();
}

//----------------------------------------------------------------------------------------------------

void InitializeNewGame() {
    UINT8 i;
    InitializeWorld();
    for (i = 0; i < MAX_PLAYERS; ++i) {
        InitializePlayer(i);
    }
    game_data.lastAction = ACTION_NONE;
    game_data.remoteAction = ACTION_NONE;
    game_data.entityCount = 0;
    game_data.lastScore = 0;
    game_data.victory = FALSE;
    srand(game_seed);
}

//----------------------------------------------------------------------------------------------------

void InitializePlayer(UINT8 player) {
    if (player < MAX_PLAYERS) {
        if (player == PLAYER_ONE) {
            game_data.players[player].direction = DIR_EAST;
            FindPlayerPosition(player, W_PLA1);
        } else if (player == PLAYER_TWO) {
            game_data.players[player].direction = DIR_WEST;
            FindPlayerPosition(player, W_PLA2);
        }
        game_data.players[player].lives = MAX_LIVES;
        game_data.players[player].shoot.alive = FALSE;
        game_data.players[player].shoot.direction = game_data.players[player].direction;
        game_data.players[player].shoot.row = 0;
        game_data.players[player].shoot.col = 0;
    }
}

//----------------------------------------------------------------------------------------------------

void FindPlayerPosition(UINT8 player, UINT8 wid) {
    UINT8 i, j;
    for (i = 0; i < MAX_ROWS; ++i) {
        for (j = 0; j < MAX_COLS; ++j) {
            if (game_data.world[i][j] == wid) {
                game_data.players[player].row = i;
                game_data.players[player].col = j;
                return;
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------

UINT8 GetOppositePlayer(UINT8 victim) {
    return victim == PLAYER_ONE ? PLAYER_TWO : PLAYER_ONE;
}

//----------------------------------------------------------------------------------------------------

void PlayerOneAsHost() {
    game_data.hostPlayer = PLAYER_ONE;
}

//----------------------------------------------------------------------------------------------------

void PlayerTwoAsHost() {
    game_data.hostPlayer = PLAYER_TWO;
}

//----------------------------------------------------------------------------------------------------

void PlayerQuitOneLife(UINT8 player) {
    if (game_data.players[player].lives > 1) {
        game_data.players[player].lives--;
        if (player == game_data.hostPlayer) {
            DrawGameScoreAndLives();
        }
    } else {
        GotoStateGameOver();
    }
}

//----------------------------------------------------------------------------------------------------

void PlayerAddScore(UINT8 player, UINT32 value) {
    if (game_data.hostPlayer == player) {
        game_data.lastScore += value;
        DrawGameScoreAndLives();
    }
}

//----------------------------------------------------------------------------------------------------

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
        if (game_data.entityCount > 0) {
            game_data.entityCount--;
        }
        PlayerAddScore(player, SCORE_SOLDIER1);
        ClearWorldCell(row, col);
        break;
    case W_SOL2:
        if (game_data.entityCount > 0) {
            game_data.entityCount--;
        }
        PlayerAddScore(player, SCORE_SOLDIER2);
        ClearWorldCell(row, col);
        break;
    case W_LIFE:
        if (game_data.entityCount > 0) {
            game_data.entityCount--;
        }
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
        move = TRUE;
    } else if (game_data.world[row2][col2] == W_LIFE) {
        move = TRUE;
        if (game_data.players[player].lives < MAX_LIVES_COUNT) {
            game_data.players[player].lives++;
        } else {
            PlayerAddScore(player, SCORE_LIFE);
        }
        if (game_data.entityCount > 0) {
            game_data.entityCount--;
        }
        DrawGameScoreAndLives();
    }
    if (move) {
        game_data.world[row2][col2] = game_data.world[row1][col1];
        game_data.world[row1][col1] = W_EMPT;
        DrawWorlCell(row1, col1);
        DrawWorlCell(row2, col2);
        game_data.players[player].row = row2;
        game_data.players[player].col = col2;
    }
}

//----------------------------------------------------------------------------------------------------

void ExecuteAction(UINT8 player, UINT8 action) {
    if (game_data.players[player].lives > 0) {
        switch (action) {
        case ACTION_FIRE:  ExecuteActionFire(player);  break;
        case ACTION_NORTH: ExecuteActionNorth(player); break;
        case ACTION_EAST:  ExecuteActionEast(player);  break;
        case ACTION_SOUTH: ExecuteActionSouth(player); break;
        case ACTION_WEST:  ExecuteActionWest(player);  break;
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

//----------------------------------------------------------------------------------------------------

void PutGeneratedEntity(UINT8 value) {
    UINT8 c = 0, row, col;
    do {
        row = (rand() % (MAX_ROWS - 3)) + 1;
        col = (rand() % (MAX_COLS - 2)) + 1;
        if (IsCellEmpty(row, col)) {
            game_data.entityCount++;
            game_data.world[row][col] = value;
            DrawWorlCell(row, col);
            return;
        } else {
            ++c;
        }
    } while(c < 5);
}

//----------------------------------------------------------------------------------------------------

void UpdateEntityGenerator() {
    if (game_data.entityCount < MAX_ENTITY_COUNT) {
        int victim = rand() % 1000;
        if (victim <= 10) {
            PutGeneratedEntity(W_LIFE);
        } else if (victim <= 200) {
            PutGeneratedEntity(W_SOL1);
        }
    }
}

//----------------------------------------------------------------------------------------------------

void UpdateGame() {
    static UINT8 count = 0;
    UpdatePlayerShoot(PLAYER_ONE);
    UpdatePlayerShoot(PLAYER_TWO);
    if (count >= MAX_UPDATE_COUNT) {
        count = 0;
        ExecuteAction(game_data.hostPlayer, game_data.lastAction);
        game_data.lastAction = ACTION_NONE;
        UINT8 remotePlayer = GetOppositePlayer(game_data.hostPlayer);
        ExecuteAction(remotePlayer, game_data.remoteAction);
        game_data.remoteAction = ACTION_NONE;
        UpdateEntityGenerator();
    } else {
        ++count;
    }
}

//****************************************************************************************************
// Communication
//****************************************************************************************************

void SendStartSignal() {
    //TODO: Complete this function...
    //...
}

//----------------------------------------------------------------------------------------------------

void StartSignalReceived() {
    if (game_data.state == STATE_NEW_GAME) {
        // Go to the game state:
        GotoStateGame();
    } else if (game_data.state == STATE_MENU || game_data.state == STATE_SCORES ||
        game_data.state == STATE_HELP || game_data.state == STATE_GAME_OVER) {
        // Set this machine as the player 2:
        PlayerTwoAsHost();
    }
}

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
        SendStartSignal();
        DrawNewGame();
    } else {
        // Send the start signal to the player 1 & go to the game state:
        SendStartSignal();
        GotoStateGame();
    }
}

//----------------------------------------------------------------------------------------------------

void GotoStateGame() {
    // Change the current state, initialize a new game & draw the screen:
    game_data.state = STATE_GAME;
    InitializeNewGame();
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

//----------------------------------------------------------------------------------------------------

void UpdateOnKeyboard(UINT32 keys) {
    if (game_data.state == STATE_GAME) {
        if (keys & KEY_SB06) {
            game_data.lastAction = ACTION_FIRE;
        } else if (keys & KEY_SB05) {
            game_data.lastAction = ACTION_NORTH;
        } else if (keys & KEY_SB07) {
            game_data.lastAction = ACTION_SOUTH;
        } else if (keys & KEY_SB02) {
            game_data.lastAction = ACTION_WEST;
        } else if (keys & KEY_SB10) {
            game_data.lastAction = ACTION_EAST;
        }
    } else if (game_data.state == STATE_MENU) {
        if (keys & KEY_SB01) {
            GotoStateNewGame();
        } else if (keys & KEY_SB05) {
            GotoStateScores();
        } else if (keys & KEY_SB09) {
            GotoStateHelp();
        }
    } else if (game_data.state == STATE_NEW_GAME) {
        if (keys & KEY_SB16) {
            GotoStateMenu();
        }
    } else if (game_data.state == STATE_GAME_OVER) {
        GotoStateMenu();
    } else if (game_data.state == STATE_SCORES) {
        GotoStateMenu();
    } else if (game_data.state == STATE_HELP) {
        GotoStateMenu();
    }
}

//----------------------------------------------------------------------------------------------------

void UpdateOnTimer() {
    //TODO: Complete this function...
    if (game_data.state == STATE_GAME) {
        if (game_data.hostPlayer == PLAYER_ONE) {
            UpdateGame();
        }
    } else {
        ++game_seed;
    }
    //...
}

//----------------------------------------------------------------------------------------------------

void UpdateOnReceiveUART(unsigned value) {
    //TODO: Complete this function...
    if (value == 1) {
        if (game_data.state == STATE_GAME) {
            //...
        } else {
            //...
        }
    }
    //...
}
