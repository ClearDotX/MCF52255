/*
 * File:        power_main.c
 * Purpose:     Power Example
 *
 * Notes:       This test was created for the M5213EVB, but can
 *              be modified to use other platforms.  Interrupt
 *              usage may be the only change to consider.
 * 
 * License:     All software covered by license agreement in -
 *              docs/Freescale_Software_License.pdf
 */

#include "common.h"
#include "uif.h"
#include "clock/clock.h"
#include "uart/uart.h"
#include "power.h"
#include "cfm/cfm.h"
#include "mcf5225x_cfm.h"
#include "mcf5225x_intc1.h"
#include "mcf5225x_bwdt.h"



#define MCF_RTC_RTCCTL               (*(vuint32*)(0x40180010))
#define MCF_RTC_GEN_OSC_COUNT_UPPER  (*(vuint32*)(0x40180034))
#define MCF_RTC_GEN_OSC_COUNT_LOWER  (*(vuint32*)(0x40180038))
#define MCF_RESET_CONTROLLER_RCR     (*(vuint8*) (0x40110000))

#define WRITE0X5C                    0x5C
#define WRITE0X7C                    0x7C
#define WRITE0X07E0                  0x07E0
#define WRITE0X00                    0x00


void configure_RTC(void);
void
configure_RTC(void)
{
 // Set RTC clk source, enable RTC etc.
    MCF_CLOCK_RTCCR = 0x57;
    MCF_RTC_GEN_OSC_COUNT_UPPER = 0x00000000;
    MCF_RTC_GEN_OSC_COUNT_LOWER = 0x00001FFF;
        	
}

/********************************************************************/
/*
 * Setup user interface
 */
const char BANNER[] = "\nWAKEUP Time Measurement Utility\n";
const char PROMPT[] = "WAKEUP > ";

UIF_CMD UIF_CMDTAB[] =
{
    UIF_CMDS_ALL
    
    {"clk", 0,1,0,cmd_sysclk,"Set PLL output clock","<sys_clk>"},
    {"stop",0,0,0,cmd_stop,  "STOP mode (lowest power mode)", ""},
    {"wait",0,0,0,cmd_wait,  "WAIT low-power mode", ""},
    {"doze",0,0,0,cmd_doze,  "DOZE low-power mode", ""},
    {"run", 0,0,0,cmd_run,   "RUN low-power mode", ""},
    {"rtc_ena", 0,0,0,cmd_rtc_en,   "Enable RTC CLOCK ",""},
    
    
 };   

UIF_SETCMD UIF_SETCMDTAB[] =
{
    {"sysclk", 0,1,setshow_sysclk,""},
    {"clkout", 0,1,setshow_clkout,""},
    {"lpd",    0,1,setshow_lpd,""},
    {"stpmd", 0,1,setshow_stpmd,""},
    {"lvdse", 0,1,setshow_lvdse,""},
    {"xipl", 0,1,setshow_xipl,""},
    {"baud", 0,1,setshow_baud,""},
    {"step", 0,1,setshow_step,""},
    {"pwrcfg", 0,1,setshow_pwrcfg,"min/max power config"},
};

const int UIF_NUM_CMD    = UIF_CMDTAB_SIZE;
const int UIF_NUM_SETCMD = UIF_SETCMDTAB_SIZE;
int sys_clk_khz = SYS_CLK_KHZ;

/*
 * Set/show and global variables
 */
int baud      = TERMINAL_BAUD;
int stpmd     = 3;
int lvdse     = 0;
int xipl      = 0;
int exit_loop = FALSE;
int exit_low_power = FALSE;
int clkout    = ON;
int clk_step  = 8000;
int tpwrcfg   = PWR_MAX; /* power configuration for tests */
int cpwrcfg   = PWR_MAX; /* current power configuration */
int running_in_sram;
ADDRESS old_handler;
int loop_test;
volatile int abort_flag;

/********************************************************************/
void
main (void)
{
    extern uint8 __DATA_ROM[];
    extern uint8 __DATA_RAM[];

    /* Determine if execution is from Flash or SRAM */
    running_in_sram = (__DATA_ROM == __DATA_RAM) ? TRUE : FALSE;

    
    //code added to measure the wakeup time from stop3 mode.
    //configure PORTTC for GPIO output
    MCF_GPIO_PTCPAR = MCF_GPIO_PTCPAR_TIN0_GPIO ;
    MCF_GPIO_DDRTC = MCF_GPIO_DDRTC_DDRTC0;
    //set PORTTC bit 0,DTIN0,HIGH
    //it will be set to low once the system wakes up
       MCF_GPIO_PORTTC = 0x01 ; 
     
    
    /*
     * Level-sensitivity needs to be used on the IRQx pins to bring
     * the processor out of stop mode
     */
    MCF_EPORT_EPPAR = 0
        | MCF_EPORT_EPPAR_EPPA4_RISING
        | MCF_EPORT_EPPAR_EPPA5_RISING
        | MCF_EPORT_EPPAR_EPPA7_LEVEL;

    /* Set the interrupt handlers in the vector table */
    mcf5xxx_set_handler(64 + 1, (ADDRESS)sw1_handler);
    mcf5xxx_set_handler(64 + 5, (ADDRESS)sw2_handler);
    mcf5xxx_set_handler(64 + 7, (ADDRESS)abort_handler);

    /* Enable the "maximum" power cnofiguration by default */
    max_pwr_config();

    printf("\n");
    printf("**************************************************\n");
    printf("*                                                *\n");
    printf("*           WAKEUP TIME Measurement Utility            *\n");
    printf("*                                                *\n");
    printf("**************************************************\n");
    printf("\nEnter all clock settings in KHz\n");
    printf("Press ABORT button to exit a loop or low-power modes\n");
    printf("Run STOP command to measure the wakeup time \n");
    printf(HELPMSG);
    printf("\n");

    mainloop();
}
/********************************************************************/
void
mainloop (void)
{
    /* Enable interrupts to the core */
    mcf5xxx_irq_enable();
    
    while (TRUE)
    {
        printf(PROMPT);
        run_cmd();
    }
}




/* Abort ISR in assembly language */

void abort_handler(void) 

{
    asm (
            /* clears the PORTTC */
            clr.b    (0x4010000F)
            
            move.b   0x40130005,d1
            moveq    #0,d0
            move.b   d1,d0
            andi.l   #0x80,d0
            tst.l    d0
            
            moveq    #1,d0
            move.l   d0,0x20003DDC 
            moveq    #1,d1
            move.l   0x20003DD4 ,d0
            cmp.l    d0,d1
            
            clr.l    0x20003DD4 
            /*
             * Exit loop by jumping to mainloop()
             * This leaves stack frames on the stack and could
             * eventually result in stack overflow
             */
            jsr      max_pwr_config 
            
            jsr      mainloop  
          
            move.l   0x20003DD8 ,d0
            tst.l    d0
            
            clr.l    0x20003DD8 
             /* 
              * Raise the power level to max power 
              */
             /* Turn on all the peripheral clocks */ 
             moveq    #64,d0
             move.b   d0,0x40000022 
             /* Turn on the Flash and SRAM BARs */
             moveq    #33,d0
             movec    d0,rambar
             /*mcf5xxx_wr_rambar1(SRAM_ADDRESS + 0x21);*/
             lea      0x20000021 ,a0
             move.l   a0,(a7)
             /*jsr      DummyFn1 */
             /* Enable CLKOUT and BDM signals */
             jsr      clock_clkout_enable 
             jsr      cpu_pstddata_enable 
            
             moveq    #1,d0
             move.l   d0,0x20003C94 
             movem.l  8(a7),d0-d2/a0-a1
             unlk     a6
             addq.l   #4,a7
             rte
               
        
        );

}



/********************************************************************/
/*
 * Increment the PLL output clock by the smallest valid increment
 */
__interrupt__ void
sw1_handler(void)
{
    int pllclk, pwrcfg;

    /* save off current power configuration setting */
    pwrcfg = cpwrcfg;

    /* enable the max config setting */
    max_pwr_config();

    /* get the current PLL clock output */
    pllclk = clock_pll(REF_CLK_KHZ,0,0);

    while (pllclk < MAX_FSYS)
    {
        /* Bump the PLL output up to the next valid clk_step increment */
        pllclk = ((pllclk / clk_step) * clk_step) + clk_step;
        if (clock_pll(REF_CLK_KHZ, pllclk, PLL_PRETEND) == pllclk)
            break;
    }

    /* Set the PLL and calculate the SYS CLK based PLL and LPD settings */
    sys_clk_khz = clock_pll(REF_CLK_KHZ, pllclk, 0) / clock_lpd(0);

    /* Re-init the UART with the new system clock setting */
    uart_init(TERMINAL_PORT, sys_clk_khz, baud);

    printf("System Clock: %d KHz\n", sys_clk_khz);

    /* Wait for the UART to finish transmitting */
    while(!(MCF_UART_UCSR(TERMINAL_PORT) & MCF_UART_USR_TXEMP)) {};

    /* Clear the interrupt event */
    MCF_EPORT_EPFR = MCF_EPORT_EPFR_EPF1;

    /* Restore the power configuration */
    set_pwr_config(pwrcfg);
}


/********************************************************************/
/*
 * Decrement the PLL output clock by the smallest valid increment
 */
__interrupt__ void
sw2_handler(void)
{
    int pllclk, pwrcfg;

    /* save off current power configuration setting */
    pwrcfg = cpwrcfg;

    /* enable the max config setting */
    max_pwr_config();

    /* get the current PLL clock output */
    pllclk = clock_pll(REF_CLK_KHZ,0,0);

    while (pllclk > REF_CLK_KHZ)
    {
        /*
         * Decrement the PLL output so that it is set to the next valid
         * clk_step below the current one
         */
        pllclk = ((pllclk / clk_step) * clk_step) - clk_step;

        if (clock_pll(REF_CLK_KHZ, pllclk, PLL_PRETEND) == pllclk)
            break;
    }

    /* Set the PLL and calculate the SYS CLK based PLL and LPD settings */
    sys_clk_khz = clock_pll(REF_CLK_KHZ, pllclk, 0) / clock_lpd(0);

    /* Re-init the UART with the new system clock setting */
    uart_init(TERMINAL_PORT, sys_clk_khz, baud);

    printf("System Clock: %d KHz\n", sys_clk_khz);

    /* Wait for the UART to finish transmitting */
    while(!(MCF_UART_UCSR(TERMINAL_PORT) & MCF_UART_USR_TXEMP)) {};

    /* Clear the interrupt event */
    MCF_EPORT_EPFR = MCF_EPORT_EPFR_EPF5;

    /* Restore the power configuration */
    set_pwr_config(pwrcfg);
}


/********************************************************************/
void
enter_low_pwr_mode(void)
{
    /* Make sure the UART is done transmitting */
    while(!(MCF_UART_UCSR(TERMINAL_PORT) & MCF_UART_USR_TXEMP));

    /* Setup the low-power interrupt control register */
    MCF_PMM_LPICR = 0
        | MCF_PMM_LPICR_XIPL(xipl)
        | MCF_PMM_LPICR_ENBSTOP;

    /* Enable the "test" power configuration */
    set_pwr_config(tpwrcfg);

    /* 
     * Make sure instructions can be fetched from SRAM so the abort 
     * handler can run.
     */
    mcf5xxx_wr_rambar1(SRAM_ADDRESS + 0x21);

    /* Tell wake-up handler to reset the power level */
    exit_low_power = TRUE;

    /* Execute the stop instruction */
    stop_2000();
    
   
    
}

/*********************************************************************/





/********************************************************************/
void
cmd_stop(int argc, char **argv)
{
    uint32 i ;
    
    printf("Entering STOP mode\n");
    printf("    STPMD = %02b\n",stpmd);
    printf("    LVDSE = %b\n",lvdse);
    printf("    XIPL  = %03b\n",xipl);
     
    printf("The procedure is as follows \n\n");
   
    printf("1> Connect one channel-ch1 of oscilloscope to the IRQ7  \n");
    printf("2> Connect other channel-ch2 of oscilloscope to the TIN0  \n");
    printf("3> Programm the trigger on ch-1 for falling edge   \n");
    printf("4> When ABORT is pressed, IRQ7 will get low which triggers the wakeup  \n");
    printf("   of CPU, and TIN0 will be low when cpu has woken up  \n");
    printf("5> Measure the time difference between the two channels to get the wakeup time  \n\n");
    
    presskey();
      
    printf("Disabling relaxation oscillator input to BWDT    \n");
    MCF_CLOCK_BWCR = 0x00 ;
    
    printf("Disabling LVDE by writing to reset controller register  \n");
    MCF_RESET_CONTROLLER_RCR = 0x04;
    
    
    //setting GPIOs in a mode where internal pull ups are not required
    printf("Disabling GPIO mode in order to have on internal pull ups    \n");
    MCF_GPIO_PNQPAR = 0x4888;
    MCF_GPIO_PANPAR = 0xFF ;
    MCF_GPIO_PASPAR = 0x0F ;
    MCF_GPIO_PTAPAR = 0xAA ;
    
    //this is masked for the wakeuptime measurement 
    //as we are configuring port for DTIN0 timer
    //MCF_GPIO_PTCPAR = 0xFF ; 
    
    //GPIO PUAPAR (bit7..4)was getting programmed to undefined state(Alt2 mode)
    //when we write 0xF0 which doesnot exist for these pins.Only possible mode
    //for these pins are GPIO and primary and Alt1.
    
    MCF_GPIO_PUAPAR |= 0xA0 ;
    MCF_GPIO_PUBPAR = 0xF0 ; 
    MCF_GPIO_PUCPAR = 0xF0 ;
    MCF_GPIO_PQSPAR = 0x3CFF ;
    MCF_GPIO_PTGPAR = 0xF7 ;
    MCF_GPIO_PTJPAR = 0xFF ;
    MCF_GPIO_PTIPAR = 0xFF ;
    MCF_GPIO_PTHPAR = 0xAAAA ;
    MCF_GPIO_PTEPAR = 0xFF ;
    MCF_GPIO_PTFPAR = 0xFF ;
    MCF_GPIO_PDDPAR = 0xFF ;
    
    
    
    MCF_PMM_LPCR = 0
        | MCF_PMM_LPCR_LPMD_STOP
        | MCF_PMM_LPCR_STPMD(stpmd);

   
    
    if (lvdse)
    {
        MCF_PMM_LPCR |= MCF_PMM_LPCR_LVDSE;
    }

    enter_low_pwr_mode();
    
    
    
}
/********************************************************************/
void
cmd_wait(int argc, char **argv)
{
    printf("Entering WAIT mode\n");
    printf("    STPMD = %02b\n",stpmd);
    printf("    LVDSE = %b\n",lvdse);
    printf("    XIPL  = %03b\n",xipl);

    printf("Disabling relaxation oscillator input to BWDT    \n");
    MCF_CLOCK_BWCR = 0x00 ;
    
    //setting GPIOs in a mode where internal pull ups are not required
    printf("Disabling GPIO mode in order to have no internal pull ups    \n");
    
    
    MCF_GPIO_PNQPAR = 0x4888;
    MCF_GPIO_PANPAR = 0xFF ;
    MCF_GPIO_PASPAR = 0x0F ;
    MCF_GPIO_PTAPAR = 0xAA ;
    MCF_GPIO_PTCPAR = 0xFF ;
    //GPIO PUAPAR (bit7..4)was getting programmed to undefined state(Alt2 mode)
    //when we write 0xF0 which doesnot exist for these pins.Only possible mode
    //for these pins are GPIO and primary and Alt1.
    MCF_GPIO_PUAPAR |= 0xA0 ;
    MCF_GPIO_PUBPAR = 0xF0 ; 
    MCF_GPIO_PUCPAR = 0xF0 ;
    MCF_GPIO_PQSPAR = 0x3CFF ;
    MCF_GPIO_PTGPAR = 0xF7 ;
    MCF_GPIO_PTJPAR = 0xFF ;
    MCF_GPIO_PTIPAR = 0xFF ;
    MCF_GPIO_PTHPAR = 0xAAAA ;
    MCF_GPIO_PTEPAR = 0xFF ;
    MCF_GPIO_PTFPAR = 0xFF ;
    MCF_GPIO_PDDPAR = 0xFF ;
   
    
    
    
    
    MCF_PMM_LPCR = MCF_PMM_LPCR_LPMD_WAIT;

    enter_low_pwr_mode();
}
/********************************************************************/
void
cmd_doze(int argc, char **argv)
{
    printf("Entering DOZE mode\n");
    printf("    STPMD = %02b\n",stpmd);
    printf("    LVDSE = %b\n",lvdse);
    printf("    XIPL  = %03b\n",xipl);
    
    printf("Disabling relaxation oscillator input to BWDT    \n");
    MCF_CLOCK_BWCR = 0x00 ;
    
    //setting GPIOs in a mode where internal pull ups are not required
    printf("Disabling GPIO mode in order to have no internal pull ups    \n");
    
    MCF_GPIO_PNQPAR = 0x4888;
    MCF_GPIO_PANPAR = 0xFF ;
    MCF_GPIO_PASPAR = 0x0F ;
    MCF_GPIO_PTAPAR = 0xAA ;
    MCF_GPIO_PTCPAR = 0xFF ;
    //GPIO PUAPAR (bit7..4)was getting programmed to undefined state(Alt2 mode)
    //when we write 0xF0 which doesnot exist for these pins.Only possible mode
    //for these pins are GPIO and primary and Alt1.
    MCF_GPIO_PUAPAR |= 0xA0 ;
    MCF_GPIO_PUBPAR = 0xF0 ; 
    MCF_GPIO_PUCPAR = 0xF0 ;
    MCF_GPIO_PQSPAR = 0x3CFF ;
    MCF_GPIO_PTGPAR = 0xF7 ;
    MCF_GPIO_PTJPAR = 0xFF ;
    MCF_GPIO_PTIPAR = 0xFF ;
    MCF_GPIO_PTHPAR = 0xAAAA ;
    MCF_GPIO_PTEPAR = 0xFF ;
    MCF_GPIO_PTFPAR = 0xFF ;
    MCF_GPIO_PDDPAR = 0xFF ;
    
    
    MCF_PMM_LPCR = MCF_PMM_LPCR_LPMD_DOZE;

    enter_low_pwr_mode();
}


/********************************************************************/
void
cmd_idle(int argc, char **argv)
{
    printf("Entering DOZE/IDLE mode\n");

    /* Make sure the UART is done transmitting */
    while(!(MCF_UART_UCSR(TERMINAL_PORT) & MCF_UART_USR_TXEMP));

    enter_idle_mode();
}


/********************************************************************/
void
cmd_run(int argc, char **argv)
{
    printf("Entering RUN mode\n");
    printf("    STPMD = %02b\n",stpmd);
    printf("    LVDSE = %b\n",lvdse);
    printf("    XIPL  = %03b\n",xipl);

    printf("Disabling relaxation oscillator input to BWDT    \n");
    MCF_CLOCK_BWCR = 0x00 ;
            
    //setting GPIOs in a mode where internal pull ups are not required
    printf("Disabling GPIO mode in order to have no internal pull ups    \n");
    
    MCF_GPIO_PNQPAR = 0x4888;
    MCF_GPIO_PANPAR = 0xFF ;
    MCF_GPIO_PASPAR = 0x0F ;
    MCF_GPIO_PTAPAR = 0xAA ;
    MCF_GPIO_PTCPAR = 0xFF ;
    //GPIO PUAPAR (bit7..4)was getting programmed to undefined state(Alt2 mode)
    //when we write 0xF0 which doesnot exist for these pins.Only possible mode
    //for these pins are GPIO and primary and Alt1.
    MCF_GPIO_PUAPAR |= 0xA0 ;
    MCF_GPIO_PUBPAR = 0xF0 ; 
    MCF_GPIO_PUCPAR = 0xF0 ;
    MCF_GPIO_PQSPAR = 0x3CFF ;
    MCF_GPIO_PTGPAR = 0xF7 ;
    MCF_GPIO_PTJPAR = 0xFF ;
    MCF_GPIO_PTIPAR = 0xFF ;
    MCF_GPIO_PTHPAR = 0xAAAA ;
    MCF_GPIO_PTEPAR = 0xFF ;
    MCF_GPIO_PTFPAR = 0xFF ;
    MCF_GPIO_PDDPAR = 0xFF ;
    
    
    MCF_PMM_LPCR = MCF_PMM_LPCR_LPMD_RUN;

    enter_low_pwr_mode();
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
            sys_clk_khz = (clock_pll(REF_CLK_KHZ, sysclk, PLL_DISABLE) /
                           clock_lpd(0));
        }
        else
        {
            /* Set the PLL to the desired system clock */
            sys_clk_khz = (clock_pll(REF_CLK_KHZ, sysclk, 0) /
                           clock_lpd(0));
        }

        /* Re-init the UART with the new system clock setting */
        uart_init(TERMINAL_PORT, sys_clk_khz, baud);

        printf("System Clock: %d KHz\n", sys_clk_khz);
    }
}


/********************************************************************/
void
cmd_lpdiv (int argc, char **argv)
{
    int success, value, reg;
    unsigned int address = (unsigned int)(&MCF_CLOCK_SYNCR);
     
     
    //setting GPIOs in a mode where internal pull ups are not required
    printf("Disabling GPIO mode in order to have on internal pull ups    \n");
    MCF_GPIO_PNQPAR = 0x4888;
    MCF_GPIO_PANPAR = 0xFF ;
    MCF_GPIO_PASPAR = 0x0F ;
    MCF_GPIO_PTAPAR = 0xAA ;
    MCF_GPIO_PTCPAR = 0xFF ;
    //GPIO PUAPAR (bit7..4)was getting programmed to undefined state(Alt2 mode)
    //when we write 0xF0 which doesnot exist for these pins.Only possible mode
    //for these pins are GPIO and primary and Alt1.
    MCF_GPIO_PUAPAR |= 0xA0 ;
    MCF_GPIO_PUBPAR = 0xF0 ; 
    MCF_GPIO_PUCPAR = 0xF0 ;
    MCF_GPIO_PQSPAR = 0x3CFF ;
    MCF_GPIO_PTGPAR = 0xF7 ;
    MCF_GPIO_PTJPAR = 0xFF ;
    MCF_GPIO_PTIPAR = 0xFF ;
    MCF_GPIO_PTHPAR = 0xAAAA ;
    MCF_GPIO_PTEPAR = 0xFF ;
    MCF_GPIO_PTFPAR = 0xFF ;
    MCF_GPIO_PDDPAR = 0xFF ;
       
    printf("After the register is written uart configuration will not work as sys clock is modified\n");     
  
    if ((argc == 3) && ((argv[1] != NULL) && (argv[2] != NULL)))
    {
        reg = get_value(argv[1],&success,16);
        
        if (success == 0)
        {
            printf(INVALUE,argv[1]);
            return;
        }
        value = get_value(argv[2],&success,16);
        if (success == 0)
        {
            printf(INVALUE,argv[2]);
            return;
        }
        address += reg;
        (*(vuint8*)address) = (uint8)value;
        
               
         
          
    }
    
    
    
     
     
}

/********************************************************************/



/********************************************************************/
/*
 * Unsigned 64 bit multiply
 */
inline asm void
unsigned_multiply(void)
{
    move.l  d2, -(sp)
    move.l  #0, acc         

    move.l  #0x80000001, d1
    move.l  #0x80000001, d0

	/*Form upper half of product and check for negative first operand*/
    mac.l   d0, d1      
    bpl     loop1       
    
    /*Save unsigned correction*/
    move.l  d1, d2      
    
    /*Check for negative second operand*/
    loop1:
    tst.l   d1          
    bpl     loop2  
    
    /*Save unsigned correction*/     
    add.l   d0, d2     
    
    /*Form lower half and check for overflow*/
    loop2:
    mulu.l  d0, d1      
    move.l  macsr, ccr  
    bvs     loop22      
    move.l  acc, d0
    
    /*Correct for unsigned and binary point position*/
    asr.l   #1, d0      
    add.l   d2, d0      

    loop11:
    move.l  (sp)+, d2
    bra     done
    
    /*Correct for overflow*/
    loop22:
    move.l  #0x40000000, d0     
    bra     loop11
    
    done:
}

inline asm void
signed_divide (void)
{
    move.l  #0x44444444,d1
    move.l  #0x22222222,d0
    divs.l  d0,d1
}

__relocate_code__
__interrupt__ 
void
dtim0_handler(void)
{
    MCF_DTIM0_DTMR = 0;
    MCF_DTIM0_DTER = MCF_DTIM_DTER_REF;

    /* Raise the system clock */
    MCF_CLOCK_LPDR = 0;
    
    /* Turn on the peripheral clocks */ 
    MCF_SCM_PPMRC = MCF_SCM_PPMRC_ENABLE_CFM;
    asm( moveq.l #0x21,D0);
    asm( movec   d0,RAMBAR0);
    
    /* Raise the system clock further */
    clock_pll(REF_CLK_KHZ, sys_clk_khz, 0);
}

__relocate_code__
void
sleep (void)
{
    /* Disable the PLL */
    clock_pll(REF_CLK_KHZ, 0, PLL_DISABLE);
        
    /* Disable FLASHBAR */
    asm( moveq.l #0,D0);
    asm( movec   d0,RAMBAR0);
    MCF_SCM_PPMRS = MCF_SCM_PPMRS_DISABLE_CFM;
    
    /* Maximize the low-power divider */
    MCF_CLOCK_LPDR = 0xF;

    /* Start the timer */
    MCF_DTIM0_DTMR |= MCF_DTIM_DTMR_RST;
        
    /* Execute the stop instruction */
    stop_2000();
}


/*********************************************************************/

/***********************************************************************/
void presskey(void)
{
  int ch ;
  
  printf("press any key to continue   \n"); 
  ch = (int)in_char();   
  
     	
}


/***********************************************************************/






/*********************************************************************/

void
cmd_rtc_en (int argc, char **argv)
{
 
  printf("Enabling RTC clock   \n");
  configure_RTC();
      

}
/********************************************************************/


/***********************************************************************/
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
            sys_clk_khz = (clock_pll(REF_CLK_KHZ, sysclk, PLL_DISABLE) /
                           clock_lpd(0));
        }
        else
        {
            /* Set the PLL to the desired system clock */
            sys_clk_khz = (clock_pll(REF_CLK_KHZ, sysclk, 0) /
                           clock_lpd(0));
        }

        /* Re-init the UART with the new system clock setting */
        uart_init(TERMINAL_PORT, sys_clk_khz, baud);

        printf("System Clock: %d KHz\n", sys_clk_khz);
    }

    /* Show */
    else
        printf("%d KHz", sys_clk_khz);
}
/********************************************************************/
void
setshow_clkout (int argc, char **argv)
{
    /* Set */
    if (argv[2] != NULL)
    {
        if (strcasecmp(argv[2], "on") == 0)
        {
            clkout = ON;
            clock_clkout_enable();
            cpu_pstddata_enable();
        }
        else if (strcasecmp(argv[2], "off") == 0)
        {
            clkout = OFF;
            clock_clkout_disable();
            cpu_pstddata_disable();
        }
        else
            printf(INVALUE,argv[2]);
    }
    /* Show */
    else
    {
        if (clkout == ON)
            printf("ON");
        else
            printf("OFF");
    }
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
        if (success == 0)
        {
            printf(INVALUE,argv[2]);
            return;
        }

        sys_clk_khz = clock_pll(REF_CLK_KHZ, 0, 0) / clock_lpd(lpd);

        /* Re-init the UART with the new system clock setting */
        uart_init(TERMINAL_PORT, sys_clk_khz, baud);
    }

    /* Show */
    else
        printf("%d", clock_lpd(0));
}
/********************************************************************/
void
setshow_stpmd (int argc, char **argv)
{
    int success, temp;

    /* Set */
    if (argv[2] != NULL)
    {
        temp = (uint8) get_value(argv[2],&success,10);
        if (success == 0)
        {
            printf(INVALUE,argv[2]);
            return;
        }

        if ((temp > 3) || (temp < 0))
        {
            printf(INVALUE,argv[2]);
            return;
        }

        stpmd = temp;
    }

    /* Show */
    else
        printf("b'%02b",stpmd);
}
/********************************************************************/
void
setshow_lvdse (int argc, char **argv)
{
    int success, temp;

    /* Set */
    if (argv[2] != NULL)
    {
        temp = (uint8) get_value(argv[2],&success,10);
        if (success == 0)
        {
            printf(INVALUE,argv[2]);
            return;
        }

        if ((temp > 1) || (temp < 0))
        {
            printf(INVALUE,argv[2]);
            return;
        }

        lvdse = temp;
    }

    /* Show */
    else
        printf("b'%b",lvdse);
}
/********************************************************************/
void
setshow_xipl (int argc, char **argv)
{
    int success, temp;

    /* Set */
    if (argv[2] != NULL)
    {
        temp = (uint8) get_value(argv[2],&success,10);
        if (success == 0)
        {
            printf(INVALUE,argv[2]);
            return;
        }

        if ((temp > 7) || (temp < 0))
        {
            printf(INVALUE,argv[2]);
            return;
        }

        xipl = temp;
    }

    /* Show */
    else
        printf("b'%03b",xipl);
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
void
setshow_step (int argc, char **argv)
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
        clk_step = temp;
    }

    /* Show */
    else
        printf("%d", clk_step);
}
/********************************************************************/
void
setshow_pwrcfg (int argc, char **argv)
{
    /* Set */
    if (argv[2] != NULL)
    {
        if (strcasecmp(argv[2], "min") == 0)
        {
            tpwrcfg = PWR_MIN;
        }
        else if (strcasecmp(argv[2], "max") == 0)
        {
            tpwrcfg = PWR_MAX;
        }
        else if (strcasecmp(argv[2], "typ") == 0)
        {
            tpwrcfg = PWR_TYP;
        }
        else
        {
            printf(INVALUE,argv[2]);
            return;
        }
    }

    /* Show */
    else
    {
        switch (tpwrcfg)
        {
            case PWR_MIN:
                printf("MIN");
                break;
            case PWR_TYP:
                printf("TYP");
                break;
            case PWR_MAX:
            default:
                printf("MAX");
        }
    }
}
/********************************************************************/
void
min_pwr_config(void)
{
    /*
     * Put clock gating into the "minimum functional" configuration
     */
    MCF_SCM_PPMRH = 0
        | MCF_SCM_PPMRH_CDPORTS
    /*  | MCF_SCM_PPMRH_CDEPORT */
        | MCF_SCM_PPMRH_CDPIT0 
        | MCF_SCM_PPMRH_CDPIT1 
        | MCF_SCM_PPMRH_CDADC  
        | MCF_SCM_PPMRH_CDGPT  
        | MCF_SCM_PPMRH_CDPWM  
        | MCF_SCM_PPMRH_CDFCAN 
    /*  | MCF_SCM_PPMRH_CDCFM   */;

    MCF_SCM_PPMRL = 0
    /*  | MCF_SCM_PPMRL_CDG     */
        | MCF_SCM_PPMRL_CDEIM  
        | MCF_SCM_PPMRL_CDDMA  
        | MCF_SCM_PPMRL_CDUART0
        | MCF_SCM_PPMRL_CDUART1
        | MCF_SCM_PPMRL_CDUART2
        | MCF_SCM_PPMRL_CDI2C  
        | MCF_SCM_PPMRL_CDQSPI 
        | MCF_SCM_PPMRL_CDDTIM0
        | MCF_SCM_PPMRL_CDDTIM1
        | MCF_SCM_PPMRL_CDDTIM2
        | MCF_SCM_PPMRL_CDDTIM3
    /*  | MCF_SCM_PPMRL_CDINTC0 */;

    /* Disable the CLKOUT and PST/DDATA signals */
    clock_clkout_disable();
    cpu_pstddata_disable();

    if (running_in_sram)
    {
        /* Disable FLASHBAR */
        mcf5xxx_wr_rambar0(0);
        /* Disable CFM clock */
        MCF_SCM_PPMRS = MCF_SCM_PPMRS_DISABLE_CFM;
    }
    else
    {
        /* Disable SRAM instruction accesses */
        mcf5xxx_wr_rambar1(SRAM_ADDRESS + 0x35);
    }
    cpwrcfg = PWR_MIN;
}
/********************************************************************/
void
typ_pwr_config(void)
{
    /*
     * Put clock gating into a "typical" configuration
     */
    MCF_SCM_PPMRH = 0
        | MCF_SCM_PPMRH_CDPORTS
    /*  | MCF_SCM_PPMRH_CDEPORT */
        | MCF_SCM_PPMRH_CDPIT0 
        | MCF_SCM_PPMRH_CDPIT1 
        | MCF_SCM_PPMRH_CDADC  
        | MCF_SCM_PPMRH_CDGPT  
        | MCF_SCM_PPMRH_CDPWM  
        | MCF_SCM_PPMRH_CDFCAN 
    /*  | MCF_SCM_PPMRH_CDCFM   */;

    MCF_SCM_PPMRL = 0
    /*  | MCF_SCM_PPMRL_CDG     */
        | MCF_SCM_PPMRL_CDEIM  
        | MCF_SCM_PPMRL_CDDMA  
    /*  | MCF_SCM_PPMRL_CDUART0 */
        | MCF_SCM_PPMRL_CDUART1
        | MCF_SCM_PPMRL_CDUART2
        | MCF_SCM_PPMRL_CDI2C  
        | MCF_SCM_PPMRL_CDQSPI 
        | MCF_SCM_PPMRL_CDDTIM0
        | MCF_SCM_PPMRL_CDDTIM1
        | MCF_SCM_PPMRL_CDDTIM2
        | MCF_SCM_PPMRL_CDDTIM3
    /*  | MCF_SCM_PPMRL_CDINTC0 */;

    /* Disable the CLKOUT and PST/DDATA signals */
    clock_clkout_disable();
    cpu_pstddata_disable();

    if (running_in_sram)
    {
        /* Disable FLASHBAR */
        mcf5xxx_wr_rambar0(0);
        /* Disable CFM clock */
        MCF_SCM_PPMRS = MCF_SCM_PPMRS_DISABLE_CFM;
    }
    else
    {
        /* Disable SRAM instruction accesses */
        mcf5xxx_wr_rambar1(SRAM_ADDRESS + 0x35);
    }
    cpwrcfg = PWR_TYP;
}
/********************************************************************/
void
max_pwr_config(void)
{
    uint16 data_read,i ;
    /* Restore to default configuration with all clocks, etc. on */
    MCF_SCM_PPMRC = MCF_SCM_PPMRC_ENABLE_ALL;
    clock_clkout_enable();
    cpu_pstddata_enable();
    mcf5xxx_wr_rambar0(FLASH_ADDRESS + 0x21);
    mcf5xxx_wr_rambar1(SRAM_ADDRESS + 0x21);
    cpwrcfg = PWR_MAX;
   
    
    
    
}
/********************************************************************/
void
set_pwr_config(int cfg)
{
    switch (cfg)
    {
        case PWR_MIN:
            min_pwr_config();
            break;
        case PWR_TYP:
            typ_pwr_config();
            break;
        case PWR_MAX:
        default:
            max_pwr_config();
    }
}
/********************************************************************/
