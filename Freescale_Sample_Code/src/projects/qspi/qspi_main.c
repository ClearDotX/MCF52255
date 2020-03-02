/*
 * File:		qspi_main.c
 * Purpose:		QSPI Examples
 *
 * License:     All software covered by license agreement in -
 *              docs/Freescale_Software_License.pdf
 */

#include "common.h"
#include "qspi.h"
#include "gpt.h"
#include "qspi_example.h"

typedef int8 (* example_fuct) (void);

example_fuct example_functions[]={
                       qspi_transmit,
                       qspi_wraparound,
                       qspi_spif,
                       qspi_abort,
                       qspi_baudrate,
                       qspi_bits
						};

/********************************************************************/
void 
main (void)
{
	uint8 i;
	uint8 u8Continue =0;
		
	/* Set the interrupt handlers in the vector table */
	mcf5xxx_set_handler(64 + 18, (ADDRESS)QSPI_ISR);
	mcf5xxx_set_handler(64 + 41, (ADDRESS)GPT_TOF);
	mcf5xxx_set_handler(64 + 42, (ADDRESS)GPT_PAIF);
	mcf5xxx_set_handler(64 + 43, (ADDRESS)GPT_PAOVF);
	mcf5xxx_set_handler(64 + 47, (ADDRESS)GPT_C3F);	
	
	printf("\"QSPI\" module examples on MCF52259\n");
	
	/* QSPI Basic Initialization */
	if (! QSPIInit(1000, 2, 8, 1, 20, 1))
	{
		u8Continue = 'Y';
	}
	else
	{
		printf ("Initialization Error\n");
	}
	
    	/* Global interrupt enable */
	mcf5xxx_wr_sr(0x2000);

    /* Perform Tests */
    while (u8Continue == 'Y' || u8Continue == 'y')
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
    }
    
    printf ("--------------------------\n");
    printf ("End of QSPI Examples\n");
}

/********************************************************************/
	
