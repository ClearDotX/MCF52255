/*
 * File:		pit_main.c
 * Purpose:		PIT Examples
 * 
 * License:     All software covered by license agreement in -
 *              docs/Freescale_Software_License.pdf
 */

#include "common.h"
#include "uart.h"
#include "pit.h"
#include "pit_example.h"

typedef int8 (* example_fuct) (void);

/*
 * Available functions
 */
example_fuct example_functions[]={
                            pit_periodicity,
                            pit_countdown,
						   };

/********************************************************************/
void main (void)
{
	uint8 i;
	uint8 u8Continue =0;
	
	//unmask UART0 interrupts in INTC0
	MCF_INTC0_IMRL &= ~(MCF_INTC_IMRL_INT_MASK13);
        
	/* Setup the interrupt level and priority for UART0 */
	MCF_INTC0_ICR13 = MCF_INTC_ICR_IL(4) | MCF_INTC_ICR_IP(4);
	
	/* Set the interrupt handlers in the vector table */
	mcf5xxx_set_handler(64 + 13, (ADDRESS)uart0_handler);	

	/* Set the interrupt handlers in the vector table */
	mcf5xxx_set_handler(64 + 55, (ADDRESS)pit0_isr);
	mcf5xxx_set_handler(64 + 56, (ADDRESS)pit1_isr);

	printf("PIT Example for Kirin\n");
	
	/* PIT Basic Initialization */
	(void) PITInit(0, 800, 0);
	(void) PITInit(0, 800, 1);
	
	/* Global interrupt enable */
	mcf5xxx_wr_sr(0x2000);


	/* Perform Tests */
	do
	{
		for (i=0;i<sizeof(example_functions)/sizeof(example_functions[0]);i++)
		{
			if (example_functions[i]() != 0x00)
			{
				printf ("Example #%d Failed!! \n", i);
			}
			else
			{
				printf ("Example #%d OK \n", i);
			}
		}
 
		printf ("Press 'Y' to repeat examples\n");
		u8Continue = in_char();
	}while (u8Continue == 'Y' || u8Continue == 'y');

	printf ("End Examples\n");    
}

/********************************************************************/
	
