/*********************************************
 Freescale Semiconductor 2007
 (c) COPYRIGHT Freescale Semiconductor 2007
 ALL RIGHTS RESERVED.
**********************************************/

#ifndef _HANDS_ON_LAB_EXERCISES_H_
#define _HANDS_ON_LAB_EXERCISES_H_

#define HANDS_ON_LAB
#define HANDS_ON_LAB_WITH_USB
//#undef HANDS_ON_LAB_WITH_USB

/* includes */
#include "common.h"
#include "dsp_library.h"
#include "hands_on_lab_filters.h"

/* sensor measurements */
extern int16 accel_x,accel_y,accel_z,gyro_x,gyro_y;
extern int16 temperature;

/* filter structures */
extern IIR_STRUCT filter0,filter1,filter2;

/* timer measurements */
extern uint32 start_time1,start_time2,delta_time1,delta_time2;
extern uint8 sample_rate_reduction_log2;
extern uint16 sample_rate_pmr;

/* comm port */
enum port {eUSB,eSPI,eUART1,eFLASHSTORE,eFLASHDUMP};

/* functions */
void hands_on_lab_main(void);
void hands_on_lab_init(void);
void hands_on_lab_run(void);
void hands_on_lab_print(char port);

#endif // _HANDS_ON_LAB_EXERCISES_H_
