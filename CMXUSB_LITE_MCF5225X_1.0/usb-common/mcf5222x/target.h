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
#ifndef _TARGET_H_
#define _TARGET_H_

#include "hcc_types.h"
#include "mcf5222x_reg.h"

extern void _irq_restore (hcc_imask ip);
extern hcc_imask _irq_disable (void);
extern void hw_init(void);
extern void Usb_Vbus_On(void);
extern void Usb_Vbus_Off(void);
extern int Usb_Vbus_Stat(void);

#define SW1_ACTIVE()  (((MCF_EPORT_EPPDR0 & BIT5) == 0)||((MCF_EPORT_EPPDR0 & BIT7) == 0))
#define SW2_ACTIVE()  (((MCF_EPORT_EPPDR0 & BIT1) == 0)||((MCF_EPORT_EPPDR0 & BIT2) == 0))

#define LED1_ON       (MCF_GPIO_PORTTC |= 1<<0)
#define LED2_ON       (MCF_GPIO_PORTTC |= 1<<1)
#define LED3_ON       (MCF_GPIO_PORTTC |= 1<<2)
#define LED4_ON       (MCF_GPIO_PORTTC |= 1<<3)

#define LED1_OFF      (MCF_GPIO_PORTTC &= ~(1<<0))
#define LED2_OFF      (MCF_GPIO_PORTTC &= ~(1<<1))
#define LED3_OFF      (MCF_GPIO_PORTTC &= ~(1<<2))
#define LED4_OFF      (MCF_GPIO_PORTTC &= ~(1<<3))

#define LED1_TGL      (MCF_GPIO_PORTTC & (1<<0) ? LED1_OFF : LED1_ON)
#define LED2_TGL      (MCF_GPIO_PORTTC & (1<<1) ? LED2_OFF : LED2_ON)
#define LED3_TGL      (MCF_GPIO_PORTTC & (1<<2) ? LED3_OFF : LED3_ON)
#define LED4_TGL      (MCF_GPIO_PORTTC & (1<<3) ? LED4_OFF : LED4_ON)
#endif
/****************************** END OF FILE **********************************/
