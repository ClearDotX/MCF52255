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
#include "hcc_types.h"
#include "usb-drv/usb.h"
#include "usb-drv/usb_host.h"
#include "target.h"
#include "otg-drv/otg.h"
#include "uart-drv/uart.h"
#include "terminal/hcc_terminal.h"

extern void device_demo_app(void);
extern void hid_host_app(void);
typedef enum {
  appst_stopped,
  appts_starting,
  appst_running,
  appst_stopping
} app_state_t;

app_state_t host_st;
app_state_t app_st;

hcc_u8 host_run;
hcc_u8 host_stp;
hcc_u8 device_run;
hcc_u8 device_stp;
hcc_u8 sleep;
hcc_u8 srp;

/* This callback is needed by the host driver. It must be empty because pull-up control is
   done by the on-the-go driver. */
void enable_usb_pull_downs(void)
{
}

void busy_wait(void);

void busy_wait(void)
{
  terminal_process();
  otg_process();
}

/* Called by OTG driver when the bus is resuming. */
void app_resume_event(void);
void app_resume_event(void)
{
  if (sleep)
  {
    print("Bus has been resumed.\r\n");
    sleep=0;
  }
}

/* Called by the OTG driver when the bus ids entering 
   suspended state. */
void app_sleep_event(void);
void app_sleep_event(void)
{
  if (!sleep)
  {
    print("BUS has been suspended.\r\n");
    sleep=1;
  }
}

void start_host_app(void);
/* Called by the OTG driver when host mode has been activated. */
void start_host_app(void)
{
  host_run=1;
  device_stp=1;
}

/* Called by the OTG driver when host mode has been deactivated. */
void stop_host_app(void);
void stop_host_app(void)
{
  host_stp=1;
  sleep=0;
}

/* Called by the OTG driver when device mode has been activated. */
void start_device_app(void);
void start_device_app(void)
{
  host_stp=1;
  device_run=1;
}

/* Called by the OTG driver when device mode has been deactivated. */
void stop_device_app(void);
void stop_device_app(void)
{  
  device_stp=1;
  sleep=0;
}

static void start_host(void)
{  
  if (host_run || device_run)
  {
    return;
  }
  
  if (otgr_a == otg_get_role())
  {
    print("Starting A device as host.\r\n");
  }
  else
  {
    print("Starting B device as host.\r\n");
  }
  
  sleep=0;
  if (otg_host_req_bus())
  {
    print("OTG failure. Can not start as USB host.\r\n");
  }
  else
  {
    host_run=1;    
  }
}

static void start_device(void)
{
  if (host_run || device_run)
  {
    return;
  }

  sleep=0;
  if (otgr_a == otg_get_role())
  {    
    print("Starting A device as device.\r\n");
  }
  else
  {
    print("Starting B device as device\r\n");
  }
  
  if (otg_device_req_bus())
  {
    print("OTG failure. Can not start as USB device.\r\n");
  }
  else
  {
    device_run=1;
  }    
}

static void sw_perip(char *c)
{
  c++;
  if (otg_device_req_bus())
  {
    print("Failed\r\n");
  }
}

static void sw_host(char *c)
{
  c++;
  if (otg_host_req_bus())
  {
    print("Failed\r\n");
  }
}
   
const command_t cmd_perip = {"peripheral", sw_perip, "Switch to peripheral mode."};
const command_t cmd_host = {"host", sw_host, "Switch to host mode."};


int main(void)
{
  hw_init();
  uart_init(38400, 1, 'n', 8);
  terminal_init(uart_putch, uart_getch, uart_kbhit);
  terminal_add_cmd((void*)&cmd_perip);
  terminal_add_cmd((void*)&cmd_host);

  Usb_Vbus_Off();

  otg_init();

  if (otgr_a == otg_get_role())
  {
    start_host();
  }
  else if(otgr_b == otg_get_role())
  {
    start_device();
  }    

  while(1)
  {
    busy_wait();
       
    if (host_stp)
    {
      host_run=0;
      host_stp=0;
    }
    
    if (device_stp)
    {
      device_run=0;
      device_stp=0;
    }
    
    if (host_run)
    {
      sleep=0;
      hid_host_app();
      continue;
    }

    if (device_run)
    {
      sleep=0;
      device_demo_app();
      continue;
    }
  }
}
/****************************** END OF FILE **********************************/
