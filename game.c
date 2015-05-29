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
    game_data.pause = FALSE;
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
    game_data.lastGenRow = NO_GENERATED;
    game_data.lastGenCol = NO_GENERATED;
    game_data.lastGenVal = NO_GENERATED;
    game_data.lastAction = ACTION_NONE;
    game_data.remoteAction = ACTION_NONE;
    game_data.entityCount = 0;
    game_data.lastScore = 0;
    game_data.victory = FALSE;
    game_data.useInput = TRUE;
    game_data.useCommunication = TRUE;
    game_data.updateCount = 0;
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

void PlayerOneAsHost() {
    game_data.hostPlayer = PLAYER_ONE;
    Set8Led(1);
}

//----------------------------------------------------------------------------------------------------

void PlayerTwoAsHost() {
    game_data.hostPlayer = PLAYER_TWO;
    Set8Led(2);
}

//----------------------------------------------------------------------------------------------------

void DecEntityCount() {
    if (game_data.entityCount > 0) {
        game_data.entityCount--;
    }
}

//----------------------------------------------------------------------------------------------------

void PlayerQuitOneLife(UINT8 player) {
    if (game_data.players[player].lives > 1) {
        game_data.players[player].lives--;
        if (player == game_data.hostPlayer) {
            DrawGameScoreAndLives();
        }
    } else {
        game_data.players[player].lives = 0;
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

void PutGeneratedEntity(UINT8 value) {
    UINT8 c = 0, row, col;
    do {
        row = (rand() % (MAX_ROWS - 3)) + 1;
        col = (rand() % (MAX_COLS - 2)) + 1;
        if (IsCellEmpty(row, col)) {
            game_data.entityCount++;
            game_data.world[row][col] = value;
            DrawWorlCell(row, col);
            game_data.lastGenRow = row;
            game_data.lastGenCol = col;
            game_data.lastGenVal = value;
            return;
        } else {
            ++c;
        }
    } while(c < 5);
}

//----------------------------------------------------------------------------------------------------

void UpdateEntityGenerator() {
    if (game_data.hostPlayer == PLAYER_ONE) {
        game_data.lastGenRow = NO_GENERATED;
        game_data.lastGenCol = NO_GENERATED;
        game_data.lastGenVal = NO_GENERATED;
        if (game_data.entityCount < MAX_ENTITY_COUNT) {
            int victim = rand() % 1000;
            if (victim <= 10) {
                PutGeneratedEntity(W_LIFE);
            } else if (victim <= 200) {
                PutGeneratedEntity(W_SOL1);
            }
        }
    } else if (game_data.hostPlayer == PLAYER_TWO && game_data.lastGenRow != NO_GENERATED &&
            game_data.lastGenCol != NO_GENERATED && game_data.lastGenVal != NO_GENERATED) {
        UINT8 row = game_data.lastGenRow;
        UINT8 col = game_data.lastGenCol;
        game_data.entityCount++;
        game_data.world[row][col] = game_data.lastGenVal;
        DrawWorlCell(row, col);
        game_data.lastGenRow = NO_GENERATED;
        game_data.lastGenCol = NO_GENERATED;
        game_data.lastGenVal = NO_GENERATED;
    }
}

//----------------------------------------------------------------------------------------------------

void UpdateGame() {
    if (game_data.state != STATE_GAME) return;
    UpdatePlayerShoot(PLAYER_ONE);
    UpdatePlayerShoot(PLAYER_TWO);
    if (game_data.state != STATE_GAME) return;
    if (game_data.updateCount >= MAX_UPDATE_COUNT) {
        game_data.updateCount = 0;
        ExecuteAction(game_data.hostPlayer, game_data.lastAction);
        game_data.lastAction = ACTION_NONE;
        UINT8 remotePlayer = GetOppositePlayer(game_data.hostPlayer);
        ExecuteAction(remotePlayer, game_data.remoteAction);
        game_data.remoteAction = ACTION_NONE;
        UpdateEntityGenerator();
    } else {
        game_data.updateCount++;
    }
}

//----------------------------------------------------------------------------------------------------

void UpdateOnKeyboard(UINT32 keys) {
    if (game_data.state == STATE_GAME) {
        if (game_data.useInput) {
            // The keys used in the game state:
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
        }
    } else if (game_data.state == STATE_MENU) {
        // The keys used in the menu state:
        if (keys & KEY_SB01) {
            GotoStateNewGame();
        } else if (keys & KEY_SB05) {
            GotoStateScores();
        } else if (keys & KEY_SB09) {
            GotoStateHelp();
        } else if (keys & KEY_SB04) {
            SendTestMessage();
        }
    } else if (game_data.state == STATE_NEW_GAME) {
        // The keys used in the new game state:
        if (keys & KEY_SB16) {
            SendAbortMessage();
            GotoStateMenu();
        }
    } else if (game_data.state == STATE_GAME_OVER) {
        // The keys used in the game over state:
        GotoStateMenu();
    } else if (game_data.state == STATE_SCORES) {
        // The keys used in the scores state:
        GotoStateMenu();
    } else if (game_data.state == STATE_HELP) {
        // The keys used in the help state:
        GotoStateMenu();
    }
}

//----------------------------------------------------------------------------------------------------

DECL_WITH_IRQ_ATTRIBUTE(UpdateOnTimer);

void UpdateOnTimer() {
    if (game_data.pause) return;
    if (game_data.state == STATE_GAME) {
        if (game_data.hostPlayer == PLAYER_ONE) {
            StartUpdateCommunication();
            UpdateGame();
            FinishUpdateCommunication();
        }
    } else {
        ++game_seed;
    }
    ClearTimer0PendingInterrupt();
}

//****************************************************************************************************
// Communication
//****************************************************************************************************

BOOL SendByte(char data) {
    SendByteUART1(data);
    char value = GetCharUART1();
    return value == data;
}

//----------------------------------------------------------------------------------------------------

char ReceiveByte() {
    char value = GetCharUART1();
    SendByteUART1(value);
    return value;
}

//----------------------------------------------------------------------------------------------------

void StartUpdateCommunication() {
    WaitTxEmptyUART1();
    SendByte(MSG_UPDATE);
    SendByteUART1(game_data.lastAction);
    game_data.remoteAction = GetCharUART1();
    SendByte(game_data.updateCount);
}

//----------------------------------------------------------------------------------------------------

void FinishUpdateCommunication() {
    SendByte(game_data.lastGenRow);
    SendByte(game_data.lastGenCol);
    SendByte(game_data.lastGenVal);
}

//----------------------------------------------------------------------------------------------------

BOOL SendNewGameMessage() {
    WaitTxEmptyUART1();
    return SendByte(MSG_NEW_GAME);
}

//----------------------------------------------------------------------------------------------------

BOOL SendAbortMessage() {
    WaitTxEmptyUART1();
    return SendByte(MSG_ABORT);
}

//----------------------------------------------------------------------------------------------------

BOOL SendTestMessage() {
    WaitTxEmptyUART1();
    return SendByte(MSG_TEST);
}

//----------------------------------------------------------------------------------------------------

void NewGameMessageReceived() {
    if (game_data.state == STATE_NEW_GAME) {
        // Go to the game state:
        GotoStateGame();
    } else if (game_data.state == STATE_MENU || game_data.state == STATE_SCORES ||
        game_data.state == STATE_HELP || game_data.state == STATE_GAME_OVER) {
        // Set this machine as the player 2:
        PlayerTwoAsHost();
    }
}

//----------------------------------------------------------------------------------------------------

static void GenericUpdateOnReceiveUART(char value) {
    // Check the type of the message:
    switch (value) {
    case MSG_NEW_GAME:
        NewGameMessageReceived();
        break;
    case MSG_ABORT:
        InitializeGame();
        break;
    case MSG_TEST:
        if (IsPoint8Led()) {
            ClearPoint8Led();
        } else {
            SetPoint8Led();
        }
        break;
    }
}

//----------------------------------------------------------------------------------------------------

void UpdateOnReceiveUART() {
    if (!game_data.useCommunication) return;
    char value = 0;
    if (game_data.hostPlayer == PLAYER_TWO) {
        // Get the head of the message:
        value = ReceiveByte();
        // Check the state of the game:
        if (game_data.state == STATE_GAME) {
            // Check the type of the message:
            if (value == MSG_UPDATE) {
                game_data.useInput = FALSE;
                game_data.remoteAction = GetCharUART1();
                SendByteUART1(game_data.lastAction);
                game_data.updateCount = ReceiveByte();
                game_data.lastGenRow = ReceiveByte();
                game_data.lastGenCol = ReceiveByte();
                game_data.lastGenVal = ReceiveByte();
                UpdateGame();
                game_data.useInput = TRUE;
            }
        } else {
            // Check the type of the message:
            GenericUpdateOnReceiveUART(value);
        }
    } else if (game_data.hostPlayer == PLAYER_ONE) {
        // Check the state of the game:
        if (game_data.state != STATE_GAME) {
            // Get the head of the message:
            value = ReceiveByte();
            // Check the type of the message:
            GenericUpdateOnReceiveUART(value);
        }
    }
}

//----------------------------------------------------------------------------------------------------

DECL_WITH_IRQ_ATTRIBUTE(OnReceiveUART);

void OnReceiveUART() {
    UpdateOnReceiveUART();
    ClearUART1PendingInterrupt();
}
