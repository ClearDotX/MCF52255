/*
 * Copyright (c) 2004, Freescale Semiconductor
 * Freescale Confidential Proprietary
 *
 * File: FlexCAN_Drv.c
 * Purpose: FlexCAN Example
 *
 * License:     All software covered by license agreement in -
 *              docs/Freescale_Software_License.pdf
 *
 *  27 March 2008 - Modified for MCF5225x, changes to INTC vector #s - M. Lobdell
 */

#include "flexcan.h"
#include "common.h"


/********************************************************************/
/*
 * Initialize variables
 */
uint8 gBinSemaphore = 0;

#define CAN_1000K
//#define CAN_500K
//#define CAN_125K

typedef void (* flexcan_isr) (uint8 u8MB);
flexcan_isr flexcanMBISR[16];

uint8 gu8FlexCANIntSemaphore[16]={0};
uint8 gu8FlexCANLen[16]={0};
uint8 gu8FlexCANArrays[16][8]={0};
uint32 gu32ErrFlg =0;


/********************************************************************/
/*
 * Initialize the FlexCAN
 */
void FlexCANInit(void )
{
	/*
     * Register Initialization
 	 */


  FLEXCAN_GPIO_INIT();
  MCF_FlexCAN_CANMCR = MCF_FlexCAN_CANMCR_SOFTRST;
	while ((MCF_FlexCAN_CANMCR & MCF_FlexCAN_CANMCR_SOFTRST) != 0x00)
		;
	MCF_FlexCAN_CANMCR = MCF_FlexCAN_CANMCR_MDIS | MCF_FlexCAN_CANMCR_MAXMB(0xF);
	while ((MCF_FlexCAN_CANMCR & MCF_FlexCAN_CANMCR_NOTRDY) !=
			MCF_FlexCAN_CANMCR_NOTRDY);


  #ifdef CAN_125K
	MCF_FlexCAN_CANCTRL = MCF_FlexCAN_CANCTRL_PRESDIV(23)|
					   MCF_FlexCAN_CANCTRL_RJW(3)|
					   MCF_FlexCAN_CANCTRL_PSEG1(4)|
					   MCF_FlexCAN_CANCTRL_PSEG2(3)|
					   MCF_FlexCAN_CANCTRL_ERRMSK|
					   MCF_FlexCAN_CANCTRL_BOFFMSK|
					   MCF_FlexCAN_CANCTRL_PROPSEG(5);
	#endif

   #ifdef CAN_500K
	MCF_FlexCAN_CANCTRL = MCF_FlexCAN_CANCTRL_PRESDIV(5)|
					   MCF_FlexCAN_CANCTRL_RJW(3)|
					   MCF_FlexCAN_CANCTRL_PSEG1(4)|
					   MCF_FlexCAN_CANCTRL_PSEG2(3)|
					   MCF_FlexCAN_CANCTRL_ERRMSK|
					   MCF_FlexCAN_CANCTRL_BOFFMSK|
					   MCF_FlexCAN_CANCTRL_PROPSEG(5);
	#endif
   #ifdef CAN_1000K
	MCF_FlexCAN_CANCTRL = MCF_FlexCAN_CANCTRL_PRESDIV(5)|
					   MCF_FlexCAN_CANCTRL_RJW(1)|
					   MCF_FlexCAN_CANCTRL_PSEG1(1)|
					   MCF_FlexCAN_CANCTRL_PSEG2(1)|
					   MCF_FlexCAN_CANCTRL_ERRMSK|
					   MCF_FlexCAN_CANCTRL_BOFFMSK|
					   MCF_FlexCAN_CANCTRL_PROPSEG(2);
	#endif

/* //Enable this for running off of system bus frequency of 40Mhz
	MCF_FlexCAN_CANCTRL = MCF_FlexCAN_CANCTRL_PRESDIV(5)| //change divider to 4+1 for 40Mhz bus clock
					   MCF_FlexCAN_CANCTRL_RJW(1)|
					   MCF_FlexCAN_CANCTRL_PSEG1(1)|
					   MCF_FlexCAN_CANCTRL_PSEG2(1)|
					   MCF_FlexCAN_CANCTRL_CLK_SRC |   //clock from bus
					   MCF_FlexCAN_CANCTRL_ERRMSK|
					   MCF_FlexCAN_CANCTRL_BOFFMSK|
					   MCF_FlexCAN_CANCTRL_PROPSEG(2);
*/


	/*
     * MBuf Initialization
 	 */
	MCF_FLEXCAN_BUFFERS.MB0.u16CtrlStat = MB_CTRLSTAT_CODE(0x0);
	MCF_FLEXCAN_BUFFERS.MB1.u16CtrlStat = MB_CTRLSTAT_CODE(0x0);
	MCF_FLEXCAN_BUFFERS.MB2.u16CtrlStat = MB_CTRLSTAT_CODE(0x0);
	MCF_FLEXCAN_BUFFERS.MB3.u16CtrlStat = MB_CTRLSTAT_CODE(0x0);
	MCF_FLEXCAN_BUFFERS.MB4.u16CtrlStat = MB_CTRLSTAT_CODE(0x0);
	MCF_FLEXCAN_BUFFERS.MB5.u16CtrlStat = MB_CTRLSTAT_CODE(0x0);
	MCF_FLEXCAN_BUFFERS.MB6.u16CtrlStat = MB_CTRLSTAT_CODE(0x0);
	MCF_FLEXCAN_BUFFERS.MB7.u16CtrlStat = MB_CTRLSTAT_CODE(0x0);
	MCF_FLEXCAN_BUFFERS.MB8.u16CtrlStat = MB_CTRLSTAT_CODE(0x0);
	MCF_FLEXCAN_BUFFERS.MB9.u16CtrlStat = MB_CTRLSTAT_CODE(0x0);
	MCF_FLEXCAN_BUFFERS.MB10.u16CtrlStat = MB_CTRLSTAT_CODE(0x0);
	MCF_FLEXCAN_BUFFERS.MB11.u16CtrlStat = MB_CTRLSTAT_CODE(0x0);
	MCF_FLEXCAN_BUFFERS.MB12.u16CtrlStat = MB_CTRLSTAT_CODE(0x0);
	MCF_FLEXCAN_BUFFERS.MB13.u16CtrlStat = MB_CTRLSTAT_CODE(0x0);
	MCF_FLEXCAN_BUFFERS.MB14.u16CtrlStat = MB_CTRLSTAT_CODE(0x0);
	MCF_FLEXCAN_BUFFERS.MB15.u16CtrlStat = MB_CTRLSTAT_CODE(0x0);

	MCF_FlexCAN_RXGMASK = 0xFFFFFFFF;

    MCF_FLEXCAN_BUFFERS.MB1.u16CtrlStat = MB_CTRLSTAT_CODE(0x0);
    MCF_FLEXCAN_BUFFERS.MB1.u32ID.u32ExtID = MB_STANDARD_ID(0x140);	/* ID 0x140 */
    MCF_FLEXCAN_BUFFERS.MB1.u16CtrlStat = MB_CTRLSTAT_CODE(0x4);

 	MCF_FlexCAN_CANMCR &= ~(MCF_FlexCAN_CANMCR_MDIS);
	while ((MCF_FlexCAN_CANMCR & MCF_FlexCAN_CANMCR_NOTRDY) != 0x00)
		;

    MCF_FlexCAN_IMASK = 0x00;

    MCF_INTC1_ICR08 = MCF_INTC_ICR_IL(2) | MCF_INTC_ICR_IP(0);
    MCF_INTC1_ICR09 = MCF_INTC_ICR_IL(2) | MCF_INTC_ICR_IP(1);
    MCF_INTC1_ICR10 = MCF_INTC_ICR_IL(2) | MCF_INTC_ICR_IP(2);
    MCF_INTC1_ICR11 = MCF_INTC_ICR_IL(2) | MCF_INTC_ICR_IP(3);
    MCF_INTC1_ICR12 = MCF_INTC_ICR_IL(2) | MCF_INTC_ICR_IP(4);
    MCF_INTC1_ICR13 = MCF_INTC_ICR_IL(2) | MCF_INTC_ICR_IP(5);
    MCF_INTC1_ICR14 = MCF_INTC_ICR_IL(2) | MCF_INTC_ICR_IP(6);
    MCF_INTC1_ICR15 = MCF_INTC_ICR_IL(2) | MCF_INTC_ICR_IP(7);
    MCF_INTC1_ICR16 = MCF_INTC_ICR_IL(4) | MCF_INTC_ICR_IP(0);
    MCF_INTC1_ICR17 = MCF_INTC_ICR_IL(4) | MCF_INTC_ICR_IP(1);
    MCF_INTC1_ICR18 = MCF_INTC_ICR_IL(4) | MCF_INTC_ICR_IP(2);
    MCF_INTC1_ICR19 = MCF_INTC_ICR_IL(4) | MCF_INTC_ICR_IP(3);
    MCF_INTC1_ICR20 = MCF_INTC_ICR_IL(4) | MCF_INTC_ICR_IP(4);
    MCF_INTC1_ICR21 = MCF_INTC_ICR_IL(4) | MCF_INTC_ICR_IP(5);
    MCF_INTC1_ICR22 = MCF_INTC_ICR_IL(4) | MCF_INTC_ICR_IP(6);
    MCF_INTC1_ICR23 = MCF_INTC_ICR_IL(4) | MCF_INTC_ICR_IP(7);
    MCF_INTC1_ICR24 = MCF_INTC_ICR_IL(5) | MCF_INTC_ICR_IP(0);
    MCF_INTC1_ICR25 = MCF_INTC_ICR_IL(5) | MCF_INTC_ICR_IP(1);


    MCF_INTC1_IMRL &= ~(MCF_INTC_IMRL_INT_MASK25|MCF_INTC_IMRL_INT_MASK24|
                       MCF_INTC_IMRL_INT_MASK23|MCF_INTC_IMRL_INT_MASK22|
                       MCF_INTC_IMRL_INT_MASK21|MCF_INTC_IMRL_INT_MASK20|
                       MCF_INTC_IMRL_INT_MASK19|MCF_INTC_IMRL_INT_MASK18|
                       MCF_INTC_IMRL_INT_MASK17|MCF_INTC_IMRL_INT_MASK16|
                       MCF_INTC_IMRL_INT_MASK15|MCF_INTC_IMRL_INT_MASK14|
                       MCF_INTC_IMRL_INT_MASK13|MCF_INTC_IMRL_INT_MASK12|
                       MCF_INTC_IMRL_INT_MASK11|MCF_INTC_IMRL_INT_MASK10|
                       MCF_INTC_IMRL_INT_MASK9 |MCF_INTC_IMRL_INT_MASK8 |
                       MCF_INTC_IMRL_MASKALL);


    return;
}
/********************************************************************/
/* Send Data via Polling
 *
 * Parameters:
 *  pData	Pointer to data to send
 *  u8Size	Data size
 *  u32ID	Destination ID
 *	u8MB	Message Buffer
 *
 * Return Value:
 *  Success (0) or Failure (1)
 */
int8 FlexCANSendDataPoll(uint8 *pData, uint8 u8Size, uint32 u32ID, uint8 u8MB)
{
    uint8 u8Counter;
    uint8 *pDataPointer;
    uint32 u32Rescue = 0;
    tFlexCANMB *BuffPtr;
    uint16 temp = 0;

   	/*Set the buffer pointer*/
    BuffPtr =  &MCF_FLEXCAN_BUFFERS.MB0 + u8MB;

    (*BuffPtr).u16CtrlStat = MB_CTRLSTAT_CODE(0x8);
    if (!(u32ID & FLEXCAN_EXTENDEDID(0))){
        /* Standard ID */
        (*BuffPtr).u32ID.u32ExtID = MB_STANDARD_ID(u32ID);
    }
    else{
        (*BuffPtr).u32ID.u32ExtID = MB_EXTENDED_ID(u32ID - FLEXCAN_EXTENDEDID(0));
        temp |= (MB_CTRLSTAT_SRR | MB_CTRLSTAT_IDE);
    }
    pDataPointer = &((*BuffPtr).u8Data0);
    for (u8Counter = 0; u8Counter < u8Size; u8Counter++)
    {
        *(pDataPointer++) = *(pData++);
    }
    temp |= MB_CTRLSTAT_LENGTH(u8Size);
    (*BuffPtr).u16CtrlStat = MB_CTRLSTAT_CODE(0xC)|temp;


    while (!(MCF_FlexCAN_IFLAG & (1<<u8MB))){
        if (u32Rescue++ == RESCUE_VALUE){
            return 1;
        }
    }
    MCF_FlexCAN_IFLAG = (1<<u8MB);

     return 0;
}
/********************************************************************/
/* Send Data without Polling
 *
 * Parameters:
 *  pData	Pointer to data to send
 *  u8Size	Data size
 *  u32ID	Destination ID
 *	u8MB	Message Buffer
 *
 * Return Value:
 *  None
 */
void FlexCANSendDataNoPoll(uint8 *pData, uint8 u8Size, uint32 u32ID, uint8 u8MB)
{
    uint8 u8Counter;
    uint8 *pDataPointer;
    uint32 u32Rescue = 0;
    tFlexCANMB *BuffPtr;
    uint16 temp = 0;

    BuffPtr =  &MCF_FLEXCAN_BUFFERS.MB0 + u8MB;

    (*BuffPtr).u16CtrlStat = MB_CTRLSTAT_CODE(0x8);
    if (!(u32ID & FLEXCAN_EXTENDEDID(0))){
        /* Standard ID */
        (*BuffPtr).u32ID.u32ExtID = MB_STANDARD_ID(u32ID);
    }
    else{
        (*BuffPtr).u32ID.u32ExtID = MB_EXTENDED_ID(u32ID - FLEXCAN_EXTENDEDID(0));
        temp |= (MB_CTRLSTAT_SRR | MB_CTRLSTAT_IDE);
    }
    pDataPointer = &((*BuffPtr).u8Data0);
    for (u8Counter = 0; u8Counter < u8Size; u8Counter++)
    {
        *(pDataPointer++) = *(pData++);
    }
    temp |= MB_CTRLSTAT_LENGTH(u8Size);
    (*BuffPtr).u16CtrlStat = MB_CTRLSTAT_CODE(0xC)|temp;

     return;
}
/********************************************************************/
/* Receive Data via Polling
 *
 * Parameters:
 *  u8MB	Message Buffer
 *  aData	Pointer to destination address for data
 *
 * Return Value:
 *  Length of data received
 */
uint8 FlexCANReceiveDataPoll(uint8 u8MB, uint8 *aData)
{
    uint8 u8Temp;
    uint8 *pPointer;
    uint8 i;
    tFlexCANMB *BuffPtr;
    uint8 u8Len;

    BuffPtr =  &MCF_FLEXCAN_BUFFERS.MB0 + u8MB;

    /*Wait for data received*/
    while (!(MCF_FlexCAN_IFLAG&(1<<u8MB)));

    MCF_FlexCAN_IFLAG = (1<<u8MB);
    u8Temp = (*BuffPtr).u16CtrlStat;
    pPointer = &((*BuffPtr).u8Data0);
    u8Len = ((*BuffPtr).u16CtrlStat) & MB_CTRLSTAT_LENGTH(0xF);
    for (i=0; i < u8Len; i++){
        aData[i] = pPointer[i];
    }

    (*BuffPtr).u16CtrlStat = MB_CTRLSTAT_CODE(0x4);

    return u8Len;

}
/********************************************************************/
/* Set Message Buffer for Receive
 *
 * Parameters:
 *  u8MB	Message Buffer
 *  u32ID	FlexCAN ID to receive from
 *
 * Return Value:
 *  0
 */
uint8 FlexCANSetMBforRx(uint8 u8MB, uint32 u32ID)
{
    tFlexCANMB *BuffPtr;
   uint16 temp = 0;

    BuffPtr =  &MCF_FLEXCAN_BUFFERS.MB0 + u8MB;
    (*BuffPtr).u16CtrlStat = MB_CTRLSTAT_CODE(0x00);

    if (!(u32ID & FLEXCAN_EXTENDEDID(0))){
        /* Standard ID */
        (*BuffPtr).u32ID.u32ExtID = MB_STANDARD_ID(u32ID);
    }
    else{
        (*BuffPtr).u32ID.u32ExtID = MB_EXTENDED_ID(u32ID - FLEXCAN_EXTENDEDID(0));
        temp |= (MB_CTRLSTAT_SRR | MB_CTRLSTAT_IDE);
    }

    (*BuffPtr).u16CtrlStat = MB_CTRLSTAT_CODE(0x4)|temp;

    return 0;

}
/********************************************************************/
/* Generic Receive ISR
 *
 * Parameters:
 *  u8MB	Message Buffer
 *
 * Return Value:
 *  None
 */
void FlexCANGenericRXISR(uint8 u8MB){
    tFlexCANMB *BuffPtr;
    uint8 i;
    uint16 u16Temp;
    uint8 *pPointer;

     BuffPtr =  &MCF_FLEXCAN_BUFFERS.MB0 + u8MB;

    u16Temp = ((*BuffPtr).u16CtrlStat)&(MB_CTRLSTAT_IDE|MB_CTRLSTAT_SRR);
    pPointer = &((*BuffPtr).u8Data0);
    gu8FlexCANLen[u8MB] = ((*BuffPtr).u16CtrlStat) & MB_CTRLSTAT_LENGTH(0xF);
    for (i=0; i < gu8FlexCANLen[u8MB]; i++){
        gu8FlexCANArrays[u8MB][i] = pPointer[i];
    }

    /*Set Interrupt Semaphore*/
     gu8FlexCANIntSemaphore[u8MB] = 1;

    (*BuffPtr).u32ID.u32ExtID = (*BuffPtr).u32ID.u32ExtID;
    (*BuffPtr).u16CtrlStat = MB_CTRLSTAT_CODE(0x4)|u16Temp;

}
/********************************************************************/
/* Generic Transmit ISR
 *
 * Parameters:
 *  u8MB	Message Buffer
 *
 * Return Value:
 *  None
 */
void FlexCANGenericTXISR(uint8 u8MB){
    tFlexCANMB *BuffPtr;

    BuffPtr =  &MCF_FLEXCAN_BUFFERS.MB0 + u8MB;
    gu8FlexCANLen[u8MB] = ((*BuffPtr).u16CtrlStat) & MB_CTRLSTAT_LENGTH(0xF);

    /*Set Interrupt Semaphore*/
    gu8FlexCANIntSemaphore[u8MB] = 1;

}
/********************************************************************/
/* Enable Message Buffer Interrupt
 *
 * Parameters:
 *  u8MB	Message Buffer
 *	u8Mode	Select TX or RX
 *
 * Return Value:
 *  None
 */
void FlexCANEnableMBInterrupt(uint8 u8MB, uint8 u8Mode)
{
    MCF_FlexCAN_IMASK |= (1<<u8MB);
    if (u8Mode == 0x00){
        flexcanMBISR [u8MB] = FlexCANGenericRXISR;
    }
    else{
      flexcanMBISR [u8MB] = FlexCANGenericTXISR;
    }
}
/********************************************************************/
/*
 * Disable a specific message buffer interrupt
 */
void FlexCANDisableMBInterrupt(uint8 u8MB)
{
    MCF_FlexCAN_IMASK &= ~(1<<u8MB);
}
/********************************************************************/
/*
 * Disable All FlexCAN Interrupts
 */
void FlexCANDisableALLInterrupt(void)
{
    MCF_FlexCAN_IMASK = 0x00;
    MCF_FlexCAN_IFLAG = 0xFF;
    MCF_FlexCAN_CANCTRL &= ~(MCF_FlexCAN_CANCTRL_BOFFMSK|MCF_FlexCAN_CANCTRL_ERRMSK);
    MCF_FlexCAN_ERRSTAT |= (MCF_FlexCAN_ERRSTAT_ERRINT|MCF_FlexCAN_ERRSTAT_BOFFINT);
}
/********************************************************************/
/* Get Data from Receive Interrupt
 *
 * Parameters:
 *  u8MB	Message Buffer
 *	aData	Pointer to destination address for data
 *
 * Return Value:
 *  None
 */
uint8 FlexCANGetRXDataInt(uint8 u8MB, uint8 *aData){
    uint8 i;
    for (i=0; i < gu8FlexCANLen[u8MB]; i++){
        aData[i] = gu8FlexCANArrays[u8MB][i];
    }

    return gu8FlexCANLen[u8MB];

}
/********************************************************************/
/*
 * Wait for an interrupt on a particular MB
 */
void FlexCANWaitMBInt(uint8 u8MB)
{
	/*Wait for interrupt semaphore to be set*/
    while (gu8FlexCANIntSemaphore[u8MB] == 0x00);

    /*Reset to 0*/
    gu8FlexCANIntSemaphore[u8MB] = 0x00;

}
/********************************************************************/
/* Wait for a Message Buffer Interrupt on any MB
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  Message Buffer interrupt occured on
 */
uint8 FlexCANWaitALLInt(void)
{
    uint8 i = 0;

    do {
    	/*Search through MB's for a set interrupt semaphore*/
        for (i=0; i < 16; i++){
          if (gu8FlexCANIntSemaphore[i] != 0x00) {
            gu8FlexCANIntSemaphore[i] = 0x00;
            return i;
          }

        }
    }while (1);
}
/********************************************************************/
/*
 * Disable Message Buffers
 */
void FlexCANDisableMBs(void)
{
    uint8 i;
    tFlexCANMB *BuffPtr;

    BuffPtr =  &MCF_FLEXCAN_BUFFERS.MB0;

   	/*Iterate through all MB's to turn off*/
    for (i = 0; i < 15; i++){
       (*BuffPtr++).u16CtrlStat = MB_CTRLSTAT_CODE(0x00);
    }

}
/********************************************************************/
/* Set Mask
 *
 * Parameters:
 *  u8MB	 Message Buffer
 *	u32Mask  Mask
 *
 * Return Value:
 *  None
 */
void FlexCANSetMask(uint8 u8MB, uint32 u32Mask){
    if (u8MB < 14){
        MCF_FlexCAN_RXGMASK = u32Mask;
    }

    if (u8MB == 14){
        MCF_FlexCAN_RX14MASK = u32Mask;
    }
    if (u8MB == 15){
        MCF_FlexCAN_RX15MASK = u32Mask;
    }

}
/********************************************************************/
/*
 * Reset Masks
 */
void FlexCANResetMasks(void){
   	MCF_FlexCAN_RXGMASK = 0xFFFFFFFF;
   	MCF_FlexCAN_RX14MASK = 0xFFFFFFFF;
   	MCF_FlexCAN_RX15MASK = 0xFFFFFFFF;
}


/* Buf 0 used for TX */
__declspec(interrupt) void FlexCANBuf0(){
	MCF_FlexCAN_IFLAG = MCF_FlexCAN_IFLAG_BUF0I;
    (flexcanMBISR[0])(0);


}

/* Buf 1 used for TX */
__declspec(interrupt) void FlexCANBuf1(){
	MCF_FlexCAN_IFLAG = MCF_FlexCAN_IFLAG_BUF1I;
    (flexcanMBISR[1])(1);


}

/* Buf 2 used for TX */
__declspec(interrupt) void FlexCANBuf2(){
	MCF_FlexCAN_IFLAG = MCF_FlexCAN_IFLAG_BUF2I;
    (flexcanMBISR[2])(2);


}

/* Buf 3 used for TX */
__declspec(interrupt) void FlexCANBuf3(){
	MCF_FlexCAN_IFLAG = MCF_FlexCAN_IFLAG_BUF3I;
    (flexcanMBISR[3])(3);


}

/* Buf 4 used for TX */
__declspec(interrupt) void FlexCANBuf4(){
	MCF_FlexCAN_IFLAG = MCF_FlexCAN_IFLAG_BUF4I;
    (flexcanMBISR[4])(4);


}

/* Buf 5 used for TX */
__declspec(interrupt) void FlexCANBuf5(){
	MCF_FlexCAN_IFLAG = MCF_FlexCAN_IFLAG_BUF5I;
    (flexcanMBISR[5])(5);


}

/* Buf 6 used for TX */
__declspec(interrupt) void FlexCANBuf6(){
	MCF_FlexCAN_IFLAG = MCF_FlexCAN_IFLAG_BUF6I;
    (flexcanMBISR[6])(6);


}

/* Buf 7 used for TX */
__declspec(interrupt) void FlexCANBuf7(){
	MCF_FlexCAN_IFLAG = MCF_FlexCAN_IFLAG_BUF7I;
    (flexcanMBISR[7])(7);


}

/* Buf 8 used for TX */
__declspec(interrupt) void FlexCANBuf8(){
	MCF_FlexCAN_IFLAG = MCF_FlexCAN_IFLAG_BUF8I;
    (flexcanMBISR[8])(8);


}

/* Buf 9 used for TX */
__declspec(interrupt) void FlexCANBuf9(){
	MCF_FlexCAN_IFLAG = MCF_FlexCAN_IFLAG_BUF9I;
    (flexcanMBISR[9])(9);


}

/* Buf 10 used for TX */
__declspec(interrupt) void FlexCANBuf10(){
	MCF_FlexCAN_IFLAG = MCF_FlexCAN_IFLAG_BUF10I;
     (flexcanMBISR[10])(10);


}

/* Buf 11 used for TX */
__declspec(interrupt) void FlexCANBuf11(){
	MCF_FlexCAN_IFLAG = MCF_FlexCAN_IFLAG_BUF11I;
    (flexcanMBISR[11])(11);



}

/* Buf 12 used for TX */
__declspec(interrupt) void FlexCANBuf12(){
	MCF_FlexCAN_IFLAG = MCF_FlexCAN_IFLAG_BUF12I;
    (flexcanMBISR[12])(12);



}

/* Buf 13 used for TX */
__declspec(interrupt) void FlexCANBuf13(){
	MCF_FlexCAN_IFLAG = MCF_FlexCAN_IFLAG_BUF13I;
    (flexcanMBISR[13])(13);


}

/* Buf 14 used for TX */
__declspec(interrupt) void FlexCANBuf14(){
	MCF_FlexCAN_IFLAG = MCF_FlexCAN_IFLAG_BUF14I;
    (flexcanMBISR[14])(14);


}

/* Buf 15 used for TX */
__declspec(interrupt) void FlexCANBuf15(){
	MCF_FlexCAN_IFLAG = MCF_FlexCAN_IFLAG_BUF15I;
    (flexcanMBISR[15])(15);


}

/* Error Interrupt */
__declspec(interrupt) void FlexCANERR_INT()
{
      gu32ErrFlg = MCF_FlexCAN_ERRSTAT;
      MCF_FlexCAN_ERRSTAT = MCF_FlexCAN_ERRSTAT_ERRINT;
    printf("FlexCAN Error Interrupt\n\r");

	return;
}

/* Buss-Off Interrupt */
__declspec(interrupt) void FlexCANBOFF_INT()
{
	gu32ErrFlg = MCF_FlexCAN_ERRSTAT;
    MCF_FlexCAN_ERRSTAT = MCF_FlexCAN_ERRSTAT_BOFFINT;
    printf("FlexCAN Bus Off Interrupt\n\r");

	return;
}
/********************************************************************/
/* Get/Clear Error Status
 *
 * Parameters:
 *  uClr	Clear Error Flag
 *
 * Return Value:
 *  Error Flag
 */
uint32 FlexCANGetErrStat(uint8 u8Clr){
    uint32 u32temp = gu32ErrFlg;

    if (u8Clr){
        gu32ErrFlg = 0;
    }
    return u32temp;
}
/********************************************************************/
/*
 * Example on how to set the baud rate
 */
int8 FlexCANSetBaudrate(uint32 u32Baudrate, uint8 u8ClkSource)
{
	MCF_FlexCAN_CANMCR = MCF_FlexCAN_CANMCR_MDIS | MCF_FlexCAN_CANMCR_MAXMB(0xF);
	while ((MCF_FlexCAN_CANMCR & MCF_FlexCAN_CANMCR_NOTRDY) !=
			MCF_FlexCAN_CANMCR_NOTRDY);
    switch (u32Baudrate){
        case 125000:
            if (!u8ClkSource ){
        	    MCF_FlexCAN_CANCTRL = MCF_FlexCAN_CANCTRL_PRESDIV(23)|
        					   MCF_FlexCAN_CANCTRL_RJW(3)|
        					   MCF_FlexCAN_CANCTRL_PSEG1(4)|
        					   MCF_FlexCAN_CANCTRL_PSEG2(3)|
        					   MCF_FlexCAN_CANCTRL_PROPSEG(5);
             }
             else{
        	MCF_FlexCAN_CANCTRL = MCF_FlexCAN_CANCTRL_PRESDIV(19)|
        					   MCF_FlexCAN_CANCTRL_RJW(3)|
        					   MCF_FlexCAN_CANCTRL_PSEG1(4)|
        					   MCF_FlexCAN_CANCTRL_PSEG2(3)|
        					   MCF_FlexCAN_CANCTRL_PROPSEG(5)|
        					   MCF_FlexCAN_CANCTRL_CLK_SRC;

             }
        break;
        case 250000:
             if (!u8ClkSource ){
               	MCF_FlexCAN_CANCTRL = MCF_FlexCAN_CANCTRL_PRESDIV(11)|
					   MCF_FlexCAN_CANCTRL_RJW(3)|
					   MCF_FlexCAN_CANCTRL_PSEG1(4)|
					   MCF_FlexCAN_CANCTRL_PSEG2(3)|
					   MCF_FlexCAN_CANCTRL_PROPSEG(5);
            }
            else{
        	MCF_FlexCAN_CANCTRL = MCF_FlexCAN_CANCTRL_PRESDIV(9)|
        					   MCF_FlexCAN_CANCTRL_RJW(3)|
        					   MCF_FlexCAN_CANCTRL_PSEG1(4)|
        					   MCF_FlexCAN_CANCTRL_PSEG2(3)|
        					   MCF_FlexCAN_CANCTRL_PROPSEG(5)|
        					   MCF_FlexCAN_CANCTRL_CLK_SRC;

             }
       break;
        case 500000:
            if (!u8ClkSource ){
            	MCF_FlexCAN_CANCTRL = MCF_FlexCAN_CANCTRL_PRESDIV(5)|
					   MCF_FlexCAN_CANCTRL_RJW(3)|
					   MCF_FlexCAN_CANCTRL_PSEG1(4)|
					   MCF_FlexCAN_CANCTRL_PSEG2(3)|
					   MCF_FlexCAN_CANCTRL_PROPSEG(5);
            }
            else{
        	MCF_FlexCAN_CANCTRL = MCF_FlexCAN_CANCTRL_PRESDIV(4)|
        					   MCF_FlexCAN_CANCTRL_RJW(3)|
        					   MCF_FlexCAN_CANCTRL_PSEG1(4)|
        					   MCF_FlexCAN_CANCTRL_PSEG2(3)|
        					   MCF_FlexCAN_CANCTRL_PROPSEG(5)|
        					   MCF_FlexCAN_CANCTRL_CLK_SRC;
             }
        break;
        case 1000000:
             if (!u8ClkSource ){
               	MCF_FlexCAN_CANCTRL = MCF_FlexCAN_CANCTRL_PRESDIV(5)|
    					   MCF_FlexCAN_CANCTRL_RJW(1)|
    					   MCF_FlexCAN_CANCTRL_PSEG1(1)|
    					   MCF_FlexCAN_CANCTRL_PSEG2(1)|
    					   MCF_FlexCAN_CANCTRL_PROPSEG(2);
             }
            else{
               	MCF_FlexCAN_CANCTRL = MCF_FlexCAN_CANCTRL_PRESDIV(4)|
    					   MCF_FlexCAN_CANCTRL_RJW(1)|
    					   MCF_FlexCAN_CANCTRL_PSEG1(1)|
    					   MCF_FlexCAN_CANCTRL_PSEG2(1)|
    					   MCF_FlexCAN_CANCTRL_PROPSEG(2)|
    					   MCF_FlexCAN_CANCTRL_CLK_SRC;
             }
         break;
        default:
          	MCF_FlexCAN_CANMCR &= ~(MCF_FlexCAN_CANMCR_MDIS);
            while ((MCF_FlexCAN_CANMCR & MCF_FlexCAN_CANMCR_NOTRDY) != 0x00)
        	;
               return -1;
        break;
    }
 	MCF_FlexCAN_CANMCR &= ~(MCF_FlexCAN_CANMCR_MDIS);
    while ((MCF_FlexCAN_CANMCR & MCF_FlexCAN_CANMCR_NOTRDY) != 0x00)
	;

    return 0;
}