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
#ifndef _HOST_HID_JOY_H_
#define _HOST_HID_JOY_H_

extern void hid_joy_init(void);
extern hcc_u16 hid_joy_get_x(void);
extern hcc_u16 hid_joy_get_y(void);
extern hcc_u16 hid_joy_get_rz(void);
extern hcc_u16 hid_joy_get_slider(void);
extern hcc_u8 hid_joy_get_button(hcc_u8 b);
extern hcc_u16 hid_joy_get_hat(void);
extern int hid_joy_process(void);

#endif
/****************************** END OF FILE **********************************/
