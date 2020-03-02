/********************************************************************/

/*
 * File:		BWDT main.c
 * Purpose:		check BWT functionality
 *
 */

#include "common.h"
#include "uif.h"
#include<stdio.h>
/*
 * Function declaration for each testcase
 */

/* BWT TestCase Commands*/
void bwdt_read_registers                 (int, char **);
void bwdt_system_rst					 (int, char **);
void bwdt_clock_src                      (int ,char **);

//void pmm_enter_stop_mode(void);

//cmd  min_arg max_arg flag func  Description syntax  block regression_flag error_flag

UIF_CMD UIF_CMDTAB[] = 
{
    UIF_CMD_HELP 
	{"bwt_read_registers"             ,0,0,0, bwdt_read_registers,"Displays bwdt registers"," "}, \
	{"bwt_sys_rst"                    ,0,0,0, bwdt_system_rst,"Enable Backup Watchdog Timer"," "},
	{"bwt_clk_src"                    ,0,0,0, bwdt_clock_src,"Enable BWT from two clock sources"," "},
};

const int UIF_NUM_CMD    = UIF_CMDTAB_SIZE;

UIF_SETCMD UIF_SETCMDTAB[] = {"",0,0,NULL,""};
const int UIF_NUM_SETCMD = 0;


/***********************************************************************/

void main (void)
{
	int i;	
	char j;
	char **arr;
	
    /* Global interrupt enable */
	mcf5xxx_irq_enable();

	/* Command Prompt */
   	printf("\n\nBackup Watchdog Timer Module\n");
    while (TRUE)
    {
        printf("BWT>" );
        run_cmd();
    } 
}
/********************************************************************/

void
bwdt_read_registers(int argc, char ** argv)
{
    printf("BWCR    = %#02X\n", MCF_BWT_WCR);
    printf("BWMR    = %#02X\n", MCF_BWT_WMR);
    printf("BWCNTR  = %#02X\n", MCF_BWT_WCNTR);
    printf("BWSR    = %#02X\n", MCF_BWT_WSR);
    printf("BWCR_CLK= %#02X\n", MCF_CLOCK_BWCR);
      
  
}


/*********************************************************************/

/*********************************************************************/

void bwdt_system_rst (int argc, char ** argv)
{
  uint32 i ;
  uint8 exit;
  uint8 ch;
  printf("Enables Watchdog Timer\n");
  printf("Press 0 to let timer expire. Press 1 to continuously service request\n");
  printf("Must to power-on reset to disable the watchdog timer\n");
  
  exit=0;
  ch = in_char(); 
  out_char((char)ch);
  out_char(0x0D);    /* CR */
  out_char(0x0A);    /* LF */
  
  if(ch == '0')
  {
  	printf("Wait, system reset will generate soon \n");
  }  
  else
  {
  	printf("System reset will NOT be generated \n");	
  }
    
  //select system clock as wdt clock source
  MCF_CLOCK_BWCR = 0x00; 
  MCF_BWT_WMR  = 0xFFFF;
  /*enable the watch dog timer*/
  MCF_BWT_WCR  = 0x01 ;

  while(1)
  {
  	if(ch == '0')
    {
     	while(MCF_BWT_WCNTR != 0x1111);
        printf("WCNTR  = %#02X\n", MCF_BWT_WCNTR);
        while(MCF_BWT_WCNTR != 0xff);
        printf("WCNTR  = %#02X\n", MCF_BWT_WCNTR);
        printf("MCF_RCM_RCR   = %#02X\n", MCF_RCM_RCR ); 
    }    
    else
    {	   
	   //wait for the value in BWCNTR to change
       for(i=0;i<1000;i++);
       //write the sequence to service the WDT
       MCF_BWT_WSR = 0x5555;
       MCF_BWT_WSR = 0xAAAA;
       printf("Press any key to EXIT !  \n\n");
  
       while(MCF_BWT_WCNTR != 0xdddd)
       {
       	if((MCF_UART_USR(TERMINAL_PORT) & MCF_UART_USR_RXRDY))
   	   	{
         	//break this loop if key pressed
         	exit=1;
    	 	break; 
       	}          	
       }
       printf("WCNTR  = %#02X\n", MCF_BWT_WCNTR);
       if(exit)
       	break;
       
               
	}
  }
}

/********************************************************************/

/********************************************************************/
/*This function checks that the bwdt can be clocked either by the   */
/*system clock or the relaxation oscillator                         */


void bwdt_clock_src (int argc, char ** argv)
{
  uint32 i ;
  uint8 ch;
  
  printf("Press 0 to select system clock for BWT source\n");
  printf("Press 1 for relaxation oscillator \n");
  printf("Must to power-on reset to disable the watchdog timer\n");
  ch = in_char(); 
  out_char((char)ch);
  out_char(0x0D);    /* CR */
  out_char(0x0A);    /* LF */
  
  if(ch == '0')
  {
  	printf("\nSystem clock as source clk of BWDT \n");
    MCF_CLOCK_BWCR = 0x00; 
    MCF_BWT_WMR  = 0xFFFF;
    /*enable the watch dog timer*/
    MCF_BWT_WCR  = 0x01 ;
  }  
  else
  {
  	printf("\nRelaxation oscillator as a source clk of BWDT \n");
    MCF_CLOCK_BWCR = 0x01; 
    MCF_BWT_WMR  = 0xFFFF;
    /*enable the watch dog timer*/
    MCF_BWT_WCR  = 0x01 ;	
  }
 
  if(ch == '0')
  {	   
  	printf("The system reset will be generated in \n");
   	printf("approximately 7 seconds with system clock and value 0xffff\n"); 
   	while(MCF_BWT_WCNTR != 0x1111);
    printf("\nBWCNTR  = %#02X\n", MCF_BWT_WCNTR);
    while(MCF_BWT_WCNTR != 0xff);
    printf("\nBWCNTR  = %#02X\n", MCF_BWT_WCNTR);
  }   
  else
  {   	    
  	printf("The system reset will be generated in \n");
   	printf("approximately 33 seconds with relaxation oscillator and value 0xffff\n"); 
    while(MCF_BWT_WCNTR != 0x1111);
    printf("\nBWCNTR  = %#02X\n", MCF_BWT_WCNTR);
    while(MCF_BWT_WCNTR != 0xff);
    printf("\nBWCNTR  = %#02X\n", MCF_BWT_WCNTR);
  }     
}

/**********************************************************************/









