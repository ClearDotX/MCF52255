/*
 * File:		flexcan_example.c
 * Purpose:		Main process
 *
 */

#include "common.h"
#include "flexcan.h"
#include "flexcan_example.h"
#include "gpt.h"

static int8 compare_tolerance(uint32 u32Val1, uint32 u32Val2, uint8 u8tolerance);
static void printArray(uint8 *u8Data, uint8 u8Len);
static void test_idle_state(void);

/********************************************************************/
int8 flexcan_baudrate (void)
{
    uint32 u32Time;
    uint8 aDummy[1];
    uint8 i,j;
    uint32 au32BaudrateExamples[] = { 125000, 250000, 500000, 1000000};
 
    printf("\t***********************\t\n");
    printf("FlexCAN Baudrate Example\n");
    printf("Connect CANTX to GPT3 and press a key\n");

    GPTInitPulseMeasurement(0);
    test_idle_state();
    WAIT_4_USER_INPUT();
    for (j=0; j < 2; j++)
    {
        for (i = 0; i < (sizeof(au32BaudrateExamples)/sizeof(au32BaudrateExamples[0])); 
             i++)
        {
            if (FlexCANSetBaudrate(au32BaudrateExamples[i],j) == -1)
                return 1;
            GPTStartPulseMeasurement();
            FlexCANSendDataNoPoll(aDummy, 0, FLEXCAN_STANDARDID(0x400), 0);
            u32Time = GPTGetMeasurement(); 
            if (u32Time == 0){
                printf("Error\n"); 
                return 1;
            }
            printf("Pulse Time #%d: ",i+j*(sizeof(au32BaudrateExamples)/sizeof(au32BaudrateExamples[0])));
            printf("%d \n",u32Time);
            
            if (compare_tolerance(u32Time, (SYS_CLK_KHZ / 2)/(au32BaudrateExamples[i]/1000), 2) != 0x00)
            {
                return 1;            
            }
        }
    }
    return 0;
}

/********************************************************************/
int8 flexcan_loopback (void)
{
    uint8 aDummy[] = { 1,2,3,4,5,6,7,8,9};
    uint8 aDummyR[] = { 0,1,2,3,4,5,6,7,8};
    uint8 i,u8Len;
 
    printf("\t***********************\t\n");
    printf("FlexCAN LoopBack Example\n");
    printf("Press a key when ready\n");
    WAIT_4_USER_INPUT();
    
    test_idle_state();
    MCF_FlexCAN_ENABLE_LOOPBACK();
    
    for (i = 0; i < (sizeof(aDummy)/sizeof(aDummy[0])); i++)
    {
        FlexCANSetMBforRx (1, FLEXCAN_STANDARDID(i*0x80));      
        if (FlexCANSendDataPoll(aDummy, i, FLEXCAN_STANDARDID(i*0x80),0) != 0x00) 
        {
            return 1;
        }
            
        printf("Message #%d " ,i);
        printf("Sent with ID:%x\n" ,i*0x80);
        
        u8Len = FlexCANReceiveDataPoll (1,&aDummyR[0]);
        printf("Message Received #%d " ,i);
        printArray(aDummyR, u8Len);
    }

    MCF_FlexCAN_DISABLE_LOOPBACK();
    
    
    return 0;
}

/********************************************************************/
int8 flexcan_MB0txnoerrpoll (void)
{
    uint8 aDummy[] = { 1,2,3,4,5,6,7,8,9};
    uint8 i;
    uint8 u8Integrity;
 
    printf("\t***********************\t\n");
    printf("FlexCAN TX with Polling Example in MB0\n");
    printf("1. Connect UART2_CAN connector to CAN bus\n");
    printf("2. Enable CANAlyzer @ 1Mhz \n");
    printf("3. Press a key when Ready \n");
    WAIT_4_USER_INPUT();
    
    test_idle_state();
    for (i = 0; i < (sizeof(aDummy)/sizeof(aDummy[0])); i++)
    {
        if (FlexCANSendDataPoll(aDummy, i, FLEXCAN_STANDARDID(0x80*i),0) != 0x00) 
        {
            return 1;
        }
            
        printf("Message #%d " ,i+1);
        printf("Sent with ID:%x\n" ,i*0x80);
    }
    for (i = 0; i < (sizeof(aDummy)/sizeof(aDummy[0])); i++)
    {
        if (FlexCANSendDataPoll(aDummy, i, FLEXCAN_EXTENDEDID(0x1000000*i),0) != 0x00) 
        {
            return 1;
        }
            
        printf("Message #%d " ,i+1);
        printf("Sent with extended ID:%x\n" ,i*0x1000000);
    }
        
   printf("Messages Acknowledged!\n");
   TEST_USER_VERIFY(u8Integrity);
   
   return 0;
}

/********************************************************************/
int8 flexcan_MBxtxnoerrpoll (void)
{
    uint8 aDummy[] = { 1,2,3,4,5,6,7,8,9};
    uint8 i;
    uint8 u8Integrity;
 
    printf("\t***********************\t\n");
    printf("FlexCAN TX with Polling Example in all MBs\n");
    printf("1. Connect UART2_CAN connector to CAN bus\n");
    printf("2. Enable CANAlyzer @ 1Mhz \n");
    printf("3. Press a key when Ready \n");
    WAIT_4_USER_INPUT();
    
    test_idle_state();
    for (i = 0; i < 16; i++)
    {
        if (FlexCANSendDataPoll(aDummy, 8, FLEXCAN_STANDARDID(0x40*i),i) != 0x00) 
        {
            return 1;
        }
            
        printf("Message in MB#%d " ,i);
        printf("Sent with ID:%x\n" ,i*0x40);
    }
    for (i = 0; i < 16; i++)
    {
        if (FlexCANSendDataPoll(aDummy, 8, FLEXCAN_EXTENDEDID(0x1000000*i),i) != 0x00) 
        {
            return 1;
        }
            
        printf("Message in MB#%d " ,i);
        printf("Sent with Extended ID:%x\n" ,i*0x1000000);
    }
       
   printf("Messages Acknowledged!\n");
   TEST_USER_VERIFY(u8Integrity);
   
   return 0;
}

/********************************************************************/
int8 flexcan_MBxtxnoerrint (void)
{
    uint8 aDummy[] = { 1,2,3,4,5,6,7,8,9};
    uint8 i;
    uint8 u8Integrity;
 
    printf("\t***********************\t\n");
    printf("FlexCAN TX with Interrupts Example in all MBs\n");
    printf("1. Connect UART2_CAN connector to CAN bus\n");
    printf("2. Enable CANAlyzer @ 1Mhz \n");
    printf("3. Press a key when Ready \n");
    WAIT_4_USER_INPUT();
    
    test_idle_state();
    for (i = 0; i < 16; i++)
    {
        FlexCANDisableMBs();
        FlexCANEnableMBInterrupt(i, 1);
        FlexCANSendDataNoPoll(aDummy, 8, FLEXCAN_STANDARDID(0x40*i),i);
        FlexCANWaitMBInt(i);    
        printf("Message in MB#%d " ,i);
        printf("Sent with ID:%x\n" ,i*0x40);
    }
    for (i = 0; i < 16; i++)
    {
        FlexCANDisableMBs();
        FlexCANEnableMBInterrupt(i, 0);
        FlexCANSendDataNoPoll(aDummy, 8, FLEXCAN_EXTENDEDID(0x1000000*i),i);        
        FlexCANWaitMBInt(i);    
        printf("Message in MB#%d " ,i);
        printf("Sent with Extended ID:%x\n" ,i*0x1000000);
    }
       
   printf("Messages Acknowledged!\n");
   TEST_USER_VERIFY(u8Integrity);
   
   return 0;
}

/********************************************************************/
int8 flexcan_MB0rxnoerrpoll (void)
{
    uint8 aDummy[] = { 1,2,3,4,5,6,7,8,9};
    uint8 u8Len;
    uint8 i;
    uint8 u8Integrity;
 
    printf("\t***********************\t\n");
    printf("FlexCAN RX in MB0 with Polling Example\n");
    printf("1. Connect UART2_CAN connector to CAN bus\n");
    printf("2. Enable CANAlyzer @ 1Mhz \n");
    printf("3. Send 9 Messageframes with ID 0x140 with CANAlyzer \n");
    
    test_idle_state();
    FlexCANSetMBforRx (0, FLEXCAN_STANDARDID(0x140));      
    for (i = 0; i < 9; i++){
        u8Len = FlexCANReceiveDataPoll (0,&aDummy[0]);
        printf("Message #%d " ,i);
        printArray(aDummy, u8Len);
    }
        
   printf("Messages Received!\n");
   TEST_USER_VERIFY(u8Integrity);
   
   return 0;
}

/********************************************************************/
int8 flexcan_MBxrxnoerrpoll (void)
{
    uint8 aDummy[] = { 1,2,3,4,5,6,7,8,9};
    uint8 u8Len;
    uint8 i;
    uint8 u8Integrity;
 
    printf("\t***********************\t\n");
    printf("FlexCAN RX in all MBs with Polling Example\n");
    printf("1. Connect UART2_CAN connector to CAN bus\n");
    printf("2. Enable CANAlyzer @ 1Mhz \n");
    printf("3. Send 16 Messageframes with ID 0x140 with CANAlyzer \n");
    
    test_idle_state();
    for (i = 0; i < 16; i++)
    {
        FlexCANDisableMBs();
        FlexCANSetMBforRx (i, FLEXCAN_STANDARDID(0x140));      
        u8Len = FlexCANReceiveDataPoll (i,&aDummy[0]);
        printf("Message in MB#%d " ,i+1);
        printArray(aDummy, u8Len);
    }
        
   printf("Messages Received!\n");
   TEST_USER_VERIFY(u8Integrity);
   
   return 0;
}

/********************************************************************/
int8 flexcan_MB0rxnoerrint (void)
{
    uint8 aDummy[] = { 1,2,3,4,5,6,7,8,9};
    uint8 u8Len;
    uint8 i;
    uint8 u8Integrity;
 
    printf("\t***********************\t\n");
    printf("FlexCAN RX in MB0 with Interrupts Example\n");
    printf("1. Connect UART2_CAN connector to CAN bus\n");
    printf("2. Enable CANAlyzer @ 1Mhz \n");
    printf("3. Send 5 Messageframes with ID 0x140 with CANAlyzer \n");
    
    test_idle_state();
    FlexCANDisableMBs();
    FlexCANSetMBforRx (0, FLEXCAN_STANDARDID(0x140));      
    FlexCANEnableMBInterrupt(0, 0);
    for (i = 0; i < 5; i++)
    {
        FlexCANWaitMBInt(0);
        u8Len = FlexCANGetRXDataInt(0,&aDummy[0]);
        printf("Message #%d " ,i);
        printArray(aDummy, u8Len);
   }
        
   printf("Messages Received!\n");
   TEST_USER_VERIFY(u8Integrity);
   
   return 0;
}

/********************************************************************/
int8 flexcan_MBxrxnoerrint (void)
{
    uint8 aDummy[] = { 1,2,3,4,5,6,7,8,9};
    uint8 u8Len;
    uint8 i;
    uint8 u8Integrity;
 
    printf("\t***********************\t\n");
    printf("FlexCAN RX in all MBs with Interrupts Example\n");
    printf("1. Connect UART2_CAN connector to CAN bus\n");
    printf("2. Enable CANAlyzer @ 1Mhz \n");
    printf("3. Send 16 Messageframes with ID 0x140 with CANAlyzer \n");
    
    test_idle_state();
    for (i = 0; i < 16; i++)
    {
        FlexCANDisableMBs();
        FlexCANSetMBforRx (i, FLEXCAN_STANDARDID(0x140));      
        FlexCANEnableMBInterrupt(i, 0);
        FlexCANWaitMBInt(i);
        u8Len = FlexCANGetRXDataInt(i,&aDummy[0]);
        printf("Message in MB#%d " ,i+1);
        printArray(aDummy, u8Len);
        FlexCANDisableMBInterrupt(i);
   }
        
   printf("Messages Received!\n");
   TEST_USER_VERIFY(u8Integrity);
   
   return 0;
}

/********************************************************************/
int8 flexcan_MBxfilterrxnoerrint (void)
{
    uint8 aDummy[] = { 1,2,3,4,5,6,7,8,9};
    uint8 u8Len;
    uint8 i;
    uint8 j;
    uint8 u8Integrity;
 
    printf("\t***********************\t\n");
    printf("FlexCAN filtered RX in all MBs with Interrupts Example\n");
    printf("1. Connect UART2_CAN connector to CAN bus\n");
    printf("2. Enable CANAlyzer @ 1Mhz \n");
    printf("3. Send 16 Messageframes using CANAlyzer:\n");
    printf("   n=Message Buffer Number\n");
    printf("   X=Don't Care\n");
    printf("\t MB0-MB6 (MBn) -> Std IDs 0xnXX \n");
    printf("\t MB7-MB13 (MBn) -> Ext IDs: Bits 29-27 = n \n\t\t\t and bits 26-0 = X \n");
    printf("\t MB14 -> Std ID 0x700\n");
    printf("\t MB15 -> Ext ID 0x100000\n");
    printf("4. Press any key to stop\n");
    
    test_idle_state();
    for (i = 0; i < 7; i++)
    {
        FlexCANSetMBforRx (i, FLEXCAN_STANDARDID(i<<8));    
        FlexCANEnableMBInterrupt(i, 0);
    }
    for (i = 0; i < 7; i++)
    {
        FlexCANSetMBforRx (7+i, FLEXCAN_EXTENDEDID(i<<26));    
        FlexCANEnableMBInterrupt(7+i, 0);
    }
    FlexCANSetMBforRx (14, FLEXCAN_STANDARDID(0x700));    
    FlexCANEnableMBInterrupt(14, 0);
    FlexCANSetMBforRx (15, FLEXCAN_EXTENDEDID(0x1C000000));    
    FlexCANEnableMBInterrupt(15, 0);
    FlexCANSetMask(0,MB_EXTENDED_ID(0x1C000000));
    FlexCANSetMask(14,MB_STANDARD_ID(0xFFF));
    FlexCANSetMask(15,MB_EXTENDED_ID(0x1FFFFFFF));
    
    while (char_present() != 0x00)
        WAIT_4_USER_INPUT();
    
    do {
     j = FlexCANWaitALLInt();
        u8Len = FlexCANGetRXDataInt(j,&aDummy[0]);
        printf("Message in MB#%d " ,j);
        printArray(aDummy, u8Len);   
        printf("Press any key to Stop\n");
        j = (uint8)char_present();
    }while (j==0);
   
        
   printf("Messages Received!\n");
   TEST_USER_VERIFY(u8Integrity);
   
   return 0;
}

/********************************************************************/
int8 flexcan_errpoll (void)
{
    uint8 aDummy[] = { 1,2,3,4,5,6,7,8,9};
    uint32 i;
    uint32 u32expected;
 
    test_idle_state();
    /* TEST ACKNOWLEDGE ERROR */
    printf("\t***********************\t\n");
    printf("FlexCAN Error with polling Example\n");
    printf("1. Connect UART2_CAN connector to CAN bus\n");
    printf("2. Enable CANAlyzer @ 1Mhz \n");
    printf("3. Disable ACK in CANAlyzer \n");
    printf("4. Press a key when ready\n");
    WAIT_4_USER_INPUT();
    
	test_idle_state();
    FlexCANSendDataNoPoll(aDummy, 8, FLEXCAN_STANDARDID(0x40),0);
    for (i = 0; i < 100000; i++);
    u32expected = MCF_FlexCAN_ERRSTAT_ACKERR | MCF_FlexCAN_ERRSTAT_TXWRN | 
                MCF_FlexCAN_ERRSTAT_FLTCONF(MCF_FlexCAN_ERRSTAT_FLTCONF_PASSIVE)|
                MCF_FlexCAN_ERRSTAT_ERRINT;
    if ( (FLEXCAN_ERRORFLG &u32expected) != u32expected) 
    {
        printf ("ERRFLG: %x\n", FLEXCAN_ERRORFLG);
        printf("Errors Not Detected!\n");     
        return 1;
    }
    else
    {
        /* ERROR */
        printf("ACK Error Detected OK!\n");
        printf("5. Disconnect CAN_EN TX Jumper \n");
        printf("6. Press a key when ready\n");
        WAIT_4_USER_INPUT();
        FLEXCAN_CLEAR_ERR();
        FlexCANSendDataNoPoll(aDummy, 8, FLEXCAN_STANDARDID(0x40),0);
        for (i = 0; i < 100000; i++);
        u32expected = MCF_FlexCAN_ERRSTAT_BIT1ERR |
                    MCF_FlexCAN_ERRSTAT_FLTCONF(MCF_FlexCAN_ERRSTAT_FLTCONF_BUSOFF)|
                    MCF_FlexCAN_ERRSTAT_ERRINT|MCF_FlexCAN_ERRSTAT_BOFFINT;
        if ( (FLEXCAN_ERRORFLG &u32expected) != u32expected) {
            printf ("ERRFLG: %x\n", FLEXCAN_ERRORFLG);
            printf("Errors Not Detected!\n");     
            return 1;
        }
        else
        {
            printf("Bus OFF Detected OK!\n");
            test_idle_state();
            return 0;
        }
   }
}
/********************************************************************/
static int8 compare_tolerance(uint32 u32Val1, uint32 u32Val2, uint8 u8tolerance){
    uint32 diff;
    
    if (u32Val1 >= u32Val2){
        diff = u32Val1 - u32Val2;  
    }
    else{
        diff = u32Val2 - u32Val1;
    }
    
    if (diff <= u8tolerance){
        return 0;
    }
    else{
        return 1;
    }
}

static void test_idle_state(void){
    FlexCANDisableALLInterrupt();
    FlexCANDisableMBs();
    FlexCANResetMasks();
}

static void printArray(uint8 *u8Data, uint8 u8Len){
    uint8 i;
    
    for (i=0; i < u8Len; i++){
        printf ("%d: %d\t", i, u8Data[i]);
    }
    printf ("\n");
}