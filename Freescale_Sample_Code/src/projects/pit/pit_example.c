/*
 * File:		pit_example.c
 * Purpose:		PIT Examples
 * 
 * License:     All software covered by license agreement in -
 *              docs/Freescale_Software_License.pdf
 */

#include "common.h"
#include "pit.h"
#include "pit_example.h"

static void example_idle_state(void);
static void LED_Toggle(void);

/********************************************************************/
int8 pit_periodicity (void)
{
    uint8 u8Char;
    uint8 j;
 
 
    printf("\t***********************\t\n");
    printf("Periodicity Example\n");
    printf("Connect DTIN0 to O-Scope and press a key when ready\n");

    WAIT_4_USER_INPUT();
    example_idle_state();
    
    /* Use both PITs */
    for (j=0; j < 2; j++)
    {
    	if (PIT_SetFreq(FREQUENCY,j) == -1)
        {
            printf("Error\n");
            return -1;
        }
            
        PIT_EnableInts(LED_Toggle, j);
        LED_Toggle();
        PIT_Start(j);
            
        printf("PIT #%d\t", j);
        printf ("Frequency of toggles is %dHz. OK?(y/n)", FREQUENCY);
        u8Char = WAIT_4_USER_INPUT();
        PIT_Stop(j);
        printf("\n");
        if ((u8Char != 'Y') && (u8Char != 'y'))
        {
            return -1;
        }
    }
    return 0;
}

/********************************************************************/
int8 pit_countdown (void)
{
    uint8 u8Char;
    uint8 j;
 
    printf("\t***********************\t\n");
    printf("CountDown Example\n");
    printf("Connect DTIN0 to O-Scope, set to trigger on rising edge, "); 
    printf("and expect a %dms long high signal\n",COUNTDOWN); 
    printf("Press a key when ready\n");

    WAIT_4_USER_INPUT();
    example_idle_state();
    
    for (j=0; j < 2; j++)
    {
    	MCF_GPIO_PORTTC = 0x00;
        MCF_GPIO_PORTTC = 0x01;	//DTIN0 output high 
        	
        /* Start countdown for COUNTDOWN milliseconds */
        if (PIT_CountDown(COUNTDOWN,j) == -1)
        {
        	printf("Error\n");
            return -1;
        }
            
		MCF_GPIO_PORTTC = 0x00;	//DTIN0 output low 

        printf("PIT #%d\t", j);
        printf("Time DTIN0 held high was %dms. OK?(y/n)", COUNTDOWN);
        u8Char = WAIT_4_USER_INPUT();
        PIT_Stop(j);

  
        printf("\n");
        if ((u8Char != 'Y') && (u8Char != 'y'))
        {
        	return -1;
            
        }
      	printf("Reset trigger to retest with PIT1\n");
    	printf("Press a key when ready\n");
        WAIT_4_USER_INPUT();
    }
    return 0;
}

/********************************************************************/
static void example_idle_state(void){
    (void) PITInit(0, 800, 0);
    (void) PITInit(0, 800, 1);
    MCF_GPIO_PTCPAR = 0x00; 	//set DTIN0 as GPIO
    MCF_GPIO_DDRTC = 0x01;		//set DTIN0 as output
    MCF_GPIO_PORTTC = 0x00;	//DTIN0 output low    
}

/********************************************************************/
static void LED_Toggle(void){
    MCF_GPIO_PORTTC ^= 0x01;
}
