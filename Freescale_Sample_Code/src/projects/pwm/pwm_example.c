/*
 * File:		pwm_example.c
 * Purpose:		PWM Examples
 * 
 * License:     All software covered by license agreement in -
 *              docs/Freescale_Software_License.pdf
 */

#include "common.h"
#include "uart.h"
#include "pwm.h"
#include "pwm_example.h"

#define WAIT_4_USER_INPUT()    in_char()

static PWMInfo pwm0info, pwm1info;

static void example_idle_state(void);

static void example_idle_state(void)
{
	//GPIO setting for PWM, PWM0,2,4,6 also connect to led
	MCF_GPIO_PTCPAR |= MCF_GPIO_PTCPAR_DTIN0_PWM0;
	MCF_GPIO_PTCPAR |= MCF_GPIO_PTCPAR_DTIN1_PWM2;
	MCF_GPIO_PTCPAR |= MCF_GPIO_PTCPAR_DTIN2_PWM4;
	MCF_GPIO_PTCPAR |= MCF_GPIO_PTCPAR_DTIN3_PWM6;
	
	MCF_GPIO_PTAPAR |= MCF_GPIO_PTAPAR_ICOC0_PWM1;
	MCF_GPIO_PTAPAR |= MCF_GPIO_PTAPAR_ICOC1_PWM3;
	MCF_GPIO_PTAPAR |= MCF_GPIO_PTAPAR_ICOC2_PWM5;
	MCF_GPIO_PTAPAR |= MCF_GPIO_PTAPAR_ICOC3_PWM7;

	/* PWM0 info */
	pwm0info.u8ALign = 1;	//u8Align = 0, left align; u8Align = 1, center align
	pwm0info.u8Clksel = 0;	//u8Clksel = 0, select clock A; u8Clksel = 1, select clock SA
	pwm0info.u8Concat = 0;	//u8Concat = 0, 8 8bit PWM ch; u8Concat = 1, 4 16bit ch
	pwm0info.u8Polarity = 1;	//u8Polarity = 0, ch low at beginning; u8Polarity = 1, ch high at beginning

	/*
	*	Clock A and B prescaler
	*	0x0: sysclk/2
	*	0x1: sysclk/4
	*
	...
	*
	*	0x7: sysclk/128
	*/
	pwm0info.u8PrescalerA = 0x07;
	pwm0info.u8PrescalerB = 0x07;

	/*
	*	Clock SA and SB divisor
	*	
	*	Clock SA = Clock A / (2 * divisor)
	*
	*	0x0: divisor 256
	*	0x1: divisor 1
	*	0x2: divisor 2
	*	...
	*	0xff: divisor 255
	*/
	pwm0info.u8SADivisor = 0x0;
	pwm0info.u8SBDivisor = 0x0;

	/*
	*	PWM period = (pwminfo.u8Align + 1)*pwminfo.u8Period /(SYS_CLK_KHZ*1000)
	*/
	pwm0info.u8Period = 150;	//PWM period = 0.4ms
	/*
		duty cycle = |1 - pwminfo.u8Polarity - pwminfo.u8Duty/pwminfo.u8Period| 
	*/
	pwm0info.u8Duty = 100;	//duty cycle

	pwm0info.u8StopInDebug = 0;
	pwm0info.u8StopInDoze = 0;

	
	/* PWM1 info */
	pwm1info.u8ALign = 0; //u8Align = 0, left align; uAlign = 1, center align
	pwm1info.u8Clksel = 1;	//u8Clksel = 0, select clock A; u8Clksel = 1, select clock SA
	pwm1info.u8Concat = 0;	//u8Concat = 0, 8 8bit PWM ch; u8Concat = 1, 4 16bit ch
	pwm1info.u8Polarity = 0;	//u8Polarity = 0, ch low at beginning; u8Polarity = 1, ch high at beginning

	/*
	*	Clock A and B prescaler
	*	0x0: sysclk/2
	*	0x1: sysclk/4
	*
	...
	*
	*	0x7: sysclk/128
	*/
	pwm1info.u8PrescalerA = 0x06;
	pwm1info.u8PrescalerB = 0x06;

	/*
	*	Clock SA and SB divisor
	*	
	*	Clock SA = Clock A / (2 * divisor)
	*
	*	0x0: divisor 256
	*	0x1: divisor 1
	*	0x2: divisor 2
	*	...
	*	0xff: divisor 255
	*/
	pwm1info.u8SADivisor = 0x7d;
	pwm1info.u8SBDivisor = 0x7d;

	/*
	*	PWM period = (pwminfo.u8Align + 1)*pwminfo.u8Period /(SYS_CLK_KHZ*1000)
	*/
	pwm1info.u8Period = 150;	//PWM period = 50ms
	/*
		duty cycle = |1 - pwminfo.u8Polarity - pwminfo.u8Duty/pwminfo.u8Period| 
	*/
	pwm1info.u8Duty = 100;	//duty cycle

	pwm1info.u8StopInDebug = 0;
	pwm1info.u8StopInDoze = 0;

	PWMInit(pwm0info,0);
	PWMInit(pwm1info,1);
}

int8 pwm_basictest(void)
{
	uint8 u8Char;
    uint8 j;

    printf("\t***********************\t\n");
    printf("PWM basic Test\n");
    printf("Connect PWM0 (TIN0 on EVB) and PWM1 (IC0 on EVB) to O-Scope and press a key when ready\n");

    WAIT_4_USER_INPUT();
    example_idle_state();
    
    /* Use both PWMs */
    for (j=0; j < 2; j++)
    {
        PWMEnable(j);
        printf("PWM #%d\t", j);
        printf ("PWM Channel %d working OK?(y/n)", j);
        u8Char = WAIT_4_USER_INPUT();
        PWMDisable(j);
        printf("\n");
        if ((u8Char != 'Y') && (u8Char != 'y'))
        {
            return -1;
        }
    }
    return 0;
}


int8 pwm_concat(void)
{
	uint8 u8Char;
 
    printf("\t***********************\t\n");
    printf("PWM concat mode Test\n");
    printf("Connect PWM0 (TIN0 on EVB) and PWM1 (IC0 on EVB) to O-Scope and press a key when ready\n");	
    WAIT_4_USER_INPUT();
	//GPIO setting for PWM, PWM0,2,4,6 also connect to led
	MCF_GPIO_PTCPAR |= MCF_GPIO_PTCPAR_DTIN0_PWM0;
	MCF_GPIO_PTCPAR |= MCF_GPIO_PTCPAR_DTIN1_PWM2;
	MCF_GPIO_PTCPAR |= MCF_GPIO_PTCPAR_DTIN2_PWM4;
	MCF_GPIO_PTCPAR |= MCF_GPIO_PTCPAR_DTIN3_PWM6;
	
	MCF_GPIO_PTAPAR |= MCF_GPIO_PTAPAR_ICOC0_PWM1;
	MCF_GPIO_PTAPAR |= MCF_GPIO_PTAPAR_ICOC1_PWM3;
	MCF_GPIO_PTAPAR |= MCF_GPIO_PTAPAR_ICOC2_PWM5;
	MCF_GPIO_PTAPAR |= MCF_GPIO_PTAPAR_ICOC3_PWM7;
	
	MCF_PWM_PWMPOL |= MCF_PWM_PWMPOL_PPOL1;
	MCF_PWM_PWMCLK &= ~MCF_PWM_PWMCLK_PCLK1;//select A
	MCF_PWM_PWMCAE |= MCF_PWM_PWMCAE_CAE1; //center align
	MCF_PWM_PWMPRCLK = MCF_PWM_PWMPRCLK_PCKA(0x7);
	//40ms
	MCF_PWM_PWMPER(0) = MCF_PWM_PWMPER_PERIOD(0x3a);
	MCF_PWM_PWMPER(1) = MCF_PWM_PWMPER_PERIOD(0x98);
	MCF_PWM_PWMDTY(0) = MCF_PWM_PWMDTY_DUTY(0x27);
	MCF_PWM_PWMDTY(1) = MCF_PWM_PWMDTY_DUTY(0x10);
	MCF_PWM_PWMCTL |= MCF_PWM_PWMCTL_CON01;
	MCF_PWM_PWME = 0x02;

    printf ("PWM Concat functioning? (y/n)");
    u8Char = WAIT_4_USER_INPUT();
    printf("\n");
    if ((u8Char != 'Y') && (u8Char != 'y'))
    {
       return -1;
    }
    return 0;
}

