/*
 * File:		interrupts.h
 * Purpose:     Provide macros used to setup interrupt handlers
 *
 * Notes:
 *
 * License:     All software covered by license agreement in -
 *              docs/Freescale_Software_License.pdf
 */

#ifndef __INTERRUPTS_H__
#define __INTERRUPTS_H__

/********************************************************************/

/* Vector table definitions */
typedef void (*intcHandler)();
extern volatile intcHandler __VECTOR_RAM[];
/* Macro to overwrite the default interrupt handler for a particualr interrupt */
#define InstallInterruptHandler(index, fn) __VECTOR_RAM[index] = fn;

/* ColdFire Exception Vector Table */

#define RESET_STACK    0
#define RESET_VECTOR   1
#define ACCESS_ERROR   2
#define ADDRESS_ERROR  3
#define ILLINST        4
#define RESERVED5      5
#define RESERVED6      6
#define RESERVED7      7
#define PRIVVIO        8
#define TRACE          9
#define LINEA          10
#define LINEF          11
#define DEBUG_INTR     12
#define RESERVED13     13
#define FORMAT_ERROR   14
#define UNINIT_INTR    15
#define RESERVED16     16
#define RESERVED17     17
#define RESERVED18     18
#define RESERVED19     19
#define RESERVED20     20
#define RESERVED21     21
#define RESERVED22     22
#define RESERVED23     23
#define SPURIOUS_INTR  24
#define AUTOVEC_LVL1   25
#define AUTOVEC_LVL2   26
#define AUTOVEC_LVL3   27
#define AUTOVEC_LVL4   28
#define AUTOVEC_LVL5   29
#define AUTOVEC_LVL6   30
#define AUTOVEC_LVL7   31
#define TRAP0          32
#define TRAP1          33
#define TRAP2          34
#define TRAP3          35
#define TRAP4          36
#define TRAP5          37
#define TRAP6          38
#define TRAP7          39
#define TRAP8          40
#define TRAP9          41
#define TRAP10         42
#define TRAP11         43
#define TRAP12         44
#define TRAP13         45
#define TRAP14         46
#define TRAP15         47
#define RESERVED48     48
#define RESERVED49     49
#define RESERVED50     50
#define RESERVED51     51
#define RESERVED52     52
#define RESERVED53     53
#define RESERVED54     54
#define RESERVED55     55
#define RESERVED56     56
#define RESERVED57     57
#define RESERVED58     58
#define RESERVED59     59
#define RESERVED60     60
#define RESERVED61     61
#define RESERVED62     62
#define RESERVED63     63

// Interrupt Source Assignment For Interrupt Controller 0

#define INT_EDGEPORT_1            0x41
#define INT_EDGEPORT_2            0x42
#define INT_EDGEPORT_3            0x43
#define INT_EDGEPORT_4            0x44
#define INT_EDGEPORT_5            0x45
#define INT_EDGEPORT_6            0x46
#define INT_EDGEPORT_7            0x47
#define INT_SCM_SWTI              0x48
#define INT_DMA_DONE_CHANNEL_0    0x49
#define INT_DMA_DONE_CHANNEL_1    0x4a
#define INT_DMA_DONE_CHANNEL_2    0x4b
#define INT_DMA_DONE_CHANNEL_3    0x4c
#define INT_UART0                 0x4d
#define INT_UART1                 0x4e
#define INT_UART2                 0x4f

#define INT_I2C_IIF               0x51
#define INT_QSPI                  0x52
#define INT_TMR0                  0x53
#define INT_TMR1                  0x54
#define INT_TMR2                  0x55
#define INT_TMR3                  0x56
#define INT_FEC_X_INTF            0x57
#define INT_FEC_X_INTB            0x58
#define INT_FEC_UN                0x59
#define INT_RL                    0x5a
#define INT_R_INTF                0x5b
#define INT_R_INTB                0x5c
#define INT_MII                   0x5d
#define INT_LC                    0x5e
#define INT_HBERR                 0x5f
#define INT_GRA                   0x60
#define INT_EBERR                 0x61
#define INT_BABT                  0x62
#define INT_BABR                  0x63

#define INT_TIMA_TOF              0x69
#define INT_TIMA_PAIF             0x6a
#define INT_TIMA_PAOVF            0x6b
#define INT_TIMA_C0F              0x6c
#define INT_TIMA_C1F              0x6d
#define INT_TIMA_C2F              0x6e
#define INT_TIMA_C3F              0x6f
#define INT_PMM_LVDF              0x70
#define INT_ADC_ADCA              0x71
#define INT_ADC_ADCB              0x72
#define INT_ADC_ADCINT            0x73
#define INT_PWM                   0x74
#define INT_USB                   0x75

#define INT_PIT0_PIF              0x77
#define INT_PIT1_PIF              0x78

#define INT_CFM_CBEIF             0x7a
#define INT_CFM_CCIF              0x7b
#define INT_CFM_PVIF              0x7c
#define INT_CFM_AEIF              0x7d
#define INT_I2C1_IIF              0x7e
#define INT_RTC                   0x7f


// Interrupt Source Assignment For Interrupt Controller 1

#define INT_FLEXCAN_BUF0I         0x88
#define INT_FLEXCAN_BUF1I         0x89
#define INT_FLEXCAN_BUF2I         0x8a
#define INT_FLEXCAN_BUF3I         0x8b
#define INT_FLEXCAN_BUF4I         0x8c
#define INT_FLEXCAN_BUF5I         0x8d
#define INT_FLEXCAN_BUF6I         0x8e
#define INT_FLEXCAN_BUF7I         0x8f
#define INT_FLEXCAN_BUF8I         0x90
#define INT_FLEXCAN_BUF9I         0x91
#define INT_FLEXCAN_BUF10I        0x92
#define INT_FLEXCAN_BUF11I        0x93
#define INT_FLEXCAN_BUF12I        0x94
#define INT_FLEXCAN_BUF13I        0x95
#define INT_FLEXCAN_BUF14I        0x96
#define INT_FLEXCAN_BUF15I        0x97
#define INT_FLEXCAN_ERR           0x98
#define INT_FLEXCAN_BOFF          0x99


#endif /* #ifndef __INTERRUPTS_H__ */
