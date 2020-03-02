/***************************************************************************
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

#ifndef _USB_MST_H_
#define _USB_MST_H_

#include "hcc_types.h"

extern int usb_mst_reset(void);
extern int usb_mst_getnlun(void);
extern int usb_mst_transfer(hcc_u8 cmd_length, void *cmd, hcc_u8 dir, hcc_u32 buf_length, void *buf);
extern int usb_mst_init(void);

#endif
/****************************** END OF FILE **********************************/
