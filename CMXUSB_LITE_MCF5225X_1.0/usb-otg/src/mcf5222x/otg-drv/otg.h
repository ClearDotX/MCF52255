/***************************************************************************
 *
 *            Copyright (c) 2007 by CMX Systems, Inc.
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
#ifndef _OTG_H_
#define _OTG_H_

#include "hcc_types.h"
#include "usb-drv/usb.h"

#define USB_FILL_OTG_DESC(hnp, srp) \
  (hcc_u8)3, 0x9, ((hnp) ? (1<<1) : 0) | ((srp) ? (1<<0) : 0)

typedef enum {
  otgr_none,
  otgr_a,
  otgr_b
} otg_role_t;

typedef enum {
  otgm_none,
  otgm_host,
  otgm_peripheral
} otg_mode_t;

extern void otg_init(void);
extern hcc_u8 otg_host_req_bus(void);
extern hcc_u8 otg_host_drop_bus(void);
extern hcc_u8 otg_device_req_bus(void);
extern hcc_u8 otg_device_drop_bus(void);
extern otg_role_t otg_get_role(void);
extern void otg_process(void);
extern hcc_u8 otg_get_sleep(void);
extern otg_mode_t otg_get_mode(void);
extern callback_state_t usb_ep0_otg_callback(void);
#endif
/****************************** END OF FILE **********************************/
