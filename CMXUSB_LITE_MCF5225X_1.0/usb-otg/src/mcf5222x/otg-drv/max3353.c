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
#include "otg-low.h"
#include "i2c-drv/i2c.h"
#include "timer-drv/timer.h"


#define MAX3353_I2C_ADDR  0x58

#define MAN_REG0    0x0u
#define MAN_REG1    0x1u
#define MAN_REG2    0x2u
#define MAN_REG3    0x3u

#define PROD_ID0    0x4u
#define PROD_ID1    0x5u
#define PROD_ID2    0x6u
#define PROD_ID3    0x7u

#define CTRL_REG1   0x10
#define CTRL_REG1_IRQ_MODE      (1u<<1)
#define CTRL_REG1_BDISC_ACONN   (1u<<2)
#define CTRL_REG1_DP_PULLUP     (1u<<4)
#define CTRL_REG1_DM_PULLUP     (1u<<5)
#define CTRL_REG1_DP_PULLDWN    (1u<<6)
#define CTRL_REG1_DM_PULLDWN    (1u<<7)

#define CTRL_REG2   0x11
#define CTRL_REG2_SDWN          (1u<<0)
#define CTRL_REG2_VBUS_CHG1     (1u<<1)
#define CTRL_REG2_VBUS_CHG2     (1u<<2)
#define CTRL_REG2_VBUS_DRV      (1u<<3)
#define CTRL_REG2_VBUS_DISCHG   (1u<<4)

#define STAT_REG    0x13
#define STAT_REG_VBUS_VALID     (1u<<0)
#define STAT_REG_SESSION_VALID  (1u<<1)
#define STAT_REG_SESSION_END    (1u<<2)
#define STAT_REG_ID_GND         (1u<<3)
#define STAT_REG_ID_FLOAT       (1u<<4)
#define STAT_REG_A_HNP          (1u<<5)
#define STAT_REG_B_HNP          (1u<<6)

#define IMASK_REG   0x14
#define IEDGE_REG   0x15
#define ILATCH_REG  0x16h


extern void mcf5222x_init_b(void);
extern void mcf5222x_init_a(void);
extern void mcf5222x_stop_a(void);
extern void mcf5222x_stop_b(void);

static hcc_u8 max3353_init(void);

static hcc_u8 max3353_init(void)
{
  hcc_u32 man_id;
  hcc_u32 prod_id;

  /* Slave address will newer be used. */
  i2c_init(1);
  /* MAX 3353E i2c gan go up to 400KHz. */
  if (i2c_set_bps(400000))
  {
    /* Failed to set baudrate! */
    CMX_ASSERT(0);
  }
  
  /* Read and check chip manufacturer and product ID bytes. */
  man_id = (hcc_u32)(i2c_receive_byte(MAN_REG3, MAX3353_I2C_ADDR)<<24);
  man_id |= (hcc_u32)(i2c_receive_byte(MAN_REG2, MAX3353_I2C_ADDR)<<16);
  man_id |= (hcc_u32)(i2c_receive_byte(MAN_REG1, MAX3353_I2C_ADDR)<<8);
  man_id |= (hcc_u32)i2c_receive_byte(MAN_REG0, MAX3353_I2C_ADDR);
  
  prod_id = (hcc_u32)(i2c_receive_byte(PROD_ID3, MAX3353_I2C_ADDR)<<24);
  prod_id |= (hcc_u32)(i2c_receive_byte(PROD_ID2, MAX3353_I2C_ADDR)<<16);
  prod_id |= (hcc_u32)(i2c_receive_byte(PROD_ID1, MAX3353_I2C_ADDR)<<8);
  prod_id |= (hcc_u32)i2c_receive_byte(PROD_ID0, MAX3353_I2C_ADDR);
  
  /* UPPS: some maxim chips return a product id that differs from the 
     documented value. As a workaround the MSB is not compared. */
/*  if (man_id != 0x33530b6aul || prod_id != 0x01425a48ul)*/     
  prod_id &= 0x00ffffff;
  if (man_id != 0x33530b6aul || prod_id != 0x00425a48ul)
  {
    /* Invalid chip identificated. */
    return(1);
  }
  /* Disconnect discharge resistor, stop charging VBUS, stop charing VBUS 
     for 105mS, put part into operating state (exit suspend). */
  i2c_send_byte(0x00, CTRL_REG2, MAX3353_I2C_ADDR);
  
  /* Disable pull-up and pull-down resistors, disable autoconnect, 
     set IRQ mode to open drain */
  i2c_send_byte(0x00, CTRL_REG1, MAX3353_I2C_ADDR);
  /* Disable all interrupts. */
  i2c_send_byte(0x00, IMASK_REG, MAX3353_I2C_ADDR);
  return(0);
}

hcc_u8 otgdrv_init(void)
{
  return(max3353_init());
}

hcc_u8 otgdrv_init_b(void)
{ 
  mcf5222x_init_b();
  return(0);
}

/* Inicialize low-level driver. */
hcc_u8 otgdrv_init_a(void)
{
  mcf5222x_init_a();
  return(0);
}

/* Start requesting a session. */
hcc_u8 otgdrv_req_session(void)
{
  /* Ensure BUS is turned off for at lease 5 mS before starting 
     SRP. */
  start_mS_timer(5);
  do {
    if (otgvl_sess_end != otgdrv_get_vbus_level())
    {
      return(1);
    }
  } while(!check_mS_timer());

  /* First do data line pulsing, and then go for VBUS pulsing. */
  otgdrv_set_vbus(otgvs_charge);
  i2c_send_byte(CTRL_REG2_VBUS_CHG1, CTRL_REG2, MAX3353_I2C_ADDR);
  /* Wait till chip finishes VBUS chargeing. ~100mS */
  start_mS_timer(101);
  while(!check_mS_timer())
    ;
  otgdrv_set_vbus(otgvs_off);
  
  /* Now host has 5-10 seconds to start up. */    
  start_mS_timer(6000);
  while(!check_mS_timer() && otgvl_vld != otgdrv_get_vbus_level())
    ;
    
  /* Check if host ackowledged us */
  if (otgvl_vld != otgdrv_get_vbus_level())
  {
    /* If not, start data pulsing. */
    otgdrv_set_pull(otgp_up_p);
    /* Wait 5-10 mS. */
    start_mS_timer(7);
    while(!check_mS_timer())
      ;
    /* Stop data pulsing. */
    otgdrv_set_pull(otgp_off);

    start_mS_timer(110);
    while(!check_mS_timer())
      ;            
    /* Now host has 5-10 seconds to start up. */    
    start_mS_timer(6000);
    while(!check_mS_timer() && !otgdrv_got_SE0())
      ;
    if (!otgdrv_got_SE0())
    {
      return(1);  
    }
  }
  return(0);
}

/* Return the status of the id signal. */
otg_role_t otgdrv_get_id(void)
{
  hcc_u8 tmp=i2c_receive_byte(STAT_REG, MAX3353_I2C_ADDR);
  if (tmp & STAT_REG_ID_FLOAT)
  {
    return(otgr_b);
  }
  
  if (tmp & STAT_REG_ID_GND)
  {
    return(otgr_a);
  }
 
  CMX_ASSERT(0);
  return(otgr_a);
}

void otgdrv_set_vbus(otgvbus_state_t st)
{
  switch(st)
  {
  case otgvs_off:
    i2c_send_byte(0, CTRL_REG2, MAX3353_I2C_ADDR);  
    break;
  case otgvs_on:
    i2c_send_byte(CTRL_REG2_VBUS_DRV, CTRL_REG2, MAX3353_I2C_ADDR);
    break;
  case otgvs_charge:
    i2c_send_byte(CTRL_REG2_VBUS_CHG1, CTRL_REG2, MAX3353_I2C_ADDR);
    break;
  case otgvs_discharge:
    i2c_send_byte(CTRL_REG2_VBUS_DISCHG, CTRL_REG2, MAX3353_I2C_ADDR);
    break;
  }
}

otgvlevel_t otgdrv_get_vbus_level(void)
{
  /* Unfortunately MAX3353 has no comparator for 2.1 V. Thus otglv_vbus_chrg
     can not be returned. */
  hcc_u8 tmp=i2c_receive_byte(STAT_REG, MAX3353_I2C_ADDR);
  if (tmp & STAT_REG_VBUS_VALID)
  {/* VBUS is above 4.6 V */
    return(otgvl_vld);
  }
  if (tmp & STAT_REG_SESSION_VALID)
  {/* VBUS is above 1.4 V */
    return(otgvl_sess_vld);
  }
  if (tmp & STAT_REG_SESSION_END)
  {/* VBUS is below 0.5 V */
    return(otgvl_sess_end);
  }
  /* VBUS is 1.4 and 0.5 V */
  return(otglv_sess_ivld);
}

void otgdrv_set_pull(otgpull_t pt)
{
  switch(pt)
  {
  case otgp_off:
    i2c_send_byte(0, CTRL_REG1, MAX3353_I2C_ADDR);      
    break;
  case otgp_down:
    i2c_send_byte(CTRL_REG1_DP_PULLDWN | CTRL_REG1_DM_PULLDWN, CTRL_REG1, MAX3353_I2C_ADDR);
    break;
  case otgp_up_p:
    i2c_send_byte(CTRL_REG1_DP_PULLUP, CTRL_REG1, MAX3353_I2C_ADDR);  
    break;
  case otgp_up_m:
    i2c_send_byte(CTRL_REG1_DM_PULLUP, CTRL_REG1, MAX3353_I2C_ADDR);  
    break;
  }
}
/****************************** END OF FILE **********************************/
