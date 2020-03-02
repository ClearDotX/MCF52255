/*********************************************
 Freescale Semiconductor 2007
 (c) COPYRIGHT Freescale Semiconductor 2007
 ALL RIGHTS RESERVED.
**********************************************/

#include "hands_on_lab_exercises.h"

/*******************************************************************/
/* global variables for hands-on lab                               */
/*******************************************************************/

/* sensor measurements */
int16 accel_x,accel_y,accel_z;
int16 temperature;

/* filter structures */
IIR_STRUCT filter0,filter1,filter2;

/* timer measurements */
uint32 start_time1,start_time2,delta_time1,delta_time2;
uint8  sample_rate_reduction_log2 = 0; // PIT timer prescaler
uint16 sample_rate_pmr = 13200; // PIT timer modulus

/*******************************************************************/
/* main loop for hands-on lab                                      */
/*******************************************************************/
void hands_on_lab_main(void) {

#ifdef HANDS_ON_LAB_WITH_USB
  /* configure the usb port */
  usb_configure();
#endif

  /* initialize datapath structures */
  hands_on_lab_init();  

  /* enable all interrupts to the core */
  mcf5xxx_irq_enable();
  printf(" Starting main loop \n");

  /* spin here since all remaining functions are now interrupt driven */
  while (1) {

#ifdef HANDS_ON_LAB_WITH_USB
    /* maintain usb communication */
    usb_process();
#else
    /* prevent compiler from stripping this loop out */
    nop();
#endif
  }
}

/********************************************************************/
/* initialize datapath structures                                   */
/********************************************************************/
void hands_on_lab_init(void) {

  /* initialize PIT0 timer for hands_on_lab_run() and hands_on_lab_print() */
  PIT0_Timer_Init(sample_rate_reduction_log2,sample_rate_pmr); // (((sys_clk/2)/2^0)/13200) = 330us

  /* start DMA timer for measuring filter execution times */
  MCF_DTIM0_DTMR = 0x00000003;

  /* configure GPIO to control accelerometers */
  G_INIT(); // Accel init

  /* initialize ADC */
  adc_init();

  /* FAE FILTER DESIGN EXPERIMENTS
     -----------------------------
  
  // Experiment 1: Walk-in Demo - Lowpass and Highpass Filter Demonstration
  // Experiment 2: Highpass Digital Filter Design
  // Experiment 3: Cascaded Filter Design
  // Experiment 4: Interactive Filter Design
  // Experiment 5: Signal Aliasing
  */  

  // -------------------------------------------------------------------------------------------------------------- 
  // NON-CASCADED FILTER DESIGN : EXPERIMENTS 1, 2, 4 AND 5
  
  // sensor ---> filter1 ----------------> output1 (data path 1)
  //        |
  //        ---> filter2 ----------------> output2 (data path 2)

  // ******************************************************************************************************
  // data path 1:NON-CASCADED FILTERS
  
  // select one by uncommenting the iir_init line. make sure only one filter is selected.
  
  // EXPERIMENT 1: WALK-IN DEMO
  // 4th order highpass Chebyshev_2 filter:fc=350Hz
  iir_init(&filter1,&accel_z,cheby1_4_hp_coef,cheby1_4_hp_num_sf,cheby1_4_hp_den_sf,cheby1_4_hp_order);
  
  // EXPERIMENT 2: HIGHPASS FILTER DESIGN
  // 4th order highpass Chebyshev_2 filter:fc=900Hz
  // iir_init(&filter1,&accel_z,cheby4_hp_coef,cheby4_hp_num_sf,cheby4_hp_den_sf,cheby4_hp_order);
  
  // ADDITIONAL HIGHPASS FILTERS
  // 3rd order highpass Butterworth filter:fc=100Hz
  // iir_init(&filter1,&accel_z,butter3_hp_coef,butter3_hp_num_sf,butter3_hp_den_sf,butter3_hp_order);
  
  // ADDITIONAL LOWPASS FILTERS
  // 3rd order lowpass Butterworth filter:fc=120 Hz
  // iir_init(&filter1,&accel_z,butter3_lp_coef,butter3_lp_num_sf,butter3_lp_den_sf,butter3_lp_order);
  
  // 4th order lowpass Butterworth filter:fc=350 Hz
  // iir_init(&filter1,&accel_z,butter4_lp_coef,butter4_lp_num_sf,butter4_lp_den_sf,butter4_lp_order);
  
  // 5th order lowpass Butterworth filter:fc=120 Hz
  // iir_init(&filter1,&accel_z,butter5_lp_coef,butter5_lp_num_sf,butter5_lp_den_sf,butter5_lp_order);
  
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  
  // ******************************************************************************************************
  // data path 2:NON-CASCADED FILTERS
  
  // select one by uncommenting the iir_init line. make sure only one filter is selected.
  
  // EXPERIMENT 1: WALK-IN DEMO
  // 3rd order lowpass Butterworth filter:fc=120 Hz
  iir_init(&filter2,&accel_z,butter3_lp_coef,butter3_lp_num_sf,butter3_lp_den_sf,butter3_lp_order);
  
  // EXPERIMENT 2: HIGHPASS FILTER DESIGN
  // 4th order highpass Butterworth filter:fc=350 Hz
  // iir_init(&filter2,&accel_z,butter4_hp_coef,butter4_hp_num_sf,butter4_hp_den_sf,butter4_hp_order);
  
  // ADDITIONAL HIGHPASS FILTERS
  // 3rd order highpass Butterworth filter:fc=100Hz
  // iir_init(&filter2,&accel_z,butter3_hp_coef,butter3_hp_num_sf,butter3_hp_den_sf,butter3_hp_order);
  
  // 4th order highpass Chebyshev_2 filter:fc=350Hz
  // iir_init(&filter2,&accel_z,cheby1_4_hp_coef,cheby1_4_hp_num_sf,cheby1_4_hp_den_sf,cheby1_4_hp_order);
  
  // 4th order highpass Chebyshev_2 filter:fc=900Hz
  // iir_init(&filter2,&accel_z,cheby4_hp_coef,cheby4_hp_num_sf,cheby4_hp_den_sf,cheby4_hp_order);
  
  // ADDITIONAL LOWPASS FILTERS
  // 3rd order lowpass Butterworth filter:fc=120 Hz
  // iir_init(&filter2,&accel_z,butter3_lp_coef,butter3_lp_num_sf,butter3_lp_den_sf,butter3_lp_order);
  
  // 4th order lowpass Butterworth filter:fc=350 Hz
  // iir_init(&filter2,&accel_z,butter4_lp_coef,butter4_lp_num_sf,butter4_lp_den_sf,butter4_lp_order);
  
  // ADDITIONAL BANDPASS FILTERS
  // 4th order bandpass Chebychev_2 filter:fc=350 Hz
  // iir_init(&filter2,&accel_z,cheby1_4_bp_coef,cheby1_4_bp_num_sf,cheby1_4_bp_den_sf,cheby1_4_bp_order);
  
  // 4th order bandpass Chebychev_2 filter:fc=900 Hz
  // iir_init(&filter2,&accel_z,cheby2_4_bp_coef,cheby2_4_bp_num_sf,cheby2_4_bp_den_sf,cheby2_4_bp_order);
  
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  
  
  // -------------------------------------------------------------------------------------------------------------- 

  // CASCADED FILTER DESIGN : EXPERIMENT 4
  
  // sensor ---> filter0 ---> filter1 ---> output1 (data path 1)
  //        |
  //        ---> filter2 ----------------> output2 (data path 2)
  
  /*
  // ******************************************************************************************************
  // data path 1:CASCADED FILTERS
  
  // 3rd order highpass Butterworth filter:fc=100Hz
  iir_init(&filter0,&accel_z,butter3_hp_coef,butter3_hp_num_sf,butter3_hp_den_sf,butter3_hp_order);
  
  // 4th order lowpass Butterworth filter:fc=350Hz
  iir_init(&filter1,&filter0.output,butter4_lp_coef,butter4_lp_num_sf,butter4_lp_den_sf,butter4_lp_order);
  
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  
  // ******************************************************************************************************
  // data path 2:CASCADED FILTERS
  
  // 4th order bandpass Chebychev_2 filter:fc=350 Hz
  iir_init(&filter2,&accel_z,cheby1_4_bp_coef,cheby1_4_bp_num_sf,cheby1_4_bp_den_sf,cheby1_4_bp_order);
  
  // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
  */

}

/********************************************************************/
/* sample ADC's and execute filters                                 */
/********************************************************************/
void hands_on_lab_run(void) {
int16 G_XOUT_RAW, G_YOUT_RAW, G_ZOUT_RAW;

/* Clear stop bit */
	 		MCF_ADC_CTRL1 &= ~MCF_ADC_CTRL1_STOP0;
	 		 			

			/* Set start bit */
      		MCF_ADC_CTRL1 |= MCF_ADC_CTRL1_START0; 			
			
			/* Waiting for "Ready" bit to be set */
      		while (!(MCF_ADC_ADSTAT & MCF_ADC_ADSTAT_RDY0))	
      		{};
      		
			/* Array of comversions being done on AN0 */
      		G_XOUT_RAW = (MCF_ADC_ADRSLT0 >> 3);

			/* Set stop bit */
 			MCF_ADC_CTRL1 |= MCF_ADC_CTRL1_STOP0; 	


  // sample ADC's
  accel_x     = G_XOUT_RAW;
  accel_y     = G_YOUT_RAW;
  accel_z     = G_ZOUT_RAW;// jpw POT_RAW
  temperature = 0;

  // data path 1 : execute filter(s) and get timestamp
  start_time1 = MCF_DTIM0_DTCN;
  
  // comment the following line for NON-CASCADED FILTERS, uncomment for CASCADED FILTERS
  // iir(&filter0);
  iir(&filter1);
  delta_time1 = MCF_DTIM0_DTCN - start_time1;
    
  // data path 2 : execute filter and get timestamp  
  start_time2 = MCF_DTIM0_DTCN;
  iir(&filter2);
  delta_time2 = MCF_DTIM0_DTCN - start_time2;

  /* Set start bits (A and B) */
  MCF_ADC_CTRL1 |= MCF_ADC_CTRL1_START0;
  MCF_ADC_CTRL2 |= MCF_ADC_CTRL2_START1;
}

/********************************************************************/
/* print inputs and outputs of filters                              */
/********************************************************************/
void hands_on_lab_print(char port) {
  
  uint32 sample_period_cycles = sample_rate_pmr << (1 + sample_rate_reduction_log2);  

  if (port == eUSB) {
    usb_printf("*%04x %04x %04x %08x %08x %08x %08x\n",accel_z,filter1.output,filter2.output,start_time1,delta_time1,delta_time2,sample_period_cycles);
  }

  if (port == eUART1) {
    printf("*%04x %04x %04x %08x %08x %08x %08x\n",accel_z,filter1.output,filter2.output,start_time1,delta_time1,delta_time2,sample_period_cycles);
  }
}

