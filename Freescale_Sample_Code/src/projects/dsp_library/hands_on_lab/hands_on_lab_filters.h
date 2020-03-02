/*********************************************
 Freescale Semiconductor 2007
 (c) COPYRIGHT Freescale Semiconductor 2007
 ALL RIGHTS RESERVED.
**********************************************/

#ifndef _HANDS_ON_LAB_FILTERS_
#define _HANDS_ON_LAB_FILTERS_

#include "common.h"
#include "dsp_library_structures.h"

// 3rd order lowpass Butterworth filter:fc=120 Hz
extern int16 butter3_lp_coef[8];
extern uint8 butter3_lp_num_sf;
extern uint8 butter3_lp_den_sf;
extern uint8 butter3_lp_order;

// 4th order lowpass Butterworth filter:fc=350Hz
extern int16 butter4_lp_coef[10];
extern uint8 butter4_lp_num_sf;
extern uint8 butter4_lp_den_sf;
extern uint8 butter4_lp_order;

// 5th order lowpass Butterworth filter:fc=120 Hz
extern int16 butter5_lp_coef[12];
extern uint8 butter5_lp_num_sf;
extern uint8 butter5_lp_den_sf;
extern uint8 butter5_lp_order;

// 3rd order highpass Butterworth filter:fc=100Hz
extern int16 butter3_hp_coef[10];
extern uint8 butter3_hp_num_sf;
extern uint8 butter3_hp_den_sf;
extern uint8 butter3_hp_order;

// 4th order highpass Butterworth filter:fc=350Hz
extern int16 butter4_hp_coef[10];
extern uint8 butter4_hp_num_sf;
extern uint8 butter4_hp_den_sf;
extern uint8 butter4_hp_order;
    
// 4th order highpass Chebyshev_2 filter:fc=350Hz
extern int16 cheby1_4_hp_coef[10];
extern uint8 cheby1_4_hp_num_sf;
extern uint8 cheby1_4_hp_den_sf;
extern uint8 cheby1_4_hp_order;

// 4th order highpass Chebyshev_2 filter:fc=900Hz
extern int16 cheby4_hp_coef[10];
extern uint8 cheby4_hp_num_sf;
extern uint8 cheby4_hp_den_sf;
extern uint8 cheby4_hp_order;
  
// 4th order bandpass Chebyshev_1 filter:fc=350Hz
extern int16 cheby1_4_bp_coef[10];
extern uint8 cheby1_4_bp_num_sf;
extern uint8 cheby1_4_bp_den_sf;
extern uint8 cheby1_4_bp_order;

// 4th order bandpass Chebyshev_1 filter:fc=900Hz
extern int16 cheby2_4_bp_coef[10];
extern uint8 cheby2_4_bp_num_sf;
extern uint8 cheby2_4_bp_den_sf;
extern uint8 cheby2_4_bp_order;

#endif // _HANDS_ON_LAB_FILTERS_
