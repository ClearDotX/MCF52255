/*
 * File:    pwm.h
 * Purpose: Header for the Pulse Width Modulation Module
 * 
 * License:     All software covered by license agreement in -
 *              docs/Freescale_Software_License.pdf
 */

#ifndef _PWM_H_
#define _PWM_H_

#include "common.h"

extern uint8 shutdown;

typedef struct __PWMInfo
{
	uint8 u8Clksel;		//select channel input clock
	uint8 u8PrescalerA;	//select prescaler A
	uint8 u8PrescalerB;	//select prescaler B
	uint8 u8SADivisor;	//select divisior to divide clock A to clock SA
	uint8 u8SBDivisor;	//select divisior to divice clock B to clock SB
	uint8 u8ALign;		//select left align or center align mode
	uint8 u8Concat;		//selects if concatenate as 16bit mode
	uint8 u8Polarity;	//selects channel polarity
	uint8 u8StopInDoze;	//select if PWM stops while in doze mode
	uint8 u8StopInDebug;	//select if PWM stops while in debug mode
	uint8 u8Period;		//set PWM output period
	uint8 u8Duty;		//set PWM output duty cycle
} PWMInfo;


int8 PWMInit(PWMInfo pwminfo, uint8 u8PWMModule);
void PWMEnable(uint8 u8PWMModule);
void PWMDisable(uint8 u8PWMModule);
void PWMShutdown(void);
void PWMExitShutdown(void);

__interrupt__ void pwm_handler(void);



#endif /* _PWM_H_ */

