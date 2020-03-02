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
#ifndef _USB_UTILS_H_
#define _USB_UTILS_H_

/* Standard descriptor types */
#define STDDTYPE_DEVICE        1u
#define STDDTYPE_CONFIGURATION 2u
#define STDDTYPE_STRING        3u
#define STDDTYPE_INTERFACE     4u
#define STDDTYPE_ENDPOINT      5u

/* Setup packet fields. */
#define STP_DIR_IN            (1u<<7)
#define STP_DIR_OUT           (0u<<7)
#define STP_TYPE_STD          (0u<<5)
#define STP_TYPE_CLASS        (1u<<5)
#define STP_TYPE_VENDOR       (2u<<5)
#define STP_RECIPIENT_DEVICE  (0<<0)
#define STP_RECIPIENT_IFC     (1<<0)
#define STP_RECIPIENT_ENDPT   (2<<0)
#define STP_RECIPIENT_OTHER   (3<<0)

/* Standard request values */
#define STDRQ_GET_STATUS         0u
#define STDRQ_CLEAR_FEATURE      1u
#define STDRQ_SET_FEATURE        3u
#define STDRQ_SET_ADDRESS        5u
#define STDRQ_GET_DESCRIPTOR     6u
#define STDRQ_SET_DESCRIPTOR     7u
#define STDRQ_GET_CONFIGURATION  8u
#define STDRQ_SET_CONFIGURATION  9u
#define STDRQ_GET_INTERFACE      10u
#define STDRQ_SET_INTERFACE      11u
#define STDRQ_SYNCH_FRAME        12u


#define DBUFFER_SIZE    255u

typedef enum {
  stderr_none=0,
  stderr_host,
  stderr_bad_desc
} std_error_t;

extern std_error_t std_error;

extern int get_dev_desc(void);
extern int get_cfg_desc(hcc_u8 ndx);
extern int set_address(hcc_u8 address);
extern int set_config(hcc_u8 cfg);
extern int get_device_info(device_info_t *res);
extern int get_ifc_info(ifc_info_t *res, hcc_u16 offset);
extern hcc_u16 find_ifc_ndx(hcc_u8 ndx);
extern hcc_u16 find_ifc_csp(hcc_u8 class, hcc_u8 sclass, hcc_u8 protocol);
extern hcc_u16 find_descriptor(hcc_u8 type, hcc_u16 start, hcc_u8 next);
extern int get_ep_info(ep_info_t *res, hcc_u16 offset);
extern void fill_setup_packet(hcc_u8* dst, hcc_u8 dir, hcc_u8 type, hcc_u8 recipient,
                       hcc_u8 req, hcc_u16 val, hcc_u16 ndx, hcc_u16 len);
extern int get_cfg_info(cfg_info_t *res);
extern int set_ep0_psize(void);
extern int enumerate_device(void);
extern hcc_u8 dbuffer[DBUFFER_SIZE];
#endif
/****************************** END OF FILE **********************************/
