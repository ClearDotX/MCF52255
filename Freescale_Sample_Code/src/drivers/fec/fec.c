/*
 * File:    fec.c
 * Purpose: Driver for the Fast Ethernet Controller (FEC)
 *
 * Notes:
 */

#include "common.h"
#include "fec.h"

/********************************************************************/
/*
 * Log of FEC events
 */
FEC_EVENT_LOG fec_log[2];

/*
 * Queues for FEC buffers
 */
QUEUE txbd_queue[2];
QUEUE rxbd_queue[2];
QUEUE send_queue[2];

/********************************************************************/
/*
 * Write a value to a PHY's MII register.
 *
 * Parameters:
 *  ch          FEC channel
 *  phy_addr    Address of the PHY.
 *  reg_addr    Address of the register in the PHY.
 *  data        Data to be written to the PHY register.
 *
 * Return Values:
 *  0 on failure
 *  1 on success.
 *
 * Please refer to your PHY manual for registers and their meanings.
 * mii_write() polls for the FEC's MII interrupt event and clears it. 
 * If after a suitable amount of time the event isn't triggered, a 
 * value of 0 is returned.
 */
int
fec_mii_write(int ch, int phy_addr, int reg_addr, int data)
{
    int timeout;
    uint32 eimr;

    /* Clear the MII interrupt bit */
    MCF_FEC_EIR = MCF_FEC_EIR_MII;

    /* Mask the MII interrupt */
    eimr = MCF_FEC_EIMR;
    MCF_FEC_EIMR &= ~MCF_FEC_EIMR_MII;

    /* Write to the MII Management Frame Register to kick-off the MII write */
    MCF_FEC_MMFR = 0
        | MCF_FEC_MMFR_ST_01
        | MCF_FEC_MMFR_OP_WRITE
        | MCF_FEC_MMFR_PA(phy_addr)
        | MCF_FEC_MMFR_RA(reg_addr)
        | MCF_FEC_MMFR_TA_10
        | MCF_FEC_MMFR_DATA(data);

    /* Poll for the MII interrupt (interrupt should be masked) */
    for (timeout = 0; timeout < MII_TIMEOUT; timeout++)
    {
        if (MCF_FEC_EIR & MCF_FEC_EIR_MII)
            break;
    }
    if(timeout == MII_TIMEOUT)
        return 0;

    /* Clear the MII interrupt bit */
    MCF_FEC_EIR = MCF_FEC_EIR_MII;

    /* Restore the EIMR */
    MCF_FEC_EIMR = eimr;

    return 1;
}
/********************************************************************/
/*
 * Read a value from a PHY's MII register.
 *
 * Parameters:
 *  ch          FEC channel
 *  phy_addr    Address of the PHY.
 *  reg_addr    Address of the register in the PHY.
 *  data        Pointer to storage for the Data to be read
 *              from the PHY register (passed by reference)
 *
 * Return Values:
 *  0 on failure
 *  1 on success.
 *
 * Please refer to your PHY manual for registers and their meanings.
 * mii_read() polls for the FEC's MII interrupt event and clears it. 
 * If after a suitable amount of time the event isn't triggered, a 
 * value of 0 is returned.
 */
int
fec_mii_read(int ch, int phy_addr, int reg_addr, uint16* data)
{
    int timeout;
    uint32 eimr;

    /* Clear the MII interrupt bit */
    MCF_FEC_EIR = MCF_FEC_EIR_MII;

    /* Mask the MII interrupt */
    eimr = MCF_FEC_EIMR;
    MCF_FEC_EIMR &= ~MCF_FEC_EIMR_MII;

    /* Write to the MII Management Frame Register to kick-off the MII read */
    MCF_FEC_MMFR = 0
        | MCF_FEC_MMFR_ST_01
        | MCF_FEC_MMFR_OP_READ
        | MCF_FEC_MMFR_PA(phy_addr)
        | MCF_FEC_MMFR_RA(reg_addr)
        | MCF_FEC_MMFR_TA_10;

    /* Poll for the MII interrupt (interrupt should be masked) */
    for (timeout = 0; timeout < MII_TIMEOUT; timeout++)
    {
        if (MCF_FEC_EIR & MCF_FEC_EIR_MII)
            break;
    }

    if(timeout == MII_TIMEOUT)
        return 0;

    /* Clear the MII interrupt bit */
    MCF_FEC_EIR = MCF_FEC_EIR_MII;

    /* Restore the EIMR */
    MCF_FEC_EIMR = eimr;

    *data = (uint16)(MCF_FEC_MMFR & 0x0000FFFF);

    return 1;
}
/********************************************************************/
/*
 * Initialize the MII interface controller
 *
 * Parameters:
 *  ch      FEC channel
 *  sys_clk System Clock Frequency (in MHz)
 */
void
fec_mii_init(int ch, int sys_clk)
{
    /*
     * Initialize the MII clock (EMDC) frequency
     *
     * Desired MII clock is 2.5MHz
     * MII Speed Setting = System_Clock / (2.5MHz * 2)
     * (plus 1 to make sure we round up)
     */
    MCF_FEC_MSCR = MCF_FEC_MSCR_MII_SPEED((sys_clk/5)+1);

    /*
     * Make sure the external interface signals are enabled
     */
    MCF_GPIO_PNQPAR = MCF_GPIO_PNQPAR_IRQ3_FEC_MDIO
					| MCF_GPIO_PNQPAR_IRQ5_FEC_MDC;
}

