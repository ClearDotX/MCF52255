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
#ifndef _OTG_LOW_H_
#define _OTG_LOW_H_

#include "otg.h"
#include "hcc_types.h"

typedef enum{
  otgvs_off,
  otgvs_on,
  otgvs_charge,
  otgvs_discharge
} otgvbus_state_t;

typedef enum {
  otgp_off,
  otgp_down,
  otgp_up_p,
  otgp_up_m
} otgpull_t;

typedef enum {
  otglv_sess_ivld,  /* invalid level */
  otgvl_sess_end,   /* below B session end level (0.8V). */  
  otgvl_sess_vld,   /* above A session valid level (0.8V). */
  otglv_vbus_chrg,  /* above A charge min (2.1 V). */
  otgvl_vld         /* above A valid level (4.4V). */
} otgvlevel_t;

/* Inicialize low-level driver. */
extern hcc_u8 otgdrv_init(void);
extern hcc_u8 otgdrv_init_a(void);
extern hcc_u8 otgdrv_init_b(void);

/* Set VBUS state (on, off, charge). */
extern void otgdrv_set_vbus(otgvbus_state_t st);
/* Return the current level of the VBUS. */
extern otgvlevel_t otgdrv_get_vbus_level(void);
/* Return the status of the id signal. */
extern otg_role_t otgdrv_get_id(void);
/* Return true if a device is connected to the host. */
extern hcc_u8 otgdrv_get_bconn(void);
/* select which pull resistor to activate .*/
extern void otgdrv_set_pull(otgpull_t pt);

/* return one if reset signaling is active */
extern hcc_u8 otgdrv_got_SE0(void);

/* return one if bus is suspended */
extern hcc_u8 otgdrv_get_suspend(void);

/* start SRP */
extern hcc_u8 otgdrv_req_session(void);

#endif
/****************************** END OF FILE **********************************/
