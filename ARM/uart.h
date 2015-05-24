/*********************************************************************************************
* Fichero:	uart.h
* Autor:
* Descrip:	cabecera de las funciones de UART
* Version: <P5-ARM-simple>
*********************************************************************************************/

#ifndef __UART_H__
#define __UART_H__

#ifdef __cplusplus
extern "C" {
#endif

#define BAUDS_115200 115200

/*--- definicón de constantes ---*/
#define LF_char 0x0A	/* Caracter de salto de línea */
#define CR_char 0x0D    /* Caracter de retorno de carro */

/*--- prototipos de funciones ---*/
void UART_Init(int bauds);
void UART_Config(void);

void UART0_TxEmpty(void);
char UART0_Getch(void);
void UART0_SendByte(int data);
void UART0_SendString(char *pt);
void UART0_Printf(char *fmt,...);

void UART1_TxEmpty(void);
char UART1_Getch(void);
void UART1_SendByte(int data);
void UART1_SendString(char *pt);
void UART1_Printf(char *fmt,...);

#ifdef __cplusplus
}
#endif

#endif /* __UART_H__ */
