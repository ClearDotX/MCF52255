/*
 * File:    pit.c
 * Purpose: Driver for the Programmable Interrupt Timer     
 * 
 * License:     All software covered by license agreement in -
 *              docs/Freescale_Software_License.pdf
 */

#include "pit.h"
#include "common.h"


/*
 * Initialize Variables     
 */
pit_isr pit_example_isr[2];
static uint32 pow(uint8 u8Pow2);
static void pit_countdown_isr(void);

uint8 u8PITSemaphore = PIT_IDLE;
uint8 u8CountdownPIT = 0;


/********************************************************************/
/* 
 * Initialize PIT
 *
 *	1. Disable PIT Module 
 *  2. Set Prescaler
 *  3. Set Modulus 
 *  4. Set Reload according to mode
 */
int8 PITInit(uint8 u8Prescaler, uint16 u16Modulus, uint8 u8PITModule)
{
	
	MCF_PIT_PCSR(u8PITModule) = 0x00 | MCF_PIT_PCSR_PRE(u8Prescaler);
	MCF_PIT_PMR(u8PITModule) = u16Modulus;
	
    /* Enable reload for periodic mode */
    MCF_PIT_PCSR(u8PITModule) |= MCF_PIT_PCSR_RLD;

    /* Enable Interrupts for PIT0 (55) and PIT1(56) */
    MCF_INTC0_IMRH &= ~(MCF_INTC_IMRH_INT_MASK55|MCF_INTC_IMRH_INT_MASK56);
    MCF_INTC0_IMRL &= ~(MCF_INTC_IMRL_MASKALL);
    MCF_INTC0_ICR55 = MCF_INTC_ICR_IL(3)|MCF_INTC_ICR_IP(4);
    MCF_INTC0_ICR56 = MCF_INTC_ICR_IL(3)|MCF_INTC_ICR_IP(5);
    
    return 0;	
}
/********************************************************************/
/* 
 * Enable PIT Interrupts and set ISR
 */
void PIT_EnableInts(pit_isr pit_isr_ptr, uint8 u8PITModule)
{
    
    MCF_PIT_PCSR(u8PITModule) |= MCF_PIT_PCSR_PIE;
    pit_example_isr[u8PITModule] = pit_isr_ptr;
}
/********************************************************************/
/* 
 * Disable PIT Interrupts
 */
void PIT_DisableInts(uint8 u8PITModule)
{
    MCF_PIT_PCSR(u8PITModule) &= ~MCF_PIT_PCSR_PIE;
}
/********************************************************************/
/* 
 * Enable PIT
 */
void PIT_Start(uint8 u8PITModule)
{
    PIT_ENABLE(u8PITModule);
}
/********************************************************************/
/* 
 * Stop PIT
 */
void PIT_Stop(uint8 u8PITModule)
{
    PIT_DISABLE(u8PITModule);
}

/********************************************************************/
/* 
 * PIT Countdown
 *   Takes a time in milliseconds, and then creates a countdown timer
 *   to execute pit_countdown_isr when the counter reaches zero.
 *
 * Parameters:
 *  u32Period	Time in milliseconds to countdown
 *  u8PITModule	The PIT module (0 or 1) to use
 *
 * Return Values:
 *  0 for OK
 */
int8 PIT_CountDown(uint32 u32Period, uint8 u8PITModule)
{
	pit_isr pit_temp_isr;
	
	/* Backup the ISR */
	pit_temp_isr = pit_example_isr[u8PITModule];
	
	/* Disable reloading of the PIT */
    MCF_PIT_PCSR(u8PITModule) &= ~MCF_PIT_PCSR_RLD;
    
    /* Enable overwrite */
    MCF_PIT_PCSR(u8PITModule) |= MCF_PIT_PCSR_OVW;

    if ( PIT_SetPeriod (u32Period,u8PITModule) == -1){
        return -1;
    }
	PIT_EnableInts(pit_countdown_isr, u8PITModule);
	u8CountdownPIT = u8PITModule;
	
	u8PITSemaphore = PIT_COUNTING;
	PIT_Start(u8PITModule);

    
    /* Wait until countdown finishes */
    while (u8PITSemaphore != PIT_DONE);
    
    u8PITSemaphore = PIT_IDLE;
    
	/* Reload the original ISR */
	pit_example_isr[u8PITModule] = pit_temp_isr;
    
    return 0;	  
}


/********************************************************************/
/* 
 * PIT Countdown ISR
 */
static void pit_countdown_isr(void)
{
    u8PITSemaphore = PIT_DONE;
    MCF_PIT_PCSR(u8CountdownPIT) &= ~MCF_PIT_PCSR_EN;
}

/********************************************************************/
/* 
 * Set PIT Frequency in Hz
 */
uint32 PIT_SetFreq (uint32 u32FreqHz, uint8 u8PITModule) 
{
    uint8 i;
    uint32 u32Modulus;
    
    for (i=0; i < 16; i++){
        u32Modulus = (SYS_CLK_KHZ*1000/2) / (u32FreqHz*pow(i));
        if (u32Modulus < 65535){
            /* Try to find a good prescaler */
            MCF_PIT_PCSR(u8PITModule) &= ~MCF_PIT_PCSR_PRE(0x0F);
            MCF_PIT_PCSR(u8PITModule) |= MCF_PIT_PCSR_PRE(i);
            MCF_PIT_PMR(u8PITModule) = u32Modulus;
            return ((SYS_CLK_KHZ*1000/2) / (pow(i)*u32Modulus));
        }
    }
    return -1;
}

/********************************************************************/
/* 
 * Set PIT Period in milliseconds
 */
uint32 PIT_SetPeriod (uint32 u32Periodms, uint8 u8PITModule) 
{
    uint8 i;
    uint32 u32Modulus;
    
    for (i=0; i < 16; i++){
        u32Modulus = ((SYS_CLK_KHZ/2) * u32Periodms) / pow(i);
        if (u32Modulus < 65535){
            /* Try to find a good prescaler */
            MCF_PIT_PCSR(u8PITModule) &= ~MCF_PIT_PCSR_PRE(0x0F);
            MCF_PIT_PCSR(u8PITModule) |= MCF_PIT_PCSR_PRE(i);
            MCF_PIT_PMR(u8PITModule) = u32Modulus;
            return ((pow(i)*u32Modulus) / (SYS_CLK_KHZ/2));
        }
    }
    return -1;  
}

/********************************************************************/
/* 
 * PIT Overwrite
 * Counter starts from an initial value, every time it is below a threshold, new value
 * (half of initial value) is loaded into PMR, and threshold is also halved, this continues
 * until threshold is one, then no new value is loaded, timer stops
 *
 * Parameters:
 *  u16Initial		Initial value to load into PMR
 *  u16Threshold	Threshold below which an overwrite occurs
 *  u8PITModule	The PIT module (0 or 1) to use
 *
 * Return Values:
 *  0 for OK
 */
int8 PIT_Overwrite(uint16 u16Initial,uint16 u16Threshold,uint8 u8PITModule)
{
	pit_isr pit_temp_isr;
	
	/* Backup the ISR */
	pit_temp_isr = pit_example_isr[u8PITModule];

	/* Load initial value */
	MCF_PIT_PMR(u8PITModule) = u16Initial;
	
	/* Disable reloading of the PIT */
    MCF_PIT_PCSR(u8PITModule) &= ~MCF_PIT_PCSR_RLD;
    
    /* Enable overwrite */
    MCF_PIT_PCSR(u8PITModule) |= MCF_PIT_PCSR_OVW;

    if ( PIT_SetPeriod (5000,u8PITModule) == -1){
        return -1;
    }
	PIT_EnableInts(pit_countdown_isr, u8PITModule);
	u8CountdownPIT = u8PITModule;
	
	u8PITSemaphore = PIT_COUNTING;
	PIT_Start(u8PITModule);
    
    /* Wait until countdown finishes */
    while (u8PITSemaphore != PIT_DONE)
    {
    	printf("MCF_PIT_PCNTR is 0x%x\n", MCF_PIT_PCNTR(u8PITModule));
		if(MCF_PIT_PCNTR(u8PITModule) < u16Threshold)
		{
			u16Initial /= 2;
			u16Threshold /= 2;
			if(u16Threshold != 0x01)
				MCF_PIT_PMR(u8PITModule) = u16Initial;
		}
   	}
 
    u8PITSemaphore = PIT_IDLE;
    
	/* Reload the original ISR */
	pit_example_isr[u8PITModule] = pit_temp_isr;
    
    return 0;	  
}

/************************************************
	* Doze mode test
	PIT starts down-counting from 0x8000, enter Doze mode when current timer value 
	less than 0x4000, PIT counting stops then and waked up from uart interrupt
	
************************************************/
int8 PIT_Dozemode(uint8 u8PITModule)
{
	pit_isr pit_temp_isr;
	uint32 i;
	uint8 test_finish = 0;
	
	/* Backup the ISR */
	pit_temp_isr = pit_example_isr[u8PITModule];
		
	//enable low power stop mode, interrupt request level 3-7 exit low-power mode
	MCF_PMM_LPICR = MCF_PMM_LPICR_ENBSTOP | MCF_PMM_LPICR_XLPM_IPL(0x2);
	MCF_PMM_LPCR = MCF_PMM_LPCR_LPMD_DOZE;	//enter doze mode
	
	MCF_PIT_PCSR(u8PITModule) |= MCF_PIT_PCSR_DOZE; //PIT stopped in doze mode
	MCF_PIT_PCSR(u8PITModule) |= MCF_PIT_PCSR_OVW; //Load PMR value immediately
	MCF_PIT_PMR(u8PITModule) = 0x8000;

    if ( PIT_SetPeriod (5000,u8PITModule) == -1){
        return -1;
    }
	PIT_EnableInts(pit_countdown_isr, u8PITModule);
	u8CountdownPIT = u8PITModule;
	
	u8PITSemaphore = PIT_COUNTING;
	PIT_Start(u8PITModule);

    /* Wait until countdown finishes */
    while (u8PITSemaphore != PIT_DONE)
    {
		printf("MCF_PIT_PCNTR is 0x%x\n", MCF_PIT_PCNTR(u8PITModule));

		if((MCF_PIT_PCNTR(u8PITModule) < 0x6000)&&(!test_finish))
		{	
			//execute stop instruction, enter doze mode now
			stop_2000();		
			test_finish = 1;
		}		
   	}

	printf("MCF_PIT_PCNTR is 0x%x\n", MCF_PIT_PCNTR(u8PITModule));
	
    u8PITSemaphore = PIT_IDLE;
    
	/* Reload the original ISR */
	pit_example_isr[u8PITModule] = pit_temp_isr;
    
    return 0;	  

}

/************************************************
	* Debug mode test	
************************************************/
int8 PIT_Debugmode(uint8 u8PITModule)
{
	pit_isr pit_temp_isr;
	uint8 i;
	
	/* Backup the ISR */
	pit_temp_isr = pit_example_isr[u8PITModule];
		
	MCF_PIT_PCSR(u8PITModule) |= MCF_PIT_PCSR_DBG; //PIT stopped in doze mode
	MCF_PIT_PCSR(u8PITModule) |= MCF_PIT_PCSR_OVW; //Load PMR value immediately
	MCF_PIT_PMR(u8PITModule) = 0x8000;

    if ( PIT_SetPeriod (5000,u8PITModule) == -1){
        return -1;
    }
	PIT_EnableInts(pit_countdown_isr, u8PITModule);
	u8CountdownPIT = u8PITModule;
	
	u8PITSemaphore = PIT_COUNTING;
	PIT_Start(u8PITModule);

    /* Wait until countdown finishes */
    while (u8PITSemaphore != PIT_DONE)
    {
		printf("MCF_PIT_PCNTR is 0x%x\n", MCF_PIT_PCNTR(u8PITModule));

		if(MCF_PIT_PCNTR(u8PITModule) < 0x6000)
		{	
			//set one breakpoints at the for statement and one
			//at the last nop statement
			printf("*********************************************************************\n");
			printf("Set one bp at the for statement and one at the last nop statement\n");
			printf("Wait for some time in between executing the two statement\n");			
			printf("*********************************************************************\n");
			
			printf("MCF_PIT_PCNTR is 0x%x\n", MCF_PIT_PCNTR(u8PITModule));
			for(i = 0; i < 5; i++)
				nop();
			printf("MCF_PIT_PCNTR is 0x%x\n", MCF_PIT_PCNTR(u8PITModule));
			nop();
			printf("Is the printed timer value very close(y/n)\n");
			in_char();
		}		
   	}
	
    u8PITSemaphore = PIT_IDLE;
    
	/* Reload the original ISR */
	pit_example_isr[u8PITModule] = pit_temp_isr;
    
    return 0;	  

}

/********************************************************************/
/* 
 * PIT0 ISR
 */
__interrupt__ void pit0_isr()
{
    MCF_PIT0_PCSR |= MCF_PIT_PCSR_PIF;
    pit_example_isr[0]();
}

/********************************************************************/
/* 
 * PIT1 ISR
 */
__interrupt__ void pit1_isr()
{
    MCF_PIT1_PCSR |= MCF_PIT_PCSR_PIF;
    pit_example_isr[1]();
}

/********************************************************************/
/* 
 * Recursively calculate value to the power of 2
 */
static uint32 pow(uint8 u8Pow2)
{
    if (!u8Pow2){
        return 1;
    }
    else{
        return (2*pow(u8Pow2-1));
    }
}
