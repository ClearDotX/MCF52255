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
#include "host_hid_kbd.h"
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
/* KBD states */
#define HKST_INVALID    0u
#define HKST_ACTIVE     1u
/*****************************************************************************
 * Module variables.
 *****************************************************************************/
struct {
  hcc_u8 state;
  hcc_u8 lctrl;
  hcc_u8 lshift;
  hcc_u8 lalt;
  hcc_u8 lgui;
  hcc_u8 rctrl;
  hcc_u8 rshift;
  hcc_u8 ralt;
  hcc_u8 rgui;
  hcc_u8 keys[6];
  hcc_u8 in_report_data[8];
  hcc_u8 out_report_data[1];
} static hid_kbd_info;
static const rp_item_t imp_0_lctrl={
  hid_kbd_info.in_report_data+0, /*data*/
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
static const rp_item_t imp_0_lshift={
  hid_kbd_info.in_report_data+0, /*data*/
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
static const rp_item_t imp_0_lalt={
  hid_kbd_info.in_report_data+0, /*data*/
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
static const rp_item_t imp_0_lgui={
  hid_kbd_info.in_report_data+0, /*data*/
  1,     /*size*/
  3,     /*shift*/
  0,     /*count (only for array items)*/  
  0,     /*signed?*/
  0,     /*min value read can return*/
  1,     /*max value read can return*/
  0,     /*min vale device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};
static const rp_item_t imp_0_rctrl={
  hid_kbd_info.in_report_data+0, /*data*/
  1,     /*size*/
  4,     /*shift*/
  0,     /*count (only for array items)*/  
  0,     /*signed?*/
  0,     /*min value read can return*/
  1,     /*max value read can return*/
  0,     /*min vale device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};
static const rp_item_t imp_0_rshift={
  hid_kbd_info.in_report_data+0, /*data*/
  1,     /*size*/
  5,     /*shift*/
  0,     /*count (only for array items)*/  
  0,     /*signed?*/
  0,     /*min value read can return*/
  1,     /*max value read can return*/
  0,     /*min vale device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};
static const rp_item_t imp_0_ralt={
  hid_kbd_info.in_report_data+0, /*data*/
  1,     /*size*/
  6,     /*shift*/
  0,     /*count (only for array items)*/  
  0,     /*signed?*/
  0,     /*min value read can return*/
  1,     /*max value read can return*/
  0,     /*min vale device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};
static const rp_item_t imp_0_rgui={
  hid_kbd_info.in_report_data+0, /*data*/
  1,     /*size*/
  7,     /*shift*/
  0,     /*count (only for array items)*/  
  0,     /*signed?*/
  0,     /*min value read can return*/
  1,     /*max value read can return*/
  0,     /*min vale device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};
static const rp_item_t out_0_num={
  hid_kbd_info.out_report_data+0, /*data*/
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
static const rp_item_t out_0_caps_lock={
  hid_kbd_info.out_report_data+0, /*data*/
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
static const rp_item_t out_0_scroll_lock={
  hid_kbd_info.out_report_data+0, /*data*/
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
static const rp_item_t out_0_compose={
  hid_kbd_info.out_report_data+0, /*data*/
  1,     /*size*/
  3,     /*shift*/
  0,     /*count (only for array items)*/  
  0,     /*signed?*/  
  0,     /*min value read can return*/
  1,     /*max value read can return*/
  0,     /*min vale device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};
static const rp_item_t out_0_kana={
  hid_kbd_info.out_report_data+0, /*data*/
  1,     /*size*/
  4,     /*shift*/
  0,     /*count (only for array items)*/
  0,     /*signed?*/  
  0,     /*min value read can return*/
  1,     /*max value read can return*/
  0,     /*min vale device can report*/
  1,     /*max value device can report*/
  1      /*resolution*/
};
static const rp_item_t imp_0_key_array={
  hid_kbd_info.in_report_data+2, /*data*/
  8,     /*size*/
  0,     /*shift*/
  6,     /*count (only for array items)*/
  0,     /*signed?*/  
  0,     /*min value read can return*/
  101,   /*max value read can return*/
  0,     /*min vale device can report*/
  101,   /*max value device can report*/
  1      /*resolution*/
};
static const report_tree_item_t b_kbd_report_tree[] = {
  {0x0001, 0x0006, 0x0000, 0, 0},  /* application collection */
  {0x0007, 0x00e0, 0x0000, 1, (rp_item_t *) &imp_0_lctrl},
  {0x0007, 0x00e1, 0x0000, 1, (rp_item_t *) &imp_0_lshift},
  {0x0007, 0x00e2, 0x0000, 1, (rp_item_t *) &imp_0_lalt},
  {0x0007, 0x00e3, 0x0000, 1, (rp_item_t *) &imp_0_lgui},
  {0x0007, 0x00e4, 0x0000, 1, (rp_item_t *) &imp_0_rctrl},
  {0x0007, 0x00e5, 0x0000, 1, (rp_item_t *) &imp_0_rshift},
  {0x0007, 0x00e6, 0x0000, 1, (rp_item_t *) &imp_0_ralt},
  {0x0007, 0x00e7, 0x0000, 1, (rp_item_t *) &imp_0_rgui},
  {0x0005, 0x0001, 0x0000, 1, (rp_item_t *) &out_0_num},
  {0x0005, 0x0002, 0x0000, 1, (rp_item_t *) &out_0_caps_lock},
  {0x0005, 0x0003, 0x0000, 1, (rp_item_t *) &out_0_scroll_lock},
  {0x0005, 0x0004, 0x0000, 1, (rp_item_t *) &out_0_compose},
  {0x0005, 0x0005, 0x0000, 1, (rp_item_t *) &out_0_kana},
  {0x0007, 0x0000, 0x0065, 1, (rp_item_t *) &imp_0_key_array},
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
 *    hid_kbd_init
 * In:
 *    -
 * Out:
 *    -
 *
 * Description:
 *    Iitialize keyboard driver.
 *
 * Assumptions:
 *    -
 *****************************************************************************/
void hid_kbd_init(void)
{
  int x;
  hid_kbd_info.lctrl=hid_kbd_info.lshift=
     hid_kbd_info.lalt=hid_kbd_info.lgui=
     hid_kbd_info.rctrl=hid_kbd_info.rshift=
     hid_kbd_info.ralt=hid_kbd_info.rgui=0;
     
  hid_kbd_info.state=HKST_INVALID;
  for(x=0; x< sizeof(hid_kbd_info.in_report_data); x++)
  {
    hid_kbd_info.in_report_data[x]=0;
  }
  for(x=0; x< sizeof(hid_kbd_info.out_report_data); x++)
  {
    hid_kbd_info.out_report_data[x]=0;
  }
}
/*****************************************************************************
 * Name:
 *    hid_kbd_set_leds
 * In:
 *    num    - required status of num lock LED (0 off , 1 on)
 *    caps   - required status of CAPS lock LED (0 off , 1 on)
 *    scroll - required status of scroll lock LED (0 off , 1 on)
 *    compo  - required status of compose lock LED (0 off , 1 on)
 *    kana   - required status of kana lock LED (0 off , 1 on)
 * Out:
 *    -
 *
 * Description:
 *    Set status of LEDS on the keyboard.
 *
 * Assumptions:
 *    -
 *****************************************************************************/
void hid_kbd_set_leds(hcc_u8 num, hcc_u8 caps, hcc_u8 scroll, hcc_u8 compo, hcc_u8 kana)
{
  /* update report buffer */
  write_item((rp_item_t *) &out_0_num, (hcc_u32)(num ? 1 : 0), 0);
  write_item((rp_item_t *) &out_0_caps_lock, (hcc_u32)(caps ? 1 : 0), 0);  
  write_item((rp_item_t *) &out_0_scroll_lock, (hcc_u32)(scroll ? 1 : 0), 0);  
  write_item((rp_item_t *) &out_0_compose, (hcc_u32)(compo ? 1 : 0), 0);  
  write_item((rp_item_t *) &out_0_kana, (hcc_u32)(kana ? 1 : 0), 0);  
  
  /* send report to device */
  hid_set_report(0, hid_kbd_info.out_report_data, 1);
}
/*****************************************************************************
 * Name:
 *    hid_kbd_in_convert
 * In:
 *    -
 * Out:
 *    -
 *
 * Description:
 *    Update stored values with values got in the last report.
 *
 * Assumptions:
 *    -
 *****************************************************************************/
static void hid_kbd_in_convert(void)
{
  hcc_u8 x;
  hid_kbd_info.lctrl=(hcc_u8)read_item((rp_item_t *) &imp_0_lctrl, 0);
  hid_kbd_info.lshift=(hcc_u8)read_item((rp_item_t *) &imp_0_lshift, 0);
  hid_kbd_info.lalt=(hcc_u8)read_item((rp_item_t *) &imp_0_lalt, 0);
  hid_kbd_info.lgui=(hcc_u8)read_item((rp_item_t *) &imp_0_lgui, 0);
  hid_kbd_info.rctrl=(hcc_u8)read_item((rp_item_t *) &imp_0_rctrl, 0);
  hid_kbd_info.rshift=(hcc_u8)read_item((rp_item_t *) &imp_0_rshift, 0);
  hid_kbd_info.ralt=(hcc_u8)read_item((rp_item_t *) &imp_0_ralt, 0);
  hid_kbd_info.rgui=(hcc_u8)read_item((rp_item_t *) &imp_0_rgui, 0);
  for(x=0; x < sizeof(hid_kbd_info.keys); x++)
  {    
    hid_kbd_info.keys[x]=(hcc_u8)read_item((rp_item_t *) &imp_0_key_array, x);
  }
  
}
/*****************************************************************************
 * Name:
 *    hid_kbd_process
 * In:
 *    -
 * Out:
 *    0 - all ok
 *    1 - error encountered
 *
 * Description:
 *    Give CPU time to keyboard driver. This function shall be called
 *    periodically. It handles keyboard states, and will update values readable
 *    trough the API.
 *
 * Assumptions:
 *    -
 *****************************************************************************/
int hid_kbd_process(void)
{
  hcc_u32 r;
  switch(hid_kbd_info.state)
  {
  case HKST_INVALID:
    /* Set idle time to infinite. Deveice shall send a report only if we ask for
       a report. */
    hid_set_idle(0,0);
   
    /* Get the "default values". */
    if (!hid_get_report(0, hid_kbd_info.in_report_data, sizeof(hid_kbd_info.in_report_data)))
    {
      /* Convert report. */
      hid_kbd_in_convert();
      /* Switch state. */
      hid_kbd_info.state=HKST_ACTIVE;
    }
    else
    {
      /* Communication error. Dropp device. */
      return(1);
    }
    break;
  case HKST_ACTIVE:
    /* Get next report from device. */
    r=host_receive(hid_kbd_info.in_report_data, sizeof(hid_kbd_info.in_report_data), hid_info.in_it_ep);
    switch (r)
    {
    default:
    case -1u:
      /* Communication error, dropp device.  */
      return(1);
    case sizeof(hid_kbd_info.in_report_data):
      /* Got new values, convert data. */
      hid_kbd_in_convert();
      break;
    case 0:
      /* No data returned. Check if device is still connected. */
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
/*****************************************************************************
 * Name:
 *    hid_kbd_get_lshift
 * In:
 *    -
 * Out:
 *    0 - released
 *    1 - pressed
 *
 * Description:
 *    Get status of left shift.
 *
 * Assumptions:
 *    -
 *****************************************************************************/
hcc_u8 hid_kbd_get_lshift(void)
{
  return(hid_kbd_info.lshift);
}
/*****************************************************************************
 * Name:
 *    hid_kbd_get_rshift
 * In:
 *    -
 * Out:
 *    0 - released
 *    1 - pressed
 *
 * Description:
 *    Get status of right shift.
 *
 * Assumptions:
 *    -
 *****************************************************************************/
hcc_u8 hid_kbd_get_rshift(void)
{
  return(hid_kbd_info.rshift);
}
/*****************************************************************************
 * Name:
 *    hid_kbd_get_ralt
 * In:
 *    -
 * Out:
 *    0 - released
 *    1 - pressed
 *
 * Description:
 *    Get status of rigth alt.
 *
 * Assumptions:
 *    -
 *****************************************************************************/
hcc_u8 hid_kbd_get_ralt(void)
{
  return(hid_kbd_info.ralt);
}
/*****************************************************************************
 * Name:
 *    hid_kbd_get_lalt
 * In:
 *    -
 * Out:
 *    0 - released
 *    1 - pressed
 *
 * Description:
 *    Get status of left alt.
 *
 * Assumptions:
 *    -
 *****************************************************************************/
hcc_u8 hid_kbd_get_lalt(void)
{
  return(hid_kbd_info.lalt);
}
/*****************************************************************************
 * Name:
 *    hid_kbd_get_rctrl
 * In:
 *    -
 * Out:
 *    0 - released
 *    1 - pressed
 *
 * Description:
 *    Get status of right control.
 *
 * Assumptions:
 *    -
 *****************************************************************************/
hcc_u8 hid_kbd_get_rctrl(void)
{
  return(hid_kbd_info.rctrl);
}
/*****************************************************************************
 * Name:
 *    hid_kbd_get_lctrl
 * In:
 *    -
 * Out:
 *    0 - released
 *    1 - pressed
 *
 * Description:
 *    Get status of left control.
 *
 * Assumptions:
 *    -
 *****************************************************************************/
hcc_u8 hid_kbd_get_lctrl(void)
{
  return(hid_kbd_info.lctrl);
}
/*****************************************************************************
 * Name:
 *    hid_kbd_get_rgui
 * In:
 *    -
 * Out:
 *    0 - released
 *    1 - pressed
 *
 * Description:
 *    Get status of right gui.
 *
 * Assumptions:
 *    -
 *****************************************************************************/
hcc_u8 hid_kbd_get_rgui(void)
{
  return(hid_kbd_info.rgui);
}
/*****************************************************************************
 * Name:
 *    hid_kbd_get_lgui
 * In:
 *    -
 * Out:
 *    0 - released
 *    1 - pressed
 *
 * Description:
 *    Get status of left gui.
 *
 * Assumptions:
 *    -
 *****************************************************************************/
hcc_u8 hid_kbd_get_lgui(void)
{
  return(hid_kbd_info.lgui);
}
/*****************************************************************************
 * Name:
 *    hid_kbd_get_nkey
 * In:
 *    -
 * Out:
 *    Number of keys pressed.
 *
 * Description:
 *    Return the number of pressed keys.
 *
 * Assumptions:
 *    -
 *****************************************************************************/
hcc_u8 hid_kbd_get_nkey(void)
{
  hcc_u8 nkey;
  
  for(nkey=0; nkey < sizeof(hid_kbd_info.keys)/sizeof(hid_kbd_info.keys[0]); nkey++)
  {
    if (hid_kbd_info.keys[nkey] == 0
        || hid_kbd_info.keys[nkey] == 1)
    {        
      break;
    }
  }
  return(nkey);
}
/*****************************************************************************
 * Name:
 *    hid_kbd_get_key
 * In:
 *    Index of pressed key.
 * Out:
 *    Scan code.
 *
 * Description:
 *    Return the scan code of ndx th pressed key.
 *
 * Assumptions:
 *    -
 *****************************************************************************/
hcc_u8 hid_kbd_get_key(hcc_u8 ndx)
{
  return(hid_kbd_info.keys[ndx]);
}
/****************************** END OF FILE **********************************/
