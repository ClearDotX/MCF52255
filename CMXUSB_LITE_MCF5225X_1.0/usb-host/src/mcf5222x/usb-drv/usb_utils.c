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
#include "usb_host.h"
#include "usb_utils.h"

/* Descriptors will be read to this buffer. */
hcc_u8 dbuffer[DBUFFER_SIZE];
std_error_t std_error;
/*****************************************************************************
 * fill_setup_packet
 * IN: 
 * OUT:
 * Assumptions: n/a
 * Description:
 *   Create data for a setup packet. 
 *****************************************************************************/
void fill_setup_packet(hcc_u8* dst, hcc_u8 dir, hcc_u8 type, hcc_u8 recipient,
                       hcc_u8 req, hcc_u16 val, hcc_u16 ndx, hcc_u16 len)
{
  dst[0]=(hcc_u8)(dir | type | recipient);
  dst[1]=req;
  dst[2]=(hcc_u8)val;
  dst[3]=(hcc_u8)(val>>8);
  dst[4]=(hcc_u8)ndx;
  dst[5]=(hcc_u8)(ndx>>8);
  dst[6]=(hcc_u8)(len);
  dst[7]=(hcc_u8)(len>>8);
}

/*****************************************************************************
 * get_dev_desc
 * IN: 
 * OUT:
 * Assumptions: n/a
 * Description:
 *   Read device desriptor to dbuffer.  
 *****************************************************************************/
int get_dev_desc(void)
{    
   hcc_u8 setup[8];
   hcc_u8 retry=3;
   
   std_error=stderr_none;
   fill_setup_packet(setup, STP_DIR_IN, STP_TYPE_STD, STP_RECIPIENT_DEVICE,
   STDRQ_GET_DESCRIPTOR, (STDDTYPE_DEVICE<<8)|0, 0, 18);
   
   do {    
     if (18 == host_receive_control(setup, dbuffer, 0))
     {
       /* Check returned descriptor type and length (ignore extra bytes). */
       if ((USBDSC_TYPE(dbuffer) == STDDTYPE_DEVICE) 
          && (USBDSC_LENGTH(dbuffer) <= 18))   
       {
         return(0);
       }
     }
   } while(retry--);
   std_error=stderr_host;
   return(1);
}

/*****************************************************************************
 * get_cfg_desc
 * IN: 
 * OUT:
 * Assumptions: n/a
 * Description:
 *   Read device desriptor to dbuffer.  
 *****************************************************************************/
int get_cfg_desc(hcc_u8 ndx)
{    
   hcc_u8 setup[8];
   hcc_u16 length=5;
   hcc_u8 retry=3;

   std_error=stderr_none;      
   do {
     fill_setup_packet(setup, STP_DIR_IN, STP_TYPE_STD, STP_RECIPIENT_DEVICE,
     STDRQ_GET_DESCRIPTOR, (hcc_u16)((STDDTYPE_CONFIGURATION<<8)|ndx), 0, length);
     if (length == host_receive_control(setup, dbuffer, 0))
     {
       /* Check returned descriptor type and length (ignore extra bytes) */
       if ((USBDSC_TYPE(dbuffer) == STDDTYPE_CONFIGURATION)
          && (USBDSC_LENGTH(dbuffer) <= 9))
       {
         length=RD_LE16(dbuffer+2);       
         CMX_ASSERT(length < DBUFFER_SIZE);   
       
         fill_setup_packet(setup, STP_DIR_IN, STP_TYPE_STD, STP_RECIPIENT_DEVICE,
         STDRQ_GET_DESCRIPTOR, (hcc_u16)((STDDTYPE_CONFIGURATION<<8)|ndx), 0, length);
       
         if (length == host_receive_control(setup, dbuffer, 0))
         {
           return(0);
         }      
       }
     }
   }while(retry--);
   std_error=stderr_host;
   return(1);
}

/*****************************************************************************
 * set_ep0_psize
 * IN: 
 * OUT:
 * Assumptions: n/a
 * Description:
 *   Read device desriptor to dbuffer.  
 *****************************************************************************/
int set_ep0_psize(void)
{    
  hcc_u8 setup[8];
   
  std_error=stderr_none;   
  fill_setup_packet(setup, STP_DIR_IN, STP_TYPE_STD, STP_RECIPIENT_DEVICE,
  STDRQ_GET_DESCRIPTOR, (STDDTYPE_DEVICE<<8) | 0, 0, 8);
   
  if (8 == host_receive_control(setup, dbuffer, 0))
  {
    if ((USBDSC_TYPE(dbuffer) == STDDTYPE_DEVICE)
       && (USBDSC_LENGTH(dbuffer) <= 18))
    {
      host_modify_ep(0, EPTYPE_CTRL, 0, 0, DEVDESC_PACKET_SIZE(dbuffer));
      return(0);
    }
  }
  std_error=stderr_host;  
  return(1);
}

/*****************************************************************************
 * set_address
 * IN: 
 * OUT:
 * Assumptions: n/a
 * Description:
 *   Set device address.
 *****************************************************************************/
int set_address(hcc_u8 address)
{
  hcc_u8 setup[8];
  hcc_u8 retry=3;

  std_error=stderr_none;
  fill_setup_packet(setup, STP_DIR_OUT, STP_TYPE_STD, STP_RECIPIENT_DEVICE,
  STDRQ_SET_ADDRESS, address, 0, 0);
  
  do {    
    if (0==host_send_control(setup, dbuffer, 0))
    {
      /* we need to wait maximum 50 mS to let the device change its address. */
      host_ms_delay(45);    
      return(0);
    }
  }while(retry--);  
  std_error=stderr_host;  
  return(1);
}
/*****************************************************************************
 * set_config
 * IN: 
 * OUT:
 * Assumptions: n/a
 * Description:
 *   Set device configuration
 *****************************************************************************/
int set_config(hcc_u8 cfg)
{
  hcc_u8 setup[8];
  hcc_u8 retry=3;
 
  std_error=stderr_none;
  fill_setup_packet(setup, STP_DIR_OUT, STP_TYPE_STD, STP_RECIPIENT_DEVICE,
  STDRQ_SET_CONFIGURATION, cfg, 0, 0);
  do {      
    if (0==host_send_control(setup, dbuffer, 0))
    {
      return(0);
    }
  } while(retry--);
  std_error=stderr_host;
  return(1);
}

/*****************************************************************************
 * get_device_info
 * IN: 
 * OUT:
 * Assumptions: n/a
 * Description:
 *   Get device identifycation info.
 *****************************************************************************/
int get_device_info(device_info_t *res)
{
   /* read descriptor from device. */
   std_error=stderr_none;
   if (get_dev_desc())
   {
     std_error=stderr_host;
     return(1);
   }
   
   /* give read values to caller */  
   res->clas=DEVDESC_CLASS(dbuffer);
   res->sclas=DEVDESC_SCLASS(dbuffer);
   res->protocol=DEVDESC_PROTOCOL(dbuffer);
   res->rev=DEVDESC_REV(dbuffer);
   res->vid=DEVDESC_VID(dbuffer);
   res->pid=DEVDESC_PID(dbuffer);
   res->ncfg=DEVDESC_NCFG(dbuffer);
   
   return(0);
}

/*****************************************************************************
 * find_descriptor
 * IN: 
 * OUT:
 * Assumptions: n/a
 * Description:
 *   find a descriptor in the loaded configuration info 
 *****************************************************************************/
hcc_u16 find_descriptor(hcc_u8 type, hcc_u16 start, hcc_u8 next)
{
  hcc_u8 *p=dbuffer+start;
  
  CMX_ASSERT(USBDSC_TYPE(dbuffer) == STDDTYPE_CONFIGURATION);

  if (next)
  {
    p+=USBDSC_LENGTH(p);
  }

  while(p < dbuffer+CONFDESC_TOTLENGTH(dbuffer))
  {
    if(USBDSC_TYPE(p) == type)
    {
      return((hcc_u16)(p-dbuffer));
    }    
    p+=USBDSC_LENGTH(p);    
  }
  return((hcc_u16)-1u);
}
/*****************************************************************************
 * find_ifc_csp
 * IN: 
 * OUT:
 * Assumptions: n/a
 * Description:
 *   find an interface in the loaded config descriptor
 *****************************************************************************/
/* Currently only able to find the first matching ifc. Add start. */ 
hcc_u16 find_ifc_csp(hcc_u8 class, hcc_u8 sclass, hcc_u8 protocol)
{
  int ifc;
  hcc_u8 *p=dbuffer;      

  /* at least class must be specifyed */
  CMX_ASSERT(class != 0);
  /* config descriptor shall be in dbuffer */  
  CMX_ASSERT(USBDSC_TYPE(dbuffer) == STDDTYPE_CONFIGURATION);
  /* pharse configuration descriptor */
  for(ifc=0; ifc < CONFDESC_INTRFACES(dbuffer); ifc++)
  {
    while(USBDSC_TYPE(p) != STDDTYPE_INTERFACE)
    {
      p+=USBDSC_LENGTH(p);
    }
        
    /* if interface class is ok */
    if (IFCDESC_CLASS(p) == class)
    {
      /* check subclass and protocol if needed */
      if (((sclass == 0) || (IFCDESC_SCLASS(p) == sclass))
         && ((protocol == 0) || (IFCDESC_PROTOCOL(p) == protocol)))
      { /* return start address of interface */
        return((hcc_u8)((hcc_u32)p-(hcc_u32)dbuffer));
      }
    }
    p+=USBDSC_LENGTH(p);
  }
  return(0);
}

/*****************************************************************************
 * find_ifc_ndx
 * IN: 
 * OUT:
 * Assumptions: n/a
 * Description:
 *   find an interface in the loaded config descriptor
 *****************************************************************************/
hcc_u16 find_ifc_ndx(hcc_u8 ndx)
{
  int ifc;
  hcc_u8 *p=dbuffer;      
  
  /* config descriptor shall be in dbuffer */
  CMX_ASSERT(USBDSC_TYPE(dbuffer) == STDDTYPE_CONFIGURATION);    
  
  /* pharse configuration descriptor */
  for(ifc=0; ifc < CONFDESC_INTRFACES(dbuffer); ifc++)
  {
    while(USBDSC_TYPE(p) != STDDTYPE_INTERFACE)
    {
      p+=USBDSC_LENGTH(p);
    }
        
    /* Nonboot hid mouse */
    if (IFCDESC_MY_NDX(p) == ndx)
    {
      return((hcc_u16)((hcc_u32)p-(hcc_u32)dbuffer));
    }
    p+=USBDSC_LENGTH(p);
  }
  return(0);  
}

/*****************************************************************************
 * get_cfg_info
 * IN: 
 * OUT:
 * Assumptions: n/a
 * Description:
 *   Get configuration info
 *****************************************************************************/
int get_cfg_info(cfg_info_t *res)
{
  res->nifc=CONFDESC_INTRFACES(dbuffer);
  res->ndx=CONFDESC_MY_NDX(dbuffer);
  res->str=CONFDESC_MY_STR(dbuffer);
  res->attrib=CONFDESC_ATTRIB(dbuffer);
  res->max_power=CONFDESC_MAX_POW(dbuffer);
  return(0);
}

/*****************************************************************************
 * get_ifc_info
 * IN: 
 * OUT:
 * Assumptions: n/a
 * Description:
 *   Get interface info
 *****************************************************************************/
int get_ifc_info(ifc_info_t *res, hcc_u16 offset)
{
  res->clas=IFCDESC_CLASS(&dbuffer[offset]);
  res->sclas=IFCDESC_SCLASS(&dbuffer[offset]);
  res->protocol=IFCDESC_PROTOCOL(&dbuffer[offset]);
  res->ndx=IFCDESC_MY_NDX(&dbuffer[offset]);
  res->alt_set=IFCDESC_ALTERNATE(&dbuffer[offset]);
  res->str=IFCDESC_MY_STR(&dbuffer[offset]);
  res->nep=IFCDESC_ENDPONTS(&dbuffer[offset]);  
  return(0);
}

/*****************************************************************************
 * get_ep_info
 * IN: 
 * OUT:
 * Assumptions: n/a
 * Description:
 *   Get endpoint info
 *****************************************************************************/
int get_ep_info(ep_info_t *res, hcc_u16 offset)
{
  res->address=EPDESC_ADDRESS(&dbuffer[offset]);
  res->type=EPDESC_ATTRIB(&dbuffer[offset]);
  res->interval=EPDESC_INTERVAL(&dbuffer[offset]);
  res->psize=EPDESC_PSIZE(&dbuffer[offset]);
  return(0);
}
/****************************** END OF FILE **********************************/
