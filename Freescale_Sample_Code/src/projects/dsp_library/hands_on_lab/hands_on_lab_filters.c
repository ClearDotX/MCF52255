/*********************************************
 Freescale Semiconductor 2007
 (c) COPYRIGHT Freescale Semiconductor 2007
 ALL RIGHTS RESERVED.
**********************************************/

#include "hands_on_lab_filters.h"

// 3rd order lowpass Butterworth filter:fc=120 Hz
int16 butter3_lp_coef[8]    = {0x18f9, 0x4aec, 0x4aec, 0x18f9,  // numerator coefficients
			       0x136e, 0xbc11, 0x501d, 0xe000}; // denominator coefficients
uint8 butter3_lp_num_sf      = 22; // numerator fixed point scale factor
uint8 butter3_lp_den_sf      = 13; // denominator fixed point scale factor
uint8 butter3_lp_order       = 3;  // filter order

// 4th order lowpass Butterworth filter:fc=350Hz
int16 butter4_lp_coef[10]    = {0x109e, 0x427a, 0x63b7, 0x427a, 0x109e,  // numerator coefficients
				0xfb86, 0x1a58, 0xc2b8, 0x4342, 0xe000}; // denominator coefficients
uint8 butter4_lp_num_sf      = 19; // numerator fixed point scale factor
uint8 butter4_lp_den_sf      = 13; // denominator fixed point scale factor
uint8 butter4_lp_order       = 4;  // filter order

// 5th order lowpass Butterworth filter:fc=120 Hz
int16 butter5_lp_coef[12]    = {0x0AB6, 0x358E, 0x6B1D, 0x6B1D, 0x358E, 0x0AB6,  // numerator coefficients
				0x0722, 0xd698, 0x60ab, 0x8e78, 0x4320, 0xe000}; // denominator coefficients
uint8 butter5_lp_num_sf      = 27; // numerator fixed point scale factor
uint8 butter5_lp_den_sf      = 12; // denominator fixed point scale factor
uint8 butter5_lp_order       = 5;  // filter order


// 3rd order highpass Butterworth filter:fc=100Hz
int16 butter3_hp_coef[10]    = {0xe601, 0x4dfe, 0xb202, 0x19ff,  // numerator coefficients
				0x151f, 0xb7e5, 0x52c0, 0xe000}; // denominator coefficients
uint8 butter3_hp_num_sf      = 13; // numerator fixed point scale factor
uint8 butter3_hp_den_sf      = 13; // denominator fixed point scale factor
uint8 butter3_hp_order       = 3;  // filter order

// 4th order highpass Butterworth filter:fc=350Hz
int16 butter4_hp_coef[10]    = {0x0ccf, 0xccc2, 0x4cdd, 0xccc2, 0x0ccf,  // numerator coefficients
				0xfadf, 0x1d99, 0xbcff, 0x473d, 0xe000}; // denominator coefficients
uint8 butter4_hp_num_sf      = 13; // numerator fixed point scale factor
uint8 butter4_hp_den_sf      = 13; // denominator fixed point scale factor
uint8 butter4_hp_order       = 4;  // filter order

    
// 4th order highpass Chebyshev_2 filter:fc=350Hz
int16 cheby1_4_hp_coef[10]    = {0x0b4f, 0xd379, 0x4272, 0xd379, 0x0b4f,  // numerator coefficients
				 0xfc00, 0x1799, 0xc838, 0x3ebc, 0xe000}; // denominator coefficients
uint8 cheby1_4_hp_num_sf      = 13; // numerator fixed point scale factor
uint8 cheby1_4_hp_den_sf      = 13; // denominator fixed point scale factor
uint8 cheby1_4_hp_order       = 4;  // filter order


// 4th order highpass Chebyshev_2 filter:fc=900Hz
int16 cheby4_hp_coef[10]    = {0x173c, 0xabfc, 0x7a04, 0xabfc, 0x173c,  // numerator coefficients
			       0xfd68, 0xfbca, 0xbe23, 0xe803, 0xe000}; // denominator coefficients
uint8 cheby4_hp_num_sf      = 17; // numerator fixed point scale factor
uint8 cheby4_hp_den_sf      = 16; // denominator fixed point scale factor
uint8 cheby4_hp_order       = 4;  // filter order

  
// 4th order bandpass Chebyshev_1 filter:fc=350Hz
int16 cheby1_4_bp_coef[10]    = {0x26b2, 0x0000, 0xb29b, 0x0000, 0x26b2,  // numerator coefficients
				 0xE67E, 0x5098, 0x8792, 0x5a5b, 0xc000}; // denominator coefficients
uint8 cheby1_4_bp_num_sf      = 20; // numerator fixed point scale factor
uint8 cheby1_4_bp_den_sf      = 13; // denominator fixed point scale factor
uint8 cheby1_4_bp_order       = 4;  // filter order


// 4th order bandpass Chebyshev_1 filter:fc=900Hz
int16 cheby2_4_bp_coef[10]    = {0x22cd, 0x0000, 0xba65, 0x000, 0x22cd,   // numerator coefficients
				 0xd71a, 0xcb58, 0x8f26, 0xbd99, 0xc000}; // denominator coefficients
uint8 cheby2_4_bp_num_sf      = 18; // numerator fixed point scale factor
uint8 cheby2_4_bp_den_sf      = 14; // denominator fixed point scale factor
uint8 cheby2_4_bp_order       = 4;  // filter order
