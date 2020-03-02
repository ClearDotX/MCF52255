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
#include "scsi.h"
#include "usb_mst.h"
#include "usb-drv/usb_host.h"
#include "usb-drv/usb_utils.h"

#define USB_RERTY_COUNT 3

typedef enum {
  msterr_none,
  msterr_comunication_error
} mst_error_t;

struct {
  hcc_u16 ifcd_offset;
  hcc_u8 cfg_ndx;
  hcc_u8 ifc_ndx;
  hcc_u8 alt_set;
  hcc_u8 in_ep;
  hcc_u8 out_ep;
  hcc_u8 nlun;
  mst_error_t mst_error;  
} mst_info;


/* class specific requests */
#define RQMST_MASS_STORAGE_RESET   0xffu
#define RQMST_GET_MAX_LUN          0xfeu

int usb_mst_reset(void)
{
  unsigned char setup[8]={
    (0<<7) | (1<<5) | 1       /*out, class specific, to interface */
    ,RQMST_MASS_STORAGE_RESET /* command */
    ,0,0                      /* value = 0 */
    ,0,0                      /* index = 0 */
    ,0,0                      /* length = 0 */
  };

  mst_info.mst_error=msterr_none;
  if (0!=host_send_control((hcc_u8 *)setup, 0, 0))
  {
    mst_info.mst_error=msterr_comunication_error;
    return(1);
  }

  return(0);
}

static int usb_clear_stall_rd(void)
{
  hcc_u8 setup[8];
  mst_info.mst_error=msterr_none;
  fill_setup_packet(setup, STP_DIR_OUT, STP_TYPE_STD, STP_RECIPIENT_ENDPT
  ,STDRQ_CLEAR_FEATURE, 0, (hcc_u8)(mst_info.in_ep|0x80), 0);
  if (0!=host_send_control((hcc_u8 *)setup, 0, 0))
  {
    mst_info.mst_error=msterr_comunication_error;
    return(1);
  }
  return(0);
}

static int usb_clear_stall_wr(void)
{
  hcc_u8 setup[8];
  mst_info.mst_error=msterr_none;  
  fill_setup_packet(setup, STP_DIR_OUT, STP_TYPE_STD, STP_RECIPIENT_ENDPT
  ,STDRQ_CLEAR_FEATURE, 0, mst_info.out_ep, 0);
  if (0!=host_send_control((hcc_u8 *)setup, 0, 0))
  {
    mst_info.mst_error=msterr_comunication_error;
    return(1);
  }
  return(0);
}

int usb_mst_getnlun(void)
{
  unsigned char nlun=0;

  unsigned char setup[8]={
    (1<<7) | (1<<5) | 1       /* in, class specific, to interface */
    ,RQMST_GET_MAX_LUN        /* command */
    ,0,0                      /* value = 0 */
    ,0,0                      /* index = 0 */
    ,1,0                      /* length = 1 */
  };
  mst_info.mst_error=msterr_none;
  if (1 != host_receive_control((hcc_u8 *)setup, &nlun, 0))
  {
    if (tr_error != tre_stall)
    {        
    mst_info.mst_error=msterr_comunication_error;
    return(-1);
  }
    nlun=0;
  }
  return(nlun);
}


static int usb_mst_search_ifc(void)
{
  device_info_t dev_inf;
  ifc_info_t ifc_info;
  cfg_info_t cfg_info;
  hcc_u8 cfg;

  mst_info.ifcd_offset=0;

  /* see what device is this */
  if (get_device_info(&dev_inf))
  {
    return(0);
  }

  /* Mass-storage devices have the class code in the interface
     descriptor(s) and not in the device descriptor. Look for
     appropriate class code in interface desctiptor. */
  /* check all configurations */
  for(cfg=0; cfg < dev_inf.ncfg; cfg++)
  {
    hcc_u16 ifc;
    hcc_u8 ep;
    hcc_u16 ep_ofs;

    /* get the configuration descriptor */
    if (get_cfg_desc(cfg))
    {
      continue;
    };

    /* find a mass-storage interface in this configuration */
    ifc=find_ifc_csp(0x08, 0, 0);

    if (!ifc)
    {
      /* not found, get the next configuration */
      continue;
    }

    /* get interface info */
    get_ifc_info(&ifc_info, ifc);

    if(ifc_info.sclas != 0x6 && ifc_info.protocol != 0x50)
    { /* we only support the SCSI command set and the 
         bulk only protocol.  */
       continue;
    }

    /* At this point we found a configuration that has the right 
       mass storage interface. 
       Store information needed for operating the device. */
    get_cfg_info(&cfg_info);
    mst_info.cfg_ndx=cfg_info.ndx;

    mst_info.ifc_ndx=ifc_info.ndx;
    mst_info.ifcd_offset=ifc;
    mst_info.alt_set=ifc_info.alt_set;
    
    /* Find bulk in + out endpoints. */
    ep_ofs=ifc;
    for(ep=0; ep < ifc_info.nep; ep++)
    {
      ep_info_t ep_inf;
      ep_ofs=find_descriptor(STDDTYPE_ENDPOINT, ep_ofs, 1);
      get_ep_info(&ep_inf, ep_ofs);
      if (ep_inf.type==EPTYPE_BULK)
      {
        if (ep_inf.address > 0x80)
        {
          mst_info.in_ep=host_add_ep(ep_inf.type, (hcc_u8)ep_inf.address, 0, ep_inf.psize);
        }
        else
        {
          mst_info.out_ep=host_add_ep(ep_inf.type, (hcc_u8)ep_inf.address, 0, ep_inf.psize);
        }
      }
    }
    
    if (mst_info.in_ep == 0 || mst_info.out_ep == 0)
    {
      /* we need one in and one out endpoint */
      continue;
    }
    /* start device */
    set_config(mst_info.cfg_ndx);
    return(1);
  }
  return(0);  
}

int usb_mst_init(void)
{
  mst_info.mst_error=msterr_none;
  mst_info.nlun=0;
  /* Currently SCSI init will return allways 0. */
  if (usb_mst_search_ifc())
  {   
    mst_info.nlun=(hcc_u8)(usb_mst_getnlun()+1u);
  
    if (mst_info.nlun == 0)
    {
      return(0);
    }
  }
  return(mst_info.nlun);
}

static int usb_mst_soft_reset_recovery(void)
{
  /* Send "soft" reset. (request on ep 0)*/
  if (usb_mst_reset())
  {
    return(ERR_IO_REMOVED);
  }
  /* Clear stall on bulk in */
  if (usb_clear_stall_rd())
  {
    return(ERR_IO_REMOVED);
  }
  /* Clear stall on bulk out */
  if (usb_clear_stall_wr())
  {
    return(ERR_IO_REMOVED);
  }
  return(0);
}

int usb_mst_transfer(hcc_u8 cmd_length, void *cmd, hcc_u8 dir, hcc_u32 buf_length, void *buf)
{
  hcc_u8 cbw[31];
  hcc_u8 csw[13];
  int retry,
      first_csw=1;
  hcc_u32 r;
  static hcc_u32 tag = 0x0;
  
  
  /* Clear SCSI part of CBW. (Other bytes will be set allways.) */
  for(r=0; r<16/4; r++)
  {
    ((hcc_u32*)(&cbw[15]))[r]=0u;
  }
  
  /* Create command block wrapper. */
    /* Signature */
  
  WR_BE32(cbw, 0x55534243);
   /* tag. */
  tag++;
  ((hcc_u32*)cbw)[1]=tag;
  /* Transfer length */
  WR_LE32(&cbw[8], buf_length);
  /* bmCBWFlags */
  cbw[12]=(hcc_u8)(dir ? 0x80 : 0);
  /* bmCBWLun */
  cbw[13]=(hcc_u8)(((char *)cmd)[1] >> 5);
  /* bCBWCBLength */
  cbw[14]=cmd_length;
  CMX_ASSERT(cmd_length < 17);
  for(r=0; r<cmd_length; r++)
  {
    (&cbw[15])[r]=((hcc_u8*)cmd)[r];
  }

  /* Send command. */
  retry=USB_RERTY_COUNT;
  retry_cmd:
  r=host_send((hcc_u8 *)cbw, 31, mst_info.out_ep);
  if (r != 31)
  {/* Command write error. Spec says we shall do reset recovery. So do it
      and retry. */
    if (--retry)
    {
      if (ERR_IO_REMOVED != usb_mst_soft_reset_recovery())
      {  
        goto retry_cmd;
      }
    }
    /* If all retry failde, then we treat the device as disconnected. */
    return(ERR_IO_REMOVED);
  }
  
  /* send/read data if needed */
  if (buf_length > 0 && dir != DIR_NONE)
  {
    switch(dir)
    {
    case DIR_IN: /* transfer data to host */
      r=host_receive((hcc_u8 *)buf, buf_length, mst_info.in_ep);
      if (r != buf_length)
      {
        /* Less data read than expected. In this state the spec says, the device may
          stall its endpoint to stop the transfer. Another way is if the device sends a 
          short packet. 
          Since on windows we don't know if the endpoint is stalled or not, we send a
          CLEAR_FEATURE ENDPOINT_HALT request to the endpoint anyway. Though this is not
          neccessary in the sencond case, it can not harm. After this we continue to the
          status stage, to see what happend. */
        usb_clear_stall_rd();
      }
      break;
    case DIR_OUT: /* transfer data to device */
      r=host_send((hcc_u8*)buf, buf_length, mst_info.out_ep);
      if (r != buf_length)
      {
        /* Data write failed. If the device is still connected, the only way this may
          happen, if the device stalled its input endpoint. 
          Clear stall, and continue to the status stage to see what happend. */
        usb_clear_stall_wr();
      }
      break;
    }
  }
  /* Read csw. */
  retry_csw:
  r=host_receive((hcc_u8*)csw, 13, mst_info.in_ep);
  /* Is there a read error? */
  if (r != 13)
  {
    /* Spec says, if the in endpoint is stalled, then we need to unstall it, and 
       retry reding the CSW. 
       On windows we don't know if the endpoint is stalled or not. So we try to
       unstall it (which may not harm), and retry. */
    if (first_csw)
    { /* If this is the first retry...*/
      first_csw=0;
      usb_clear_stall_rd();
      goto retry_csw;
    }
    if (ERR_IO_REMOVED != usb_mst_soft_reset_recovery())
    {
      /* Now we retry the whole transfer. */
      if (--retry)
      {
        goto retry_cmd;
      }
    }
    /* If the transfer is retried but still unsuccesfull, then 
       we threat the device as removed. */
    return(ERR_IO_REMOVED);
  }

  /* Process csw. */
  /* Check if CSW is valid. */
    /* Size shall be 13. Size less than 13 is already handled. Checking size
       more than 13 can be tricky. The host controller will request no more
       packets when at least 13 bytes have been received. If the packet size 
       is more than 13 bytes, then the host controller driver can tell us, that
       it has some unread bytes beyond the 13 we requested. If not, then an
       extra IN transfer should be done which shall be NAK -ed by the device to
       see, there is nore data to be read. A simple read will not stop when receiving
       a NAK from the device but it will retry till a timeout is due (or forever if 
       the driver does not support timeouts). So checking this can be lengthy and is
       dangerous.
       Lets see, how we may get to this state:
       -device wants to send more data during an IN transfer, and thus extra data 
        packets will be miss-threated as the status data. In this case having a valid 
        CSW is really unlikely (the signature + TAG may be invalid, combination of other 
        filed may be invalid). Thus there is only a really low chance we will not get an
        error while checking the CSW. This will result in a reset recovery and that will
        get us synchronised to the device again.
       -the device sends us some grabage before the real CSW. This is the same situation
        as abowe. The CSW will be invalid, and so on...
       -the device sends us some grabage after the real CSW. This means we will read out
        the grabage bytes during the next read, and thus the next command will likely fail.

       So it is more or less safe to not care about extra bytes here.*/

    /* Check the signature and the TAG. */
  if (0x55534253 != RD_BE32(csw) || tag != ((hcc_u32*)csw)[1])
  { /* CSW is invalid. 
       Spec says, we shall do reset recovery here and thus make the device ready to
       process the next CBW. Ok, we recover, and as "next command" retry the 
       transfer. */
    if (ERR_IO_REMOVED != usb_mst_soft_reset_recovery())
    {
      if (--retry)
      {
        goto retry_cmd;
      }
    }
    /* If we retried enough times, we treat the device as disconnected. */
    return(ERR_IO_REMOVED);
  }
    /* Check if the CSW is meaningfull. */
  if (!(((csw[12] == 0) || (csw[12] == 1)) && RD_LE32(&csw[8]) <= buf_length)
      || csw[12] > 2)
  { /* CSW is not meaning full. The specification does specified how to handle 
       this situation. We threat this the same as if the CSW would be invalid. */
    if (ERR_IO_REMOVED != usb_mst_soft_reset_recovery())
    {
      if (--retry)
      {
        goto retry_cmd;
      }
    }
    /* If we retried enough times, we treat the device as disconnected. */
    return(ERR_IO_REMOVED);
  }

  /* Check transfer status. */
  if (csw[12] == 2)
  { /* Device detected a phase error. Spec says, we shall do a reset recovery. 
       After that, we retry. */
    if (ERR_IO_REMOVED != usb_mst_soft_reset_recovery())
    {
      if (--retry)
      {
        goto retry_cmd;
      }
    }
    /* If we retried enough times, we treat the device as disconnected. */
    return(ERR_IO_REMOVED);
  }

  /* If the device reports no error, but some part of the buffer could not
     be processed by it, we will start another transfer with modified
     buffer pointer and length. (e.g. send/read the unprocessed data
     again. */
  if (csw[12] == 0 && RD_LE32(&csw[8]) != 0)
  { /* Modify buffer pointer and length. */
    buf=((hcc_u8*)buf)+buf_length-RD_LE32(&csw[8]);
    buf_length -= RD_LE32(&csw[8]);
    /* Retry. Note: we will not count number of retryes here, since the 
       device may send it's data in any number of parts. This is a bit 
       dangerous, since a faulty device may get us into an endless loop. */
    goto retry_cmd;
  }

  /* Device returned chck condition status. */
  if (csw[12] == 1)
  {
    /* Upper layer shall decide if a retry is needed or possible or not. */
    return(ERR_IO_CHECK);
  }
  return(ERR_IO_NONE);
}
/****************************** END OF FILE **********************************/
