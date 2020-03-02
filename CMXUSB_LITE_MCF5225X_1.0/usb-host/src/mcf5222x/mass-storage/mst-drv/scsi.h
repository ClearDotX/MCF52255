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

#ifndef _SCSI_H_
#define _SCSI_H_

#include "hcc_types.h"
/* User visible error codes for logical units. */
  /* No error*/
#define LERR_NONE            0x0u
  /* LUN does not exist. */
#define LERR_INVALID_DEVICE  0x1u
  /* LUN is not responsive. */
#define LERR_CRITICAL_ERROR  0x2u
  /* Device IO write failed. */
#define LERR_IOWRITE_ERROR   0x3u
  /* Device IO read failed. */
#define LERR_IOREAD_ERROR    0x4u

/* Error codes returned by the lower layer communication driver. */
#define ERR_IO_NONE     0
#define ERR_IO_CHECK    1
#define ERR_IO_REMOVED  2
#define ERR_IO_READ     3
#define ERR_IO_WRITE    4

/* Transfer directions. */
#define DIR_NONE 2
#define DIR_IN   1
#define DIR_OUT  0

/* Structure to store LUN (logical unit) information. */
typedef struct {
  char pdt;
  char vendor[8];
  char prod_id[16];
  char rev[4];
  hcc_u32 no_of_sectors;
  hcc_u32 sector_size;
  int error_code;
  hcc_u8 sense_key;
  hcc_u8 sense_data;
  hcc_u8 sense_qualifyer;
  hcc_u32 sense_info;
} lun_info_t;

typedef int rw_func_t(hcc_u8 cmd_length, void *cmd, hcc_u8 dir, hcc_u32 buf_length, void *buf);

extern int scsi_init(void);
extern int scsi_open_device(rw_func_t *f);
extern int scsi_close_device(void);
extern int scsi_read(hcc_u32 lba, hcc_u32 length, void *buffer);
extern int scsi_write(hcc_u32 lba, hcc_u32 length, void *buffer);
extern int scsi_test_unit_ready(void);
extern int scsi_reset_lun(void);
extern lun_info_t *scsi_get_lun_info(void);
#endif
/****************************** END OF FILE **********************************/
