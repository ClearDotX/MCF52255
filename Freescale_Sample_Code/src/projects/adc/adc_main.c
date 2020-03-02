/*
 * File:		adc_main.c
 * Purpose:		ADC Example
 *
 * License:     All software covered by license agreement in -
 *              docs/Freescale_Software_License.pdf
 */

#include "common.h"
#include "uif.h"


/********************************************************************/
/*
 * Setup user interface
 */
 

/* Defining command prompt characters as ADC */
const char PROMPT[] = "ADC> ";
    
/* Defining the different commands */
void cmd_offset (int, char **);
void cmd_dump (int, char **);
void cmd_disable (int, char **);
void cmd_average (int, char **);
void cmd_start (int, char **);
void cmd_stop (int, char **);
void cmd_sleep (int, char **);
void cmd_zero (int, char **);
void cmd_adlst1 (int, char **);
void cmd_adlst2 (int, char **);
void cmd_adllmt (int, char **);
void cmd_adhlmt (int, char **);

/* Filling the command table */
UIF_CMD UIF_CMDTAB[] =
{
    UIF_CMDS_ALL
    {"offset",0,2,0,cmd_offset,"Channel & Offset Amount","<ch> <offset>"},
    {"dump",0,0,0,cmd_dump,"Display ADC registers",""},
    {"disable",0,1,0,cmd_disable,"Disable register(s) 0-7","<ch>"},
    {"average",0,0,0,cmd_average,"Average x Conversions"},
    {"start",0,0,0,cmd_start,"Start Conversions"},
    {"stop",0,0,0,cmd_stop,"Stop conversions"},
    {"sleep",0,0,0,cmd_sleep,"Sleep conversion test"},
    {"zero",0,3,0,cmd_zero,"Zero Cross Enable","<ch> <offset>"},
    {"adlst1",0,3,0,cmd_adlst1,"Channel List 1 Register","<ch> <sample>"},
    {"adlst2",0,3,0,cmd_adlst2,"Channel List 2 Register","<ch> <sample>"},
    {"adllmt",0,3,0,cmd_adllmt,"Low Limit Register","<ch> <offset>"},
    {"adhlmt",0,3,0,cmd_adhlmt,"High Limit Register","<ch> <offset>"},

};

/* Defining the different set/show commands */
void setshow_chncfg (int, char **);
void setshow_smode (int, char **);

/* Filling the set/show command table */
UIF_SETCMD UIF_SETCMDTAB[2];
UIF_SETCMD UIF_SETCMDTAB[] =
{
  {"chncfg", 0,1,setshow_chncfg,"set/show, <chncfg> <mode>"},
  {"smode", 0,1,setshow_smode,"set/show, <smode> <mode>"},
};

const int UIF_NUM_CMD    = UIF_CMDTAB_SIZE;
const int UIF_NUM_SETCMD = UIF_SETCMDTAB_SIZE;

int sys_clk_khz = SYS_CLK_KHZ;
int running_in_sram;
int baud = TERMINAL_BAUD;

int running = 0;

#define PLL_DISABLE   (0x0001)



__declspec(interrupt) void ADCA_EOS(){
	
	MCF_ADC_ADSTAT |= MCF_ADC_ADSTAT_EOSI0;   
    
	printf("ADC A - End of Scan Interrupt");
 
}

__declspec(interrupt) void ADCB_EOS(){

	MCF_ADC_ADSTAT |= MCF_ADC_ADSTAT_EOSI1;  //clear interrupt 

	printf("ADC B - End of Scan Interrupt");
 
}

__declspec(interrupt) void ADC_INT(){


	if (MCF_ADC_ADSTAT & MCF_ADC_ADSTAT_HLMTI)
	{
		printf("ADC High Limit Interrupt\n");
		MCF_ADC_ADSTAT |= MCF_ADC_ADSTAT_HLMTI;  //clear interrupt 
			
	}
	if (MCF_ADC_ADSTAT & MCF_ADC_ADSTAT_LLMTI)
	{
		printf("ADC Low Limit Interrupt\n");
		MCF_ADC_ADSTAT |= MCF_ADC_ADSTAT_LLMTI;  //clear interrupt 
			
	}
	if (MCF_ADC_ADSTAT & MCF_ADC_ADSTAT_ZCI)
	{
		printf("ADC Zero Crossing Interrupt\n");
		MCF_ADC_ADSTAT |= MCF_ADC_ADSTAT_ZCI ;  //clear interrupt 
			
	}
       
}


/********************************************************************/
void 
main (void)
{

	mcf5xxx_irq_enable();
	
	printf("\n");
    printf("**************************************************\n");
    printf("*                                                *\n");
    printf("*                  ADC Utility                   *\n");
    printf("*                                                *\n");
    printf("**************************************************\n");
    printf(HELPMSG);
	printf("\n");
	
		
	/* Initialize the CRTL1 register to all 0's */
    MCF_ADC_CTRL1 = 0;  
    
    /* Setting divisor in CTRL2 register */
    MCF_ADC_CTRL2 = MCF_ADC_CTRL2_DIV(3);
    
    /* Setting PortAN as ADC functionality */
    MCF_GPIO_PANPAR |= MCF_GPIO_PANPAR_PANPAR0
  					| MCF_GPIO_PANPAR_PANPAR1
  					| MCF_GPIO_PANPAR_PANPAR2
   					| MCF_GPIO_PANPAR_PANPAR3
  					| MCF_GPIO_PANPAR_PANPAR4
   					| MCF_GPIO_PANPAR_PANPAR5
   					| MCF_GPIO_PANPAR_PANPAR6
   					| MCF_GPIO_PANPAR_PANPAR7;
    
    
    /* Setting PortQS for ADC SYNC functionality */
    MCF_GPIO_PQSPAR |= MCF_GPIO_PQSPAR_QSPI_CS2_SYNCB
  					|  MCF_GPIO_PQSPAR_QSPI_CS3_SYNCA;

   					
    
    /* Setting Power Register appropriately */
    MCF_ADC_POWER = MCF_ADC_POWER_PUDELAY(4);


   /* Enable ADC Interrupts */

    MCF_INTC0_ICR49 = MCF_INTC_ICR_IL(2) | MCF_INTC_ICR_IP(1);
    MCF_INTC0_ICR50 = MCF_INTC_ICR_IL(4) | MCF_INTC_ICR_IP(3);
    MCF_INTC0_ICR51 = MCF_INTC_ICR_IL(5) | MCF_INTC_ICR_IP(5);

    MCF_INTC0_IMRH &= ~(MCF_INTC_IMRH_INT_MASK49|MCF_INTC_IMRH_INT_MASK50| 
                       MCF_INTC_IMRH_INT_MASK51|MCF_INTC_IMRL_MASKALL);
     

    /* Command Prompt */
    while (TRUE)
    {

        printf(PROMPT);
        run_cmd();
    
    }
    
}
/*****************************************************************************/
	/* Interrupt handler sequence for Sleep Command */
	
__interrupt__ 
void
dtim0_handler(void)
{
    /* Reset the system clock */
    clock_lpd(1);
    clock_pll(REF_CLK_KHZ, sys_clk_khz, 0);
    
    /* Turn on UART0 clock */
    MCF_SCM_PPMRH = MCF_SCM_PPMRH & ~MCF_SCM_PPMRL_CDUART0;	

    /* Turn on ADC clock */
    MCF_SCM_PPMRH = MCF_SCM_PPMRH &~ MCF_SCM_PPMRH_CDADC;	
    
    /* Stop conversions after doing first conversion */
    MCF_ADC_ADSDIS = MCF_ADC_ADSDIS_DS1;
    			
    /* Clear stop bit */
    MCF_ADC_CTRL1 &= ~MCF_ADC_CTRL1_STOP0; 					
        
    /* Set start bit */		
	MCF_ADC_CTRL1 |= MCF_ADC_CTRL1_START0;
	
	/* Waiting for Ready bits to be set */					
	while (!(MCF_ADC_ADSTAT & MCF_ADC_ADSTAT_RDY0))			
	{};

	/* Set stop bit	*/					
	MCF_ADC_CTRL1 |= MCF_ADC_CTRL1_STOP0;
	
	/* Shutting off ADC clock */
	MCF_SCM_PPMRH |= MCF_SCM_PPMRH_CDADC;
						
	/* Diplay the ADC result register for the conversion */
    printf("\tADRSLT0  = %04X\n",  (MCF_ADC_ADRSLT0));		
 	
    /* Wait for the UART to finish transmitting */
    while(!(MCF_UART_UCSR(TERMINAL_PORT) & MCF_UART_USR_TXEMP)) {}; 
 	
 	/* 
 	 * 	Turning off PLL, dividing clock by 32768 to get the lowest 
 	 *	frequency possible
 	 */
 	clock_pll(REF_CLK_KHZ, 0, PLL_DISABLE);
    clock_lpd(32768);
    
    /* Resetting Counter */      
    MCF_DTIM0_DTCN = 0;
    MCF_DTIM0_DTER = MCF_DTIM_DTER_REF;
}

	/* 
	 *	The sleep command puts the processor in to the lowest power stop mode,	
	 *	waking the processor up once a second, doing one A/D conversion, 		
	 *	displaying the result of the conversion, and going back to low power    
	 *  mode. 
	 */																		
 
void
cmd_sleep (int argc, char **argv)
{
    int count;
    
    /* Resetting CTRL1 register to 0 */
    MCF_ADC_CTRL1 = 0;
    
    printf("Running test\n");

    /* Wait for the UART to finish transmitting */
    while(!(MCF_UART_UCSR(TERMINAL_PORT) & MCF_UART_USR_TXEMP)) {}; 

    /* Change the LEDs that are on to show its working */
    board_led_display(6);

    /* Turn off all module clocks except GPIO, UART0 and INTC0 */

    MCF_SCM_PPMRH = 0
    /*  | MCF_SCM_PPMRH_CDGPIO */
        | MCF_SCM_PPMRH_CDEPORT
        | MCF_SCM_PPMRH_CDPIT0 
        | MCF_SCM_PPMRH_CDPIT1 
        | MCF_SCM_PPMRH_CDADC  
        | MCF_SCM_PPMRH_CDGPT  
        | MCF_SCM_PPMRH_CDPWM  
        | MCF_SCM_PPMRH_CDCFM;
          
    MCF_SCM_PPMRL = 0
        | MCF_SCM_PPMRL_CDMINIBUS     
        | MCF_SCM_PPMRL_CDDMA     
    /*  | MCF_SCM_PPMRL_CDUART0   */
        | MCF_SCM_PPMRL_CDUART1   
        | MCF_SCM_PPMRL_CDUART2   
        | MCF_SCM_PPMRL_CDI2C0     
        | MCF_SCM_PPMRL_CDQSPI 
        | MCF_SCM_PPMRL_CDI2C1     
        | MCF_SCM_PPMRL_CDRTC             
    /*  | MCF_SCM_PPMRL_CDDTIM0    */
        | MCF_SCM_PPMRL_CDDTIM1    
        | MCF_SCM_PPMRL_CDDTIM2    
        | MCF_SCM_PPMRL_CDDTIM3
	/*	| MCF_SCM_PPMRL_CDINTC0    */	
		| MCF_SCM_PPMRL_CDINTC1  
		| MCF_SCM_PPMRL_CDFEC       
         ;

 	/* 
 	 * Determine which memory the code is running from (SRAM of Flash) 
 	 * and turn off other memory to save power
 	 */
 	 
    if (running_in_sram)
    {
        /* Disable clock to CFM */
       MCF_SCM_PPMRH |= MCF_SCM_PPMRH_CDCFM;
    }
    else
        /* Disable SRAM instruction accesses */
        mcf5xxx_wr_rambar1(SRAM_ADDRESS + 0x35);       

    /* Setting Interrupt Controller priority and level */
    MCF_INTC0_ICR(19) = 0
        | MCF_INTC_ICR_IP(7)
        | MCF_INTC_ICR_IL(1);
    
    /* Unmasking the corresponding interrupt mask for DTIM0 */
    MCF_INTC0_IMRL &= ~(0
        | MCF_INTC_IMRL_INT_MASK19 
        | MCF_INTC_IMRL_MASKALL);
    mcf5xxx_set_handler(64 + 19, (ADDRESS)dtim0_handler);
    mcf5xxx_irq_enable();


    /* Enable DMA Timer 0 */
    
    /* 
     *	Change the value of the DTRR (value in parenthesis; currently
     *	set to 175) to increase or decrease frequency that the processor 
     *	comes out of sleep mode; it's currently set to come out of sleep 
     *	and do converstion approximately once every second. The timing of 
     *	interrupt depends on several variables, such as the frequency at 
     *	which the processor is running
     */
    
    /* Setting the DMA Timer 0 registers accordingly */ 
    MCF_DTIM0_DTRR = (175);
    MCF_DTIM0_DTER = MCF_DTIM_DTER_REF;
    MCF_DTIM0_DTMR = 0
        | MCF_DTIM_DTMR_PS(0)
        | MCF_DTIM_DTMR_ORRI
        | MCF_DTIM_DTMR_OM
        | MCF_DTIM_DTMR_FRR
        | MCF_DTIM_DTMR_CLK_DIV1;
    
    /* Putting processor in to Doze Mode */
    MCF_PMM_LPCR = MCF_PMM_LPCR_LPMD_DOZE;

    /* Disable the PLL */
    clock_pll(REF_CLK_KHZ, 0, PLL_DISABLE);
    clock_lpd(32768);
    /* sys clock now = (8MHz / 32768) = 244 Hz */
	
    /* Number of times to execute the sleep/wake-up loop */
    count = 2;
    while(1)
    {
		/* Turn on the Timer */
	    MCF_DTIM0_DTMR |= MCF_DTIM_DTMR_RST;
	    
        /* Execute the stop instruction */
        stop_2000();

	    if (!--count)
	    {
			/* Turn off the Timer */
		    MCF_DTIM0_DTMR = 0;
		    
       	    mcf5xxx_irq_disable();

			/* Reset the system clock */
	    	clock_lpd(1);
			clock_pll(REF_CLK_KHZ, sys_clk_khz, 0);
	    	
	    	/* Turning all module clocks back on */
	        MCF_SCM_PPMRH = 0;
	        MCF_SCM_PPMRL = 0;
	                
			/* Turn LEDs back on */
	        board_led_display(15);
	        	        
	        printf("Test complete\n");
	        break;
	    }
    }
}
/***************************************************************************/
   /* 
   	*	This command will find the average of x conversions (defined by data)
   	*	on one A/D channel (AN0 in this example). It does so by accumulating 
   	*	the results of each of the conversions, then dividing by the number 
   	*	of conversions that were done to find the average.
	*/   	   
	 
void
cmd_average (int argc, char **argv)
    {
    	/* Making declarations */
      	int i;												
    	int data[1000];										
    	int avg = 0;										

		/* Loop for doing conversions */
	
		/* 
		 *  Change the value from 1000 to a desired value if you would like
		 *	to use a different number of conversions for the average test
		 */
      	for (i = 0; i < 1000; i++)
      	{
			/* Clear stop bit */
	 		MCF_ADC_CTRL1 &= ~MCF_ADC_CTRL1_STOP0; 			

			/* Set start bit */
      		MCF_ADC_CTRL1 |= MCF_ADC_CTRL1_START0; 			
			
			/* Waiting for "Ready" bit to be set */
      		while (!(MCF_ADC_ADSTAT & MCF_ADC_ADSTAT_RDY0))	
      		{};
      		
			/* Array of comversions being done on AN0 */
      		data[i] = (MCF_ADC_ADRSLT0 >> 3);

			/* Set stop bit */
 			MCF_ADC_CTRL1 |= MCF_ADC_CTRL1_STOP0; 				
      	}
      	
		/* 
		 *	Dividing by the number of measurements to determine the average 
		 *  The value for i here must match the value of i in previous "for" 
		 *	loop 
		 */
		 
      	for(i = 0; i < 1000; i++)									
      	{
			/* Displaying the value of each conversion */
			/* Uncomment the following line to see the value of each conversion */
			/* printf("data [%d] = %04d\n",i,data[i]); */			
			
			/* Running cumulation of conversions */
			avg += data[i];										
      	}
	
		/* Displaying the average */
		/* The divider here must match the previous values of i */
		printf("\nAverage = 0x%04d\n",avg/1000);				

    }
/*********************************************************************/
	/*
	 * The offset command allows you to set the voltage offset for   
	 *	conversions. When setting an offset, it should be entered in 
	 *	decimal form.												 
	 */
		
void
cmd_offset (int argc, char **argv)
{
    int i, success, offset;
    
    /* 
     *	If not the right number of arguments for the offset command, 
     *	display the current offset value for all 8 A/D channels 
     */
    	
    if (argc == 0 || argc == 1)
    {
        for (i = 0; i < 8; i++ )
        {
        	printf("OFFSET%d = %#04X (%d)\n", i, MCF_ADC_ADOFS(i)>>3, MCF_ADC_ADOFS(i)>>3);
        }
    }
    else    
    {
        /* If correct number of arguments, get channel for the offset */
        i = get_value(argv[1],&success,10);
        if (success == 0 || i > 7 || i < 0)
        {
            /* If incorrect range for channel, display error message */
			printf(INVALUE,argv[1]);
            printf(" Value must be between 0 and 7\n");
            return;
        }

        /* Get the value of offset */
        offset = get_value(argv[2],&success,10);
        if (success == 0 || offset > 4095 || offset < 0)
        {
            /* If incorrect range for offset, display error message */
            printf(INVALUE,argv[2]);
            printf(" Value must be between 0 and 4095\n");
            return;
        }
        
        /* Set the offset to the appropriate channel */
        MCF_ADC_ADOFS(i) = (uint16)MCF_ADC_ADOFS_OFFSET(offset);
        
    }
}
/********************************************************************/
	/* 
	 *	This command displays the current contents of all of the ADC 
	 *	registers
     */
void
cmd_dump (int argc, char **argv)
{
    /* Diplay the ADC control registers */
    printf("\tADSTAT   = %04X\n",  (MCF_ADC_ADSTAT));		
    printf("\tADLSTAT  = %04X\n",  (MCF_ADC_ADLSTAT));
    printf("\tADZCSTAT = %04X\n",  (MCF_ADC_ADZCSTAT));
    printf("\tADCR1    = %04X\n",  (MCF_ADC_CTRL1));
    printf("\tADZCC    = %04X\n",  (MCF_ADC_ADZCC));
    printf("\tADLST1   = %04X\n",  (MCF_ADC_ADLST1));
    printf("\tADLST2   = %04X\n",  (MCF_ADC_ADLST2));
    printf("\tADSDIS   = %04X\n",  (MCF_ADC_ADSDIS));
											 
	/* Diplay the ADC result registers */										 
    printf("\n");
    printf("\tADRSLT0  = %04X\n",  (MCF_ADC_ADRSLT0));	
    printf("\tADRSLT1  = %04X\n",  (MCF_ADC_ADRSLT1));
    printf("\tADRSLT2  = %04X\n",  (MCF_ADC_ADRSLT2));
    printf("\tADRSLT3  = %04X\n",  (MCF_ADC_ADRSLT3));
    printf("\tADRSLT4  = %04X\n",  (MCF_ADC_ADRSLT4));
    printf("\tADRSLT5  = %04X\n",  (MCF_ADC_ADRSLT5));
    printf("\tADRSLT6  = %04X\n",  (MCF_ADC_ADRSLT6));
    printf("\tADRSLT7  = %04X\n",  (MCF_ADC_ADRSLT7));

    /* Diplay the ADC low limit registers */
    printf("\n");
    printf("\tADLLMT0  = %04X\n",  (MCF_ADC_ADLLMT0));	
    printf("\tADLLMT1  = %04X\n",  (MCF_ADC_ADLLMT1));
    printf("\tADLLMT2  = %04X\n",  (MCF_ADC_ADLLMT2));
    printf("\tADLLMT3  = %04X\n",  (MCF_ADC_ADLLMT3));
    printf("\tADLLMT4  = %04X\n",  (MCF_ADC_ADLLMT4));
    printf("\tADLLMT5  = %04X\n",  (MCF_ADC_ADLLMT5));
    printf("\tADLLMT6  = %04X\n",  (MCF_ADC_ADLLMT6));
    printf("\tADLLMT7  = %04X\n",  (MCF_ADC_ADLLMT7));
    
    /* Diplay the ADC high limit registers */
    printf("\n");
    printf("\tADHLMT0  = %04X\n",  (MCF_ADC_ADHLMT0));	
    printf("\tADHLMT1  = %04X\n",  (MCF_ADC_ADHLMT1));
    printf("\tADHLMT2  = %04X\n",  (MCF_ADC_ADHLMT2));
    printf("\tADHLMT3  = %04X\n",  (MCF_ADC_ADHLMT3));
    printf("\tADHLMT4  = %04X\n",  (MCF_ADC_ADHLMT4));
    printf("\tADHLMT5  = %04X\n",  (MCF_ADC_ADHLMT5));
    printf("\tADHLMT6  = %04X\n",  (MCF_ADC_ADHLMT6));
    printf("\tADHLMT7  = %04X\n",  (MCF_ADC_ADHLMT7));
    
    /* Diplay the ADC offset registers */
    printf("\n");
    printf("\tADOFS0  = %04X\n",  (MCF_ADC_ADOFS0));		
    printf("\tADOFS1  = %04X\n",  (MCF_ADC_ADOFS1));
    printf("\tADOFS2  = %04X\n",  (MCF_ADC_ADOFS2));
    printf("\tADOFS3  = %04X\n",  (MCF_ADC_ADOFS3));
    printf("\tADOFS4  = %04X\n",  (MCF_ADC_ADOFS4));
    printf("\tADOFS5  = %04X\n",  (MCF_ADC_ADOFS5));
    printf("\tADOFS6  = %04X\n",  (MCF_ADC_ADOFS6));
    printf("\tADOFS7  = %04X\n",  (MCF_ADC_ADOFS7));
}

/********************************************************************/
	/* This command issues a simple start of ADC conversions. */
void
cmd_start (int argc, char **argv)
{
	/* Clear stop bit */
	MCF_ADC_CTRL1 &= ~MCF_ADC_CTRL1_STOP0; 			

	/* Set start bit */
	MCF_ADC_CTRL1 |= MCF_ADC_CTRL1_START0; 
	
				
}

/********************************************************************/
	/* This command stops all ADC conversions. */
void
cmd_stop (int argc, char **argv)
{
	/* Set stop bit */
	MCF_ADC_CTRL1 |= MCF_ADC_CTRL1_STOP0; 
	printf("\nAll Conversions stopped\n");
	
	
}

/********************************************************************/
	/* This command disables ADC channels from conversions. */
void
cmd_disable (int argc, char **argv)
{
    int success, channel;
    
    /* If incorrect number of arguments */
    if (argc == 0)
    {
        {
			/* Display Disable register */
        	printf("ADSDIS = %#02X \n", MCF_ADC_ADSDIS);
        }
    }
    else    
    {
        /* Get user input for the desired channel to disable*/
        channel = get_value(argv[1],&success,10);
        if (success == 0)
        {
            printf(INVALUE,argv[1]);
            return;
        }
        
		/* Set disable bit */
     	MCF_ADC_ADSDIS |= (0x1 << channel);
        
    }
}

/**********************************************************************/
	/*
	 *	The zero command allows you to set the zero crossing condition
	 *	for conversions crossing the 0V boundary. When setting an     
	 *	offset, it should be entered in decimal form.
	 */

void
cmd_zero (int argc, char **argv)
{
    int channel, success, mode;
    
    /* If not the right number of arguments for the zero command, 
    	display the current value in the ADZCC register */    
    if (argc == 0 || argc == 1)
    {
       printf("ADZCC = %#04X\n", MCF_ADC_ADZCC);
    }
 
    /* If there are the correct number of arguments for the command: */
    else 
    {
        /* Get user input of A/D channel  */
        channel = get_value(argv[1],&success,10);
        if (success == 0 || channel > 7 || channel < 0)
        {
            printf(INVALUE,argv[1]);
            printf(" Value must be between 0 and 7\n");
            return;
        }
        
        /* Get user input of zero crossing configuration */
        mode = get_value(argv[2],&success,10);
        if (success == 0 || mode > 3 || mode < 0)
        {
            printf(INVALUE,argv[2]);
            printf(" Value must be between 0 and 3\n");
            return;
        }
       
        /* Set the Zero Crossing bits in the ADZCC to the desired configuration */
        MCF_ADC_ADZCC = MCF_ADC_ADZCC & ~ (0x3 << (channel * 2))
        	| ((mode & 0x3) << (channel * 2));
    }
}
/**********************************************************************/
	/*
	 * The adlst command allows you to order the list of the analog	 
	 *	input channels to be converted when the next scan is 		 
	 *	initiated. When setting the ADLST it should be entered in    
	 *	decimal form.   
	 */
	 
void
cmd_adlst1 (int argc, char **argv)
{
    int sample, success, mode;
    
    /* If not the right number of arguments for the adlst1 command, 
    	display the current value for the ADLST1 register */
    if (argc == 0 || argc == 1)
    {
       	/* Display ADLST1 register */
		printf("ADLST1 = %#04X\n", MCF_ADC_ADLST1);
    }

    /* If there are the correct number of arguments for the command: */
    else
    {
        /* Get user input of list configuration */
        sample = get_value(argv[1],&success,10);
        if (success == 0 || sample > 3 || sample < 0)
        {
            printf(INVALUE,argv[1]);
            printf(" Value must be between 0 and 3\n");
            return;
        }
        
        /* Get user input of A/D channel  */
        mode = get_value(argv[2],&success,10);
        if (success == 0 || mode > 7 || mode < 0)
        {
            printf(INVALUE,argv[2]);
            printf(" Value must be between 0 and 7\n");
            return;
        }
 
        /* Set the Channel List 1 bits in the ADLST1 to the desired configuration */
        MCF_ADC_ADLST1 = MCF_ADC_ADLST1 & ~ (0x7 << (sample * 4))
        	| ((mode & 0x7) << (sample * 4));
    }
}
/********************************************************************/
	/*
	 *  The adlst command allows you to order the list of the analog
	 *	input channels to be converted when the next scan is 		  
	 *	initiated. When setting the ADLST it should be entered in     
	 *	decimal form.   											  
	 */

void
cmd_adlst2 (int argc, char **argv)
{
    int sample, success, mode;
    
    /* If not the right number of arguments for the adlst2 command, 
    	display the current value for the ADLST2 register */
    if (argc == 0 || argc == 1)
    {
		/* Display ADLST2 register */
       printf("ADLST2 = %#04X\n", MCF_ADC_ADLST2);
    }
     
    /* If there are the correct number of arguments for the command: */
    else
    {
        /* Get user input of list configuration */
        sample = get_value(argv[1],&success,10);
        if (success == 0 || sample > 7 || sample < 4)
        {
            printf(INVALUE,argv[1]);
            printf(" Value must be between 4 and 7\n");
            return;
        }
        
        /* Get user input of A/D channel  */
        mode = get_value(argv[2],&success,10);
        if (success == 0 || mode > 7 || mode < 0)
        {
            printf(INVALUE,argv[2]);
            printf(" Value must be between 0 and 7\n");
            return;
        }
 
        /* Set the Channel List 2 bits in the ADLST2 to the desired configuration */
        MCF_ADC_ADLST2 = (uint16)MCF_ADC_ADLST2 & ~ (0x7 << ((sample - 4) * 4))
        	| ((mode & 0x7) << ((sample - 4) * 4));
    }
}
/********************************************************************/
	/*
	 *  This command allows you to set the low limit comparison for 
	 *	conversions. When declaring an offset, it should be entered 
	 *	in decimal form, with a value of between 0 and 4095. 		
	 */

void
cmd_adllmt (int argc, char **argv)
{
    int i, success, llmt;
    
    /* 
     *	If not the right number of arguments for the adllmt command, 
     *	display the current adllmt value for all 8 A/D channels
     */
     
    if (argc == 0 || argc == 1)
    {
        for (i = 0; i < 8; i++ )
        {
        	printf("ADLLMT%d = %#04X (%d)\n", i, MCF_ADC_ADLLMT(i), MCF_ADC_ADLLMT(i) >> 3);
        }
    }
    else    
    {
        /* If correct number of arguments, get the value of low limit */
        llmt = get_value(argv[2],&success,10);
        if (success == 0 || llmt > 4095 || llmt < 0)
        {
            /* If incorrect range for offset, display error message */
            printf(INVALUE,argv[2]);
            printf(" Value must be between 0 and 4095\n");
            return;
        }
        
        /* Get channel for the low limit */
        i = get_value(argv[1],&success,10);
        if (success == 0 || i > 7 || i < 0)
        {
            /* If incorrect range for channel, display error message */
            printf(INVALUE,argv[1]);
            printf(" Value must be between 0 and 7\n");
            return;
        }
        
        /* Set the low limit to the appropriate channel */
        MCF_ADC_ADLLMT(i) = (uint16)MCF_ADC_ADLLMT_LLMT(llmt);
        
    }
}
/********************************************************************/
	/*
	 * This command allows you to set the high limit comparison for 
	 *	conversions. When declaring an offset, it should be entered 
	 *	in decimal form, with a value of between 0 and 4095. 		
	 */

void
cmd_adhlmt (int argc, char **argv)
{
    int i, success, hlmt;
    
    /* 
     *	If not the right number of arguments for the adhlmt command, 
     *	display the current adhlmt value for all 8 A/D channels 
     */
     
    if (argc == 0 || argc == 1)
    {
        for (i = 0; i < 8; i++ )
        {
        	printf("ADHLMT%d = %#04X (%d)\n", i, MCF_ADC_ADHLMT(i), MCF_ADC_ADHLMT(i) >> 3);
        }
    }
    else    
    {
        /* If correct number of arguments, get the value of high limit */
        hlmt = get_value(argv[2],&success,10);
        if (success == 0 || hlmt > 4095 || hlmt < 0)
        {
            /* If incorrect range for offset, display error message */
            printf(INVALUE,argv[2]);
            printf(" Value must be between 0 and 4095\n");
            return;
        }
        
        /* Get channel for the high limit */
        i = get_value(argv[1],&success,10);
        if (success == 0 || i > 7 || i < 0)
        {
            /* If incorrect range for channel, display error message */
            printf(INVALUE,argv[1]);
            printf(" Value must be between 0 and 7\n");
            return;
        }
        
        /* Set the high limit to the appropriate channel */
        MCF_ADC_ADHLMT(i) = (uint16)MCF_ADC_ADHLMT_HLMT(hlmt);
        
    }
}



/********************************************************************/

	/* Set/Show Commands */

/********************************************************************/

	/* This command sets the Scan Mode in the ADC control 1 register */
	
void
setshow_smode (int argc, char **argv)
{
    int success, smode;
    
    /* Set */
    if (argv[2] != NULL)
    {
        /* Get user input for the smode bits in control 1 register */
        smode = get_value(argv[2],&success,2);
        if (success == 0)
        {
            printf(INVALUE,argv[2]);
            return;
        }
        
        /* Set the SMODE bits in the ADCR1 to the desired configuration */
        MCF_ADC_CTRL1 = (uint16)MCF_ADC_CTRL1_SMODE(smode);
    }
    
    /* Show the current value of the smode bits in control 1 register */
    else
        printf("%03b", (MCF_ADC_CTRL1)&0x7);
}
/********************************************************************/
	/* This command changes the Channel Configuration in the ADC
		control 1 register. */
		
void
setshow_chncfg (int argc, char **argv)
{
    int success, chncfg;
    
    /* Set */
    if (argv[2] != NULL)
    {
        /* Get user input for the chncfg bits in the ADCR1 register */
        chncfg = get_value(argv[2],&success,2);
        if (success == 0)
        {
            printf(INVALUE,argv[2]);
            return;
        }
        
        /* Set the CHNCFG bits in the ADCR1 to the desired configuration */
        MCF_ADC_CTRL1 = (uint16)MCF_ADC_CTRL1_CHNCFG(chncfg);
    }
    
    /* Show current value of the chncfg bits in the ADCR1 register */
    else
        printf("%04b", (MCF_ADC_CTRL1 >>4)&0xF);
}
/********************************************************************/

