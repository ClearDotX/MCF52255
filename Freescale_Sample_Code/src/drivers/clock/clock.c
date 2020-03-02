/*
 * File:    clock.c
 * Purpose: Driver for the on-chip Clock module
 *
 * Notes:       
 * 
 * License:     All software covered by license agreement in -
 *              docs/Freescale_Software_License.pdf
 */

#include "common.h"
#include "clock/clock.h"

/********************************************************************/
/* Initialize the PLL
 * 
 * Parameters:
 *  fref    PLL reference clock frequency in KHz
 *  fsys    Desired PLL output frequency in KHz
 *  flags   Operating parameters
 *
 * Return Value:
 *  The resulting output system frequency
 */
int
clock_pll (int fref, int fsys, int flags)
{
    int syncr, mfd_max, mfd_min, rfd_max;
    int i, temp, fout, mfd, rfd, done;
    
    /* Check for the disable flag */
    if (flags & PLL_DISABLE)
    {
        MCF_CLOCK_SYNCR &= ~MCF_CLOCK_SYNCR_PLLEN;
        return fref;
    }
    
    /* Check bounds of reference clock */
    if((fref >= MAX_FREF) || (fref <= MIN_FREF))
        return fref;
        
    if (fsys == 0)
    {
        return (clock_speed()*1000);           
    }
    
    /* Check bounds of requested system clock */
    if (fsys > MAX_FSYS)
        fsys = MAX_FSYS;

    /* Determine maximum possible multiplier (must be even) */
    mfd_max = MAX_FVCO / fref;
    mfd_max &= ~1;
    if (mfd_max > MAX_MFD)
        mfd_max = MAX_MFD;
    
    /* Determine maximum possible output based on max multiplier */
    fout = fref * mfd_max;

    /* Determine target output based on fsys and max possible */ 
    if (fout > fsys)
        fout = fsys;
    
    /* Determine the minimum multiplier */
    for (mfd_min = mfd_max; 
         mfd_min > MIN_MFD && fout < (fref * mfd_min); 
         mfd_min-=2)
    {
    	;	
    }
         
    /* Set preliminary divider maximum */         
    rfd_max = MAX_RFD;

    /*
     * Loop across the valid MFD and RFD settings starting with
     * the max MFD and min RFD and find the closest match less-than
     * or equal-to the desired output frequency
     */
    done = FALSE;
    for (mfd = mfd_max; mfd >= mfd_min; mfd-=2)
    {
        for (rfd = MIN_RFD; rfd < rfd_max; rfd <<= 1)
        {
            temp = fref * mfd / rfd;
            
            if (fout == temp)
            {
                /* exact match */
                done = TRUE;
                break;
            }
            if (fout > temp)
            {
                /* new upper bound for the rfd */
                rfd_max = rfd;
                /* is this the nearest match? */
                if ((rfd != 1) && ((fout - temp) < (fref / (rfd>>1))))
                    done = TRUE;
                break;
            }
        }
        if (done)
            break;
     }
    if (!done)
    {
        /* 
         * Fell out of loop before finding an exact match or getting 
         * as close as possible. Adjust mfd to nearest match.
         */
        mfd += 2;
    }
    
    fout = fref * mfd / rfd;

    /* Encode MFD and RFD settings */
    mfd = (mfd - MIN_MFD) / 2;
    for (i = 0; rfd > MIN_RFD; rfd >>= 1, i++) {};
    rfd = i;

    /*
     * Temp fix for PLL bug on Rev 0
     */
    if ((MCF_CCM_CIR & 0x003F) == 0)
        (*(vuint8 *)(&__IPSBAR[0x120006])) |= 0x04;

    if (!(flags & PLL_PRETEND))
    {
        /* 
         * Initialize the PLL to generate the new system clock frequency 
         * A higher divider is used first with the desired MFD.  Once 
         * locked, the desired RFD is applied
         */
        syncr = MCF_CLOCK_SYNCR & ~(0x7700);
        temp = (MCF_CLOCK_SYNCR & 0x7000) >> 12;
        MCF_CLOCK_SYNCR = syncr 
            | MCF_CLOCK_SYNCR_RFD(rfd + 1)
            | MCF_CLOCK_SYNCR_MFD(temp)
            | MCF_CLOCK_SYNCR_PLLEN;
        MCF_CLOCK_SYNCR = syncr 
            | MCF_CLOCK_SYNCR_RFD(rfd + 1)
            | MCF_CLOCK_SYNCR_MFD(mfd)
            | MCF_CLOCK_SYNCR_PLLEN;
            
        /* Wait for the PLL to lock */  
        while (!(MCF_CLOCK_SYNSR & MCF_CLOCK_SYNSR_LOCK)) {};

        /* Finish off the initialization */
        MCF_CLOCK_SYNCR = syncr
            | MCF_CLOCK_SYNCR_RFD(rfd)
            | MCF_CLOCK_SYNCR_MFD(mfd)
            | MCF_CLOCK_SYNCR_CLKSRC
            | MCF_CLOCK_SYNCR_PLLEN;
    }

    return fout;
}
/********************************************************************/
/*
 * Initialize the Low Power Divider circuit
 * 
 * Parameters:
 *  div     Desired system frequency divider
 *
 * Return Value:
 *  The divider value applied
 *  -1 if divider was out of valid bounds
 */
int
clock_lpd (int div)
{
    int i, j;

    /* Check bounds of divider */
    if ((div < MIN_LPD) || (div > MAX_LPD))
        return (1 << (MCF_CLOCK_LPDR & 0x0F));

    /* Round divider down to nearest power of two */
    for (i = 0, j = div; j != 1; j >>= 1, i++) 
    {};
    
    /* Apply the divider to the system clock */
    MCF_CLOCK_LPDR = MCF_CLOCK_LPDR_LPD(i);

    return (1 << i);
}

/********************************************************************/
/*
 * Return the current clock speed in MHz
 * 
 * Parameters:
 *  None
 *
 * Return Value:
 *  Current clock speed in MHz
 */
int
clock_speed()
{
	int fout, fref, mfd, rfd,input_clk,divider,lpdr;
	input_clk=EXT_CLK_MHZ;
	divider=MCF_CLOCK_CCHR+1;
	fref=input_clk/divider;
	lpdr=MCF_CLOCK_LPDR;
		
	mfd = (MCF_CLOCK_SYNCR & 0x7000) >> 12;
    mfd = MIN_MFD + (mfd * 2);
    rfd = (MCF_CLOCK_SYNCR & 0x0700) >> 8;
    rfd = 1 << rfd;
    
    /* Return current PLL output */
    if ((MCF_CLOCK_SYNCR & MCF_CLOCK_SYNCR_CLKSRC) &&
            (MCF_CLOCK_SYNCR & MCF_CLOCK_SYNCR_PLLEN))
    {
    	fout=(fref * (mfd / rfd))/(1<<lpdr); 
    }
   	else
   	{
        fout=fref/(1<<lpdr);    		
   	}
   	
   	return fout;
}

/********************************************************************/
/*
 * These functions control the PSTCLK/CLKOUT signal. Disabling the 
 * CLKOUT signal reduces power and EMI. However, some BDM interface
 * cables will rely on the PSTCLK/CLKOUT signal to clock the logic
 * that interfaces to the DSCLK, DSI, and DSO signals.
 */
void
clock_clkout_disable(void)
{
    MCF_CLOCK_SYNCR |= MCF_CLOCK_SYNCR_DISCLK;
}

void
clock_clkout_enable(void)
{
    MCF_CLOCK_SYNCR &= ~MCF_CLOCK_SYNCR_DISCLK;
}
/********************************************************************/
