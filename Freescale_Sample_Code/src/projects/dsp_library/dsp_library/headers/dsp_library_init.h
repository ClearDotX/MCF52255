/*********************************************
 Freescale Semiconductor 2007
 (c) COPYRIGHT Freescale Semiconductor 2007
 ALL RIGHTS RESERVED.
**********************************************/

/* Prototypes for data structure initialization routines. */

#ifndef _DSP_LIBRARY_INIT_H_
#define _DSP_LIBRARY_INIT_H_

#include "common.h"
#include "dsp_library_defines.h"
#include "dsp_library_structures.h"

/******************************************************************************/
/*              DATAPATH STRUCTURE INITIALIZATION PROTOTYPES                  */
/******************************************************************************/

/* initialize an IIR structure of any order, since they are all
   essentially the same except for the buffer size. and because the
   buffer is the last element in the structure, all element offsets
   are the same across orders. */
void iir_init(void *x,                            // struct to initialize (IIR3_STRUCT, IIR4_STRUCT,...)
	      int16 *input,                       // input
	      void *coef,                         // filter coefficients
	      uint8 num_sf,                       // numerator coefficient scale factor
	      uint8 den_sf,                       // denominator coefficient scale factor
	      uint8 order);                       // filter order

/* map each order of IIR structure to the same general initialization
   routine. this helps to maintain a one-to-one relationship between
   structure, init function, and asm function. */
#define iir2_init iir_init
#define iir3_init iir_init
#define iir4_init iir_init
#define iir5_init iir_init
#define iir6_init iir_init

#endif // _DSP_LIBRARY_INIT_H_
