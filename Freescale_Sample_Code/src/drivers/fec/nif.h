/*
 * File:        nif.h
 * Purpose:     Definition of a Network InterFace.
 *
 * Notes:
 *
 */

#ifndef _NIF_H
#define _NIF_H

/********************************************************************/

/* 
 * Maximum number of supported protoocls: IP, ARP, RARP 
 */
#define MAX_SUP_PROTO   (3)

typedef struct NIF_t
{
    ETH_ADDR    hwa;        /* ethernet card hardware address */
    ETH_ADDR    broadcast;  /* broadcast address */
    int         mtu;        /* hardware maximum transmission unit */
    int         ch;         /* ethernet channel associated with this NIF */
    void        (*txcallback)(NBUF *); /* Optional callback function
                                          following Tx interrupt */

    struct SUP_PROTO_t
    {
        uint16  protocol;
        void    (*handler)(struct NIF_t *, NBUF *);
        void    *info;
    } protocol[MAX_SUP_PROTO];

    unsigned short  num_protocol;

} NIF;

/*
 * Give everyone access to the two NIFs
 */
extern NIF nif[];

/********************************************************************/

NIF *
nif_init (NIF *);

int
nif_protocol_exist (NIF *, uint16);

void
nif_protocol_handler (NIF *, uint16, NBUF *);

void *
nif_get_protocol_info (NIF *, uint16);

int
nif_bind_protocol (NIF *, uint16, void (*)(NIF *, NBUF *), void *);

void
nif_set_hwa (NIF *, uint8*);

void
nif_set_ch (NIF *, int);

void
nif_set_txcallback (NIF *, void (*)(NBUF *));

/********************************************************************/

#endif /* _NIF_H */
