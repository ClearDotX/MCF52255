/****************************************************************************
 *
 *            Copyright (c) 2006-2007 by CMX Systems, Inc.
 *
 * This software is copyrighted by and is the sole property of
 * CMX.  All rights, title, ownership, or other interests
 * in the software remain the property of CMX.  This
 * software may only be used in accordance with the corresponding
 * license agreement.  Any unauthorized use, duplication, transmission,
 * distribution, or disclosure of this software is expressly forbidden.
 *
 * This Copyright notice may not be removed or modified without prior
 * written consent of CMX.
 *
 * CMX reserves the right to modify this software without notice.
 *
 * CMX Systems, Inc.
 * 12276 San Jose Blvd. #511
 * Jacksonville, FL 32223
 * USA
 *
 * Tel:  (904) 880-1840
 * Fax:  (904) 880-1632
 * http: www.cmx.com
 * email: cmx@cmx.com
 *
 ***************************************************************************/
#include "hcc_types.h"
#include "mcf5222x_reg.h"
#include "uart-drv/uart.h"

/****************************************************************************
 ************************** Macro definitions *******************************
 ***************************************************************************/
#define RAM_BUFFER_SIZE 0xffu
#define UART_NUMBER   0u
#define	SYSTEM_CLOCK  80000ul	/* system bus frequency in KHz */

#if UART_NUMBER==0
  #define MCF_GPIO_PUXPAR            MCF_GPIO_PUAPAR
  #define MCF_GPIO_PUXPAR_RXD0_RXD0  MCF_GPIO_PUAPAR_RXD0_RXD0
  #define MCF_GPIO_PUXPAR_TXD0_TXD0  MCF_GPIO_PUAPAR_TXD0_TXD0
#elif UART_NUMBER==1
  #define MCF_GPIO_PUXPAR            MCF_GPIO_PUBPAR
  #define MCF_GPIO_PUXPAR_RXD0_RXD0  MCF_GPIO_PUBPAR_RXD1_RXD1
  #define MCF_GPIO_PUXPAR_TXD0_TXD0  MCF_GPIO_PUBPAR_TXD1_TXD1
#else
  #define MCF_GPIO_PUXPAR            MCF_GPIO_PUCPAR
  #define MCF_GPIO_PUXPAR_RXD0_RXD0  MCF_GPIO_PUCPAR_RXD2_RXD2
  #define MCF_GPIO_PUXPAR_TXD0_TXD0  MCF_GPIO_PUCPAR_TXD2_TXD2
#endif

/****************************************************************************
 ************************** Module variables ********************************
 ***************************************************************************/
static hcc_u16 bps_divider_used;
volatile long rs232_ctr;

/****************************************************************************
 ************************** Function definitions ****************************
 ***************************************************************************/

/*****************************************************************************
 * Configure UART.
 ****************************************************************************/
void uart_init(hcc_u32 bps, hcc_u8 stp, hcc_u8 par, hcc_u8 ndata)
{
  hcc_u8 umr1=0, umr2=0;

  /* Init GPIO pins. */
  MCF_GPIO_PUXPAR = MCF_GPIO_PUXPAR_RXD0_RXD0 | MCF_GPIO_PUXPAR_TXD0_TXD0;  

  /* Disable receiver and transmitter */
  MCF_UART_UCR(UART_NUMBER) = MCF_UART_UCR_TX_DISABLED | MCF_UART_UCR_RX_DISABLED;
  
  /* Reset transmitter */
  MCF_UART_UCR(UART_NUMBER) = MCF_UART_UCR_RESET_TX;
 /* and receiver */
  MCF_UART_UCR(UART_NUMBER) = MCF_UART_UCR_RESET_RX;
  /* Reset Mode Register */
  MCF_UART_UCR(UART_NUMBER) = MCF_UART_UCR_RESET_MR;

  switch(par)
  {
  default:
  case 0: /* no parity */
    umr1 |= MCF_UART_UMR_PM_NONE;
    break;
  case 1: /* odd parity */
    umr1 |= MCF_UART_UMR_PM_ODD;
    break;  
  case 2: /* even parity */
    break;
  }
  
  switch(ndata)
  {
  case 5:
    break;  
  case 6:
    umr1 |= MCF_UART_UMR_BC_6;
    break;
  case 7:
    umr1 |= MCF_UART_UMR_BC_7;
    break;
  case 8:
  default:  
    umr1 |= MCF_UART_UMR_BC_8;
    break;      
  }
  
  switch(stp)
  {
  default:
  case 1: 
    umr2 |= MCF_UART_UMR_SB_STOP_BITS_1;
    break;
  case 2:
    umr2 |= MCF_UART_UMR_SB_STOP_BITS_15;
    break;
  case 3:
    umr2 |= MCF_UART_UMR_SB_STOP_BITS_2;
  }

  /* Set line coding. */
  MCF_UART_UMR(UART_NUMBER) = umr1;  
  MCF_UART_UMR(UART_NUMBER) = umr2; 

  /* Set Rx and Tx baud by SYSTEM CLOCK */
  MCF_UART_UCSR(UART_NUMBER) = MCF_UART_UCSR_RCS_SYS_CLK | MCF_UART_UCSR_TCS_SYS_CLK;

  MCF_UART_UIMR(UART_NUMBER) = 0;
  uart_set_bps(bps);
  /* Enable receiver and transmitter */
  MCF_UART_UCR(UART_NUMBER) = MCF_UART_UCR_TX_ENABLED | MCF_UART_UCR_RX_ENABLED;

  /********************************************************************
   * Wait for RS232 driver chip to powerup. On a USB device pre-enumeration
   * current is limited to 100mA So we wait here.
   *
   ********************************************************************/
  for ( rs232_ctr=0x200000; rs232_ctr > 0; rs232_ctr--) /*wait for rs232 chip*/
    ;
}

/*****************************************************************************
 * Return current used baud rate.
 ****************************************************************************/
hcc_u32 uart_get_bps(void)
{
  return(((SYSTEM_CLOCK*125)/(bps_divider_used*4)));
}

/*****************************************************************************
 * Seat baud rate.
 ****************************************************************************/
hcc_u32 uart_set_bps(hcc_u32 bps)
{
  /* Calculate baud settings */
  hcc_u32 d = ((SYSTEM_CLOCK*125)/(bps*4));

  if (d> 0xffff)
  {
    d=0xffff;
  }
  if (d <2)
  {
    d=2;
  }
  bps_divider_used=(hcc_u16)d;
  MCF_UART_UBG1(UART_NUMBER) = (hcc_u8)(d >> 8);
  MCF_UART_UBG2(UART_NUMBER) = (hcc_u8)d;
  return(uart_get_bps());
}

/*****************************************************************************
 * Send one character. Wait till transmitter is empty.
 ****************************************************************************/
int uart_putch(char c)
{
  /* If space is available in the FIFO */
  if (MCF_UART_USR(UART_NUMBER) & MCF_UART_USR_TXRDY)
  {
    /* Send the character */
    MCF_UART_UTB(UART_NUMBER) = (unsigned char)c;
    return((int)(unsigned char)c);
  }
  return(((int)(unsigned char)c)+1);
}

/*****************************************************************************
 * Wait till a character is received, and return it.
 ****************************************************************************/
int uart_getch(void)
{
  return ((int)MCF_UART_URB(UART_NUMBER));
}

/*****************************************************************************
 * Return !=0 if there is at least one pending character in the receiver.
 ****************************************************************************/
int uart_kbhit(void)
{
  return((int)((MCF_UART_USR(UART_NUMBER) & MCF_UART_USR_RXRDY) ? 1 : 0));
}
/****************************** END OF FILE **********************************/
