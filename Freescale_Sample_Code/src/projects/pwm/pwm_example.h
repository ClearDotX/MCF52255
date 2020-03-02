/*
 * File:    pit.h
 * Purpose: Header for the Programmable Interrupt Timer Driver
 * 
 * License:     All software covered by license agreement in -
 *              docs/Freescale_Software_License.pdf
 */
 
#ifndef __PWM_EXAMPLE_H
#define __PWM_EXAMPLE_H

#include "common.h"

int8 pwm_basictest(void);
int8 pwm_shutdown(void);
int8 pwm_restart(void);
int8 pwm_dozemode(void);
int8 pwm_debugmode(void);
int8 pwm_concat(void);

#endif /* __PWM_EXAMPLE_H */

