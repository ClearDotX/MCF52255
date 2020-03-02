

#include "common.h"
#include "m52259evb_cpld.h"

#define MRAM_START_ADDRESS	(*(vuint8*)(0x80000000))
#define CPLD_START_ADDRESS	(*(vuint8*)(0x80080000))


/********************************************************************/

typedef int8 (* example_fuct) (void);

/*
 * Functions Available
 */
example_fuct example_functions[]={
                                  cpld_8bit_cs1_test,
                                  //cpld_16bit_cs0_test,
						         };

void main (void)
{

	uint8 i;
	uint8 u8Continue =0;
	

	printf("\n\nMINIBUS to CPLD DEMO Software\n");
	

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
    printf ("End of Software\n");

    

}

