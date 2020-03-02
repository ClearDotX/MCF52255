/*
 * File:		timer_main.c
 * Purpose:		Blink LED at a periodic rate using a timer interrupt
 *
 * License:     All software covered by license agreement in -
 *              docs/Freescale_Software_License.pdf
 */

#include "common.h"

/********************************************************************/

#define DELAY   1000000/2  /* Delay in micro-seconds */

/********************************************************************/
__interrupt__ void
dtim0_handler(void)
{
    static int leds = 1;

    board_led_display(leds);

    leds = (leds + 1) % 16;
    
    /* Clear the interrupt event */
    MCF_DTIM0_DTER |= MCF_DTIM_DTER_REF;
}
/********************************************************************/
void 
main (void)
{
	
	printf("\nBlinking LEDs every half second using a timer (DTIM0) interrupt!\n");

    /* Setup the interrupt controller registers for DTIM0*/
    MCF_INTC0_ICR19 = MCF_INTC_ICR_IL(7) | MCF_INTC_ICR_IP(7);
    MCF_INTC0_IMRL &= ~(MCF_INTC_IMRL_INT_MASK19 | MCF_INTC_IMRL_MASKALL);

    /* Set the interrupt handlers in the vector table */
    mcf5xxx_set_handler(64 + 19, (ADDRESS)dtim0_handler);
	
	/* Initialize the LED's */
	leds_init();
	
    /* Global interrupt enable */
	mcf5xxx_irq_enable();

    /* Configure DTIM0 as a timeout counter */
    MCF_DTIM0_DTRR  = (DELAY - 1);
    MCF_DTIM0_DTMR  = 0
        | MCF_DTIM_DTMR_PS(SYS_CLK_MHZ)
        | MCF_DTIM_DTMR_ORRI
        | MCF_DTIM_DTMR_FRR
        | MCF_DTIM_DTMR_CLK_DIV1
        | MCF_DTIM_DTMR_RST;

    /* Nothing more to do.  Interrupt take over from here */
    while(1)
	{
        nop();
    }
}
/********************************************************************/
