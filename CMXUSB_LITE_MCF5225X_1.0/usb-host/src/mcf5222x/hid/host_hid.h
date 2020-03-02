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
#ifndef _HOST_HID_H_
#define _HOST_HID_H_
#define HIDTYPE_UNKNOWN  0u
#define HIDTYPE_KBD      1u
#define HIDTYPE_MOUSE    2u
#define HIDTYPE_JOY      3u
typedef struct {
  hcc_u16 ifcd_offset; /* offset of interface descriptor used */ 
  hcc_u16 hidver;      /* HID version supported by the device */ 
  hcc_u8 boot;         /* type of boot-protocol device talks */
  hcc_u8 ifc_ndx;      /* index of used interface */
  hcc_u8 alt_set;      /* alternate setting of used interface */
  hcc_u8 cfg_ndx;      /* configuration for HID functionality */
  hcc_u8 country;      /* country code of device */
  hcc_u8 no_of_classd; /* number of class descriptors in device */
  hcc_u8 in_it_ep;
  hcc_u8 out_it_ep;
} hid_info_t;
extern hid_info_t hid_info;
extern hcc_u8 hid_init(void);
extern hcc_u8 hid_set_report(hcc_u8 report_id, hcc_u8 *buffer, hcc_u16 length);
extern hcc_u8 hid_get_report(hcc_u8 report_id, hcc_u8 *buffer, hcc_u16 length);
extern int hid_set_protocol(hcc_u8 boot);
extern int hid_set_idle(hcc_u8 dur, hcc_u8 report_id);
extern hcc_u8 hid_get_dev_type(void);
#endif
/****************************** END OF FILE **********************************/
