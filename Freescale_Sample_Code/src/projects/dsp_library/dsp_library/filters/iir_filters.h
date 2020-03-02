/*********************************************
 Freescale Semiconductor 2007
 (c) COPYRIGHT Freescale Semiconductor 2007
 ALL RIGHTS RESERVED.
**********************************************/

/* This file contains IIR filter parameters for Butterworth filters
All normalized cutoff frequencies use the convention, f_nyquist = 1 = fs/2.
*/

#ifndef _IIR_FILTERS_H_
#define _IIR_FILTERS_H_

#include "common.h"

/* -------------------------------------------------------------------- */
/* Butterworth Lowpass Filter, order=3, cutoff frequency=0.60*f_nyquist */
extern int16 butter3_lp_0_60_coef[ 8];
extern uint8 butter3_lp_0_60_num_sf;
extern uint8 butter3_lp_0_60_den_sf;
extern uint8 butter3_lp_0_60_order;

#endif // _IIR_FILTERS_H_
