/*
 * File:    nbuf.c
 * Purpose: Implementation of network buffer scheme.
 *
 * Notes:   
 */

#include "common.h"
#include "queue.h"
#include "nbuf.h"

/********************************************************************/
/*
 * Queues used for free network buffer storage
 */
QUEUE free_queue;

/*
 * These buffers may be used directly by some Ethernet devices 
 * that require line-aligned buffers.  In order to accomplish
 * this, the nbuf data is over-allocated and adjusted.  The following
 * pointer keeps track of the original data pointer returned by malloc
 */
ADDRESS *ua_bufs;

/*
 * Global to keep track of the number and size of buffers
 * and if the buffers have been initialized.
 */
static int nNbuf;
static int sNbuf;
static int initialized = FALSE;

/********************************************************************/
/*
 * Initialize all the network buffer queues
 *
 * Parameters
 *  n number of network buffers to create
 *
 * Return Value:
 *  0 success
 *  non-zero failure 
 */
int
nbuf_init(int num, int data_size)
{
    int i;
    NBUF *nbuf;

    initialized = TRUE;
	nNbuf = 0;			//add to enable nbuf_flush
    /* Initialize the free buffer queue */
    queue_init(&free_queue);

    ua_bufs = (ADDRESS *)malloc(num * sizeof(ADDRESS));
    if (ua_bufs == NULL)
    {
    	initialized = FALSE;   //add to disable nbuf_flush
    	return -1;
    }

    for (i = 0; i < num; ++i)
    {
        /* Allocate memory for the network buffer structure */
        nbuf = (NBUF *)malloc(sizeof(NBUF));
        if (nbuf == NULL)
            return -1;

        /* Allocate memory for the actual data */
        ua_bufs[i] = (ADDRESS)malloc(data_size + 16);
        nbuf->data = (uint8 *)((uint32)(ua_bufs[i] + 15) & 0xFFFFFFF0);
        if (nbuf->data == NULL)
            return -1;

        /* Initialize the network buffer */
        nbuf->offset = 0;
        nbuf->length = 0;

        /* Add the network buffer to the free list */
        queue_add(&free_queue, (QNODE *)nbuf);
        nNbuf = i;//added to enable nbuf_flush
    }
    
    return 0;
}
/********************************************************************/
/* 
 * Return all the allocated memory to the heap
 */
void
nbuf_flush(void)
{
    NBUF *nbuf;
    int i, level = asm_set_ipl(7);
    int n = 0;

    if (initialized)
    {
        for (i = 0; i < nNbuf; ++i)
            free((uint8*)ua_bufs[i]);
		free((uint8*)ua_bufs);
        while ((nbuf = (NBUF *)queue_remove(&free_queue)) != NULL)
        {
            free(nbuf);
            ++n;
        }

        ASSERT(n == NBUF_MAX);
        asm_set_ipl(level);

        initialized = FALSE;
    }
}
/********************************************************************/
/* 
 * Allocate a network buffer from the free list
 *
 * Return Value:
 *  Pointer to a free network buffer
 *  NULL if none are available
 */
NBUF *
nbuf_alloc(void)
{
    NBUF *nbuf;
    int level = asm_set_ipl(7);

    nbuf = (NBUF *)queue_remove(&free_queue);
    asm_set_ipl(level);
    return nbuf;
}
/********************************************************************/
/*
 * Add the specified network buffer back to the free list
 *
 * Parameters:
 *  nbuf    Buffer to add back to the free list
 */
void
nbuf_free(NBUF *nbuf)
{
    int level = asm_set_ipl(7);

    nbuf->offset = 0;
    nbuf->length = NBUF_SZ;
    queue_add(&free_queue,(QNODE *)nbuf);

    asm_set_ipl(level);
}
/********************************************************************/
