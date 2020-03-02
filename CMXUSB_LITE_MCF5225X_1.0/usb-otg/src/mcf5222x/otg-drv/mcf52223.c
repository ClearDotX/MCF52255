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
#include "mcf5222x_reg.h"

void mcf5222x_init_b(void);
void mcf5222x_init_a(void);
void mcf5222x_stop_a(void);
void mcf5222x_stop_b(void);


void mcf5222x_init_b(void)
{/*Enable reset detection. */
  /* Disable all USB interrupts. */
  MCF_USB_INT_ENB = 0x0;
  /* Disable all OTG interupts. */
  MCF_USB_OTG_INT_EN = 0x0;
    /* Clear all pending events. */
  MCF_USB_INT_STAT = 0xff; 
    /* select USB clock source */
  MCF_USB_USB_CTRL = MCF_USB_USB_CTRL_CLKSRC_OSC;
  /* set port QS for USB functions  */
  MCF_GPIO_PQSPAR |= MCF_GPIO_PQSPAR_PQSPAR5(3) | MCF_GPIO_PQSPAR_PQSPAR6(3);

    /* Enable device operation mode. */
  MCF_USB_CTL = MCF_USB_CTL_USB_EN_SOF_EN;
}

void mcf5222x_init_a(void)
{
  /* Enable attach, and sleep detection in the HC. */
  /* Disable all USB interrupts. */
  MCF_USB_INT_ENB = 0x0;
  /* Disable all OTG interupts. */
  MCF_USB_OTG_INT_EN = 0x0;
    /* Clear all pending events. */
  MCF_USB_INT_STAT = 0xff; 
    /* select USB clock source */
  MCF_USB_USB_CTRL = MCF_USB_USB_CTRL_CLKSRC_OSC;

  /* set port QS for USB functions  */
  MCF_GPIO_PQSPAR |= MCF_GPIO_PQSPAR_PQSPAR5(3) | MCF_GPIO_PQSPAR_PQSPAR6(3);
    /* Enable host operation mode. */
  MCF_USB_CTL = MCF_USB_CTL_HOST_MODE_EN;  
}

void mcf5222x_stop_a(void)
{
  /* Enable attach, and sleep detection in the HC. */

    /* Clear all pending events. */
  MCF_USB_INT_STAT = 0xff; 
    /* Disable host operation mode. */
  MCF_USB_CTL = 0;
}

void mcf5222x_stop_b(void)
{
  /* Enable attach, and sleep detection in the HC. */

    /* Clear all pending events. */
  MCF_USB_INT_STAT = 0xff; 
    /* Disable host operation mode. */
  MCF_USB_CTL = 0;
}

hcc_u8 otgdrv_get_bconn(void)
{
  int x;

  /* If we can not clear the attach flag, then a device is connected. */
  MCF_USB_INT_STAT = MCF_USB_INT_STAT_ATTACH;

  /* Some delay is needed between clearing the ATTACH flag, and checking
     it again. Unfortunately there is no information about the length of 
     the delay. */  
  for(x=0; x<10000; x++)
    ;
  return((hcc_u8)(MCF_USB_INT_STAT & MCF_USB_INT_STAT_ATTACH ? 1 : 0));
}

hcc_u8 otgdrv_get_suspend(void)
{
  static hcc_u8 sleeping=0;
  
  if (sleeping)
  {
    /* If in device mode wakeup if SOF_TOK or RESUME has been set.*/
    if ((((MCF_USB_CTL & MCF_USB_CTL_HOST_MODE_EN) == 0)
        && ((MCF_USB_INT_STAT & MCF_USB_INT_STAT_RESUME) 
         || (MCF_USB_INT_STAT & MCF_USB_INT_STAT_SOF_TOK)))
    /* In host mode wakeup if RESUME has been set. */     
    ||(((MCF_USB_CTL & MCF_USB_CTL_HOST_MODE_EN) != 0)
            && MCF_USB_INT_STAT & MCF_USB_INT_STAT_RESUME))
    {
      sleeping=0;
      MCF_USB_INT_STAT=MCF_USB_INT_STAT_SLEEP;
    }
  }
  else
  {
    /* In device mode STAT_SLEEP signals suspend. */
    if ((((MCF_USB_CTL & MCF_USB_CTL_HOST_MODE_EN) == 0)
             && MCF_USB_INT_STAT & MCF_USB_INT_STAT_SLEEP)
    /* In host ode STAT_SLEEP does not seems to work. As a workaround
       we use !RESUME as a suspend event signal.
       If SOF_EN is set, bus can not bee suspended.  */
       || (((MCF_USB_CTL & (MCF_USB_CTL_HOST_MODE_EN | MCF_USB_CTL_USB_EN_SOF_EN)) 
                  == MCF_USB_CTL_HOST_MODE_EN)
             && ((MCF_USB_INT_STAT & MCF_USB_INT_STAT_RESUME)==0)))
    {
      sleeping=1;
      /* For sokme unknown reason these flags can not be cleared for a while.
         The loop solves this. */
      while(MCF_USB_INT_STAT & (MCF_USB_INT_STAT_RESUME|MCF_USB_INT_STAT_SOF_TOK))
      {        
        MCF_USB_INT_STAT=MCF_USB_INT_STAT_RESUME|MCF_USB_INT_STAT_SOF_TOK;
      }
    }
  }
   
  return(sleeping);
}

hcc_u8 otgdrv_got_SE0(void)
{
  hcc_u8 r=(hcc_u8)(MCF_USB_INT_STAT & MCF_USB_INT_STAT_USB_RST ? 1 : 0);
  if (r)
  {
    MCF_USB_INT_STAT = MCF_USB_INT_STAT_USB_RST;
  }
  return(r);
}
/****************************** END OF FILE **********************************/
