/*
 * File:        pll_main.c
 * Purpose:     PLL Example
 *
 * License:     All software covered by license agreement in -
 *              docs/Freescale_Software_License.pdf
 */

#include "common.h"
#include "uif.h"
#include "clock/clock.h"
#include "uart/uart.h"

/********************************************************************/
/*
 * Test register definitions
 */

//#define MCF_GPIO_PTEPAR                   (*(vuint8 *)(&__IPSBAR[0x100060]))
//#define MCF_GPIO_PTFPAR                   (*(vuint8 *)(&__IPSBAR[0x100061]))
//#define MCF_GPIO_PTGPAR                   (*(vuint8 *)(&__IPSBAR[0x100062]))
//#define MCF_GPIO_PTHPAR                   (*(vuint16 *)(&__IPSBAR[0x100090]))


#define MRAM_ADDRESS 0x80000000
#define TEST_ADDR    0x80006742
#define TEST_ADDR2   0x80002abc

/*
 * Setup user interface
 */
const char PROMPT[] = "PLL> ";
    
void cmd_sysclk (int, char **);
void cmd_registers (int, char **);
void set_clockout (int, char **);



UIF_CMD UIF_CMDTAB[] =
{
    UIF_CMDS_ALL
    {"clk",0,1,0,cmd_sysclk,"Set PLL output clock","<sys_clk (in KHz)>"},
    {"show_registers",0,0,0,cmd_registers,"Display the PLL registers",""},
    {"set_clockout",0,1,0,set_clockout,"Set clockout enable",""},    
};

void setshow_sysclk (int, char **);
void setshow_syncr (int, char **);
void setshow_mfd (int, char **);
void setshow_rfd (int, char **);
void setshow_lpd (int, char **);
void setshow_cchr (int, char **);
void setshow_baud (int, char **);


UIF_SETCMD UIF_SETCMDTAB[] =
{
    {"sysclk", 0,1,setshow_sysclk,""},
    {"syncr", 0,1,setshow_syncr,""},
    {"mfd", 0,1,setshow_mfd,""},
    {"rfd", 0,1,setshow_rfd,""},
    {"lpd", 0,1,setshow_lpd,""},
    {"cchr",0,1,setshow_cchr,""},
    {"baud", 0,1,setshow_baud,""},    
};

const int UIF_NUM_CMD    = UIF_CMDTAB_SIZE;
const int UIF_NUM_SETCMD = UIF_SETCMDTAB_SIZE;

int sys_clk_khz = SYS_CLK_KHZ;
int baud = TERMINAL_BAUD;

/********************************************************************/
void 
main (void)
{
    mcf5xxx_irq_enable();
    
    printf("\n");
    printf("**************************************************\n");
    printf("*                                                *\n");
    printf("*                  PLL Utility                   *\n");
    printf("*                                                *\n");
    printf("**************************************************\n");
    printf(HELPMSG);
    printf("Enter all clock settings in KHz!\n");
    printf("\n");

    while (TRUE)
    {
        printf(PROMPT);
        run_cmd();
    }
}

/********************************************************************/
void
cmd_sysclk (int argc, char **argv)
{
    int success, sysclk;
    
    if (argc == 1)
    {
        printf("System Clock: %d KHz\n", sys_clk_khz);
    }
    else
    {
        /* Wait for the UART to finish transmitting */
        while(!(MCF_UART_UCSR(TERMINAL_PORT) & MCF_UART_USR_TXEMP)) {}; 

        /* Get user input (in KHz) */
        sysclk = get_value(argv[1],&success,10);
        if (success == 0)
        {
            printf(INVALUE,argv[1]);
            return;
        }
        
        if (sysclk == 0)
        {
            /* Disable the PLL */
            sys_clk_khz = clock_pll(REF_CLK_KHZ, sysclk, PLL_DISABLE) / clock_lpd(0);
        }
        else
        {
            /* Set the PLL to the desired system clock */
            sys_clk_khz = clock_pll(REF_CLK_KHZ, sysclk, 0) / clock_lpd(0);
        }

        /* Re-init the UART with the new system clock setting */
        uart_init(TERMINAL_PORT, sys_clk_khz, baud);
        
        printf("System Clock: %d KHz\n", sys_clk_khz);
    }
}
/********************************************************************/
void
cmd_registers (int argc, char **argv)
{
    uint16 syncr = MCF_CLOCK_SYNCR;
    uint16 synsr = MCF_CLOCK_SYNSR;
    
    printf("\n");
    printf("SYNCR = %#04X\n",  syncr);
    printf("\tPLLEN   = %d\n",  (syncr & MCF_CLOCK_SYNCR_PLLEN));
    printf("\tPLLMODE = %d\n",  (syncr & MCF_CLOCK_SYNCR_PLLMODE) >> 1);
    printf("\tCLKSRC  = %d\n",  (syncr & MCF_CLOCK_SYNCR_CLKSRC)>> 2);
    printf("\tFWKUP   = %d\n",  (syncr & MCF_CLOCK_SYNCR_FWKUP)>> 5);
    printf("\tDISCLK  = %d\n",  (syncr & MCF_CLOCK_SYNCR_DISCLK)>> 6);
    printf("\tLOCEN   = %d\n",  (syncr & MCF_CLOCK_SYNCR_LOCEN)>> 7);
    printf("\tRFD     = %d\n",  (syncr & 0x0700) >> 8);
    printf("\tLOCRE   = %d\n",  (syncr & MCF_CLOCK_SYNCR_LOCRE) >> 11);
    printf("\tMFD     = %d\n",  (syncr & 0x7000) >> 12);
    printf("\tLOLRE   = %d\n",  (syncr & MCF_CLOCK_SYNCR_LOLRE) >>15);
    printf("\n");
    printf("SYNSR = %#02X\n",  synsr);
    printf("\tLOCS   = %d\n",   (synsr & MCF_CLOCK_SYNSR_LOCS) >>2);
    printf("\tLOCK   = %d\n",   (synsr & MCF_CLOCK_SYNSR_LOCK) >>3);
    printf("\tLOCKS  = %d\n",   (synsr & MCF_CLOCK_SYNSR_LOCKS)>>4);
    printf("\tCRYOSC = %d\n",   (synsr & MCF_CLOCK_SYNSR_CRYOSC)>>5);
    printf("\tOCOSC  = %d\n",   (synsr & MCF_CLOCK_SYNSR_OCOSC)>>6);
    printf("\tEXTOSC = %d\n",   (synsr & MCF_CLOCK_SYNSR_EXTOSC) >> 7);
    printf("\n");
    
    printf("ROCR     = %#04X\n", MCF_CLOCK_ROCR);
    printf("LPDR     = %#02X\n", MCF_CLOCK_LPDR);
    printf("CCHR     = %#02X\n", MCF_CLOCK_CCHR);
    printf("CCLR     = %#02X\n", MCF_CLOCK_CCLR);
    printf("OOHR     = %#02X\n", MCF_CLOCK_OCHR);
    printf("OOLR     = %#02X\n", MCF_CLOCK_OCLR);
    
    printf("PPMRH    = %#08X\n", MCF_SCM_PPMRH);
    printf("PPMRL    = %#08X\n", MCF_SCM_PPMRL);

	printf("\nInput clock is %d MHz\n",EXT_CLK_MHZ);	
	printf("\nSystem clock is %d MHz\n",clock_speed());
}
/********************************************************************/
void
setshow_sysclk (int argc, char **argv)
{
    int success, sysclk;
    
    /* Set */
    if (argv[2] != NULL)
    {
        /* Wait for the UART to finish transmitting */
        while(!(MCF_UART_UCSR(TERMINAL_PORT) & MCF_UART_USR_TXEMP)) {}; 

        /* Get user input (in KHz) */
        sysclk = get_value(argv[2],&success,10);
        if (success == 0)
        {
            printf(INVALUE,argv[2]);
            return;
        }
        
        if (sysclk == 0)
        {
            /* Disable the PLL */
            sys_clk_khz = clock_pll(REF_CLK_KHZ, sysclk, PLL_DISABLE) / clock_lpd(0);
        }
        else
        {
            /* Set the PLL to the desired system clock */
            sys_clk_khz = clock_pll(REF_CLK_KHZ, sysclk, 0) / clock_lpd(0);
        }

        /* Re-init the UART with the new system clock setting */
        uart_init(TERMINAL_PORT, sys_clk_khz, baud);
        
        printf("System Clock: %d KHz\n", sys_clk_khz);
    }
    
    /* Show */
    else
    {
        sys_clk_khz = clock_speed()*1000;    
    	printf("%d KHz", sys_clk_khz);
    }
}




/********************************************************************/
void
set_clockout (int argc, char **argv)
{
	int clkout,fout,fref,input_clk,divider,lpdr,mfd,rfd;
    int success;
    clkout=get_value(argv[1],&success,10);

     printf("Setting clockout to %d\n\n",clkout);

 	 if(clkout==1)
 	 {
 	 	MCF_CLOCK_SYNCR |=	MCF_CLOCK_SYNCR_DISCLK;
 	 }
 	 else
 	 {
 	 	MCF_CLOCK_SYNCR &=	~(MCF_CLOCK_SYNCR_DISCLK);
 	 }
 	
}


/********************************************************************/
void
setshow_syncr (int argc, char **argv)
{
    int success;
    uint16 syncr;

    /* Set */
    if (argv[2] != NULL)
    {
        /* Wait for the UART to finish transmitting */
        while(!(MCF_UART_UCSR(TERMINAL_PORT) & MCF_UART_USR_TXEMP)) {}; 

        syncr = (uint16) get_value(argv[2],&success,16);
        if (success == 0)
        {
            printf(INVALUE,argv[2]);
            return;
        }
        
        MCF_CLOCK_SYNCR = syncr;
        
        if ((MCF_CLOCK_SYNCR & MCF_CLOCK_SYNCR_CLKSRC) &&
            (MCF_CLOCK_SYNCR & MCF_CLOCK_SYNCR_PLLEN))
        {
            /* Wait for the PLL to lock */  
            while (!(MCF_CLOCK_SYNSR & MCF_CLOCK_SYNSR_LOCK)) {};
        }
        
        sys_clk_khz = clock_speed()*1000;

        /* Re-init the UART with the new system clock setting */
        uart_init(TERMINAL_PORT, sys_clk_khz, baud);
        
        printf("System Clock: %d KHz\n", sys_clk_khz);
    }
    
    /* Show */
    else
        printf("%#04X", MCF_CLOCK_SYNCR);
}

/********************************************************************/
void
setshow_lpd (int argc, char **argv)
{
    int success, lpd;

    /* Set */
    if (argv[2] != NULL)
    {
        /* Wait for the UART to finish transmitting */
        while(!(MCF_UART_UCSR(TERMINAL_PORT) & MCF_UART_USR_TXEMP)) {}; 

        lpd = (uint8) get_value(argv[2],&success,10);
        MCF_CLOCK_LPDR = MCF_CLOCK_LPDR_LPD(lpd);
        if (success == 0)
        {
            printf(INVALUE,argv[2]);
            return;
        }
        
        sys_clk_khz = clock_speed()*1000;

        /* Re-init the UART with the new system clock setting */
        uart_init(TERMINAL_PORT, sys_clk_khz, baud);
    }
    
    /* Show */
    else
        printf("%d", clock_lpd(0));
}

/********************************************************************/
void
setshow_cchr (int argc, char **argv)
{
    int success, cchr;

    /* Set */
    if (argv[2] != NULL)
    {
        cchr = (uint8) get_value(argv[2],&success,10);
        if(EXT_CLK_MHZ%(cchr+1)!=0)
        {
    		printf("%d/%d is not a whole number, terminal will no longer work\n",EXT_CLK_MHZ,(cchr+1));	
        }
    
        /* Wait for the UART to finish transmitting */
        while(!(MCF_UART_UCSR(TERMINAL_PORT) & MCF_UART_USR_TXEMP)) {}; 

		if(cchr>=0 && cchr<=0x7)
			MCF_CLOCK_CCHR=cchr;
        
        if (success == 0)
        {
            printf(INVALUE,argv[2]);
            return;
        }
        
        sys_clk_khz = clock_speed()*1000;

        /* Re-init the UART with the new system clock setting */
        uart_init(TERMINAL_PORT, sys_clk_khz, baud);
    }
    
    /* Show */
    else
        printf("%d",MCF_CLOCK_CCHR);
}
/********************************************************************/
void
setshow_mfd (int argc, char **argv)
{
    int success;
    uint8 mfd;

    /* Set */
    if (argv[2] != NULL)
    {
        /* Wait for the UART to finish transmitting */
        while(!(MCF_UART_UCSR(TERMINAL_PORT) & MCF_UART_USR_TXEMP)) {}; 

        mfd = (uint8) get_value(argv[2],&success,10);
        if (success == 0)
        {
            printf(INVALUE,argv[2]);
            return;
        }
        
        MCF_CLOCK_SYNCR = 0
            | (MCF_CLOCK_SYNCR & ~0x7000) 
            | MCF_CLOCK_SYNCR_MFD(mfd);

        /* Wait for the PLL to lock */  
        while (!(MCF_CLOCK_SYNSR & MCF_CLOCK_SYNSR_LOCK)) {};
        
        sys_clk_khz = clock_speed()*1000;

        /* Re-init the UART with the new system clock setting */
        uart_init(TERMINAL_PORT, sys_clk_khz, baud);
        
        printf("System Clock: %d KHz\n", sys_clk_khz);
    }
    
    /* Show */
    else
        printf("%d", (MCF_CLOCK_SYNCR & 0x7000) >> 12);
}
/********************************************************************/
void
setshow_rfd (int argc, char **argv)
{
    int success;
    uint8 rfd;

    /* Set */
    if (argv[2] != NULL)
    {
        /* Wait for the UART to finish transmitting */
        while(!(MCF_UART_UCSR(TERMINAL_PORT) & MCF_UART_USR_TXEMP)) {}; 

        rfd = (uint8) get_value(argv[2],&success,10);
        if (success == 0)
        {
            printf(INVALUE,argv[2]);
            return;
        }
        
        MCF_CLOCK_SYNCR = 0
            | (MCF_CLOCK_SYNCR & ~0x0700) 
            | MCF_CLOCK_SYNCR_RFD(rfd);
        
        /* 
         * Changing RFD doesn't create a loss of lock
         * so no need to wait for PLL to lock again
         */

        sys_clk_khz = clock_speed()*1000;

        /* Re-init the UART with the new system clock setting */
        uart_init(TERMINAL_PORT, sys_clk_khz, baud);
        
        printf("System Clock: %d KHz\n", sys_clk_khz);
    }
    
    /* Show */
    else
        printf("%d", (MCF_CLOCK_SYNCR & 0x0700) >> 8);
}
/********************************************************************/
void
setshow_baud (int argc, char **argv)
{
    int success, temp;

    /* Set */
    if (argv[2] != NULL)
    {
        temp = get_value(argv[2],&success,10);
        if (success == 0)
        {
            printf(INVALUE,argv[2]);
            return;
        }
        switch (temp)
        {
            case 110:  
            case 300:  
            case 600:  
            case 1200:  
            case 2400:  
            case 4800:  
            case 9600:  
            case 14400:
            case 19200:
            case 38400: 
            case 57600: 
            case 115200:
                baud = temp;
                break;    
            default:    
                printf(INVALUE,argv[2]);
                return;
        }
        /* Re-init the UART with the new system clock setting */
        uart_init(TERMINAL_PORT, sys_clk_khz, baud);
    }
    
    /* Show */
    else
        printf("%d", baud);
}
/********************************************************************/
