/*********************************************
 Freescale Semiconductor 2007
 (c) COPYRIGHT Freescale Semiconductor 2007
 ALL RIGHTS RESERVED.
**********************************************/

/********************************************************************/
/* includes                                                         */
/********************************************************************/
#include "common.h"
#include "dsp_library.h"
#include "hands_on_lab_exercises.h"

/*******************************************************************/
/* main loop                                                       */
/*******************************************************************/
void main (void) {

  /* print friendly banner */
  ANSI_cls();
  printf("Built on %s %s %4x\n",__DATE__, __TIME__,__MWERKS__ );
  printf("USB library copyright CMX systems \n");
  printf("For info on USB and USB software \n");
  printf("please see:  www.freescale.com/usb \n");
  Print_Core_info();
  printf("\n\n");

  /* start hands on lab */
  hands_on_lab_main();
}

/********************************************************************/
/* user interface to select interrupt functions.                    */
/********************************************************************/
void sw1_handler(void)  {}
void sw2_handler(void)  {}
void sw3_handler(void)  {}
void rti_1st_event()    {
  hands_on_lab_run();
}
void rti_2nd_event()    {}
void rti_3rd_event()    {}
void rti_4th_event()    {}
void rti_5th_event()    {
#ifdef HANDS_ON_LAB_WITH_USB
  hands_on_lab_print(eUSB);
#endif
//  hands_on_lab_print(eUART1);
}
void rti_6th_event()    {}

/*********************************************************************
 *
 * Copyright:
 *	Freescale Semiconductor, INC. All Rights Reserved.
 *  You are hereby granted a copyright license to use, modify, and
 *  distribute the SOFTWARE so long as this entire notice is
 *  retained without alteration in any modified and/or redistributed
 *  versions, and that such modified versions are clearly identified
 *  as such. No licenses are granted by implication, estoppel or
 *  otherwise under any patents or trademarks of Freescale
 *  Semiconductor, Inc. This software is provided on an "AS IS"
 *  basis and without warranty.
 *
 *  To the maximum extent permitted by applicable law, Freescale
 *  Semiconductor DISCLAIMS ALL WARRANTIES WHETHER EXPRESS OR IMPLIED,
 *  INCLUDING IMPLIED WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A
 *  PARTICULAR PURPOSE AND ANY WARRANTY AGAINST INFRINGEMENT WITH
 *  REGARD TO THE SOFTWARE (INCLUDING ANY MODIFIED VERSIONS THEREOF)
 *  AND ANY ACCOMPANYING WRITTEN MATERIALS.
 *
 *  To the maximum extent permitted by applicable law, IN NO EVENT
 *  SHALL Freescale Semiconductor BE LIABLE FOR ANY DAMAGES WHATSOEVER
 *  (INCLUDING WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS,
 *  BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR OTHER
 *  PECUNIARY LOSS) ARISING OF THE USE OR INABILITY TO USE THE SOFTWARE.
 *
 *  Freescale Semiconductor assumes no responsibility for the
 *  maintenance and support of this software
 ********************************************************************/
