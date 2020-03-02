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
#include "timer.h"
#include "mcf5222x_reg.h"
#include "hcc_types.h"

/* The OTG and USB host driver needs a mS timer. This is implemented using 
   PIT0 */
#define P0CLK    40000000ul
#define P0PRES_VAL       12u

void start_mS_timer(hcc_u16 delay)
{ 
  hcc_u32 val=delay*((P0CLK/1000)/(1<<P0PRES_VAL));
  
  CMX_ASSERT(val == (hcc_u16)val);
  
  MCF_PIT0_PCSR = MCF_PIT_PCSR_OVW | MCF_PIT_PCSR_PIF;
  
  MCF_PIT0_PMR = (hcc_u16)val;

  MCF_PIT0_PCSR = MCF_PIT_PCSR_PRE(P0PRES_VAL) | MCF_PIT_PCSR_EN | MCF_PIT_PCSR_OVW | MCF_PIT_PCSR_PIF;
}

hcc_u8 check_mS_timer(void)
{
  if (!(MCF_PIT0_PCSR & MCF_PIT_PCSR_PIF))
  {
    return(0);
  }
  
  MCF_PIT0_PCSR = MCF_PIT_PCSR_PIF;
  return(1);
}
/****************************** END OF FILE **********************************/
