/*
 * File:		main.c
 * Purpose:		Simple LED example
 *
 * License:     All software covered by license agreement in -
 *              docs/Freescale_Software_License.pdf
 */

#include "common.h"

/********************************************************************/

#define DELAY   150000  /* Delay in micro-seconds */

void check_switches();

/********************************************************************/


void 
main (void)
{
	int i;	
	
	 printf("\nMCF5225X Example Project\n"); 	
	 printf(" Chip ID %x\n",(MCF_CCM_CIR>>6)&0x3FF); 
     printf(" Chip rev %x\n",MCF_CCM_CIR&0x03F);	 
	
    /* Global interrupt enable */
	mcf5xxx_irq_enable();

	printf("Running the \"Example\" project\n");
	leds_init();
    /* 
     * This is the example project.
     * To create a new project, copy the example project files into
     * a new project folder and make modifications there.  The 
     * project make/build files are in the mcf5213/build directory.
     *
     * This project simply toggles the LEDs
     * If using a REVA board, pressing buttons will trigger IRQ interrupt
     * If using a REVB board, buttons tied to GPIO and polled
     */
	#ifdef M52259EVB_REVB   
    MCF_GPIO_PDDPAR=0x00;
    MCF_GPIO_DDRDD=0x00; 
    #endif 

	while(1)
	{
	    for (i = 1; i < 8; i<<=1)
	    {   
	        board_led_display(i);
	        cpu_pause(DELAY);
	        check_switches();
	    }
	    for (i = 8; i > 1; i>>=1)
	    {
	        board_led_display(i);
	        cpu_pause(DELAY);
	        check_switches();	        
	    }
	}
}
/********************************************************************/

void check_switches()
{
	#ifdef M52259EVB_REVB   
	if(~MCF_GPIO_SETDD & 0x80)
    	printf("SW3 pressed\n");		
    if(~MCF_GPIO_SETDD & 0x40)
    	printf("SW2 pressed\n");
    if(~MCF_GPIO_SETDD & 0x20)
    	printf("SW1 pressed\n");
    #endif
}
