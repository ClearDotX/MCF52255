/****************************************************************************
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
#include "mcf5222x_reg.h"
#include "usb-drv/usb.h"
#include "target.h"
#include "hid-demo/hid_mouse.h"
#include "hid-demo/hid_kbd.h"
#include "hid-demo/hid_generic.h"
#include "terminal/hcc_terminal.h"
/****************************************************************************
 ************************** Macro definitions *******************************
 ***************************************************************************/
/* none */

/****************************************************************************
 ************************** Function predefinitions. ************************
 ***************************************************************************/
/* none */

/****************************************************************************
 ************************** Global variables ********************************
 ***************************************************************************/
/* none */

/****************************************************************************
 ************************** Module variables ********************************
 ***************************************************************************/
/* none */

/****************************************************************************
 ************************** Function definitions ****************************
 ***************************************************************************/
void device_demo_app();
void device_demo_app()
{
  hcc_u8 tmp;
  /* See in what mode should we run. */
  tmp = (hcc_u8)(SW1_ACTIVE() ? 1 : 0);
  tmp |= (hcc_u8)(SW2_ACTIVE() ? 2 : 0);

  /* Start the right HID application. */
  switch(tmp)
  {
  case 0:
  default:
    print("Starting in keyboard mode\r\n");
    set_mode(dm_kbd);
    hid_kbd();
    print("Keyboard stopped\r\n");
    break;
  case 1:
    print("Starting in mouse mode\r\n");
    set_mode(dm_mouse);
    hid_mouse();
    print("Mouse stopped.\r\n");
    break;
  case 2:
    print("Starting in generic HID mode\r\n");    
    set_mode(dm_generic);
    hid_generic();
    print("Application stopped\r\n");
    break;
  }
}
/****************************** END OF FILE **********************************/
