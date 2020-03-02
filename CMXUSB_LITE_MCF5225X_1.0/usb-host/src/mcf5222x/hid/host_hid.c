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
#include "target.h"
#include "usb-drv/usb_host.h"
#include "usb-drv/usb_utils.h"
#include "hid_usage.h"
#include "host_hid.h"
#include "host_hid_mouse.h"
#include "host_hid_kbd.h"

#define HIDDESC_HID       0x21
#define HIDDESC_REPORT    0x22
#define HIDDESC_PHYSICAL  0x23

#define HIDRQ_GET_REPORT    0x1
#define HIDRQ_GET_IDLE      0x2
#define HIDRQ_GET_PROTOCOL  0x3

#define HIDRQ_SET_REPORT    0x9
#define HIDRQ_SET_IDLE      0xa
#define HIDRQ_SET_PROTOCOL  0xb

#define REPORT_ITEM_LIST_SIZE   100u

/* this type is used to access (read/write) short items in a 
   report */
typedef struct {
  hcc_u16 offset;
  hcc_u8 usage;
  hcc_u8 usage_page;
  hcc_u8 absolute;
  hcc_u8 length;
  hcc_u8 shr;
  hcc_u8 mask;
} report_item_t;

/* this structure will hold information about report item 
   properties while pharsing the report descriptor */
typedef struct {
  hcc_u8 usage;
  hcc_u8 usage_page;
  hcc_u32 log_min;
  hcc_u32 log_max;
  hcc_u32 rep_size;
  hcc_u32 rep_count;
} item_prop_t;

/* holds data needed to manage the HID driver */
hid_info_t hid_info;

/* number of allocated entryes in the following list */
hcc_u8 report_items;
/* each entry describes one HID report item */
report_item_t report_item_list[REPORT_ITEM_LIST_SIZE];

/* get the sepcified report over the control channel */
hcc_u8 hid_set_report(hcc_u8 report_id, hcc_u8 *buffer, hcc_u16 length)
{
  hcc_u8 setup[8];
  fill_setup_packet(setup, STP_DIR_OUT, STP_TYPE_CLASS, STP_RECIPIENT_IFC,
     HIDRQ_SET_REPORT, (hcc_u16)((0x02<<8) | report_id), hid_info.ifc_ndx,
     length);

  if (((hcc_u16)-1)==host_send_control(setup, buffer, 0))
  {
    return(1);
  }
  return(0);
}


/* get the sepcified report over the control channel */
hcc_u8 hid_get_report(hcc_u8 report_id, hcc_u8 *buffer, hcc_u16 length)
{
  hcc_u8 setup[8];
  fill_setup_packet(setup, STP_DIR_IN, STP_TYPE_CLASS, STP_RECIPIENT_IFC,
     HIDRQ_GET_REPORT, (hcc_u16)((0x01<<8) | report_id), hid_info.ifc_ndx,
     length);

  if (((hcc_u16)-1)==host_receive_control(setup, buffer, 0))
  {
    return(1);
  }
  return(0);
}

/* set protocol to boot/nonboot version */
int hid_set_protocol(hcc_u8 boot)
{
  hcc_u8 setup[8];
  fill_setup_packet(setup, STP_DIR_OUT, STP_TYPE_CLASS, STP_RECIPIENT_IFC,
     HIDRQ_SET_PROTOCOL, (hcc_u8)(boot ? 0 : 1), hid_info.ifc_ndx, 0);
  return(host_send_control(setup, 0, 0));
}

/* set idle time of device */
int hid_set_idle(hcc_u8 dur, hcc_u8 report_id)
{
  hcc_u8 setup[8];
  fill_setup_packet(setup, STP_DIR_OUT, STP_TYPE_CLASS, STP_RECIPIENT_IFC,
     HIDRQ_SET_IDLE, (hcc_u16)((dur<<8) | report_id), hid_info.ifc_ndx, 0);
  return(host_send_control(setup, 0, 0));
}

static int hid_search_ifc(void)
{
  device_info_t dev_inf;
  ifc_info_t ifc_info;
  cfg_info_t cfg_info;
  hcc_u8 cfg;

  hid_info.hidver=0;
  hid_info.ifcd_offset=0;

  /* see what device is this */
  if (get_device_info(&dev_inf))
  {
    return(0);
  }

  /* HID devices have their class info in the interface descriptor. */
  /* check all configurations */
  for(cfg=0; cfg < dev_inf.ncfg; cfg++)
  {
    hcc_u16 ifc;

    /* get the configuration descriptor */
    if (get_cfg_desc(cfg))
    {
      continue;
    };

    /* find a HID interface */
    ifc=find_ifc_csp(0x03, 0, 0);

    if (!ifc)
    {
      continue;
    }

    get_ifc_info(&ifc_info, ifc);

    if(ifc_info.sclas == 0x0)
    { /* non boot device */
       hid_info.boot=0;
    }
    else if (ifc_info.sclas == 0x1)
    { /* boot device */
      if (ifc_info.protocol < 3)
      {
        hid_info.boot=ifc_info.protocol;
      }
      else
      { /* bad protocol value, skipp interface */
        continue;
      }
    }
    /* we only handle the first HID interface. */

    get_cfg_info(&cfg_info);
    hid_info.cfg_ndx=cfg_info.ndx;

    hid_info.ifc_ndx=ifc_info.ndx;
    hid_info.ifcd_offset=ifc;
    hid_info.alt_set=ifc_info.alt_set;
    return(1);
  }
  return(0);
}
/* Startup HID driver. */
hcc_u8 hid_init(void)
{

  hid_info.alt_set=0;
  hid_info.boot=0;
  hid_info.country=0;
  hid_info.hidver=0;
  hid_info.ifc_ndx=0;
  hid_info.ifcd_offset=0;
  hid_info.in_it_ep=0;
  hid_info.out_it_ep=0;

  /* look for an interface supporing the HID class */
  if (hid_search_ifc())
  {
    hcc_u16 ep_ofs;
    hcc_u8 ep;
    ifc_info_t ifc_inf;

    /* Inicialize endpoints.  */
    get_ifc_info(&ifc_inf, hid_info.ifcd_offset);
    ep_ofs=hid_info.ifcd_offset;
    for(ep=0; ep < ifc_inf.nep; ep++)
    {
      ep_info_t ep_inf;
      ep_ofs=find_descriptor(STDDTYPE_ENDPOINT, ep_ofs, 1);
      get_ep_info(&ep_inf, ep_ofs);
      /* Note: HID ifc shall only have interrupt endpoints so we ignore other
                 ep types.
               A HID device shall have one IN and may have one OUT endpoint. We
               will use the last IN and OUT endpoint defined. */
      if (ep_inf.type==EPTYPE_INT)
      {
        if (ep_inf.address > 0x80)
        {
          hid_info.in_it_ep=host_add_ep(ep_inf.type, (hcc_u8)(ep_inf.address & 0x7f), ep_inf.interval, (hcc_u16)ep_inf.psize);
        }
        else
        {
          hid_info.out_it_ep=host_add_ep(ep_inf.type, (hcc_u8)(ep_inf.address & 0x7f), ep_inf.interval, ep_inf.psize);
        }
      }
      ep_ofs++;
    }

    /* Device shall have an interrupt IN endpoint */
    if (hid_info.in_it_ep == 0)
    {
      /* return error */
      return((hcc_u8)-1);
    }

    /* start device */
    if (set_config(hid_info.cfg_ndx))
    {
      /* request failed, return error */
      return((hcc_u8)-1);
    }
    else
    {
      return(0);
    }
  }
  else
  {
    /* not found, return error */
    return((hcc_u8)1u);
  }
}

hcc_u8 hid_get_dev_type(void)
{
  device_info_t devi;
  /* only mouse or keyboard devicas may have the boot filed set to a value
     other than 0 */
  if (hid_info.boot)
  {
    return(hid_info.boot);
  }
  /* Check if the device is a joystick. Since we do not have the code needed for 
     parsing report descriptors, we use pre-created data acccess structures
     for the supported joystick. We identify our joystick by vendor id and 
     product id, and release number. */
  if (!get_dev_desc())
  {
    get_device_info(&devi);
  
    if (devi.vid==0x06d6 && devi.pid==0x002f && devi.rev==0x0120)
    {
      return(HIDTYPE_JOY);
    }
  }  
  return(HIDTYPE_UNKNOWN);  
}
/****************************** END OF FILE **********************************/
