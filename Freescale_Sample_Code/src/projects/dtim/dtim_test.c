/*
 * File:		dtim_main.c
 * Purpose:		DMA Timer Examples
 * 
 * License:     All software covered by license agreement in -
 *              docs/Freescale_Software_License.pdf
 */

#include "common.h"
#include "uart.h"
#include "dtim.h"
#include "dma.h"
#include "dtim_example.h"

typedef int8 (* example_fuct) (void);

/*
 * Available functions
 */
example_fuct example_functions[]={
			//	dtim_reftest,
				dtim_extclktest,
				dtim_dmatest,
				dtim_captest
};

/********************************************************************/
void main (void)
{
	uint8 i;
	uint8 u8Continue =0;

	uart_init(0, SYS_CLK_KHZ, UART_BAUD);

	//unmask UART0 interrupts in INTC0
	MCF_INTC0_IMRL &= ~(0
					 | MCF_INTC_IMRL_INT_MASK13
					 | MCF_INTC_IMRL_MASKALL);
	
	/* Setup the interrupt level and priority for UART0 */
	MCF_INTC0_ICR13 = MCF_INTC_ICR_IL(4) | MCF_INTC_ICR_IP(4);
		
	/* Set the interrupt handlers in the vector table */
	mcf5xxx_set_handler(64 + 13, (ADDRESS)uart0_handler);	
	
	//unmask DMA0~DMA3 interrupts in INTC0
	MCF_INTC0_IMRL &= ~(0
					 | MCF_INTC_IMRL_INT_MASK9 
					 | MCF_INTC_IMRL_INT_MASK10 
					 | MCF_INTC_IMRL_INT_MASK11
					 | MCF_INTC_IMRL_INT_MASK12
					 | MCF_INTC_IMRL_MASKALL);
	
	/* Setup the interrupt level and priority for DMA0~DMA3 */
	MCF_INTC0_ICR09 = MCF_INTC_ICR_IL(3) | MCF_INTC_ICR_IP(4);
	MCF_INTC0_ICR10 = MCF_INTC_ICR_IL(3) | MCF_INTC_ICR_IP(5);
	MCF_INTC0_ICR11 = MCF_INTC_ICR_IL(3) | MCF_INTC_ICR_IP(6);
	MCF_INTC0_ICR12 = MCF_INTC_ICR_IL(3) | MCF_INTC_ICR_IP(7);
		
	/* Set the interrupt handlers in the vector table */
	mcf5xxx_set_handler(64 + 9, (ADDRESS)dma0_handler);	
	mcf5xxx_set_handler(64 + 10, (ADDRESS)dma1_handler);	
	mcf5xxx_set_handler(64 + 11, (ADDRESS)dma2_handler);	
	mcf5xxx_set_handler(64 + 12, (ADDRESS)dma3_handler);	
	
	//unmask dtim0~3 interrupts in INTC0
	MCF_INTC0_IMRL &= ~(MCF_INTC_IMRL_INT_MASK19);
    MCF_INTC0_IMRL &= ~(MCF_INTC_IMRL_INT_MASK20);
	MCF_INTC0_IMRL &= ~(MCF_INTC_IMRL_INT_MASK21);
	MCF_INTC0_IMRL &= ~(MCF_INTC_IMRL_INT_MASK22);
	
    /* Setup the interrupt level and priority for dtim0~3 */
	MCF_INTC0_ICR19 = MCF_INTC_ICR_IL(2) | MCF_INTC_ICR_IP(4);
	MCF_INTC0_ICR20 = MCF_INTC_ICR_IL(2) | MCF_INTC_ICR_IP(5);
	MCF_INTC0_ICR21 = MCF_INTC_ICR_IL(2) | MCF_INTC_ICR_IP(6);
	MCF_INTC0_ICR22 = MCF_INTC_ICR_IL(2) | MCF_INTC_ICR_IP(7);
	
 	/* Set the interrupt handlers in the vector table */
    mcf5xxx_set_handler(64 + 19, (ADDRESS)dtim0_isr);	
    mcf5xxx_set_handler(64 + 20, (ADDRESS)dtim1_isr);	
    mcf5xxx_set_handler(64 + 21, (ADDRESS)dtim2_isr);	
    mcf5xxx_set_handler(64 + 22, (ADDRESS)dtim3_isr);	

	
    /* Global interrupt enable */

	printf("DMA Timer Example for MCF5225x\n");
	
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
	
					
