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
#include "host_hid_joy.h"
#include "hid_parser.h"
#define HJST_INVALID    0u
#define HJST_ACTIVE     1u
/* Joystick state info.  */
struct {
  hcc_u16 x;
  hcc_u16 y;
  hcc_u16 rz;
  hcc_u16 slider;
  hcc_u16 hat;
  hcc_u8 buttons[6];
  hcc_u8 in_report_data[6];
  hcc_u8 state;  
} static hid_joy_info;
/* These structures define how to access items in a HID joy report */
static rp_item_t imp_0_x={
  hid_joy_info.in_report_data+0, /*data*/
  8,     /*size*/
  0,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  0xff,  /*max value read can return*/
  0,     /*min value device can report*/
  0xff,  /*max value device can report*/
  1      /*resolution*/
};
static rp_item_t imp_0_y={
  hid_joy_info.in_report_data+1, /*data*/
  8,     /*size*/
  0,     /*shift*/  
  0,     /*count (only for array items)*/  
  0,     /*signed?*/
  0,     /*min value read can return*/
  0xff,  /*max value read can return*/
  0,     /*min value device can report*/
  0xff,  /*max value device can report*/
  1      /*resolution*/
};
static rp_item_t imp_0_rz={
  hid_joy_info.in_report_data+2, /*data*/
  8,     /*size*/
  0,     /*shift*/
  0,     /*count (only for array items)*/  
  0,     /*signed?*/
  0,     /*min value read can return*/
  0xff,  /*max value read can return*/
  0,     /*min value device can report*/
  0xff,  /*max value device can report*/
  1      /*resolution*/
};
static rp_item_t imp_0_slider={
  hid_joy_info.in_report_data+3, /*data*/
  8,     /*size*/
  0,     /*shift*/
  0,     /*count (only for array items)*/  
  0,     /*signed?*/
  0,     /*min value read can return*/
  0xff,  /*max value read can return*/
  0,     /*min value device can report*/
  0xff,  /*max value device can report*/
  1      /*resolution*/
};
static rp_item_t imp_0_hat={
  hid_joy_info.in_report_data+4, /*data*/
  4,     /*size*/
  0,     /*shift*/
  0,     /*count (only for array items)*/  
  0,     /*signed?*/
  1,     /*min value read can return*/
  8,     /*max value read can return*/
  0,     /*min value device can report*/
  0x13b, /*max value device can report*/
  0x2D   /*resolution*/
};
static rp_item_t imp_0_b1={
  hid_joy_info.in_report_data+4, /*data*/
  1,     /*size*/
  4,     /*shift*/
  0,     /*count (only for array items)*/  
  0,     /*signed?*/
  0,     /*min value read can return*/
  1,     /*max value read can return*/
  0,     /*min value device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};
static rp_item_t imp_0_b2={
  hid_joy_info.in_report_data+4, /*data*/
  1,     /*size*/
  5,     /*shift*/
  0,     /*count (only for array items)*/  
  0,     /*signed?*/
  0,     /*min value read can return*/
  1,     /*max value read can return*/
  0,     /*min value device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};
static rp_item_t imp_0_b3={
  hid_joy_info.in_report_data+4, /*data*/
  1,     /*size*/
  6,     /*shift*/
  0,     /*count (only for array items)*/  
  0,     /*signed?*/
  0,     /*min value read can return*/
  1,     /*max value read can return*/
  0,     /*min value device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};
static rp_item_t imp_0_b4={
  hid_joy_info.in_report_data+4, /*data*/
  1,     /*size*/
  7,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  1,     /*max value read can return*/
  0,     /*min value device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};
static rp_item_t imp_0_b5={
  hid_joy_info.in_report_data+5, /*data*/
  1,     /*size*/
  0,     /*shift*/
  0,     /*count (only for array items)*/  
  0,     /*signed?*/
  0,     /*min value read can return*/
  1,     /*max value read can return*/
  0,     /*min value device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};
static rp_item_t imp_0_b6={
  hid_joy_info.in_report_data+5, /*data*/
  1,     /*size*/
  1,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value read can return*/
  1,     /*max value read can return*/
  0,     /*min value device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};
static hcc_u8 out_report0_buffer[3];
static rp_item_t out_0_bcr={
  out_report0_buffer+0, /*data*/
  24,    /*size*/
  0,     /*shift*/
  0,     /*count (only for array items)*/  
  0,     /*signed?*/
  0,     /*min value device can return*/
  0xff,  /*max value device can return*/
  0,     /*min vale read can report*/
  0,     /*max value read can report*/
  1      /*resolution*/
};
static hcc_u8 feat_report0_buffer[3];
static rp_item_t feat_0_bcr={
  feat_report0_buffer+0, /*data*/
  24,    /*size*/
  0,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/
  0,     /*min value device can return*/
  0xff,  /*max value device can return*/
  0,     /*min vale read can report*/
  0,     /*max value read can report*/
  1      /*resolution*/
};

static report_tree_item_t xtrust_report_tree[] = {
  {0x0001, 0x0004, 0x0000, 0, 0},
  {0x0001, 0x0001, 0x0000, 1, 0},
  {0x0001, 0x0001, 0x0000, 2, 0},
  {0x0001, 0x0030, 0x0000, 2, &imp_0_x},
  {0x0001, 0x0031, 0x0000, 2, &imp_0_y},
  {0x0001, 0x0035, 0x0000, 2, &imp_0_rz},
  {0x0001, 0x0036, 0x0000, 2, &imp_0_slider},
  {0x0001, 0x0039, 0x0000, 2, &imp_0_hat},
  {0x0009, 0x0001, 0x0000, 2, &imp_0_b1},
  {0x0009, 0x0002, 0x0000, 2, &imp_0_b2},
  {0x0009, 0x0003, 0x0000, 2, &imp_0_b3},
  {0x0009, 0x0004, 0x0000, 2, &imp_0_b4},
  {0x0009, 0x0005, 0x0000, 2, &imp_0_b5},
  {0x0009, 0x0006, 0x0000, 2, &imp_0_b6},
  {0x0000, 0x0000, 0xff, 0}
};
/****************** HID joy stuff *************************/
/* joy states */
void hid_joy_init(void)
{
  hid_joy_info.state=HJST_INVALID;
  hid_joy_info.x=0;
  hid_joy_info.y=0;
  hid_joy_info.rz=0;
  hid_joy_info.slider=0;
  hid_joy_info.hat=0;
  hid_joy_info.buttons[0]=0;
  hid_joy_info.buttons[1]=0;
  hid_joy_info.buttons[2]=0;
  hid_joy_info.buttons[3]=0;
  hid_joy_info.in_report_data[0]=0;
  hid_joy_info.in_report_data[1]=0;
  hid_joy_info.in_report_data[2]=0;
  hid_joy_info.in_report_data[3]=0;
  hid_joy_info.in_report_data[4]=0;
  hid_joy_info.in_report_data[5]=0;
}
hcc_u16 hid_joy_get_x(void)
{
  return(hid_joy_info.x);
}
hcc_u16 hid_joy_get_y(void)
{
  return(hid_joy_info.y);
}
hcc_u16 hid_joy_get_rz(void)
{
  return(hid_joy_info.rz);
}
hcc_u16 hid_joy_get_slider(void)
{
  return(hid_joy_info.slider);
}
hcc_u16 hid_joy_get_hat(void)
{
  return(hid_joy_info.hat);
}
hcc_u8 hid_joy_get_button(hcc_u8 b)
{
  if (b>sizeof(hid_joy_info.buttons)/sizeof(hid_joy_info.buttons[0]))
  {
    return(0);
  }
  return(hid_joy_info.buttons[b]);
}
static void hid_joy_convert(void)
{
  hid_joy_info.x=(hcc_u16)read_item(&imp_0_x, 0);
  hid_joy_info.y=(hcc_u16)read_item(&imp_0_y, 0 );
  hid_joy_info.rz=(hcc_u16)read_item(&imp_0_rz, 0 );
  hid_joy_info.slider=(hcc_u16)read_item(&imp_0_slider, 0 );
  hid_joy_info.hat=(hcc_u16)read_item(&imp_0_hat, 0 );
  hid_joy_info.buttons[0]=(hcc_u8)read_item(&imp_0_b1, 0);
  hid_joy_info.buttons[1]=(hcc_u8)read_item(&imp_0_b2, 0);
  hid_joy_info.buttons[2]=(hcc_u8)read_item(&imp_0_b3, 0);
  hid_joy_info.buttons[3]=(hcc_u8)read_item(&imp_0_b4, 0);
  hid_joy_info.buttons[4]=(hcc_u8)read_item(&imp_0_b5, 0);
  hid_joy_info.buttons[5]=(hcc_u8)read_item(&imp_0_b6, 0);
}
int hid_joy_process(void)
{
  hcc_u32 r;
  switch(hid_joy_info.state)
  {
  case HJST_INVALID:
  
    /* Since we use the values we already have when converiong, we set all our 
       variables to default state (0). */
    hid_joy_init(); 
    hid_joy_info.state=HJST_ACTIVE;
    /* yes, there is no break. */
  case HJST_ACTIVE:
    /* Get next report from device. */
    r=host_receive(hid_joy_info.in_report_data, sizeof(hid_joy_info.in_report_data), hid_info.in_it_ep);
    switch (r)
    {
    default:
    case -1u:
      /* Error reading data. Assume device hs been disconnected. */
      return(1);            
    case sizeof(hid_joy_info.in_report_data):
      /* Right number of bytes received. Convert readed relative data to absolute
         values. */
      hid_joy_convert();
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
