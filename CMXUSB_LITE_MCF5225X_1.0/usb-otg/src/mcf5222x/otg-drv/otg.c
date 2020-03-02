/***************************************************************************
 *
 *            Copyright (c) 2007 by CMX Systems, Inc.
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
extern void start_host_app(void);
extern void start_device_app(void);
extern void stop_host_app(void);
extern void stop_device_app(void);
extern void app_resume_event(void);
extern void app_sleep_event(void);

#include "terminal/hcc_terminal.h"

#include "otg.h"
#include "otg-low.h"
#include "timer-drv/timer.h"
#include "target.h"
#include "usb-drv/usb_host.h"
#include "usb-drv/usb_utils.h"
#include "usb-drv/usb.h"

#define info(s)
/*#define info(s) print(s)*/

#define VRISE_TMOUT         100
#define B_CONN_TMOUT        100
#define AIDL_BBDIS_TMOUT    200
#define SUSPEND_TMOUT       3

/* OTG function prototypes. */
hcc_u8 otg_vbus_off(void);
hcc_u8 otg_vbus_on(void);
hcc_u8 otg_check_role(void);
hcc_u8 otg_check_vbus(void);
hcc_u8 otg_check_bconn(void);
hcc_u8 otg_check_sleep(void);
hcc_u8 otg_check(void);
void otg_handle_event(void);

typedef enum {
   otgst_invalid=0,     /* not initialized */
   otgst_not_connected, /* neither A nor B */
   otgst_a_idle,        /* A device, bus not used */
   otgst_a_host,        /* A dev, running as host */
   otgst_a_vbus_err,    /* A dev, VBUS error detected */
   otgst_a_suspend,     /* A dev, BUS in suspend */
   otgst_a_peripheral,  /* A dev, running as peripheral */
   otgst_b_idle,        /* B dev, BUS not used */
   otgst_b_peripheral,  /* B dev, running as peripheral */
   otgst_b_host,        /* B dev, running as host */
   otgst_b_suspend      /* B dev, running as a suspended device */
} otg_state;

#define OTGEV_NONE         (0<<0)  /* No event. */
#define OTGEV_SESS_END     (1<<0)  /* B device detects session end. */
#define OTGEV_SESS_START   (1<<1)  /* B device detects session start. */
#define OTGEV_OVERCURRENT  (1<<2)  /* A device detects overcurrent situation. */
#define OTGEV_VBUS_PULSE   (1<<3)  /* A device detects VBUS pulsing. */
#define OTGEV_VBUS_OFF     (1<<4)  /* A device detects VBUS off level during VBUS off. */
#define OTGEV_VBUS_ON      (1<<5)  /* A device detects VBUS on level during VBUS on. */
#define OTGEV_DATA_PULSE   (1<<6)  /* A device detects data pulsing. */
#define OTGEV_ROLE_A       (1<<7)  /* Role change to A device. */
#define OTGEV_ROLE_B       (1<<8)  /* Role change to B device. */
#define OTGEV_HNP_START    (1<<9)  /* A device detects HNP start. */
#define OTGEV_SURP_REMOVAL (1<<10) /* A device detects B disconnection. */
#define OTGEV_SUSPEND      (1<<11) /* B device detects bus buspend. */
#define OTGEV_RESUME       (1<<12) /* B device detects bus resume. */

typedef enum {
  otgerr_none = 0,
  otgerr_bad_role,
  otgerr_bad_state,
  otgerr_vbus,
  otgerr_bcon_tmout,
  otgerr_comm,
  otgerr_not_initialized,
  otgerr_hnp_not_enabled,
  otgerr_no_hnp_ack,
  otgerr_no_hnp_start
} otg_error_t;

typedef enum {
  vbs_off,
  vbs_rising,
  vbs_falling,
  vbs_on
} vbus_state_t;

static struct {
  otg_state state;
  otg_role_t role;
  hcc_u8 a_set_b_hnp_en;
  hcc_u8 b_hnp_capable;
  hcc_u8 b_srp_capable;
  hcc_u32 event;
  vbus_state_t vbus_state;
  hcc_u8 usb_on;
  hcc_u8 pull_dn_on;
  otg_error_t err;
  hcc_u8 session;
  hcc_u8 sleep;
  hcc_u8 host_en;
  hcc_u8 perip_en;
  hcc_u8 b_hnp_enb;
  hcc_u8 a_hnp_capable;
  hcc_u8 a_alt_hnp_capable;
} otg_info;


static void otg_start_b(void)
{
  /* Should be called only from these states. */
  CMX_ASSERT(otg_info.state == otgst_a_idle
             || otg_info.state == otgst_b_idle
             || otg_info.state == otgst_not_connected);
  /* Get hardware to B idle state. */
  otgdrv_init_b();
  otg_info.state=otgst_b_idle;
  otg_info.role=otgr_b;
  otg_info.event &= ~OTGEV_ROLE_B;
}

static void otg_stop_b(void)
{
  if (otg_info.usb_on)
  {
    /* Should be called only from these states. */
    CMX_ASSERT(otg_info.state >= otgst_b_idle); 
    
    if (otg_info.state==otgst_b_peripheral
        || otg_info.state==otgst_b_suspend)
    {        
      usb_stop();
    }
    
    if (otg_info.state==otgst_b_host)
    {
      host_stop();
    }
    otg_info.usb_on = 0;
  }
  otgdrv_set_pull(otgp_off);
  otg_info.pull_dn_on=0;
  otgdrv_set_vbus(otgvs_off);
 
  otg_info.session=0;
  otg_info.state=otgst_not_connected;
  otg_info.role=otgr_none;
}

static void otg_start_a(void)
{
  /* Should be called only from these states. */
  CMX_ASSERT(otg_info.state == otgst_a_idle
             || otg_info.state == otgst_b_idle
             || otg_info.state == otgst_not_connected);
  /* Get hardware to A idle state. */
  otgdrv_init_a();
  otg_info.state=otgst_a_idle;
  otg_info.role=otgr_a;
  otg_info.event &= ~OTGEV_ROLE_A;  
}

static void otg_stop_a(void)
{
  if (otg_info.usb_on)
  {
    /* Should be called only from these states. */
    CMX_ASSERT(otg_info.state < otgst_b_idle);
  
    if (otg_info.state==otgst_a_peripheral)
    {        
      usb_stop();
    }
    else
    {
      host_stop();
    }
    otg_info.usb_on=0;
  }
  otgdrv_set_pull(otgp_off);
  otg_info.pull_dn_on=0;

  if (otg_info.vbus_state != vbs_off)
  {
    otg_vbus_off();
  } 
  otg_info.state=otgst_not_connected;
  otg_info.role=otgr_none;
}

hcc_u8 otg_check_role(void)
{
  otg_role_t r=otgdrv_get_id();
  
  /* Look for role change. */
  if (r!=otg_info.role)
  { /* our role changed */
    /* Assure previous role change was handled. */
    CMX_ASSERT((otg_info.event & (OTGEV_ROLE_A | OTGEV_ROLE_B)) == 0);
    
    if (r==otgr_a)
    {
      otg_info.event|=OTGEV_ROLE_A;
      info("EV_ROLE_A\r\n");
    }
    else if (r==otgr_b)
    {
      otg_info.event|=OTGEV_ROLE_B;    
      info("EV_ROLE_B\r\n");
    }
    else
    {
      /* Upss! Unknown role! */
      CMX_ASSERT(0);
    }
    return(1);
  }
  return(0);
}

hcc_u8 otg_check_vbus(void)
{
  hcc_u8 r=0;
  otgvlevel_t l=otgdrv_get_vbus_level();
  if (otg_info.role == otgr_a)
  { /* If vbus_on is set, voltage level shall be valid. */
    if (otg_info.vbus_state == vbs_on)
    {
      if (l != otgvl_vld)
      {
        /* Power failure! */
        otg_info.event|=OTGEV_OVERCURRENT;
        info("EV_OVERCURR\r\n");
        r=1;
      }
    }
    else if (otg_info.vbus_state == vbs_falling)
    {
      if (l == otgvl_sess_end)
      {
        /* Possible VBUS is turning off, and reached level off. */
        otg_info.event |= OTGEV_VBUS_OFF;
        info("EV_BUSOFF\r\n");
        r=1;        
      }
    }
    else if (otg_info.vbus_state == vbs_rising)
    {
      if (l == otgvl_vld)
      {
        otg_info.event |= OTGEV_VBUS_ON;
        info("EV_BUSON\r\n");
        r=1;
      }
    }
    else
    {           
      if (l != otgvl_sess_end)
      { /* We should check for otglv_vbus_chrg but MAX3353 can not return this 
           level. This anything above session end level is treated as VBUS pulse. */      

        /* VBUS pulsing detected. Note: to properly detect VBUS pulsing polling
           period shall be less than 100 mS. (This function shall be called at 
           more frekvent than each 100 mS.) */
        otg_info.event|=OTGEV_VBUS_PULSE;
        info("EV_VBUS_PULSE\r\n");
        r=1;
      }    
    }
    /* No event otherwise. */    
  }
  else if (otg_info.role == otgr_b)
  { /* A B device shall check VBUS level to detect session ends. */
    if (otg_info.session)
    {
      if(l != otgvl_vld)
      {
        /* Session ended. */
        otg_info.event |= OTGEV_SESS_END;
        info("EV_SESS_END\r\n");
        r=1;      
      }      
    }
    else
    {
      if (l == otgvl_vld && otg_info.perip_en)
      {        
        /* Session is ongoing. App may start using the BUS as a device. */
        otg_info.event |= OTGEV_SESS_START;
        info("EV_SESS_START\r\n");
        r=1;
      }    
    }
  }
  return(r);
}

hcc_u8 otg_check_bconn(void)
{
  /* Check only if we are the A device. */
  if (otg_info.role == otgr_a && (otg_info.event & OTGEV_ROLE_B) == 0
      && otg_info.pull_dn_on)
  {
    hcc_u8 b_conn=otgdrv_get_bconn();
    if (otg_info.state==otgst_a_idle)
    { /* Look for session request. */
      if (b_conn)
      {
        otg_info.event |= OTGEV_DATA_PULSE;
        info("EV_DATA_PULSE\r\n");
        return(1);
      }
    }  
    else if (otg_info.state == otgst_a_suspend)
    {/* Look for HNP start. */      
      if (otg_info.a_set_b_hnp_en && !b_conn) /* HNP is enabled for the B device */  
      {
        otg_info.event |= OTGEV_HNP_START;
        info("EV_HNP_START\r\n");
        return(1);
      }
    }
    else if (otg_info.state==otgst_a_host
             ||otg_info.state==otgst_a_vbus_err)
    {
      if (!b_conn)    
      {
        otg_info.event |= OTGEV_SURP_REMOVAL;
        info("EV_RMOVAL\r\n");
        return(1);
      }    
    }
  }    
  return(0);
}

hcc_u8 otg_check_sleep(void)
{
  if ((otg_info.role == otgr_a && 
         (otg_info.state == otgst_a_host
          || otg_info.state == otgst_a_peripheral
          || otg_info.state == otgst_a_suspend))
       || (otg_info.role==otgr_b && 
            (otg_info.state == otgst_b_peripheral
             || otg_info.state == otgst_b_suspend)))
  {
    hcc_u8 slp=otgdrv_get_suspend();
    if (otg_info.sleep)
    {
      if (!slp)
      {
        otg_info.sleep=0;
        otg_info.event |= OTGEV_RESUME;
        info("EV_RESUME\r\n");
        return(1);      
      }
    } 
    else
    {
      if(slp)
      {
        otg_info.event |= OTGEV_SUSPEND;
        otg_info.sleep=1;
        info("EV_SUSPEND\r\n");
        return(1);    
      }
    }
  }
  return(0);
}


void otg_init(void)
{
  /* set variables to default value */
  otg_info.state=otgst_not_connected;
  otg_info.a_set_b_hnp_en=0;
  otg_info.b_hnp_capable=0;
  otg_info.b_srp_capable=0;
  otg_info.role=otgr_none;
  otg_info.event=OTGEV_NONE;
  otg_info.vbus_state=vbs_off;
  otg_info.usb_on=0;
  otg_info.pull_dn_on=0;
  otg_info.err=otgerr_none;
  otg_info.session=0;
  otg_info.sleep=0;
  otg_info.host_en=0;
  otg_info.perip_en=0;
  otg_info.b_hnp_enb=0;
  otg_info.a_hnp_capable=0;
  otg_info.a_alt_hnp_capable=0;

  /* init low level otg driver */
  otgdrv_init();
  otg_check();
  otg_handle_event();
}

hcc_u8 otg_vbus_off(void)
{  /* This passes allways. */
  otg_info.err=otgerr_none;
  /* Nothing to do if VBUS is already turned off. */
  if (otg_info.vbus_state != vbs_on)
  {
    return(0);
  }

  otgdrv_set_vbus(otgvs_discharge);
  otg_info.vbus_state=vbs_falling; 
  /* Wait till VBUS falls below required level, or B device disconnects. 
     The spec does not defines a timeout. We use some filsafe value. */
  start_mS_timer(1000);
  do {
    if (otg_check())
    {
      if ((otg_info.event & OTGEV_VBUS_OFF) != 0)
      {
        otg_info.event = OTGEV_NONE;  
        info("VBUS_OFF ok\r\n");
        otg_info.state=otgst_a_idle;
        otgdrv_set_vbus(otgvs_off);
        otg_info.vbus_state=vbs_off;          
        return(0);        
      }
      
      if ((otg_info.event & OTGEV_ROLE_B) != 0)
      {
        otg_info.event = OTGEV_ROLE_B;
        info("VBUS_OFF role change\r\n");    
        otgdrv_set_vbus(otgvs_off);
        otg_info.vbus_state=vbs_off;          
        otg_handle_event();
        return(0);
      }
    }
  } while(!check_mS_timer());
  info("VBUS OFF tmout\r\n");
  otgdrv_set_vbus(otgvs_off);
  otg_info.vbus_state=vbs_off;          
  
  return(0);
}

hcc_u8 otg_vbus_on(void)
{
  /* Only an A device may power the BUS. Exit if we are not an A device. */
  if (otg_info.role != otgr_a)
  {
    otg_info.err=otgerr_bad_role;
    info("VBUS_ON badrole\r\n");    
    return(1);
  }
  
  otg_info.err=otgerr_none;
  
  /* If vbus is already on, nothing to do. */
  if (otg_info.vbus_state == vbs_on)
  {
    return(0);
  }
  
  /* Ask low-level driver to turn on VBUS. */
  otgdrv_set_vbus(otgvs_on);
  otg_info.vbus_state=vbs_rising;
    
  /* Wait till VBUS rises. */
  /* Start VBUS rise timeout measuring. */
  start_mS_timer(VRISE_TMOUT);
  
  /* Wait for events. */
  do {  
    /* Check if an we have a new event. */
    if (otg_check())
    {
      otg_info.event &= ~OTGEV_OVERCURRENT;
      otg_handle_event();
      if (otg_info.event & OTGEV_VBUS_ON)
      {
        otg_info.event &= ~OTGEV_VBUS_ON;
        otg_info.vbus_state=vbs_on;
        info("VBUS_ON ok\r\n");    
        return(0);
      }
    }
    
    if (check_mS_timer())
    {
      info("VBUS_ON tmout\r\n");
      otgdrv_set_vbus(otgvs_off);
      otg_info.vbus_state=vbs_off;
      otg_info.err=otgerr_vbus;
      otg_info.state=otgst_a_vbus_err;
      return(1);
    }
  } while(otg_info.role==otgr_a);
  info("VBUS_ON role change\r\n");  
  otg_info.err=otgerr_bad_role;
  return(1);
}

/* Application wants to start use the bus as a host. */
hcc_u8 otg_host_req_bus(void)
{
  otg_info.err=otgerr_none;
  info("host req\r\n");
  
  otg_info.host_en=1;
  otg_info.perip_en=0;
  switch(otg_info.state)
  {
  default:
    info("bad state\r\n");      
    return(1);
  /* A device is runnig as a peripheral. Turn off VBUS to end session. 
     Traverse to a_idle and continue from there. */  
  case otgst_a_peripheral:
    stop_device_app();  
    otg_stop_a();
    otg_start_a();
    
  /* We are A device in idle state. */  
  case otgst_a_idle:
    /* Turn on VBUS. */
    if (otg_vbus_on())
    {
      return(1);
    }

    /* Turn on pull-down resistors. */   
    otgdrv_set_pull(otgp_down);
    otg_info.pull_dn_on=1;
              
    /* Wait for device connection. */
    start_mS_timer(10*B_CONN_TMOUT);
    do {
      /* Look for role change.  */
      if (otg_check())
      {
        otg_handle_event();

        if (otg_info.state != otgst_a_idle)
        {
          otg_info.err=otgerr_bad_role;
          info("not in a_idle\r\n");
          break;
        }
      }
      if (check_mS_timer())
      {
        otg_info.err=otgerr_bcon_tmout;
        info("b_conn timeout\r\n");        
        break;
      }
    } while(!(otg_info.event & OTGEV_DATA_PULSE));
    
    otg_info.event &= ~OTGEV_DATA_PULSE;

    if (otg_info.err == otgerr_none)    
    {
      /* At this point BUS is ready for communication, so start host driver. */
      host_init();
      host_scan_for_device();
      otg_info.usb_on=1;

      otg_info.b_srp_capable=0;
      otg_info.b_hnp_capable=0;

      /* Get OTG descriptor from device. */
      if (!get_cfg_desc(1))
      {
        hcc_u8 setup[8];
        hcc_u16 desc_ofs=find_descriptor(0x9, 0, 0);
        /* Descriptor found. Read capabilityes. */
        if (desc_ofs != (hcc_u16)-1u)
        {
          otg_info.b_srp_capable=(hcc_u8)(dbuffer[desc_ofs+2] & (1<<0) ? 1 : 0);
          otg_info.b_hnp_capable=(hcc_u8)(dbuffer[desc_ofs+2] & (1<<1) ? 1 : 0);
        }
        if (otg_info.b_hnp_capable)
        {        
          /* Tell B device it is connected to a HNP capable port. */
          fill_setup_packet(setup, STP_DIR_OUT, STP_TYPE_STD, STP_RECIPIENT_DEVICE
            ,STDRQ_SET_FEATURE, 4, 0, 0);
          if (0!=host_send_control(setup, setup, 0))
          {
            otg_info.err=otgerr_comm;
          }
          else
          {
            /* Tell B device it may start HNP. */
            fill_setup_packet(setup, STP_DIR_OUT, STP_TYPE_STD, STP_RECIPIENT_DEVICE
              ,STDRQ_SET_FEATURE, 3, 0, 0);
            if (0!=host_send_control(setup, setup, 0))
            {
              otg_info.err=otgerr_comm;
            }
            else
            {
              otg_info.a_set_b_hnp_en=1;
            }
          }
        }
      }
      else
      {        
        info("Comm error\r\n");      
        otg_info.err=otgerr_comm;      
      }      
    }
    /* Do clean exit in case of an error. */
    if (otg_info.err != otgerr_none)
    {
      otg_stop_a();
      otg_start_a();
      return(1);      
    }
    start_host_app();
    otg_info.state=otgst_a_host;
    return(0);
    
  /* already working as a host. */
  case otgst_b_host:  
  case otgst_a_host:
    return(0);
  /* Uups! Not yet initialized. */
  case otgst_invalid:
    otg_info.err=otgerr_not_initialized;
    return(1);
  /* We are the B device, and in idle state. To get into host state, a session
     shall be requested, then we should wait till the host suspends the bus, 
     and then if set_b_hnp_en has was received start HNP. But since there is
     no guarantee host will suspend the BUS, we don't support this. */
  case otgst_b_idle:
    info("fail b_idle\r\n");
    otg_info.err=otgerr_bad_state;
    return(1);
  case otgst_b_suspend:
    /* Start HNP. */
    if (otg_info.b_hnp_enb)
    { /* Do HNP. */
      /* TODO: we should check for HNP startup conditions firs. 
         (bus shall be idle 5-150 mS). */

      stop_device_app();
      otg_info.sleep=0;
      usb_stop();
      otg_info.state=otgst_b_idle;
      otgdrv_set_pull(otgp_down);
      /* wait 25 uS to let D+ discharge to 0.8 V */         
      start_mS_timer(1);     
      otgdrv_init_a();      
      while(!check_mS_timer())
        ;    
      otgdrv_set_pull(otgp_off);
      start_mS_timer(1);     
      while(!check_mS_timer())
        ;                  
      /* look for J state (D+ hi, D- low) (look for attach) */
      start_mS_timer(3000);      
      do {
        if (otgdrv_get_bconn())
        {
          break;
        }
      }while(!check_mS_timer());
      
      if (!otgdrv_get_bconn())
      {/* No ack from A device. */
        otg_info.err=otgerr_no_hnp_ack;
        otg_start_b();
        info("noack from A\r\n");
        return(1);
      }
      otg_info.state=otgst_b_host;
      otg_info.perip_en=0;
      otg_info.host_en=1;
      host_init();
      otg_info.usb_on=1;
      start_host_app();
      return(0);
    }
    else
    {
      otg_info.err=otgerr_hnp_not_enabled;
      info("hnp_en not set\r\n");
      return(1);
    }
    break;
    
  case otgst_b_peripheral:
    /* HNP is only possible from b_suspend. */
    otg_info.err=otgerr_bad_state;
    info("not in suspend\r\n");
    return(1);
  }
  return(0);
}

hcc_u8 otg_host_drop_bus(void)
{
  otg_info.host_en=0;
  switch(otg_info.state)
  {
  case otgst_a_suspend:
  case otgst_a_host:
    otg_stop_a();
    otg_start_a();
    break;
  case otgst_b_host:
    host_stop();
    otg_info.pull_dn_on=0;
    otg_stop_b();
    otg_start_b();
    break;
  case otgst_a_vbus_err:
    otg_info.state=otgst_a_idle;
    break;
  default:
    /* empty */
    break;	
  }
  return(0);
}

hcc_u8 otg_device_req_bus(void)
{
  info("device req\r\n");
  otg_info.perip_en=1;
  otg_info.host_en=0;
  switch(otg_info.state)
  {
  case otgst_a_idle:
    /* Start up as a HOST, enable HNP during enumeration, 
       suspend BUS, and wait for HNP request. */
    if (otg_host_req_bus())
    {
      /* Failed to become host. */
      return(1);
    }
    /* continue as from a_host state */
  case otgst_a_host:   
    if (!otg_info.a_set_b_hnp_en)
    {
      /* B device does not supports HNP. */
      otg_stop_a();
      otg_start_a();
      info("hnp_en not set\r\n");
      return(1);
    }

    /* Get BUS to suspended state. */
    host_sleep();

    otg_check();
    otg_handle_event();

    CMX_ASSERT(otg_info.state=otgst_a_suspend);
    /* continue as from suspend */
  case otgst_a_suspend:
    /* Now wait for HNP start signaling from B */
    start_mS_timer(2000);
    do { /* Look for role change.  */
      otg_check();
      otg_handle_event();
      if (check_mS_timer())
      { /* B does not want to become host. */
        info("B noack\r\n");
        otg_info.perip_en=0;
        otg_info.host_en=1;
        otg_info.err=otgerr_no_hnp_start;
        return(1);
      }
    } while(otg_info.state != otgst_a_peripheral);
    start_device_app();
    return(0);

  case otgst_b_host:
   /* Simply switch back to device. */
   host_stop();
   otg_info.usb_on=0;
   stop_host_app();
   start_mS_timer(SUSPEND_TMOUT);
   while(!check_mS_timer())
     ;
   otg_info.state=otgst_b_idle;
  case otgst_b_idle:
      /* If VBUS is not valid, we need to do SRP. */
    if (otgvl_vld != otgdrv_get_vbus_level())
    {
      if (otgdrv_req_session())
      {
        info("req session fail\r\n");
        return(1);
      }
    }

    otg_info.session=1;
    otg_info.state=otgst_b_peripheral;
    /* Turn on pull-up, */    
    otgdrv_set_pull(otgp_up_p);   
    /* start device driver */
    start_device_app();    
    usb_init((2<<3) | 7, 0);    
    otg_info.usb_on=1;
    return(0);
  case otgst_b_peripheral:
  case otgst_a_peripheral:
    return(0);
  case otgst_b_suspend:
    /*TODO: wake BUS. */
    return(1);
  default:
    /* empty */
    break;
  }
  CMX_ASSERT(0);
  return(0);
}

hcc_u8 otg_device_drop_bus(void)
{
  otg_info.perip_en=0;
  otg_info.session=0;
  switch(otg_info.state)
  {
  case otgst_a_vbus_err:
    otg_info.state=otgst_a_idle;
    break;
  case otgst_a_peripheral:
    otg_stop_a();
    otg_start_a();
    break;
  case otgst_b_peripheral:
  case otgst_b_suspend:
    otg_stop_b();
    otg_start_b();
    break;
  default:
    /* empty */
    break;
  }
  return(0);
}

hcc_u8 otg_check(void)
{
  hcc_u8 todo=0;
  todo=otg_check_role();
  todo|=otg_check_vbus();
  todo|=otg_check_sleep();  
  todo|=otg_check_bconn();
  return(todo);
}

void otg_handle_event(void)
{ 
  if (otg_info.event & OTGEV_ROLE_B)
  {
    otg_info.event = OTGEV_NONE;    
    otg_stop_a();
    otg_start_b();
    info("to role B\r\n");
    return;
  }

  if (otg_info.event & OTGEV_ROLE_A)
  {
    otg_info.event = OTGEV_NONE;    
    otg_stop_b();
    otg_start_a();
    info("to role A\r\n");
    return;
  }

  if (otg_info.role == otgr_a)
  {   
    if (otg_info.event & OTGEV_OVERCURRENT)
    {
      otg_info.event = OTGEV_NONE;
      otg_stop_a();
      otg_start_a();
      otg_info.state=otgst_a_vbus_err;
      info("overcurrent\r\n");
      return;
    }
    
    if (otg_info.event & OTGEV_HNP_START)
    {
      CMX_ASSERT(otg_info.state=otgst_a_suspend);
      info("HNP ack\r\n");
      stop_host_app();      
      otg_info.sleep=0;      
      otgdrv_set_pull(otgp_up_p);
      otgdrv_init_b();

      start_mS_timer(3000);
      do {
        if (otgdrv_got_SE0())        
        {
          info("Strat usb driver.\r\n");
          usb_init((2<<3) | 7, 0);
          otg_info.usb_on=1;
          
          start_device_app();
          otg_info.host_en=0;
          otg_info.perip_en=1;
          otg_info.state=otgst_a_peripheral;
          otg_info.event=OTGEV_NONE;
          return;
        }
      }while(!check_mS_timer());
      
      otg_info.event &= ~OTGEV_HNP_START;
      info("HNP start -> removal\r\n");
      otg_info.event |= OTGEV_SURP_REMOVAL;
    }
    
    if (otg_info.event & OTGEV_SURP_REMOVAL)
    {
      otg_info.event=OTGEV_NONE;
      otg_stop_a();
      otg_start_a();
      stop_host_app();
      info("Removal\r\n");
      return;
    }
    
    if (otg_info.event & OTGEV_VBUS_PULSE)
    {
      CMX_ASSERT(otg_info.state==otgst_a_idle
                 || otg_info.state==otgst_a_vbus_err);
      if (otg_info.host_en)
      {
        otg_info.event &= ~OTGEV_VBUS_PULSE;
        if (!otg_host_req_bus())
        {
          info("SRP success\r\n");
          start_host_app();
        }
        else
        {
          info("SRP failed\r\n");
        }
      }
    }
    
    if (otg_info.event & OTGEV_SUSPEND)
    {
      if (otg_info.state==otgst_a_peripheral)
      {        
        otg_info.event &= ~OTGEV_SUSPEND;
/*      
TODO:        
        otg_stop_a();
        otg_start_a();
        info("back to host\r\n");
*/        
      }
      else if (otg_info.state==otgst_a_host)
      {
        info("a_suspend\r\n");      
        otg_info.state=otgst_a_suspend;
        otg_info.event &= ~OTGEV_SUSPEND;
        app_sleep_event();
      }      
    }
    
    if (otg_info.event & OTGEV_RESUME)
    {
      otg_info.event &= ~OTGEV_RESUME;    
      if (otg_info.state==otgst_a_suspend)
      {
        info("a_resume\r\n");
        otg_info.state=otgst_a_host;
        app_resume_event();
      }
    }
  }
  else if (otg_info.role == otgr_b)
  {   
    /* Role B events. */    
    if (otg_info.event & OTGEV_SESS_END)
    {
      otg_info.event &=~OTGEV_SESS_END;
      if (otg_info.session)
      {
        if (otg_info.state==otgst_b_peripheral
            || otg_info.state==otgst_b_suspend)
        {            
          stop_device_app();
        }

        if (otg_info.state==otgst_b_host)
        {            
          stop_host_app();
        }
      }
      otg_stop_b();
      otg_start_b();
      otg_info.sleep=0;
      info("B sess end\r\n");
      return;
    }
    
    if (otg_info.event & OTGEV_SESS_START)
    {
      CMX_ASSERT(otg_info.state==otgst_b_idle);
      otg_info.event &= ~OTGEV_SESS_START;
      if (!otg_info.session)
      {
        if (!otg_device_req_bus())
        {
          start_device_app();
          info("Session start ok.\r\n");
        }
        else
        {
          info("Session start failed\r\n");
        }
      }
      otg_info.sleep=0;
      info("B sess start\r\n");
    }    
    
    if (otg_info.event & OTGEV_SUSPEND)
    {
      otg_info.event &= ~OTGEV_SUSPEND;
      otg_info.state=otgst_b_suspend;
      info("b_suspend\r\n");
      app_sleep_event();
    }

    if (otg_info.event & OTGEV_RESUME)
    {
      otg_info.event &= ~OTGEV_RESUME;
      otg_info.state=otgst_b_peripheral;
      info("B resume\r\n");
      app_resume_event();
    }
  }

  CMX_ASSERT((otg_info.event & ~(OTGEV_VBUS_OFF|OTGEV_VBUS_ON
                                 |OTGEV_VBUS_PULSE|OTGEV_DATA_PULSE))==0);  
  return;
}

otg_role_t otg_get_role(void)
{
  return(otgdrv_get_id());
}

void otg_process(void)
{
  if (otg_check())
  {
    otg_handle_event();
    otg_info.event=OTGEV_NONE;
  }
  
}

hcc_u8 otg_get_sleep(void)
{
  return(otg_info.sleep);
}

otg_mode_t otg_get_mode(void)
{
  if (otg_info.host_en)
  {
    return(otgm_host);
  }
  if (otg_info.perip_en)
  {
    return(otgm_peripheral);
  }
  return(otgm_none);
}


callback_state_t usb_ep0_otg_callback(void)
{
  hcc_u8 *pdata=usb_get_rx_pptr(0);
  callback_state_t r=clbst_error;
  
  if (((STP_REQU_TYPE(pdata) & (1u<<7)) == 0)
      && STP_REQU_TYPE(pdata) == ((0<<7) | (0<<5) | 0)
      && STP_REQUEST(pdata) == USBRQ_SET_FEATURE
      && STP_INDEX(pdata)==0
      && STP_LENGTH(pdata)==0)
  {
    r=clbst_out;
    switch(STP_VALUE(pdata))
    {
    case 3: /* b_hnp_enable */
      otg_info.b_hnp_enb=1;
      break;
    case 4: /* a_hnp_support */
      otg_info.a_hnp_capable=1;
      break;
    case 5: /* a_alt_hnp_support */
      otg_info.a_alt_hnp_capable=1;    
      break;
    }  
  }
  return(r);
}
/****************************** END OF FILE **********************************/
