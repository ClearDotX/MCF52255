/***************************************************************************
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
#include "target.h"
#include "hcc_types.h"
#include "mcf5222x_reg.h"

//MCF5225x PORT - MPL
#define MCF_CIM_CCE             (*(hcc_reg16*)(&_IPSBAR[0x110010]))
//


extern hcc_u32 VECTOR_TABLE[];

/* These are defined in mcf5xxx_lo.s */
int asm_set_ipl (hcc_u32 val);
void mcf5xxx_wr_cacr (hcc_u32 val);
void mcf5xxx_wr_vbr (hcc_u32 val);

void init_scm(void);
void init_ints(void);
void init_board(void);

static void init_cpu()
{
	/* Disable Software Watchdog Timer */
	MCF_SCM_CWCR = 0;

	/* Enable debug */
	MCF_GPIO_PDDPAR = 0x0F;

	/* Turn Instruction Cache ON */
	mcf5xxx_wr_cacr(0
		| MCF5XXX_CACR_CENB
		| MCF5XXX_CACR_CINV
		| MCF5XXX_CACR_DISD
		| MCF5XXX_CACR_CEIB
		| MCF5XXX_CACR_CLNF_00);
}

static void init_clock()
{

//MCF52259 Update
/*Required if booting with internal relaxation oscillator & pll off, clkmod[1:0]=00 & xtal=1 */

	MCF_CLOCK_OCLR = 0xC0;   //turn on crystal
	MCF_CLOCK_CCLR = 0x00;    //switch to crystal 
    MCF_CLOCK_OCHR = 0x00; //turn off relaxation osc

/*---------------------------------------------------------------------------------*/


  MCF_CLOCK_CCHR =0x05; /* The PLL pre divider - 48MHz / 6 = 8MHz*/

  /* The PLL pre-divider affects this!!! Multiply 8Mhz reference crystal /CCHR
     by 10 to acheive system clock of 80Mhz */
  MCF_CLOCK_SYNCR = MCF_CLOCK_SYNCR_MFD(3) | MCF_CLOCK_SYNCR_CLKSRC
                      | MCF_CLOCK_SYNCR_PLLMODE | MCF_CLOCK_SYNCR_PLLEN ;
  while (!(MCF_CLOCK_SYNSR & MCF_CLOCK_SYNSR_LOCK))
    ;

}

void init_scm()
{
  /* Enable on-chip modules to access internal SRAM */
  MCF_SCM_RAMBAR = (0
    | MCF_SCM_RAMBAR_BA(SRAM_ADDRESS)
    | MCF_SCM_RAMBAR_BDE);


}

void init_ints()
{
  /* Set base address of vector table. */
  mcf5xxx_wr_vbr((hcc_u32)VECTOR_TABLE);
}

void init_board(void)
{
  /* Setup leds. */
  /* Enable signals as GPIO */
  MCF_GPIO_PTCPAR = 0
        | MCF_GPIO_PTCPAR_TIN3_GPIO
        | MCF_GPIO_PTCPAR_TIN2_GPIO
        | MCF_GPIO_PTCPAR_TIN1_GPIO
        | MCF_GPIO_PTCPAR_TIN0_GPIO;

  MCF_GPIO_PORTTC = 0x00; /* Turn LEDS off. */

    /* Enable signals as digital outputs */
  MCF_GPIO_DDRTC = 0
        | MCF_GPIO_DDRTC_DDRTC3
        | MCF_GPIO_DDRTC_DDRTC2
        | MCF_GPIO_DDRTC_DDRTC1
        | MCF_GPIO_DDRTC_DDRTC0;
  
  /* Configure Vbus control (ADM869L). Note CTS0 and RTS0 is used
     by this function. If USB is not used and UART0 needs hardware
     flow control then these lines shall be commented out. */
  MCF_GPIO_PUAPAR &= 0x3f;				       /* set CTS0 for io */
  MCF_GPIO_PORTUA |= MCF_GPIO_PORTUA_PORTUA3;  /* turn AMD (Vbus) switch off */
  MCF_GPIO_DDRUA |= MCF_GPIO_DDRUA_DDRUA3;     /* enable CTS0 as output */

  MCF_GPIO_PUAPAR &= 0xcf;					   /* set  for io */
  MCF_GPIO_DDRUA &= ~ MCF_GPIO_DDRUA_DDRUA2;   /* enable RTS0 as input */

  /* Configure switches. */
  /* by default I/O pins are configured for input -> do nothing */
}

void Usb_Vbus_On(void)
{
  MCF_GPIO_PORTUA &= ~MCF_GPIO_PORTUA_PORTUA3; /* turn AMD (Vbus) switch on */
}

void Usb_Vbus_Off(void)
{
  MCF_GPIO_PORTUA |= MCF_GPIO_PORTUA_PORTUA3; /* turn AMD (Vbus) switch onff */
}

int Usb_Vbus_Stat(void)
{
  return((MCF_GPIO_PORTUA & 0x4)>>2);
}


void _irq_restore (hcc_imask ip)
{
    asm_set_ipl(ip);
}

hcc_imask _irq_disable (void)
{
    return((hcc_imask)asm_set_ipl(7));
}

void hw_init(void)
{
  init_cpu();
  init_clock();
  init_board();
  init_scm();
  init_ints();
  _irq_restore(0);
}
/****************************** END OF FILE **********************************/
