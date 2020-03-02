/*
 * File:		io.c
 * Purpose:		Serial Input/Output routines
 *
 * Notes:       TERMINAL_PORT defined in <board>.h
 * 
 * License:     All software covered by license agreement in -
 *              docs/Freescale_Software_License.pdf
 */

#include "common.h"
#include "uart/uart.h"

/********************************************************************/
char
in_char (void)
{
	return uart_getchar(TERMINAL_PORT);
}
/********************************************************************/
void
out_char (char ch)
{
	uart_putchar(TERMINAL_PORT, ch);
}
/********************************************************************/
int
char_present (void)
{
	return uart_getchar_present(TERMINAL_PORT);
}
/********************************************************************/
