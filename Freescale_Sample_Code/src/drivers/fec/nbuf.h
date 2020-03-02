/*
 * File:        nbuf.h
 * Purpose:     Definitions for network buffer management
 *
 * Notes:       These routines implement a network buffer scheme
 */

#ifndef _NBUF_H_
#define _NBUF_H_

/********************************************************************/
/*
 * Number of network buffers to use
 */
#ifndef NBUF_MAX
#define NBUF_MAX    200
#endif

/*
 * Size of each buffer in bytes
 */
#ifndef NBUF_SZ
#define NBUF_SZ     2048
#endif

/* 
 * Buffer Descriptor Format 
 * 
 * Fields:
 * node     Pointer to next node in the queue
 * data     Pointer to the data buffer
 * offset   Index into buffer
 * length   Remaining bytes in buffer from (data + offset)
 */
typedef struct
{
    QNODE node;
    uint8 *data;   
    uint16 offset;
    uint16 length;
} NBUF;

/*
 * Functions to manipulate the network buffers.
 */
int
nbuf_init(int, int);

void
nbuf_flush(void);

NBUF *
nbuf_alloc (void);

void
nbuf_free(NBUF *);

void
nbuf_reset(void);

void
nbuf_debug_dump(void);

/********************************************************************/

#endif  /* _NBUF_H_ */
