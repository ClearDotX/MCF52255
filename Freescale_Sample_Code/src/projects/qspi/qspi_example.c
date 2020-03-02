/*
 * File:		qspi_example.c
 * Purpose:		QSPI Examples
 *
 * License:     All software covered by license agreement in -
 *              docs/Freescale_Software_License.pdf
 */

#include "common.h"
#include "qspi.h"
#include "qspi_example.h"
#include "gpt.h"

static int8 compare_tolerance(uint32 u32Val1, uint32 u32Val2, uint8 u8tolerance);
static void printArray(uint8 *u8Data, uint8 u8Len);
static void example_idle_state(void);

/********************************************************************/
/* 
 * Transmit Example
 *
 * 1. Initialize the SPI module (already made)
 * 2. Create Buffer of size n
 * 3. Fill Buffer with data
 * 4. Send Data
 * 5. Compare TX and RX
 * 6. Repeat for different sizes of data 
 */
int8 qspi_transmit(void)
{
    tQSPIBuffers *MyBuf;
    int8 i8Return = 0;
    uint8 i, j;
 
     /* Initialize SPI module */
    printf("\t***********************\t\n");
    printf("QSPI Transmission Example\n");
    printf("Connect QSPI_DIN to QSPI_DOUT and press a key\n");
    WAIT_4_USER_INPUT();
    for (i=1; i < 17; i++)
    {
        MyBuf = QSPI_InitFullBuffer(i);
        
        for (j=0; j < i; j++)
        {
            MyBuf->pu16TxData[j] = j;
            //MyBuf->pu8Cmd[j] = j;
            MyBuf->pu8Cmd[j] = (/*MCF_QSPI_QDR_CONT |*/ MCF_QSPI_QDR_BITSE | MCF_QSPI_QDR_DT |
                        MCF_QSPI_QDR_DSCK | MCF_QSPI_QDR_CS(0x0)) >> 8;
            //printf("address is %p, data is 0x%x\n", &(MyBuf->pu8Cmd[j]), MyBuf->pu8Cmd[j]);

        }

        printf("Sending %d bytes of data:  ",i);

        if (QSPIPollBufferTransfer(MyBuf) != 0x00)
        {
            QSPI_FreeFullBuffer(MyBuf);
            printf("Error\n"); 
            return -1;
        }
        
		for (j=0; j < i; j++)
        {
            if (MyBuf->pu16TxData[j] != MyBuf->pu16RxData[j]){
                QSPI_FreeFullBuffer(MyBuf);
                printf("Error\n"); 
                return -1;
            }
        }
        
        printf("OK\n"); 
        QSPI_FreeFullBuffer(MyBuf);
    }
    
    return 0;
}

/********************************************************************/
/* 
 * Finished Interrupt Example
 *
 * 1. Initialize the SPI module (already made)
 * 2. Create Buffer of less than 16 entries
 * 3. Fill Buffer with data
 * 4. Send Data
 * 6. Verify if interrupt was held and verify data
 */
int8 qspi_spif(void)
{
    tQSPIBuffers *MyBuf;
    int8 i8Return = 0;
    uint32 u32Countdown = 0;
    uint8 j;
 
     /* Initialize SPI module */
    printf("\t***********************\t\n");
    printf("QSPI SPI Finished Interrupt Example\n");
    printf("Connect QSPI_DIN to QSPI_DOUT and press a key\n");
    WAIT_4_USER_INPUT();
    
    MyBuf = QSPI_InitFullBuffer(16);
    
    for (j=0; j < 16; j++)
    {
        MyBuf->pu16TxData[j] = j;
        //MyBuf->pu8Cmd[j] = j;
		MyBuf->pu8Cmd[j] = (/*MCF_QSPI_QDR_CONT |*/ MCF_QSPI_QDR_BITSE | MCF_QSPI_QDR_DT |
                        MCF_QSPI_QDR_DSCK | MCF_QSPI_QDR_CS(0x0)) >> 8;
    }

    if (QSPIIntBufferTransfer(MyBuf) != 0x00)
    {
        QSPI_FreeFullBuffer(MyBuf);
        printf("Error\n"); 
        return -1;
    }
    
    while (!QSPIF_IntRdy())
    {
        if (u32Countdown++ == 0x100000)
        {
            QSPI_FreeFullBuffer(MyBuf);
            printf("Error\n"); 
            return -1;
        }
    }
    
    printf("Interrupt Received \n");
    
    for (j=0; j < 16; j++){
        if (MyBuf->pu16TxData[j] != MyBuf->pu16RxData[j])
        {
            QSPI_FreeFullBuffer(MyBuf);
            printf("Error\n"); 
            return -1;
        }
    }
    printf("Integrity of Data OK \n");
    
    QSPI_FreeFullBuffer(MyBuf);
    
    return 0;
}

/********************************************************************/
/* 
 * QSPI Abort Example
 *
 * 1. Initialize the SPI module (already made)
 * 2. Create a Buffer
 * 3. Start sending Buffer
 * 4. Abort Transmission
 * 5. Verify ABRT flag
 * 6. Lock Abort
 * 7. Repeat 2-4, Verify that ABRT was not set and SPE can't be cleared
 * 8. Enable Abort Access Error
 * 9. Repeat 2-4, Trying to abort must give access error
 */
int8 qspi_abort(void)
{
    tQSPIBuffers *MyBuf;
    int8 i8Return = 0;    
    uint32 u32Countdown = 0;
    uint8 j;

     /* Initialize SPI module */
    printf("\t***********************\t\n");
    printf("QSPI SPI Abort Example\n");
    printf("Connect QSPI_DIN to QSPI_DOUT and press a key\n");
    WAIT_4_USER_INPUT();
    
	MyBuf = QSPI_InitFullBuffer(16);
    
    for (j=0; j < 16; j++)
    {
        MyBuf->pu16TxData[j] = j;
        //MyBuf->pu8Cmd[j] = j;
		MyBuf->pu8Cmd[j] = (/*MCF_QSPI_QDR_CONT |*/ MCF_QSPI_QDR_BITSE | MCF_QSPI_QDR_DT |
                        MCF_QSPI_QDR_DSCK | MCF_QSPI_QDR_CS(0x0)) >> 8;
    }
    
    QSPI_Enable_Abort();
    if (QSPIIntBufferTransfer(MyBuf) != 0x00)
    {
        QSPI_FreeFullBuffer(MyBuf);
        printf("Error1\n"); 
        i8Return |= -1;
    }
    
    QSPI_ABORT_TX_SPE();	//clear SPE
    printf("MCF_QSPI_QDLYR is 0x%x\n", MCF_QSPI_QDLYR);
	
    if (QSPI_IntStat() != QSPI_BUFFSTAT_ABORTED)
    {
        i8Return |= -1;
    }
    
    printf("Buffer aborted Correctly \n");
    
    /* SAME test but with ABORT Disabled */
    QSPI_Disable_Abort();
    
    if (QSPIIntBufferTransfer(MyBuf) != 0x00)
    {
        QSPI_FreeFullBuffer(MyBuf);
        printf("Error1\n"); 
        i8Return |= -1;
    }
    
    QSPI_ABORT_TX_SPE();	//clear SPE
    printf("MCF_QSPI_QDLYR is 0x%x\n", MCF_QSPI_QDLYR);
	
    if (QSPI_IntStat() == QSPI_BUFFSTAT_ABORTED)
    {
        i8Return |= -1;
    }

    printf("Buffer locked Correctly \n");

    while (!QSPIF_IntRdy())
    {
        if (u32Countdown++ == 0x100000)
        {
            QSPI_FreeFullBuffer(MyBuf);
            printf("Error\n"); 
            return -1;
        }
    }
 	
    for (j=0; j < 16; j++)
    {
        if (MyBuf->pu16TxData[j] != MyBuf->pu16RxData[j])
        {
            QSPI_FreeFullBuffer(MyBuf);
            printf("Error\n"); 
            return -1;
        }
    }
    printf("Integrity of Data OK \n");
    
    QSPI_FreeFullBuffer(MyBuf);
    return i8Return;
}

/********************************************************************/
/* 
 * Transfer Wraparound Example
 *
 * 1. Initialize the SPI module (already made)
 * 2. Create Buffer of size n (<16)
 * 3. Fill Buffer with data
 * 4. Send Data
 * 5. Compare TX and RX
 * 6. Repeat for different sizes of data 
 */
int8 qspi_wraparound(void)
{
    tQSPIBuffers *MyBuf;
    int8 i8Return = 0;
    uint8 i, j;
 
     /* Initialize SPI module */
    printf("\t***********************\t\n");
    printf("QSPI Transfer Wraparound Example\n");
    printf("Connect QSPI_DIN to QSPI_DOUT and press a key\n");
    WAIT_4_USER_INPUT();
    for (i=17; i < 40; i++)
    {
        MyBuf = QSPI_InitFullBuffer(i);
        
        for (j=0; j < i; j++)
        {
            MyBuf->pu16TxData[j] = j+1;
            //MyBuf->pu8Cmd[j] = 0;
			MyBuf->pu8Cmd[j] = (/*MCF_QSPI_QDR_CONT |*/ MCF_QSPI_QDR_BITSE | MCF_QSPI_QDR_DT |
                        MCF_QSPI_QDR_DSCK | MCF_QSPI_QDR_CS(0x0))>>8;
        }

        printf("Sending %d bytes of data: ",i);
        if (QSPIPollBufferTransfer(MyBuf) != 0x00)
        {
            QSPI_FreeFullBuffer(MyBuf);
            printf("Error\n"); 
            return -1;
        }
        
        for (j=0; j < i; j++)
        {
            if (MyBuf->pu16TxData[j] != MyBuf->pu16RxData[j])
            {
                QSPI_FreeFullBuffer(MyBuf);
                printf("Error\n"); 
                return -1;
            }
        }
 
        printf("OK\n"); 
        QSPI_FreeFullBuffer(MyBuf);
    }
    
    return 0;
}
/********************************************************************/
/* 
 * Baudrate Example
 *
 * 1. Initialize the SPI module
 * 2. Configure for a specific baudrate
 * 3. Send a byte
 * 4. Veify Frequency
 * 5. Repeat for other baudrates 
 */
int8 qspi_baudrate (void)
{    
	uint8 u8Char;

    /* Initialize SPI module */
    printf("\t***********************\t\n");
    printf("QSPI Baudrate Example\n");
    printf("Connect QSPI_CLK to an oscilloscope and press a key\n");

    example_idle_state();
    WAIT_4_USER_INPUT();
    printf("Press 'Y' if baudrate is correct\n");
    
    if (QSPISetBaudrate(BAUDRATE) == -1)
        return 1;
        

    /* Send dummy data to test clk */
    QSPISendByteRaw(0xAA, 0x0);        
        
    printf("Desired clock: %dKhz. OK?(Y/N) ",BAUDRATE);
    u8Char = WAIT_4_USER_INPUT();
    if (( u8Char == 'Y') || (u8Char == 'y'))
    {
        /* OK */
        printf("\n"); 
    }
    else
    {
        printf("Error\n"); 
        return -1;
    }
         
    return 0;
}

/********************************************************************/
/*
 * Number of Bits Example
 *
 * 1. Initialize the SPI module
 * 2. Set for n bits
 * 3. Make one transfer
 * 4. Verify number of bits
 * 5. Repeat for other number of bits
 */
int8 qspi_bits(void)
{
    uint32 u32Pulses;
    uint8 i;
    
    /* Initialize SPI module */
    printf("\t***********************\t\n");
    printf("QSPI Number of Bits Example\n");
    printf("Connect QSPI_CLK (SCLK) to GPT3 (IC3) and press a key when ready\n");

    example_idle_state();
    WAIT_4_USER_INPUT();
    
    GPTInitPulseAcc_Counter();
    
    for (i = 8; i < 17; i++)
    {
        if (QSPISetBits(i) == -1)
            return 1;
        GPTStartPulseAccCounter();
        /* Send dummy data to test clk */
        QSPISendByteRaw(0xAA, 0x0);        
        
        printf("Number of Bits: %d",i);
        
        u32Pulses = GPTGetPulseAccCounter();
        
        printf(", Pulses: %d\n",u32Pulses);
        if (u32Pulses != i) 
        {
            printf("Error");
            return -1;
        }
    }
  
    return 0;
}


static void example_idle_state(void){
    QSPIInit(200, 0,  8, 1, 1, 1);
 }

