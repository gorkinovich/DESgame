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

#include "44b.h"
#include "44blib.h"
#include "stdio.h"
#include "yukino.h"
#include "game.h"

//****************************************************************************************************
// Logic
//****************************************************************************************************

/*
#define MAX_PLAYERS 4
#define BODY_SIZE   10

#define INC_SPEED  50
#define MAX_SPEED 100
#define MIN_SPEED 800

#define MATH_ABS(value) ((value) >= 0 ? (value) : -(value))
#define MATH_DIR(value) ((value) == 0 ? 0 : ((value) > 0 ? 1 : -1))

typedef struct {
    INT16 x, y;
} Point;

typedef struct {
    BOOL alive;
    unsigned speed;
    Point nextAngle;
    Point direction;
    Point body[BODY_SIZE];
} Entity;

static BOOL PauseGame;
static UINT8 AlivePlayers;
static Entity Players[MAX_PLAYERS];

//----------------------------------------------------------------------------------------------------

inline void InitializeGame() {
    int i, j;
    PauseGame = FALSE;
    AlivePlayers = 4;
    Set8Led(AlivePlayers);
    ClearLCD();
    const INT16 DIR_X[] = {  1,  1, -1, -1 };
    const INT16 DIR_Y[] = { -1,  1, -1,  1 };
    const INT16 POS_X[] = { 20, 299,  20, 299 };
    const INT16 POS_Y[] = { 20,  20, 219, 219 };
    // 10,10 -> 309,10 -> 10,229 -> 309,229
    for (i = 0; i < MAX_PLAYERS; ++i) {
        Players[i].alive = TRUE;
        Players[i].speed = 100;
        Players[i].nextAngle.x = 3;
        Players[i].nextAngle.y = 3;
        Players[i].direction.x = DIR_X[i] * Players[i].nextAngle.x;
        Players[i].direction.y = DIR_Y[i] * Players[i].nextAngle.y;
        INT16 x = POS_X[i] - (DIR_X[i] * BODY_SIZE / 2);
        INT16 y = POS_Y[i] - (DIR_Y[i] * BODY_SIZE / 2);
        for (j = BODY_SIZE - 1; j >= 0; --j) {
            Players[i].body[j].x = x;
            Players[i].body[j].y = y;
            PutPixelLCD(x, y, LCD_COLOR_BLACK);
            x += DIR_X[i], y += DIR_Y[i];
        }
    }
}

//----------------------------------------------------------------------------------------------------

inline BOOL IsPlayerBody(UINT8 player, INT16 x, INT16 y) {
    UINT8 i;
    for (i = 0; i < BODY_SIZE; ++i) {
        if (Players[player].body[i].x == x && Players[player].body[i].y == y) {
            return TRUE;
        }
    }
    return FALSE;
}

//----------------------------------------------------------------------------------------------------

inline BOOL CheckPlayerCollision(UINT8 player, INT16 x, INT16 y) {
    INT16 i, j;
    for (i = y - 1; i <= y + 1; ++i) {
        for (j = x - 1; j <= x + 1; ++j) {
            if (i < 0 || i >= LCD_HEIGHT || j < 0 || j >= LCD_WIDTH) {
                continue;
            } else if (!IsPlayerBody(player, j, i) && (GetPixelLCD(j, i) != LCD_COLOR_WHITE)) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

//----------------------------------------------------------------------------------------------------

inline BOOL UpdateMovePlayer(UINT8 player, INT16 x, INT16 y) {
    // Comprobar si colisiona el jugador con alguien:
    UINT8 i;
    if (CheckPlayerCollision(player, x, y)) {
        Players[player].alive = FALSE;
        for (i = 0; i < BODY_SIZE; ++i) {
            PutPixelLCD(Players[player].body[i].x, Players[player].body[i].y, LCD_COLOR_WHITE);
        }
        --AlivePlayers;
        Set8Led(AlivePlayers);
        return TRUE;
    }
    if (Players[player].alive) {
        // Desplazar al jugador repintando su posición:
        i = BODY_SIZE - 1;
        PutPixelLCD(Players[player].body[i].x, Players[player].body[i].y, LCD_COLOR_WHITE);
        for (; i > 0; --i) {
            Players[player].body[i].x = Players[player].body[i - 1].x;
            Players[player].body[i].y = Players[player].body[i - 1].y;
            PutPixelLCD(Players[player].body[i].x, Players[player].body[i].y, LCD_COLOR_BLACK);
        }
        Players[player].body[0].x = x;
        Players[player].body[0].y = y;
        PutPixelLCD(x, y, LCD_COLOR_BLACK);
        // Comprobar si ha colisionado con el borde de la pantalla:
        BOOL hcollision = (x <= 0) || (x >= LCD_WIDTH - 1);
        BOOL vcollision = (y <= 0) || (y >= LCD_HEIGHT - 1);
        if (hcollision || vcollision) {
            INT16 dx = MATH_DIR(Players[player].direction.x);
            INT16 dy = MATH_DIR(Players[player].direction.y);
            Players[player].direction.x = Players[player].nextAngle.x * dx;
            Players[player].direction.y = Players[player].nextAngle.y * dy;
            if (hcollision) { Players[player].direction.x *= -1; }
            if (vcollision) { Players[player].direction.y *= -1; }
            return TRUE;
        }
    }
    return FALSE;
}

//----------------------------------------------------------------------------------------------------

inline void UpdatePlayer(UINT8 player) {
    if (PauseGame || !Players[player].alive) return;
    INT16 x  = Players[player].body[0].x;
    INT16 y  = Players[player].body[0].y;
    INT16 dx = Players[player].direction.x;
    INT16 dy = Players[player].direction.y;
    INT16 ax = MATH_ABS(dx), ay = MATH_ABS(dy);
    INT16 bx = MATH_DIR(dx), by = MATH_DIR(dy);
    INT16 c, i, j;
    if (ax < ay) {
        c = ay / ax, x += bx;
        for (i = 0, j = 0; i < ay; ++i) {
            y += by, ++j;
            if (UpdateMovePlayer(player, x, y)) {
                break;
            }
            if (j >= c) {
                x += bx, j = 0;
            }
        }
    } else {
        c = ax / ay, y += by;
        for (i = 0, j = 0; i < ax; ++i) {
            x += bx, ++j;
            if (UpdateMovePlayer(player, x, y)) {
                break;
            }
            if (j >= c) {
                y += by, j = 0;
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------

inline void AddSpeed(UINT8 player) {
    if (Players[player].speed > MAX_SPEED) {
        Players[player].speed -= INC_SPEED;
    }
}

//----------------------------------------------------------------------------------------------------

inline void RemoveSpeed(UINT8 player) {
    if (Players[player].speed < MIN_SPEED) {
        Players[player].speed += INC_SPEED;
    }
}

//----------------------------------------------------------------------------------------------------

inline void MakeGrowAxisX(UINT8 player) {
    if (Players[player].nextAngle.y > 1) {
        Players[player].nextAngle.y -= 1;
        Players[player].nextAngle.x += 1;
    }
}

//----------------------------------------------------------------------------------------------------

inline void MakeGrowAxisY(UINT8 player) {
    if (Players[player].nextAngle.x > 1) {
        Players[player].nextAngle.x -= 1;
        Players[player].nextAngle.y += 1;
    }
}

//****************************************************************************************************
// Input
//****************************************************************************************************

void OnButtonDown(unsigned value) {
    if (value & BUTTON_LEFT) {
        TurnOnLeftLed();
        InitializeGame();
    } else if (value & BUTTON_RIGHT) {
        TurnOnRightLed();
        PauseGame = !PauseGame;
    }
}

//----------------------------------------------------------------------------------------------------

void OnButtonUp(unsigned value) {
    if (value & BUTTON_LEFT) {
        TurnOffLeftLed();
    } else if (value & BUTTON_RIGHT) {
        TurnOffRightLed();
    }
}

//----------------------------------------------------------------------------------------------------

void OnKeyboardDown() {
    if (PauseGame) return;
    unsigned keys = GetKeys();

    if (keys & KEY_SB01) AddSpeed(0);
    if (keys & KEY_SB02) RemoveSpeed(0);
    if (keys & KEY_SB03) MakeGrowAxisX(0);
    if (keys & KEY_SB04) MakeGrowAxisY(0);

    if (keys & KEY_SB05) AddSpeed(1);
    if (keys & KEY_SB06) RemoveSpeed(1);
    if (keys & KEY_SB07) MakeGrowAxisX(1);
    if (keys & KEY_SB08) MakeGrowAxisY(1);

    if (keys & KEY_SB09) AddSpeed(2);
    if (keys & KEY_SB10) RemoveSpeed(2);
    if (keys & KEY_SB11) MakeGrowAxisX(2);
    if (keys & KEY_SB12) MakeGrowAxisY(2);

    if (keys & KEY_SB13) AddSpeed(3);
    if (keys & KEY_SB14) RemoveSpeed(3);
    if (keys & KEY_SB15) MakeGrowAxisX(3);
    if (keys & KEY_SB16) MakeGrowAxisY(3);
}

//****************************************************************************************************
// Timers
//****************************************************************************************************

DECL_WITH_IRQ_ATTRIBUTE(OnTimer0);
DECL_WITH_IRQ_ATTRIBUTE(OnTimer1);
DECL_WITH_IRQ_ATTRIBUTE(OnTimer2);
DECL_WITH_IRQ_ATTRIBUTE(OnTimer3);

//----------------------------------------------------------------------------------------------------

inline void GenericOnTimer(UINT8 timer, UINT8 player) {
    if (PauseGame) return;
    UpdatePlayer(player);
    SimpleChangeTimerInterrupts(timer, Players[player].speed);
}

//----------------------------------------------------------------------------------------------------

void OnTimer0() {
    GenericOnTimer(TIMER_ID0, 0);
    ClearTimer0PendingInterrupt();
}

//----------------------------------------------------------------------------------------------------

void OnTimer1() {
    GenericOnTimer(TIMER_ID1, 1);
    ClearTimer1PendingInterrupt();
}

//----------------------------------------------------------------------------------------------------

void OnTimer2() {
    GenericOnTimer(TIMER_ID2, 2);
    ClearTimer2PendingInterrupt();
}

//----------------------------------------------------------------------------------------------------

void OnTimer3() {
    GenericOnTimer(TIMER_ID3, 3);
    ClearTimer3PendingInterrupt();
}
//*/

//****************************************************************************************************
// Main entry:
//****************************************************************************************************

void Main() {
    // Initialize the system:
    sys_init();
    InitializeSystem();

    // Initialize the output:
    TurnOnLCD();
    Set8Led(0);

    /*
    // Initialize the data:
    InitializeGame();

    // Initialize the input:
    SetOnKeyboardDown(OnKeyboardDown);
    InitializeKeyboardInterrupts();
    SetOnButtonDown(OnButtonDown);
    SetOnButtonUp(OnButtonUp);
    InitializeButtonsInterrupts();

    // Initialize the timers:
    SimpleInitializeTimerInterrupts(TIMER_ID0, Players[0].speed, (unsigned)OnTimer0);
    SimpleInitializeTimerInterrupts(TIMER_ID1, Players[1].speed, (unsigned)OnTimer1);
    SimpleInitializeTimerInterrupts(TIMER_ID2, Players[2].speed, (unsigned)OnTimer2);
    SimpleInitializeTimerInterrupts(TIMER_ID3, Players[3].speed, (unsigned)OnTimer3);
    //*/

    while (1);
}
