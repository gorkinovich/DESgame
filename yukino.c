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

#include "yukino.h"
#include "44b.h"
#include "44blib.h"
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

//****************************************************************************************************
// General
//****************************************************************************************************

#define DEFAULT_DELAY 40

//----------------------------------------------------------------------------------------------------

void ClearAllPendingInterrupts() {
    // Send the bit 1 to clean all the pending interrupts marks.
    // (Page 233, S3C44B0X RISC MICROPROCESSOR, I/O PORTS)
    // (Page 281 y 294, S3C44B0X RISC MICROPROCESSOR, INTERRUPT CONTROLLER)
    rEXTINTPND = 0x0F;
    rI_ISPC    = 0x3ffffff;
}

//----------------------------------------------------------------------------------------------------

void ClearInterruptMask() {
    // (Page 284, S3C44B0X RISC MICROPROCESSOR, INTERRUPT CONTROLLER)
    rINTMSK = 0x07FFFFFF;
}

//----------------------------------------------------------------------------------------------------

void AddInterruptMask(unsigned mask) {
    // (Page 284, S3C44B0X RISC MICROPROCESSOR, INTERRUPT CONTROLLER)
    rINTMSK &= ~(BIT_GLOBAL | mask);
}

//----------------------------------------------------------------------------------------------------

void SetInterruptModeToIRQ() {
    // Configures all the lines in IRQ mode & enable the vectorized mode.
    // (Page 280 y 283, S3C44B0X RISC MICROPROCESSOR, INTERRUPT CONTROLLER)
    rINTMOD = 0x00;
    rINTCON = 0x01;
}

//****************************************************************************************************
// Timers
//****************************************************************************************************

// (Páginas 234-253, S3C44B0X RISC MICROPROCESSOR, PWM TIMER)
// Timer input clock frequency = (MCLK / (prescaler + 1)) / divider
// Timer input clock frequency = MCLK / ((prescaler + 1) * divider)
// MCLK = 66 Mhz = 66000000 hz

#define MAX_TIMERS 6
#define MAX_PRESCALER 0xFF
#define MAX_COUNT 0xFFFF
#define TIMER_MCLK 66000000
#define TIMER_MCLK_MS 66000
//#define MAX_TIME ((MAX_COUNT * MAX_PRESCALER * 32) / TIMER_MCLK_MS)
#define MAX_TIME 8134

#define BASE_COUNT 33
#define BASE_PRESCALER 124
#define MAX_PS1DV ((MAX_PRESCALER + 1) * 32)

//----------------------------------------------------------------------------------------------------

void SimpleInitializeTimerInterrupts(UINT8 timer, unsigned milliseconds, unsigned onEventFunction) {
    if (milliseconds < MAX_TIME) {
        unsigned count = milliseconds * BASE_COUNT;
        if (count <= MAX_COUNT) {
            InitializeTimerInterrupts(timer, BASE_PRESCALER, TIMER_DIV16, count, 0, onEventFunction);
        } else {
            count = (milliseconds * TIMER_MCLK_MS) / MAX_PS1DV;
            InitializeTimerInterrupts(timer, MAX_PRESCALER, TIMER_DIV32, count, 0, onEventFunction);
        }
    } else {
        InitializeTimerInterrupts(timer, MAX_PRESCALER, TIMER_DIV32, MAX_COUNT, 0, onEventFunction);
    }
}

//----------------------------------------------------------------------------------------------------

void InitializeTimerInterrupts(UINT8 timer, UINT8 prescaler, UINT8 divider, unsigned count,
        unsigned compare, unsigned onEventFunction) {
    if (timer < MAX_TIMERS) {
        ClearAllPendingInterrupts();
        SetInterruptModeToIRQ();
        switch (timer) {
        case TIMER_ID0:
            // Configure timer 0 interruption:
            AddInterruptMask(BIT_TIMER0);
            pISR_TIMER0 = onEventFunction;
            // Configure timer 0:
            rTCFG0  = (rTCFG0 & 0xFFFFFF00) | prescaler;
            rTCFG1  = (rTCFG1 & 0xFFFFFFF0) | divider;
            rTCNTB0 = count;
            rTCMPB0 = compare;
            // Timer Control Register:
            rTCON |=  (0x01 << 1); // Activate the manual update
            rTCON &= ~(0x01 << 1); // Deactivate the manual update
            rTCON |=  (0x01 << 3); // Activate the auto-reload
            rTCON |=  (0x01 << 0); // Start the timer execution
            break;
        case TIMER_ID1:
            // Configure timer 1 interruption:
            AddInterruptMask(BIT_TIMER1);
            pISR_TIMER1 = onEventFunction;
            // Configure timer 1:
            rTCFG0  = (rTCFG0 & 0xFFFFFF00) | prescaler;
            rTCFG1  = (rTCFG1 & 0xFFFFFF0F) | (divider << 4);
            rTCNTB1 = count;
            rTCMPB1 = compare;
            // Timer Control Register:
            rTCON |=  (0x01 <<  9); // Activate the manual update
            rTCON &= ~(0x01 <<  9); // Deactivate the manual update
            rTCON |=  (0x01 << 11); // Activate the auto-reload
            rTCON |=  (0x01 <<  8); // Start the timer execution
            break;
        case TIMER_ID2:
            // Configure timer 2 interruption:
            AddInterruptMask(BIT_TIMER2);
            pISR_TIMER2 = onEventFunction;
            // Configure timer 2:
            rTCFG0  = (rTCFG0 & 0xFFFF00FF) | (prescaler << 8);
            rTCFG1  = (rTCFG1 & 0xFFFFF0FF) | (divider   << 8);
            rTCNTB2 = count;
            rTCMPB2 = compare;
            // Timer Control Register:
            rTCON |=  (0x01 << 13); // Activate the manual update
            rTCON &= ~(0x01 << 13); // Deactivate the manual update
            rTCON |=  (0x01 << 15); // Activate the auto-reload
            rTCON |=  (0x01 << 12); // Start the timer execution
            break;
        case TIMER_ID3:
            // Configure timer 3 interruption:
            AddInterruptMask(BIT_TIMER3);
            pISR_TIMER3 = onEventFunction;
            // Configure timer 3:
            rTCFG0  = (rTCFG0 & 0xFFFF00FF) | (prescaler <<  8);
            rTCFG1  = (rTCFG1 & 0xFFFF0FFF) | (divider   << 12);
            rTCNTB3 = count;
            rTCMPB3 = compare;
            // Timer Control Register:
            rTCON |=  (0x01 << 17); // Activate the manual update
            rTCON &= ~(0x01 << 17); // Deactivate the manual update
            rTCON |=  (0x01 << 19); // Activate the auto-reload
            rTCON |=  (0x01 << 16); // Start the timer execution
            break;
        case TIMER_ID4:
            // Configure timer 4 interruption:
            AddInterruptMask(BIT_TIMER4);
            pISR_TIMER4 = onEventFunction;
            // Configure timer 4:
            rTCFG0  = (rTCFG0 & 0xFF00FFFF) | (prescaler << 16);
            rTCFG1  = (rTCFG1 & 0xFFF0FFFF) | (divider   << 16);
            rTCNTB4 = count;
            rTCMPB4 = compare;
            // Timer Control Register:
            rTCON |=  (0x01 << 21); // Activate the manual update
            rTCON &= ~(0x01 << 21); // Deactivate the manual update
            rTCON |=  (0x01 << 23); // Activate the auto-reload
            rTCON |=  (0x01 << 20); // Start the timer execution
            break;
        case TIMER_ID5:
            // Configure timer 5 interruption:
            AddInterruptMask(BIT_TIMER5);
            pISR_TIMER5 = onEventFunction;
            // Configure timer 5:
            rTCFG0  = (rTCFG0 & 0xFF00FFFF) | (prescaler << 16);
            rTCFG1  = (rTCFG1 & 0xFF0FFFFF) | (divider   << 20);
            rTCNTB5 = count;
            // Timer Control Register:
            rTCON |=  (0x01 << 25); // Activate the manual update
            rTCON &= ~(0x01 << 25); // Deactivate the manual update
            rTCON |=  (0x01 << 26); // Activate the auto-reload
            rTCON |=  (0x01 << 24); // Start the timer execution
            break;
        }
        ClearAllPendingInterrupts();
    }
}

//----------------------------------------------------------------------------------------------------

void SimpleChangeTimerInterrupts(UINT8 timer, unsigned milliseconds) {
    if (milliseconds < MAX_TIME) {
        unsigned count = milliseconds * BASE_COUNT;
        if (count <= MAX_COUNT) {
            ChangeTimerInterrupts(timer, BASE_PRESCALER, TIMER_DIV16, count);
        } else {
            count = (milliseconds * TIMER_MCLK_MS) / MAX_PS1DV;
            ChangeTimerInterrupts(timer, MAX_PRESCALER, TIMER_DIV32, count);
        }
    } else {
        ChangeTimerInterrupts(timer, MAX_PRESCALER, TIMER_DIV32, MAX_COUNT);
    }
}

//----------------------------------------------------------------------------------------------------

void ChangeTimerInterrupts(UINT8 timer, UINT8 prescaler, UINT8 divider, unsigned count) {
    if (timer < MAX_TIMERS) {
        switch (timer) {
        case TIMER_ID0:
            // Configure timer 0:
            rTCFG0  = (rTCFG0 & 0xFFFFFF00) | prescaler;
            rTCFG1  = (rTCFG1 & 0xFFFFFFF0) | divider;
            rTCNTB0 = count;
            break;
        case TIMER_ID1:
            // Configure timer 1:
            rTCFG0  = (rTCFG0 & 0xFFFFFF00) | prescaler;
            rTCFG1  = (rTCFG1 & 0xFFFFFF0F) | (divider << 4);
            rTCNTB1 = count;
            break;
        case TIMER_ID2:
            // Configure timer 2:
            rTCFG0  = (rTCFG0 & 0xFFFF00FF) | (prescaler << 8);
            rTCFG1  = (rTCFG1 & 0xFFFFF0FF) | (divider   << 8);
            rTCNTB2 = count;
            break;
        case TIMER_ID3:
            // Configure timer 3:
            rTCFG0  = (rTCFG0 & 0xFFFF00FF) | (prescaler <<  8);
            rTCFG1  = (rTCFG1 & 0xFFFF0FFF) | (divider   << 12);
            rTCNTB3 = count;
            break;
        case TIMER_ID4:
            // Configure timer 4:
            rTCFG0  = (rTCFG0 & 0xFF00FFFF) | (prescaler << 16);
            rTCFG1  = (rTCFG1 & 0xFFF0FFFF) | (divider   << 16);
            rTCNTB4 = count;
            break;
        case TIMER_ID5:
            // Configure timer 5:
            rTCFG0  = (rTCFG0 & 0xFF00FFFF) | (prescaler << 16);
            rTCFG1  = (rTCFG1 & 0xFF0FFFFF) | (divider   << 20);
            rTCNTB5 = count;
            break;
        }
    }
}

//----------------------------------------------------------------------------------------------------

void ClearTimer0PendingInterrupt() { rI_ISPC = BIT_TIMER0; }
void ClearTimer1PendingInterrupt() { rI_ISPC = BIT_TIMER1; }
void ClearTimer2PendingInterrupt() { rI_ISPC = BIT_TIMER2; }
void ClearTimer3PendingInterrupt() { rI_ISPC = BIT_TIMER3; }
void ClearTimer4PendingInterrupt() { rI_ISPC = BIT_TIMER4; }
void ClearTimer5PendingInterrupt() { rI_ISPC = BIT_TIMER5; }

//****************************************************************************************************
// Buttons
//****************************************************************************************************

static ButtonEventHandler OnButtonDown_ = NULLPTR;
static ButtonEventHandler OnButtonUp_   = NULLPTR;

void SetOnButtonDown(ButtonEventHandler value) { OnButtonDown_ = value; }
void SetOnButtonUp  (ButtonEventHandler value) { OnButtonUp_ = value;   }

//----------------------------------------------------------------------------------------------------

#define EXTINT_LEFT  (0x1 << 2)
#define EXTINT_RIGHT (0x1 << 3)

#define BUTTON_DELAY 60

DECL_WITH_IRQ_ATTRIBUTE(OnEint4567);

void OnEint4567() {
    unsigned victim = rEXTINTPND;
    if (victim & EXTINT_LEFT) {
        if (OnButtonDown_) (*OnButtonDown_)(BUTTON_LEFT);
        while (IsButtonDown(BUTTON_LEFT)) DelayMs(BUTTON_DELAY);
        if (OnButtonUp_) (*OnButtonUp_)(BUTTON_LEFT);
    }
    if (victim & EXTINT_RIGHT) {
        if (OnButtonDown_) (*OnButtonDown_)(BUTTON_RIGHT);
        while (IsButtonDown(BUTTON_RIGHT)) DelayMs(BUTTON_DELAY);
        if (OnButtonUp_) (*OnButtonUp_)(BUTTON_RIGHT);
    }
    DelayMs(BUTTON_DELAY);
    rEXTINTPND = 0x0C;
    rI_ISPC = BIT_EINT4567;
}

//----------------------------------------------------------------------------------------------------

void InitializeButtonsInterrupts() {
    SetButtonsAsInterrupt();
    ClearAllPendingInterrupts();
    SetInterruptModeToIRQ();
    AddInterruptMask(BIT_EINT4567);
    pISR_EINT4567 = (unsigned)OnEint4567;
    ClearAllPendingInterrupts();
}

//----------------------------------------------------------------------------------------------------

void SetButtonsAsInterrupt() {
    // Configure the pins & enable the "pull up" in the port G.
    // (Page 230, S3C44B0X RISC MICROPROCESSOR, I/O PORTS)
    rPCONG |= 0xF000; // [PG7, PG6] = 11 -> EINT
    rPUPG  &= 0x3F;   // [PG7, PG6] =  0 -> Activate
    // Configure the invocation mode of the interruption.
    // (Page 232, S3C44B0X RISC MICROPROCESSOR, I/O PORTS)
    rEXTINT = (rEXTINT & 0x00FFFFFF) | 0x22000000;
    // [EINT7, EINT6] = 010 -> Int. low level, down flank
}

//----------------------------------------------------------------------------------------------------

void SetButtonsAsInput() {
    // Configure the pins & enable the "pull up" in the port G.
    // (Page 230, S3C44B0X RISC MICROPROCESSOR, I/O PORTS)
    rPCONG &= 0x0FFF; // [PG7, PG6] = 00 -> Input
    rPUPG  &= 0x3F;   // [PG7, PG6] =  0 -> Activate
}

//----------------------------------------------------------------------------------------------------

BOOL AnyButtonDown() {
    return (~rPDATG & (BUTTON_LEFT | BUTTON_RIGHT)) != 0;
}

//----------------------------------------------------------------------------------------------------

BOOL IsButtonDown(unsigned mask) {
    return (~rPDATG & mask) != 0;
}

//****************************************************************************************************
// Keyboard
//****************************************************************************************************

#define KEYBOARD_BASE 0x06000000
#define KEYBOARD_SB01SB13_OFFSET 0xFD
#define KEYBOARD_SB02SB14_OFFSET 0xFB
#define KEYBOARD_SB03SB15_OFFSET 0xF7
#define KEYBOARD_SB04SB16_OFFSET 0xEF
#define KEYBOARD_VALUE_MASK 0xF

#define KEYBOARD_SB01SB13 (*(volatile UINT8 *)(KEYBOARD_BASE + KEYBOARD_SB01SB13_OFFSET))
#define KEYBOARD_SB02SB14 (*(volatile UINT8 *)(KEYBOARD_BASE + KEYBOARD_SB02SB14_OFFSET))
#define KEYBOARD_SB03SB15 (*(volatile UINT8 *)(KEYBOARD_BASE + KEYBOARD_SB03SB15_OFFSET))
#define KEYBOARD_SB04SB16 (*(volatile UINT8 *)(KEYBOARD_BASE + KEYBOARD_SB04SB16_OFFSET))

static unsigned KeyboardValues = 0;
static char KeyboardCodes[MAX_KEYS] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
static unsigned KeyboardSequence[MAX_KEYS] = {
    KEY_SB01, KEY_SB05, KEY_SB09, KEY_SB13, KEY_SB02, KEY_SB06, KEY_SB10, KEY_SB14,
    KEY_SB03, KEY_SB07, KEY_SB11, KEY_SB15, KEY_SB04, KEY_SB08, KEY_SB12, KEY_SB16
};

//----------------------------------------------------------------------------------------------------

static KeyboardEventHandler OnKeyboardDown_ = NULLPTR;
static KeyboardEventHandler OnKeyboardUp_   = NULLPTR;

void SetOnKeyboardDown(KeyboardEventHandler value) { OnKeyboardDown_ = value; }
void SetOnKeyboardUp  (KeyboardEventHandler value) { OnKeyboardUp_ = value;   }

//----------------------------------------------------------------------------------------------------

#define KEYBOARD_PORT 0x02
#define KEYBOARD_DELAY1 40
#define KEYBOARD_DELAY2 60

DECL_WITH_IRQ_ATTRIBUTE(OnEint1);

void OnEint1() {
    DelayMs(KEYBOARD_DELAY1);
    UpdateKeys();
    if (AnyKeyDown()) {
        if (OnKeyboardDown_) (*OnKeyboardDown_)();
        while (~rPDATG & KEYBOARD_PORT) DelayMs(KEYBOARD_DELAY2);
        if (OnKeyboardUp_) (*OnKeyboardUp_)();
    }
    DelayMs(KEYBOARD_DELAY2);
    rI_ISPC = BIT_EINT1;
}

//----------------------------------------------------------------------------------------------------

void InitializeKeyboardInterrupts() {
    SetKeyboardAsInterrupt();
    ClearAllPendingInterrupts();
    SetInterruptModeToIRQ();
    AddInterruptMask(BIT_EINT1);
    pISR_EINT1 = (unsigned)OnEint1;
    ClearAllPendingInterrupts();
}

//----------------------------------------------------------------------------------------------------

void SetKeyboardAsInterrupt() {
    // Configure the pins & enable the "pull up" in the port G.
    // (Page 230, S3C44B0X RISC MICROPROCESSOR, I/O PORTS)
    rPCONG |= 0x000C; // [PG1] = 11 -> EINT
    rPUPG  &= 0xFD;   // [PG1] =  0 -> Activate
    // Configure the invocation mode of the interruption.
    // (Page 232, S3C44B0X RISC MICROPROCESSOR, I/O PORTS)
    rEXTINT = (rEXTINT & 0xFFFFFF0F) | 0x00000020;
    // [EINT1] = 010 -> Int. low level, down flank
}

//----------------------------------------------------------------------------------------------------

void SetKeyboardAsInput() {
    // Configure the pins & enable the "pull up" in the port G.
    // (Page 230, S3C44B0X RISC MICROPROCESSOR, I/O PORTS)
    rPCONG &= 0xFFF3; // [PG1] = 00 -> Input
    rPUPG  &= 0xFD;   // [PG1] =  0 -> Activate
}

//----------------------------------------------------------------------------------------------------

void UpdateKeys() {
    UINT8 row1 = KEYBOARD_SB01SB13, row2 = KEYBOARD_SB02SB14;
    UINT8 row3 = KEYBOARD_SB03SB15, row4 = KEYBOARD_SB04SB16;

    row1 = (~row1) & KEYBOARD_VALUE_MASK;
    row2 = (~row2) & KEYBOARD_VALUE_MASK;
    row3 = (~row3) & KEYBOARD_VALUE_MASK;
    row4 = (~row4) & KEYBOARD_VALUE_MASK;

    KeyboardValues = (row1 << 12) | (row2 << 8) | (row3 << 4) | (row4);
}

//----------------------------------------------------------------------------------------------------

unsigned GetKeys() {
    return KeyboardValues;
}

//----------------------------------------------------------------------------------------------------

BOOL AnyKeyDown() {
    return KeyboardValues != 0;
}

//----------------------------------------------------------------------------------------------------

BOOL IsKeyDown(unsigned mask) {
    return (KeyboardValues & mask) != 0;
}

//----------------------------------------------------------------------------------------------------

void SetKeyboardCodes(char codes[MAX_KEYS]) {
    int i = 0;
    for (; i < MAX_KEYS; ++i) {
        KeyboardCodes[i] = codes[i];
    }
}

//----------------------------------------------------------------------------------------------------

char DecodeFirstKey(unsigned value) {
    int i = 0;
    for (; i < MAX_KEYS; ++i) {
        if (value & KeyboardSequence[i]) {
            return KeyboardCodes[i];
        }
    }
    return -1;
}

//----------------------------------------------------------------------------------------------------

char ReadOneKey() {
    return DecodeFirstKey(KeyboardValues);
}

//----------------------------------------------------------------------------------------------------

char UpdateAndReadOneKey() {
    UpdateKeys();
    return DecodeFirstKey(KeyboardValues);
}

//----------------------------------------------------------------------------------------------------

char ImmediateReadOneKey() {
    UINT8 temp = KEYBOARD_SB01SB13 & KEYBOARD_VALUE_MASK;
    switch (temp) {
    case 0x7: return KeyboardCodes[0]; // SB01
    case 0xB: return KeyboardCodes[1]; // SB05
    case 0xD: return KeyboardCodes[2]; // SB09
    case 0xE: return KeyboardCodes[3]; // SB13
    }

    temp = KEYBOARD_SB02SB14 & KEYBOARD_VALUE_MASK;
    switch (temp) {
    case 0x7: return KeyboardCodes[4]; // SB02
    case 0xB: return KeyboardCodes[5]; // SB06
    case 0xD: return KeyboardCodes[6]; // SB10
    case 0xE: return KeyboardCodes[7]; // SB14
    }

    temp = KEYBOARD_SB03SB15 & KEYBOARD_VALUE_MASK;
    switch (temp) {
    case 0x7: return KeyboardCodes[8];  // SB03
    case 0xB: return KeyboardCodes[9];  // SB07
    case 0xD: return KeyboardCodes[10]; // SB11
    case 0xE: return KeyboardCodes[11]; // SB15
    }

    temp = KEYBOARD_SB04SB16 & KEYBOARD_VALUE_MASK;
    switch (temp) {
    case 0x7: return KeyboardCodes[12]; // SB04
    case 0xB: return KeyboardCodes[13]; // SB08
    case 0xD: return KeyboardCodes[14]; // SB12
    case 0xE: return KeyboardCodes[15]; // SB16
    }

    return -1;
}

//****************************************************************************************************
// 8-segments led
//****************************************************************************************************

#define SEGMENT_A 0x7F
#define SEGMENT_B 0xBF
#define SEGMENT_C 0xDF
#define SEGMENT_D 0xF7
#define SEGMENT_E 0xFB
#define SEGMENT_F 0xFD
#define SEGMENT_G 0xFE
#define SEGMENT_P 0xEF

#define SYMBOL_0 (SEGMENT_A & SEGMENT_B & SEGMENT_C & SEGMENT_D & SEGMENT_E & SEGMENT_G)
#define SYMBOL_1 (SEGMENT_B & SEGMENT_C)
#define SYMBOL_2 (SEGMENT_A & SEGMENT_B & SEGMENT_F & SEGMENT_E & SEGMENT_D)
#define SYMBOL_3 (SEGMENT_A & SEGMENT_B & SEGMENT_F & SEGMENT_C & SEGMENT_D)
#define SYMBOL_4 (SEGMENT_G & SEGMENT_F & SEGMENT_B & SEGMENT_C)
#define SYMBOL_5 (SEGMENT_A & SEGMENT_G & SEGMENT_F & SEGMENT_C & SEGMENT_D)
#define SYMBOL_6 (SEGMENT_A & SEGMENT_G & SEGMENT_E & SEGMENT_D & SEGMENT_C & SEGMENT_F)
#define SYMBOL_7 (SEGMENT_A & SEGMENT_B & SEGMENT_C)
#define SYMBOL_8 (SEGMENT_G & SEGMENT_A & SEGMENT_B & SEGMENT_F & SEGMENT_E & SEGMENT_D & SEGMENT_C)
#define SYMBOL_9 (SEGMENT_F & SEGMENT_G & SEGMENT_A & SEGMENT_B & SEGMENT_C & SEGMENT_D)
#define SYMBOL_A (SEGMENT_E & SEGMENT_G & SEGMENT_A & SEGMENT_B & SEGMENT_C & SEGMENT_F)
#define SYMBOL_B (SEGMENT_G & SEGMENT_E & SEGMENT_D & SEGMENT_C & SEGMENT_F)
#define SYMBOL_C (SEGMENT_A & SEGMENT_G & SEGMENT_E & SEGMENT_D)
#define SYMBOL_D (SEGMENT_F & SEGMENT_E & SEGMENT_D & SEGMENT_C & SEGMENT_B)
#define SYMBOL_E (SEGMENT_A & SEGMENT_G & SEGMENT_E & SEGMENT_D & SEGMENT_F)
#define SYMBOL_F (SEGMENT_A & SEGMENT_G & SEGMENT_E & SEGMENT_F)

static unsigned char NumberToSymbol_[] = {
    SYMBOL_0, SYMBOL_1, SYMBOL_2, SYMBOL_3, SYMBOL_4, SYMBOL_5, SYMBOL_6, SYMBOL_7,
    SYMBOL_8, SYMBOL_9, SYMBOL_A, SYMBOL_B, SYMBOL_C, SYMBOL_D, SYMBOL_E, SYMBOL_F
};

static unsigned char Led8Data = 0;

//----------------------------------------------------------------------------------------------------

void Initialize8Led() {
    Led8Data = 0xFF;
    LED8ADDR = Led8Data;
}

//----------------------------------------------------------------------------------------------------

void Set8Led(unsigned value) {
    if (IsPoint8Led()) {
        Led8Data = NumberToSymbol_[value & 0x0F] & SEGMENT_P;
    } else {
        Led8Data = NumberToSymbol_[value & 0x0F];
    }
    LED8ADDR = Led8Data;
}

//----------------------------------------------------------------------------------------------------

BOOL IsPoint8Led() {
    return (~(Led8Data | SEGMENT_P) & 0xFF) != 0;
}

//----------------------------------------------------------------------------------------------------

void SetPoint8Led() {
    Led8Data &= SEGMENT_P;
    LED8ADDR = Led8Data;
}

//----------------------------------------------------------------------------------------------------

void ClearPoint8Led() {
    Led8Data |= ~SEGMENT_P;
    LED8ADDR = Led8Data;
}

//****************************************************************************************************
// Leds
//****************************************************************************************************

#define GET_LED(led) (~rPDATB & (led))
#define SET_LED(led, value) (rPDATB = ((value) & (led) ? rPDATB & ~(led) : rPDATB | (led)))

//----------------------------------------------------------------------------------------------------

unsigned GetLeds() {
    return GET_LED(LED_LEFT | LED_RIGHT);
}

//----------------------------------------------------------------------------------------------------

void SetLeds(unsigned value) {
    SET_LED(LED_LEFT, value);
    SET_LED(LED_RIGHT, value);
}

//----------------------------------------------------------------------------------------------------

void SwitchLeds() {
    SetLeds(~rPDATB ^ (LED_LEFT | LED_RIGHT));
}

//----------------------------------------------------------------------------------------------------

void TurnOnLeds() {
    SetLeds(LED_LEFT | LED_RIGHT);
}

//----------------------------------------------------------------------------------------------------

void TurnOffLeds() {
    SetLeds(0);
}

//----------------------------------------------------------------------------------------------------

void TurnOnLeftLed() {
    SET_LED(LED_LEFT, LED_LEFT);
}

//----------------------------------------------------------------------------------------------------

void TurnOffLeftLed() {
    SET_LED(LED_LEFT, 0);
}

//----------------------------------------------------------------------------------------------------

void TurnOnRightLed() {
    SET_LED(LED_RIGHT, LED_RIGHT);
}

//----------------------------------------------------------------------------------------------------

void TurnOffRightLed() {
    SET_LED(LED_RIGHT, 0);
}

//****************************************************************************************************
// LCD
//****************************************************************************************************

extern const uint8 font[256*16];

#define LCD_BUFFER_FACTOR 8
#define LCD_BUFFER_WLINE  (LCD_WIDTH / LCD_BUFFER_FACTOR)
#define LCD_BUFFER_SIZE   (LCD_WIDTH * LCD_HEIGHT / LCD_BUFFER_FACTOR)

static UINT32 ScreenBufferLCD[LCD_BUFFER_SIZE] __attribute__ ((section (".lcdbuffer")));

//#define LCD_BUFFER_WLINE (LCD_WIDTH / 2)
//#define LCD_BUFFER_SIZE  (LCD_WIDTH * LCD_HEIGHT / 2)
//static UINT8 ScreenBufferLCD[LCD_BUFFER_SIZE] __attribute__ ((section (".lcdbuffer")));

// Add in the link file (*.ld):
// . = 0x0C300000;
// .lcdbuffer : { *(.lcdbuffer) }

//----------------------------------------------------------------------------------------------------

inline UINT32 PixelToIndexLCD(UINT32 x, UINT32 y) {
    return LCD_BUFFER_WLINE * y + x / LCD_BUFFER_FACTOR;
}

inline UINT16 PixelToOffsetLCD(UINT16 x) {
    return (((LCD_BUFFER_FACTOR - 1) - (x % LCD_BUFFER_FACTOR)) * 4);
}

//----------------------------------------------------------------------------------------------------

void InitializeLCD() {
    // Configure the LCD controller & turn it off:
    // + Screen resolution: 320x240
    // + Scan mode: 4 bit single scan
    // + LCD mode: 16 levels of gray
    // + Color palettes: no
    // + Refresh: 120 Hz
    // + Self-refresh: disabled
    // + Sync polarity signal: normal
    // + VM commutation: each frame
    // + The horizontal blanking width: minimum
    // + Delay and pulse width horizontal sync: minimum
    // + Dithering values: by default
    rLCDCON1   = 0x0000D021;
    rLCDCON2   = 0x00013CEF;
    rLCDSADDR1 = 0x16180000;
    rLCDSADDR2 = 0x00184B00;
    rLCDSADDR3 = 0x00050;
    rREDLUT    = 0x0;
    rBLUELUT   = 0x0;
    rGREENLUT  = 0x0;
    rDITHMODE  = 0x12210;
    rDP1_2     = 0xA5A5;
    rDP4_7     = 0xBA5DA65;
    rDP3_5     = 0xA5A5F;
    rDP2_3     = 0xD6B;
    rDP5_7     = 0xEB7B5ED;
    rDP3_4     = 0x77DBE;
    rDP4_5     = 0x7EBDF;
    rDP6_7     = 0x7FDFBFE;
}

//----------------------------------------------------------------------------------------------------

BOOL IsOnLCD() {
    unsigned int value = rLCDCON1;
    return (value & 0x00000001) != 0;
}

//----------------------------------------------------------------------------------------------------

void TurnOnLCD() {
    rLCDCON1 |= 0x00000001;
}

//----------------------------------------------------------------------------------------------------

void TurnOffLCD() {
    rLCDCON1 &= ~(0x00000001);
}

//----------------------------------------------------------------------------------------------------

UINT8 GetPixelLCD(UINT16 x, UINT16 y) {
    if (x < LCD_WIDTH && y < LCD_HEIGHT) {
        UINT32 aux = ScreenBufferLCD[PixelToIndexLCD(x, y)];
        return (aux >> PixelToOffsetLCD(x)) & 0x0F;
    } else {
        return 0xFF;
    }
}

//----------------------------------------------------------------------------------------------------

void PutPixelLCD(UINT16 x, UINT16 y, UINT8 color) {
    if (x < LCD_WIDTH && y < LCD_HEIGHT) {
        UINT32 index = PixelToIndexLCD(x, y);
        UINT32 aux = ScreenBufferLCD[index];
        UINT16 offset = PixelToOffsetLCD(x);
        ScreenBufferLCD[index] = (aux & ~(0x0F << offset)) | ((color & 0x0F) << offset);
    }
}

//----------------------------------------------------------------------------------------------------

void FillLCD(UINT8 color) {
    UINT16 x, y;
    for(y = 0; y < LCD_HEIGHT; ++y) {
        for(x = 0; x < LCD_WIDTH; ++x) {
            PutPixelLCD(x, y, color);
        }
    }
}

//----------------------------------------------------------------------------------------------------

void ClearLCD() {
    FillLCD(LCD_COLOR_WHITE);
}

//----------------------------------------------------------------------------------------------------

void DrawHorizontalLineLCD(UINT16 left, UINT16 right, UINT16 y, UINT8 color, UINT16 width) {
    UINT16 row, col, end = y + width;
    for(row = y; row < end; ++row) {
        for(col = left; col <= right; ++col) {
            PutPixelLCD(col, row, color);
        }
    }
}

//----------------------------------------------------------------------------------------------------

void DrawVerticalLineLCD(UINT16 x, UINT16 up, UINT16 down, UINT8 color, UINT16 width) {
    UINT16 row, col, end = x + width;
    for(row = up; row <= down; ++row) {
        for(col = x; col < end; ++col) {
            PutPixelLCD(col, row, color);
        }
    }
}

//----------------------------------------------------------------------------------------------------

void DrawBoxLineLCD(UINT16 left, UINT16 up, UINT16 right, UINT16 down, UINT8 color, UINT16 width) {
    DrawHorizontalLineLCD(left, right, up, color, width);
    DrawVerticalLineLCD(left, up, down, color, width);
    DrawVerticalLineLCD(right, up, down, color, width);
    DrawHorizontalLineLCD(left, right, down, color, width);
}

//----------------------------------------------------------------------------------------------------

void PutCharLCD(UINT16 x, UINT16 y, UINT8 color, char value) {
    UINT16 row, col;
    for (row = 0; row < FONT_HEIGHT; ++row) {
        UINT8 mask = font[value * FONT_HEIGHT + row];
        for (col = 0; col < FONT_WIDTH; ++col) {
            if (mask & (0x80 >> col)) {
                PutPixelLCD(x + col, y + row, color);
            } else {
                PutPixelLCD(x + col, y + row, LCD_COLOR_WHITE);
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------

void PutStringLCD(UINT16 x, UINT16 y, UINT8 color, const char * value) {
    char c;
    UINT16 i = 0, dx = x, dy = y;
    while (c = value[i]) {
        if (c == '\n') {
            dx = x, dy += FONT_HEIGHT;
        } else if (c == ' ') {
            dx += FONT_WIDTH;
        } else {
            PutCharLCD(dx, dy, color, c);
            dx += FONT_WIDTH;
        }
        ++i;
    }
}

//----------------------------------------------------------------------------------------------------

INT32 PrintLCD(UINT16 x, UINT16 y, UINT8 color, const char * format, ...) {
    va_list arg_params;
    char buffer[512];
    va_start(arg_params, format);
    INT32 result = vsprintf(buffer, format, arg_params);
    va_end(arg_params);
    PutStringLCD(x, y, color, buffer);
    return result;
}

//----------------------------------------------------------------------------------------------------

void PutChar2LCD(UINT16 x, UINT16 y, UINT8 color, char value) {
    UINT16 row, col;
    for (row = 0; row < FONT_HEIGHT; ++row) {
        UINT8 mask = font[value * FONT_HEIGHT + row];
        for (col = 0; col < FONT_WIDTH; ++col) {
            if (mask & (0x80 >> col)) {
                PutPixelLCD(x + col * 2, y + row * 2, color);
                PutPixelLCD(x + col * 2 + 1, y + row * 2, color);
                PutPixelLCD(x + col * 2, y + row * 2 + 1, color);
                PutPixelLCD(x + col * 2 + 1, y + row * 2 + 1, color);
            } else {
                PutPixelLCD(x + col * 2, y + row * 2, LCD_COLOR_WHITE);
                PutPixelLCD(x + col * 2 + 1, y + row * 2, LCD_COLOR_WHITE);
                PutPixelLCD(x + col * 2, y + row * 2 + 1, LCD_COLOR_WHITE);
                PutPixelLCD(x + col * 2 + 1, y + row * 2 + 1, LCD_COLOR_WHITE);
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------

void PutString2LCD(UINT16 x, UINT16 y, UINT8 color, const char * value) {
    char c;
    UINT16 i = 0, dx = x, dy = y;
    while (c = value[i]) {
        if (c == '\n') {
            dx = x, dy += (FONT_HEIGHT * 2);
        } else if (c == ' ') {
            dx += (FONT_WIDTH * 2);
        } else {
            PutChar2LCD(dx, dy, color, c);
            dx += (FONT_WIDTH * 2);
        }
        ++i;
    }
}

//----------------------------------------------------------------------------------------------------

INT32 Print2LCD(UINT16 x, UINT16 y, UINT8 color, const char * format, ...) {
    va_list arg_params;
    char buffer[512];
    va_start(arg_params, format);
    INT32 result = vsprintf(buffer, format, arg_params);
    va_end(arg_params);
    PutString2LCD(x, y, color, buffer);
    return result;
}

//****************************************************************************************************
// UART
//****************************************************************************************************

#define UART_BUFFER_SIZE 256

static BOOL UsePollingUART0 = TRUE;
static BOOL UsePollingUART1 = TRUE;
static UINT8 BufferUART0[UART_BUFFER_SIZE];
static UINT8 BufferUART1[UART_BUFFER_SIZE];
static volatile UINT32 ReadPtrUART0 = 0;
static volatile UINT32 WritePtrUART0 = 0;
static volatile UINT32 ReadPtrUART1 = 0;
static volatile UINT32 WritePtrUART1 = 0;

DECL_WITH_IRQ_ATTRIBUTE(OnRxUART0);
DECL_WITH_IRQ_ATTRIBUTE(OnRxUART1);

#define MOVE_POINTER(ptr) ptr = (ptr + 1) % UART_BUFFER_SIZE

//----------------------------------------------------------------------------------------------------

void OnRxUART0() {
	UINT32 nextPtr = WritePtrUART0;
	MOVE_POINTER(nextPtr);
	if (nextPtr != ReadPtrUART0) {
		WritePtrUART0 = nextPtr;
		while (!(rUTRSTAT0 & 0x1));
		BufferUART0[WritePtrUART0] = RdURXH0();
	}
    rI_ISPC = BIT_URXD0;
}

//----------------------------------------------------------------------------------------------------

void OnRxUART1() {
	UINT32 nextPtr = WritePtrUART1;
	MOVE_POINTER(nextPtr);
	if (nextPtr != ReadPtrUART1) {
		WritePtrUART1 = nextPtr;
		while (!(rUTRSTAT1 & 0x1));
		BufferUART1[WritePtrUART1] = RdURXH1();
	}
    rI_ISPC = BIT_URXD1;
}

//----------------------------------------------------------------------------------------------------

void InitializeUART(int bauds) {
	InitializeUART0(bauds);
	InitializeUART1(bauds);
}

//----------------------------------------------------------------------------------------------------

void ActivateInterruptsUART() {
	ActivateInterruptsUART0();
	ActivateInterruptsUART1();
}

//----------------------------------------------------------------------------------------------------

void InitializeUART0(int bauds) {
    // Configuration:
    // ULCON: Normal mode operation, No parity, One stop bit per frame, 8-bits length
    // UCON:  Tx:Level, Rx:Pulse, Rx time out disable, Rx error status interrupt disable,
    //        Normal operation, Normal transmit, Tx/Rx interrupt request or polling mode
    // UFCON: FIFO disable
    // UMCON: ACF disable, Inactivate nRTS
    rULCON0 = 0x03;
    rUCON0  = 0x205;
    rUFCON0 = 0x00;
    rUMCON0 = 0x00;
    // Baud rate divisior register:
    rUBRDIV0 = ((int) (MCLK / 16.0 / bauds + 0.5) - 1);
}

//----------------------------------------------------------------------------------------------------

void ActivateInterruptsUART0() {
    ClearAllPendingInterrupts();
    SetInterruptModeToIRQ();
    AddInterruptMask(BIT_URXD0);
    pISR_URXD0 = (unsigned)OnRxUART0;
    ClearAllPendingInterrupts();

	ReadPtrUART0 = 0;
	WritePtrUART0 = 0;
	UsePollingUART0 = FALSE;
}

//----------------------------------------------------------------------------------------------------

void WaitTxEmptyUART0() {
	while (!(rUTRSTAT0 & 0x4));
}

//----------------------------------------------------------------------------------------------------

char GetCharUART0() {
	if (UsePollingUART0) {
		// The buffer register has a received data:
		while (!(rUTRSTAT0 & 0x1));
		// Return the last received byte:
		return (char)RdURXH0();
	} else {
		while (ReadPtrUART0 == WritePtrUART0);
		char data = (char)BufferUART0[ReadPtrUART0];
		MOVE_POINTER(ReadPtrUART0);
		return data;
	}
}

//----------------------------------------------------------------------------------------------------

void SendCharUART0(char victim) {
	// Wait transmit buffer empty:
	while (!(rUTRSTAT0 & 0x2));
	// Send the carriage return when new line:
    if (victim == LF_CHAR) {
	   WrUTXH0(CR_CHAR);
	}
    // Send the data:
	WrUTXH0(victim);
}

//----------------------------------------------------------------------------------------------------

void SendStringUART0(char * victim) {
	while (*victim) {
		SendCharUART0(*victim++);
	}
}

//----------------------------------------------------------------------------------------------------

void SendPrintfUART0(char * format, ...) {
    va_list arg_params;
    char buffer[512];
    va_start(arg_params, format);
    vsprintf(buffer, format, arg_params);
    SendStringUART0(buffer);
    va_end(arg_params);
}

//----------------------------------------------------------------------------------------------------

void GetBufferUART0(unsigned char * buffer, unsigned int size) {
	unsigned int i = 0;
	while (i < size) {
		buffer[i++] = (unsigned char)GetCharUART0();
	}
}

//----------------------------------------------------------------------------------------------------

void SendBufferUART0(unsigned char * buffer, unsigned int size) {
	unsigned int i = 0;
	while (i < size) {
		SendByteUART0(buffer[i++]);
	}
}

//----------------------------------------------------------------------------------------------------

void SendByteUART0(unsigned char byte) {
	// Wait transmit buffer empty:
	while (!(rUTRSTAT0 & 0x2));
    // Send the data:
	WrUTXH0(byte);
}

//----------------------------------------------------------------------------------------------------

void InitializeUART1(int bauds) {
    // Configuration:
    // ULCON: Normal mode operation, No parity, One stop bit per frame, 8-bits length
    // UCON:  Tx:Level, Rx:Pulse, Rx time out disable, Rx error status interrupt disable,
    //        Normal operation, Normal transmit, Tx/Rx interrupt request or polling mode
    // UFCON: FIFO disable
    // UMCON: ACF disable, Inactivate nRTS
    rULCON1 = 0x03;
    rUCON1  = 0x205;
    rUFCON1 = 0x00;
    rUMCON1 = 0x00;
    // Baud rate divisior register:
    rUBRDIV1 = ((int) (MCLK / 16.0 / bauds + 0.5) - 1);
}

//----------------------------------------------------------------------------------------------------

void ActivateInterruptsUART1() {
    ClearAllPendingInterrupts();
    SetInterruptModeToIRQ();
    AddInterruptMask(BIT_URXD1);
    pISR_URXD1 = (unsigned)OnRxUART1;
    ClearAllPendingInterrupts();

	ReadPtrUART1 = 0;
	WritePtrUART1 = 0;
	UsePollingUART1 = FALSE;
}

//----------------------------------------------------------------------------------------------------

void WaitTxEmptyUART1() {
	while (!(rUTRSTAT1 & 0x4));
}

//----------------------------------------------------------------------------------------------------

char GetCharUART1() {
	if (UsePollingUART1) {
		// The buffer register has a received data:
		while (!(rUTRSTAT1 & 0x1));
		// Return the last received byte:
		return (char)RdURXH1();
	} else {
		while (ReadPtrUART1 == WritePtrUART1);
		char data = (char)BufferUART1[ReadPtrUART1];
		MOVE_POINTER(ReadPtrUART1);
		return data;
	}
}

//----------------------------------------------------------------------------------------------------

void SendCharUART1(char victim) {
	// Wait transmit buffer empty:
	while (!(rUTRSTAT1 & 0x2));
	// Send the carriage return when new line:
    if (victim == LF_CHAR) {
	   WrUTXH1(CR_CHAR);
	}
    // Send the data:
	WrUTXH1(victim);
}

//----------------------------------------------------------------------------------------------------

void SendStringUART1(char * victim) {
	while (*victim) {
		SendCharUART1(*victim++);
	}
}

//----------------------------------------------------------------------------------------------------

void SendPrintfUART1(char * format, ...) {
    va_list arg_params;
    char buffer[512];
    va_start(arg_params, format);
    vsprintf(buffer, format, arg_params);
    SendStringUART1(buffer);
    va_end(arg_params);
}

//----------------------------------------------------------------------------------------------------

void GetBufferUART1(unsigned char * buffer, unsigned int size) {
	unsigned int i = 0;
	while (i < size) {
		buffer[i++] = (unsigned char)GetCharUART1();
	}
}

//----------------------------------------------------------------------------------------------------

void SendBufferUART1(unsigned char * buffer, unsigned int size) {
	unsigned int i = 0;
	while (i < size) {
		SendByteUART1(buffer[i++]);
	}
}

//----------------------------------------------------------------------------------------------------

void SendByteUART1(unsigned char byte) {
	// Wait transmit buffer empty:
	while (!(rUTRSTAT1 & 0x2));
    // Send the data:
	WrUTXH1(byte);
}

//****************************************************************************************************
// Initialization
//****************************************************************************************************

void InitializeSystem() {
    ClearInterruptMask();
    OnButtonDown_ = NULLPTR;
    OnButtonUp_ = NULLPTR;
    KeyboardValues = 0;
    OnKeyboardDown_ = NULLPTR;
    OnKeyboardUp_ = NULLPTR;
    Led8Data = 0;

    InitializeLCD();
    ClearLCD();
    TurnOffLeds();
    Initialize8Led();
}
