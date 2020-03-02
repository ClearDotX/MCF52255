/*
 * File name: m52259evb_cpld.h
 * Purpose:   m52259evb cpld Header file
 */
 
#ifndef _CPLD_H_
#define _CPLD_H_

#include "common.h"

/*
 * Macros
 */
#define WAIT_4_USER_INPUT()    in_char()
#define TEST_USER_VERIFY(input) \
   printf("Press 'N' if integrity is incorrect\n");\
   input = in_char(); \
   if ((input == 'N') || (input == 'n')) \
    return 1; \
   else \
   return 0


/*
 * Functions
 */
int8 cpld_8bit_cs1_test (void);
int8 cpld_16bit_cs1_test (void);
int8 cpld_16bit_cs0_test (void);
int8 cpld_lcd_test (void);



#endif /* _CPLD_EXAMPLE_H_ */