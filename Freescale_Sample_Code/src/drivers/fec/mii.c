/*!
 * \file    mii.c
 * \brief   Media Independent Interface (MII) driver
 * \version $Revision: 1.1 $
 * \author  Michael Norman
 * 
 * \warning This driver assumes that FEC0 is used for all MII management
 *          communications.  For dual PHYs, etc., insure that FEC0_MDC and
 *          FEC0_MDIO are connected to the PHY's MDC and MDIO.
 */

#include "common.h"
#include "mii.h"

/********************************************************************/
/*!
 * \brief Write a value to a PHY's MII register.
 * 
 * \param   phy_addr    Address of the PHY
 * \param   reg_addr    Address of the register in the PHY
 * \param   data        Data to be written to the PHY register
 * \return  0 if write is successful; 1 if write times out
 *
 * mii_write() polls for the FEC's MII interrupt event (which should
 * be masked from the interrupt handler) and clears it. If after a
 * suitable amount of time the event isn't triggered, a non-zero value 
 * is returned.
 */
int 
mii_write(int phy_addr, int reg_addr, uint16 data)
{
	int timeout;

	/* Clear the MII interrupt bit */
	MCF_FEC_EIR = MCF_FEC_EIR_MII;

	/* Initiatate the MII Management write */
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
		return 1;

	/* Clear the MII interrupt bit */
	MCF_FEC_EIR = MCF_FEC_EIR_MII;

	return 0;
}
/********************************************************************/
/*!
 * \brief   Read a value from a PHY's MII register.
 * \param   phy_addr    Address of the PHY
 * \param   reg_addr    Address of the register in the PHY
 * \param   data        Pointer to location were read data will be stored
 * \return  0 if write is successful; 1 if write times out
 *
 * mii_read() polls for the FEC's MII interrupt event (which should
 * be masked from the interrupt handler) and clears it. If after a
 * suitable amount of time the event isn't triggered, a non-zero value 
 * is returned.
 */
int 
mii_read(int phy_addr, int reg_addr, uint16 *data)
{
	int timeout;

	/* Clear the MII interrupt bit */
	MCF_FEC_EIR = MCF_FEC_EIR_MII;

	/* Initiatate the MII Management read */
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
		return 1;

	/* Clear the MII interrupt bit */
	MCF_FEC_EIR = MCF_FEC_EIR_MII;

	*data = (uint16)MCF_FEC_MMFR & 0x0000FFFF;

	return 0;
}
/********************************************************************/
/*!
 * \brief   Initialize the MII interface controller
 * \param   System Clock Frequency (in MHz)
 * \warning The system clock in this case is the clock that drives
 *          the FEC logic.  This may be different from the speed at which 
 *          the CPU is operating.
 * 
 * Initialize the MII clock (EMDC) frequency. The desired MII clock is 2.5MHz:
 *
 * MII Speed Setting = System_Clock / (2.5MHz * 2)
 * (plus 1 to round up)
 */
void
mii_init(int sys_clk_mhz)
{
    MCF_FEC_MSCR = MCF_FEC_MSCR_MII_SPEED((sys_clk_mhz/5)+1);
}
/********************************************************************/
