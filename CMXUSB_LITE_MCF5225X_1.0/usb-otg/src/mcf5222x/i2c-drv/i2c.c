/***************************************************************************
 *
 *            Copyright (c) 2006-2007 by CMX Systems, Inc.
 *
 * This software is copyrighted by and is the sole property of
 * CMX.  All rights, title, ownership, or other interests
 * in the software remain the property of CMX.  This
 * software may only be used in accordance with the corresponding
 * license agreement.  Any unauthorized use, duplication, transmission,
 * distribution, or disclosure of this software is expressly forbidden.
 *
 * This Copyright notice may not be removed or modified without prior
 * written consent of CMX.
 *
 * CMX reserves the right to modify this software without notice.
 *
 * CMX Systems, Inc.
 * 12276 San Jose Blvd. #511
 * Jacksonville, FL 32223
 * USA
 *
 * Tel:  (904) 880-1840
 * Fax:  (904) 880-1632
 * http: www.cmx.com
 * email: cmx@cmx.com
 *
 ***************************************************************************/

#include "i2c.h"
#include "mcf5222x_reg.h"

#define I2C_CLK  96000

void iic_delay(void);

void iic_delay(void)
{
	volatile hcc_u16 i;
	/* Wait for a bit */
	i=500;
	while(i--)
	;
}

const hcc_u16 i2c_prescaler_val[] = 
{    
    28, 30, 34, 40, 44, 48, 56, 68 ,80 ,88
  , 104, 128 ,144 ,160 ,192 ,240, 288, 320, 384, 480
  , 576, 640, 768, 960, 1152, 1280, 1536, 1920, 2304, 2560
  , 3072, 3840, 20, 22, 24, 26, 28, 32, 36, 40
  , 48, 56, 64, 72, 80, 96, 112, 128, 160, 192
  , 224, 256, 320, 384, 448, 512, 640, 768, 896, 1024
  , 1280, 1536, 1792, 2048
};

hcc_u8 i2c_set_bps(hcc_u32 bps)
{
  hcc_u8 x;
  hcc_u8 best_ndx=(hcc_u8)-1u;
  hcc_u16 e=(hcc_u16)-1u;
  hcc_u32 d=(I2C_CLK*1000ul)/bps;
  
  for(x=0; x<sizeof(i2c_prescaler_val)/sizeof(i2c_prescaler_val[0]); x++)
  {
    hcc_u16 e1;
    if (d>i2c_prescaler_val[x])
    {
      continue;
    }
    
    e1=(hcc_u16)(i2c_prescaler_val[x]-d);
    if (e1<e)
    {
      e=e1;
      best_ndx=x;
    }
  }

  if (best_ndx == (hcc_u8)-1u)
  {
    return(1);
  }
  /* set the frequency near 400KHz, see MCF52223RM table for details */ 
  MCF_I2C_I2FDR = best_ndx;
  return(0);
}

void i2c_init(hcc_u8 addr)
{
	hcc_u8 temp;
	
	/* Enable the I2C signals */
	MCF_GPIO_PASPAR |= ( MCF_GPIO_PASPAR_SDA_SDA
							 | MCF_GPIO_PASPAR_SCL_SCL);
							 
    i2c_set_bps(400000ul);
    
	/* start the module */
	MCF_I2C_I2CR = MCF_I2C_I2CR_IEN | 0;

    /* set slave address */
    MCF_I2C_I2AR = addr;

	
	/* if bit busy set, send a stop condition to slave module */
	if( MCF_I2C_I2SR & MCF_I2C_I2SR_IBB)
	{
		MCF_I2C_I2CR = 0;						/* clear control register */
		MCF_I2C_I2CR = MCF_I2C_I2CR_IEN |		/* enable module */
					   MCF_I2C_I2CR_MSTA;		/* send a START conditionn */
		temp = MCF_I2C_I2DR;					/* dummy read */
		MCF_I2C_I2SR = 0;						/* clear status register */
		MCF_I2C_I2CR = 0;						/* clear control register */
		MCF_I2C_I2CR = MCF_I2C_I2CR_IEN | 0;	/* enable the module again */
	}
	return;	
}

hcc_u8 i2c_receive_byte(hcc_u8 address, hcc_u8 id)
{
	hcc_u8 data;
    /* wait till I2C is busy. */
	while(MCF_I2C_I2SR & MCF_I2C_I2SR_IBB)
	;		
	
	MCF_I2C_I2CR |= MCF_I2C_I2CR_MTX;			/* setting in Tx mode */
	/* send start condition */
	MCF_I2C_I2CR |= MCF_I2C_I2CR_MSTA;
	MCF_I2C_I2DR = id;							/* devide ID to write */
	/* wait until one byte transfer completion */
	while( !(MCF_I2C_I2SR & MCF_I2C_I2SR_IIF ))
	;
	/* clear the completion transfer flag */
	MCF_I2C_I2SR &= ~MCF_I2C_I2SR_IIF;
	MCF_I2C_I2DR = address;						/* memory address */
	/* wait until one byte transfer completion */
	while( !(MCF_I2C_I2SR & MCF_I2C_I2SR_IIF ))
	;
	/* clear the completion transfer flag */
	MCF_I2C_I2SR &= ~MCF_I2C_I2SR_IIF;	 	
	MCF_I2C_I2CR |= MCF_I2C_I2CR_RSTA;			/* resend start */
	MCF_I2C_I2DR = (hcc_u8)(id | 0x01);					/* device id to read */
	/* wait until one byte transfer completion */
	while( !(MCF_I2C_I2SR & MCF_I2C_I2SR_IIF ))
	;
	/* clear the completion transfer flag */
	MCF_I2C_I2SR &= ~MCF_I2C_I2SR_IIF;
	MCF_I2C_I2CR &= ~MCF_I2C_I2CR_MTX;			/* setting in Rx mode */
	MCF_I2C_I2CR |= MCF_I2C_I2CR_TXAK;			/* send NO ACK */
	data = MCF_I2C_I2DR;						/* dummy read */
	/* wait until one byte transfer completion */
	while( !(MCF_I2C_I2SR & MCF_I2C_I2SR_IIF ))
	;
	/* clear the completion transfer flag */
	MCF_I2C_I2SR &= ~MCF_I2C_I2SR_IIF;
	data = MCF_I2C_I2DR;						/* read data received */
	/* wait until one byte transfer completion */
	while( !(MCF_I2C_I2SR & MCF_I2C_I2SR_IIF ))
	;
	/* clear the completion transfer flag */
	MCF_I2C_I2SR &= ~MCF_I2C_I2SR_IIF;
	/* generates stop condition */
	MCF_I2C_I2CR &= ~MCF_I2C_I2CR_MSTA;
	/* send the received data */
	return data;
}

void i2c_send_byte(int data, int address, int id)
{
    /* wait till I2C is busy. */
	while(MCF_I2C_I2SR & MCF_I2C_I2SR_IBB)
	;		

	MCF_I2C_I2CR |= MCF_I2C_I2CR_MTX;			/* setting in Tx mode */
	/* generates start condition */
	MCF_I2C_I2CR |= MCF_I2C_I2CR_MSTA;
	MCF_I2C_I2DR = (hcc_u8)id;							/* set devide ID to write */
	/* wait until one byte transfer completion */
	while( !(MCF_I2C_I2SR & MCF_I2C_I2SR_IIF ))
	;
	/* clear the completion transfer flag */
	MCF_I2C_I2SR &= ~MCF_I2C_I2SR_IIF;
	MCF_I2C_I2DR =(hcc_u8)address;						/* memory address */
	iic_delay();
	/* wait until one byte transfer completion */
	while( !(MCF_I2C_I2SR & MCF_I2C_I2SR_IIF ))
	;
	/* clear the completion transfer flag */
	MCF_I2C_I2SR &= ~MCF_I2C_I2SR_IIF;	
	MCF_I2C_I2DR = (hcc_u8)data;						/* memory data */
	/* wait until one byte transfer completion */
	iic_delay();
	while( !(MCF_I2C_I2SR & MCF_I2C_I2SR_IIF ))
	;
	/* clear the completion transfer flag */
	MCF_I2C_I2SR &= ~MCF_I2C_I2SR_IIF;	
	/* generates stop condition */
	MCF_I2C_I2CR &= ~MCF_I2C_I2CR_MSTA;
	return;
}
/****************************** END OF FILE **********************************/
