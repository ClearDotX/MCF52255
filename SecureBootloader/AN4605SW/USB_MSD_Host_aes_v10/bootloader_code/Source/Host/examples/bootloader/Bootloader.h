/******************************************************************************
 *
 * Freescale Semiconductor Inc.
 * (c) Copyright 2004-2009 Freescale Semiconductor, Inc.
 * ALL RIGHTS RESERVED.
 *
 **************************************************************************//*!
 *
 * @file Bootloader.h
 *
 * @author
 *
 * @version
 *
 * @date 
 *
 * @brief This file is for a USB Mass-Storage Device bootloader.  This file 
 *           has the structures and definitions for the bootloader
 *
 *****************************************************************************/
#ifndef _BOOTLOADER_H_
#define _BOOTLOADER_H_

#include "derivative.h"

/* Define for MM52259 */
#if (defined __MCF52259_H__)
#define MIN_RAM1_ADDRESS        0x20000000
#define MAX_RAM1_ADDRESS        0x2000FFFF
#define MIN_FLASH1_ADDRESS      0x00000000
#define MAX_FLASH1_ADDRESS      0x0007FFFF

#if (defined __DEBUG__)
#define IMAGE_ADDR              ((uint_32_ptr)0x10000)
#else
#define IMAGE_ADDR              ((uint_32_ptr)0xC000)
#endif

#define ERASE_SECTOR_SIZE       (0x1000)  /* 4K bytes*/
/* Define for JM128 */
#elif (defined _MCF51JM128_H)
#define MIN_RAM1_ADDRESS        0x00800000
#define MAX_RAM1_ADDRESS        0x00803FFF
#define MIN_FLASH1_ADDRESS      0x00000000
#define MAX_FLASH1_ADDRESS      0x0001FFFF

#if (defined __DEBUG__)
#define IMAGE_ADDR              ((uint_32_ptr)0x11000)
#else
#define IMAGE_ADDR              ((uint_32_ptr)0xD000)
#endif

#define ERASE_SECTOR_SIZE       (0x0400)  /* 4K bytes*/

/* Define for K60 */
#elif (defined MCU_MK60N512VMD100)
#define MIN_RAM1_ADDRESS        0x1FFF0000
#define MAX_RAM1_ADDRESS        0x20010000
#define MIN_FLASH1_ADDRESS      0x00000000
#define MAX_FLASH1_ADDRESS      0x0007FFFF
#define IMAGE_ADDR              ((uint_32_ptr)0x10000)
#define ERASE_SECTOR_SIZE       (0x800)  /* 2K bytes*/
#endif

//#define S19_RECORD_HEADER       0x53303033
#define S19_RECORD_HEADER_BIG_ENDIAN       0x53300000
#define S19_RECORD_HEADER_LITTLE_ENDIAN    0x00003053
/* DES space less than this is protected */
#define FLASH_PROTECTED_ADDRESS (int)&IMAGE_ADDR[0]
#define FLASH_ADDR_OFFSET       0x44000000
#define First4Bytes             4

/* RAM locations for USB Buffers */
#define USB_BUFFER_START        0x20000400
#define MSD_BUFFER_SIZE         512
#define BDT_SIZE                16
#define ICP_BUFFER_SIZE         64

/* File types */
#define UNKNOWN                 0
#define RAW_BINARY              1
#define CODE_WARRIOR_BINARY     2
#define S19_RECORD              3

void _Entry(void) ;
extern long __SP_INIT;

/* Bootloader Status */
#define BootloaderReady         0
#define BootloaderS19Error      1
#define BootloaderFlashError    2
#define BootloaderSuccess       3
#define BootloaderStarted       4


#define  FLASH_IMAGE_SUCCESS    0
#define  FLASH_IMAGE_ERROR      1

#define BUFFER_LENGTH           (1024)    /* 1K bytes*/


uint_8           FlashApplication(uint_8* arr,uint_32 length);
extern           int _startup(void); 
#endif
/* EOF */
