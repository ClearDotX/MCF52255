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
#include "hcc_types.h"
#include "mcf5222x_reg.h"
#include "timer-drv/timer.h"
#include "usb_host.h"
#include "usb_utils.h"

#define DEBUG_TRACE 0u
#define DSC_BUG 0u
/*****************************************************************************
 * Macro definitions.
 *****************************************************************************/
#define MIN(a,b)  ((a) < (b) ? (a) : (b)) 

/* Number of maximum endpoints supported per device. */
#define MAX_EP_PER_DEVICE  3u

/* This macro shall evaluate to a uint32 pointer to the start address of the
   buffer descriptor table (BDT). The BDT has 32 bytes for each endpoint.
   The BDT shall be alignet to 512 byte boundary! */
extern hcc_u32 _BDT_BASE[];
#define BDT_BASE              ((hcc_u32*)(_BDT_BASE))
#define BDT_CTL_RX(ep, b)     (BDT_BASE[((ep)<<3)+((b)<<1)+0])
#define BDT_ADR_RX(ep, b)     (BDT_BASE[((ep)<<3)+((b)<<1)+1])
#define BDT_CTL_TX(ep, b)     (BDT_BASE[((ep)<<3)+((b)<<1)+4])
#define BDT_ADR_TX(ep, b)     (BDT_BASE[((ep)<<3)+((b)<<1)+5])

#define BDT_CTL_STALL BIT2
#define BDT_CTL_DTS   BIT3
#define BDT_CTL_DATA  BIT6
#define BDT_CTL_OWN   BIT7

/* Token values. */
#define TOKEN_SETUP   0xd
#define TOKEN_IN      0x9
#define TOKEN_OUT     0x1
#define TOKEN_NAK     0xa
#define TOKEN_STALL   0xe
#define TOKEN_ACK     0x2

/* Maximum possible packet size for endpoint 0. */
#define MAX_EP0_PSIZE   64u
#define MIN_EP0_PSIZE   8u

#if DEBUG_TRACE == 1
#define MKDBG_TRACE(evnt, epndx)   (evstk[evndx].ev=(evnt), evstk[evndx++].ep=(epndx))
#else
#define MKDBG_TRACE(evnt, epndx)
#endif 


/*****************************************************************************
 * External references.
 *****************************************************************************/
/* none */

/*****************************************************************************
 * Local types.
 *****************************************************************************/
 /* Information about endpoints of connected devices. */
 typedef struct {
  hcc_u16 last_due;
  hcc_u16 psize; 
  hcc_u8 type;
  hcc_u8 address;
  hcc_u8 interval;
  hcc_u8 tgl_rx;
  hcc_u8 tgl_tx;
} device_ep_t;

/* Information about connected devices. */
typedef struct {
  hcc_u8 address;
  hcc_u8 low_speed;
  device_ep_t eps[MAX_EP_PER_DEVICE];
} dev_table_element_t;

#if DEBUG_TRACE == 1
enum event
{
  ev_none = 0,
  ev_bus_reset,
  ev_host_init,
  ev_no_attach,
  ev_attach_ls_dev,
  ev_attach_fs_dev,
  ev_host_stop,
  ev_trt_setup,
  ev_trt_in,
  ev_trt_out,
  ev_got_ack,
  ev_got_nak,
  ev_got_stall,
  ev_got_unknown,
  ev_got_data_error,
  ev_disconnect,
  ev_send_control,
  ev_receive_control,
  ev_send,
  ev_receive,
  ev_no_device,
  ev_no_ep,
  ev_ep0_psize_failed,
  ev_set_address_failed
};
#endif
/*****************************************************************************
 * Module variables.
 *****************************************************************************/
#if DEBUG_TRACE == 1

volatile hcc_u8 evndx=0;

struct
{
  enum event ev;
  hcc_u8 ep;
} evstk[1<<(sizeof(evndx)<<3)];

#endif

/* This structure tells us which packet buffer was last used for TX and RX. */
struct {
  hcc_u8 next_rx;
  hcc_u8 next_tx;
} ep_info;

/* This table contains information about connected devices. Since we do not
   support external HUBS and this hardware has no ROOT HUB, only one device
   can be connected at a time. */
dev_table_element_t my_device;

/* Error code for the device. */
tr_error_t tr_error;
/*****************************************************************************
 * Function prototypes
 *****************************************************************************/
/*  */

/*****************************************************************************
 * Function declarations
 *****************************************************************************/

/*****************************************************************************
 * evt_disconnect
 * IN:  -
 * OUT: -
 * Assumptions:
 * Description:
 *   Put hardware and firmware to disconnected state. 
 *****************************************************************************/
static void evt_disconnect(void)
{
  MKDBG_TRACE(ev_disconnect, 0);

#if DSC_BUG == 0
  while(MCF_USB_CTL & MCF_USB_CTL_TXDSUSPEND_TOKBUSY)
  {
    if (MCF_USB_INT_STAT & MCF_USB_INT_STAT_USB_RST)
    {
      MCF_USB_CTL &= ~MCF_USB_CTL_TXDSUSPEND_TOKBUSY;
      break;
    }
  }
#endif 

  my_device.address = INVALID_ADDRESS;
  my_device.low_speed = 0;
  /* disable SOF generation */
  MCF_USB_INT_STAT = MCF_USB_INT_STAT_USB_RST | MCF_USB_INT_STAT_ATTACH;  
  host_sleep();
}

static hcc_u8 evt_connect(void)
{
  hcc_u8 ep;
  volatile int x;

  /* debounce (100 mS) */
  host_ms_delay(100);

  /* clear attach event */
  MCF_USB_INT_STAT = MCF_USB_INT_STAT_ATTACH;
  
  /* Some delay is needed between clearing the ATTACH flag, and checking
     it again. Unfortunately there is no information about the length of 
     the delay. */
  for(x=0; x<10000; x++)
    ;

  /* Is a device connected? */
  if((MCF_USB_INT_STAT & MCF_USB_INT_STAT_ATTACH) == 0)
  { /* no */
    return(0);
  } 
  
  /* A newly connected device shall have address 0, */
  my_device.address=0;
  /* and only ep0 is working. We assume packet size of
     ep0 is the possible minimum. We will read the real
     value during enumeration. */
  host_modify_ep(0, EPTYPE_CTRL, 0, 0, MIN_EP0_PSIZE);

  /* remove all endpoints except 0 */
  for(ep=1;ep<MAX_EP_PER_DEVICE;ep++)
  {
    host_remove_ep(ep);
  }

  /* clear low speed bit to make JSTATE detection consistent. */
  MCF_USB_ADDR=0;

  /* let settle D+ and D- to the right state */
  host_ms_delay(1);

  /* Check if device is low or high speed. */
  if ((MCF_USB_CTL & MCF_USB_CTL_JSTATE) == 0)
  {
    MKDBG_TRACE(ev_attach_ls_dev, 0);
    /* Low speed device. */
    my_device.low_speed=1;
    MCF_USB_ADDR = MCF_USB_ADDR_LS_EN;  
  }
  else
  {
    MKDBG_TRACE(ev_attach_fs_dev, 0);  
    my_device.low_speed=0;
  }
  return(1);
}

static hcc_u8 chk_dev(void)
{
  if (my_device.address != INVALID_ADDRESS)
  {    
    /* If a reset was seen while we have not been issued a reset, then 
       the device is disconnected (surprise removal). */
    if (MCF_USB_INT_STAT & MCF_USB_INT_STAT_USB_RST)
    {
      evt_disconnect();
    }
  }
  
  if (my_device.address == INVALID_ADDRESS)
  { 
    volatile int x;
    /* If we can not clear the attach flag, then a device is connected. */
    MCF_USB_INT_STAT = MCF_USB_INT_STAT_ATTACH;

    /* Some delay is needed between clearing the ATTACH flag, and checking
       it again. Unfortunately there is no information about the length of 
       the delay. */
    for(x=0; x< 10000; x++)
      ;

    if (MCF_USB_INT_STAT & MCF_USB_INT_STAT_ATTACH)
    {
      evt_connect();
    }
  }  
  return((hcc_u8)(my_device.address != INVALID_ADDRESS));
}
/*****************************************************************************
 * host_has_device
 * IN:  -
 * OUT: -
 * Assumptions:
 * Description:
 *   Return true if a device is still connected.
 *****************************************************************************/
hcc_u8 host_has_device(void)
{
  if ((MCF_USB_CTL & MCF_USB_CTL_HOST_MODE_EN) == 0)
  {
    tr_error=tre_not_running;
    return(0);
  }
  tr_error=tre_none;
  return chk_dev();
}
 
 
/*****************************************************************************
 * host_add_ep
 * IN: 
 *     type:     type of endpoint
 *     address:  address of endpoint
 *     interval: poll interval for periodic endpoints
 *     psize:    maximum packet size
 * OUT:
 *     endpoint handle
 * Assumptions:
 * Description:
 *   Add a new endpoint to a device
 *****************************************************************************/
hcc_u8 host_add_ep(hcc_u8 type, hcc_u8 address, hcc_u8 interval, hcc_u16 psize)
{
  hcc_u8 x;
  
  if ((MCF_USB_CTL & MCF_USB_CTL_HOST_MODE_EN) == 0)
  {
    tr_error=tre_not_running;
    return(INVALID_ADDRESS);
  }

  for(x=0; x < MAX_EP_PER_DEVICE; x++)
  {
    if (my_device.eps[x].address == INVALID_ADDRESS)
    {
      my_device.eps[x].type=type;
      my_device.eps[x].address=(hcc_u8)(address & 0x7f);
      my_device.eps[x].interval=interval;
      my_device.eps[x].psize=psize;
      my_device.eps[x].tgl_rx=0;
      my_device.eps[x].tgl_tx=0;
      my_device.eps[x].last_due=0;
      return(x);
    }
  }
  return(INVALID_ADDRESS);
}
 
/*****************************************************************************
 * host_remove_ep
 * IN:  ep_handle: handle of endpoint
 * OUT: n/a
 * Assumptions:
 *   The parameter is a valid handle.
 * Description:
 *   Remove an dendpoint of a device. 
 *****************************************************************************/
void host_remove_ep(hcc_u8 ep_handle)
{
  CMX_ASSERT(ep_handle < MAX_EP_PER_DEVICE);
  
  my_device.eps[ep_handle].address=INVALID_ADDRESS;
}

/*****************************************************************************
 * host_modify_ep
 * IN: 
 *     ep_handle: handle of an endpoint
 *     type:      type of endpoint
 *     address:   address of endpoint
 *     interval:  poll interval for periodic endpoints
 *     psize:     maximum packet size      
 * OUT:
 * Assumptions:
 * Description:
 *   Add a new endpoint to a device
 *****************************************************************************/
void host_modify_ep(hcc_u8 ep_handle, hcc_u8 type, hcc_u8 address, hcc_u8 interval, hcc_u16 psize)
{
  CMX_ASSERT(ep_handle < MAX_EP_PER_DEVICE);
  my_device.eps[ep_handle].type=type;
  my_device.eps[ep_handle].address=address;
  my_device.eps[ep_handle].interval=interval;
  my_device.eps[ep_handle].psize=psize;       
}
 
/*****************************************************************************
 * host_reset_bus
 * IN: n/a
 * OUT: n/a
 * Assumptions: n/a
 * Description:
 *   Do reset signaling on the USB bus.
 *****************************************************************************/
void host_reset_bus(void)
{
  hcc_u8 ep=0;

  MKDBG_TRACE(ev_bus_reset, 0);
  
  if ((MCF_USB_CTL & MCF_USB_CTL_HOST_MODE_EN) == 0)
  {
    tr_error=tre_not_running;
    return;
  }

  /********* Prepare for BUS reset. */
  /* Wait till pending tokens are processed. */ 
#if DSC_BUG == 0
  while(MCF_USB_CTL & MCF_USB_CTL_TXDSUSPEND_TOKBUSY)
  {
    if (MCF_USB_INT_STAT & MCF_USB_INT_STAT_USB_RST)
    {
      MCF_USB_CTL &= ~MCF_USB_CTL_TXDSUSPEND_TOKBUSY;
      break;
    }
  }
#endif 
  /********* reset */ 
  /* Start reset signaling. */
  MCF_USB_CTL |= MCF_USB_CTL_RESET;

  /* The minumum reset signal length is 10 mS. We use an 1 mS timer so it has 
     at least 1mS error (1 period). USB specifies +-0.05% accuracy for frame
     interval. So we are good enough if we wait for 11 SOF due times. */
  host_ms_delay(11);
  /* stop reset signaling */
  MCF_USB_CTL &= ~MCF_USB_CTL_RESET;

  /* Clear reset event. */
  MCF_USB_INT_STAT = MCF_USB_INT_STAT_USB_RST;
  
  /********* do firmware reset */  
  /* A reset device shall have address 0, */
  if (my_device.address != INVALID_ADDRESS)
  {  
    my_device.address=0;
  }
  /* and only ep0 is working. We assume packet size of
     ep0 is the possible minimum. We will read the real
     value during enumeration. */
  host_modify_ep(0, EPTYPE_CTRL, 0, 0, MIN_EP0_PSIZE);

  /* remove all endpoints except 0 */
  for(ep=1;ep<MAX_EP_PER_DEVICE;ep++)
  {
    host_remove_ep(ep);
  }
  
  /* check if we have a device connected */
  /* if a device is connected, it will answer to address 0 */
  if (chk_dev())
  {    
    /* enable SOF generation */
    MCF_USB_CTL |= MCF_USB_CTL_USB_EN_SOF_EN;
    
    MCF_USB_INT_STAT = MCF_USB_INT_STAT_SLEEP | MCF_USB_INT_STAT_RESUME;

    /* device may need max 10 mS reset recovery time */
    /*host_ms_delay(10);*/
    host_ms_delay(100);      
    /* Read out endpoint 0 packet size. */
    if (set_ep0_psize())
    {
      MKDBG_TRACE(ev_ep0_psize_failed, (hcc_u8)-1u);
      return;
    }
    /* set device address */
    if (set_address(1))
    {
      MKDBG_TRACE(ev_set_address_failed, (hcc_u8)-1u);
      return;
    }
    my_device.address=1;        
  }  
}

/*****************************************************************************
 * host_init
 * IN: n/a
 * OUT: n/a
 * Assumptions: n/a
 * Description:
 *   Inicialize host.
 *****************************************************************************/
void host_init(void)
{
  hcc_u8 ep;
  MKDBG_TRACE(ev_host_init, 0);
  /****** inicialize module variables. */
  my_device.address=INVALID_ADDRESS;
  ep_info.next_tx=0;
  ep_info.next_rx=0;
  tr_error=tre_none;
  
  /* remove all endpoints */
  for(ep=0;ep<MAX_EP_PER_DEVICE;ep++)
  {
    host_remove_ep(ep);
  } 
  /****** Configure hardware. */
  
  /* Clear all pending events. */
  MCF_USB_INT_STAT = 0xff;
  
  /* select USB clock source */
  MCF_USB_USB_CTRL = MCF_USB_USB_CTRL_CLKSRC_OSC;

  /* use first rx and tx buffer */
  MCF_USB_CTL |= MCF_USB_CTL_ODD_RST;
  /* set BDT address */
  MCF_USB_BDT_PAGE_01 = (hcc_u8)(((hcc_u32)BDT_BASE) >> 8);
  MCF_USB_BDT_PAGE_02 = (hcc_u8)(((hcc_u32)BDT_BASE) >> 16);
  MCF_USB_BDT_PAGE_03 = (hcc_u8)(((hcc_u32)BDT_BASE) >> 24);

  /* Set SOF threshold */
  MCF_USB_SOF_THLDL = 74;
  
  /* Enable pull-down resistors. */
  enable_usb_pull_downs();
  
  /* Enable host operation mode. */
  MCF_USB_CTL = MCF_USB_CTL_HOST_MODE_EN;
}

/*****************************************************************************
 * host_scan_for_device
 * IN: n/a
 * OUT: 0: no device
 *      1: device detected
 * Assumptions: n/a
 * Description:
 *   Check if the device has been attached. If yes, set its basic parameters.
 *****************************************************************************/
int host_scan_for_device(void)
{ 
  if ((MCF_USB_CTL & MCF_USB_CTL_HOST_MODE_EN) == 0)
  {
    tr_error=tre_not_running;
    return(0);
  }

  tr_error=tre_none;

  if (chk_dev())
  {
    host_reset_bus();
    if (tr_error!= tre_none)
    {        
      return(0);
    }
    return(1);
  }
  return(0);
}

/*****************************************************************************
 * host_stop
 * IN: n/a
 * OUT: n/a
 * Assumptions: n/a
 * Description:
 *   Stop host operation.
 *****************************************************************************/
void host_stop(void)
{
  MKDBG_TRACE(ev_host_stop, 0);

  if ((MCF_USB_CTL & MCF_USB_CTL_HOST_MODE_EN) == 0)
  {
    tr_error=tre_not_running;
    return;
  }
  
  /* Disable all interrupt sources. */
  MCF_USB_INT_ENB = 0;
  MCF_USB_ERR_ENB = 0;
  /* Clear all pending events. */
  MCF_USB_INT_STAT =0;
  MCF_USB_ERR_STAT =0;
  /* Disable SOF generation. */
  MCF_USB_CTL &= ~MCF_USB_CTL_USB_EN_SOF_EN;

  /* Stop USB.*/
  MCF_USB_CTL = 0;
  
  MCF_USB_USB_CTRL = 0;
}

/*****************************************************************************
 * usb_host_start_transaction
 * IN: type   - type of transaction
 *     buffer - data area for the transfer
 *     length - size of the buffer
 * OUT: 
 *      -1: error
 *       x: number of processed bytes otherwise.
 * Assumptions: n/a
 * Description:
 *   Start the specified transatcion on the USB.
 *****************************************************************************/
static hcc_u16 usb_host_start_transaction(hcc_u8 type, hcc_u8 *buffer, hcc_u16 length, hcc_u8 ep)
{
  hcc_u8 token;
  hcc_u32* bdt_ctl;
  hcc_u8 retry=3;

  if ((MCF_USB_CTL & MCF_USB_CTL_HOST_MODE_EN) == 0)
  {
    tr_error=tre_not_running;
    return((hcc_u16)-1u);
  }

  /* Check if a device is attached. */
  if (my_device.address == INVALID_ADDRESS)
  {
    tr_error=tre_no_device;
    MKDBG_TRACE(ev_no_device, 0);
    return((hcc_u16)-1u);
  }
  
  if (my_device.eps[ep].address == INVALID_ADDRESS)
  {
    tr_error=tre_no_ep;
    MKDBG_TRACE(ev_no_ep, ep);
    return((hcc_u16)-1u);
  }

  /* Set device address. */
  MCF_USB_ADDR = (hcc_u8)(my_device.low_speed ? 
                        my_device.address | MCF_USB_ADDR_LS_EN : 
                        my_device.address);
  /* We are not talking over a HUB. */
  MCF_USB_ENDPT0 = MCF_USB_ENDPT0_HOST_WO_HUB;
 
  /* If this is an interrupt endpoint wait till the endpoint is due. */
  if (my_device.eps[ep].type == EPTYPE_INT)
  {      
    hcc_u16 elapsed;
    /* Disable hardware retry. */
    MCF_USB_ENDPT0 |= MCF_USB_ENDPT0_RETRY_DIS;
    /* Disable software retry. */
    retry=1;

    /* wait till frame is due */
    do {
      elapsed=(hcc_u16)(MCF_USB_FRM_NUM-my_device.eps[ep].last_due);
      elapsed &= ((1<<11)-1);
      if (MCF_USB_INT_STAT & MCF_USB_INT_STAT_USB_RST)
      {
        evt_disconnect();
        tr_error=tre_disconnected;
        return((hcc_u16)-1u);
      }
    } while(elapsed < my_device.eps[ep].interval);

    
    my_device.eps[ep].last_due += my_device.eps[ep].interval;
    my_device.eps[ep].last_due &= ((1<<11)-1);    
  }
  
  do  {
    tr_error=tre_none;  
    retry--;
    switch(type)
    {
      case TRT_SETUP:
        MKDBG_TRACE(ev_trt_setup, ep);
        /* Configure bi-directional communication. */
        /* Usb spec says setup packets shall be accepted 
           even if the device was not able to process its packet
           buffer. Thus NAK handshake shaould not be given to a 
           setup packet by the device. Anyway some devices will
           happily NAK setup packets :( */
        MCF_USB_ENDPT0 |= /*MCF_USB_ENDPT0_RETRY_DIS |*/ 0x0d;
    
        /* After the setup we shall send/receive DATA1 packets. */
        my_device.eps[ep].tgl_tx=1;
        my_device.eps[ep].tgl_rx=1;
        /* Set data buffer address. */
        WR_LE32(&BDT_ADR_TX(0, ep_info.next_tx), (hcc_u32)buffer);
        /* Set packet properties and give buffer to USB. */
        bdt_ctl=&BDT_CTL_TX(0, ep_info.next_tx);
        WR_LE32(bdt_ctl, (0x8<<16) | BDT_CTL_OWN | 0);
        /* Wait till pending tokens are processed. */ 
#if DSC_BUG == 0
        while(MCF_USB_CTL & MCF_USB_CTL_TXDSUSPEND_TOKBUSY)
        {
          if (MCF_USB_INT_STAT & MCF_USB_INT_STAT_USB_RST)
          {
            MCF_USB_CTL &= ~MCF_USB_CTL_TXDSUSPEND_TOKBUSY;
            evt_disconnect();
            tr_error=tre_disconnected;
            return((hcc_u16)-1u);
          }
        }
#endif  
        /* Start transaction. */
        MCF_USB_TOKEN=(hcc_u8)((TOKEN_SETUP<<4) | (my_device.eps[ep].address | (0<<7)));
        break;
      case TRT_IN:
        MKDBG_TRACE(ev_trt_in, ep);
        /* Configure bi-directional communication + auto repeat. */
        MCF_USB_ENDPT0 |= 0x0d;
        /* Set RX buffer address. */
        WR_LE32(&BDT_ADR_RX(0, ep_info.next_rx), (hcc_u32)buffer);
        /* Set packet properies and give next buffer to USB. */
        bdt_ctl=&BDT_CTL_RX(0, ep_info.next_rx);     
        WR_LE32(bdt_ctl, (length<<16) | BDT_CTL_OWN | my_device.eps[ep].tgl_rx);
        my_device.eps[ep].tgl_rx =(hcc_u8)(my_device.eps[ep].tgl_rx ? 0 : BDT_CTL_DATA);
        /* Wait till pending tokens are processed. */ 
#if DSC_BUG == 0        
        while(MCF_USB_CTL & MCF_USB_CTL_TXDSUSPEND_TOKBUSY)
        {
          if (MCF_USB_INT_STAT & MCF_USB_INT_STAT_USB_RST)
          {
            MCF_USB_CTL &= ~MCF_USB_CTL_TXDSUSPEND_TOKBUSY;          
            evt_disconnect();
            tr_error=tre_disconnected;
            return((hcc_u16)-1u);
          }
        }
#endif          
        /* Start transaction. */
        MCF_USB_TOKEN=(hcc_u8)((TOKEN_IN<<4) | (my_device.eps[ep].address | (1<<7)));
        break;
      case TRT_OUT:
        MKDBG_TRACE(ev_trt_out, ep);
        /* Configure bi-directional communication + auto repeat. */
        MCF_USB_ENDPT0 |= 0x0d;
        /* Set TX buffer address. */
        WR_LE32(&BDT_ADR_TX(0, ep_info.next_tx), (hcc_u32)buffer);
        /* Set packet properties ang give buffer to USB. */
        bdt_ctl=&BDT_CTL_TX(0, ep_info.next_tx);
        WR_LE32(bdt_ctl, (length<<16) | BDT_CTL_OWN | my_device.eps[ep].tgl_tx);
        my_device.eps[ep].tgl_tx = (hcc_u8)(my_device.eps[ep].tgl_tx ? 0 : BDT_CTL_DATA);
        /* Wait till pending tokens are processed. */ 
#if DSC_BUG == 0        
        while(MCF_USB_CTL & MCF_USB_CTL_TXDSUSPEND_TOKBUSY)
        {
          if (MCF_USB_INT_STAT & MCF_USB_INT_STAT_USB_RST)
          {
            MCF_USB_CTL &= ~MCF_USB_CTL_TXDSUSPEND_TOKBUSY;          
            evt_disconnect();
            tr_error=tre_disconnected;
            return((hcc_u16)-1u);
          }
        }
#endif        
        /* Start transaction. */
        MCF_USB_TOKEN=(hcc_u8)((TOKEN_OUT<<4) | (my_device.eps[ep].address | (0<<7)));
        break;
      default:
        CMX_ASSERT(0);
    }
 
    /* Wait for transaction end. */
    while((MCF_USB_INT_STAT & (MCF_USB_INT_STAT_TOK_DNE | MCF_USB_INT_STAT_STALL | MCF_USB_INT_STAT_ERROR)) ==0)
    {
      if (MCF_USB_INT_STAT & MCF_USB_INT_STAT_USB_RST)
      {
        evt_disconnect();
        tr_error=tre_disconnected;
        return((hcc_u16)-1u);
      }
    }

    if ((MCF_USB_ERR_STAT & ~MCF_USB_ERR_STAT_CRC5_EOF) != 0)    
    {
      MCF_USB_ERR_STAT = 0xff;
      MCF_USB_INT_STAT = MCF_USB_INT_STAT_ERROR;
      tr_error=tre_data_error;
      continue;
    }

    /* device not disconnected while waiting for an aswer */
    /* clear transfer ok event */
    MCF_USB_INT_STAT=MCF_USB_INT_STAT_TOK_DNE;
    MCF_USB_CTL &= ~MCF_USB_CTL_TXDSUSPEND_TOKBUSY;
    
    /* switch to next buffer */
    if (type== TRT_SETUP || type == TRT_OUT)
    {  
      ep_info.next_tx ^= 0x1u;
    }
    else
    {
      ep_info.next_rx ^= 0x1u;      
    }
      
    if (MCF_USB_INT_STAT & MCF_USB_INT_STAT_STALL)
    {        /* device has an error */
      MCF_USB_INT_STAT=MCF_USB_INT_STAT_STALL;
      MKDBG_TRACE(ev_got_stall, ep);
      tr_error=tre_stall;
      return((hcc_u16)-1u);
    }
      
    /* Check transaction status.*/
    token=(hcc_u8)((RD_LE32(bdt_ctl) >> 2) & 0x0f);
    
    /* transaction accepted by device */
    switch (token)
    {
    default:
    case TOKEN_ACK:
      MKDBG_TRACE(ev_got_ack, ep);
      return((hcc_u16)((RD_LE32(bdt_ctl) >> 16u) & 0x3ffu));
    case TOKEN_NAK:
      /* device is not ready */
      MKDBG_TRACE(ev_got_nak, ep);      
      if (my_device.eps[ep].type == EPTYPE_INT)
      {
        return(0);
      }
      /* retry */
      break;
    case TOKEN_STALL: /* endpoint stalled by device */
      /* we can not get here beacuse we already checked
         INT_STAT_STALL. */
      tr_error=tre_stall;
      return((hcc_u16)-1u);
    case 0xf:  /* data error, retry */
      MKDBG_TRACE(ev_got_data_error, ep);
      tr_error=tre_data_error;
      break;
    case 0:  /* no answer, retry. */
      MKDBG_TRACE(ev_got_unknown, ep);
      tr_error=tre_silent;
      break; 
    }
  }while(retry);
  return((hcc_u16)-1u);
}

/*****************************************************************************
 * host_send_control
 * IN: buffer - data area for the transfer
 *     length - size of the buffer
 * OUT: != -1: Number of transmitted data bytes
 *      -1   : error
 * Assumptions: n/a
 * Description:
 *   Make an OUT transfer on a control endpoint.
 *****************************************************************************/
hcc_u16 host_send_control(hcc_u8 *setup_data, hcc_u8* buffer, hcc_u8 ep)
{
  hcc_u32 curr=0;
  hcc_u16 length=RD_LE16(setup_data+6);
  
  MKDBG_TRACE(ev_send_control, ep);
  
  if ((MCF_USB_CTL & MCF_USB_CTL_HOST_MODE_EN) == 0)
  {
    tr_error=tre_not_running;
    return((hcc_u16)-1u);
  }
  
  /* setup transaction. */  
  if (((hcc_u16)-1u)==usb_host_start_transaction(TRT_SETUP, setup_data, 8, ep))
  {
    return((hcc_u16)-1u);
  }
 
  /* data transactions */
  while(curr<length)
  {
    hcc_u16 psize=(hcc_u16)(MIN(my_device.eps[ep].psize, length));
    hcc_u8 r=(hcc_u8)usb_host_start_transaction(TRT_OUT, buffer+curr, psize, ep);
    if (r != psize)
    {
      CMX_ASSERT(r==((hcc_u8)-1u));
      return((hcc_u16)-1u);
    }
    curr += psize;
  }
  /* handshake transaction */
  my_device.eps[ep].tgl_rx = BDT_CTL_DATA;
  if (((hcc_u16)-1u)==usb_host_start_transaction(TRT_IN, (void *)0, 0, ep))
  {
    return((hcc_u16)-1u);
  }

  return((hcc_u16)curr);
}

/*****************************************************************************
 * host_send
 * IN: buffer - data area for the transfer
 *     length - size of the buffer
 * OUT: != -1: Number of transmitted data bytes
 *      -1   : error
 * Assumptions: n/a
 * Description:
 *   Make an OUT transfer on a non control endpoint.
 *****************************************************************************/
hcc_u32 host_send(hcc_u8* buffer, hcc_u32 length, hcc_u8 ep)
{
  hcc_u32 curr=0;
   
  MKDBG_TRACE(ev_send, ep);

  if ((MCF_USB_CTL & MCF_USB_CTL_HOST_MODE_EN) == 0)
  {
    tr_error=tre_not_running;
    return(0);
  }
  
  /* data transactions */
  while(curr<length)
  {
    hcc_u16 psize=(hcc_u16)(MIN(my_device.eps[ep].psize, length));
    /* do transaction */
    hcc_u16 r=usb_host_start_transaction(TRT_OUT, buffer+curr, psize, ep);
    if (r!=psize)
    {
      CMX_ASSERT(r==((hcc_u16)-1u));
      break;
    }
    curr += psize;    
  }
  return(curr);
}

/*****************************************************************************
 * host_receive
 * IN: buffer - data area for the transfer
 *     length - size of the buffer
 * OUT: != -1: Number of transmitted data bytes
 *      -1   : error
 * Assumptions: n/a
 * Description:
 *   Make an IN transfer on a control endpoint.
 *****************************************************************************/ 
hcc_u32 host_receive(hcc_u8* buffer, hcc_u32 length, hcc_u8 ep)
{
  hcc_u32 curr=0;
  
  MKDBG_TRACE(ev_receive, ep);

  if ((MCF_USB_CTL & MCF_USB_CTL_HOST_MODE_EN) == 0)
  {
    tr_error=tre_not_running;
    return(0);
  } 
 
  /* data transactions */
  while(curr<length)
  {
    hcc_u16 got,
            psize=(hcc_u16)(MIN(my_device.eps[ep].psize, length));
    got=usb_host_start_transaction(TRT_IN, buffer+curr, psize, ep);
    if (got == ((hcc_u16)-1u))
    {
      break;
    }
    curr += got;
    /* short packet means end of transfer */
    if (got != my_device.eps[ep].psize)
    {
      break;
    }
  }
  return(curr);
}


/*****************************************************************************
 * host_receive_control
 * IN: buffer - data area for the transfer
 *     length - size of the buffer
 * OUT: != -1: Number of transmitted data bytes
 *      -1   : error
 * Assumptions: n/a
 * Description:
 *   Make an IN transfer on a control endpoint.
 *****************************************************************************/ 
hcc_u16 host_receive_control(hcc_u8 *setup_data, hcc_u8* buffer, hcc_u8 ep)
{
  hcc_u32 curr=0;
  hcc_u16 length = RD_LE16(setup_data+6);
  
  MKDBG_TRACE(ev_receive_control, ep);
  /* setup transaction. */ 

  if ((MCF_USB_CTL & MCF_USB_CTL_HOST_MODE_EN) == 0)
  {
    tr_error=tre_not_running;
    return((hcc_u16) -1u);
  }

  if (((hcc_u16)-1u)==usb_host_start_transaction(TRT_SETUP, setup_data, 8, ep))
  {
    return((hcc_u16)-1u);
  }
  
  /* data transactions */
  while(curr<length)
  {
    hcc_u16 got,
            psize=(hcc_u16)(MIN(my_device.eps[ep].psize, length));
    got=usb_host_start_transaction(TRT_IN, buffer+curr, psize, ep);
    curr += got;
    if (got == ((hcc_u16)-1u))
    {
      return((hcc_u16)-1u);
    }
    /* short packet means end of transfer */
    if (got != my_device.eps[ep].psize)
    {
      break;
    }
  }
  /* handshake transaction */
  my_device.eps[0].tgl_tx = BDT_CTL_DATA;
  if (((hcc_u16)-1u)==usb_host_start_transaction(TRT_OUT, (void *)0, 0, ep))
  {
    return((hcc_u16)-1u);
  }

  return((hcc_u16)curr);
}

void host_ms_delay(hcc_u32 delay)
{
  start_mS_timer((hcc_u16)delay);
  do {
  } while(!check_mS_timer());
}

/*****************************************************************************
 * host_drop_device
 * IN: n/a
 * OUT: n/a
 * Assumptions: n/a
 * Description:
 *   Remove device fro host. 
 *****************************************************************************/ 
static void host_drop_device(void)
{

  if ((MCF_USB_CTL & MCF_USB_CTL_HOST_MODE_EN) == 0)
  {
    tr_error=tre_not_running;
    return;
  }

  /* if we don't have any device */
  if (my_device.address == INVALID_ADDRESS)
  {
    return;
  }
  host_sleep();
}


/*****************************************************************************
 * host_sleep
 * IN: n/a
 * OUT: n/a
 * Assumptions: n/a
 * Description:
 *   Put the BUS into low-power mode.
 *****************************************************************************/ 
void host_sleep(void)
{
  /* Sleep does not seems to work when SOF_TOK is off. Thus we treat
     resume=0 while suspended as sleep. */
  MCF_USB_INT_STAT = MCF_USB_INT_STAT_RESUME 
                     | MCF_USB_INT_STAT_SOF_TOK;
  /* disable SOF generation */
  MCF_USB_CTL &= ~MCF_USB_CTL_USB_EN_SOF_EN;
  /* Wait 3 mS to let BUS enter suspended state. */
  host_ms_delay(3);
}

/*****************************************************************************
 * host_wakeup
 * IN: n/a
 * OUT: n/a
 * Assumptions: n/a
 * Description:
 *   Wake-up.
 *****************************************************************************/ 
void host_wakeup(void)
{
  if ((MCF_USB_CTL & MCF_USB_CTL_HOST_MODE_EN) == 0)
  {
    tr_error=tre_not_running;
    return;
  }

  /* if we don't have any device */
  if (my_device.address == INVALID_ADDRESS)
  {
    return;
  }

  /* Check if device has been disconnected. */
  if (!chk_dev())
  {
    return;
  }
  
  /* Start wakeup signaling. */
  MCF_USB_CTL |= MCF_USB_CTL_RESUME;
  host_ms_delay(20);
  MCF_USB_CTL &= ~MCF_USB_CTL_RESUME;
  
  /* enable SOF generation */
  MCF_USB_CTL |= MCF_USB_CTL_USB_EN_SOF_EN;    
  MCF_USB_INT_STAT = MCF_USB_INT_STAT_SLEEP | MCF_USB_INT_STAT_RESUME;  
  /* Wakeup recovery. */
  host_ms_delay(10); 
}

/****************************** END OF FILE **********************************/
