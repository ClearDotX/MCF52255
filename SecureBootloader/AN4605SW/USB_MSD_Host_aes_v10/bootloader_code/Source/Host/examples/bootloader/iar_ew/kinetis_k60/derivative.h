/*
 * Note: This file is recreated by the project wizard whenever the MCU is
 *       changed and should not be edited by hand
 */

/* Include the derivative-specific header file */
#include <MK60N512VMD100.h>

#define _MK_xxx_H_
#define USE_POLL
#define USE_PIT1

#define printf printf_kinetis
#define sprintf sprintf_kinetis
#define LITTLE_ENDIAN
#define __DEBUG__
#define TERM_PORT           UART5_BASE_PTR

#define BSP_LED1 1 << 28
#define BSP_LED2 1 << 29
