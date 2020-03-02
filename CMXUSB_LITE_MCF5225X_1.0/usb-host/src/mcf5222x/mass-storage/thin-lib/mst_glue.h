#ifndef _MSTGLUE_H_
#define _MSTGLUE_H_

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

#ifdef __cplusplus
extern "C" {
#endif

unsigned char mst_readsector(void *_data,unsigned long sector);
unsigned char mst_writesector(void *_data,unsigned long sector);

#if F_FORMATTING
unsigned char mst_getphy(F_PHY *phy);
#endif

/******************************************************************************
 *
 *  definitions for highlevel
 *
 *****************************************************************************/
#define drv_readsector mst_readsector
#define drv_writesector mst_writesector

#if F_FORMATTING
#define drv_getphy mst_getphy
#endif

/******************************************************************************
 *
 *  End of mstglue.h.
 *
 *****************************************************************************/
#endif /* _MSTGLUE_H_ */
