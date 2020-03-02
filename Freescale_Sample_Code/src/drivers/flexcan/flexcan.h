/*
 *
 * File name   : flexcan.h
 * Description : FlexCAN Example 
 *               
 * License:     All software covered by license agreement in -
 *              docs/Freescale_Software_License.pdf
 *
 *   27 March 2008 - Updated for Kirin3, corrected port location - M. Lobdell
 */
 
#ifndef _FLEXCAN_H_
#define _FLEXCAN_H_

#include "common.h"
#define MCF521x

#define FLEXCAN_GPIO_INIT()   MCF_GPIO_PUCPAR = 0 | MCF_GPIO_PUCPAR_URXD2_CANRX | MCF_GPIO_PUCPAR_UTXD2_CANTX;

#define FLEXCAN_EXTENDEDID(x) (x|0x80000000)   
#define FLEXCAN_STANDARDID(x) (x|0x00000000)   
#define RESCUE_VALUE          0x10000
#define MCF_FLEXCAN_BUFFERS	  (*(tFlexCANBuffers *)(void *)(&__IPSBAR[0x170080]))

    
/* 
 * Flexcan structs
 */
typedef union {
  uint32 u32ExtID;
      struct {
        uint16 u16StdID;
        uint16 u16StdTimeStamp;
      } u32StdIDField;
} tFlexCANID;

typedef struct tFlexCANMB
{
    uint16 u16CtrlStat;  
    uint16 u8TimeStamp;
    tFlexCANID u32ID;
    uint8 u8Data0;
    uint8 u8Data1;
    uint8 u8Data2;
    uint8 u8Data3;
    uint8 u8Data4;
    uint8 u8Data5;
    uint8 u8Data6;
    uint8 u8Data7;
}tFlexCANMB;

typedef struct tFlexCANBuffers
{
    tFlexCANMB MB0;     
    tFlexCANMB MB1;
    tFlexCANMB MB2;
    tFlexCANMB MB3;
    tFlexCANMB MB4;
    tFlexCANMB MB5;
    tFlexCANMB MB6;
    tFlexCANMB MB7;
    tFlexCANMB MB8;
    tFlexCANMB MB9;
    tFlexCANMB MB10;
    tFlexCANMB MB11;
    tFlexCANMB MB12;
    tFlexCANMB MB13;
    tFlexCANMB MB14;
    tFlexCANMB MB15;
}tFlexCANBuffers;

/*
 * Device specific defines
 */
#ifdef MCF521x
#define MB_CTRLSTAT_CODE(x)     ((x&0x0F)<<8)
#define MB_CTRLSTAT_LENGTH(x)   (x&0x0F)
#define MB_CTRLSTAT_RTR   		(0x10)
#define MB_CTRLSTAT_IDE   		(0x20)
#define MB_CTRLSTAT_SRR   		(0x40)
#define MB_STANDARD_ID(x)  		((x&0x7FF)<<18)
#define MB_EXTENDED_ID(x)  		((x&0x1FFFFFFF))
#endif

#ifdef MCF5282
#define MB_CTRLSTAT_CODE(x)     ((x&0x0F)<<4)
#define MB_CTRLSTAT_LENGTH(x)   (x&0x0F)
#define MB_EXTENDED_DATA_ID(x)  (((x&0x7FFF)<<1)|((x&0x38000)<<1)|((x&0x1FFC0000)<<3)|0x180000)
#define MB_EXTENDED_RTR_ID(x)   (((x&0x7FFF)<<1)|((x&0x38000)<<1)|((x&0x1FFC0000)<<3)|0x180001)
#define MB_STANDARD_DATA_ID(x)  ((x&0x7FF)<<5)
#define MB_STANDARD_RTR_ID(x)   (((x&0x7FF)<<5)|0x10)
#define STANDARD_MASK(x)        ((x&0x7FF)<<21)
#define EXTENDED_MASK(x)        (((x&0x7FFF)<<1)|((x&0x38000)<<1)|((x&0x1FFC0000)<<3))
#endif


/*
 * Flexcan Macros
 */

#define MCF_FlexCAN_ENABLE_LOOPBACK()   (MCF_FlexCAN_CANCTRL |= MCF_FlexCAN_CANCTRL_LPB)
#define MCF_FlexCAN_DISABLE_LOOPBACK()  (MCF_FlexCAN_CANCTRL &= ~MCF_FlexCAN_CANCTRL_LPB)
#define FLEXCAN_ERRORFLG            (MCF_FlexCAN_ERRSTAT)
#define FLEXCAN_CLEAR_ERR()         (MCF_FlexCAN_ERRSTAT = MCF_FlexCAN_ERRSTAT_ERRINT)
#define FLEXCAN_CLEAR_BOFF()        (MCF_FlexCAN_ERRSTAT = MCF_FlexCAN_ERRSTAT_BOFFINT)


/********************************************************************/
/*
 * Functions provided by this driver
 */
void FlexCANInit(void);
int8 FlexCANSendDataPoll(uint8 *pData, uint8 u8Size, uint32 u38ID, uint8 u8MB);
void FlexCANSendDataNoPoll(uint8 *pData, uint8 u8Size, uint32 u38ID, uint8 u8MB);
uint8 FlexCANReceiveDataPoll(uint8 u8MB, uint8 *aData);
void FlexCANWaitMBInt(uint8 u8MB);
uint8 FlexCANWaitALLInt(void);
uint8 FlexCANSetMBforRx(uint8 u8MB, uint32 u32ID);
int8 FlexCANSetBaudrate(uint32 u32Baudrate, uint8 u8ClkSource);
void FlexCANDisableMBs(void);
void FlexCANEnableMBInterrupt(uint8 u8MB, uint8 u8Mode);
void FlexCANDisableMBInterrupt(uint8 u8MB);
void FlexCANDisableALLInterrupt(void);
uint8 FlexCANGetRXDataInt(uint8 u8MB, uint8 *aData);
void FlexCANSetMask(uint8 u8MB, uint32 u32Mask);
void FlexCANResetMasks(void);

#endif /* _FLEXCAN_H_ */