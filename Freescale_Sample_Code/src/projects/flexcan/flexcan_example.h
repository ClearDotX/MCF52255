/*
 * File name: flexcan_example.h
 * Purpose:   FlexCAN Header file
 */
 
#ifndef _FLEXCAN_EXAMPLE_H_
#define _FLEXCAN_EXAMPLE_H_

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
int8 flexcan_baudrate (void);
int8 flexcan_loopback (void);
int8 flexcan_MB0txnoerrpoll (void);
int8 flexcan_MBxtxnoerrpoll(void);
int8 flexcan_MBxtxnoerrint (void);
int8 flexcan_MB0rxnoerrpoll(void);
int8 flexcan_MBxrxnoerrpoll(void);
int8 flexcan_MB0rxnoerrint(void);
int8 flexcan_MBxrxnoerrint (void);
int8 flexcan_MBxfilterrxnoerrint(void);
int8 flexcan_errpoll(void);



#endif /* _FLEXCAN_EXAMPLE_H_ */