/********************************************************************/
/* Coldfire C Header File
 *
 *     Date      : 2009/04/30
 *     Revision  : 0.8
 *
 *     Copyright : 1997 - 2009 Freescale Semiconductor, Inc. All Rights Reserved.
 *
 *     http      : www.freescale.com
 *     mail      : support@freescale.com
 */

#ifndef __MCF52255_CCM_H__
#define __MCF52255_CCM_H__


/*********************************************************************
*
* Chip Configuration Module (CCM)
*
*********************************************************************/

/* Register read/write macros */
#define MCF_CCM_CCR                          (*(vuint16*)(0x40110004))
#define MCF_CCM_RCON                         (*(vuint16*)(0x40110008))
#define MCF_CCM_CIR                          (*(vuint16*)(0x4011000A))
#define MCF_CCM_CCE                          (*(vuint16*)(0x40110010))


/* Bit definitions and macros for MCF_CCM_CCR */
#define MCF_CCM_CCR_Mode(x)                  (((x)&0x7)<<0x8)
#define MCF_CCM_CCR_MODE_SINGLECHIP          (0x600)
#define MCF_CCM_CCR_MODE_EZPORT              (0x500)
#define MCF_CCM_CCR_LOAD                     (0x8000)

/* Bit definitions and macros for MCF_CCM_RCON */
#define MCF_CCM_RCON_RLOAD                   (0x20)

/* Bit definitions and macros for MCF_CCM_CIR */
#define MCF_CCM_CIR_PRN(x)                   (((x)&0x3F)<<0)
#define MCF_CCM_CIR_PIN(x)                   (((x)&0x3FF)<<0x6)

/* Bit definitions and macros for MCF_CCM_CCE */
#define MCF_CCM_CCE_USBEND                   (0x4000)
#define MCF_CCM_CCE_MBMOD                    (0x8000)


#endif /* __MCF52255_CCM_H__ */
