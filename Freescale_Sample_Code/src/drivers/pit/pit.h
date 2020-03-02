/*
 * File:    pit.h
 * Purpose: Header for the Programmable Interrupt Timer Driver
 * 
 * License:     All software covered by license agreement in -
 *              docs/Freescale_Software_License.pdf
 */
 
#ifndef _PIT_H_
#define _PIT_H_

#include "common.h"
#define MCF521x


/*
 * Constants
 */
#define PIT_COUNTDOWN 0x00
#define PIT_PERIODIC 0x01

#define PIT_IDLE 0x00
#define PIT_COUNTING 0x01
#define PIT_DONE 0x02

typedef void (* pit_isr) (void);

/*
 * PIT Macros
 */
#define PIT_ENABLE(MOD)  (MCF_PIT_PCSR(MOD) |= MCF_PIT_PCSR_EN)
#define PIT_DISABLE(MOD)  (MCF_PIT_PCSR(MOD) &= ~MCF_PIT_PCSR_EN)

/********************************************************************/
/*
 * Functions provided by this driver
 */
int8 PITInit(uint8 u8Prescaler, uint16 u16Modulus, uint8 u8PITModule);
void PIT_EnableInts(pit_isr pit_isr_ptr, uint8 u8PITModule);
int8 PIT_CountDown(uint32 u32FreqHz, uint8 u8PITModule);
void PIT_DisableInts(uint8 u8PITModule);
void PIT_Start(uint8 u8PITModule);
void PIT_Stop(uint8 u8PITModule);

uint32 PIT_SetPeriod (uint32 u32Periodms, uint8 u8PITModule);
uint32 PIT_SetFreq (uint32 u32FreqKhz, uint8 u8PITModule);

int8 PIT_Overwrite(uint16 u16Initial,uint16 u16Threshold,uint8 u8PITModule);
int8 PIT_Dozemode(uint8 u8PITModule);
int8 PIT_Debugmode(uint8 u8PITModule);

__interrupt__ void pit0_isr();
__interrupt__ void pit1_isr();

#endif /* _PIT_H_ */
