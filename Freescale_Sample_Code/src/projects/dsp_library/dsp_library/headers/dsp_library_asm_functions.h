/*********************************************
 Freescale Semiconductor 2007
 (c) COPYRIGHT Freescale Semiconductor 2007
 ALL RIGHTS RESERVED.
**********************************************/

/* Prototypes for assembly functions */

#ifndef _DSP_LIBRARY_ASM_FUNCTIONS_H_
#define _DSP_LIBRARY_ASM_FUNCTIONS_H_

#include "dsp_library_defines.h"
#include "dsp_library_structures.h"

/******************************************************************************/
/*                      ASSEMBLY FUNCTION PROTOTYPES                          */
/******************************************************************************/


void iir2_asm(IIR2_STRUCT *);             // 2nd order IIR filter
void iir3_asm(IIR3_STRUCT *);             // 3rd order IIR filter
void iir4_asm(IIR4_STRUCT *);             // 4th order IIR filter
void iir5_asm(IIR5_STRUCT *);             // 5th order IIR filter
void iir6_asm(IIR6_STRUCT *);             // 6th order IIR filter

#endif // _DSP_LIBRARY_ASM_FUNCTIONS_H_
