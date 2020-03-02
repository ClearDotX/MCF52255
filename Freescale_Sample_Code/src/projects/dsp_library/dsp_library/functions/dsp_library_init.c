/*********************************************
 Freescale Semiconductor 2007
 (c) COPYRIGHT Freescale Semiconductor 2007
 ALL RIGHTS RESERVED.
**********************************************/

/* Data structure initialization routines. */

#include "dsp_library_init.h"

/******************************************************************************/
/*                DATA STRUCTURE INITIALIZATION PROTOTYPES                    */
/******************************************************************************/

/* initialize an IIR structure of any order, since they are all
   essentially the same except for the buffer size. and because the
   buffer is the last element in the structure, all element offsets
   are the same across orders. */
void iir_init(void *x, int16 *input, void *coef, uint8 num_sf, uint8 den_sf, uint8 order) {
  uint8 i;
  IIR_STRUCT *y;

  y          = x;
  y->output  = 0;
  y->diff_sf = num_sf - den_sf;
  y->den_sf  = den_sf;
  y->input   = input;
  y->flags   = 0;
  y->coef    = coef;
  y->order   = order;
  
  for (i=0; i<2*order; i++) {
    y->buffer[i] = 0;
  }
}
