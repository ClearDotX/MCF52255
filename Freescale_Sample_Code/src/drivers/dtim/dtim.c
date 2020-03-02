/*
 * File:    dtim.c
 * Purpose: Source for the DMA Timer Driver
 * 
 * License:     All software covered by license agreement in -
 *              docs/Freescale_Software_License.pdf
 */

#include "dtim.h"

dtim_isr dtim_example_isr[4];

static uint32 oldval = 0, newval = 0;
uint32 diff = 0;

/* DMA Timer Initialization */
int8 dtim_init(DTIM_Info info, uint8 u8DTimModule)
{
	//timer mode reg
	MCF_DTIM_DTMR(u8DTimModule) = MCF_DTIM_DTMR_CE(info.u8CapEdge)
					| MCF_DTIM_DTMR_CLK(info.u8ClkSrc);
	if(info.u8OutputMode)	//toggle output
		MCF_DTIM_DTMR(u8DTimModule) |= MCF_DTIM_DTMR_OM;
	else
		MCF_DTIM_DTMR(u8DTimModule) &= ~MCF_DTIM_DTMR_OM;
	if(info.u8Restart)	//restart
		MCF_DTIM_DTMR(u8DTimModule) |= MCF_DTIM_DTMR_FRR;
	else
		MCF_DTIM_DTMR(u8DTimModule) &= ~MCF_DTIM_DTMR_FRR;

	//Timer extended register
	if(info.u8DMAEn)	//DMA request enable
		MCF_DTIM_DTXMR(u8DTimModule) |=  MCF_DTIM_DTXMR_DMAEN;	
	else
		MCF_DTIM_DTXMR(u8DTimModule) &=  ~MCF_DTIM_DTXMR_DMAEN;	
	if(info.u8Halt)	//timer stop in halt
		MCF_DTIM_DTXMR(u8DTimModule) |=  MCF_DTIM_DTXMR_HALTED;	
	else
		MCF_DTIM_DTXMR(u8DTimModule) &=  ~MCF_DTIM_DTXMR_HALTED;	
	if(info.u8Mode16)
		MCF_DTIM_DTXMR(u8DTimModule) |=  MCF_DTIM_DTXMR_MODE16;
	else
		MCF_DTIM_DTXMR(u8DTimModule) &=  ~MCF_DTIM_DTXMR_MODE16;

	return 0;
}


void dtim_start(uint8 u8DTimModule)
{
	MCF_DTIM_DTMR(u8DTimModule)  |= MCF_DTIM_DTMR_RST;
}

void DTimEnableInts(uint8 u8DTimModule, dtim_isr isr)
{
	MCF_DTIM_DTMR(u8DTimModule) |= MCF_DTIM_DTMR_ORRI;
	dtim_example_isr[u8DTimModule] = isr;
}


void DTimDisableInts(uint8 u8DTimModule)
{
	MCF_DTIM_DTMR(u8DTimModule) &= ~MCF_DTIM_DTMR_ORRI;
}

__interrupt__ void dtim0_isr()
{
	uint8 status = MCF_DTIM0_DTER;
	
	if(status&MCF_DTIM_DTER_REF)
		MCF_DTIM0_DTER |= MCF_DTIM_DTER_REF;			
	else if(status&MCF_DTIM_DTER_CAP)
	{
		MCF_DTIM0_DTER |= MCF_DTIM_DTER_CAP;
		newval = MCF_DTIM0_DTCR;
		diff = newval - oldval;
		oldval = newval;
	}
	dtim_example_isr[0]();
}

__interrupt__ void dtim1_isr()
{
	uint8 status = MCF_DTIM1_DTER;
	
	if(status&MCF_DTIM_DTER_REF)
    	MCF_DTIM1_DTER |= MCF_DTIM_DTER_REF;			
	else if(status&MCF_DTIM_DTER_CAP)
    	MCF_DTIM1_DTER |= MCF_DTIM_DTER_CAP;
	
	dtim_example_isr[1]();
}

__interrupt__ void dtim2_isr()
{
	uint8 status = MCF_DTIM2_DTER;
	
	if(status&MCF_DTIM_DTER_REF)
    	MCF_DTIM2_DTER |= MCF_DTIM_DTER_REF;			
	else if(status&MCF_DTIM_DTER_CAP)
    	MCF_DTIM2_DTER |= MCF_DTIM_DTER_CAP;
	dtim_example_isr[2]();
}

__interrupt__ void dtim3_isr()
{
	uint8 status = MCF_DTIM3_DTER;
	
	if(status&MCF_DTIM_DTER_REF)
    	MCF_DTIM3_DTER |= MCF_DTIM_DTER_REF;			
	else if(status&MCF_DTIM_DTER_CAP)
    	MCF_DTIM3_DTER |= MCF_DTIM_DTER_CAP;
	dtim_example_isr[3]();
}



