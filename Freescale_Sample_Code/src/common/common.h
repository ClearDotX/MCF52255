/*
 * File:		common.h
 * Purpose:		File to be included by all project files
 *
 * Notes:
 * 
 * License:     All software covered by license agreement in -
 *              docs/Freescale_Software_License.pdf
 */

#ifndef _COMMON_H_
#define _COMMON_H_

/********************************************************************/

/*
 * Debug prints ON (#define) or OFF (#undef)
 */
#undef DEBUG_PRINT 
#undef DEBUG_PRINT_D0D1 

/* 
 * Include the generic CPU header file 
 */
#include "mcf5xxx.h"

/* 
 * Include the specific CPU header file 
 */
#include "mcf5225x.h"

/*Determine which board header file to use. DEMO and EVB boards use the same */
#if TWR_MCF5225X
   #include "mcf5225x_twr.h"
#else
   #include "mcf5225x_evb.h"
#endif

/* 
 * Include any toolchain specfic header files 
 */
#if (defined(__MWERKS__)  || defined(__CWCC__))
#include "build/cw/cw.h"
#elif (defined(__DCC__))
#include "build/wrs/diab.h"
#elif (defined(__ghs__))
#include "build/ghs/ghs.h"
#endif
 
/* 
 * Include common utilities
 */
#include "assert.h"
#include "io.h"
#include "stdlib.h"


/********************************************************************/

#endif /* _COMMON_H_ */
