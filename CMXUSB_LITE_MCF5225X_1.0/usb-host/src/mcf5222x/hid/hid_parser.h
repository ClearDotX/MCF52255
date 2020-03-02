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
#ifndef _HID_PARSER_H_
#define _HID_PARSER_H_

#include "hcc_types.h"
#include "hid_usage.h"

/* Report data item. */
typedef struct {
  hcc_u8  *data;
  hcc_u32 size;
  hcc_u8  shift;
  hcc_u8  count;
  hcc_u8  sign;
  hcc_u32 logical_min;  /*min value device can return*/
  hcc_u32 logical_max;  /*max value device can return*/
  hcc_u32 physical_min; /*min vale read can report*/
  hcc_u32 physical_max; /*max value read can report*/
  hcc_u32 resolution;
} rp_item_t;

typedef struct{
  hcc_u16 usage_page;
  hcc_u16 usage_min;
  hcc_u16 usage_max;  
  hcc_u8 level;
  rp_item_t *rpi;  
} report_tree_item_t;

hcc_u32 read_item(rp_item_t *ri, hcc_u8 ndx);
hcc_u32 write_item(rp_item_t *ri, hcc_u32 value, hcc_u8 ndx);

#endif
/****************************** END OF FILE **********************************/
