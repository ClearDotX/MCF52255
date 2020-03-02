/*********************************************
 Freescale Semiconductor 2007
 (c) COPYRIGHT Freescale Semiconductor 2007
 ALL RIGHTS RESERVED.
**********************************************/

/* Preprocessor defines for the DSP library */

#ifndef _DSP_LIBRARY_DEFINES_H_
#define _DSP_LIBRARY_DEFINES_H_

/******************************************************************************/
/*                               GENERAL                                      */
/******************************************************************************/
	
/* set to save and restore processor state within assembly functions */
#define SAVE_RESTORE_STATE

/* set to pass function arguments on the stack. otherwise assume arg is passed in A0 */
/* #define USE_STACK_ARGS */

/* set to use A6 stack frame */
/* #define USE_A6_STACK_FRAME */

/******************************************************************************/
/*                            IIR FILTER                                      */
/******************************************************************************/

/* offsets into IIR data structure. since the only variable-length
   element is always last, these offsets work for all iir filter
   structures regardless of order */
#define IIR_OUTPUT_OFFSET		0
#define IIR_NUM_SF_OFFSET		2
#define IIR_DEN_SF_OFFSET		3
#define IIR_INPUT_OFFSET                4
#define	IIR_FLAGS_OFFSET		8
#define IIR_COEF_OFFSET			12
#define IIR_ORDER_OFFSET		16
#define IIR_BUFFER_OFFSET		20

#endif // _DSP_LIBRARY_DEFINES_H_
