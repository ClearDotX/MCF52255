/*
 * File:    power.h
 * Purpose: Power measurement utility
 *
 * Notes:
 * 
 * License:     All software covered by license agreement in -
 *              docs/Freescale_Software_License.pdf
 */

#ifndef _POWER_H_
#define _POWER_H_

/********************************************************************/
/*
 * Function Prototypes 
 */
void mainloop (void);
void enter_low_pwr_mode(void);
void cmd_sysclk (int, char **);
void cmd_stop (int, char **);
void cmd_wait (int, char **);
void cmd_doze (int, char **);
void cmd_run (int, char **);
void cmd_loop (int, char**);
void cmd_rd (int, char **);
void cmd_ppm (int, char **);
void cmd_app1 (int, char **);
void cmd_app2 (int, char **);
void cmd_rtc_en (int, char **);
void cmd_periph_clk(int, char **);
void cmd_periph_clk_power(int, char **);
void presskey(void);
void cmd_lpdiv (int, char **);
void cmd_readreg (int , char **);
void setshow_sysclk (int, char **);
void setshow_clkout (int, char **);
void setshow_lpd (int, char **);
void setshow_stpmd (int, char **);
void setshow_lvdse (int, char **);
void setshow_xipl (int, char **);
void setshow_baud (int, char **);
void setshow_step (int, char **);
void setshow_pwrcfg (int, char **);
void min_pwr_config (void);
void max_pwr_config (void);
void set_pwr_config (int);
void uart_init (int, int, int);
void cpu_pstddata_disable(void);
void cpu_pstddata_enable(void);
void init_gpio_stopidd(void);
void init_gpio_enableidd(void);

#if 0
__interrupt__
void abort_handler(void);
#endif

#if 1
void abort_handler(void); 
#endif

__interrupt__
void sw1_handler(void);
__interrupt__
void sw2_handler(void);



/*
 * Useful Definitions
 */
#define LPM_STOP    1
#define LPM_WAIT    2
#define LPM_DOZE    3
#define LPM_RUN     4

#define PWR_MIN     0
#define PWR_MAX     1
#define PWR_TYP     2

/*
 * App1 Definitions
 */ 
#define APP1_OUTER_ITERATIONS   2000
#define APP1_INNER_ITERATIONS   1000

/*
 * App2 Definitions
 */
#define APP2_ITERATIONS     4000
#define LOWEST_SYS_CLK      (((REF_CLK_KHZ) * 1000) / 32768)
#define OVERHEAD_CONST      100



/********************************************************************/

#endif /* _POWER_H_ */

