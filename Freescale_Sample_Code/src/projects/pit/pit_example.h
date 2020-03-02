/*
 * File name   : pit_test.h
 * Project name: PIT Example
 * 
 * License:     All software covered by license agreement in -
 *              docs/Freescale_Software_License.pdf
 */
 
#ifndef _PIT_EXAMPLE_H_
#define _PIT_EXAMPLE_H_

#include "common.h"

/*
 * PIT Values
 */
#define FREQUENCY 10000
#define COUNTDOWN 5
#define INITIAL 0x8000
#define THRESHOLD 0x2000

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
 * Functions provided for the PIT example
 */
int8 pit_periodicity (void);
int8 pit_countdown (void);
int8 pit_overwrite (void);
int8 pit_dozemode (void);
int8 pit_debugmode (void);

#endif /* _PIT_EXAMPLE_H_ */
