/*
 * File:    gpt.h
 * Purpose: Driver for the on-chip Clock module
 *
 * Notes:
 * 
 * License:     All software covered by license agreement in -
 *              docs/Freescale_Software_License.pdf
 */

#ifndef _GPT_H_	
#define _GPT_H_	

#include "common.h"

/*
 * Constants
 */
#define X                  0
#define NO_USE             X
#define LIMIT              65535


/*
 * GPT Macros
 */
#define GPT_START_TIMER()   MCF_GPT_GPTSCR1 |= MCF_GPT_GPTSCR1_GPTEN
#define GPT_STOP_TIMER()    MCF_GPT_GPTSCR1 &= ~MCF_GPT_GPTSCR1_GPTEN
#define GPT_EN_RESET_CH3()  MCF_GPT_GPTSCR2 |= MCF_GPT_GPTSCR2_TCRE
#define GPT_DIS_RESET_CH3() MCF_GPT_GPTSCR2 &= ~MCF_GPT_GPTSCR2_TCRE
#define GPT_START_PULSEACC()   MCF_GPT_GPTPACTL |= MCF_GPT_GPTPACTL_PAE
#define GPT_STOP_PULSEACC()   MCF_GPT_GPTPACTL &= ~MCF_GPT_GPTPACTL_PAE
#define GPT_EN_TOV(CH)		MCF_GPT_GPTTOV |= (1<<CH)
#define GPT_DIS_TOV(CH)		MCF_GPT_GPTTOV &= ~(1<<CH)



/********************************************************************/
/*
 * Functions provided by this driver
 */
void GPTAInitResetState(void);
void GPTInitPulseMeasurement(uint8 u8Prescaler);
void GPTInitCycleMeasurement(uint8 u8Prescaler);
int8 GPTStartPulseMeasurement(void);
uint32 GPTGetMeasurement(void);
uint32 GPTGetMeasurement_Poll(void);

void GPTInitPulseAcc_Counter(void);
int8 GPTStartPulseAccCounter(void);
uint32 GPTGetPulseAccCounter(void);

void GPTInitOC(uint16 u16Counter, uint8 u8Channel, uint8 u8Mode);

void GPT_TOF_Pulse_Meas(void);
void GPT_C3F_Pulse_Meas(void);
void GPT_PAIF_Pulse_Acc_Counter(void);
void GPT_PAOVF_Pulse_Acc_Counter(void);

__interrupt__ void GPT_PAIF();
__interrupt__ void GPT_PAOVF();
__interrupt__ void GPT_TOF();
__interrupt__ void GPT_C3F();

#endif /*_GPT_H_*/

