/*
 * File name: gpt_example.h
 * Purpose:   Header for GPT Examples  
 * 
 * License:     All software covered by license agreement in -
 *              docs/Freescale_Software_License.pdf
 */
 
#ifndef GPT_EXAMPLE_H
#define GPT_EXAMPLE_H

#include "common.h"

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
 * Functions
 */

int8 gpt_gpio (void);
int8 gpt_pulseacc(void);
int8 gpt_ic(void);
int8 gpt_oc(void);
int8 gpt_pwm(void);


#endif /* GPT_EXAMPLE_H */