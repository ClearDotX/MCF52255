/*
 * File:    fecbd.c
 * Purpose: Provide a simple buffer management driver
 *
 * Notes:       
 */

#include "common.h"
#include "fec.h"

/*
 * These pointers are used to reference into the chunck of data set 
 * aside for buffer descriptors
 */
FECBD* RxBD[2];
FECBD* TxBD[2];

/* 
 * Buffer descriptor indexes 
 */
static int iTxbd_head[2];
static int iTxbd_tail[2];
static int iRxbd[2];

/*
 * Queues for FEC buffers
 */
extern QUEUE txbd_queue[];
extern QUEUE rxbd_queue[];

/*
 * Local flags
 */
static int initialized[2] = {FALSE, FALSE};
int nRxBD[2] = {0,0};
int nTxBD[2] = {0,0};

/********************************************************************/
/*
 * Initialize the FEC Buffer Descriptor ring 
 * Buffer Descriptor format is defined by the MCDAPI
 *
 * Parameters:
 *  ch      FEC channel
 *  nrxbd   Number of Rx buffer descriptors to create
 *  ntxbd   Number of Tx buffer descriptors to create
 *
 * Return Value:
 *  0 is returned on success
 *  An error code is returned on failure
 *
 * Note: 
 *  The network buffers must be initialzed before the BD rings can 
 *  initialized.
 */
int
fecbd_init(int ch, int nrxbd, int ntxbd)
{
    NBUF *nbuf;
    int i;

    if (TRUE == initialized[ch])
        fecbd_flush(ch);
    
    /* Allocate the buffer descriptors */
    RxBD[ch] = (FECBD *)malloc(sizeof(FECBD) * nrxbd);
    if (RxBD[ch] == NULL)
        return ERR_MALLOC;
    ASSERT(!((int)RxBD[ch] & 0x3));
    TxBD[ch] = (FECBD *)malloc(sizeof(FECBD) * ntxbd);
    if (TxBD[ch] == NULL)
    {
        free(RxBD[ch]);
        return ERR_MALLOC;
    }
    ASSERT(!((int)TxBD[ch] & 0x3));

    /* Initialize the buffer descriptor ring buffer queues */
    queue_init(&txbd_queue[ch]);
    queue_init(&rxbd_queue[ch]);
    
    /* Initialize the Rx Buffer Descriptor ring */
    for (i = 0; i < nrxbd; ++i)
    {
        /* Grab a network buffer from the free list */
        nbuf = nbuf_alloc();
        if (nbuf == NULL)
        {
            free(RxBD[ch]);
            free(TxBD[ch]);
            return ERR_NBUFALLOC;
        }

        /* Initialize the BD */
        RxBD[ch][i].status = RX_BD_E ;//| RX_BD_INTERRUPT;
        RxBD[ch][i].length = RX_BUF_SZ;
        RxBD[ch][i].data =  nbuf->data;

        /* Add the network buffer to the RxBD queue */
        queue_add(&rxbd_queue[ch],(QNODE *)nbuf);       
    }

    /* Set the WRAP bit on the last one */
    RxBD[ch][i-1].status |= RX_BD_W;

    /* Initialize the Tx Buffer Descriptor ring */
    for (i = 0; i < ntxbd; ++i)
    {
        TxBD[ch][i].status = TX_BD_TC;//TX_BD_INTERRUPT;
        TxBD[ch][i].length = 0;
        TxBD[ch][i].data = NULL;
    }

    /* Set the WRAP bit on the last one */
    TxBD[ch][i-1].status |= TX_BD_W;

    /* Initialize the buffer descriptor indexes */
    iTxbd_head[ch] = iTxbd_tail[ch] = iRxbd[ch] = 0;

    initialized[ch] = TRUE;
    nRxBD[ch] = nrxbd;
    nTxBD[ch] = ntxbd;

    return 0;
}
/********************************************************************/
/* 
 * Clear all buffer descriptor ring entries and remove all buffers 
 * from the Rx and Tx BD ring queues.  The FEC should have already 
 * been shutdown because this will remove all active buffers and 
 * buffer descriptors.
 */
void
fecbd_flush(int ch)
{
    NBUF *nbuf;
    int i;
    
    for (i = 0; i < nRxBD[ch]; ++i)
    {
        RxBD[ch][i].status = 0;
        RxBD[ch][i].length = 0;
        RxBD[ch][i].data = NULL;
        
        nbuf = (NBUF *)queue_remove(&rxbd_queue[ch]);
        ASSERT(nbuf);
        nbuf_free(nbuf);
    }
    
    for (i = 0; i < nTxBD[ch]; ++i)
    {
        TxBD[ch][i].status = 0;
        TxBD[ch][i].length = 0;
        TxBD[ch][i].data = NULL;
    }
    
    while ((nbuf = (NBUF *)queue_remove(&txbd_queue[ch])) != NULL)
    {
        nbuf_free(nbuf);
    }

    nRxBD[ch] = nTxBD[ch] = 0;
    initialized[ch] = FALSE;
}
/********************************************************************/
void
fecbd_dump(int ch)
{
#ifdef DEBUG_PRINT
    int i;

    if (initialized[ch] == FALSE)
    {
        printf("FEC%d BDs not initialized\n",ch);
    }
    else
    {
        printf("\n------------ FEC%d BDs -----------\n",ch);
        printf("RxBD Ring: (iRxbd = %d)\n",iRxbd[ch]);
        for (i = 0; i < nRxBD[ch]; i++)
        {
            printf("%02d: BD Addr=0x%08x, Ctrl=0x%04x, Lgth=%04d, DataPtr=0x%08x\n",
                i, &RxBD[ch][i], 
                RxBD[ch][i].status, 
                RxBD[ch][i].length, 
                RxBD[ch][i].data);
        }
        printf("\n");
        printf("TxBD Ring: (iTxbd_head = %d, iTxbd_tail = %d)\n",iTxbd_head[ch], iTxbd_tail[ch]);
        for (i = 0; i < nTxBD[ch]; i++)
        {
            printf("%02d: BD Addr=0x%08x, Ctrl=0x%04x, Lgth=%04d, DataPtr=0x%08x\n",
                i, &TxBD[ch][i], 
                TxBD[ch][i].status, 
                TxBD[ch][i].length, 
                TxBD[ch][i].data);
        }
        printf("--------------------------------\n\n");
    }
#endif
}
/********************************************************************/
/* 
 * Return the address of the first buffer descriptor in the ring.
 *
 * Parameters:
 *  ch          FEC channel
 *  direction   Rx or Tx Macro
 *
 * Return Value:
 *  The start address of the selected Buffer Descriptor ring
 */
uint32
fecbd_get_start(int ch, int direction)
{
    switch (direction)
    {
        case Rx:
            return (uint32)(RxBD[ch]);
        case Tx:
        default:
            return (uint32)(TxBD[ch]);
	}
}
/********************************************************************/
FECBD *
fecbd_rx_alloc(int ch)
{
    int i = iRxbd[ch];

    /* Check to see if the ring of BDs is full */
    if (RxBD[ch][i].status & RX_BD_E)
        return NULL;

    /* Increment the circular index */
    iRxbd[ch] = (uint8)((iRxbd[ch] + 1) % nRxBD[ch]);

    return &RxBD[ch][i];
}
/********************************************************************/
/*
 * This function keeps track of the next available Tx BD in the ring
 *
 * Parameters:
 *  ch  FEC channel
 *
 * Return Value:
 *  Pointer to next available buffer descriptor.
 *  NULL if the BD ring is full
 */
FECBD *
fecbd_tx_alloc(int ch)
{
    int i = iTxbd_head[ch];

    /* Check to see if the ring of BDs is ready */
    if ((TxBD[ch][i].status & TX_BD_R) || (TxBD[ch][i].data != NULL))
//    if ((iTxbd_tail == iTxbd_head) && (TxBD[ch][i].data != NULL))
        return NULL;

    /* Increment the circular index */
    iTxbd_head[ch] = (iTxbd_head[ch] + 1) % nTxBD[ch];

    return &TxBD[ch][i];
}
/********************************************************************/
/*
 * This function keeps track of the Tx BDs that have already been
 * processed by the FEC
 *
 * Parameters:
 *  ch  FEC channel
 *
 * Return Value:
 *  Pointer to the oldest buffer descriptor that has already been sent
 *  by the FEC, NULL if the BD ring is empty
 */
FECBD *
fecbd_tx_free(int ch)
{
    int i = iTxbd_tail[ch];

    /* Check to see if the ring of BDs is empty */
    if ((TxBD[ch][i].data == NULL) || (TxBD[ch][i].status & TX_BD_R))
//    if ((iTxbd_tail == iTxbd_head) && (TxBD[ch][i].data == NULL))
        return NULL;

    /* Increment the circular index */
    iTxbd_tail[ch] = (iTxbd_tail[ch] + 1) % nTxBD[ch];

    return &TxBD[ch][i];
}
/********************************************************************/
