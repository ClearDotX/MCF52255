/*
 * File:    gpt.c
 * Purpose: Driver for the General Purpose Timer      
 * 
 * License:     All software covered by license agreement in -
 *              docs/Freescale_Software_License.pdf
 */
#include "gpt.h"

void (* GPT_TOF_ISR) (void);
void (* GPT_C3_ISR) (void);
void (* GPT_PAIF_ISR) (void);
void (* GPT_PAOVF_ISR) (void);

enum tPulseSemaphore 
{
    SEMAPHORE_FREE = 0,
    SEMAPHORE_1,
    SEMAPHORE_2,
    SEMAPHORE_RDY,
    SEMAPHORE_ERR
};

/*
 * Initialize Variables     
 */
uint8 gu8GPTOverflows = 0;
enum tPulseSemaphore geSemaphore  = SEMAPHORE_FREE;
uint32 gu32PulseMeasure = 0;
uint16 gu16PulseAccCounter = 0;
uint16 gu16PulseAccCounterOVF = 0;



/********************************************************************/
/* 
 * Initialize GPT in Reset State
 */
void GPTAInitResetState(void)
{
    MCF_GPT_GPTSCR1 = 0x00;
    MCF_GPT_GPTPORT = 0x00;
    MCF_GPT_GPTDDR = 0x00;
    MCF_GPT_GPTIOS = 0x00;
    MCF_GPT_GPTCTL1 = 0x00;   
    MCF_GPT_GPTCTL2 = 0x00;
    MCF_GPT_GPTIE = 0x00;
    MCF_GPT_GPTSCR2 = 0x00;
    MCF_GPT_GPTPACTL = 0x00;
    MCF_GPT_GPTTOV = 0x00;
    
    return;
}
/********************************************************************/
/* 
 * Initialize GPT to measure a pulse
 */
void GPTInitPulseMeasurement(uint8 u8Prescaler)
{

    MCF_GPT_GPTIOS &= ~MCF_GPT_GPTIOS_IOS3;
    MCF_GPT_GPTSCR1 = 0;
    MCF_GPT_GPTCTL2 = MCF_GPT_GPTCTL2_EDG3A|MCF_GPT_GPTCTL2_EDG3B;
    MCF_GPT_GPTIE = MCF_GPT_GPTIE_CI3;
    MCF_GPT_GPTSCR2 = MCF_GPT_GPTSCR2_TOI | MCF_GPT_GPTSCR2_PR(u8Prescaler);
    GPT_TOF_ISR = GPT_TOF_Pulse_Meas;
    GPT_C3_ISR = GPT_C3F_Pulse_Meas;
    MCF_GPIO_PTAPAR &= ~(MCF_GPIO_PTAPAR_PTAPAR3(3));
    MCF_GPIO_PTAPAR |= (MCF_GPIO_PTAPAR_PTAPAR3(1));
    
    /* Set interrupts */
    MCF_INTC0_ICR41 = MCF_INTC_ICR_IL(3)|MCF_INTC_ICR_IP(3);
    MCF_INTC0_ICR47 = MCF_INTC_ICR_IL(3)|MCF_INTC_ICR_IP(4);
    
    MCF_INTC0_IMRH &= ~(MCF_INTC_IMRH_INT_MASK41| MCF_INTC_IMRH_INT_MASK47);
    MCF_INTC0_IMRL &= ~(MCF_INTC_IMRL_MASKALL);
    
    return;
}
/********************************************************************/
/* 
 * Initialize GPT Channel for Output Compare
 */
void GPTInitOC(uint16 u16Counter, uint8 u8Channel, uint8 u8Mode)
{
   uint16 *ptr;

	MCF_GPT_GPTCTL1 &= ~(3<<(u8Channel*2));
	MCF_GPT_GPTCTL1 |= (u8Mode<<(u8Channel*2));
	MCF_GPT_GPTIOS |= MCF_GPT_GPTIOS_IOS(1<<u8Channel);
	ptr = (uint16*) (&MCF_GPT_GPTC0 + u8Channel);
	*ptr = u16Counter;
}
/********************************************************************/
/* 
 * Initialize GPT as a pulse accumulator counter
 */
void GPTInitPulseAcc_Counter(void)
{

    MCF_GPT_GPTIOS = MCF_GPT_GPTIOS_IOS(0x00);
    MCF_GPT_GPTSCR1 = MCF_GPT_GPTSCR1_TFFCA;
    MCF_GPT_GPTCTL1 = 0x00;
    MCF_GPT_GPTCTL2 = 0x00;
    
    MCF_GPT_GPTPACTL = 0x00;
    
    MCF_GPIO_PTAPAR &= ~(MCF_GPIO_PTAPAR_PTAPAR3(3));
    MCF_GPIO_PTAPAR |= (MCF_GPIO_PTAPAR_PTAPAR3(1));
    
    return;
}
/********************************************************************/
/* 
 * Initialize GPT to measure a cycle (2 rising edges)
 */
void GPTInitCycleMeasurement(uint8 u8Prescaler)
{

    MCF_GPT_GPTIOS = MCF_GPT_GPTIOS_IOS(0x00);
    MCF_GPT_GPTSCR1 = 0;
    MCF_GPT_GPTCTL2 = MCF_GPT_GPTCTL2_EDG3A;
    MCF_GPT_GPTIE = MCF_GPT_GPTIE_CI3;
    MCF_GPT_GPTSCR2 = MCF_GPT_GPTSCR2_TOI | MCF_GPT_GPTSCR2_PR(u8Prescaler);
    GPT_TOF_ISR = GPT_TOF_Pulse_Meas;
    GPT_C3_ISR = GPT_C3F_Pulse_Meas;
    MCF_GPIO_PTAPAR = MCF_GPIO_PTAPAR_PTAPAR3(1);
    
    /* Set interrupts */
    MCF_INTC0_ICR41 = MCF_INTC_ICR_IL(3)|MCF_INTC_ICR_IP(3);
    MCF_INTC0_ICR47 = MCF_INTC_ICR_IL(3)|MCF_INTC_ICR_IP(4);
    
    MCF_INTC0_IMRH &= ~(MCF_INTC_IMRH_INT_MASK41| MCF_INTC_IMRH_INT_MASK47);
    MCF_INTC0_IMRL &= ~(MCF_INTC_IMRL_MASKALL);
    
    
    return;
}
/********************************************************************/
/* 
 * Start pulse accumulator
 */
int8 GPTStartPulseAccCounter(void)
{
    if (geSemaphore != SEMAPHORE_FREE)
        return -1;
    geSemaphore = SEMAPHORE_1;
    gu16PulseAccCounter = 0;
    gu16PulseAccCounterOVF = 0;
    GPT_START_PULSEACC(); /* Enable Pulse accumulator */
    return 0;
}
/********************************************************************/
/* 
 * Get value of pulse accumulator
 */
uint32 GPTGetPulseAccCounter(void)
{
    static uint16 u16PrevValue = 0;
    uint32 u32TotalCount = 0;
    
    if (geSemaphore != SEMAPHORE_1)
        return 0;
    GPT_STOP_PULSEACC(); /* Disable Pulse accumulator */
    
    
    if (MCF_GPT_GPTPACNT >= u16PrevValue)
    {
        u32TotalCount = (MCF_GPT_GPTPACNT - u16PrevValue);    
    }
    else
    {
        u32TotalCount = (0x10000 + MCF_GPT_GPTPACNT - u16PrevValue);
    }
    
    u16PrevValue = MCF_GPT_GPTPACNT;
    gu16PulseAccCounter = 0;
    gu16PulseAccCounterOVF = 0;
    
    geSemaphore = SEMAPHORE_FREE;
    return u32TotalCount;
}

/********************************************************************/
/* 
 * Start Pulse Measurement
 */
int8 GPTStartPulseMeasurement(void)
{
    if (geSemaphore != SEMAPHORE_FREE)
        return -1;
   
    gu8GPTOverflows = 0;
    gu32PulseMeasure = 0;
    MCF_GPT_GPTFLG1 |= MCF_GPT_GPTFLG1_CF3;
    MCF_GPT_GPTFLG2 |= MCF_GPT_GPTFLG2_TOF;
    geSemaphore = SEMAPHORE_1;
    MCF_GPT_GPTSCR2 &= ~MCF_GPT_GPTSCR2_TCRE;
    GPT_START_TIMER();
    
    return 0;
}

/********************************************************************/
/* 
 * Get Measurement
 */
uint32 GPTGetMeasurement(void)
{   
    while (geSemaphore != SEMAPHORE_RDY)
    {
        if (geSemaphore == SEMAPHORE_ERR)
        return 0;
    };
    
    geSemaphore = SEMAPHORE_FREE; 
    MCF_GPT_GPTSCR2 &= ~MCF_GPT_GPTSCR2_TCRE;
    return gu32PulseMeasure;
}
 
/********************************************************************/
/* 
 * Get Measurement
 */ 
uint32 GPTGetMeasurement_Poll(void)
{
    if (geSemaphore!= SEMAPHORE_RDY)
        return 0;
    
    geSemaphore = SEMAPHORE_FREE; 
    MCF_GPT_GPTSCR2 &= ~MCF_GPT_GPTSCR2_TCRE;
    return gu32PulseMeasure;
}

 
/********************************************************************/
/* 
 * GPT Timer Overflow Counter
 */ 
void GPT_TOF_Pulse_Meas(void)
{
    gu8GPTOverflows++;
 	return;	
}
/********************************************************************/
/* 
 * GPT Timer Overflow on Timer Channel 3
 */
void GPT_C3F_Pulse_Meas(void)
{
    static uint32 u32Offset;
    
    
	switch(geSemaphore){
	    case SEMAPHORE_1:
	    	u32Offset = MCF_GPT_GPTC3;
	    	gu8GPTOverflows = 0;
	    	//MCF_GPT_GPTSCR2 |= MCF_GPT_GPTSCR2_TCRE;
	    	geSemaphore = SEMAPHORE_2;
	    	break;
	    case SEMAPHORE_2:
        	gu32PulseMeasure = MCF_GPT_GPTC3 + (gu8GPTOverflows * 0x10000);
        	gu32PulseMeasure -= u32Offset;
        	GPT_STOP_TIMER();
        	MCF_GPT_GPTFLG1 |= MCF_GPT_GPTFLG1_CF3;
        	MCF_GPT_GPTFLG2 |= MCF_GPT_GPTFLG2_TOF;
        	geSemaphore = SEMAPHORE_RDY;
	    	break;
	    default:
        	GPT_STOP_TIMER();
        	geSemaphore = SEMAPHORE_ERR;
	    	break;
	}
	return;	
}

/********************************************************************/
/* 
 * GPT Timer Overflow
 */
void GPT_PAIF_Pulse_Acc_Counter(void)
{
    gu16PulseAccCounter++;
 	return;	
}

/********************************************************************/
/* 
 * GPT Timer Overflow
 */
void GPT_PAOVF_Pulse_Acc_Counter(void)
{
    gu16PulseAccCounterOVF++;
	return;	
}

/********************************************************************/
/* 
 * GPT Timer Overflow Interrupt
 */
__interrupt__ void GPT_TOF()
{
   MCF_GPT_GPTFLG2 |= MCF_GPT_GPTFLG2_TOF;
	GPT_TOF_ISR();

	return;	
}

/********************************************************************/
/* 
 * GPT Channel 3 Interrupt
 */
__interrupt__ void GPT_C3F()
{
   MCF_GPT_GPTFLG1 |= MCF_GPT_GPTFLG1_CF3;
   GPT_C3_ISR();
	return;	
}
/********************************************************************/
/* 
 * GPT Pulse Accumulator Interrupt
 */
__interrupt__ void GPT_PAIF()
{
   MCF_GPT_GPTPAFLG |= MCF_GPT_GPTPAFLG_PAIF;
	GPT_PAIF_ISR();

	return;	
}
/********************************************************************/
/* 
 * GPT Pulse Accumulator Overflow Interrupt
 */
__interrupt__ void GPT_PAOVF()
{
    MCF_GPT_GPTPAFLG |= MCF_GPT_GPTPAFLG_PAOVF;
    GPT_PAOVF_ISR();
	return;	
}