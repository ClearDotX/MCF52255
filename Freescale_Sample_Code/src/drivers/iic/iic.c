/*
 * File:		I2C.c
 * Purpose:		I2C transfer functions and interrupt handler
 * 
 * License:     All software covered by license agreement in -
 *              docs/Freescale_Software_License.pdf
 */
 
#include "iic/iic.h"


/********************************************************************/
/*
 * IIC initialization as master
 *
 * Parameters:
 *  None
 */
void I2Cinit()
{
	uint8 temp;
	
	/* Enable the I2C signals */
	MCF_GPIO_PASPAR |= ( MCF_GPIO_PASPAR_SDA_SDA
							 | MCF_GPIO_PASPAR_SCL_SCL);
							 
	/* set the frequency near 400KHz, see MCF52223RM table for details */ 
	MCF_I2C_I2FDR = MCF_I2C_I2FDR_IC(0x32);
	/* start the module */
	MCF_I2C_I2CR = 0 | MCF_I2C_I2CR_IEN;
	
	/* if bit busy set, send a stop condition to slave module */
	if( MCF_I2C_I2SR & MCF_I2C_I2SR_IBB)
	{
		/* clear control register */
		MCF_I2C_I2CR = 0;			
					
		/* enable module and send a START condition*/			
		MCF_I2C_I2CR = MCF_I2C_I2CR_IEN |		
					   MCF_I2C_I2CR_MSTA;
					   
		/* dummy read */			   		
		temp = MCF_I2C_I2DR;	
		
		/* clear status register */				
		MCF_I2C_I2SR = 0;	
		
		/* clear control register */					
		MCF_I2C_I2CR = 0;				
		
		/* enable the module again */		
		MCF_I2C_I2CR = 0 | MCF_I2C_I2CR_IEN;	
	}
	return;	
}

/********************************************************************/
/*
 * I2CreceiveByte: I2C read byte
 *
 * Parameters: address: address to read
 *			   id: I2C device to read
 *
 * Return : data: byte read it from device
 */
uint8 I2CreceiveByte(uint8 address, uint8 id)
{
	uint8 data;
	
	/* setting in Tx mode */
	MCF_I2C_I2CR |= MCF_I2C_I2CR_MTX;
				
	/* send start condition */
	MCF_I2C_I2CR |= MCF_I2C_I2CR_MSTA;
	
	/* devide ID to write */
	MCF_I2C_I2DR = id;				
				
	/* wait until one byte transfer completion */
	while( !(MCF_I2C_I2SR & MCF_I2C_I2SR_IIF ))
	;
	/* clear the completion transfer flag */
	MCF_I2C_I2SR &= ~MCF_I2C_I2SR_IIF;
	
	/* memory address */
	MCF_I2C_I2DR = address;
							
	/* wait until one byte transfer completion */
	while( !(MCF_I2C_I2SR & MCF_I2C_I2SR_IIF ))
	;
	/* clear the completion transfer flag */
	MCF_I2C_I2SR &= ~MCF_I2C_I2SR_IIF;	 
	
	/* resend start */	
	MCF_I2C_I2CR |= MCF_I2C_I2CR_RSTA;		
	
	/* device id to read */	
	MCF_I2C_I2DR = (uint8)(id | 0x01);					
	
	/* wait until one byte transfer completion */
	while( !(MCF_I2C_I2SR & MCF_I2C_I2SR_IIF ))
	;
	/* clear the completion transfer flag */
	MCF_I2C_I2SR &= ~MCF_I2C_I2SR_IIF;
	
	/* setting in Rx mode */
	MCF_I2C_I2CR &= ~MCF_I2C_I2CR_MTX;	
	
	/* send NO ACK */	
	MCF_I2C_I2CR |= MCF_I2C_I2CR_TXAK;	
	
	/* dummy read */		
	data = MCF_I2C_I2DR;			
				
	/* wait until one byte transfer completion */
	while( !(MCF_I2C_I2SR & MCF_I2C_I2SR_IIF ))
	;
	/* clear the completion transfer flag */
	MCF_I2C_I2SR &= ~MCF_I2C_I2SR_IIF;
	
	/* read data received */
	data = MCF_I2C_I2DR;			
				
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

/********************************************************************/
/*
 * I2CsendByte: send byte to I2C device
 *
 * Parameters: data: byte to write
 *			   address: address to write
 *			   id: I2C device to write
 *
 * Return : None.
 */
void I2CsendByte(uint8 data, uint8 address, uint8 id)
{
	mcf5xxx_irq_disable();
	
	/* setting in Tx mode */
	MCF_I2C_I2CR |= MCF_I2C_I2CR_MTX;		
		
	/* generates start condition */
	MCF_I2C_I2CR |= MCF_I2C_I2CR_MSTA;
	
	/* set devide ID to write */
	MCF_I2C_I2DR = id;						
		
	/* wait until one byte transfer completion */
	while( !(MCF_I2C_I2SR & MCF_I2C_I2SR_IIF ))
	;
	/* clear the completion transfer flag */
	MCF_I2C_I2SR &= ~MCF_I2C_I2SR_IIF;
	
	/* memory address */
	MCF_I2C_I2DR = address;						
	iic_delay();
	/* wait until one byte transfer completion */
	while( !(MCF_I2C_I2SR & MCF_I2C_I2SR_IIF ))
	;
	/* clear the completion transfer flag */
	MCF_I2C_I2SR &= ~MCF_I2C_I2SR_IIF;	
	
	/* memory data */
	MCF_I2C_I2DR = data;						
	/* wait until one byte transfer completion */
	iic_delay();
	while( !(MCF_I2C_I2SR & MCF_I2C_I2SR_IIF ))
	;
	/* clear the completion transfer flag */
	MCF_I2C_I2SR &= ~MCF_I2C_I2SR_IIF;	
	/* generates stop condition */
	MCF_I2C_I2CR &= ~MCF_I2C_I2CR_MSTA;
	mcf5xxx_irq_enable();
	return;
}


/********************************************************************/