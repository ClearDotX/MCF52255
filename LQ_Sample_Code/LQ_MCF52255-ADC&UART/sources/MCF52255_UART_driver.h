#ifndef __jwy_MCF52255_UART_H_
#define __jwy_MCF52255_UART_H_

#include "MCF52255.h"

#define EnableInterrupts asm { move.w SR,D0; andi.l #0xF8FF,D0; move.w D0,SR;  }

#define wait 0
#define over 1

void uart0_init(uint16 baudrate);
void uart0_putchar(char c);
unsigned char uart0_getchar();
void uart0_putstr(char *str);
void uart0_printf(char *str,uint32 data);
void uart0_printfs(char *str,uint8 data[] ,uint32 num);

void uart1_init(unsigned long baudrate);
void uart1_putchar(char c);
unsigned char uart1_getchar();
void uart1_putstr(char *str);


void uart2_init(unsigned long baudrate);
void uart2_putchar(char c);
unsigned char uart2_getchar();
void uart2_putstr(char *str);



void uart_init(uint8 channel,int32 baudrate);
void uart_putchar(uint8 channel,unsigned char c);
void uart_putstr(uint8 channel,char*str);
unsigned char uart_getchar(uint8 channel);

void DMA_init(void);

__declspec(interrupt:0) void DMA0_handler(void);
__declspec(interrupt:0) void UART0_interrupt(void);
__declspec(interrupt:0) void UART1_interrupt(void);
__declspec(interrupt:0) void UART2_interrupt(void);

#endif