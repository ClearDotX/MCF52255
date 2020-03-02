
/*
 * File:    pwm.c
 * Purpose: Source for the Pulse Width Modulation Module
 * 
 * License:     All software covered by license agreement in -
 *              docs/Freescale_Software_License.pdf
 */

#include "pwm.h"

uint8 shutdown = 0;

/*
*	PWM Initialization for module u8PWMModule
*
*/
int8 PWMInit(PWMInfo pwminfo, uint8 u8PWMModule)
{
	//GPIO setting for PWM, polarity setting, input clock and alignment setting
	switch(u8PWMModule)
	{
	case 0:		
		if(pwminfo.u8Polarity)
			MCF_PWM_PWMPOL |= MCF_PWM_PWMPOL_PPOL0;
		else
			MCF_PWM_PWMPOL &= ~MCF_PWM_PWMPOL_PPOL0;
		if(pwminfo.u8Clksel)
			MCF_PWM_PWMCLK |= MCF_PWM_PWMCLK_PCLK0;	//select SA
		else
			MCF_PWM_PWMCLK &= ~MCF_PWM_PWMCLK_PCLK0;//select A
		if(pwminfo.u8ALign)
			MCF_PWM_PWMCAE |= MCF_PWM_PWMCAE_CAE0;
		else
			MCF_PWM_PWMCAE &= ~MCF_PWM_PWMCAE_CAE0;
		break;
	case 2:	
		if(pwminfo.u8Polarity)
			MCF_PWM_PWMPOL |= MCF_PWM_PWMPOL_PPOL2;
		else
			MCF_PWM_PWMPOL &= ~MCF_PWM_PWMPOL_PPOL2;
		if(pwminfo.u8Clksel)
			MCF_PWM_PWMCLK |= MCF_PWM_PWMCLK_PCLK2;	//select SB
		else
			MCF_PWM_PWMCLK &= ~MCF_PWM_PWMCLK_PCLK2;//select B
		if(pwminfo.u8ALign)
			MCF_PWM_PWMCAE |= MCF_PWM_PWMCAE_CAE2;
		else
			MCF_PWM_PWMCAE &= ~MCF_PWM_PWMCAE_CAE2;
		break;
	case 4:
		if(pwminfo.u8Polarity)
			MCF_PWM_PWMPOL |= MCF_PWM_PWMPOL_PPOL4;
		else
			MCF_PWM_PWMPOL &= ~MCF_PWM_PWMPOL_PPOL4;
		if(pwminfo.u8Clksel)
			MCF_PWM_PWMCLK |= MCF_PWM_PWMCLK_PCLK4;	//select SA
		else
			MCF_PWM_PWMCLK &= ~MCF_PWM_PWMCLK_PCLK4;//select A
		if(pwminfo.u8ALign)
			MCF_PWM_PWMCAE |= MCF_PWM_PWMCAE_CAE4;
		else
			MCF_PWM_PWMCAE &= ~MCF_PWM_PWMCAE_CAE4;
		break;
	case 6:
		if(pwminfo.u8Polarity)
			MCF_PWM_PWMPOL |= MCF_PWM_PWMPOL_PPOL6;
		else
			MCF_PWM_PWMPOL &= ~MCF_PWM_PWMPOL_PPOL6;
		if(pwminfo.u8Clksel)
			MCF_PWM_PWMCLK |= MCF_PWM_PWMCLK_PCLK6;	//select SB
		else
			MCF_PWM_PWMCLK &= ~MCF_PWM_PWMCLK_PCLK6;//select B
		if(pwminfo.u8ALign)
			MCF_PWM_PWMCAE |= MCF_PWM_PWMCAE_CAE6;
		else
			MCF_PWM_PWMCAE &= ~MCF_PWM_PWMCAE_CAE6;
		break;
	case 1:		
		if(pwminfo.u8Polarity)
			MCF_PWM_PWMPOL |= MCF_PWM_PWMPOL_PPOL1;
		else
			MCF_PWM_PWMPOL &= ~MCF_PWM_PWMPOL_PPOL1;
		if(pwminfo.u8Clksel)
			MCF_PWM_PWMCLK |= MCF_PWM_PWMCLK_PCLK1;	//select SA
		else
			MCF_PWM_PWMCLK &= ~MCF_PWM_PWMCLK_PCLK1;//select A
		if(pwminfo.u8ALign)
			MCF_PWM_PWMCAE |= MCF_PWM_PWMCAE_CAE1;
		else
			MCF_PWM_PWMCAE &= ~MCF_PWM_PWMCAE_CAE1;
		break;
	case 3:	
		if(pwminfo.u8Polarity)
			MCF_PWM_PWMPOL |= MCF_PWM_PWMPOL_PPOL3;
		else
			MCF_PWM_PWMPOL &= ~MCF_PWM_PWMPOL_PPOL3;
		if(pwminfo.u8Clksel)
			MCF_PWM_PWMCLK |= MCF_PWM_PWMCLK_PCLK3;	//select SB
		else
			MCF_PWM_PWMCLK &= ~MCF_PWM_PWMCLK_PCLK3;//select B
		if(pwminfo.u8ALign)
			MCF_PWM_PWMCAE |= MCF_PWM_PWMCAE_CAE3;
		else
			MCF_PWM_PWMCAE &= ~MCF_PWM_PWMCAE_CAE3;
		break;
	case 5:		
		if(pwminfo.u8Polarity)
			MCF_PWM_PWMPOL |= MCF_PWM_PWMPOL_PPOL5;
		else
			MCF_PWM_PWMPOL &= ~MCF_PWM_PWMPOL_PPOL5;
		if(pwminfo.u8Clksel)
			MCF_PWM_PWMCLK |= MCF_PWM_PWMCLK_PCLK5;	//select SA
		else
			MCF_PWM_PWMCLK &= ~MCF_PWM_PWMCLK_PCLK5;//select A
		if(pwminfo.u8ALign)
			MCF_PWM_PWMCAE |= MCF_PWM_PWMCAE_CAE5;
		else
			MCF_PWM_PWMCAE &= ~MCF_PWM_PWMCAE_CAE5;
		break;
	case 7:	
		if(pwminfo.u8Polarity)
			MCF_PWM_PWMPOL |= MCF_PWM_PWMPOL_PPOL7;
		else
			MCF_PWM_PWMPOL &= ~MCF_PWM_PWMPOL_PPOL7;
		if(pwminfo.u8Clksel)
			MCF_PWM_PWMCLK |= MCF_PWM_PWMCLK_PCLK7;	//select SB
		else
			MCF_PWM_PWMCLK &= ~MCF_PWM_PWMCLK_PCLK7;//select B
		if(pwminfo.u8ALign)
			MCF_PWM_PWMCAE |= MCF_PWM_PWMCAE_CAE7;
		else
			MCF_PWM_PWMCAE &= ~MCF_PWM_PWMCAE_CAE7;
		break;
	default:
		break;
	}
	
	//Prescaler for clock A and B
	MCF_PWM_PWMPRCLK = MCF_PWM_PWMPRCLK_PCKA(pwminfo.u8PrescalerA)| MCF_PWM_PWMPRCLK_PCKB(pwminfo.u8PrescalerB);

	//Divisor from clock SA to A and SB to B
	MCF_PWM_PWMSCLA = MCF_PWM_PWMSCLA_SCALEA(pwminfo.u8SADivisor);
	MCF_PWM_PWMSCLB = MCF_PWM_PWMSCLB_SCALEB(pwminfo.u8SBDivisor);
		
	//set PWM period
	MCF_PWM_PWMPER(u8PWMModule) = MCF_PWM_PWMPER_PERIOD(pwminfo.u8Period);
	
	//set PWM duty cycle
	MCF_PWM_PWMDTY(u8PWMModule) = MCF_PWM_PWMDTY_DUTY(pwminfo.u8Duty);

	if((pwminfo.u8Concat&0x01) == 0x01)
		//channel 0 and 1 as 16bit mode
		MCF_PWM_PWMCTL |= MCF_PWM_PWMCTL_CON01;
	if((pwminfo.u8Concat&0x02) == 0x02)
		//channel 2 and 3 as 16bit mode
		MCF_PWM_PWMCTL |= MCF_PWM_PWMCTL_CON23;
	if((pwminfo.u8Concat&0x04) == 0x04)
		//channel 4 and 5 as 16bit mode
		MCF_PWM_PWMCTL |= MCF_PWM_PWMCTL_CON45;
	if((pwminfo.u8Concat&0x08) == 0x08)
		//channel 6 and 7 as 16bit mode
		MCF_PWM_PWMCTL |= MCF_PWM_PWMCTL_CON67;

	if(pwminfo.u8StopInDoze)
		MCF_PWM_PWMCTL |= MCF_PWM_PWMCTL_PSWAI;
	else
		MCF_PWM_PWMCTL &= ~MCF_PWM_PWMCTL_PSWAI;
	if(pwminfo.u8StopInDebug)
		MCF_PWM_PWMCTL |= MCF_PWM_PWMCTL_PFRZ;
	else
		MCF_PWM_PWMCTL &= ~MCF_PWM_PWMCTL_PFRZ;

#if 0	
	printf("MCF_PWM_PWMPRCLK is 0x%x\n",MCF_PWM_PWMPRCLK);
	printf("MCF_PWM_PWMSCLA is 0x%x\n",MCF_PWM_PWMSCLA);
	printf("MCF_PWM_PWMSCLB is 0x%x\n",MCF_PWM_PWMSCLB);
	printf("MCF_PWM_PWMPER(u8PWMModule) is 0x%x\n",MCF_PWM_PWMPER(u8PWMModule) );
	printf("MCF_PWM_PWMCLK is 0x%x\n", MCF_PWM_PWMCLK);
	printf("MCF_PWM_PWMCAE is 0x%x\n", MCF_PWM_PWMCAE);
	printf("MCF_PWM_PWMCTL is 0x%x\n",MCF_PWM_PWMCTL);
#endif

	return 0;
}

void PWMEnable(uint8 u8PWMModule)
{
	MCF_PWM_PWME |= (0x01 << u8PWMModule);
}

void PWMDisable(uint8 u8PWMModule)
{
	MCF_PWM_PWME &= ~(0x01 << u8PWMModule);
}

void PWMShutdown(void)
{	
	//unmask PWM interrupts in INTC0
	MCF_INTC0_IMRH &= ~(MCF_INTC_IMRH_INT_MASK52 | MCF_INTC_IMRL_MASKALL);
        
    /* Setup the interrupt level and priority for UART0 */
	MCF_INTC0_ICR52 = MCF_INTC_ICR_IL(3) | MCF_INTC_ICR_IP(4);
	
 	/* Set the interrupt handlers in the vector table */
	mcf5xxx_set_handler(64 + 52, (ADDRESS)pwm_handler);	
	
	MCF_PWM_PWMSDN |= MCF_PWM_PWMSDN_IE;	//interrupt enable when PWM7IN changed
//	MCF_PWM_PWMSDN |= MCF_PWM_PWMSDN_LVL;	//force PWM shutdown level to high
	MCF_PWM_PWMSDN |= MCF_PWM_PWMSDN_PWM7IL;	//PWM7 input is active high
	
	MCF_PWM_PWMSDN |= MCF_PWM_PWMSDN_SDNEN;	//enable emergency shutdown
}

void PWMExitShutdown(void)
{
	MCF_INTC0_IMRH |= MCF_INTC_IMRH_INT_MASK52;	//mask PWM interrupt
	MCF_PWM_PWMSDN &= ~MCF_PWM_PWMSDN_IE;	//interrupt enable when PWM7IN changed
	MCF_PWM_PWMSDN &= ~MCF_PWM_PWMSDN_SDNEN;	//enable emergency shutdown
}

void PWMRestart(void)
{
	MCF_PWM_PWMSDN |= MCF_PWM_PWMSDN_RESTART;
}

__interrupt__ void pwm_handler(void)
{
	MCF_PWM_PWMSDN |= MCF_PWM_PWMSDN_IF;	
	shutdown = 1;
}
