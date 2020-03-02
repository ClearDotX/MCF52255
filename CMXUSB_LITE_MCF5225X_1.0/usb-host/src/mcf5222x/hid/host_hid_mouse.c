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
#include "hid_parser.h"
/*****************************************************************************
 * External references.
 *****************************************************************************/
/* none */
/*****************************************************************************
 * Local types.
 *****************************************************************************/
/* none */
/*****************************************************************************
 * Macro definitions.
 *****************************************************************************/
/* Mouse states */
#define HMST_INVALID    0u
#define HMST_ACTIVE     1u
/*****************************************************************************
 * Module variables.
 *****************************************************************************/
/* Mouse state info. All mouses connected to the system shall modify this
   structure. Currently only one mouse is supported. */
struct {
  hcc_u8 state;
  hcc_u16 x;
  hcc_u16 y;
  hcc_u8 buttons[3];
  hcc_u8 report_data[4];
} static hid_mouse_info;
/* These structures define how to access items in a HID mouse report, if the
   mouse talks the boot protocol. Nearly all mouse is compatibile with this
   protocol. */
/* Access button 1 state. */
static const rp_item_t imp_0_b1={
  hid_mouse_info.report_data+0, /*data*/
  1,     /*size*/
  0,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  1,     /*max value read can return*/
  0,     /*min vale device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};
/* Access button 2 state. */
static const rp_item_t imp_0_b2={
  hid_mouse_info.report_data+0, /*data*/
  1,     /*size*/
  1,     /*shift*/
  0,     /*count (only for array items)*/  
  0,     /*signed?*/
  0,     /*min value read can return*/
  1,     /*max value read can return*/
  0,     /*min vale device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};
/* Access button 3 state. */   
static const rp_item_t imp_0_b3={
  hid_mouse_info.report_data+0, /*data*/
  1,     /*size*/
  2,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  1,     /*max value read can return*/
  0,     /*min vale device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};
/* Access x coordinate change. */
static const rp_item_t imp_0_x={
  hid_mouse_info.report_data+1, /*data*/
  8,     /*size*/
  0,     /*shift*/
  0,     /*count (only for array items)*/  
  1,     /*signed?*/
  0,     /*min value read can return*/
  0xff,  /*max value read can return*/
  0,     /*min vale device can report*/
  0xff,  /*max value device can report*/
  1      /*resolution*/
};
/* Access y coordinate change. */
static const rp_item_t imp_0_y={
  hid_mouse_info.report_data+2, /*data*/
  8,     /*size*/
  0,     /*shift*/
  0,     /*count (only for array items)*/  
  1,     /*signed?*/
  0,     /*min value read can return*/
  0xff,  /*max value read can return*/
  0,     /*min vale device can report*/
  0xff,  /*max value device can report*/
  1      /*resolution*/
};
static const report_tree_item_t b_mouse_report_tree[] = {
  {0x0001, 0x0002, 0x0000, 0, 0},  /* application collection */
  {0x0001, 0x0001, 0x0000, 1, 0},  /* physical collection */
  {0x0009, 0x0001, 0x0000, 2, (rp_item_t *) &imp_0_b1}, /* button 1 */
  {0x0009, 0x0002, 0x0000, 2, (rp_item_t *) &imp_0_b2}, /* button 2 */
  {0x0009, 0x0003, 0x0000, 2, (rp_item_t *) &imp_0_b3}, /* button 3 */
  {0x0001, 0x0030, 0x0000, 2, (rp_item_t *) &imp_0_x},  /* x pos */
  {0x0001, 0x0031, 0x0000, 2, (rp_item_t *) &imp_0_y},  /* y pos */
  {0x0000, 0x0000, 0xff, 0}
};
/*****************************************************************************
 * Function predefinitions.
 *****************************************************************************/
/* none */
/*****************************************************************************
 * Function definitions.
 *****************************************************************************/
/*****************************************************************************
 * Name:
 *    hid_mouse_init
 * In:
 *    -
 * Out:
 *    -
 *
 * Description:
 *    Iitialize mose driver.
 *
 * Assumptions:
 *    -
 *****************************************************************************/
void hid_mouse_init(void)
{
  hid_mouse_info.state=HMST_INVALID;
  hid_mouse_info.x=0;
  hid_mouse_info.y=0;
  hid_mouse_info.buttons[0]=0;
  hid_mouse_info.buttons[1]=0;
  hid_mouse_info.buttons[2]=0;
  hid_mouse_info.report_data[0]=0;
  hid_mouse_info.report_data[1]=0;
  hid_mouse_info.report_data[2]=0;
  hid_mouse_info.report_data[3]=0;  
}
/*****************************************************************************
 * Name:
 *    hid_mouse_get_x
 * In:
 *    -
 * Out:
 *    Current value of X offset.
 *
 * Description:
 *    Returns X offset.
 *
 * Assumptions:
 *    -
 *****************************************************************************/
hcc_u16 hid_mouse_get_x(void)
{
  return(hid_mouse_info.x);
}
/*****************************************************************************
 * Name:
 *    hid_mouse_get_y
 * In:
 *    -
 * Out:
 *    Current value of Y offset.
 *
 * Description:
 *    Returns Y offset.
 *
 * Assumptions:
 *    -
 *****************************************************************************/
hcc_u16 hid_mouse_get_y(void)
{
  return(hid_mouse_info.y);
}
/*****************************************************************************
 * Name:
 *    hid_mouse_get_button
 * In:
 *    b - index of button.
 * Out:
 *    0 - button released
 *    1 - button pressed
 *
 * Description:
 *    Return state os a button.
 *
 * Assumptions:
 *    -
 *****************************************************************************/
hcc_u8 hid_mouse_get_button(hcc_u8 b)
{
  if (b>2)
  {
    return(0);
  }
  return(hid_mouse_info.buttons[b]);
}
/*****************************************************************************
 * Name:
 *    hid_mouse_convert
 * In:
 *    -
 * Out:
 *    -
 *
 * Description:
 *    Update stored values with values got in the last report. Some values
 *    contain relative data and thus instead of overwriting we add the cnahge.
 *
 * Assumptions:
 *    -
 *****************************************************************************/
static void hid_mouse_convert(void)
{
  int tmp;
  tmp=hid_mouse_info.x+(signed int)read_item((rp_item_t *) &imp_0_x, 0);
  if (tmp < 0)
  {
    tmp=0;
  }
  if (tmp > 0xffff)
  {
    tmp=0xffff;
  }
  hid_mouse_info.x=(hcc_u16)tmp;
  tmp=hid_mouse_info.y+(signed int)read_item((rp_item_t *) &imp_0_y,0 );
  if (tmp < 0)
  {
    tmp=0;
  }
  if (tmp > 0xffff)
  {
    tmp=0xffff;
  }
  hid_mouse_info.y=(hcc_u16)tmp;
  hid_mouse_info.buttons[0]=(hcc_u8)read_item((rp_item_t *) &imp_0_b1, 0);
  hid_mouse_info.buttons[1]=(hcc_u8)read_item((rp_item_t *) &imp_0_b2, 0);
  hid_mouse_info.buttons[2]=(hcc_u8)read_item((rp_item_t *) &imp_0_b3, 0);
}
/*****************************************************************************
 * Name:
 *    hid_mouse_process
 * In:
 *    -
 * Out:
 *    0 - all ok
 *    1 - error encountered
 *
 * Description:
 *    Give CPU time to mouse driver. This function shall be called 
 *    periodically. It handles mouse states, and will update values readable
 *    trough the API.
 *
 * Assumptions:
 *    -
 *****************************************************************************/
int hid_mouse_process(void)
{
  hcc_u32 r;
  switch(hid_mouse_info.state)
  {
  case HMST_INVALID:
    /* Since we use the values we already have when converiong, we set all our
       variables to default state (0). */
    hid_mouse_init();
    /* Get default values from device. */
    //if (!hid_get_report(0, hid_mouse_info.report_data, sizeof(hid_mouse_info.report_data)))
    //comment by shenli : As some DELL mouse does not support this request, and it might cause the mouse STALL,
    //so I just remove this position init request
    {
      /* Convert report data. */
      hid_mouse_convert();
      /* Change state. */
      hid_mouse_info.state=HMST_ACTIVE;
    }
   // else
   // {
      /* Failed to get report. Assume device has been disconnected. */
      //return(1);
   // }
    break;
  case HMST_ACTIVE:
    /* Get next report from device. */
    r=host_receive(hid_mouse_info.report_data, sizeof(hid_mouse_info.report_data), hid_info.in_it_ep);
    switch (r)
    {
    case -1u:
      /* Error reading data. Assume device hs been disconnected. */
      return(1);
    default:      
    case sizeof(hid_mouse_info.report_data):
      /* Right number of bytes received. Convert readed relative data to absolute
         values. */
      hid_mouse_convert();
      break;
    case 0:
      /* Device has no datat to send. See if device is still connected. */
      if (!host_has_device())
      {
        return(1);
      }
      break;
    }
    break;
  }
  return(0);
}
/****************************** END OF FILE **********************************/
