/*********************************************
 Freescale Semiconductor 2007
 (c) COPYRIGHT Freescale Semiconductor 2007
 ALL RIGHTS RESERVED.
**********************************************/

/* C implementations of DSP functions. These functions should only be
   used sparingly since they are not optimized assembly. */

#include "dsp_library_c_functions.h"

/* wrapper to select filter implementation */
void iir(void *x) {
  IIR_STRUCT *y;

  y = x;
  switch(y->order) {
  case 2 : iir2_asm(x); break;
  case 3 : iir3_asm(x); break;
  case 4 : iir4_asm(x); break;
  case 5 : iir5_asm(x); break;
  case 6 : iir6_asm(x); break;
  }
}
