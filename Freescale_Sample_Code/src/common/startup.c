/*
 * File:    startup.c
 * Purpose: Generic application startup code
 *
 * Notes:   
 * 
 * License:     All software covered by license agreement in -
 *              docs/Freescale_Software_License.pdf
 */

#include "common.h"

/********************************************************************/
void
common_startup(void)
{
    register uint32 n;
    register uint8 *dp, *sp;

    /* 
     * Copy the vector table to RAM 
     */
    if (__VECTOR_RAM != VECTOR_TABLE)
    {
        for (n = 0; n < 256; n++)
            __VECTOR_RAM[n] = VECTOR_TABLE[n];
    }
    mcf5xxx_wr_vbr((uint32)__VECTOR_RAM);

    /* 
     * Move initialized data from ROM to RAM. 
     */
    if (__DATA_ROM != __DATA_RAM)
    {
        dp = (uint8 *)__DATA_RAM;
        sp = (uint8 *)__DATA_ROM;
        n = __DATA_END - __DATA_RAM;
        while (n--)
            *dp++ = *sp++;
    }
 
    /* 
     * Zero uninitialized data 
     */
    if (__BSS_START != __BSS_END)
    {
        sp = (uint8 *)__BSS_START;
        n = __BSS_END - __BSS_START;
        while (n--)
            *sp++ = 0;
    }
}
/********************************************************************/
