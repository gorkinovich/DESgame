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

//****************************************************************************************************
// General
//****************************************************************************************************

#define DEFAULT_DELAY 40

//----------------------------------------------------------------------------------------------------

void ClearAllPendingInterrupts() {
	// Enviamos el bit 1 para borrar todas las marcas de interrupciones pendientes.
	// (Página 233, S3C44B0X RISC MICROPROCESSOR, I/O PORTS)
	// (Página 281 y 294, S3C44B0X RISC MICROPROCESSOR, INTERRUPT CONTROLLER)
	rEXTINTPND = 0x0F;
	rI_ISPC    = 0x3ffffff;
}

//----------------------------------------------------------------------------------------------------

void ClearInterruptMask() {
	// (Página 284, S3C44B0X RISC MICROPROCESSOR, INTERRUPT CONTROLLER)
	rINTMSK = 0x07FFFFFF;
}

//----------------------------------------------------------------------------------------------------

void AddInterruptMask(unsigned mask) {
	// (Página 284, S3C44B0X RISC MICROPROCESSOR, INTERRUPT CONTROLLER)
	rINTMSK &= ~(BIT_GLOBAL | mask);
}

//----------------------------------------------------------------------------------------------------

void SetInterruptModeToIRQ() {
	// Configura todas las líneas al modo IRQ y habilita el modo vectorizado.
	// (Página 280 y 283, S3C44B0X RISC MICROPROCESSOR, INTERRUPT CONTROLLER)
	rINTMOD = 0x00;
	rINTCON = 0x01;
}

//****************************************************************************************************
// Relojes
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
			// Configurar la interrupción para el timer 0:
			AddInterruptMask(BIT_TIMER0);
			pISR_TIMER0 = onEventFunction;
			// Configurar el timer 0:
			rTCFG0  = (rTCFG0 & 0xFFFFFF00) | prescaler;
			rTCFG1  = (rTCFG1 & 0xFFFFFFF0) | divider;
			rTCNTB0 = count;
			rTCMPB0 = compare;
			// Timer Control Register:
			rTCON |=  (0x01 << 1); // Activar la actualización manual
			rTCON &= ~(0x01 << 1); // Desactivar la actualización manual
			rTCON |=  (0x01 << 3); // Activar la auto-recarga
			rTCON |=  (0x01 << 0); // Iniciar la ejecución del timer
			break;
		case TIMER_ID1:
			// Configurar la interrupción para el timer 1:
			AddInterruptMask(BIT_TIMER1);
			pISR_TIMER1 = onEventFunction;
			// Configurar el timer 1:
			rTCFG0  = (rTCFG0 & 0xFFFFFF00) | prescaler;
			rTCFG1  = (rTCFG1 & 0xFFFFFF0F) | (divider << 4);
			rTCNTB1 = count;
			rTCMPB1 = compare;
			// Timer Control Register:
			rTCON |=  (0x01 <<  9); // Activar la actualización manual
			rTCON &= ~(0x01 <<  9); // Desactivar la actualización manual
			rTCON |=  (0x01 << 11); // Activar la auto-recarga
			rTCON |=  (0x01 <<  8); // Iniciar la ejecución del timer
			break;
		case TIMER_ID2:
			// Configurar la interrupción para el timer 2:
			AddInterruptMask(BIT_TIMER2);
			pISR_TIMER2 = onEventFunction;
			// Configurar el timer 2:
			rTCFG0  = (rTCFG0 & 0xFFFF00FF) | (prescaler << 8);
			rTCFG1  = (rTCFG1 & 0xFFFFF0FF) | (divider   << 8);
			rTCNTB2 = count;
			rTCMPB2 = compare;
			// Timer Control Register:
			rTCON |=  (0x01 << 13); // Activar la actualización manual
			rTCON &= ~(0x01 << 13); // Desactivar la actualización manual
			rTCON |=  (0x01 << 15); // Activar la auto-recarga
			rTCON |=  (0x01 << 12); // Iniciar la ejecución del timer
			break;
		case TIMER_ID3:
			// Configurar la interrupción para el timer 3:
			AddInterruptMask(BIT_TIMER3);
			pISR_TIMER3 = onEventFunction;
			// Configurar el timer 3:
			rTCFG0  = (rTCFG0 & 0xFFFF00FF) | (prescaler <<  8);
			rTCFG1  = (rTCFG1 & 0xFFFF0FFF) | (divider   << 12);
			rTCNTB3 = count;
			rTCMPB3 = compare;
			// Timer Control Register:
			rTCON |=  (0x01 << 17); // Activar la actualización manual
			rTCON &= ~(0x01 << 17); // Desactivar la actualización manual
			rTCON |=  (0x01 << 19); // Activar la auto-recarga
			rTCON |=  (0x01 << 16); // Iniciar la ejecución del timer
			break;
		case TIMER_ID4:
			// Configurar la interrupción para el timer 4:
			AddInterruptMask(BIT_TIMER4);
			pISR_TIMER4 = onEventFunction;
			// Configurar el timer 4:
			rTCFG0  = (rTCFG0 & 0xFF00FFFF) | (prescaler << 16);
			rTCFG1  = (rTCFG1 & 0xFFF0FFFF) | (divider   << 16);
			rTCNTB4 = count;
			rTCMPB4 = compare;
			// Timer Control Register:
			rTCON |=  (0x01 << 21); // Activar la actualización manual
			rTCON &= ~(0x01 << 21); // Desactivar la actualización manual
			rTCON |=  (0x01 << 23); // Activar la auto-recarga
			rTCON |=  (0x01 << 20); // Iniciar la ejecución del timer
			break;
		case TIMER_ID5:
			// Configurar la interrupción para el timer 5:
			AddInterruptMask(BIT_TIMER5);
			pISR_TIMER5 = onEventFunction;
			// Configurar el timer 5:
			rTCFG0  = (rTCFG0 & 0xFF00FFFF) | (prescaler << 16);
			rTCFG1  = (rTCFG1 & 0xFF0FFFFF) | (divider   << 20);
			rTCNTB5 = count;
			// Timer Control Register:
			rTCON |=  (0x01 << 25); // Activar la actualización manual
			rTCON &= ~(0x01 << 25); // Desactivar la actualización manual
			rTCON |=  (0x01 << 26); // Activar la auto-recarga
			rTCON |=  (0x01 << 24); // Iniciar la ejecución del timer
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
			// Configurar el timer 0:
			rTCFG0  = (rTCFG0 & 0xFFFFFF00) | prescaler;
			rTCFG1  = (rTCFG1 & 0xFFFFFFF0) | divider;
			rTCNTB0 = count;
			break;
		case TIMER_ID1:
			// Configurar el timer 1:
			rTCFG0  = (rTCFG0 & 0xFFFFFF00) | prescaler;
			rTCFG1  = (rTCFG1 & 0xFFFFFF0F) | (divider << 4);
			rTCNTB1 = count;
			break;
		case TIMER_ID2:
			// Configurar el timer 2:
			rTCFG0  = (rTCFG0 & 0xFFFF00FF) | (prescaler << 8);
			rTCFG1  = (rTCFG1 & 0xFFFFF0FF) | (divider   << 8);
			rTCNTB2 = count;
			break;
		case TIMER_ID3:
			// Configurar el timer 3:
			rTCFG0  = (rTCFG0 & 0xFFFF00FF) | (prescaler <<  8);
			rTCFG1  = (rTCFG1 & 0xFFFF0FFF) | (divider   << 12);
			rTCNTB3 = count;
			break;
		case TIMER_ID4:
			// Configurar el timer 4:
			rTCFG0  = (rTCFG0 & 0xFF00FFFF) | (prescaler << 16);
			rTCFG1  = (rTCFG1 & 0xFFF0FFFF) | (divider   << 16);
			rTCNTB4 = count;
			break;
		case TIMER_ID5:
			// Configurar el timer 5:
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
// Botones
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
	// Configurar los pines y habilitar el "pull up" en el puerto G.
	// (Página 230, S3C44B0X RISC MICROPROCESSOR, I/O PORTS)
	rPCONG |= 0xF000; // [PG7, PG6] = 11 -> EINT
	rPUPG  &= 0x3F;   // [PG7, PG6] =  0 -> Activar
	// Configurar el modo de invocación de la interrupción.
	// (Página 232, S3C44B0X RISC MICROPROCESSOR, I/O PORTS)
	rEXTINT = (rEXTINT & 0x00FFFFFF) | 0x22000000;
	// [EINT7, EINT6] = 010 -> Int. bajo nivel, flanco de bajada
}

//----------------------------------------------------------------------------------------------------

void SetButtonsAsInput() {
	// Configurar los pines y habilitar el "pull up" en el puerto G.
	// (Página 230, S3C44B0X RISC MICROPROCESSOR, I/O PORTS)
	rPCONG &= 0x0FFF; // [PG7, PG6] = 00 -> Entrada
	rPUPG  &= 0x3F;   // [PG7, PG6] =  0 -> Activar
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
// Teclado
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
	// Configurar los pines y habilitar el "pull up" en el puerto G.
	// (Página 230, S3C44B0X RISC MICROPROCESSOR, I/O PORTS)
	rPCONG |= 0x000C; // [PG1] = 11 -> EINT
	rPUPG  &= 0xFD;   // [PG1] =  0 -> Activar
	// Configurar el modo de invocación de la interrupción.
	// (Página 232, S3C44B0X RISC MICROPROCESSOR, I/O PORTS)
	rEXTINT = (rEXTINT & 0xFFFFFF0F) | 0x00000020;
	// [EINT1] = 010 -> Int. bajo nivel, flanco de bajada
}

//----------------------------------------------------------------------------------------------------

void SetKeyboardAsInput() {
	// Configurar los pines y habilitar el "pull up" en el puerto G.
	// (Página 230, S3C44B0X RISC MICROPROCESSOR, I/O PORTS)
	rPCONG &= 0xFFF3; // [PG1] = 00 -> Entrada
	rPUPG  &= 0xFD;   // [PG1] =  0 -> Activar
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
// Led 8 segmentos
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

// Añadir en fichero de enlace (*.ld):
// . = 0x0C300000;
// .lcdbuffer : { *(.lcdbuffer) }

//----------------------------------------------------------------------------------------------------

void InitializeLCD() {
	// Configura el controlador del LCD dejándolo apagado:
	// + Resolución: 320x240
	// + Modo de barrido: 4 bit single scan
	// + Modo del LCD: 16 niveles de gris
	// + Paletas de color: no
	// + Frecuencia de refresco: 120 Hz
	// + Self-refresh: desactivado
	// + Polaridad de señales de sincronismo: normal
	// + Conmutación de VM: en cada frame
	// + Anchura del blanking horizontal: mínima
	// + Retardo y anchura de pulso de sincronismo horizontal: mínimos
	// + Valores de dithering: por defecto
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
	return (rLCDCON1 & 0x00000001) != 0;
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
	UINT32 aux = ScreenBufferLCD[LCD_BUFFER_WLINE * y + x / LCD_BUFFER_FACTOR];
	return (aux >> (((LCD_BUFFER_FACTOR - 1) - (x % LCD_BUFFER_FACTOR)) * 4)) & 0x0F;
}

//----------------------------------------------------------------------------------------------------

void PutPixelLCD(UINT16 x, UINT16 y, UINT8 color) {
	unsigned index = LCD_BUFFER_WLINE * y + x / LCD_BUFFER_FACTOR;
	UINT32 aux = ScreenBufferLCD[index];
	UINT16 offset = (((LCD_BUFFER_FACTOR - 1) - (x % LCD_BUFFER_FACTOR)) * 4);
	aux = (aux & ~(0x0F << offset)) | ((color & 0x0F) << offset);
	ScreenBufferLCD[index] = aux;
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

//****************************************************************************************************
// LCD
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
