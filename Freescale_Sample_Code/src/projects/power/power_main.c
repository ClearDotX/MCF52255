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

/********************************************************************/
/*
 * Setup user interface
 */
const char BANNER[] = "\nPower Measurement Utility\n";
const char PROMPT[] = "PWR> ";

UIF_CMD UIF_CMDTAB[] =
{
    UIF_CMDS_ALL
    {"clk", 0,1,0,cmd_sysclk,"Set PLL output clock","<sys_clk>"},
    {"stop",0,0,0,cmd_stop,  "STOP mode (lowest power mode)", ""},
    {"wait",0,0,0,cmd_wait,  "WAIT low-power mode", ""},
    {"doze",0,0,0,cmd_doze,  "DOZE low-power mode", ""},
    {"run", 0,0,0,cmd_run,   "RUN low-power mode", ""},
    {"loop", 0,1,0,cmd_loop, "Enter loop", "<one|nop|tpf|poll>"},
    {"rd",0,0,0,cmd_rd,      "Register Disply",""},
    {"ppm", 0,2,0,cmd_ppm,   "Periph Pwr Mngt tests",""},
    {"crunch", 0,0,0,cmd_app1,   "Number Crunching Application",""},
    {"wakeup", 0,0,0,cmd_app2,   "Sleep/wakeup Application",""},
};

UIF_SETCMD UIF_SETCMDTAB[] =
{
    {"sysclk", 0,1,setshow_sysclk,""},
    {"clkout", 0,1,setshow_clkout,""},
    {"lpd", 0,1,setshow_lpd,""},
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
int stpmd     = 2;
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

    /*
     * Level-sensitivity needs to be used on the IRQx pins to bring
     * the processor out of stop mode
     */
    MCF_EPORT_EPPAR = 0
        | MCF_EPORT_EPPAR_EPPA4_RISING
        | MCF_EPORT_EPPAR_EPPA5_RISING
        | MCF_EPORT_EPPAR_EPPA7_LEVEL;

    /* Set the interrupt handlers in the vector table */
    mcf5xxx_set_handler(64 + 1, (ADDRESS)sw2_handler);
    mcf5xxx_set_handler(64 + 5, (ADDRESS)sw1_handler);
    mcf5xxx_set_handler(64 + 7, (ADDRESS)abort_handler);

    /* Enable the "maximum" power cnofiguration by default */
    max_pwr_config();

    printf("\n");
    printf("**************************************************\n");
    printf("*                                                *\n");
    printf("*           Power Measurement Utility            *\n");
    printf("*                                                *\n");
    printf("**************************************************\n");
    printf("\nEnter all clock settings in KHz\n");
    printf("Press ABORT button to exit a loop or low-power modes\n");
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
/********************************************************************/
__relocate_code__
__interrupt__ void
abort_handler(void)
{
	printf("In abort\n");
    /* Wait for the switch to deassert */
    while (!(MCF_EPORT_EPPDR & MCF_EPORT_EPPDR_EPPD7)) {};

    abort_flag = TRUE;

    if (exit_loop == TRUE)
    {
        exit_loop = FALSE;

        /*
         * Exit loop by jumping to mainloop()
         * This leaves stack frames on the stack and could
         * eventually result in stack overflow
         */
        max_pwr_config();
        mainloop();
    }
    if (exit_low_power)
    {
        exit_low_power = FALSE;
        
        /* 
         * Raise the power level to max power 
         */
         
        /* Turn on all the peripheral clocks */ 
        MCF_SCM_PPMRC = MCF_SCM_PPMRC_ENABLE_ALL;
        
        /* Turn on the Flash and SRAM BARs */
        asm( moveq.l #0x21,D0);
        asm( movec   d0,RAMBAR0);
        mcf5xxx_wr_rambar1(SRAM_ADDRESS + 0x21);
        
        /* Enable CLKOUT and BDM signals */
        clock_clkout_enable();
        cpu_pstddata_enable();

        cpwrcfg = PWR_MAX;
    }

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
        | MCF_PMM_LPICR_XLPM_IPL(xipl)
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

/********************************************************************/
void
cmd_stop(int argc, char **argv)
{
    printf("Entering STOP mode\n");
    printf("    STPMD = %02b\n",stpmd);
    printf("    LVDSE = %b\n",lvdse);
    printf("    XIPL  = %03b\n",xipl);

    MCF_PMM_LPCR = 0
        | MCF_PMM_LPCR_LPMD_STOP
        | MCF_PMM_LPCR_STPMD(stpmd);

    printf("Disabling relaxation oscillator input to BWDT    \n");
    MCF_CLOCK_BWCR = 0x00 ;
    
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
cmd_loop (int argc, char **argv)
{
    if (argc == 1)
    {
        printf("Enter an argument:\n");
        printf("   \"one\" = while(1) loop\n");
        printf("   \"nop\"  = nop loop\n");
        printf("   \"tpf\"  = tpf loop\n");
        printf("   \"poll\" = poll uart\n");
    }
    else
    {
        /* Wait for the UART to finish transmitting */
        while(!(MCF_UART_UCSR(TERMINAL_PORT) & MCF_UART_USR_TXEMP)) {};

        set_pwr_config(tpwrcfg);

        /* Enter the desired loop */
        if (strcasecmp(argv[1], "one") == 0)
        {
            /* Tell the abort handler to go to the mainloop */
            exit_loop = TRUE;

            while (1)
            {
                /* null */
            }
        }
        else if (strcasecmp(argv[1], "nop") == 0)
        {
            /* Tell the abort handler to go to the mainloop */
            exit_loop = TRUE;

            while (1)
            {
                nop();
                nop();
                nop();
                nop();
            }
        }
        else if (strcasecmp(argv[1], "tpf") == 0)
        {
            /* Tell the abort handler to go to the mainloop */
            exit_loop = TRUE;

            while (1)
            {
                tpf();
                tpf();
                tpf();
                tpf();
            }
        }
        else if (strcasecmp(argv[1], "poll") == 0)
        {
            while (!(MCF_UART_USR(TERMINAL_PORT) & MCF_UART_USR_RXRDY))
            {
                /* null */
            }

            /* Clear the UART FIFO */
            uart_getchar(TERMINAL_PORT);
        }
        else
        {
            printf(INVALUE,argv[1]);
        }

        max_pwr_config();
    }
}
/********************************************************************/
void
cmd_rd (int argc, char **argv)
{
    printf("SYNCR    = %#04X\n", MCF_CLOCK_SYNCR);
    printf("SYNSR    = %#02X\n", MCF_CLOCK_SYNSR);
    printf("LPCR     = %#02X\n", MCF_PMM_LPCR);
    printf("LPICR    = %#02X\n", MCF_PMM_LPICR);
    printf("PPMRH    = %#08X\n", MCF_SCM_PPMRH);
    printf("PPMRL    = %#08X\n", MCF_SCM_PPMRL);
    printf("BWCR     = %#08X\n", MCF_CLOCK_BWCR);
}   
/********************************************************************/
void
cmd_ppm (int argc, char **argv)
{
    /*
     * Usage:
     *
     * ppm <gated_clk>
     *
     * Reference the RM for gated clock assignments in the PPMRH/L
     * The core is always active and running the same loop.  Therefore,
     * a baseline power measurement can be taken with all peripherals
     * off and then the consumption of each peripheral can be determined
     * by enabling them individually.
     *
     * 0 is an invalid peripheral number and should be used to
     * turn off all the peripherals
     *
     * If no gated_clk is specified, the ABORT button can be used to
     * cycle through a loop which individually enables all the valid
     * gated clocks.
     *
     * The only valid image target is in the internal SRAM. Since this
     * test will turn off the CFM (unless it is the specified peripheral).
     */
    int success, i = 0;

    if (!running_in_sram)
    {
        printf("This test is only valid if running from SRAM\n");
        return;
    }
    /* Disable FLASHBAR */
    mcf5xxx_wr_rambar0(0);

    if (argc == 2)
    {
        i = get_value(argv[1],&success,10);
        if (success == 0)
        {
            printf(INVALUE,argv[1]);
            return;
        }
    }


    /* Disable the CLKOUT and PST/DDATA signals */
    clock_clkout_disable();
    cpu_pstddata_disable();

    /*
     * Test with just the specified clock enabled.  The INTC will be
     * disabled unless it is the one specified, so the ABORT button
     * will not be able to be used to exit the test.
     */
    if (argc >= 2)
    {
        int test;

        printf("Peripheral #%d\n", i);
        if (argc == 3)
        {
            test = get_value(argv[2],&success,10);
            if (!success)
            {
                printf(INVALUE,argv[2]);
                return;
            }
        }

        /* Wait for the UART to finish transmitting */
        while(!(MCF_UART_UCSR(TERMINAL_PORT) & MCF_UART_USR_TXEMP)) {};

        if (argc == 3)
        {
            switch (test)
            {
                case 0: /* 244 Hz */
                    clock_pll(0, 0, PLL_DISABLE);
                    clock_lpd(32768);
                    break;
                case 1: /* 16 MHz */
                    clock_pll(REF_CLK_KHZ, 16000, 0);
                    break;
                case 2: /* 32 MHz */
                    clock_pll(REF_CLK_KHZ, 32000, 0);
                    break;
                case 3: /* 48 MHz */
                    clock_pll(REF_CLK_KHZ, 48000, 0);
                    break;
                case 4: /* 64 MHz */
                    clock_pll(REF_CLK_KHZ, 64000, 0);
                    break;
                case 5: /* 80 MHz */
                    clock_pll(REF_CLK_KHZ, 80000, 0);
                    break;
                default:
                    printf(INVALUE,argv[2]);
                    return;
            }
        }

        if (i != 99)
        {
            MCF_SCM_PPMRS = MCF_SCM_PPMRS_DISABLE_ALL;
            MCF_SCM_PPMRC = MCF_SCM_PPMRC_CLEAR_CDG;
            MCF_SCM_PPMRC = i;
        }
        abort_flag = FALSE;
        while (abort_flag == FALSE)
        {
            nop();
        }
    }

    /*
     * If no argument was passed in, then cycle through all the valid
     * ones. The INTC is always enabled so that the ABORT button can
     * be used to cycle through the valid gated clocks.  A baseline
     * should be taken with just the core and with the core plus the
     * INTC so that the INTC power can be subtracted from the power
     * measured here.
     */
    else
    {
        int test;
        int8 clocks[] = { 0,  4,  5,  6,  7,  9, 10, 13, 14, 15, 16,
                         32, 35, 36, 39, 40, 41, 42, 43, 99};

        for (test = 0; test < 6; test++)
        {
            printf("-- Test %d --\n", test);
            for (i = 0; i < sizeof(clocks); i++)
            {
                printf("Gated Clock #%d\n", clocks[i]);
                /* Wait for the UART to finish transmitting */
                while(!(MCF_UART_UCSR(TERMINAL_PORT) & MCF_UART_USR_TXEMP)) {};

                switch (test)
                {
                    case 0: /* 244 Hz */
                        clock_pll(0, 0, PLL_DISABLE);
                        clock_lpd(32768);
                        break;
                    case 1: /* 16 MHz */
                        clock_pll(REF_CLK_KHZ, 16000, 0);
                        break;
                    case 2: /* 32 MHz */
                        clock_pll(REF_CLK_KHZ, 32000, 0);
                        break;
                    case 3: /* 48 MHz */
                        clock_pll(REF_CLK_KHZ, 48000, 0);
                        break;
                    case 4: /* 64 MHz */
                        clock_pll(REF_CLK_KHZ, 64000, 0);
                        break;
                    case 5: /* 80 MHz */
                        clock_pll(REF_CLK_KHZ, 80000, 0);
                        break;
                }
                if (clocks[i] != 99)
                {
                    min_pwr_config();
                    MCF_SCM_PPMRC = clocks[i];
                }

                abort_flag = FALSE;
                while (abort_flag == FALSE)
                {
                    nop();
                }

                clock_lpd(1);
                clock_pll(REF_CLK_KHZ, sys_clk_khz, 0);
                max_pwr_config();
            }
        }
    }
}
/********************************************************************/
/*
 * Unsigned 64 bit multiply
 */
inline asm void
unsigned_multiply(void)
{
    move.l  d2, -(sp)
    move.l  #0, acc0         

    move.l  #0x80000001, d1
    move.l  #0x80000001, d0

	/*Form upper half of product and check for negative first operand*/
    mac.l   d0, d1, acc0      
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
    move.l  acc0, d0
    
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

/********************************************************************/
void
cmd_app1 (int argc, char **argv)
{
    /*
     * Sample application that exercises the MAC unit.  This command
     * executes some math routines for a fixed number of iterations.
     * The runtime is measured by a timer and printed upon completion..
     */
     
    int i, j, pwrcfg, cycles;

    /* Save off current power configuration setting */
    pwrcfg = cpwrcfg;

    /* Print this message now before the power configuration is changed */
    printf("Press the ABORT button on the board to start\n");
    
    /* Wait for the UART to finish transmitting */
    while(!(MCF_UART_UCSR(TERMINAL_PORT) & MCF_UART_USR_TXEMP)) {};
   
    /* Enable the min config setting */
    min_pwr_config();

    /*
     * Configure DMA Timer 0 as a counter to measure the runtime
     */
    /* Turn on clock to DMA Timer 0 */
    MCF_SCM_PPMRC = MCF_SCM_PPMRC_ENABLE_DTIM0;

    /* Enable DMA Timer 0 */
    MCF_DTIM0_DTMR = 0
        | MCF_DTIM_DTMR_PS(0)
        | MCF_DTIM_DTMR_ORRI
        | MCF_DTIM_DTMR_OM
        | MCF_DTIM_DTMR_CLK_DIV1;
            
    /* Put the MAC unit in fraction mode */
    asm( move.l  #0x20, macsr); 
    
    /* 
     * Wait for "Start" indication on IRQ7/Abort
     */
    abort_flag = FALSE;
    while(abort_flag == FALSE)
    {};
        
    for (j = 0; j < APP1_OUTER_ITERATIONS; j++)
    {
        if (j == 0)
        {
            /* Start the timer */
            MCF_DTIM0_DTMR |= MCF_DTIM_DTMR_RST;
        }
        
        /* Run the tests */
        for (i = 0; i < APP1_INNER_ITERATIONS; i++)
        {
            unsigned_multiply();
            signed_divide();
        }
        
        if (j == 0)
        {
            /* Read the counter */
            cycles = MCF_DTIM0_DTCN;
            /* Disable DMA Timer 0 */
            MCF_DTIM0_DTMR = 0;
        }
        /*
         * The counters are done in this way because the Atmel 
         * timers don't have the ability to count the entire 
         * loop without having to manage overflows. This 
         * matches the loops done on the other two MCUs.
         */
    }

    /* Restore the power configuration */
    set_pwr_config(pwrcfg);
    
    printf("Cycles: %d\n", cycles*APP1_OUTER_ITERATIONS);
}
/********************************************************************/
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

void
cmd_app2 (int argc, char **argv)
{
    /*
     * Sample application that exercises the MAC unit.  This command
     * goes into low-power stop mode and wakes up every second to execute
     * a fixed number of multiplication and division operations.
     */
     
    int i, pwrcfg, usecs;
    int active_tmr, asleep_tmr;

    /* Save off current power configuration setting */
    pwrcfg = cpwrcfg;

    printf("Running Sleep/Wakeup Test\n");
    printf("Press the ABORT button on the board to stop\n");
    
    /* Wait for the UART to finish transmitting */
    while(!(MCF_UART_UCSR(TERMINAL_PORT) & MCF_UART_USR_TXEMP)) {};

    /* 
     * Make sure instructions can be fetched from SRAM so the abort 
     * handler can run.
     */
    mcf5xxx_wr_rambar1(SRAM_ADDRESS + 0x21);

    /* Disable the CLKOUT and PST/DDATA signals */
   // clock_clkout_disable();
    cpu_pstddata_disable();

    /* Turn off all clocks except CFM, EPORT, INTC, and DTIM0 */
    MCF_SCM_PPMRH = 0
    /*	| MCF_SCM_PPMRH_CDGPIO   */
    /*  | MCF_SCM_PPMRH_CDEPORT  */
        | MCF_SCM_PPMRH_CDPIT0 
        | MCF_SCM_PPMRH_CDPIT1 
        | MCF_SCM_PPMRH_CDADC  
        | MCF_SCM_PPMRH_CDGPT  
        | MCF_SCM_PPMRH_CDPWM  
    /*  | MCF_SCM_PPMRH_CDCFM */
        ;
          
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
  
    /* Setup the low-power mode */
    MCF_PMM_LPCR = MCF_PMM_LPCR_LPMD_DOZE;

    /* Set DTIM0 interrupt level and priority */
    MCF_INTC0_ICR(19) = 0
        | MCF_INTC_ICR_IP(7)
        | MCF_INTC_ICR_IL(7);
    
    /* Enable DTIM0 interrupts in the interrupt controller */
    MCF_INTC0_IMRL &= ~(0
        | MCF_INTC_IMRL_INT_MASK19 
        | MCF_INTC_IMRL_MASKALL);
 
    /* Put the DTIM0 interrupt handler in the vector table */
    mcf5xxx_set_handler(64 + 19, (ADDRESS)dtim0_handler);
    
    /* Enable interrupts to the CF core */
    mcf5xxx_irq_enable();
        
    /* Enable TIN0 as a GPIO output */
    MCF_GPIO_PTCPAR = MCF_GPIO_PTCPAR_DTIN0_GPIO;
    MCF_GPIO_PORTTC = 1;
    MCF_GPIO_DDRTC = MCF_GPIO_DDRTC_DDRTC0;

    /* Calculate the prescaler for a 1us timer tick period */
    active_tmr = 0
        | MCF_DTIM_DTMR_PS(sys_clk_khz / 1000)
        | MCF_DTIM_DTMR_OM
        | MCF_DTIM_DTMR_CLK_DIV1
        | MCF_DTIM_DTMR_RST;
        
    /* 
     * While inactive, the clock will be as low as possible and tick will
     * be a single clock cycle.  The interrupt is enambled to wake us out
     * of doze mode.
     */
    asleep_tmr = 0
        | MCF_DTIM_DTMR_PS(0)
        | MCF_DTIM_DTMR_ORRI
        | MCF_DTIM_DTMR_OM
        | MCF_DTIM_DTMR_CLK_DIV1;

    /* Enable the Ports module so we can toggle a GPIO */
    MCF_SCM_PPMRC = MCF_SCM_PPMRC_ENABLE_PORTS;
    
    /* Put the MAC unit in fraction mode */
    asm( move.l  #0x20, macsr); 
    
    exit_low_power = TRUE;
    abort_flag = FALSE;
    while(abort_flag == FALSE)
    {   
        /* Toggle TIN0 output pin */
        MCF_GPIO_PORTTC ^= 1;
	
        /* Start the timer */
        MCF_DTIM0_DTMR = active_tmr;

        /* Do the work */
        for (i = 0; i < APP2_ITERATIONS; i++)
        {
            unsigned_multiply();
            signed_divide();
        }

        /* 
         * Calculate the time already spent and subtract it from the reference
         * needed to wake up every one second with a timer clocked by the 
         * slowest system clock possible.
         */
        usecs = MCF_DTIM0_DTCN;
        MCF_DTIM0_DTMR = 0;
        MCF_DTIM0_DTMR = (uint16)asleep_tmr;
        MCF_DTIM0_DTRR = LOWEST_SYS_CLK - ((usecs * LOWEST_SYS_CLK) / 1000000) - OVERHEAD_CONST;

        sleep();
        /* The CPU will wake up when the DTIM0 interrupt fires */
    }

    /* Restore the power configuration */
    set_pwr_config(pwrcfg);
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
    /*  | MCF_SCM_PPMRH_CDGPIO  */
    /*  | MCF_SCM_PPMRH_CDEPORT */
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
   /*   | MCF_SCM_PPMRH_CDGPIO  */
    	| MCF_SCM_PPMRH_CDEPORT 
        | MCF_SCM_PPMRH_CDPIT0 
        | MCF_SCM_PPMRH_CDPIT1 
        | MCF_SCM_PPMRH_CDADC  
        | MCF_SCM_PPMRH_CDGPT  
        | MCF_SCM_PPMRH_CDPWM  
  /*    | MCF_SCM_PPMRH_CDCFM  */
        ;  
          
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
    /*  | MCF_SCM_PPMRL_CDDTIM0   */
    /*  | MCF_SCM_PPMRL_CDDTIM1   */ 
    /*  | MCF_SCM_PPMRL_CDDTIM2   */ 
    /*  | MCF_SCM_PPMRL_CDDTIM3	  */
	/*	| MCF_SCM_PPMRL_CDINTC0   */	
	/*	| MCF_SCM_PPMRL_CDINTC1   */
		| MCF_SCM_PPMRL_CDFEC      
         ;     
     
  

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
