/*! 
 * \file    memtest.h
 * \brief   General-purpose memory testing functions.
 * \author  Michael Norman
 * \version $Revision: 1.1 $
 *
 * Based entirely on the memory tests written by Michael Barr.  I've 
 * added tests for each bus size so that the size of the device can be
 * specified run-time instead of compile time.  The original copyright 
 * is here:
 * 
 * Copyright (c) 1998 by Michael Barr.  This software is placed into
 * the public domain and may be used for any purpose.  However, this
 * notice must not be changed or removed and no warranty is either
 * expressed or implied by its publication or distribution.
 */

#ifndef _MEMTEST_H_
#define _MEMTEST_H_

/********************************************************************/

/*
 * Function prototypes.
 */

int
walktest_databus_8(uint8 *);

int
walktest_databus_16(uint16 *);

int
walktest_databus_32(uint32 *);

uint8 *
memtest_addrbus_8(uint8 *, int);

uint16 *
memtest_addrbus_16(uint16 *, int);

uint32 *
memtest_addrbus_32(uint32 *, int);

uint8 * 
memtest_device_8(volatile uint8 *, int);

uint16 * 
memtest_device_16(volatile uint16 *, int);

uint32 * 
memtest_device_32(volatile uint32 *, int);

/********************************************************************/

#endif

