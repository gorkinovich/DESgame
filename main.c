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

#define MAX_TIME_COUNT 50
extern GameData game_data;

//****************************************************************************************************
// Input
//****************************************************************************************************

void OnButtonDown(unsigned value) {
    if (value & BUTTON_LEFT) {
        TurnOnLeftLed();
        ResetScores();
        InitializeGame();
    } else if (value & BUTTON_RIGHT) {
        TurnOnRightLed();
        game_data.pause = !game_data.pause;
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
    if (game_data.pause) return;
    UpdateOnKeyboard(GetKeys());
}

//----------------------------------------------------------------------------------------------------

void MainLoopWithPolling() {
    while (1) {
        if (HasReceivedDataUART1()) {
            UpdateOnReceiveUART();
        }
    }
}

//----------------------------------------------------------------------------------------------------

void MainLoop() {
    while (1);
}

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

    // Initialize the data:
    InitializeGame();

    // Initialize the input:
    SetOnKeyboardDown(OnKeyboardDown);
    InitializeKeyboardInterrupts();
    SetOnButtonDown(OnButtonDown);
    SetOnButtonUp(OnButtonUp);
    InitializeButtonsInterrupts();

    // Initialize the timers:
    SimpleInitializeTimerInterrupts(TIMER_ID0, MAX_TIME_COUNT, (unsigned)UpdateOnTimer);

    // Initialize the UART1:
    InitializeUART(BAUDS_115200);
    //ActivateInterruptsUART1((unsigned)UpdateOnReceiveUART);

    // Call the main loop:
    MainLoopWithPolling();
}
