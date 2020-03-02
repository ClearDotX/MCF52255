/*
 * File:    rand.c
 * Purpose: Pseudo-random number generation from the C standard example 
 *
 * Notes:   Assumes RAND_MAX is 32768
 * 
 * License:     All software covered by license agreement in -
 *              docs/Freescale_Software_License.pdf
 */

#include "common.h"

/********************************************************************/
static int next = 1;

int 
rand(void)
{
  next = next * 1103515245 + 12345;
  return (next>>16) & 0x7FFF;
}

void 
srand(int seed)
{
  next = seed;
}
/********************************************************************/
