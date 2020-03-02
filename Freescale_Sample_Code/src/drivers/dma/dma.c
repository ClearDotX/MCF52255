/*
 * File:        dma.c
 * Purpose:     Provide common ColdFire DMA routines
 *
 * Notes:       
 *              
 * License:     All software covered by license agreement in -
 *              docs/Freescale_Software_License.pdf
 */

#include "common.h"
#include "dma/dma.h"

volatile uint8 int_status = 0x0;

/*
*	Initialize DMA channel with DMA_Info
*	
*	Parameters:
*	ch 	DMA channel to initialze
*	DMA_Info	contains info such as source addr, dest addr, byte count, etc.
*/
void DMA_init(uint8 ch, DMA_Info *info)
{
	//source address register
	MCF_DMA_SAR(ch) = (uint32)info->srcadd;	    
	//destination address register
	MCF_DMA_DAR(ch) = (uint32)info->destadd;	           
	//byte count
	MCF_DMA_BCR(ch) = info->bytecnt;
	MCF_DMA_DCR(ch) = info->ctrl;	
#if 0	
	printf("MCF_DMA_SAR is 0x%x\n",MCF_DMA_SAR(ch));
	printf("MCF_DMA_DAR is 0x%x\n",MCF_DMA_DAR(ch));
	printf("MCF_DMA_BCR is 0x%x\n",MCF_DMA_BCR(ch));
	printf("MCF_DMA_DCR is 0x%x\n",MCF_DMA_DCR(ch));
#endif
}


/*
* Start DMA channel
*/
void DMA_start(uint8 ch, DMA_Info *info)
{
	MCF_DMA_DCR(ch) |= info->ctrl;
}

/*
*	DMA0 Interrupt Service routine 
*/
__interrupt__ void dma0_handler(void)
{	
	int_status = MCF_DMA0_DSR;
	if(int_status & MCF_DMA_DSR_DONE)
	{
		if(int_status & MCF_DMA_DSR_CE)
		{
			printf("configuration error\n");
		}
		else if(int_status & MCF_DMA_DSR_BED)
		{
			printf("destination bus error\n");
		}
		else if(int_status & MCF_DMA_DSR_BES)
		{
			printf("source bus error\n");
		}
		else
		{	
			printf("dma0 transfer done\n");
		}
		//clear DMA0 interrupt
		MCF_DMA0_DSR |= MCF_DMA_DSR_DONE;		
	}
	if(int_status & MCF_DMA_DSR_BSY)
	{
		printf("busy\n");
	}
	if(int_status & MCF_DMA_DSR_REQ)
	{
		printf("transfer remaining but channel not selected\n");
	}
}
/*
*	DMA1 Interrupt Service routine 
*/
__interrupt__ void dma1_handler(void)
{
	int_status = MCF_DMA1_DSR;

	if(int_status & MCF_DMA_DSR_DONE)
	{
		if(int_status & MCF_DMA_DSR_CE)
		{
			printf("configuration error\n");
		}
		else if(int_status & MCF_DMA_DSR_BED)
		{
			printf("destination bus error\n");
		}
		else if(int_status & MCF_DMA_DSR_BES)
		{
			printf("source bus error\n");
		}
		else
		{	
			printf("dma1 transfer done\n");
		}
		//clear DMA1 interrupt
		MCF_DMA1_DSR |= MCF_DMA_DSR_DONE;		
	}
	if(int_status & MCF_DMA_DSR_BSY)
	{
		printf("busy\n");
	}
	if(int_status & MCF_DMA_DSR_REQ)
	{
		printf("transfer remaining but channel not selected\n");
	}
}

/*
*	DMA2 Interrupt Service routine 
*/
__interrupt__ void dma2_handler(void)
{
	int_status = MCF_DMA2_DSR;
	
	if(int_status & MCF_DMA_DSR_DONE)
	{
		if(int_status & MCF_DMA_DSR_CE)
		{
			printf("configuration error\n");
		}
		else if(int_status & MCF_DMA_DSR_BED)
		{
			printf("destination bus error\n");
		}
		else if(int_status & MCF_DMA_DSR_BES)
		{
			printf("source bus error\n");
		}
		else
		{	
			printf("dma2 transfer done\n");
		}
		//clear DMA2 interrupt
		MCF_DMA2_DSR |= MCF_DMA_DSR_DONE;		
	}
	if(int_status & MCF_DMA_DSR_BSY)
	{
		printf("busy\n");
	}
	if(int_status & MCF_DMA_DSR_REQ)
	{
		printf("transfer remaining but channel not selected\n");
	}
}

/*
*	DMA3 Interrupt Service routine 
*/
__interrupt__ void dma3_handler(void)
{
	int_status = MCF_DMA3_DSR;
	
	if(int_status & MCF_DMA_DSR_DONE)
	{
		if(int_status & MCF_DMA_DSR_CE)
		{
			printf("configuration error\n");
		}
		else if(int_status & MCF_DMA_DSR_BED)
		{
			printf("destination bus error\n");
		}
		else if(int_status & MCF_DMA_DSR_BES)
		{
			printf("source bus error\n");
		}
		else
		{	
			printf("dma3 transfer done\n");
		}
		//clear DMA3 interrupt
		MCF_DMA3_DSR |= MCF_DMA_DSR_DONE;		
	}
	if(int_status & MCF_DMA_DSR_BSY)
	{
		printf("busy\n");
	}
	if(int_status & MCF_DMA_DSR_REQ)
	{
		printf("transfer remaining but channel not selected\n");
	}
}
