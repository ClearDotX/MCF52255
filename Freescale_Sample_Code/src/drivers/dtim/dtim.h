/*
 * File:    dtim.h
 * Purpose: Header for the DMA Timer Driver
 * 
 * License:     All software covered by license agreement in -
 *              docs/Freescale_Software_License.pdf
 */
 
#ifndef __DTIM_H
#define __DTIM_H

#include "common.h"

typedef void (* dtim_isr) (void);

typedef struct __DTIM_Info
{
	uint8 u8OutputMode;	//active low or toggle output
	uint8 u8Prescaler;	//input clk divider
	uint8 u8CapEdge;	//rising, falling or both edge capture
	uint8 u8ClkSrc;		//sysclk, sysclk/16 or DTINn input
	uint8 u8Restart;	//freerun or restart
	uint8 u8Halt;		//timer stop while core halted
	uint8 u8Mode16;		//increment timer by 1 or by 65537
	uint8 u8DMAEn;		//DMA request enable
	uint32 u32Reference;
	uint32 u32CapValue;
}DTIM_Info;

extern uint32 diff;

int8 dtim_init(DTIM_Info info, uint8 u8DTimModule);
void dtim_start(uint8 u8DTimModule);
void DTimEnableInts(uint8 u8DTimModule, dtim_isr isr);
void DTimDisableInts(uint8 u8DTimModule);

__interrupt__ void dtim0_isr();
__interrupt__ void dtim1_isr();
__interrupt__ void dtim2_isr();
__interrupt__ void dtim3_isr();


#endif /* __DTIM_H */

