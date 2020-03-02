/*
 * File:		io.h
 * Purpose:		Serial Input/Output routines
 *
 * License:     All software covered by license agreement in -
 *              docs/Freescale_Software_License.pdf
 */

#ifndef _IO_H
#define _IO_H

/********************************************************************/

char	
in_char(void);

void
out_char(char);

int
char_present(void);

int		
printf(const char *, ... );

int
sprintf(char *, const char *, ... );

/********************************************************************/

#endif
