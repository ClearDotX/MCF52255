/*
 * File:		cfm.h
 * Purpose:     Provide common ColdFire Flash Module routines.
 * 
 * License:     All software covered by license agreement in -
 *              docs/Freescale_Software_License.pdf
 */

#ifndef __CFM_H__
#define __CFM_H__

/* Last Address of execution code is where data begins */
#define FLASH_DATA_ADDR ((vuint32)&__DATA_ROM[0x1000] & 0x0FFFF000)

/* address in memory map to write in FLASH */
#define FLASH_START_ADDRESS	 CFM_IPS_FLASH_ADDR

/* End address for region of flash */
#define FLASH_END_ADDRESS					(vuint32)&__FLASH_SIZE+0x44000000

/* Flash erase page size */
#define FLASH_PAGE_SIZE						0x0800

#define CFM_IPS_FLASH_ADDR 					0x44000000

/********************************************************************/

volatile int CFM_init(unsigned long);

void CFM_mass_erase ();
volatile int CFM_data_erase();
volatile int CFM_page_erase (unsigned int);
volatile int CFM_write (unsigned int, unsigned int);

int CFM_blank_check ();
int CFM_page_erase_verify (unsigned int);

void SpSub(void); 
void SpSubEnd(void);

/********************************************************************/

#endif /* __CFM_H__ */
