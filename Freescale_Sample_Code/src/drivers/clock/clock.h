/*
 * File:    clock.h
 * Purpose: Driver for the on-chip Clock module
 *
 * Notes:
 * 
 * License:     All software covered by license agreement in -
 *              docs/Freescale_Software_License.pdf
 */

#ifndef _CLOCK_H_
#define _CLOCK_H_

/********************************************************************/
/* 
 * PLL min/max specifications
 */
#define MAX_FVCO    80000   /* KHz */
#define MAX_FSYS    80000   /* KHz */
#define MAX_FREF    10000   /* KHz */
#define MIN_FREF    2000    /* KHz */
#define MAX_MFD     18      /* Multiplier (not encoded) */
#define MIN_MFD     4       /* Multiplier (not encoded) */
#define MAX_RFD     128     /* Divider (not encoded) */
#define MIN_RFD     1       /* Divider (not encoded) */

/*
 * Low Power Divider specifications
 */
#define MIN_LPD     (1 << 0)    /* Divider (not encoded) */
#define MAX_LPD     (1 << 15)   /* Divider (not encoded) */

/* 
 * Operating Parameters
 * Pass to clock_pll() the 'flags' argument
 */
#define PLL_DISABLE   (0x0001)
#define PLL_PRETEND   (0x0002)
#define PLL_CLKSRC    (0x0004)
#define PLL_FWKUP     (0x0020)
#define PLL_DISCLK    (0x0040)
#define PLL_LOCEN     (0x0080)
#define PLL_LOCRE     (0x0800)
#define PLL_LOLRE     (0x8000)
#define PLL_ALLFLAGS  (0x88E4)

/********************************************************************/
/*
 * Functions provided by this driver
 */
int clock_pll (int, int, int);
int clock_speed (void);
int clock_lpd (int);
void clock_clkout_disable(void);
void clock_clkout_enable(void);

/********************************************************************/

#endif /* _CLOCK_H_ */
