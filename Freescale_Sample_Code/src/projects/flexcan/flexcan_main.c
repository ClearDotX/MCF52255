/*
 * File:		flexcan_main.c
 * Purpose:		Flexcan Examples
 *
 */

#include "common.h"
#include "flexcan.h"
#include "flexcan_example.h"

typedef int8 (* example_fuct) (void);

/*
 * Functions Available
 */
example_fuct example_functions[]={
                                  flexcan_loopback,
                                  flexcan_MB0txnoerrpoll,
                                  flexcan_MBxtxnoerrpoll,
                                  flexcan_MBxtxnoerrint,
                                  flexcan_MB0rxnoerrpoll,
                                  flexcan_MBxrxnoerrpoll,
                                  flexcan_MBxrxnoerrint,
                                  flexcan_MBxfilterrxnoerrint,
                                  flexcan_errpoll,
						          flexcan_baudrate,
						         };

/********************************************************************/
void 
main (void)
{
	uint8 i;
	uint8 u8Continue =0;
	
	
    MCF_GPIO_PUAPAR |= MCF_GPIO_PUAPAR_UCTS0_GPIO;
	MCF_GPIO_DDRUA  |= MCF_GPIO_DDRUA_DDRUA3;       //set to output
	MCF_GPIO_PORTUA &= ~MCF_GPIO_PORTUA_PORTUA3;    //drive low
	
	
    /* Global interrupt enable */

	printf("\"FlexCAN\" module examples on MCF52259\n");
	
    /* FlexCAN Basic Initialization */
    FlexCANInit();

    mcf5xxx_wr_sr(0x2000);

    do{
    for (i=0;i<sizeof(example_functions)/sizeof(example_functions[0]);i++)
    {
        if (example_functions[i]() != 0x00)
        {
            printf ("Example #%d Failed!! \n", i);
        }
        else{
            printf ("Example #%d OK \n", i);
        }
    }
 
    printf ("Press 'Y' to repeat test\n");
    u8Continue = in_char();
    }while (u8Continue == 'Y' || u8Continue == 'y');

    printf ("--------------------------\n");
    printf ("End of FlexCAN Examples\n");

    
}

/********************************************************************/
	
