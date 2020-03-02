/*
 * File:    iic.h
 * Purpose: Header for the Inter-Integrated Circuit
 * 
 * License:     All software covered by license agreement in -
 *              docs/Freescale_Software_License.pdf
 */
 
#ifndef _IIC_H_
#define _IIC_H_

#include "common.h"


/********************************************************************/
/*
 * Functions provided by this driver
 */
void I2Cinit(void);
uint8 I2CreceiveByte(uint8 address, uint8 id);
void I2CsendByte(uint8 data, uint8 address, uint8 id)


#endif /* _IIC_H_ */