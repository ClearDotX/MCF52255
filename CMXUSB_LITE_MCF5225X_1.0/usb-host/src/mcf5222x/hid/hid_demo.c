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
#include "host_hid.h"
#include "uart-drv/uart.h"
#include "mcf5222x_reg.h"
#include "host_hid_mouse.h"
#include "host_hid_kbd.h"
#include "host_hid_joy.h"
#include "utils/utils.h"

static void print(char *s)
{
  while(*s != '\0')
  {
    int c;
    do
    {
      c=uart_putch(*s);
    }while(c != (int)*s);
    s++;
  }
}

#ifndef ON_THE_GO

void enable_usb_pull_downs(void)
{
  MCF_USB_OTG_CTRL = MCF_USB_OTG_CTRL_DP_LOW | MCF_USB_OTG_CTRL_DM_LOW |MCF_USB_OTG_CTRL_OTG_EN;
  MCF_GPIO_PQSPAR |= MCF_GPIO_PQSPAR_PQSPAR5(3) | MCF_GPIO_PQSPAR_PQSPAR6(3);
}

hcc_u8 host_stp=0;
hcc_u8 sleep=0;
#define busy_wait()

#else
extern hcc_u8 host_stp;
extern hcc_u8 sleep;
extern void busy_wait(void);
#endif

#ifndef ON_THE_GO
int main(void)
#else
int hid_host_app(void);
int hid_host_app(void)
#endif
{

  struct {
    hcc_u16 x;
    hcc_u16 y;
    hcc_u8 b1;
    hcc_u8 b2;
    hcc_u8 b3;
    hcc_u8 changed;
  } m_data;

  struct {
    hcc_u8 keys[6];
    hcc_u8 r_shift;
    hcc_u8 l_shift;
    hcc_u8 r_alt;
    hcc_u8 l_alt;
    hcc_u8 r_ctrl;
    hcc_u8 l_ctrl;
    hcc_u8 r_gui;
    hcc_u8 l_gui;    
    hcc_u8 changed;
  } k_data;
  
  struct {
    hcc_u16 hat;
    hcc_u16 slider;
    hcc_u16 x;
    hcc_u16 y;
    hcc_u16 rz;  
    hcc_u8 b1;
    hcc_u8 b2;
    hcc_u8 b3;
    hcc_u8 b4;
    hcc_u8 b5;
    hcc_u8 b6;
    hcc_u8 changed;
  } j_data;
  
  hcc_u8 start=1;
  hcc_u8 hid_type=HIDTYPE_UNKNOWN;
  
  m_data.x=m_data.y=0;
  m_data.b1=m_data.b2=m_data.b3=0;
  m_data.changed=1;
  
#ifndef ON_THE_GO  
  hw_init();
  uart_init(115200, 1, 'n', 8);
  Usb_Vbus_On();

  host_init();
#endif

  if (host_stp)
  {
    return(0);
  }

  print("Host application started.\r\n");

  while(!host_stp)
  {
    hcc_u8 sleeping=0;
    busy_wait();
    if(start)
    {
      start=0;
    }
    else
    {
      /* a device is already connected, wait till it is disconnected */
      print("Waiting for device removal.\r\n");
      while(host_has_device() && !host_stp)
      {        
        busy_wait();
      }      
      print("Device disconnected.\r\n");
      
      if (host_stp)
      {  
        break;
      }
    }
      
    /* At this point no device is attached. Wait till attachment. */    
    print("Waiting for device...\r\n");
    while(!host_scan_for_device() && !host_stp)
    {
      busy_wait();
    }

    if (host_stp)
    {
      break;
    }     

    print("Device connected.\r\n");
    if (hid_init())
    {
      print("HID initialisation error.");
      continue;
    }
    
    hid_type=hid_get_dev_type();
    switch(hid_type)
    {
    case HIDTYPE_MOUSE:
      print("  Device is a HID mouse.\r\n");
      break;
    case HIDTYPE_KBD:
      print("  Device is a HID keyboard.\r\n");
      break;
    case HIDTYPE_JOY:
      print("  Device is the XTrust QZ 501 predator joystick.\r\n");    
      break;
    default:
      print("  Device is an unsupported HID device.\r\n");
      continue;
    }
    
    while(!host_stp)
    {
      busy_wait();
      
      if (!sleeping)
      {
        if (sleep)
        {
          print("Suspending BUS.\r\n");
          host_sleep();
          sleeping=1;
          continue;
        }
      }
      else
      {
        if (sleep==0)
        {
          print("Resuming BUS.\r\n");
          sleeping=0;
        }
        continue;
      }
      
      switch(hid_type)
      {
      case HIDTYPE_MOUSE:
        if (hid_mouse_process())
        {
          goto loop_exit;
        }
        if (m_data.x != hid_mouse_get_x())
        {
          m_data.changed=1;
          m_data.x=hid_mouse_get_x();
        }
        if (m_data.y != hid_mouse_get_y())
        {
          m_data.changed=1;
          m_data.y=hid_mouse_get_y();
        }
        if (m_data.b1 != hid_mouse_get_button(0))
        {
          m_data.changed=1;
          m_data.b1=hid_mouse_get_button(0);
        }
        if (m_data.b2 != hid_mouse_get_button(1))
        {
          m_data.changed=1;
          m_data.b2=hid_mouse_get_button(1);
        }
        if (m_data.b3 != hid_mouse_get_button(2))
        {
          m_data.changed=1;
          m_data.b3=hid_mouse_get_button(2);
        }
        
        if (m_data.changed)
        {
          char buf[20];
          m_data.changed=0;
          print("x offset=");
          itoa(m_data.x, buf, 7);
          print(buf);
          print(", y offset=");
          itoa(m_data.y, buf, 7);
          print(buf);
          print(", button1=");
          if (m_data.b1)
          {            
            print("  ON");
          }
          else
          {
            print(" OFF");
          }
          print(", button2=");
          if (m_data.b2)
          {            
            print("  ON");
          }
          else
          {
            print(" OFF");
          }
          print(", button3=");
          if (m_data.b3)
          {            
            print("  ON");
          }
          else
          {
            print(" OFF");
          }
          print("\r\n");
        }

        break;

      case HIDTYPE_KBD:

        if (hid_kbd_process())
        {
          goto loop_exit;
        }
        
        if (k_data.l_shift!=hid_kbd_get_lshift())
        {
          k_data.changed=1;
          k_data.l_shift=hid_kbd_get_lshift();
        }
        if (k_data.r_shift!=hid_kbd_get_rshift())
        {
          k_data.changed=1;        
          k_data.r_shift=hid_kbd_get_rshift();
        }
        
        if (k_data.l_alt!=hid_kbd_get_lalt())
        {
          k_data.changed=1;
          k_data.l_alt=hid_kbd_get_lalt();
        }
        
        if (k_data.r_alt!=hid_kbd_get_ralt())
        {
          k_data.changed=1;
          k_data.r_alt=hid_kbd_get_ralt();
        }
        
        if (k_data.l_ctrl!=hid_kbd_get_lctrl())
        {
          k_data.changed=1;
          k_data.l_ctrl=hid_kbd_get_lctrl();  
        }
             
        if (k_data.r_ctrl!=hid_kbd_get_rctrl())
        {
          k_data.changed=1;
          k_data.r_ctrl=hid_kbd_get_rctrl();
        }
        if (k_data.l_gui!=hid_kbd_get_lgui())
        {
          k_data.changed=1;
          k_data.l_gui=hid_kbd_get_lgui();  
        }
             
        if (k_data.r_gui!=hid_kbd_get_rgui())
        {
          k_data.changed=1;
          k_data.r_gui=hid_kbd_get_rgui();
        }
        
        {
          int nkey=hid_kbd_get_nkey();
          hcc_u8 key_ndx;
          for(key_ndx=0; key_ndx<nkey; key_ndx++)
          {
            if (k_data.keys[key_ndx]!=hid_kbd_get_key(key_ndx))
            {
              k_data.changed=1;
              k_data.keys[key_ndx]=hid_kbd_get_key(key_ndx);
            }
          }
          while(key_ndx<sizeof(k_data.keys)/sizeof(k_data.keys[0]))
          {
            if (k_data.keys[key_ndx]!=0)
            {
              k_data.changed=1;
              k_data.keys[key_ndx]=0;
            }            
            key_ndx++;
          }
        }
        
        if (k_data.changed)
        {
          int x;
          k_data.changed=0;
          if (k_data.l_shift | k_data.r_shift)
          {
            print("SHIFT ");
          }
          if (k_data.l_alt | k_data.r_alt)
          {
            print("ALT ");
          }
          if (k_data.l_ctrl | k_data.r_ctrl)
          {
            print("CTRL ");
          }
          if (k_data.l_gui | k_data.r_gui)
          {
            print("GUI ");
          }
          
          print("keycodes: ");
          for (x=0; x < sizeof(k_data.keys)/sizeof(k_data.keys[0]); x++)
          {
            char buf[8];
            itoa(k_data.keys[x], buf, 5);
            print(buf);
            print(" ");
          }
          print("\r\n");
          
        }
        break;

      case HIDTYPE_JOY:
        if (hid_joy_process())
        {
          goto loop_exit;
        }
        if (j_data.x != hid_joy_get_x())
        {
          j_data.changed=1;
          j_data.x=hid_joy_get_x();
        }
        if (j_data.y != hid_joy_get_y())
        {
          j_data.changed=1;
          j_data.y=hid_joy_get_y();
        }
        
        if (j_data.rz != hid_joy_get_rz())
        {
          j_data.changed=1;
          j_data.rz=hid_joy_get_rz();
        }

        if (j_data.slider != hid_joy_get_slider())
        {
          j_data.changed=1;
          j_data.slider=hid_joy_get_slider();
        }

        if (j_data.hat != hid_joy_get_hat())
        {
          j_data.changed=1;
          j_data.hat=hid_joy_get_hat();
        }
        
        if (j_data.b1 != hid_joy_get_button(0))
        {
          j_data.changed=1;
          j_data.b1=hid_joy_get_button(0);
        }
        if (j_data.b2 != hid_joy_get_button(1))
        {
          j_data.changed=1;
          j_data.b2=hid_joy_get_button(1);
        }
        if (j_data.b3 != hid_joy_get_button(2))
        {
          j_data.changed=1;
          j_data.b3=hid_joy_get_button(2);
        }
        if (j_data.b4 != hid_joy_get_button(3))
        {
          j_data.changed=1;
          j_data.b4=hid_joy_get_button(3);
        }

        if (j_data.b5 != hid_joy_get_button(4))
        {
          j_data.changed=1;
          j_data.b5=hid_joy_get_button(4);
        }

        if (j_data.b6 != hid_joy_get_button(5))
        {
          j_data.changed=1;
          j_data.b6=hid_joy_get_button(5);
        }

        if (j_data.changed)
        {
          char buf[20];
          j_data.changed=0;
          print("x offset=");
          itoa(j_data.x, buf, 7);
          print(buf);
          print(", y offset=");
          itoa(j_data.y, buf, 7);
          print(buf);          
          print(", rz offset=");
          itoa(j_data.rz, buf, 7);
          print(buf);          
          print(", slider value=");
          itoa(j_data.slider, buf, 7);
          print(buf);                    
          print(", HAT value=");
          itoa(j_data.hat, buf, 7);
          print(buf);
          
          print(", button1=");
          if (j_data.b1)
          {            
            print("  ON");
          }
          else
          {
            print(" OFF");
          }
          print(", button2=");
          if (j_data.b2)
          {            
            print("  ON");
          }
          else
          {
            print(" OFF");
          }
          print(", button3=");
          if (j_data.b3)
          {            
            print("  ON");
          }
          else
          {
            print(" OFF");
          }
          print(", button4=");
          if (j_data.b4)
          {            
            print("  ON");
          }
          else
          {
            print(" OFF");
          }
          print(", button5=");
          if (j_data.b5)
          {            
            print("  ON");
          }
          else
          {
            print(" OFF");
          }
          print(", button6=");
          if (j_data.b6)
          {            
            print("  ON");
          }
          else
          {
            print(" OFF");
          }
          
          print("\r\n");
        }
        break;        
      }
    }    
    loop_exit:
      ;
  }
  print("Host application stopped.\r\n");  
  return(0);
}
/****************************** END OF FILE **********************************/
