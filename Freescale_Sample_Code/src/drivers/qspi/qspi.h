/*
 * File:    qspi.h
 * Purpose: Header for the Queued Serial Peripheral Interface
 * 
 * License:     All software covered by license agreement in -
 *              docs/Freescale_Software_License.pdf
 */
 
 
#ifndef _QSPI_H_
#define _QSPI_H_

#include "common.h"


/*
 * QSPI Macros
 */
#define QSPI_GPIO_INIT()  MCF_GPIO_PQSPAR = 0x1555;
#define QSPI_ENABLE_SPIF()  (MCF_QSPI_QIR |= MCF_QSPI_QIR_SPIFE)
#define QSPI_DISABLE_SPIF()  (MCF_QSPI_QIR &= ~MCF_QSPI_QIR_SPIFE)
#define QSPI_ENABLE_ABRT()  (MCF_QSPI_QIR |= MCF_QSPI_QIR_ABRTE)
#define QSPI_DISABLE_ABRT()  (MCF_QSPI_QIR &= ~MCF_QSPI_QIR_ABRTE)
#define QSPI_ENABLE_WCEF()  (MCF_QSPI_QIR |= MCF_QSPI_QIR_WCEFE)
#define QSPI_DISABLE_WCEF()  (MCF_QSPI_QIR &= ~MCF_QSPI_QIR_WCEFE)
#define QSPI_ENABLE_ABRTLOCK() (MCF_QSPI_QIR |= MCF_QSPI_QIR_ABRTL)
#define QSPI_DISABLE_ABRTLOCK() (MCF_QSPI_QIR &= ~MCF_QSPI_QIR_ABRTL)
#define QSPI_ABORT_TX_SPE() (MCF_QSPI_QDLYR &= ~MCF_QSPI_QDLYR_SPE)



/*
 * Constants
 */
#define QSPI_IDLE   0x00
#define QSPI_BUSY   0x01

#define QSPI_BUFFSTAT_IDLE      0x00
#define QSPI_BUFFSTAT_TXRDY     0x01
#define QSPI_BUFFSTAT_BUSY      0x02
#define QSPI_BUFFSTAT_RXRDY     0x03
#define QSPI_BUFFSTAT_ABORTED   0x04
#define QSPI_TRANSMIT_ADDRESS 	0x00
#define QSPI_RECEIVE_ADDRESS 	0x10
#define QSPI_COMMAND_ADDRESS 	0x20

/*
 * QSPI Buffer structure definition
 */
typedef struct tQSPIBuffers
{
    uint8 u8Size;         
    uint16 *pu16TxData;
    uint16 *pu16RxData;
    uint8  *pu8Cmd;
    uint8 u8Stat;
}tQSPIBuffers;


/********************************************************************/
/*
 * Functions provided by this driver
 */
int8 QSPIInit(uint16 u16Baudrate, uint8 u8ClkAttrib, 
              uint8 u8Bits, uint8 u8ClkDly, uint8 u8DlyAft, uint8 u8Pol);
int8 QSPISetBaudrate(uint16 u16Baudrate);
void QSPISendByteRaw(uint8 u8Byte, uint8 u8CS);
void QSPISendWordRaw(uint16 u16Word, uint8 u8CS);
uint8 QSPIPollTransferByteRaw(uint8 u8Byte, uint8 u8CS);
int8 QSPISetBits(uint8 u8NumberofBits);

struct tQSPIBuffers* QSPI_InitFullBuffer(uint8 u8Size);
struct tQSPIBuffers* QSPI_InitSimpleBuffer(uint8 u8Size, uint16 *pu16TxData, uint16 *pu16RxData);
int8 QSPI_FreeFullBuffer(tQSPIBuffers *sQSPIBuff);
int8 QSPI_FreeSimpleBuffer(tQSPIBuffers *sQSPIBuff);

int8 QSPIIntBufferTransfer(tQSPIBuffers *sQSPIBuff);

uint8 QSPI_IntStat(void);

void QSPI_Enable_Abort(void);
void QSPI_Disable_Abort(void);

__interrupt__ void QSPI_ISR();

#endif /* _QSPI_H_ */