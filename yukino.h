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

#ifndef __YUKINO_H__
#define __YUKINO_H__

#include "common_types.h"

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------------------------------------
// Macros
//------------------------------------------------------------

#define NULLPTR ((void*)0)

typedef unsigned char      BOOL;
typedef unsigned char      UINT8;
typedef unsigned short     UINT16;
typedef unsigned int       UINT32;
typedef unsigned long long UINT64;
typedef signed char        INT8;
typedef signed short       INT16;
typedef signed int         INT32;
typedef signed long long   INT64;

#define DECL_WITH_IRQ_ATTRIBUTE(func) void func () __attribute__ ((interrupt ("IRQ")))

//------------------------------------------------------------
// General
//------------------------------------------------------------

void ClearAllPendingInterrupts();
void ClearInterruptMask();
void AddInterruptMask(unsigned mask);
void SetInterruptModeToIRQ();

//------------------------------------------------------------
// Timers
//------------------------------------------------------------

#define TIMER_ID0 0
#define TIMER_ID1 1
#define TIMER_ID2 2
#define TIMER_ID3 3
#define TIMER_ID4 4
#define TIMER_ID5 5

#define TIMER_DIV2  0
#define TIMER_DIV4  1
#define TIMER_DIV8  2
#define TIMER_DIV16 3
#define TIMER_DIV32 4

void SimpleInitializeTimerInterrupts(UINT8 timer, unsigned milliseconds,
		unsigned onEventFunction);
void InitializeTimerInterrupts(UINT8 timer, UINT8 prescaler, UINT8 divider,
		unsigned count, unsigned compare, unsigned onEventFunction);

void SimpleChangeTimerInterrupts(UINT8 timer, unsigned milliseconds);
void ChangeTimerInterrupts(UINT8 timer, UINT8 prescaler, UINT8 divider, unsigned count);

void ClearTimer0PendingInterrupt();
void ClearTimer1PendingInterrupt();
void ClearTimer2PendingInterrupt();
void ClearTimer3PendingInterrupt();
void ClearTimer4PendingInterrupt();
void ClearTimer5PendingInterrupt();

//------------------------------------------------------------
// Buttons
//------------------------------------------------------------

#define BUTTON_LEFT  (0x1 << 6)
#define BUTTON_RIGHT (0x1 << 7)

typedef void (*ButtonEventHandler) (unsigned);

void SetOnButtonDown(ButtonEventHandler value);
void SetOnButtonUp(ButtonEventHandler value);

void InitializeButtonsInterrupts();
void SetButtonsAsInterrupt();
void SetButtonsAsInput();

BOOL AnyButtonDown();
BOOL IsButtonDown(unsigned mask);

//------------------------------------------------------------
// Keyboard
//------------------------------------------------------------

#define KEY_SB01 (0x01 << 15)
#define KEY_SB05 (0x01 << 14)
#define KEY_SB09 (0x01 << 13)
#define KEY_SB13 (0x01 << 12)

#define KEY_SB02 (0x01 << 11)
#define KEY_SB06 (0x01 << 10)
#define KEY_SB10 (0x01 <<  9)
#define KEY_SB14 (0x01 <<  8)

#define KEY_SB03 (0x01 <<  7)
#define KEY_SB07 (0x01 <<  6)
#define KEY_SB11 (0x01 <<  5)
#define KEY_SB15 (0x01 <<  4)

#define KEY_SB04 (0x01 <<  3)
#define KEY_SB08 (0x01 <<  2)
#define KEY_SB12 (0x01 <<  1)
#define KEY_SB16 (0x01 <<  0)

#define MAX_KEYS 16

typedef void (*KeyboardEventHandler) ();

void SetOnKeyboardDown(KeyboardEventHandler value);
void SetOnKeyboardUp(KeyboardEventHandler value);

void InitializeKeyboardInterrupts();
void SetKeyboardAsInterrupt();
void SetKeyboardAsInput();

void UpdateKeys();
unsigned GetKeys();
BOOL AnyKeyDown();
BOOL IsKeyDown(unsigned mask);

void SetKeyboardCodes(char codes[MAX_KEYS]);
char DecodeFirstKey(unsigned value);
char ReadOneKey();
char UpdateAndReadOneKey();
char ImmediateReadOneKey();

//------------------------------------------------------------
// 8-segments led
//------------------------------------------------------------

void Initialize8Led();
void Set8Led(unsigned value);

BOOL IsPoint8Led();
void SetPoint8Led();
void ClearPoint8Led();

//------------------------------------------------------------
// Leds
//------------------------------------------------------------

#define LED_LEFT  (0x01 <<  9)
#define LED_RIGHT (0x01 << 10)

unsigned GetLeds();
void SetLeds(unsigned value);
void SwitchLeds();

void TurnOnLeds();
void TurnOffLeds();
void TurnOnLeftLed();
void TurnOffLeftLed();
void TurnOnRightLed();
void TurnOffRightLed();

//------------------------------------------------------------
// LCD
//------------------------------------------------------------

#define LCD_WIDTH  320
#define LCD_HEIGHT 240

#define LCD_COLOR_WHITE     0x0
#define LCD_COLOR_LIGHTGRAY 0x5
#define LCD_COLOR_DARKGRAY  0xA
#define LCD_COLOR_BLACK     0xF

void InitializeLCD();
BOOL IsOnLCD();
void TurnOnLCD();
void TurnOffLCD();

UINT8 GetPixelLCD(UINT16 x, UINT16 y);
void PutPixelLCD(UINT16 x, UINT16 y, UINT8 color);
void FillLCD(UINT8 color);
void ClearLCD();

void DrawHorizontalLineLCD(UINT16 left, UINT16 right, UINT16 y, UINT8 color, UINT16 width);
void DrawVerticalLineLCD(UINT16 x, UINT16 up, UINT16 down, UINT8 color, UINT16 width);
void DrawBoxLineLCD(UINT16 left, UINT16 up, UINT16 right, UINT16 down, UINT8 color, UINT16 width);
void PutCharLCD(UINT16 x, UINT16 y, UINT8 color, char value);
void PutStringLCD(UINT16 x, UINT16 y, UINT8 color, const char * value);
INT32 PrintLCD(UINT16 x, UINT16 y, UINT8 color, const char * format, ...);
void PutChar2LCD(UINT16 x, UINT16 y, UINT8 color, char value);
void PutString2LCD(UINT16 x, UINT16 y, UINT8 color, const char * value);
INT32 Print2LCD(UINT16 x, UINT16 y, UINT8 color, const char * format, ...);

//------------------------------------------------------------
// Initialization
//------------------------------------------------------------

void InitializeSystem ();

#ifdef __cplusplus
}
#endif

#endif
