/*
 * File:        cfm.c
 * Purpose:     Provide common ColdFire CFM routines for serial IO            
 * 
 * License:     All software covered by license agreement in -
 *              docs/Freescale_Software_License.pdf
 */

#include "common.h"
#include "cfm/cfm.h"




/********************************************************************/
/*
 * Initialize the CFM clock dividor and protection 
 *
 * Parameters:
 *  FlashClk      Flash clock divisor based on passed sys/2 (FlashClk) frequency
 */
volatile int CFM_init(unsigned long FlashClk)
{
	unsigned char fclk_val;
	
	if ((MCF_CFM_CFMCLKD && MCF_CFM_CFMCLKD_DIVLD) != 1)
	{
	    /* Initialize FCLKDIV register to ensure we can program/erase */
		if (FlashClk >= 12000) 
		{
			/* FDIV8 set since above 12MHz clock */
			fclk_val = (unsigned char)(FlashClk/8/200 - 1); 
			MCF_CFM_CFMCLKD |= fclk_val | MCF_CFM_CFMCLKD_PRDIV8;
		}
		else
		{
			fclk_val = (unsigned char)(FlashClk/200 - 1);
			MCF_CFM_CFMCLKD |= fclk_val;
		}
	}

	/* Disable all protection (if LOCK not set)*/
	MCF_CFM_CFMPROT = 0x00; 
	
	/* Clear any errors  */
	MCF_CFM_CFMUSTAT |= (MCF_CFM_CFMUSTAT_PVIOL|MCF_CFM_CFMUSTAT_ACCERR);
	
	return(0);
}

/********************************************************************/
/*
 * Perform a mass erase of all the flash memory. Do not use when 
 *    executing out of the flash
 *
 * Parameters:
 *		None
 */
void CFM_mass_erase ()
{
    volatile unsigned int rdVal = 0;
    rdVal = MCF_CFM_CFMUSTAT; 
    while ((rdVal&(MCF_CFM_CFMUSTAT_CBEIF | MCF_CFM_CFMUSTAT_CCIF)) != (MCF_CFM_CFMUSTAT_CBEIF | MCF_CFM_CFMUSTAT_CCIF))
        rdVal = MCF_CFM_CFMUSTAT;
            
    (*(volatile vuint32 *)(CFM_IPS_FLASH_ADDR)) =  -1;
    MCF_CFM_CFMCMD = MCF_CFM_CFMCMD_MASS_ERASE;
    MCF_CFM_CFMUSTAT = MCF_CFM_CFMUSTAT_CBEIF;

    rdVal = MCF_CFM_CFMUSTAT; 
    while ((rdVal&(MCF_CFM_CFMUSTAT_CBEIF | MCF_CFM_CFMUSTAT_CCIF)) != (MCF_CFM_CFMUSTAT_CBEIF | MCF_CFM_CFMUSTAT_CCIF))
        rdVal = MCF_CFM_CFMUSTAT;
  
}

/********************************************************************/
/*
 * Perform an erase of all the flash memory not being occupied by the program. 
 *
 * Parameters:
 *		None
 */
volatile int CFM_data_erase( void )
{
	unsigned long address;

	for(address = FLASH_DATA_ADDR; address < (uint32)&__FLASH_SIZE; address += FLASH_PAGE_SIZE )
	{
		CFM_page_erase(address);
	}
		
	if (MCF_CFM_CFMUSTAT & MCF_CFM_CFMUSTAT_ACCERR) {return 0;}
	if (MCF_CFM_CFMUSTAT & MCF_CFM_CFMUSTAT_PVIOL)  {return 0;}
	return(1);
}
 

/********************************************************************/
/*
 * Performs a page erase of the flash memory. 
 * Note: assumes that the flash clock dividor and block protection 
 *       has been preset.
 *
 * Parameters:
 *  address     An address within the page to be erased
 *
 */
volatile int CFM_page_erase (unsigned int address)
{   
    /* Allocate space on stack to run flash command out of SRAM */
	char buf[100];
	int (*RunOnStack)(void) = (int(*)(void))buf;   
	memcpy(buf, (void*)SpSub, (char*)SpSubEnd - (char*)SpSub);

	
    (*(volatile vuint32 *)(FLASH_START_ADDRESS+address)) =  -1;
	
	MCF_CFM_CFMCMD = MCF_CFM_CFMCMD_PAGE_ERASE;
	RunOnStack();
	
	/* Check for Errors */
	if (MCF_CFM_CFMUSTAT & MCF_CFM_CFMUSTAT_ACCERR) {return 0;}
	if (MCF_CFM_CFMUSTAT & MCF_CFM_CFMUSTAT_PVIOL)  {return 0;}
	return(1);
	
}

/********************************************************************/
/*
 * Performs a write of the flash memory. 
 * Note: assumes that the flash clock dividor and block protection 
 *       has been preset.
 *
 * Parameters:
 *  address     The location of memory to be programed
 *  data        The data to be programed to memory
 *
 */
volatile int CFM_write (unsigned int address, unsigned int data)
{
	/* Allocate space on stack to run flash command out of SRAM */
	char localbuf[100];
	int (*RunOnStack)(void) = (int(*)(void))localbuf;
	memcpy(localbuf, (void*)SpSub, (char*)SpSubEnd - (char*)SpSub);

	/* Check to make sure long word is aligned */
	if((uint32)address & 0x00000001) {return 0;} 

	/* Clear any errors */
	MCF_CFM_CFMUSTAT = (MCF_CFM_CFMUSTAT_PVIOL | MCF_CFM_CFMUSTAT_ACCERR); 
	
	/* Wait until buffer is free */
	while (!(MCF_CFM_CFMUSTAT & MCF_CFM_CFMUSTAT_CBEIF)){};
	
 	(*(volatile vuint32 *)(FLASH_START_ADDRESS+address)) =  data;
	
	MCF_CFM_CFMCMD = MCF_CFM_CFMCMD_WORD_PROGRAM;
	RunOnStack();
	
	/* Check for Errors */
	if (MCF_CFM_CFMUSTAT & MCF_CFM_CFMUSTAT_ACCERR) {return 0;}
	if (MCF_CFM_CFMUSTAT & MCF_CFM_CFMUSTAT_PVIOL)  {return 0;}
	return 1;

}
/********************************************************************/
/*
 * Perform a blank check on the flash memory
 * Note: assumes that the flash clock dividor and block protection 
 *       has been preset.
 *
 * Parameters:
 *
 * Returns:
 *   true   if the flash memory is completely erased
 *   false  otherwie
 */
int CFM_blank_check ()
{
    volatile unsigned int rdVal = 0;
    
	/* Allocate space on stack to run flash command out of SRAM */
	char localbuf[100];
	int (*RunOnStack)(void) = (int(*)(void))localbuf;
	memcpy(localbuf, (void*)SpSub, (char*)SpSubEnd - (char*)SpSub);

    rdVal = MCF_CFM_CFMUSTAT; 
    while ((rdVal&(MCF_CFM_CFMUSTAT_CBEIF | MCF_CFM_CFMUSTAT_CCIF)) != (MCF_CFM_CFMUSTAT_CBEIF | MCF_CFM_CFMUSTAT_CCIF))
        rdVal = MCF_CFM_CFMUSTAT;
            
    (*(volatile vuint32 *)(CFM_IPS_FLASH_ADDR)) =  -1;
    
    MCF_CFM_CFMCMD = MCF_CFM_CFMCMD_BLANK_CHECK;
	RunOnStack();
    
    /* Check to see if flash erased */
    if ((MCF_CFM_CFMUSTAT) & MCF_CFM_CFMUSTAT_BLANK)
    {
        MCF_CFM_CFMUSTAT = (MCF_CFM_CFMUSTAT) & (MCF_CFM_CFMUSTAT_BLANK);
        return 1;
    }
    else
    {
        return 0;
    }
}

/********************************************************************/
/*
 * Verifies that a page was erased
 * Note: assumes that the flash clock divider and block protection 
 *       has been preset.
 *
 * Parameters:
 *  address     The location of memory to be verified
 * Returns:
 *   1   if the flash memory is completely erased
 *   0 	 otherwie
 */
int CFM_page_erase_verify (unsigned int address)
{
    volatile unsigned int rdVal = 0;
    
    /* Allocate space on stack to run flash command out of SRAM */
	char localbuf[100];
	int (*RunOnStack)(void) = (int(*)(void))localbuf;
	memcpy(localbuf, (void*)SpSub, (char*)SpSubEnd - (char*)SpSub);
    
    /* Check to make sure long word is aligned */
	if((uint32)address & 0x00000001) {return 0;} 
    
    rdVal = MCF_CFM_CFMUSTAT; 
    while ((rdVal&(MCF_CFM_CFMUSTAT_CBEIF | MCF_CFM_CFMUSTAT_CCIF)) != (MCF_CFM_CFMUSTAT_CBEIF | MCF_CFM_CFMUSTAT_CCIF))
        rdVal = MCF_CFM_CFMUSTAT;
            
    (*(volatile vuint32 *)(CFM_IPS_FLASH_ADDR+address)) =  -1;
    
    MCF_CFM_CFMCMD = MCF_CFM_CFMCMD_PAGE_ERASE_VERIFY;
 	RunOnStack();
    
    /* Checks to see if page erased */
    if ((MCF_CFM_CFMUSTAT) & MCF_CFM_CFMUSTAT_BLANK)
    {
        MCF_CFM_CFMUSTAT = (MCF_CFM_CFMUSTAT) & (MCF_CFM_CFMUSTAT_BLANK);
        return 1;
    }
    else
    {
        return 0;
    }
}

/********************************************************************/
/*
 *	Code required to run in SRAM to perform flash commands. 
 *  All else can be run in flash.
 *
 */   
void SpSub(void) 
{
	asm(
		//	mcf5xxx_irq_disable();
		move.w  SR,D7       	/* current sr    */
		move.l  D7,D5       	/* save return value  */
		move.w  #0x0700,D6      /* set level to 7    */
		andi.l  #0x0000F8FF,D7  /* zero out current IPL  */
		or.l    D6,D7           /*    */
		move.w  D7,SR           /* place new IPL in sr   */

		//	MCF_CFM_CFMUSTAT = CBEIF;
		move.l   #MCF_CFM_CFMUSTAT,a4  
		move.b   #0x80,d1     
		move.b   d1,(a4)

		//	while (!(MCF_CFM_CFMUSTAT & CCIF)){};//wait until execution complete
		loop:
		move.l   #MCF_CFM_CFMUSTAT,a4  
		move.b   (a4),d2
		moveq    #0,d1
		move.b   d2,d1
		andi.l   #0x40,d1     
		beq.s    loop

		//	mcf5xxx_irq_enable();
		move.w  D5,SR
	);
}
/* Leave this immediately after SpSub */
void SpSubEnd(void){} 

/********************************************************************/