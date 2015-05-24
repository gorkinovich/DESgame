#include "s3c44b0x.h"
#include "s3cev40.h"
#include "iic.h"

extern uint32 mclk; // Frecuencia actual del reloj (Hz)

extern void DelayMs(int ms_time);

void iic_init() {
	// Activar el reloj para los bloques ICC, GPIO y BDMA.
	// NOTA: Si no se activa BDMA, el controlador IIC no es accesible.
	CLKCON |= (1 << 13); // ICC = 1
	CLKCON |= (1 << 10); // GPIO = 1
	CLKCON |= (1 <<  7); // BDMA = 1

	PCONF = (PCONF & (~0xF)) | 0xA; // PF1 = 10 (IICSDA), PF0 = 10 (IICSCL)
	PUPF &= 0xFC;                   // Pull-up enable: PF1 = 0, PF0 = 0
	
	// Configurar la dirección del slave:
	IICADD = S3C44B0X_SLAVE_ADDRESS;

	// Generación ACK habilitada, interrupciones habilitadas:
	// ACK = 1, Interrupción Tx/Rx = 1, Preescalado: ((mclk / 16) / 250000) - 1
	IICCON = (1 << 7) | (1 << 5) | ((mclk / 4000000 - 1) & 0x0F);

	// Activa Tx/Rx:
	IICSTAT = (1 << 4);
}

void iic_putByte_start(uint8 byte) {
	// Escribe el dato
	IICDS = byte;
	// Máster Tx, start condition, Tx/Rx habilitada
	IICSTAT = (IICSTAT & 0x0F) | 0xF0;
	// Comienza la transmisión (borrando pending bit del IICCON)
	IICCON &= ~(1 << 4);
	// Espera la recepción de ACK
	while (!(IICCON & 0x10)); //while (IICSTAT & 0x1);
}

void iic_putByte(uint8 byte) {
	// Escribe el dato
	IICDS = byte;
	// Comienza la transmisión del dato (borrando pending bit del IICCON)
	IICCON &= ~(1 << 4);
	// Espera la recepción de ACK
	while (!(IICCON & 0x10)); //while (IICSTAT & 0x1);
}

void iic_putByte_stop(uint8 byte) {
	// Escribe el dato
	IICDS = byte;
	// Comienza la trasmisión del dato (borrando pending bit del IICCON)
	IICCON &= ~(1 << 4);
	// Espera la recepción de ACK
	while (!(IICCON & 0x10)); //while (IICSTAT & 0x1);

	// Máster Tx, stop condition, Tx/Rx habilitada
	IICSTAT = (IICSTAT & 0x0F) | 0xD0;
	// Comienza la trasmisión de STOP (borrando pending bit del IICCON)
	IICCON &= ~(1 << 4);
	// Espera a que la stop condition tenga efecto (5 ms para la at24c04)
	DelayMs(5);
}

void iic_getByte_start(uint8 byte) {
	// Escribe el dato
	IICDS = byte;
	// Máster Rx, start condition, Tx/Rx habilitada
	IICSTAT = (IICSTAT & 0x0F) | 0xB0;
	// Comienza la transmisión (borrando pending bit del IICCON)
	IICCON &= ~(1 << 4);
	// Espera la rececpión de ACK
	while (!(IICCON & 0x10)); //while (IICSTAT & 0x1);
}

uint8 iic_getByte() {
	// Reanuda la recepción (borrando pending bit del IICCON)
	IICCON &= ~(1 << 4);
	// Espera la recepción del dato
	while (!(IICCON & 0x10)); //while (IICSTAT & 0x1);
	return IICDS; // Lee el dato
}

uint8 iic_getByte_stop(int8 ack) {
	IICCON = (IICCON & ~(1 << 7)) | (ack << 7); // Activa o no la generación de ACK.

	// Reanuda la recepción (borrando pending bit del IICCON)
	IICCON &= ~(1 << 4);
	// Espera la recepción del dato
	while (!(IICCON & 0x10)); //while (IICSTAT & 0x1);
	uint8 byte = IICDS; // Lee el dato

	// Máster Rx, stop condition, Tx/Rx habilitada
	IICSTAT = (IICSTAT & 0x0F) | 0x90;
	// Comienza la trasmisión de STOP (borrando pending bit del IICCON)
	IICCON &= ~(1 << 4);
	// Espera a que la stop condition tenga efecto (5 ms para la at24c04)
	DelayMs(5);

	IICCON |= (1 << 7); // Habilita la generación de ACK
	return byte;
}
