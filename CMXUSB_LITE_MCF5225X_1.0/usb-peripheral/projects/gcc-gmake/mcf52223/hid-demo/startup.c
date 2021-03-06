/****************************************************************************
 *
 *            Copyright (c) 2006-2007 by CMX Systems, Inc.
 *
 * This software is copyrighted by and is the sole property of
 * CMX.  All rights, title, ownership, or other interests
 * in the software remain the property of CMX.  This
 * software may only be used in accordance with the corresponding
 * license agreement.  Any unauthorized use, duplication, transmission,
 * distribution, or disclosure of this software is expressly forbidden.
 *
 * This Copyright notice may not be removed or modified without prior
 * written consent of CMX.
 *
 * CMX reserves the right to modify this software without notice.
 *
 * CMX Systems, Inc.
 * 12276 San Jose Blvd. #511
 * Jacksonville, FL 32223
 * USA
 *
 * Tel:  (904) 880-1840
 * Fax:  (904) 880-1632
 * http: www.cmx.com
 * email: cmx@cmx.com
 *
 ***************************************************************************/
#include "hcc_types.h"


void low_level_init(void);
/*****************************************************************************
 * Do low level C inicialization.
 ****************************************************************************/
void low_level_init(void)
{
    extern char __COPY_START[];
    extern char __COPY_DST[];
    extern char __COPY_END[];
    extern char __ZERO_START[];
    extern char __ZERO_END[];
    register hcc_u32 n;
    register hcc_u8 *dp, *sp;


    /*
     * Move initialized data from ROM to RAM.
     */
    if (__COPY_START != __COPY_DST)
    {
        dp = (hcc_u8 *)__COPY_DST;
        sp = (hcc_u8 *)__COPY_START;
        n = (hcc_u32)(__COPY_END - __COPY_DST);
        while (n--)
            *dp++ = *sp++;
    }

    /*
     * Zero uninitialized data
     */
    if (__ZERO_START != __ZERO_END)
    {
        sp = (hcc_u8 *)__ZERO_START;
        n = (hcc_u32)(__ZERO_END - __ZERO_START);
        while (n--)
            *sp++ = 0;
    }
}
/****************************** END OF FILE **********************************/

