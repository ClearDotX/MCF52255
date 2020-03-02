/*********************************************
 Freescale Semiconductor 2007
 (c) COPYRIGHT Freescale Semiconductor 2007
 ALL RIGHTS RESERVED.
**********************************************/

/* DSP library data structures */

#ifndef _DSP_LIBARY_STRUCTURES_H_
#define _DSP_LIBARY_STRUCTURES_H_

#include "common.h"
#include "dsp_library_defines.h"

/******************************************************************************/
/*                  DATAPATH STRUCTURE DEFINITIONS                            */
/******************************************************************************/

/* Define a structure for each type of assembly function. Each
   structure defines elements to all inputs, outputs, coefficients,
   and any other variables used by the function. Each assembly
   function is aware of its respective structure, and indexes into the
   structure accordingly. Therefore, any structure changes
   consequently require assembly changes. By using different
   structures, memory is used efficiently and the assembly function
   can make best use of the movem instruction.

   Pointers must be used for datapath inputs in order to daisy-chain
   the functions and to write multiple outputs.

   The sole argument to every assembly function is simply a
   pointer to its respective structure. */


/* IIR filter. Since all IIR filter structures are essentially the
 same, except for the buffer sizes, define a preprocessor macro and
 then call it for all filter orders. */
#define DEFINE_IIR_STRUCT(ORDER)                                                             \
typedef struct {                                                                             \
  int16 output;                          /* output data                                   */ \
  uint8 diff_sf;                         /* difference between num and den scale factors) */ \
  uint8 den_sf;                          /* denominator scale factor                      */ \
  int16 *input;                          /* input data                                    */ \
  uint32 flags;                          /* status flags                                  */ \
  int16 (*coef)[(ORDER+1)*2];            /* coefficient vector                            */ \
  uint32 order;                          /* filter order                                  */ \
  int16 buffer[ORDER*2];                 /* input/output buffer                           */ \
} IIR##ORDER##_STRUCT; 

DEFINE_IIR_STRUCT(2)
DEFINE_IIR_STRUCT(3)
DEFINE_IIR_STRUCT(4)
DEFINE_IIR_STRUCT(5)
DEFINE_IIR_STRUCT(6)
#define IIR_STRUCT IIR6_STRUCT

#endif // _DSP_LIBARY_STRUCTURES_H_
