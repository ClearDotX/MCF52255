/*
 * File:		uart.h
 * Purpose:     Provide common ColdFire UART routines for polled serial IO
 *
 * Notes:
 * 
 * License:     All software covered by license agreement in -
 *              docs/Freescale_Software_License.pdf
 */

#ifndef __UART_H__
#define __UART_H__

/********************************************************************/
#define UART_DMA_SUPPORT 0
#define UART_HARDWARE_FLOW_CONTROL 0

void uart_init (int, int, int);
char uart_getchar (int);
void uart_putchar (int, char);
int  uart_getchar_present (int);

__interrupt__ void uart0_handler(void);
__interrupt__ void uart1_handler(void);
__interrupt__ void uart2_handler(void);



/********************************************************************/

#endif /* __UART_H__ */
