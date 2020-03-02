#ifndef ___MCF52110_UART_H_
#define ___MCF52110_UART_H_

#include "MCF52255.h"

#define EnableInterrupts asm { move.w SR,D0; andi.l #0xF8FF,D0; move.w D0,SR;  }

#define wait 0
#define over 1

void uart0_init(uint32 SYS_CLOCK,uint16 baudrate);
void uart0_putchar(char c);
unsigned char uart0_getchar();
void uart0_putstr(char *str);

__declspec(interrupt:0) void UART0_interrupt(void);
__declspec(interrupt:0) void UART1_interrupt(void);
__declspec(interrupt:0) void UART2_interrupt(void);

#endif