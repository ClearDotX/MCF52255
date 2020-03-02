

#include "common.h"
#include "m52259evb_cpld.h"


#define CS0_START_ADDRESS	(*(vuint8*)(0x80000000))
#define CS1_START_ADDRESS	(*(vuint8*)(0x80080000))
#define MRAM_START_ADDRESS	CS0_START_ADDRESS
#define CPLD_START_ADDRESS	CS1_START_ADDRESS


int8 cpld_8bit_cs1_test (void)
{
	uint32 cs0_address, cs1_address;
	uint8 data8 = 0x00;
	unsigned int n = 0;


	printf("\nMiniBus to CPLD 8-bit non-muxed mode interface\n");
    printf("Requires CPLD to be programmed for 8-bit interface\n");
    printf("Use the \"altera\\cpld\\Minibus Latch 8bit cs1\" project\n");

	
	//initialize MiniBus pins	
	MCF_GPIO_PTEPAR = 0xFF;
	MCF_GPIO_PTFPAR = 0xFF;
	MCF_GPIO_PTGPAR = 0xFF;
	MCF_GPIO_PTHPAR = 0x5555;
	MCF_GPIO_PASPAR = MCF_GPIO_PASPAR_MB_ALE_MB_CS1;        
                

	//initialize chip select base addresses
	cs0_address = 0x80000000;		//for MRAM
	cs1_address = 0x80080000;		//for CPLD
 

	//MRAM
    MCF_FBCS0_CSAR = MCF_FBCS_CSAR_BA(cs0_address);
    MCF_FBCS0_CSCR =    MCF_FBCS_CSCR_WS(1)
    				  |	MCF_FBCS_CSCR_AA
 					  | MCF_FBCS_CSCR_PS_8;	         
    MCF_FBCS0_CSMR =    MCF_FBCS_CSMR_BAM_512K
                      | MCF_FBCS_CSMR_V;


	//CPLD - 8bit non-mux  
	MCF_FBCS1_CSAR = MCF_FBCS_CSAR_BA(cs1_address); 
    MCF_FBCS1_CSCR =    MCF_FBCS_CSCR_WS(1)
    				  |	MCF_FBCS_CSCR_AA
 					  | MCF_FBCS_CSCR_PS_8;     
   
       
    MCF_FBCS1_CSMR =    MCF_FBCS_CSMR_BAM_512K
                      | MCF_FBCS_CSMR_V;



	printf("\nHit any key to begin\n");
  	in_char();
  	  
  	n = 0x00008000;
  	data8=0x00;
  	
  	*(vuint8*)(&CPLD_START_ADDRESS + n) = data8;  
  	printf("WRITE: 0x%08x\n",data8);
  	printf("Now reading back data after an 'OR' with 0x0F\n");
  	
  	data8=0x00;  //clear data8 variable;
  	data8=(*(vuint8*)(&CPLD_START_ADDRESS + n));      
  	printf("READ : 0x%08x\n",data8);

	return 0;
}



int8 cpld_16bit_cs0_test (void)
{
	uint32 cs0_address;
	uint8 data8 = 0x00;
	uint16 data16 = 0x0000;
	unsigned int n = 0;

	
	printf("\n\nMiniBus to CPLD 16-bit muxed mode interface\n");
	printf("Requires CPLD to be programmed for 16-bit interface\n");
    printf("Use the \"altera\\cpld\\Minibus Latch 16bit cs0\" project\n");	

	
	//initialize MiniBus pins	
	MCF_GPIO_PTEPAR = 0xFF;
	MCF_GPIO_PTFPAR = 0xFF;
	MCF_GPIO_PTGPAR = 0xFF;
	MCF_GPIO_PTHPAR = 0x5555;
	MCF_GPIO_PASPAR = MCF_GPIO_PASPAR_MB_ALE_MB_ALE; 

	//initialize chip select base address
	cs0_address = 0x80000000;
   
 
    //16bit mux-mode   
    MCF_FBCS0_CSAR = MCF_FBCS_CSAR_BA(cs0_address);    
    
    MCF_FBCS0_CSCR =    MCF_FBCS_CSCR_WS(1)
    				  | MCF_FBCS_CSCR_MUX	   //muxed mode (split bus mode)
    				  |	MCF_FBCS_CSCR_AA
 					  | MCF_FBCS_CSCR_PS_16;   
  
       
    MCF_FBCS0_CSMR =    MCF_FBCS_CSMR_BAM_512K
                      | MCF_FBCS_CSMR_V;
                      
  	printf("\nRemove MRAM_CS jumper\n");
  	printf("\nHit any key to begin\n\n");
  	in_char();

  	data16=0x1234;
  	(*(vuint16*)(&CS0_START_ADDRESS + n)) = data16;  
  	printf("WRITE 0x%08x: 0x%08x\n",(&CPLD_START_ADDRESS), data16);
  	
  	printf("\nNow reading back data after an 'OR' with 0x0F0F\n");
  	data16=0x0000;  //clear variable;
  	data16=(*(vuint16*)(&CS0_START_ADDRESS + n));      
  	printf("READ  0x%08x: 0x%08x\n", (&CPLD_START_ADDRESS), data16);


	return 0;
}

