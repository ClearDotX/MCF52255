/*
 * File:		mcf5225x_twr.c
 * Purpose:		Source to TWR-MCF5225x board specific routines
 *
 * Notes:
 * 
 * License:     All software covered by license agreement in -
 *              docs/Freescale_Software_License.pdf
 */
/********************************************************************/
#include "common.h"


/********************************************************************/
/*
 * Setup LED on the EVB
 */
void
leds_init(void)
{
    /* Enable signals as GPIO */
    MCF_GPIO_PTCPAR = 0
        | MCF_GPIO_PTCPAR_DTIN3_GPIO
        | MCF_GPIO_PTCPAR_DTIN2_GPIO
        | MCF_GPIO_PTCPAR_DTIN1_GPIO
        | MCF_GPIO_PTCPAR_DTIN0_GPIO;
    
    /* Enable signals as digital outputs */
    MCF_GPIO_DDRTC = 0
        | MCF_GPIO_DDRTC_DDRTC3
        | MCF_GPIO_DDRTC_DDRTC2
        | MCF_GPIO_DDRTC_DDRTC1
        | MCF_GPIO_DDRTC_DDRTC0;

	MCF_GPIO_PORTTC = 0x00; // TURN LEDS OFF

}

/********************************************************************/
/* 
 * Display the lower 4 bits of 'number' on the 4 LEDs connected to 
 * TIN[3:0]
 *
 *  LED: LED4 LED3 LED2 LED1
 *  PIN: TIN3 TIN2 TIN1 TIN0
 *  BIT:    3    2    1    0
 */
void
board_led_display(uint8 number)
{
    /* Set output values */
    MCF_GPIO_PORTTC = number;
}
/********************************************************************/
/*
 * These functions enable and disable the PST and DDATA BDM signals.
 *
 * Disabling PST/DDATA conserves power and reduces EMI.  Enabling these
 * signals enables real-time trace capabilities.  Some BDM interface
 * cables use the PST signals to detect when the processor is halted
 * (PST[3:0] = 1111).
 *
 * The PST/DDATA signals can only be disabled by setting the CSR[PCD]
 * bit. The CSR is only writeable via the BDM or the WDEBUG instrcution
 *
 * The PSTCLK is not disabled by setting the CSR[PCD] bit
 * Instead it is controlled by the SYNCR[DISCLK] bit. The CLOCK driver
 * provides functions to control the PSTCLK/CLKOUT signal.
 *
 * The ALLPST  signal is the logical
 * AND of PST[3:0] and is used by BDM interface cables to detect when
 * the processor is halted. The ALLPST signal is also controlled
 * by these functions.
 */

uint16 wdebug_pstddata_off[4] = {0x2C80, 0x0002, 0x0000, 0x0000};
uint16 wdebug_pstddata_on[4]  = {0x2C80, 0x0000, 0x0000, 0x0000};

void
cpu_pstddata_disable(void)
{
    mcf5xxx_exe_wdebug(wdebug_pstddata_off);
}

void
cpu_pstddata_enable(void)
{
    mcf5xxx_exe_wdebug(wdebug_pstddata_on);
}
