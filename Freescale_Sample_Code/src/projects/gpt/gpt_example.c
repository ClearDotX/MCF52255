/*
 * File:		gpt_example.c
 * Purpose:		GPT Examples
 *
 * License:     All software covered by license agreement in -
 *              docs/Freescale_Software_License.pdf
 */

#include "common.h"
#include "gpt.h"
#include "gpt_example.h"

static void example_idle_state(void);

/********************************************************************/
/* 1. Initialize the GPT module
 * 2. Configure pins as I/O
 * 3. Send data
 * 4. verify Data
 * 5. Repeat changing pins
 */
int8 gpt_gpio(void)
{
     /* Initialize GPT module */
    example_idle_state();
    printf("\t***********************\t\n");
    printf("GPT GPIO Examples\n");
    printf("Connect GPT0 to GPT1 (IC0 and IC1 on EVB) and GPT2 to GPT3 (IC2 and IC3 on EVB) and press a key\n");
    WAIT_4_USER_INPUT();
    
    /* Set GPT2-GPT0 as outputs and send '1' */
    MCF_GPIO_PTAPAR = 0x55;
    MCF_GPT_GPTDDR = 0x5;
    MCF_GPT_GPTPORT = 0x5;
    if (MCF_GPT_GPTPORT != 0xF)
    {
        printf("Error\n"); 
            return -1;
    }
    
    /* Set GPT2-GPT0 as outputs and send '0' */
    MCF_GPT_GPTPORT = 0x0;
    if (MCF_GPT_GPTPORT != 0x00)
    {
        printf("Error\n"); 
            return -1;
    }

    /* Set GPT3-GPT1 as outputs and send '1' */
    MCF_GPT_GPTDDR = 0xA;
    MCF_GPT_GPTPORT = 0xA;
    if (MCF_GPT_GPTPORT != 0x0F){
        printf("Error\n"); 
            return -1;
    }

    /* Set GPT3-GPT1 as outputs and send '1' */
    MCF_GPT_GPTPORT = 0x0;
    if (MCF_GPT_GPTPORT != 0x00){
        printf("Error\n"); 
            return -1;
    }
        
    printf("OK\n"); 
    
    return 0;
}

/********************************************************************/
/* 1. Initialize the GPT module
 * 2. Configure GPT2 as output
 * 3. Configure GPT3 as Pulse Accumulator
 * 4. Send different pulses 
 * 5. Verify if Pulses are correct 
 * 6. repeat for several # of pulses
 */
int8 gpt_pulseacc(void)
{
 
    uint16 i,j ;
    
     /* Initialize SPI module */
    example_idle_state();
    printf("\t***********************\t\n");
    printf("GPT GPIO Example\n");
    printf("Connect GPT2 to GPT3 (IC2 and IC3 on EVB) and press a key\n");
    WAIT_4_USER_INPUT();
    
    MCF_GPIO_PTAPAR = 0x50;
    MCF_GPT_GPTPORT = 0x0;
    MCF_GPT_GPTDDR = 0x4;
    
    GPTInitPulseAcc_Counter();
    GPTStartPulseAccCounter();
    for (j=0; j < 1000;j++)
    {
        MCF_GPT_GPTPORT = 0x4;
    	MCF_GPT_GPTPORT = 0x0;
    }
    if (GPTGetPulseAccCounter() != j)
    {
    	printf("Error\n"); 
        return -1;
    }    
    
    printf("OK\n"); 
    return 0;
}

/********************************************************************/
/* 1. Initialize the GPT module
 * 2. Configure GPT2 as output
 * 3. Configure GPT3 as Input Capture
 * 4. Send a pulse 
 * 5. Verify if input capture received the pulse 
 */
int8 gpt_ic(void)
{
    uint16 i ;
    
     /* Initialize GPT module */
    example_idle_state();
    printf("\t***********************\t\n");
    printf("GPT Input Capture Example\n");
    printf("Connect GPT2 to GPT3 (IC2 and IC3 on EVB) and press a key\n");
    WAIT_4_USER_INPUT();
    
    MCF_GPIO_PTAPAR = 0x50;
    MCF_GPT_GPTPORT = 0x0;
    MCF_GPT_GPTDDR = 0x4;
    
    GPTInitPulseMeasurement(0);
    
    GPTStartPulseMeasurement();
    MCF_GPT_GPTPORT = 0x4;
    asm (nop);
    MCF_GPT_GPTPORT = 0x0;
    asm (nop);
    MCF_GPT_GPTPORT = 0x4;
    for (i=0; i < 100; i++){
        if (GPTGetMeasurement_Poll() != 0x00){
            printf("OK\n"); 
            return 0;                
        }
    }
    printf("Error\n"); 
    return -1;
}

/********************************************************************/
/* 1. Initialize the GPT module
 * 2. Configure GPT2 as Output Compare
 * 3. Configure GPT3 as Input Capture
 * 4. Send a pulse 
 * 5. Verify if input capture received the pulse 
 */
int8 gpt_oc(void)
{
    uint32 u32Pulse;
    uint32 u32CountDown = 10000;
      
    /* Initialize GPT module */  
    example_idle_state();
    printf("\t***********************\t\n");
    printf("GPT Output Compare Example\n");
    printf("Connect GPT2 to GPT3 (IC2 and IC3 on EVB) and press a key\n");
    WAIT_4_USER_INPUT();
    
    
    MCF_GPIO_PTAPAR = 0x50;
    
    GPTInitPulseMeasurement(0x00);
    GPTInitOC(10000, 2, 1);
    GPTStartPulseMeasurement();
    
    do
    {
    	u32Pulse = GPTGetMeasurement_Poll();
    } while (!u32Pulse && ((u32CountDown--)!=0x00));
    
    if (!u32CountDown)
    {
    	printf("Error\n"); 
    	return -1;
    }
    
    /* Compare the pulse of the signal with the expected */
    if ((u32Pulse > 10010 || u32Pulse < 9990) && u32Pulse != 65536)
    {
    	printf("Error\n"); 
    	return -1;
    }
    
    return 0;
}

/********************************************************************/
/* 1. Initialize the GPT module
 * 2. Configure GPT2 as Output Compare
 * 3. Configure GPT3 as Input Capture
 * 4. Send a pulse 
 * 5. Verify if input capture received the pulse 
 */
int8 gpt_pwm(void)
{
    uint16 u16Duty[] = {10, 50, 1, 99};
    uint16 u16PerPrescaler[] = {0, 5, 7};
    uint8 ch;
	uint8 i,j;
      
    /* Initialize GPT module */  
    example_idle_state();
    
    printf("\t***********************\t\n");
    printf("GPT PWM Example\n");
    printf("Connect GPT0 (IC0 on EVB) O-scope and press a key\n");
    WAIT_4_USER_INPUT();
    
    
    MCF_GPIO_PTAPAR = 0x01;
    
    GPT_EN_TOV(0);
    GPT_START_TIMER();
    
    for (i=0; i < sizeof(u16PerPrescaler)/ sizeof(u16PerPrescaler[0]); i++)
    {
	    MCF_GPT_GPTSCR2 &= ~(MCF_GPT_GPTSCR2_PR(7));	
		MCF_GPT_GPTSCR2 |= MCF_GPT_GPTSCR2_PR(u16PerPrescaler[i]);
	
		printf("Period should be about %dHz\n", SYS_CLK_KHZ/2*1000/
	 									      ((1<<u16PerPrescaler[i])*65536));
		for (j=0; j < sizeof(u16Duty)/ sizeof(u16Duty[0]); j++)
		{
			GPTInitOC(65536*u16Duty[j]/100, 0, 2);	
			printf("Duty should be approx. %d%%\n", u16Duty[j]);
			printf("Press 'N' if integrity is incorrect\n");
			ch = in_char(); 
			if ((ch == 'N') || (ch == 'n')) 
		   		{
		   			printf("Error\n"); 
		    		return -1;
		   		}
		    
		}
    }
    return 0;
}



static void example_idle_state(void){
    GPTAInitResetState();
 }

