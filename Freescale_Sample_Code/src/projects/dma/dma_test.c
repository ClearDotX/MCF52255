
/*
 * File:		dma_test.c
 * Purpose:		DMA Examples
 *
 * License:     All software covered by license agreement in -
 *              docs/Freescale_Software_License.pdf
 */

#include "common.h"
#include "uart.h"
#include "dma.h"
#include "dma_example.h"

typedef int8 (* example_fuct) (void);

/*
 * Available functions
 */
example_fuct example_functions[]={
						sram_dma_test,
						smod_dmod_test,
						dma_extreq,
						channel_link						   	
						   };


/********************************************************************/
void 
main (void)
{
	uint8 i;
	uint8 u8Continue =0;

	//unmask DMA0~DMA3 interrupts in INTC0
	MCF_INTC0_IMRL &= ~(0
					 | MCF_INTC_IMRL_INT_MASK9 
					 | MCF_INTC_IMRL_INT_MASK10 
					 | MCF_INTC_IMRL_INT_MASK11
					 | MCF_INTC_IMRL_INT_MASK12
					 | MCF_INTC_IMRL_MASKALL);
	
	/* Setup the interrupt level and priority for DMA0~DMA3 */
	MCF_INTC0_ICR09 = MCF_INTC_ICR_IL(4) | MCF_INTC_ICR_IP(4);
	MCF_INTC0_ICR10 = MCF_INTC_ICR_IL(4) | MCF_INTC_ICR_IP(5);
	MCF_INTC0_ICR11 = MCF_INTC_ICR_IL(4) | MCF_INTC_ICR_IP(6);
	MCF_INTC0_ICR12 = MCF_INTC_ICR_IL(4) | MCF_INTC_ICR_IP(7);
		
	/* Set the interrupt handlers in the vector table */
	mcf5xxx_set_handler(64 + 9, (ADDRESS)dma0_handler);	
	mcf5xxx_set_handler(64 + 10, (ADDRESS)dma1_handler);	
	mcf5xxx_set_handler(64 + 11, (ADDRESS)dma2_handler);	
	mcf5xxx_set_handler(64 + 12, (ADDRESS)dma3_handler);	

	/* Enable DMA master access in both supervisor and user mode */
	MCF_SCM_MPR = MCF_SCM_MPR_MPR(0x05);
			
	/* Global interrupt enable */
	
	printf("DMA Example for MCF5225x\n");
	 	
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
	
