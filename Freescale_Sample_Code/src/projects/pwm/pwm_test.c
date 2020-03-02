/*
 * File:		pwm_test.c
 * Purpose:		PWM Examples
 *
 * License:     All software covered by license agreement in -
 *              docs/Freescale_Software_License.pdf
 */

#include "common.h"
#include "uart.h"
#include "pwm/pwm.h"
#include "pwm_example.h"

typedef int8 (* example_fuct) (void);

/*
 * Available functions
 */
example_fuct example_functions[]={
				pwm_basictest,
				pwm_concat
						   };


/********************************************************************/
void 
main (void)
{
	uint8 i;
	uint8 u8Continue =0;

	/* Global interrupt enable */
	
	printf("PWM Example for Kirin\n");
	 	
	mcf5xxx_wr_sr(0x2000);

	
	/* Perform Tests */
    do
    {
    	for (i=0;i<sizeof(example_functions)/sizeof(example_functions[0]);i++)
    	{
        	if (example_functions[i]() != 0x00)
        	{
            	printf ("Example #%d Failed!! \n", i);
        	}
        	else
        	{
            	printf ("Example #%d OK \n", i);
        	}
    	}
 
    	printf ("Press 'Y' to repeat examples\n");
    	u8Continue = in_char();
    }while (u8Continue == 'Y' || u8Continue == 'y');

    printf ("End Examples\n");    
}
	
