/*
 * File:        dma.h
 * Purpose:     Provide common ColdFire DMA routines
 *
 * Notes:       
 *              
 * License:     All software covered by license agreement in -
 *              docs/Freescale_Software_License.pdf
 */


#ifndef __DMA_H__
#define __DMA_H__

typedef struct __dma_info 
{
	uint32 srcadd;
	uint32 destadd;
	uint32 bytecnt;
	uint32 ctrl;
	uint8 status;
} DMA_Info;

volatile extern uint8 int_status;

void DMA_init(uint8 ch, DMA_Info *info);
void DMA_start(uint8 ch, DMA_Info *info);
__interrupt__ void dma0_handler(void);
__interrupt__ void dma1_handler(void);
__interrupt__ void dma2_handler(void);
__interrupt__ void dma3_handler(void);


#endif /* __DMA_H */


