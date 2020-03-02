/*
 * File name   : qspi_example.h
 * Purpose     : Header file for QSPI Examples
 * 
 * License:     All software covered by license agreement in -
 *              docs/Freescale_Software_License.pdf
 */
 
#ifndef _QSPI_EXAMPLE_H_
#define _QSPI_EXAMPLE_H_

#include "common.h"

/*
 * Baudrate Definition
 */
#define BAUDRATE 1500

/*
 * Macros
 */
#define WAIT_4_USER_INPUT()    in_char()
#define TEST_USER_VERIFY(input) \
   TEST_PRINT_1 ("Press 'N' if integrity is incorrect\n");\
   input = in_char(); \
   if ((input == 'N') || (input == 'n')) \
    return 1; \
   else \
   return 0

/*
 * Example Functions
 */

int8 qspi_baudrate (void);
int8 qspi_transmit(void);
int8 qspi_wraparound(void);
int8 qspi_bits (void);
int8 qspi_spif(void);
int8 qspi_abort(void);

#endif /* _QSPI_EXAMPLE_H_ */