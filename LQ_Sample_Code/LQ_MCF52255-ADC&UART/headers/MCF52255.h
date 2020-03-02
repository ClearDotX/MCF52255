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

#ifndef __MCF52255_H__
#define __MCF52255_H__


/********************************************************************/
/*
 * The basic data types
 */

typedef unsigned char           uint8;   /*  8 bits */
typedef unsigned short int      uint16;  /* 16 bits */
typedef unsigned long int       uint32;  /* 32 bits */

typedef signed char             int8;    /*  8 bits */
typedef signed short int        int16;   /* 16 bits */
typedef signed long int         int32;   /* 32 bits */

typedef volatile uint8          vuint8;  /*  8 bits */
typedef volatile uint16         vuint16; /* 16 bits */
typedef volatile uint32         vuint32; /* 32 bits */

#ifdef __cplusplus
extern "C" {
#endif

#pragma define_section system ".system" far_absolute RW

/***
 * MCF52255 Derivative Memory map definitions from linker command files:
 * __IPSBAR, __RAMBAR, __RAMBAR_SIZE, __FLASHBAR, __FLASHBAR_SIZE linker
 * symbols must be defined in the linker command file.
 */

extern __declspec(system)  uint8 __IPSBAR[];
extern __declspec(system)  uint8 __RAMBAR[];
extern __declspec(system)  uint8 __RAMBAR_SIZE[];
extern __declspec(system)  uint8 __FLASHBAR[];
extern __declspec(system)  uint8 __FLASHBAR_SIZE[];

#define IPSBAR_ADDRESS   (uint32)__IPSBAR
#define RAMBAR_ADDRESS   (uint32)__RAMBAR
#define RAMBAR_SIZE      (uint32)__RAMBAR_SIZE
#define FLASHBAR_ADDRESS (uint32)__FLASHBAR
#define FLASHBAR_SIZE    (uint32)__FLASHBAR_SIZE


#include "MCF52255_SCM.h"
#include "MCF52255_DMA.h"
#include "MCF52255_UART.h"
#include "MCF52255_I2C.h"
#include "MCF52255_QSPI.h"
#include "MCF52255_DTIM.h"
#include "MCF52255_INTC.h"
#include "MCF52255_FEC.h"
#include "MCF52255_GPIO.h"
#include "MCF52255_PAD.h"
#include "MCF52255_RCM.h"
#include "MCF52255_CCM.h"
#include "MCF52255_PMM.h"
#include "MCF52255_CLOCK.h"
#include "MCF52255_EPORT.h"
#include "MCF52255_BWT.h"
#include "MCF52255_PIT.h"
#include "MCF52255_FlexCAN.h"
#include "MCF52255_CANMB.h"
#include "MCF52255_RTC.h"
#include "MCF52255_ADC.h"
#include "MCF52255_GPT.h"
#include "MCF52255_PWM.h"
#include "MCF52255_USB_OTG.h"
#include "MCF52255_CFM.h"
#include "MCF52255_RNGA.h"

#ifdef __cplusplus
}
#endif


#endif /* __MCF52255_H__ */
