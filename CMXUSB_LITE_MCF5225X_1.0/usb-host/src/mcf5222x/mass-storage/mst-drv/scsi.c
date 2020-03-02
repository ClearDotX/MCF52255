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
#include "usb-drv/usb_host.h"

/******************************************************************************
 Macro definitions 
 *****************************************************************************/
#define RETRY_WAIT_MS  10

/* Known SCSI peripheral device types (PDTs) */
  /* Direct access device. */
#define SCSI_PDT_DA       0x0
  /* CDROM device. */
#define SCSI_PDT_CD       0x5
  /* Optical memory device. */
#define SCSI_PDT_OM       0x7
  /* Simplified direct-access device */
#define SCSI_PDT_SDA      0x0e


/* Mandatory requests for all usb mass-storage devices. */
#define SCSI_CMD_INQUIRY            0x12u
#define SCSI_CMD_REQUEST_SENSE      0x03u
#define SCSI_CMD_SEND_DIAGNOSTIC    0x1du
#define SCSI_CMD_TEST_UNIT_READY    0x00u

/* Requests for direct access devices. */
#define SCSI_CMD_FORMAT_UNIT        0x04u
#define SCSI_CMD_READ_10            0x28u
#define SCSI_CMD_READ_12            0xa8u
#define SCSI_CMD_READ_CAPACITY      0x25u
#define SCSI_CMD_RELEASE            0x17u
#define SCSI_CMD_RESERVE            0x16u
#define SCSI_CMD_WRITE_10           0x2au
#define SCSI_CMD_WRITE_12           0xaau
#define SCSI_CMD_START_STOP         0x1bu
#define SCSI_CMD_TOCPMAATIP         0x43u
/* Sense keys fot direct access devices: */
  /* No problem. */
#define SCSI_SK_NO_SENSE          0x00
  /* Recovered read or write error */
#define SCSI_SK_RECOVERED_ERROR   0x01
  /* Device is not ready. */
#define SCSI_SK_NOT_READY         0x02
  /* Unrecovered read error */
#define SCSI_SK_MEDIUM_ERROR      0x03
  /* Self diagnostic failed, Unrecovered read error. */
#define SCSI_SK_HARDWARE_ERROR    0x04
  /* Invalid logical block address, Unsupported option requested */
#define SCSI_SK_ILLEGAL_REQUEST   0x05
  /* Target reset or medium change	since last command. */
#define SCSI_SK_UNIT_ATTENTION    0x06
  /* Overrun or other error that might be resolved by repeating the command */
#define SCSI_SK_ABORTED_COMMAND   
  /*Attempt to write on write protected medium */
#define SCSI_SK_DATA_PROTECT    

/*#define RD_BE32(p) (((p)[0] << 24) | ((p)[1] << 16) | ((p)[2] << 8) | ((p)[3] & 0xff))
#define WR_BE32(p, w) (((((hcc_u8*)(p))[0] = (w) >> 24), (((hcc_u8*)(p))[1] = (w) >> 16)) \
                        , (((hcc_u8*)(p))[2] = (w) >> 8), (((hcc_u8*)(p))[3] = (w) >> 0))
#define WR_BE16(p, w) ((((hcc_u8*)(p))[0] = (w) >> 8), (((hcc_u8*)(p))[1] = (w) >> 0))

*/

/******************************************************************************
 Type definitions 
 *****************************************************************************/
 /* none */
 
/******************************************************************************
 Function pre-definitions
 *****************************************************************************/
 static int scsi_cmd_inquiry(void);
 static int scsi_cmd_read_capacity(void);
 static int scsi_cmd_test_unit_ready(void);
 static int scsi_cmd_request_sense(void);
 static int scsi_cmd_read_10(hcc_u32 lba, hcc_u16 length, void *buffer);
 static int scsi_cmd_read_12(hcc_u32 lba, hcc_u32 length, void *buffer);
 static int scsi_cmd_write_10(hcc_u32 lba, hcc_u16 length, void *buffer);
 static int scsi_cmd_write_12(hcc_u32 lba, hcc_u32 length, void *buffer);
/******************************************************************************
 Local variables
 *****************************************************************************/
static rw_func_t *transfer;
static lun_info_t lun;
/******************************************************************************
 Function declaration
 *****************************************************************************/
/******************************************************************************
 scsi_init

 Description:
   Inicializes the SCSI layer. Shall be called once before any other SCSI 
   function.
 *****************************************************************************/
int scsi_init(void)
{
  transfer=0;
  lun.error_code=LERR_INVALID_DEVICE;
  lun.sector_size=0;
  return(0);
}

/******************************************************************************
 scsi_reset_lun

 Description:
   Reset the specified logical unit.
 *****************************************************************************/
int scsi_reset_lun(void)
{   
  lun.error_code=LERR_NONE;
  lun.pdt=0;
  lun.no_of_sectors=0;
  lun.prod_id[0]='\0';
  lun.rev[0]='\0';
  lun.vendor[0]='\0';
  lun.sector_size=0;
  lun.sense_data=0;
  lun.sense_info=0;

  /* Read LUN info (type, and some strings) */
  if (scsi_cmd_inquiry())
  {
    return(1);
  }
  /* check */
  scsi_cmd_test_unit_ready();
  /* Read media size. */
  if (scsi_cmd_read_capacity())
  {
    return(1);
  }
  return(0);
}
/******************************************************************************
 scsi_open_device

 Description:
   Add a new SCSI device. All logical units of the device will be inicialized
   too.
 *****************************************************************************/
int scsi_open_device(rw_func_t *f)
{ 
  int r=0;
  
  CMX_ASSERT(f != 0);
  
  transfer=f;
  r=scsi_reset_lun();
  return(r);
}

/******************************************************************************
 scsi_close_device

 Description:
   Remove an SCSI device.
 *****************************************************************************/
int scsi_close_device()
{
  transfer=0;
  lun.error_code=LERR_INVALID_DEVICE;
  lun.sector_size=0;
  return(0);
}

/******************************************************************************
 scsi_copy32

 Description:
   Copy 32bit words. 
 *****************************************************************************/
static void scsi_copy32(void *dest, void *source, hcc_u32 length)
{ 
  while(length)
  {
    ((hcc_u32*)dest)[length]=((hcc_u32*)source)[length];
    length--;
  }
}

/******************************************************************************
 scsi_cmd_inquiry

 Description:
   Issue inquiry command on the communication channel, and set LUN status
   to reflect reported values.
 *****************************************************************************/
static int scsi_cmd_inquiry(void)
{
  int err;
  union {
    hcc_u8 cmd[6];
    hcc_u8 resp[36];
  } data;
  
  /* Build SCSI command block */
  data.cmd[0]=SCSI_CMD_INQUIRY;
  data.cmd[1]=0 << 5;   /* EVPD is not supported. */
  data.cmd[2]=0;        /* page code is allways 0 (because EVPD=0) */ 
  data.cmd[3]=0;        /* reserved */
  data.cmd[4]=36;       /* allocationlength is 36 */
  data.cmd[5]=0;        /* Cotrol (flag=0, link=0) */
  /* Start transfer. */
  err=(*transfer)(6, &data.cmd[0], DIR_IN, 36, &data.resp[0]);
  if (err)
  {
    switch(err)
    {
    case ERR_IO_NONE: /* No error */
      break;
    case ERR_IO_CHECK: /* check condition  */
      /* INQUIRY shall only return check condition, if the 
         request failed. */
      break;
    case ERR_IO_REMOVED:  /* This means device is unusable. */
      lun.error_code=LERR_CRITICAL_ERROR;
      return(1);
    case ERR_IO_READ:
      lun.error_code=LERR_IOREAD_ERROR;
      return(1);
    case ERR_IO_WRITE:
      lun.error_code=LERR_IOWRITE_ERROR;
      return(1);
    }
  }
  /* Process returned data. */
    /* We only process the PDT filed, and identification strings. */
  lun.pdt=(char)(data.resp[0] & 0x1f);
  scsi_copy32(lun.vendor, data.resp+8, 8/3);
  scsi_copy32(lun.prod_id, data.resp+16, 16/4);
  scsi_copy32(lun.rev, data.resp+32, 4/4);
  lun.error_code=LERR_NONE;
  return(0);
}

/******************************************************************************
 scsi_cmd_read_capacity

 Description:
   Issue read_capacity request to the specified LUN.
 *****************************************************************************/
static int scsi_cmd_read_capacity()
{
  struct {
    hcc_u8 cmd[10];
    hcc_u8 resp[8];
  } data;
  int err;

  lun.no_of_sectors=0;
  lun.sector_size=0;

  /* Build SCSI command block. */
  data.cmd[0]=SCSI_CMD_READ_CAPACITY;
  data.cmd[1]=0 << 5;        /* No relative addressing. */
  data.cmd[2]=data.cmd[3]=data.cmd[4]=data.cmd[5]=0; /* LBA=0 (because PMI=0) */
  data.cmd[6]=data.cmd[7]=0; /* reserved */
  data.cmd[8]=0;             /* PMI =0 */
  data.cmd[9]=0;             /* Cotrol (flag=0, link=0) */
  /* Send command block. */
  while(1)
  {
    err=(*transfer)(10, &data.cmd[0], DIR_IN, 8, &data.resp[0]);
    if (err)
    {
      switch(err)
      {
      case ERR_IO_NONE: /* No error */
        break;
      case ERR_IO_CHECK: /* Request status from device. */
        /* Get error info from device. */

        if (scsi_cmd_request_sense())
        { /* If request failed, dropp device. */
          return(1);
        }
        host_ms_delay(RETRY_WAIT_MS);        
        continue;
      case ERR_IO_REMOVED:  /* This means device is unusable. */
        lun.error_code=LERR_CRITICAL_ERROR;
        return(1);
      case ERR_IO_READ:
        lun.error_code=LERR_IOREAD_ERROR;
        return(1);
      case ERR_IO_WRITE:
        lun.error_code=LERR_IOWRITE_ERROR;
        return(1);
      }
    }
    break;
  }
  /* Process returned data. */
  lun.no_of_sectors=RD_BE32(data.resp);
  lun.sector_size=RD_BE32(data.resp+4);
  return(0);
}

/******************************************************************************
 scsi_cmd_test_unit_ready

 Description:
   Query the status of the device (see if it is ready to answer the next 
   request). 
 *****************************************************************************/
static int scsi_cmd_test_unit_ready()
{
  hcc_u8 cmd[6];    
  int err;

  /* Build SCSI command block. */
  cmd[0]=SCSI_CMD_TEST_UNIT_READY;
  cmd[1]=0<<5;
  cmd[2]=cmd[3]=cmd[4]=0; /* reserved */
  cmd[5]=0;               /* Cotrol (flag=0, link=0) */
  /* Send command block. */
  err=(*transfer)(6, &cmd[0], DIR_NONE, 0, 0);
  if (err)
  {
    switch(err)
    {
    case ERR_IO_NONE: /* No error */
      break;
    case ERR_IO_CHECK: /* Request status from device. */
      break;
    case ERR_IO_REMOVED:  /* This means device is unusable. */
      lun.error_code=LERR_CRITICAL_ERROR;
      return(1);
    case ERR_IO_READ:
      lun.error_code=LERR_IOREAD_ERROR;
      return(1);
    case ERR_IO_WRITE:
      lun.error_code=LERR_IOWRITE_ERROR;
      return(1);
    }
  }
  return(0);
}

/******************************************************************************
 scsi_cmd_request_sense

 Description:
   Get detailed error information from device. 
 *****************************************************************************/
static int scsi_cmd_request_sense()
{
  union {
    hcc_u8 cmd[6];
    hcc_u8 resp[18];
  } data;
  int err;

  /* Build SCSI command block. */
  data.cmd[0]=SCSI_CMD_REQUEST_SENSE;
  data.cmd[1]=0<<5;
  data.cmd[2]=data.cmd[3]=0; /* reserved */
  data.cmd[4]=18;       /* Allocation length (only read basic info)*/
  data.cmd[5]=0;        /* Cotrol (flag=0, link=0) */
  /* Send command block. */
  err=(*transfer)(6, &data.cmd[0], DIR_IN, 18, &data.resp[0]);
  if (err)
  {
    switch(err)
    {
    case ERR_IO_NONE: /* No error */
      break;
    case ERR_IO_CHECK: /* Check condition shall be returned by the device
                          only if there is a serious error (for example 
                          if the request is misformatted, or the device
                          is in a bad state and can not answer. */
    case ERR_IO_REMOVED:  /* This means device is unusable. */
      lun.error_code=LERR_CRITICAL_ERROR;
      return(1);
    case ERR_IO_READ:
      lun.error_code=LERR_IOREAD_ERROR;
      return(1);
    case ERR_IO_WRITE:
      lun.error_code=LERR_IOWRITE_ERROR;
      return(1);
    }
  }
  /* Process returned data. */
    /* Not sure what error pages are used by the mass-storage. The specification
       is unclear about this (says nothing). So we expect vendor specific current
       errors. (Accept only vendor specific current errors.)*/
  CMX_ASSERT(data.resp[0] == 0x70);

  if (data.resp[0] == 0x70)
  {
    lun.sense_key = data.resp[2];
    lun.sense_data = data.resp[12];
    lun.sense_qualifyer = data.resp[13];
    lun.sense_info = RD_BE32(&data.resp[3]);
  }
  else
  {
    /* Oopss! Unknown report format! :( */
    CMX_ASSERT(0);
  }
  return(0);
}

/******************************************************************************
 scsi_cmd_read_10

 Description:
   Read some blocks to memory. This version can read 65535 blocks.
 *****************************************************************************/
static int scsi_cmd_read_10(hcc_u32 lba, hcc_u16 length, void *buffer)
{
  hcc_u8 cmd[10];
  int err;

  /* Build SCSI command block. */
  cmd[0]=SCSI_CMD_READ_10;
  cmd[1]=0<<5;
  WR_BE32(cmd+2, lba);
  cmd[6]=0; /* reserved */
  WR_BE16(cmd+7, length);
  cmd[9]=0;        /* Cotrol (flag=0, link=0) */
  /* Send command block. */
  while(1)
  {
    err=(*transfer)(10, cmd, DIR_IN, length * lun.sector_size, buffer);
    if (err)
    {
      switch(err)
      {
      case ERR_IO_NONE: /* No error */
        break;
      case ERR_IO_CHECK: 
        scsi_cmd_request_sense();
        host_ms_delay(RETRY_WAIT_MS);
        continue;
      case ERR_IO_REMOVED:  /* This means device is unusable. */
        lun.error_code=LERR_CRITICAL_ERROR;
        return(1);
      case ERR_IO_READ:
        lun.error_code=LERR_IOREAD_ERROR;
        return(1);
      case ERR_IO_WRITE:
        lun.error_code=LERR_IOWRITE_ERROR;
        return(1);
      }
    }
    break;
  }
  return(0);
}

/******************************************************************************
 scsi_cmd_read_12

 Description:
   Read some blocks to memory. This version can read 4294967295 blocks.
 *****************************************************************************/
static int scsi_cmd_read_12(hcc_u32 lba, hcc_u32 length, void *buffer)
{
  hcc_u8 cmd[12];
  int err;


  /* Build SCSI command block. */
  cmd[0]=SCSI_CMD_READ_12;
  cmd[1]=0<<5;
  WR_BE32(cmd+2, lba);
  WR_BE32(cmd+6, length);
  cmd[10]=0;       /* reserved */
  cmd[11]=0;       /* Cotrol (flag=0, link=0) */
  /* Send command block. */
  while(1)
  {
    err=(*transfer)(12, cmd, DIR_IN, length * lun.sector_size, buffer);  
    if (err)
    {
      switch(err)
      {
      case ERR_IO_NONE: /* No error */
        break;
      case ERR_IO_CHECK: 
        scsi_cmd_request_sense();
        host_ms_delay(RETRY_WAIT_MS);
        continue;
      case ERR_IO_REMOVED:  /* This means device is unusable. */
        lun.error_code=LERR_CRITICAL_ERROR;
        return(1);
      case ERR_IO_READ:
        lun.error_code=LERR_IOREAD_ERROR;
        return(1);
      case ERR_IO_WRITE:
        lun.error_code=LERR_IOWRITE_ERROR;
        return(1);
      }
    }
    break;
  }
  return(0);
}

/******************************************************************************
 scsi_read

 Description:
   Read some blocks to memory.
 *****************************************************************************/
int scsi_read(hcc_u32 lba, hcc_u32 length, void *buffer)
{
  int r;
  if (length < 0xffff)
  {
    r=scsi_cmd_read_10(lba, (hcc_u16)length, buffer);
  }
  else
  {
    r=scsi_cmd_read_12(lba, length, buffer);
  }
  return(r);
}

/******************************************************************************
 scsi_cmd_write_10

 Description:
   Write some blocks to media. This version can read 65535 blocks.
 *****************************************************************************/
static int scsi_cmd_write_10(hcc_u32 lba, hcc_u16 length, void *buffer)
{
  hcc_u8 cmd[10];
  int err;

  /* Build SCSI command block. */
  cmd[0]=SCSI_CMD_WRITE_10;
  cmd[1]=0 <<5;
  WR_BE32(cmd+2, lba);
  cmd[6]=0; /* reserved */
  WR_BE16(cmd+7, length);
  cmd[9]=0;        /* Cotrol (flag=0, link=0) */
  /* Send command block. */
  while(1)
  {
    err=(*transfer)(10, cmd, DIR_OUT, length * lun.sector_size, buffer);
    if (err)
    {
      switch(err)
      {
      case ERR_IO_NONE: /* No error */
        break;
      case ERR_IO_CHECK: 
        scsi_cmd_request_sense();
        host_ms_delay(RETRY_WAIT_MS);
        continue;
      case ERR_IO_REMOVED:  /* This means device is unusable. */
        lun.error_code=LERR_CRITICAL_ERROR;
        return(1);
      case ERR_IO_READ:
        lun.error_code=LERR_IOREAD_ERROR;
        return(1);
      case ERR_IO_WRITE:
        lun.error_code=LERR_IOWRITE_ERROR;
        return(1);
      }
    }
    break;
  }
  return(0);
}

/******************************************************************************
 scsi_cmd_write_12

 Description:
   Write some blocks to media. This version can read 4294967295 blocks.
 *****************************************************************************/
static int scsi_cmd_write_12(hcc_u32 lba, hcc_u32 length, void *buffer)
{
  hcc_u8 cmd[12];
  int err;

  /* Build SCSI command block. */
  cmd[0]=SCSI_CMD_WRITE_12;
  cmd[1]=0<<5;
  WR_BE32(cmd+2, lba);
  WR_BE32(cmd+6, length);
  cmd[10]=0;       /* reserved */
  cmd[11]=0;       /* Cotrol (flag=0, link=0) */
  /* Send command block. */
  while(1)
  {
    err=(*transfer)(12, cmd, DIR_OUT, length * lun.sector_size, buffer);
    if (err)
    {
      switch(err)
      {
      case ERR_IO_NONE: /* No error */
        break;
      case ERR_IO_CHECK: 
        scsi_cmd_request_sense();
        host_ms_delay(RETRY_WAIT_MS);
        continue;
      case ERR_IO_REMOVED:  /* This means device is unusable. */
        lun.error_code=LERR_CRITICAL_ERROR;
        return(1);
      case ERR_IO_READ:
        lun.error_code=LERR_IOREAD_ERROR;
        return(1);
      case ERR_IO_WRITE:
        lun.error_code=LERR_IOWRITE_ERROR;
        return(1);
      }
    }
    break;
  }
  return(0);
}

/******************************************************************************
 scsi_write

 Description:
   Write some blocks to media.
 *****************************************************************************/
int scsi_write(hcc_u32 lba, hcc_u32 length, void *buffer)
{
  int r;
  if (length < 0xffff)
  {
    r=scsi_cmd_write_10(lba, (hcc_u16)length, buffer);
  }
  else
  {
    r=scsi_cmd_write_12(lba, length, buffer);
  }
  return(r);
}

/******************************************************************************
 scsi_test_unit_ready

 Description:
   Check device status.
 *****************************************************************************/
int scsi_test_unit_ready(void)
{
  return(scsi_cmd_test_unit_ready());
}

/******************************************************************************
 scsi_get_lun_info

 Description:
   Return structure with LUN specific information.
 *****************************************************************************/
lun_info_t *scsi_get_lun_info(void)
{
  if (transfer == 0)
  {
    return(0);
  }
  return(&lun);
}
/****************************** END OF FILE **********************************/
