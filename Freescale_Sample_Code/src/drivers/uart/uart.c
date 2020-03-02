/*
 * File:        uart.c
 * Purpose:     Provide common ColdFire UART routines for serial IO
 *
 * Notes:       
 *              
 * License:     All software covered by license agreement in -
 *              docs/Freescale_Software_License.pdf
 */

#include "common.h"
#include "uart/uart.h"

/********************************************************************/
/*
 * Initialize the UART for 8N1 operation, interrupts disabled, and
 * no hardware flow-control
 *
 * Parameters:
 *  uartch      UART channel to initialize
 *  sysclk      UART System Clock (in KHz)
 *  baud        UART baud rate
 *  settings    Initialization parameters
 */
void 
uart_init (int uartch, int sysclk, int baud)
{
    register uint16 ubgs;

	/*
	* Enable UART Port on GPIO
	*/
	switch(uartch)
	{
	case 0:
    MCF_GPIO_PUAPAR = 0
        | MCF_GPIO_PUAPAR_URXD0_URXD0
        | MCF_GPIO_PUAPAR_UTXD0_UTXD0;
	break;
	case 1:
    MCF_GPIO_PUBPAR = 0
        | MCF_GPIO_PUBPAR_URXD1_URXD1
        | MCF_GPIO_PUBPAR_UTXD1_UTXD1;
	break;
	case 2:
    MCF_GPIO_PUCPAR = 0
        | MCF_GPIO_PUCPAR_URXD2_URXD2
        | MCF_GPIO_PUCPAR_UTXD2_UTXD2;
	break;
	default:
		break;
	}

    /* 
     * Reset Transmitter 
     */
    MCF_UART_UCR(uartch) = MCF_UART_UCR_RESET_TX;

    /* 
     * Reset Receiver 
     */
    MCF_UART_UCR(uartch) = MCF_UART_UCR_RESET_RX;

    /* 
     * Reset Mode Register 
     */
    MCF_UART_UCR(uartch) = MCF_UART_UCR_RESET_MR;

#if UART_DMA_SUPPORT == 1	
		MCF_UART_UIMR(uartch) = (0
		| MCF_UART_UIMR_DB              
		| MCF_UART_UIMR_COS) ;
#else	
	/*
	* Enable UART interrupt
	*/
	MCF_UART_UIMR(uartch) = (0
	//	| MCF_UART_UIMR_TXRDY
		| MCF_UART_UIMR_FFULL_RXRDY        
		| MCF_UART_UIMR_DB              
		| MCF_UART_UIMR_COS) ;
#endif

    /* 
      * No parity, 8-bits per character, RXRDY is source for interrupt or DMA request
      * enable receiver request-to-send
      */
    MCF_UART_UMR(uartch) = (0
        | MCF_UART_UMR_PM_NONE
#if UART_HARDWARE_FLOW_CONTROL == 1
		| MCF_UART_UMR_RXRTS;
#endif
        | MCF_UART_UMR_BC_8);

    /* 
      * Normal mode(No echo or loopback), 1 stop bit 
      * enable transmitter clear-to-send
      */
    MCF_UART_UMR(uartch) = (0
        | MCF_UART_UMR_CM_NORMAL
#if UART_HARDWARE_FLOW_CONTROL == 1
		| MCF_UART_UMR_TXCTS
		| MCF_UART_UMR_TXRTS
#endif
        | MCF_UART_UMR_SB_STOP_BITS_1);



	/*
	* Initialize input enable control, UACR
	* trigger interrupt when /CTS has change of state
	*/
	MCF_UART_UACR(uartch) = MCF_UART_UACR_IEC;
	
    /* 
     * Select Rx and Tx clocks
     */
    MCF_UART_UCSR(uartch) = (0
        | MCF_UART_UCSR_RCS_SYS_CLK
        | MCF_UART_UCSR_TCS_SYS_CLK);

    /* 
     * Calculate baud settings 
     */
    ubgs = (uint16)((sysclk*1000)/(baud * 32));

    MCF_UART_UBG1(uartch) = (uint8)((ubgs & 0xFF00) >> 8);
    MCF_UART_UBG2(uartch) = (uint8)(ubgs & 0x00FF);
	
    /* 
     * Enable receiver and transmitter 
     */
    MCF_UART_UCR(uartch) = (0
        | MCF_UART_UCR_TX_ENABLED
        | MCF_UART_UCR_RX_ENABLED);
}
/********************************************************************/
/*
 * Wait for a character to be received on the specified UART
 *
 * Return Values:
 *  the received character
 */
char
uart_getchar (int channel)
{
    /* Wait until character has been received */
    while (!(MCF_UART_USR(channel) & MCF_UART_USR_RXRDY))
        ; 
    return MCF_UART_URB(channel);
}
/********************************************************************/
/*
 * Wait for space in the UART Tx FIFO and then send a character
 */ 
void
uart_putchar (int channel, char ch)
{
    /* Wait until space is available in the FIFO */
    while (!(MCF_UART_USR(channel) & MCF_UART_USR_TXRDY))
		;
    /* Send the character */
    MCF_UART_UTB(channel) = (uint8)ch;
}
/********************************************************************/
/*
 * Check to see if a character has been received
 *
 * Return values:
 *  0       No character received
 *  1       Character has been received
 */
int
uart_getchar_present (int channel)
{
    return (MCF_UART_USR(channel) & MCF_UART_USR_RXRDY);
}
/********************************************************************/

static char str[] = "Hello world\n";

__interrupt__ void
uart0_handler(void)
{
	static uint8 usr = 0;
	static uint8 uisr = 0;
	static index = 0;
	static uint8 ch;
	
	usr = MCF_UART0_USR;
	uisr = MCF_UART0_UISR;
	
	//transmit what receive back to console
	if(usr & MCF_UART_USR_RXRDY)
	{
		//receiver ready
		ch = MCF_UART0_URB;
		MCF_UART0_UTB = (uint8)ch;
	}
	if(usr & MCF_UART_USR_TXRDY)
	{
		//transmitter ready
		if(str[index] != '\0')
			MCF_UART0_UTB= (uint8)str[index++];
	}

	if(usr & MCF_UART_USR_FFULL)
	{
		//receiver FIFO is full
	}
	if(usr & MCF_UART_USR_TXEMP)
	{
		//transmitter empty
	}
	if(usr & MCF_UART_USR_OE)
	{
		//overrun occured
	}
	if(usr & MCF_UART_USR_PE)
	{
		//parity error occured
	}
	if(usr & MCF_UART_USR_FE)
	{
		//frame error occured
	}
	if(usr & MCF_UART_USR_RB)
	{
		//received break
	}
	if(uisr & MCF_UART_UISR_DB)
	{
		//delta break
	}
	if(uisr & MCF_UART_UISR_COS)
	{
		//change of state occured on /CTS
	}
}


__interrupt__ void
uart1_handler(void)
{
	static uint8 usr = 0;
	static uint8 uisr = 0;
	static index = 0;
	static uint8 ch;
	
	usr = MCF_UART1_USR;
	uisr = MCF_UART1_UISR;

	//transmit what receive back to console
	if(usr & MCF_UART_USR_RXRDY)
	{
		//receiver ready
		ch = MCF_UART1_URB;
		MCF_UART1_UTB = (uint8)ch;
	}	
	if(usr & MCF_UART_USR_TXRDY)
	{
		//transmitter ready
		if(str[index] != '\0')
			MCF_UART1_UTB= (uint8)str[index++];
	}
	if(usr & MCF_UART_USR_FFULL)
	{
		//receiver FIFO is full
	}
	if(usr & MCF_UART_USR_TXEMP)
	{
		//transmitter empty
	}
	if(usr & MCF_UART_USR_OE)
	{
		//overrun occured
	}
	if(usr & MCF_UART_USR_PE)
	{
		//parity error occured
	}
	if(usr & MCF_UART_USR_FE)
	{
		//frame error occured
	}
	if(usr & MCF_UART_USR_RB)
	{
		//received break
	}
	if(uisr & MCF_UART_UISR_DB)
	{
		//delta break
	}
	if(uisr & MCF_UART_UISR_COS)
	{
		//change of state occured on /CTS
	}
}

__interrupt__ void
uart2_handler(void)
{
	static uint8 usr = 0;
	static uint8 uisr = 0;
	static index = 0;
	static uint8 ch;
	
	usr = MCF_UART2_USR;
	uisr = MCF_UART2_UISR;

	//transmit what receive back to console
	if(usr & MCF_UART_USR_RXRDY)
	{
		//receiver ready
		ch = MCF_UART2_URB;
		MCF_UART2_UTB = (uint8)ch;
	}	
	if(usr & MCF_UART_USR_TXRDY)
	{
		//transmitter ready
		if(str[index] != '\0')
			MCF_UART2_UTB= (uint8)str[index++];
	}
	if(usr & MCF_UART_USR_FFULL)
	{
		//receiver FIFO is full
	}
	if(usr & MCF_UART_USR_TXEMP)
	{
		//transmitter empty
	}
	if(usr & MCF_UART_USR_OE)
	{
		//overrun occured
	}
	if(usr & MCF_UART_USR_PE)
	{
		//parity error occured
	}
	if(usr & MCF_UART_USR_FE)
	{
		//frame error occured
	}
	if(usr & MCF_UART_USR_RB)
	{
		//received break
	}
	if(uisr & MCF_UART_UISR_DB)
	{
		//delta break
	}
	if(uisr & MCF_UART_UISR_COS)
	{
		//change of state occured on /CTS
	}	
}

