/*********************************************
 Freescale Semiconductor 2007
 (c) COPYRIGHT Freescale Semiconductor 2007
 ALL RIGHTS RESERVED.
**********************************************/

/* This file contains IIR filter parameters for Butterworth filters
All normalized cutoff frequencies use the convention, f_nyquist = 1 = fs/2.
*/

#include "iir_filters.h"

/* -------------------------------------------------------------------- */
/* Butterworth Lowpass Filter, order=3, cutoff frequency=0.60*f_nyquist */
int16 butter3_lp_0_60_coef[ 8] = {   8419,  25256,  25256,   8419,  // numerator coefficients
                                    -1845, -13821, -18915,      0}; // denominator coefficients
uint8 butter3_lp_0_60_num_sf   = 15; // numerator fixed point scale factor
uint8 butter3_lp_0_60_den_sf   = 15; // denominator fixed point scale factor
uint8 butter3_lp_0_60_order    =  3; // filter order
