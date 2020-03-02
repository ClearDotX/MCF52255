/*
 * File:    qspi.c
 * Purpose: Driver for the Queued Serial Peripheral Interface    
 * 
 * License:     All software covered by license agreement in -
 *              docs/Freescale_Software_License.pdf
 */

#include "qspi.h"


/*
 * Initialize Variables     
 */
static uint8 u8QSPIStat = QSPI_IDLE;
tQSPIBuffers *sQSPIInterruptBuf;

void (* QSPI_SPIF_ISR) (void);
void (* QSPI_ABRT_ISR) (void);
void QSPI_SPIF_Test_ISR(void);
void QSPI_ABRT_Test_ISR(void);
static void QSPISetTransferCommand (unsigned char u8CS, unsigned char u8Cont);
static void QSPISetTransferData (unsigned char u8Data);
static int8 QSPIPollBufferTransfer_NoWrap(tQSPIBuffers *sQSPIBuff);
static int8 QSPIPollBufferTransfer_Wrap(tQSPIBuffers *sQSPIBuff);

/********************************************************************/
/* 
 * Initialize the QSPI
 */
int8 QSPIInit(uint16 u16Baudrate, uint8 u8ClkAttrib, 
              uint8 u8Bits, uint8 u8ClkDly, uint8 u8DlyAft, uint8 u8Pol) 
{
    int8 i8Ret=0;
    
    QSPI_GPIO_INIT();
    
    
    /* Set as a Master always and set CPOL & CPHA */
    MCF_QSPI_QMR = (MCF_QSPI_QMR_MSTR |         
                        ((u8ClkAttrib&0x3)<<8));    
    i8Ret |= QSPISetBits(u8Bits);
    i8Ret |= QSPISetBaudrate(u16Baudrate);
        MCF_QSPI_QDLYR = (MCF_QSPI_QDLYR_QCD(u8ClkDly) | 
                          MCF_QSPI_QDLYR_DTL(u8DlyAft));
        MCF_QSPI_QIR = (MCF_QSPI_QIR_WCEFB | MCF_QSPI_QIR_ABRTB |
                            MCF_QSPI_QIR_ABRTL | MCF_QSPI_QIR_WCEF |               
                            MCF_QSPI_QIR_ABRT | MCF_QSPI_QIR_SPIF);
        MCF_QSPI_QWR = MCF_QSPI_QWR_CSIV&u8Pol;
    
    /* Set interrupts */
    
    MCF_INTC0_ICR18 = MCF_INTC_ICR_IL(3)|MCF_INTC_ICR_IP(6);
    
    MCF_INTC0_IMRH &= ~(0);
    MCF_INTC0_IMRL &= ~(MCF_INTC_IMRL_INT_MASK18|MCF_INTC_IMRL_MASKALL);

    
    return i8Ret;
}

/********************************************************************/
/* 
 * Set the Baudrate
 */
int8 QSPISetBaudrate(uint16 u16Baudrate)
{
    uint16 u16Baud; 
    
    u16Baud = ((SYS_CLK_KHZ/2)/u16Baudrate);
     
    if ((u16Baud> 1 ) && (u16Baud < 255)){
         /* Set baudrate if parameter is OK */
         MCF_QSPI_QMR &= ~MCF_QSPI_QMR_BAUD(0xFF);
         MCF_QSPI_QMR |= MCF_QSPI_QMR_BAUD((uint8)u16Baud);
        return 0;
    }
    else
        return -1;
}

/********************************************************************/
/* 
 * Set Bits
 */
int8 QSPISetBits(uint8 u8NumberofBits)
{
    uint8 u8Bits; 
    
    if (u8NumberofBits >7 && u8NumberofBits < 17){
        u8Bits = (u8NumberofBits&0x0F);
        MCF_QSPI_QMR &= ~MCF_QSPI_QMR_BITS(0xF);
        MCF_QSPI_QMR |= MCF_QSPI_QMR_BITS(u8Bits);
        return 0;
    }
    else{
        return -1;
    }
}

/********************************************************************/
/* 
 * Send Byte
 */
void QSPISendByteRaw(uint8 u8Byte, uint8 u8CS)
{

    MCF_QSPI_QAR = QSPI_COMMAND_ADDRESS;
    QSPISetTransferCommand(u8CS, 0);
    MCF_QSPI_QAR = QSPI_TRANSMIT_ADDRESS;
    QSPISetTransferData(u8Byte);
    
    MCF_QSPI_QWR =  MCF_QSPI_QWR_CSIV|MCF_QSPI_QWR_ENDQP(0x00)|MCF_QSPI_QWR_NEWQP(0x00);
    MCF_QSPI_QDLYR |= MCF_QSPI_QDLYR_SPE;
    
}

/********************************************************************/
/* 
 * Send Word
 */
void QSPISendWordRaw(uint16 u16Word, uint8 u8CS)
{

    MCF_QSPI_QAR = QSPI_COMMAND_ADDRESS;
    QSPISetTransferCommand(u8CS, 0);
    MCF_QSPI_QAR = QSPI_TRANSMIT_ADDRESS;
    MCF_QSPI_QDR = u16Word;
    
    MCF_QSPI_QWR =  MCF_QSPI_QWR_CSIV|MCF_QSPI_QWR_ENDQP(0x00)|MCF_QSPI_QWR_NEWQP(0x00);
    MCF_QSPI_QDLYR |= MCF_QSPI_QDLYR_SPE;
    
}

/********************************************************************/
/* 
 * Send Byte via Polling
 */
uint8 QSPIPollTransferByteRaw(uint8 u8Byte, uint8 u8CS)
{

    MCF_QSPI_QAR = QSPI_COMMAND_ADDRESS;
    QSPISetTransferCommand(u8CS, 0);
    MCF_QSPI_QAR = QSPI_TRANSMIT_ADDRESS;
    QSPISetTransferData(u8Byte);
    
    MCF_QSPI_QWR =  MCF_QSPI_QWR_CSIV|MCF_QSPI_QWR_ENDQP(0x01)|MCF_QSPI_QWR_NEWQP(0x00);
    MCF_QSPI_QDLYR |= MCF_QSPI_QDLYR_SPE;
    
    while (!(MCF_QSPI_QIR & MCF_QSPI_QIR_SPIF))
        ; /* Empty Body */
    
    MCF_QSPI_QAR = QSPI_RECEIVE_ADDRESS;
    return MCF_QSPI_QDR;
}


/********************************************************************/
/* Init a QSPI Buffer for SPI transfers
 *   1. Allocate memory for the Buffer
 *   2. Allocate memory for the Transmit Buffer
 *   3. Allocate memory for the Reception Buffer
 *   4. Allocate memory for the Commands
 *   5. Initialize flags 
 */
struct tQSPIBuffers* QSPI_InitFullBuffer(uint8 u8Size)
{
    tQSPIBuffers *ptr;
    
    ptr=(tQSPIBuffers*)malloc(sizeof(tQSPIBuffers));
    ptr->u8Size=u8Size;
    ptr->pu16TxData=(uint16*)malloc((u8Size)*sizeof(uint16));
    ptr->pu16RxData=(uint16*)malloc((u8Size)*sizeof(uint16));
    ptr->pu8Cmd=(uint8*)malloc((u8Size)*sizeof(uint8));
    ptr->u8Stat = QSPI_BUFFSTAT_IDLE;

    return ptr;
}

/********************************************************************/
/* Init a QSPI Buffer for SPI transfers
 *   1. Allocate memory for the Buffer
 *   2. Allocate memory for the Commands
 *   3. Initialize flags 
 */
struct tQSPIBuffers* QSPI_InitSimpleBuffer(uint8 u8Size, uint16 *pu16TxData, uint16 *pu16RxData)
{
    tQSPIBuffers *ptr;
    
    ptr=(tQSPIBuffers*)malloc(sizeof(tQSPIBuffers));
    ptr->u8Size=u8Size;
    ptr->pu16TxData=pu16TxData;
    ptr->pu16RxData=pu16RxData;
    ptr->pu8Cmd=(uint8*)malloc((u8Size)*sizeof(uint8));
    ptr->u8Stat = QSPI_BUFFSTAT_IDLE;

    return ptr;
}

/********************************************************************/
/* Free QSPI Buffer for SPI transfers
 *   1. Free memory for the Transmit Buffer
 *   2. Free memory for the Reception Buffer
 *   3. Free memory for the Commands
 *   4. Release flags 
 *   5. Free Buffer memory
 */
int8 QSPI_FreeFullBuffer(tQSPIBuffers *sQSPIBuff)
{
    free (sQSPIBuff->pu16TxData);
    free (sQSPIBuff->pu16RxData);
    free (sQSPIBuff->pu8Cmd);
    free (sQSPIBuff);
    return 0;
}

/********************************************************************/
/* Free QSPI Buffer for SPI transfers
 *   1. Free memory for the Commands
 *   2. Release flags 
 *   3. Free Buffer memory
 */
int8 QSPI_FreeSimpleBuffer(tQSPIBuffers *sQSPIBuff)
{
    free (sQSPIBuff->pu8Cmd);
    free (sQSPIBuff);
    return 0;
}

/********************************************************************/
/* 
 * Interrupt Buffer Transfer	
 */
int8 QSPIIntBufferTransfer(tQSPIBuffers *sQSPIBuff)
{
    uint8 j;
    
    if (u8QSPIStat != QSPI_IDLE)
        return -1;
    
    MCF_QSPI_QIR |= (MCF_QSPI_QIR_SPIF | MCF_QSPI_QIR_SPIFE);
    
    if (sQSPIBuff->u8Size > 16){
        /* If wraparound must be enabled */
         return -1;
    }
    else{
        QSPI_SPIF_ISR = QSPI_SPIF_Test_ISR;
        sQSPIInterruptBuf = sQSPIBuff;
        for (j=0; j < sQSPIBuff->u8Size; j++){
                /* Fill the buffers for Wraparound */
                sQSPIBuff->u8Stat = QSPI_BUFFSTAT_BUSY;
                MCF_QSPI_QAR = QSPI_COMMAND_ADDRESS+j;
                MCF_QSPI_QDR = MCF_QSPI_QDR_DATA(sQSPIBuff->pu8Cmd[j]);
                MCF_QSPI_QAR = QSPI_TRANSMIT_ADDRESS+j;
                MCF_QSPI_QDR = sQSPIBuff->pu16TxData[j];
        }
        MCF_QSPI_QWR = (MCF_QSPI_QWR&MCF_QSPI_QWR_CSIV) |
                        MCF_QSPI_QWR_ENDQP((sQSPIBuff->u8Size)-1) | MCF_QSPI_QWR_NEWQP(0);
        MCF_QSPI_QDLYR |= MCF_QSPI_QDLYR_SPE;
        return 0;
    }
    
}

/********************************************************************/
/* 
 * Check to see if receive interrupt ready	
 */
int8 QSPIF_IntRdy(void)
{
    if (sQSPIInterruptBuf->u8Stat == QSPI_BUFFSTAT_RXRDY) 
        return 1;
    else
        return 0;
}

/********************************************************************/
/* 
 * Return interrupt status
 */
uint8 QSPI_IntStat(void)
{
    return (sQSPIInterruptBuf->u8Stat);
}

/********************************************************************/
/* 
 * Transfer using Polling
 */
int8 QSPIPollBufferTransfer(tQSPIBuffers *sQSPIBuff)
{
    uint8 u8Counter =0;
    
    if (u8QSPIStat != QSPI_IDLE)
        return -1;
    
    MCF_QSPI_QIR |= MCF_QSPI_QIR_SPIF;
    
    if (sQSPIBuff->u8Size > 16){
        /* If wraparound must be enabled */
         return QSPIPollBufferTransfer_Wrap(sQSPIBuff);
    }
    else{
        return QSPIPollBufferTransfer_NoWrap(sQSPIBuff);
    }
    
}

/********************************************************************/
/* 
 * Transfer using polling if wrap around is needed	
 */
static int8 QSPIPollBufferTransfer_Wrap(tQSPIBuffers *sQSPIBuff)
{
    uint8 u8Counter =0;
    uint8 u8RxCounter = 0;
    uint8 i, j;
    
    /* If wraparound must be enabled */
    for (j=0; j < 16; j++){
        /* Fill the buffers for Wraparound */
        MCF_QSPI_QAR = QSPI_COMMAND_ADDRESS+j;
        MCF_QSPI_QDR = MCF_QSPI_QDR_DATA(sQSPIBuff->pu8Cmd[u8Counter]);
        MCF_QSPI_QAR = QSPI_TRANSMIT_ADDRESS+j;
        MCF_QSPI_QDR = sQSPIBuff->pu16TxData[u8Counter++];
    }
    
    MCF_QSPI_QWR = (MCF_QSPI_QWR&MCF_QSPI_QWR_CSIV) | MCF_QSPI_QWR_WREN |
                    MCF_QSPI_QWR_ENDQP(15) | MCF_QSPI_QWR_NEWQP(0);
    MCF_QSPI_QDLYR |= MCF_QSPI_QDLYR_SPE;
    
    while ((MCF_QSPI_QWR&MCF_QSPI_QWR_CPTQP(0xF))!= MCF_QSPI_QWR_CPTQP(0))
        ; /* Empty Body */

    i = 0;
    while (u8Counter < sQSPIBuff->u8Size){
        while ((MCF_QSPI_QWR&MCF_QSPI_QWR_CPTQP(0xF))!= MCF_QSPI_QWR_CPTQP(i+1))
            ; /* Empty Body */
        MCF_QSPI_QAR = QSPI_RECEIVE_ADDRESS+i;
        sQSPIBuff->pu16RxData[u8RxCounter++] = MCF_QSPI_QDR;
        MCF_QSPI_QAR = QSPI_COMMAND_ADDRESS+i;
        MCF_QSPI_QDR = MCF_QSPI_QDR_DATA(sQSPIBuff->pu8Cmd[u8Counter]);
        MCF_QSPI_QAR = QSPI_TRANSMIT_ADDRESS+i;
        MCF_QSPI_QDR = sQSPIBuff->pu16TxData[u8Counter++];
        if (i<15){
            i++;
        }
        else{
            i = 0;
            while (!(MCF_QSPI_QIR & MCF_QSPI_QIR_SPIF))
                ; /* Empty Body */
            MCF_QSPI_QIR |= MCF_QSPI_QIR_SPIF;
        }
    }
    
    for (;i<15;i++){
        while ((MCF_QSPI_QWR&MCF_QSPI_QWR_CPTQP(0xF))!= MCF_QSPI_QWR_CPTQP(i))
            ; /* Empty Body */
         MCF_QSPI_QAR = QSPI_RECEIVE_ADDRESS+i;
         sQSPIBuff->pu16RxData[u8RxCounter++] = MCF_QSPI_QDR;
    }
    while (!(MCF_QSPI_QIR & MCF_QSPI_QIR_SPIF))
        ; /* Empty Body */

    MCF_QSPI_QIR |= MCF_QSPI_QIR_SPIF;
    MCF_QSPI_QWR = (MCF_QSPI_QWR&MCF_QSPI_QWR_CSIV) | 
                    MCF_QSPI_QWR_ENDQP(((sQSPIBuff->u8Size)%16)-1) | MCF_QSPI_QWR_NEWQP(0);
    MCF_QSPI_QAR = QSPI_RECEIVE_ADDRESS+15;
         sQSPIBuff->pu16RxData[u8RxCounter++] = MCF_QSPI_QDR;
    
     while (!(MCF_QSPI_QIR & MCF_QSPI_QIR_SPIF))
        ; /* Empty Body */
    MCF_QSPI_QAR = QSPI_RECEIVE_ADDRESS;
    for (j=u8RxCounter; j < sQSPIBuff->u8Size; j++){
        /* Fill the buffers for Wraparound */
        sQSPIBuff->pu16RxData[j] = MCF_QSPI_QDR;
    }
 
     MCF_QSPI_QIR |= MCF_QSPI_QIR_SPIF;
    
    return 0;
}

/********************************************************************/
/* 
 * Transfer using polling if wrap around is not needed	
 */
static int8 QSPIPollBufferTransfer_NoWrap(tQSPIBuffers *sQSPIBuff)
{
    uint8 j;
    for (j=0; j < sQSPIBuff->u8Size; j++){
            /* Fill the buffers for Wraparound */
            MCF_QSPI_QAR = QSPI_COMMAND_ADDRESS+j;
            MCF_QSPI_QDR = MCF_QSPI_QDR_DATA(sQSPIBuff->pu8Cmd[j]);
            MCF_QSPI_QAR = QSPI_TRANSMIT_ADDRESS+j;
            MCF_QSPI_QDR = sQSPIBuff->pu16TxData[j];
    }
    MCF_QSPI_QWR = (MCF_QSPI_QWR&MCF_QSPI_QWR_CSIV) |
                    MCF_QSPI_QWR_ENDQP((sQSPIBuff->u8Size)-1) | MCF_QSPI_QWR_NEWQP(0);
    MCF_QSPI_QDLYR |= MCF_QSPI_QDLYR_SPE;
    
    
    while (!(MCF_QSPI_QIR & MCF_QSPI_QIR_SPIF))
        ; /* Empty Body */
    
    MCF_QSPI_QAR = QSPI_RECEIVE_ADDRESS;
    for (j=0; j < sQSPIBuff->u8Size; j++){
        /* Fill the buffers for Wraparound */
        sQSPIBuff->pu16RxData[j] = MCF_QSPI_QDR;
    }
    MCF_QSPI_QIR |= MCF_QSPI_QIR_SPIF;
    
    return 0;
}

/********************************************************************/
/* 
 * Enable Abort Interrupt	
 */
void QSPI_Enable_Abort(void)
{
	/* Enable Abort Interrupt */
    QSPI_ENABLE_ABRT(); 
    
    /* Disable Abort Lock */
    QSPI_DISABLE_ABRTLOCK(); 
    
    /* Set Abort ISR */
    QSPI_ABRT_ISR = QSPI_ABRT_Test_ISR;
}

/********************************************************************/
/* 
 * Disable Abort Interrupt	
 */
void QSPI_Disable_Abort(void)
{
	/* Enable Abort Interrupt */
    QSPI_DISABLE_ABRT(); 
    
    /* Enable Abort Lock */
    QSPI_ENABLE_ABRTLOCK();
}

/********************************************************************/
/* 
 * Set the transfer command	
 */
static void QSPISetTransferCommand (unsigned char u8CS, unsigned char u8Cont)
{
    MCF_QSPI_QDR = (u8Cont << 15)|(MCF_QSPI_QDR_BITSE | MCF_QSPI_QDR_DT |
                        MCF_QSPI_QDR_DSCK | MCF_QSPI_QDR_CS(u8CS));
    return;
}

/********************************************************************/
/* 
 * Transfer data by putting it into QDR	
 */
static void QSPISetTransferData (unsigned char u8Data)
{
    MCF_QSPI_QDR = u8Data;
    return;
}


/*************************INTERRUPTS ********************************/

/********************************************************************/
/* 
 * Abort ISR	
 */
void QSPI_ABRT_Test_ISR (void)
{
    
    sQSPIInterruptBuf->u8Stat = QSPI_BUFFSTAT_ABORTED;
}

/********************************************************************/
/* 
 * SPIF ISR	
 */
void QSPI_SPIF_Test_ISR (void)
{
    uint8 i;
    
    MCF_QSPI_QAR = QSPI_RECEIVE_ADDRESS;
    for (i=0; i <sQSPIInterruptBuf->u8Size; i++ ){
       sQSPIInterruptBuf->pu16RxData[i] = MCF_QSPI_QDR;     
    }
    
    sQSPIInterruptBuf->u8Stat = QSPI_BUFFSTAT_RXRDY;
}

/********************************************************************/
/* 
 * QSPI ISR	
 */
__interrupt__ void QSPI_ISR()
{
   if (MCF_QSPI_QIR & (MCF_QSPI_QIR_ABRT))
   {
        MCF_QSPI_QIR |= MCF_QSPI_QIR_ABRT;
        QSPI_ABRT_ISR();
   }

   if (MCF_QSPI_QIR & (MCF_QSPI_QIR_SPIF))
   {
        MCF_QSPI_QIR |= MCF_QSPI_QIR_SPIF;
        QSPI_SPIF_ISR();
   }

	return;	
}



