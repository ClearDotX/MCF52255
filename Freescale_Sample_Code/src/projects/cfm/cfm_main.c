/*
 * File:		cfm_main.c
 * Purpose:		Coldfire Flash Module Examples
 *
 * License:     All software covered by license agreement in -
 *              docs/Freescale_Software_License.pdf
 */
#include "common.h"
#include "cfm/cfm.h"

/********************************************************************/
void 
main (void)
{

	int i,n;
	char res;

    /* Determine if execution is from Flash or SRAM */
    char running_in_sram = (__DATA_ROM == __DATA_RAM) ? TRUE : FALSE;

   printf("\nID %x",((MCF_CCM_CIR & 0xFFC0) >> 6));
   printf(" Rev %d",(MCF_CCM_CIR & 0x003F));

    if(running_in_sram)
    {
    	printf("\nCFM Example on SRAM\n");	
		mcf5xxx_irq_enable();	
    
		/* Configure flash to read and write */
		printf("\nInitialize Flash\n");
		CFM_init(SYS_CLK_KHZ/2);
		
		
    	/* perform a mass erase */
		printf("Perform a mass erase\n");
		CFM_mass_erase();
		
		/* perform a blank check */
    	printf ("Perform a blank check\n");
		if (!CFM_blank_check())
		{
	  	  printf ("ERROR: Blank check after mass erase failed\n");
		}
	
		/* Write data from 0 to entire flash */
    	printf ("Writing data to all flash\n");
    	for(n=0;n<(uint32)&__FLASH_SIZE;n=n+4)
			res=CFM_write (n, 0x01);

		/* perform a blank check */
    	printf ("Perform a blank check\n");
		if (CFM_blank_check())
		{
	  	  printf ("ERROR: Blank Check should return false\n");
		}		    
    
        /* perform a mass erase */
		printf("Perform a mass erase\n");
		CFM_mass_erase();
    
    	/* perform a write */
    	printf("Perform a write to 0x58\n");
    	CFM_write(0x58,0xabcdefab);
    	
    	/* perform a write */
    	printf("Perform a write to 0x1110\n");
    	CFM_write(0x1110,0x12345678);
    	
    	
    	/* perform a page erase check */
    	printf ("Perform a page erase check on page 1\n");
		if (CFM_page_erase_verify(0x7F0))
		{
		    printf ("ERROR: Page Erase check after program passed\n");
		}
		
		/* Erase page 1 */
   		printf ("Perform a page erase on page 1\n");
    	CFM_page_erase(0x7F0);
    	
    	/* perform a page erase check */
    	printf ("Perform a page erase check on page 1\n");
		if (!CFM_page_erase_verify(0x600))
		{
		    printf ("ERROR: Page 1 Should Be Erased\n");
		}
		
    	/* perform a page erase check */
    	printf ("Perform a page erase check on page 2\n");
		if (CFM_page_erase_verify(0x1700))
		{
		    printf ("ERROR: Page Should Not Be Erased\n");
		}		
		
        /* perform a mass erase */
		printf("Perform a mass erase\n");
		CFM_mass_erase();
    
    }
	else
	{
		
		printf("\nCFM Example on Flash\n");	
		mcf5xxx_irq_enable();	
    
		/* Configure flash to read and write */
		printf("\nInitialize Flash\n");
		CFM_init(SYS_CLK_MHZ*1000/2);

    	/* Erase flash not being occupied to program currently in flash */
		printf("Erasing Available flash\n");
		printf("From 0x%08X ",FLASH_DATA_ADDR);
		printf("To 0x%08x \n\n",(uint32)&__FLASH_SIZE);
		res = CFM_data_erase ();

		printf("Data erasing Complete\n");


		/* Write data from FLASH_DATA_ADDRESS to 0x3FFFF */
    	printf ("Writing data to available flash\n");
    	for(n=FLASH_DATA_ADDR;n<(uint32)&__FLASH_SIZE;n=n+4)
			res=CFM_write (n, 0x11223344);
	
		/* perform a page erase check */
    	printf ("Perform a page erase check at 0x4000\n");
		if (CFM_page_erase_verify(0x4000))
		{
		    printf ("ERROR: Page should not be erased\n");
		}
	
	
		/* perform a data erase */
   	    printf ("Erasing available flash\n");
		res = CFM_data_erase ();
	
		/* perform a blank check */
    	printf ("Perform a blank check\n");
		if (CFM_blank_check())
		{
	 	   printf ("ERROR: Flash should not be blanked\n");
		}
	
		/* perform a page erase check */
    	printf ("Perform a page erase check at 0x4000\n");
		if (!CFM_page_erase_verify(0x4000))
		{
		    printf ("ERROR: Page should be erased\n");
		}
	
		/* program some data on a page */
    	printf ("Program data at 0x4700\n");
		CFM_write (0x4700, 0x12345678);
	
		/* program some data on a page */
		printf ("Program data at 0x5900\n");
		CFM_write (0x5900, 0xabcdefab);

		/* perform a page erase check */
    	printf ("Perform a page erase check on 0x4000 page\n");
		if (CFM_page_erase_verify(0x4750))
		{
		    printf ("ERROR: Page Erase check after program passed\n");
		}
	
		/* Erase the previously written page at 4700 */
   		printf ("Perform a page erase on 0x4000 page\n");
    	CFM_page_erase(0x4550);
    
		/* perform a page erase check */
    	printf ("Perform a page erase check on 0x4000 page\n");
		if (!CFM_page_erase_verify(0x4650))
		{
		    printf ("ERROR: Should have seen page erased\n");
		}
    
		/* perform a page erase check */
    	printf ("Perform a page erase check on 0x5000 page\n");
		if (CFM_page_erase_verify(0x5A00))
		{
		    printf ("ERROR: Should not have seen page erased\n");
		}
	
	}
	


	
	printf("*********End of CFM Example*********\n");
}

/********************************************************************/
