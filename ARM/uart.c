/*********************************************************************************************
* Fichero:	uart.c
* Autor:
* Descrip:	funciones de UART
* Version: <P6-ARM-simple>
*********************************************************************************************/

/*--- ficheros de cabecera ---*/
#include "44b.h"
#include "uart.h"
#include <stdarg.h>
#include <stdio.h>

//-------------------------------------------------------------------------------------------

/*--- definicion de constantes ---*/
#define KEY_BUFLEN 100

/*--- variables globales ---*/
static unsigned char keyBuf[KEY_BUFLEN]; // buffer de recepcion
volatile static int keyBufRdPt=0; // puntero de lectura
volatile static int keyBufWrPt=0; // puntero de escritura
static char *uart0TxStr; // puntero al buffer de envio
static char uartUsePolling = 1;

/*--- prototipos de funciones ---*/
void Uart0_RxInt(void) __attribute__ ((interrupt ("IRQ")));
void Uart0_TxInt(void) __attribute__ ((interrupt ("IRQ")));

/*--- RUTINAS DE TRATAMIENTO ---*/
void Uart0_RxInt(void){
	/* Se encarga de leer los datos de la UART y los almacena
	   en un buffer como ilustra la figuA. Y para evitar qeu se pierdan datos
	   comprobamos es estatus de los punteros de lectura y escritura */

	keyBufWrPt++;                       // estamos con el de escritura
	keyBufWrPt%=KEY_BUFLEN;
	if(keyBufWrPt!=keyBufRdPt){
		while (!(rUTRSTAT0 & 0x1));     // esperamos que la uart tenga datos
		keyBuf[keyBufWrPt] = RdURXH0(); // la macro esa que lee el caracter "a mano"
										// leemos del registro URXH0,
										// que contiene el dato recibido
	}else{
		keyBufWrPt+=KEY_BUFLEN-1;
		keyBufWrPt%=KEY_BUFLEN;
	}

	rI_ISPC |= (BIT_URXD0);
}

void Uart0_TxInt(void){
	/* Se encarga de ir leyendo caracteres del buffer de envio y los escribe en la UART.
	Con cada interrupcion desplaza el puntero del buffer de envío y escribe un nuevo caracter
	como ilustra la figura. Cuando se llega al final del string almacenado en el buffer de envio,
	se enmascara de nuevo int_utdx0.
	*/
	char caracter = *uart0TxStr;
	uart0TxStr++;

	if(caracter != '\0'){
		UART0_SendByte(caracter);
	}else{ //lega al final
		rINTMSK |= (BIT_UTXD0);
	}
	rI_ISPC |= (BIT_UTXD0);
}

//-------------------------------------------------------------------------------------------

/*--- implementación de las funciones ---*/
void UART_Init(int bauds) {
    /* UART0 */
    rULCON0=0x3;     // Modo normal, no paridad, 1b stop, 8b char
    rUCON0=0x205;    // tx=nivel, rx=flanco, no rx-timeout ni rx-error, normal, int/polling
    rUBRDIV0= ((int)(MCLK / 16.0 / bauds + 0.5) - 1); // formula division de frecuencia
    rUFCON0=0x0;     // Desactivar FIFO
    rUMCON0=0x0;	 // Desactivar control de flujo

    /* UART1 */
    rULCON1=0x3;     // Modo normal, no paridad, 1b stop, 8b char
    rUCON1=0x205;    // tx=nivel, rx=flanco, no rx-timeout ni rx-error, normal, int/polling
    rUBRDIV1= ((int)(MCLK / 16.0 / bauds + 0.5) - 1); // formula division de frecuencia
    rUFCON1=0x0;	// Desactivar FIFO
    rUMCON1=0x0;	// Desactivar control de flujo
}

void UART_Config(void) {
	// si no se usan interrupciones, no es necesario hacer nada

	rI_ISPC |= ( (0x1)<<21 );	// Borra INTPND escribiendo 1s en I_ISPC

	rEXTINTPND = 0xf;	// Borra EXTINTPND escribiendo 1s en el propio registro

	rINTMOD = 0x0;	// Configura las lineas como de tipo IRQ mediante INTMOD

	rINTCON = 0x1;		// Habilita int. vectorizadas y la linea IRQ (FIQ no) mediante INTCON

	rINTMSK = 0xffffffff&~BIT_GLOBAL&~BIT_URXD0;
					// enmascaramos todas las interupciones salvo global y recepcion/envio

	pISR_URXD0=(unsigned)Uart0_RxInt;
	pISR_UTXD0=(unsigned)Uart0_TxInt;

	rI_ISPC = 0xffff;
	rEXTINTPND = 0xf;

	keyBufRdPt=0; // puntero de lectura
	keyBufWrPt=0; // puntero de escritura

	uartUsePolling = 0;
}

//-------------------------------------------------------------------------------------------

void UART0_TxEmpty(void) {
    while (!(rUTRSTAT0 & 0x4)); 	     // esperar a que el shifter de TX se vacie
}


char UART0_Getch(void) {
	if (uartUsePolling) {
		// Versión poling
		while (!(rUTRSTAT0 & 0x1));        // esperar a que el buffer contenga datos
		return RdURXH0();		   		   // devolver el caracter
	} else {
		// Versión Interrupciones
		while(keyBufRdPt==keyBufWrPt); // si son iguales, hay que esperar
										  // a que dejen de serlo
		keyBufRdPt++;
		keyBufRdPt%=KEY_BUFLEN;
		return keyBuf[keyBufRdPt]; // devolver el caracter
	}
}

void UART0_SendByte(int data) {
    //char localBuf[2] = { '\0', '\0' };
    if (data == '\n') {
	   while (!(rUTRSTAT0 & 0x2));     // esperar a que THR se vacie
	   WrUTXH0('\r');			       // escribir retorno de carro (utilizar macro)
	}
	while (!(rUTRSTAT0 & 0x2)); 	   // esperar a que THR se vacie
	WrUTXH0(data);				       // escribir data (utilizar macro)
}

void UART0_SendString(char * pt) {
	//	 Por poling
	while (*pt)						    // mandar byte a byte hasta completar string
		UART0_SendByte(*pt++);

	if (!uartUsePolling) {
		// Por interrupciones
		//	uart0TxStr = pt; // para que apunte al comienzo del string
		//	// desenmascaramos int_utxd0 poniendo un cero
		//	rINTMSK &= ~(BIT_UTXD0);
		//	//mientras sigue habilitada la interrupcion espera
		while(!(rINTMSK & BIT_UTXD0));
	}
}

void UART0_Printf(char * fmt, ...) {
    va_list ap;
    char string[256];

    va_start(ap, fmt);
    vsprintf(string, fmt, ap);
    UART0_SendString(string);
    va_end(ap);
}

//-------------------------------------------------------------------------------------------

void UART1_TxEmpty(void) {
    while (!(rUTRSTAT1 & 0x4)); 	     // esperar a que el shifter de TX se vacie
}


char UART1_Getch(void) {
    while (!(rUTRSTAT1 & 0x1));        // esperar a que el buffer contenga datos
	return RdURXH1();		   		   // devolver el caracter
}

void UART1_SendByte(int data) {
    //char localBuf[2] = { '\0', '\0' };
    if (data == '\n') {
	   while (!(rUTRSTAT1 & 0x2));     // esperar a que THR se vacie
	   WrUTXH1('\r');			       // escribir retorno de carro (utilizar macro)
	}
	while (!(rUTRSTAT1 & 0x2)); 	   // esperar a que THR se vacie
	WrUTXH1(data);				       // escribir data (utilizar macro)
}

void UART1_SendString(char * pt) {
    while (*pt)						   // mandar byte a byte hasta completar string
        UART1_SendByte(*pt++);
}

void UART1_Printf(char * fmt, ...) {
    va_list ap;
    char string[256];

    va_start(ap, fmt);
    vsprintf(string, fmt, ap);
    UART1_SendString(string);
    va_end(ap);
}
