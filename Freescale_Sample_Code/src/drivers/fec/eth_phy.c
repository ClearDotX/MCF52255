/*!
 * \file    eth_phy.c
 * \brief   Ethernet Physical Layer Interface Driver
 * \version $Revision: 1.1 $
 * \author  Michael Norman
 * 
 * This is a generic driver for all Ethernet PHYs with the basic MII registers
 */

#include "common.h"
#include "mii.h"
#include "eth_phy.h"

/* Variable to save off auto-negotiate settings */
int eth_phy_anar = 0
    | PHY_ANAR_100BTX_FDX
    | PHY_ANAR_100BTX
    | PHY_ANAR_10BT_FDX
    | PHY_ANAR_10BT;

/********************************************************************/
/*!
 * \brief   Enable the Ethernet PHY in auto-negotiate mode
 * \param   phy_addr    Address of the PHY
 * \param   speed       Desired speed (MII_10BASE_T or MII_100BASE_TX)
 * \param   duplex      Desired duplex (MII_FDX or MII_HDX)
 * \return  0 if successful; non-zero otherwise
 */
int
eth_phy_autoneg(int phy_addr, MII_SPEED speed, MII_DUPLEX duplex)
{
    int timeout;
    uint16 settings;

    /* Reset the PHY */
    if (mii_write(phy_addr, PHY_BMCR, PHY_BMCR_RESET))
        return 1;

    /* Read back the contents of the CTRL register and verify
     * that RESET is not set - this is a sanity check to ensure
     * that we are talking to the PHY correctly. RESET should
     * always be cleared. */
     for (timeout =0; timeout < MII_LINK_TIMEOUT; timeout++)
	{
		asm
		{
			nop;	//wait 500 us here	
		}
	}
    if (mii_read(phy_addr, PHY_BMCR, &settings) || settings & PHY_BMCR_RESET)
        return 1;

    /* Set the Auto-Negotiation Advertisement Register */
    if (speed == MII_10BASE_T)
    {
        settings = (duplex == MII_FDX) 
            ? PHY_ANAR_10BT_FDX | PHY_ANAR_10BT 
            : PHY_ANAR_10BT;
    }
    else /* (speed == MII_100BASE_TX) */
    {
        settings = (duplex == MII_FDX)  
            ? PHY_ANAR_100BTX_FDX   | 
              PHY_ANAR_100BTX       | 
              PHY_ANAR_10BT_FDX     | 
              PHY_ANAR_10BT
            : PHY_ANAR_10BT_FDX     | 
              PHY_ANAR_10BT;
    }
    
    /* Save off the settings we just advertised */
    eth_phy_anar = settings;
    
    if (mii_write(phy_addr, PHY_ANAR, settings))
        return 1;
        
    /* Enable Auto-Negotiation */
    if (mii_write(phy_addr, PHY_BMCR, PHY_BMCR_AN_ENABLE | PHY_BMCR_AN_RESTART))
        return 1;
	for (timeout =0; timeout < 100*MII_LINK_TIMEOUT; ++timeout)
	{
		asm
		{
			nop;	//wait 500 us here	
		}
	}
    /* Wait for auto-negotiation to complete */
    for (timeout = 0; timeout < MII_LINK_TIMEOUT; ++timeout)
    {
       // if (mii_read(phy_addr, PHY_BMSR, &settings))
        //   return 1;
        mii_read(phy_addr, PHY_BMSR, &settings);
        if (settings & PHY_BMSR_AN_COMPLETE)
            break;
    }
    /* Read the BMSR one last time */
    if (mii_read(phy_addr, PHY_BMSR, &settings))
        return 1;
    if (timeout == MII_LINK_TIMEOUT || !(settings & PHY_BMSR_LINK))
        return 1;
    else
        return 0;
}
/********************************************************************/
/*!
 * \brief   Enable the Ethernet PHY in manual mode
 * \param   phy_addr    Address of the PHY
 * \param   speed       Desired speed (MII_10BASE_T or MII_100BASE_TX)
 * \param   duplex      Desired duplex (MII_FDX or MII_HDX)
 * \param   loop        Put PHY in loopback mode?
 * \return  0 if successful; non-zero otherwise
 */
int 
eth_phy_manual(int phy_addr, MII_SPEED speed, MII_DUPLEX duplex, int loop)
{
    int timeout;
    uint16 settings = 0;
  
    /* Reset the PHY */
    if (mii_write(phy_addr, PHY_BMCR, PHY_BMCR_RESET))
        return 1;
    
    /* Read back the contents of the CTRL register and verify
     * that RESET is not set - this is a sanity check to ensure
     * that we are talking to the PHY correctly. RESET should
     * always be cleared. */
    if (mii_read(phy_addr, PHY_BMCR, &settings) || (settings & PHY_BMCR_RESET))
        return 1;
    
    if (loop)
        settings |= PHY_BMCR_LOOP;
    if (duplex == MII_FDX)
        settings |= PHY_BMCR_FDX;
    if (speed == MII_100BASE_TX)
        settings |= PHY_BMCR_SPEED;

    if (mii_write(phy_addr, PHY_BMCR, settings))
        return 1;
    
    /* Wait for link */
    for (timeout = 0; timeout < MII_LINK_TIMEOUT; ++timeout)
    {
        if (mii_read(phy_addr, PHY_BMSR, &settings))
            return 1;
        if (settings & PHY_BMSR_LINK)
            break;
    }

    if (timeout == MII_LINK_TIMEOUT || !(settings & PHY_BMSR_LINK))
        return 1;
    else
        return 0;
}
/********************************************************************/
/*!
 * \brief   Get the auto-negotiated speed
 * \param   phy_addr    Address of the PHY
 * \param   speed       Pointer where speed data is stored
 * \return  0 if successful; non-zero otherwise
 */
int 
eth_phy_get_speed(int phy_addr, int *speed)
{
    int timeout;
    uint16 settings = 0;

    /* Get Link Partner settings */
    for (timeout = 0; timeout < MII_TIMEOUT; ++timeout)
    {
        if (mii_read(phy_addr, PHY_ANLPAR, &settings))
            return 1;
    }
    if (timeout == MII_TIMEOUT)
        return 1;

    settings &= eth_phy_anar;
    if (settings & PHY_ANLPAR_100BT4     ||
        settings & PHY_ANLPAR_100BTX_FDX ||
        settings & PHY_ANLPAR_100BTX)
        *speed = MII_100BASE_TX;
    else
        *speed = MII_10BASE_T;

    return 0;
}
/********************************************************************/
/*!
 * \brief   Get the auto-negotiated duplex
 * \param   phy_addr    Address of the PHY
 * \param   speed       Pointer where speed data is stored
 * \return  0 if successful; non-zero otherwise
 */
int 
eth_phy_get_duplex(int phy_addr, int *speed)
{
    int timeout;
    uint16 settings = 0;

    /* Get Link Partner settings */
    for (timeout = 0; timeout < MII_TIMEOUT; ++timeout)
    {
        if (mii_read(phy_addr, PHY_ANLPAR, &settings))
            return 1;
    }
    if (timeout == MII_TIMEOUT)
        return 1;

    settings &= eth_phy_anar;
    if (settings & PHY_ANLPAR_100BTX_FDX ||
        settings & PHY_ANLPAR_10BTX_FDX)
        *speed = MII_FDX;
    else
        *speed = MII_HDX;

    return 0;
}
/********************************************************************/
/*!
 * \brief   Print all the MII registers (0x00-0x1F)
 * \param   phy_addr    Address of the PHY
 */
int 
eth_phy_reg_dump(int phy_addr)
{
    int j;
    uint16 settings;
    
    printf("\n    MII Register Block\n");
    printf("--------------------------------");
    for (j = 0; j < 32; j++)
    {
        mii_read(phy_addr, j, &settings);
        if (!(j % 4))
            printf("\n0x%02X-0x%02X : %04X ", j, j + 3, settings);
        else
            printf("%04X ", settings);
    }
    printf("\n");
}
/********************************************************************/
