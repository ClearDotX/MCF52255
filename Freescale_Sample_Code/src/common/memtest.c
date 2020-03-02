/*! 
 * \file    memtest.c
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

#include "common.h"
#include "memtest.h"

/********************************************************************/
/*!
 * \brief   8-bit memory data bus test
 * \param   addr Address at which to perform the data bus test
 * \return  0 for success, non-zero for failure
 *
 * Test the 8-bit data bus wiring in a memory region by performing a walking 
 * 1's test at a fixed address within that region.  The address (and hence 
 * the memory region) is selected by the caller.
 */
int
walktest_databus_8(uint8 *addr)
{
    uint8 pattern;
    // uint32 addr;

    /* Perform a walking 1's test at the given address */
    
  //  for (addr = 0x20000000; addr < 0x20001000; addr++)
    {
    	for (pattern = 1; pattern != 0; pattern <<= 1)
    	{
        	/* Write the test pattern */
        	*addr = pattern;

        	/* Read it back (immediately is okay for this test) */
        	if (*addr != pattern) 
            	return (pattern);
    	}
    }
    return 0;
}
/********************************************************************/
/*!
 * \brief   16-bit memory data bus test
 * \param   addr Address at which to perform the data bus test
 * \return  0 for success, non-zero for failure
 *
 * Test the 16-bit data bus wiring in a memory region by performing a walking 
 * 1's test at a fixed address within that region.  The address (and hence 
 * the memory region) is selected by the caller.
 */
int
walktest_databus_16(uint16 *addr)
{
    uint16 pattern;

    /* Perform a walking 1's test at the given address */
   // for (addr = 0x20000000; addr < 0x20001000; addr++)
    {
	    for (pattern = 1; pattern != 0; pattern <<= 1)
	    {
    	    /* Write the test pattern */
        	*(uint16*)addr = pattern;

        	/* Read it back (immediately is okay for this test) */
        	if (*(uint16*)addr != pattern) 
            	return (pattern);
    	}
    }
    return 0;
}
/********************************************************************/
/*!
 * \brief   32-bit memory data bus test
 * \param   addr Address at which to perform the data bus test
 * \return  0 for success, non-zero for failure
 *
 * Test the 32-bit data bus wiring in a memory region by performing a walking 
 * 1's test at a fixed address within that region.  The address (and hence 
 * the memory region) is selected by the caller.
 */
int
walktest_databus_32(uint32 *addr)
{
    uint32 pattern;

    /* Perform a walking 1's test at the given address */
   // for (addr = 0x20000000; addr < 0x20001000; addr++)
    {
        for (pattern = 1; pattern != 0; pattern <<= 1)
    	{
        	/* Write the test pattern */
        	*(uint32*)addr = pattern;

        	/* Read it back (immediately is okay for this test) */
        	if (*(uint32*)addr != pattern) 
           	 return (pattern);
    	}
    }
    return 0;
}

/********************************************************************/
/*!
 * \brief   8-bit memory address bus test
 * \param   baseaddr Address at which to start the test
 *          bytes    Number of bytes to test
 * \return  0 for success, non-zero for failure.  A non-zero result is 
 *          the first address at which an aliasing problme was discovered.
 * 
 * Test the address bus wiring in a memory region by performing a walking
 * 1's test on the relevant bits of the address and checking for aliasing. 
 * This test will find single-bit address failures such as stuck-high, 
 * stuck-low, and shorted pins.  The base address and size of the region 
 * are selected by the caller.  
 *
 * For best results, the selected base address should have enough LSB 0's 
 * to guarantee single address bit changes.  For example, to test a 
 * 64-Kbyte region, select a base address on a 64-Kbyte boundary.  
 * Also, select the region size as a power-of-two--if at all possible.
 */
uint8 *
memtest_addrbus_8(uint8 *baseaddr, int bytes)
{
    unsigned long addressMask = (bytes - 1);
    unsigned long offset;
    unsigned long testOffset;

    uint8 pattern     = 0xAA;
    uint8 antipattern = 0x55;


    /* Write the default pattern at each of the power-of-two offsets */
    for (offset = 1; (offset & addressMask) != 0; offset <<= 1)
    {
        baseaddr[offset] = pattern;
    }

    /* Check for address bits stuck high */
    testOffset = 0;
    baseaddr[testOffset] = antipattern;

    for (offset = 1; (offset & addressMask) != 0; offset <<= 1)
    {
        if (baseaddr[offset] != pattern)
        {
            return ((uint8*) &baseaddr[offset]);
        }
    }

    baseaddr[testOffset] = pattern;

    /* Check for address bits stuck low or shorted */
    for (testOffset = 1; (testOffset & addressMask) != 0; testOffset <<= 1)
    {
        baseaddr[testOffset] = antipattern;

		if (baseaddr[0] != pattern)
		{
			return ((uint8 *) &baseaddr[testOffset]);
		}

        for (offset = 1; (offset & addressMask) != 0; offset <<= 1)
        {
            if ((baseaddr[offset] != pattern) && (offset != testOffset))
            {
                return ((uint8 *) &baseaddr[testOffset]);
            }
        }

        baseaddr[testOffset] = pattern;
    }

    return 0;
}
/********************************************************************/
/*!
 * \brief   16-bit memory address bus test
 * \param   baseaddr Address at which to start the test
 *          bytes    Number of bytes to test
 * \return  0 for success, non-zero for failure.  A non-zero result is 
 *          the first address at which an aliasing problme was discovered.
 * 
 * Test the address bus wiring in a memory region by performing a walking
 * 1's test on the relevant bits of the address and checking for aliasing. 
 * This test will find single-bit address failures such as stuck-high, 
 * stuck-low, and shorted pins.  The base address and size of the region 
 * are selected by the caller.  
 *
 * For best results, the selected base address should have enough LSB 0's 
 * to guarantee single address bit changes.  For example, to test a 
 * 64-Kbyte region, select a base address on a 64-Kbyte boundary.  
 * Also, select the region size as a power-of-two--if at all possible.
 */
uint16 *
memtest_addrbus_16(uint16 *baseaddr, int bytes)
{
    unsigned long addressMask = (bytes/2 - 1);
    unsigned long offset;
    unsigned long testOffset;

    uint16 pattern     = 0xAAAA;
    uint16 antipattern = 0x5555;


    /* Write the default pattern at each of the power-of-two offsets */
    for (offset = 1; (offset & addressMask) != 0; offset <<= 1)
    {
        baseaddr[offset] = pattern;
    }

    /* Check for address bits stuck high */
    testOffset = 0;
    baseaddr[testOffset] = antipattern;

    for (offset = 1; (offset & addressMask) != 0; offset <<= 1)
    {
        if (baseaddr[offset] != pattern)
        {
            return ((uint16*) &baseaddr[offset]);
        }
    }

    baseaddr[testOffset] = pattern;

    /* Check for address bits stuck low or shorted */
    for (testOffset = 1; (testOffset & addressMask) != 0; testOffset <<= 1)
    {
        baseaddr[testOffset] = antipattern;

		if (baseaddr[0] != pattern)
		{
			return ((uint16 *) &baseaddr[testOffset]);
		}

        for (offset = 1; (offset & addressMask) != 0; offset <<= 1)
        {
            if ((baseaddr[offset] != pattern) && (offset != testOffset))
            {
                return ((uint16 *) &baseaddr[testOffset]);
            }
        }

        baseaddr[testOffset] = pattern;
    }

    return 0;
}
/********************************************************************/
/*!
 * \brief   32-bit memory address bus test
 * \param   baseaddr Address at which to start the test
 *          bytes    Number of bytes to test
 * \return  0 for success, non-zero for failure.  A non-zero result is 
 *          the first address at which an aliasing problme was discovered.
 * 
 * Test the address bus wiring in a memory region by performing a walking
 * 1's test on the relevant bits of the address and checking for aliasing. 
 * This test will find single-bit address failures such as stuck-high, 
 * stuck-low, and shorted pins.  The base address and size of the region 
 * are selected by the caller.  
 *
 * For best results, the selected base address should have enough LSB 0's 
 * to guarantee single address bit changes.  For example, to test a 
 * 64-Kbyte region, select a base address on a 64-Kbyte boundary.  
 * Also, select the region size as a power-of-two--if at all possible.
 */
uint32 *
memtest_addrbus_32(uint32 *baseaddr, int bytes)
{
    unsigned long addressMask = (bytes/4 - 1);
    unsigned long offset;
    unsigned long testOffset;

    uint32 pattern     = 0xAAAAAAAA;
    uint32 antipattern = 0x55555555;


    /* Write the default pattern at each of the power-of-two offsets */
    for (offset = 1; (offset & addressMask) != 0; offset <<= 1)
    {
        baseaddr[offset] = pattern;
    }

    /* Check for address bits stuck high */
    testOffset = 0;
    baseaddr[testOffset] = antipattern;

    for (offset = 1; (offset & addressMask) != 0; offset <<= 1)
    {
        if (baseaddr[offset] != pattern)
        {
            return ((uint32 *) &baseaddr[offset]);
        }
    }

    baseaddr[testOffset] = pattern;

    /* Check for address bits stuck low or shorted */
    for (testOffset = 1; (testOffset & addressMask) != 0; testOffset <<= 1)
    {
        baseaddr[testOffset] = antipattern;

		if (baseaddr[0] != pattern)
		{
			return ((uint32 *) &baseaddr[testOffset]);
		}

        for (offset = 1; (offset & addressMask) != 0; offset <<= 1)
        {
            if ((baseaddr[offset] != pattern) && (offset != testOffset))
            {
                return ((uint32 *) &baseaddr[testOffset]);
            }
        }

        baseaddr[testOffset] = pattern;
    }

    return 0;
}
/********************************************************************/
/*!
 * \brief   8-bit memory device test
 * \param   baseaddr Address at which to start the test
 *          bytes    Number of bytes to test
 * \return  0 for success, non-zero for failure.  A non-zero result is 
 *          the first address at which an incorrect value is read back.
 * 
 * Test the integrity of a physical memory device by performing an 
 * increment/decrement test over the entire region.  In the process every 
 * storage bit  in the device is tested as a zero and a one.  The base 
 * address and the size of the region are selected by the caller.
 */
uint8 * 
memtest_device_8(volatile uint8 *baseaddr, int bytes)	
{
    unsigned long offset;
    unsigned long nWords = bytes / sizeof(uint8);

    uint8 pattern;
    uint8 antipattern;


    /* Fill memory with a known pattern */
    for (pattern = 1, offset = 0; offset < nWords; pattern++, offset++)
    {
        baseaddr[offset] = pattern;
    }

    /* Check each location and invert it for the second pass */
    for (pattern = 1, offset = 0; offset < nWords; pattern++, offset++)
    {
        if (baseaddr[offset] != pattern)
        {
            return ((uint8 *) &baseaddr[offset]);
        }

        antipattern = ~pattern;
        baseaddr[offset] = antipattern;
    }

    /* Check each location for the inverted pattern and zero it */
    for (pattern = 1, offset = 0; offset < nWords; pattern++, offset++)
    {
        antipattern = ~pattern;
        if (baseaddr[offset] != antipattern)
        {
            return ((uint8 *) &baseaddr[offset]);
        }
    }

    return 0;
}
/********************************************************************/
/*!
 * \brief   16-bit memory device test
 * \param   baseaddr Address at which to start the test
 *          bytes    Number of bytes to test
 * \return  0 for success, non-zero for failure.  A non-zero result is 
 *          the first address at which an incorrect value is read back.
 * 
 * Test the integrity of a physical memory device by performing an 
 * increment/decrement test over the entire region.  In the process every 
 * storage bit  in the device is tested as a zero and a one.  The base 
 * address and the size of the region are selected by the caller.
 */
uint16 * 
memtest_device_16(volatile uint16 *baseaddr, int bytes)	
{
    unsigned long offset;
    unsigned long nWords = bytes / sizeof(uint16);

    uint16 pattern;
    uint16 antipattern;


    /* Fill memory with a known pattern */
    for (pattern = 1, offset = 0; offset < nWords; pattern++, offset++)
    {
        baseaddr[offset] = pattern;
    }

    /* Check each location and invert it for the second pass */
    for (pattern = 1, offset = 0; offset < nWords; pattern++, offset++)
    {
        if (baseaddr[offset] != pattern)
        {
            return ((uint16 *) &baseaddr[offset]);
        }

        antipattern = ~pattern;
        baseaddr[offset] = antipattern;
    }

    /* Check each location for the inverted pattern and zero it */
    for (pattern = 1, offset = 0; offset < nWords; pattern++, offset++)
    {
        antipattern = ~pattern;
        if (baseaddr[offset] != antipattern)
        {
            return ((uint16 *) &baseaddr[offset]);
        }
    }

    return 0;
}
/********************************************************************/
/*!
 * \brief   32-bit memory device test
 * \param   baseaddr Address at which to start the test
 *          bytes    Number of bytes to test
 * \return  0 for success, non-zero for failure.  A non-zero result is 
 *          the first address at which an incorrect value is read back.
 * 
 * Test the integrity of a physical memory device by performing an 
 * increment/decrement test over the entire region.  In the process every 
 * storage bit  in the device is tested as a zero and a one.  The base 
 * address and the size of the region are selected by the caller.
 */
uint32 * 
memtest_device_32(volatile uint32 *baseaddr, int bytes)	
{
    unsigned long offset;
    unsigned long nWords = bytes / sizeof(uint32);

    uint32 pattern;
    uint32 antipattern;


    /* Fill memory with a known pattern */
    for (pattern = 1, offset = 0; offset < nWords; pattern++, offset++)
    {
        baseaddr[offset] = pattern;
    }

    /* Check each location and invert it for the second pass */
    for (pattern = 1, offset = 0; offset < nWords; pattern++, offset++)
    {
        if (baseaddr[offset] != pattern)
        {
            return ((uint32 *) &baseaddr[offset]);
        }

        antipattern = ~pattern;
        baseaddr[offset] = antipattern;
    }

    /* Check each location for the inverted pattern and zero it */
    for (pattern = 1, offset = 0; offset < nWords; pattern++, offset++)
    {
        antipattern = ~pattern;
        if (baseaddr[offset] != antipattern)
        {
            return ((uint32 *) &baseaddr[offset]);
        }
    }
    
    return 0;
}
/********************************************************************/
