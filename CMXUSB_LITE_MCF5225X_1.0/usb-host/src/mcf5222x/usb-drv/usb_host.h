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
#ifndef _USB_HOST_H_
#define _USB_HOST_H_

#include "hcc_types.h"

/* Transaction types. */
#define TRT_NONE  0u
#define TRT_IN    1u
#define TRT_OUT   2u
#define TRT_SETUP 3u

#define INVALID_ADDRESS  ((hcc_u8)-1u)
#define RETVAL_ERROR  ((hcc_u16) -1u)
#define DEVICE_IS_INVALID(dt, dev)  ((dt)[(dev)].address == INVALID_ADDRESS)

/* Access common properties of USB descriptors. */ 
#define USBDSC_LENGTH(p)  (((hcc_u8*)(p))[0])
#define USBDSC_TYPE(p)    (((hcc_u8*)(p))[1])

/* Read device descriptor members. */
#define DEVDESC_BCDUSB(p)      RD_LE16(((hcc_u8*)(p))+2)
#define DEVDESC_CLASS(p)       (((hcc_u8*)(p))[4])
#define DEVDESC_SCLASS(p)      (((hcc_u8*)(p))[5])
#define DEVDESC_PROTOCOL(p)    (((hcc_u8*)(p))[6])
#define DEVDESC_PACKET_SIZE(p) (((hcc_u8*)(p))[7])
#define DEVDESC_VID(p)         RD_LE16(((hcc_u8*)(p))+8)
#define DEVDESC_PID(p)         RD_LE16(((hcc_u8*)(p))+10)
#define DEVDESC_REV(p)         RD_LE16(((hcc_u8*)(p))+12)
#define DEVDESC_MANUFACT(p)   (((hcc_u8*)(p))[14))
#define DEVDESC_PRODUCT(p)    (((hcc_u8*)(p))[15])
#define DEVDESC_SERNO(p)      (((hcc_u8*)(p))[16])
#define DEVDESC_NCFG(p)       (((hcc_u8*)(p))[17])

/* Read configuration descriptor members. */
#define CONFDESC_TOTLENGTH(p) RD_LE16(((hcc_u8*)(p))+2)
#define CONFDESC_INTRFACES(p) (((hcc_u8*)(p))[4])
#define CONFDESC_MY_NDX(p)    (((hcc_u8*)(p))[5])
#define CONFDESC_MY_STR(p)    (((hcc_u8*)(p))[6])
#define CONFDESC_ATTRIB(p)    (((hcc_u8*)(p))[7])
#define CONFDESC_MAX_POW(p)   (((hcc_u8*)(p))[8])

/* Read interface descriptor members. */
#define IFCDESC_MY_NDX(p)     (((hcc_u8*)(p))[2])
#define IFCDESC_ALTERNATE(p)  (((hcc_u8*)(p))[3])
#define IFCDESC_ENDPONTS(p)   (((hcc_u8*)(p))[4])
#define IFCDESC_CLASS(p)      (((hcc_u8*)(p))[5])
#define IFCDESC_SCLASS(p)     (((hcc_u8*)(p))[6])
#define IFCDESC_PROTOCOL(p)   (((hcc_u8*)(p))[7])
#define IFCDESC_MY_STR(p)     (((hcc_u8*)(p))[8])

#define EPDESC_ADDRESS(p)     (((hcc_u8*)(p))[2])
#define EPDESC_ATTRIB(p)      (((hcc_u8*)(p))[3])
#define EPDESC_PSIZE(p)       RD_LE16(((hcc_u8*)(p))+4)
#define EPDESC_INTERVAL(p)    (((hcc_u8*)(p))[6])

#define EPTYPE_INVALID        ((hcc_u8)-1u)
#define EPTYPE_CTRL           ((hcc_u8)0u)
#define EPTYPE_ISO            ((hcc_u8)1u)
#define EPTYPE_BULK           ((hcc_u8)2u)
#define EPTYPE_INT            ((hcc_u8)3u)


/* Driver returns information extracted from device descriptors using this
   type. */
typedef struct {
  hcc_u16 vid;
  hcc_u16 pid;
  hcc_u16 rev;
  hcc_u8  clas;
  hcc_u8  sclas;
  hcc_u8  protocol;
  hcc_u8  ncfg;
} device_info_t;

/* Driver returns information extracted from interface descriptors using this
   type. */
typedef struct {
  hcc_u8 clas;
  hcc_u8 sclas;
  hcc_u8 protocol;
  hcc_u8 ndx;
  hcc_u8 alt_set;
  hcc_u8 str;
  hcc_u8 nep; 
} ifc_info_t;

/* Driver returns information extracted from configuration descriptors using
   this type. */
typedef struct {
  hcc_u8 nifc;
  hcc_u8 ndx;
  hcc_u8 str;
  hcc_u8 attrib;
  hcc_u8 max_power; 
} cfg_info_t;

/* Driver returns information extracted from endpoint descriptors using this
   type. */
typedef struct {
  hcc_u16 psize;
  hcc_u8 address;
  hcc_u8 type;
  hcc_u8 interval;
} ep_info_t;

/* Error codes. */
typedef enum {
  tre_none=0,
  tre_not_running,  /* host operation disabled */
  tre_no_device,    /* no device connected */
  tre_disconnected, /* device has been disconnected */
  tre_stall,        /* endpoint halted by device*/
  tre_data_error,   /* communicatin error (CRC, etc...)*/
  tre_silent,       /* device returned no handshake at all */
  tre_no_ep         /* endpoint not configured */
} tr_error_t;

extern tr_error_t tr_error;
/* Driver call this. Shall enable pull down resistors. */
extern void enable_usb_pull_downs(void);
/* Shall be called before any other call. Initializes driver and hardware. */
extern void host_init(void);
/* Stop host functionality. */
extern void host_stop(void);
/* Send USB reset to connected device. */
extern void host_reset_bus(void);
/* Send data to the device on the control channel. */
extern hcc_u16 host_send_control(hcc_u8 *setup_data, hcc_u8* buffer, hcc_u8 ep);
/* Get data from device on the control channel. */
extern hcc_u16 host_receive_control(hcc_u8 *setup_data, hcc_u8* buffer, hcc_u8 ep);
/* Send data to the device on non control channels. */
extern hcc_u32 host_send(hcc_u8* buffer, hcc_u32 length, hcc_u8 ep);
/* Get data from device on non control channels. */
extern hcc_u32 host_receive(hcc_u8* buffer, hcc_u32 length, hcc_u8 ep);

/* Define an endpoint for the device. Driver above the host driver shall call
   it when selecting a configuration. */
extern hcc_u8 host_add_ep(hcc_u8 type, hcc_u8 address, hcc_u8 interval, hcc_u16 psize);
/* Remove an endpoint for the device. Driver above the host driver shall call
   it when selecting a configuration. */
extern void host_remove_ep(hcc_u8 ep_handle);
/* Modifi an endpoint of the device. Driver above the host driver may call
   it when selecting a configuration. */
extern void host_modify_ep(hcc_u8 ep_handle, hcc_u8 type, hcc_u8 address, hcc_u8 interval, hcc_u16 psize);
/* This function will wait the specifyed number of mS. */
extern void host_ms_delay(hcc_u32);
/* Check if a device is connected, and do basic configuration. */
extern int host_scan_for_device(void);
/* Return non0 if a device is connected. Use to see if a the device is still 
   connected.*/
extern hcc_u8 host_has_device(void);

extern void host_sleep(void);
extern void host_wakeup(void);
#endif
/****************************** END OF FILE **********************************/
