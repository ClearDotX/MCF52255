/********************************************************************/

/*
 * File:		RTC main.c
 * Purpose:		Check RTC functionality
 */

#include "common.h"
#include "uif.h"

/*
 * Function declaration for each testcase
 */

/* RTC TestCase Commands*/
void rtc_read_registers                 (int, char **);
void rtc_alarm                    		(int, char **); 
void rtc_seconds_interrupt        		(int, char **);
void rtc_clock_utility                  (int, char **);
void rtc_stop_watch                     (int, char **);

// General functions for RTC
void rtc_set_tod (int, int, int, int);
void rtc_enable_interrupt (int);
void rtc_disable_interrupt (int);
void rtc_clear_interrupt(int);
void rtc_set_alarm(int, int, int, int);
void rtc_write_prescaler (uint32);
void rtc_display_prescaler ();

void rtc_display_tod (void);


//       cmd         min_arg max_arg flag func        Description                         syntax         block regression_flag error_flag

UIF_CMD UIF_CMDTAB[] = 
{
	UIF_CMD_HELP 
	{"rtc_read_registers"                ,0,0,0, rtc_read_registers,"displays rtc registers"," "}, \
	{"rtc_stop_watch"                    ,0,0,0, rtc_stop_watch,"checks rtc stop watch functionality"," "}, \
	{"rtc_clock_utility"                 ,0,0,0, rtc_clock_utility,"displays real time every second"," " }, \
	{"rtc_alarm"  				 		 ,0,0,0, rtc_alarm,"check rtc alarm functionality"," "}, \
	{"rtc_seconds_interrupt"       		 ,0,0,0, rtc_seconds_interrupt,"check seconds interrupt"," "}, \
};

const int UIF_NUM_CMD    = UIF_CMDTAB_SIZE;

UIF_SETCMD UIF_SETCMDTAB[] = {"",0,0,NULL,""};
const int UIF_NUM_SETCMD = 0;

volatile int rtc_intr_serviced;
volatile int rtc_intr_status;
static int rtc_reset_done;

void 
main (void)
{
	int i;	
	char j;
	char **arr;
	
    /* Global interrupt enable */
	mcf5xxx_irq_enable();

     
	// rtc_write_prescaler (0x00001FFF);
	
	 // Set Interrupt Level and Priority Level for RTC
     MCF_INTC0_ICR63 = 0x08;
     
    printf("\n\nRTC Examples\n");
	printf("MCF_CLOCK_RTCCR = %x\n",MCF_CLOCK_RTCCR);
   	
   	/* Command Prompt */   	
    while (TRUE)
    {
        printf("RTC> ");
        run_cmd();
    } 
}
/********************************************************************/

__interrupt__ void rtc_interrupt_handler (void)
{
  // Update global status variables
  rtc_intr_status = MCF_RTC_RTCISR;
  rtc_intr_serviced = 1;

  // Clear interrupt
  MCF_RTC_RTCISR = 0xFFFFFFFF;
}

void
rtc_write_prescaler (uint32 value)
{ 
    // Set RTC clk source, enable RTC etc.
     MCF_CLOCK_RTCCR = 0x57;
     
     MCF_RTC_RTCGOCU = 0x00000000;
     MCF_RTC_RTCGOCL = value;
}

void
rtc_display_prescaler ()
{
	printf("Prescaler Value is: %0d\n",MCF_RTC_RTCGOCL);
}

void 
rtc_alarm (int argc, char ** argv)
{
	rtc_write_prescaler (0x00001FFF);
	
  	mcf5xxx_set_handler(127,(ADDRESS)rtc_interrupt_handler);

	// Unmask the Interrupt for RTC
	MCF_INTC0_IMRL = 0xFFFFFFFF;
  	MCF_INTC0_IMRH = 0x7FFFFFFF;
  	
  // Enable RTC alarm interrupt
  rtc_disable_interrupt(MCF_RTC_RTCIENR_ALL);
  rtc_enable_interrupt(MCF_RTC_RTCIENR_ALM);
  
  // Set Alarm Time
  rtc_set_alarm(1,0,0,0);
  printf("Alarm time is set to(dd:hh:mm:ss): 01:00:00:00\n\n");
  
  // Write value to sec/min/hour/days counter registers to get alarm after 10sec
  rtc_set_tod(0,23,59,50);
  printf("Current time is ");
  rtc_display_tod();

  // Wait for interrupt
  printf("Wait for alarm interrupt\n\n");
  rtc_intr_serviced = 0;
  while(!rtc_intr_serviced);
  
  // Check register values
  if(rtc_intr_status & MCF_RTC_RTCISR_ALM)
  {
    printf("Alarm interrupt occured\n\n");
    if(MCF_RTC_DAYS != 1)
      printf("rtc_check_alarm: unexpected value in days register %d\n",MCF_RTC_DAYS);
    if(MCF_RTC_SECONDS != 0)
      printf("rtc_check_alarm: unexpected value in seconds register %d\n",MCF_RTC_SECONDS);
    if(MCF_RTC_HOURMIN != 0)
      printf("rtc_check_alarm: unexpected value in hr/min register %d\n",MCF_RTC_HOURMIN);
    rtc_display_tod();
  }
  else
  {
    printf("rtc_check_alarm: unexpected interrupt occured\n");
  }
  rtc_disable_interrupt(MCF_RTC_RTCIENR_ALL);
}

void 
rtc_seconds_interrupt (int argc, char ** argv)
{
  int sec = 58;
  int num_seconds;
  
  rtc_write_prescaler (0x00001FFF);
  mcf5xxx_set_handler(127,(ADDRESS)rtc_interrupt_handler);

	// Unmask the Interrupt for RTC
	MCF_INTC0_IMRL = 0xFFFFFFFF;
  	MCF_INTC0_IMRH = 0x7FFFFFFF;
  	
  // Write some initial value to sec/min/hour/days counter registers
  // In this case values are: seconds = 58, mins = 0, hours = 0 days = 0
  rtc_set_tod(0,0,0,sec);
  rtc_display_tod();

  // Enable RTC seconds interrupt
  rtc_disable_interrupt(MCF_RTC_RTCIENR_ALL);
  MCF_RTC_RTCISR = 0x0000FFFF;
  rtc_enable_interrupt(MCF_RTC_RTCIENR_1HZ);

  for(num_seconds=0;num_seconds<10;num_seconds++) 
  {
    if(sec == 59)
      sec = 0;
    else
      sec++;

    rtc_intr_serviced = 0;
    while(!rtc_intr_serviced);
    if(rtc_intr_status & MCF_RTC_RTCISR_1HZ)
    {
      if(MCF_RTC_SECONDS != sec)
      {
	      printf("rtc_check_seconds_interrupt: Invalid value in SECONDS register: %X\n",MCF_RTC_SECONDS);
	      rtc_display_tod();
      }
      else
      {
      	  rtc_display_tod();
      }
    }
    else
    {
      printf("rtc_check_seconds_interrupt: Unexpected interrupt occured\n");
    }
  } // End of for loop
  
  rtc_disable_interrupt(MCF_RTC_RTCIENR_ALL);
}

void 
rtc_clock_utility (int argc, char ** argv)
{
  int day, hr, min, sec;
  
  rtc_write_prescaler (0x00001FFF);
  
  mcf5xxx_set_handler(127,(ADDRESS)rtc_interrupt_handler);
  
  // Unmask the Interrupt for RTC
	MCF_INTC0_IMRL = 0xFFFFFFFF;
  	MCF_INTC0_IMRH = 0x7FFFFFFF;
	
  // Take input from user for initial time setting
  // TODO - get input from user and also set oscillator clock to correct value
  // Enable RTC seconds interrupt and display time every second
  
  rtc_set_tod(0,0,1,1);
  MCF_RTC_RTCCTL = MCF_RTC_RTCCTL_EN;

  rtc_disable_interrupt(MCF_RTC_RTCIENR_ALL);
  
  rtc_enable_interrupt(MCF_RTC_RTCIENR_1HZ);
  
  printf("Interrupt Reg = 0x%08x\n", MCF_RTC_RTCIENR);
  
  rtc_intr_serviced = 0;  
  
  printf("Press any key to break this loop\n");
  while(1)
  {
    while(!rtc_intr_serviced);
    rtc_intr_serviced = 0;
    sec = MCF_RTC_SECONDS & 0x3F;
    min = MCF_RTC_HOURMIN & 0x3F;
    hr = (MCF_RTC_HOURMIN >> 8) & 0x1F;
    day = MCF_RTC_DAYS;
    printf("Time: %02d:%02d:%02d:%02d\r", day, hr, min, sec);
    if((MCF_UART_USR(TERMINAL_PORT) & MCF_UART_USR_RXRDY))
    {
        //break this loop if key pressed
        
        printf("Exiting\n");
    	break; 
    }
  }
}

void
rtc_read_registers(int argc, char ** argv)
{
  printf("RTC Registers:\n");
  printf("MCF_RTC_RTCCTL      			= %08X\n",MCF_RTC_RTCCTL);
  printf("MCF_RTC_RTCISR      			= %08X\n",MCF_RTC_RTCISR);
  printf("MCF_RTC_RTCIENR     			= %08X\n",MCF_RTC_RTCIENR);
  printf("MCF_RTC_RTCGOCU	= %08X\n",MCF_RTC_RTCGOCU);
  printf("MCF_RTC_RTCGOCL	= %08X\n",MCF_RTC_RTCGOCL);
  printf("MCF_RTC_DAYR        			= %08X\n",MCF_RTC_DAYS);
  printf("MCF_RTC_HOURMIN    			= %08X\n",MCF_RTC_HOURMIN);
  printf("MCF_RTC_SECONDS     			= %08X\n",MCF_RTC_SECONDS);
  printf("MCF_RTC_ALARM_DAY   			= %08X\n",MCF_RTC_ALRM_DAY);
  printf("MCF_RTC_ALARM_HM    			= %08X\n",MCF_RTC_ALRM_HM);
  printf("MCF_RTC_ALARM_SEC   			= %08X\n",MCF_RTC_ALRM_SEC);
  printf("MCF_RTC_STPWCH      			= %08X\n",MCF_RTC_STPWCH);
}


void
rtc_stop_watch (int argc, char ** argv)
{
  uint32 count = 1;
  
  rtc_write_prescaler (0x00001FFF);
  
 mcf5xxx_set_handler(127,(ADDRESS) rtc_interrupt_handler);
  
  // Unmask the Interrupt for RTC
	MCF_INTC0_IMRL = 0xFFFFFFFF;
  	MCF_INTC0_IMRH = 0x7FFFFFFF;
  	
  MCF_RTC_RTCCTL = MCF_RTC_RTCCTL_EN;

  // Allign on minutes interrupt
  MCF_RTC_STPWCH = count;
  rtc_set_tod(0,0,59,59); // to get minutes interrupt after one second
  MCF_RTC_RTCISR = 0x0000FFFF;
  rtc_disable_interrupt(MCF_RTC_RTCIENR_ALL);
  rtc_enable_interrupt(MCF_RTC_RTCIENR_MIN);
  rtc_intr_serviced = 0;
  while(!rtc_intr_serviced);
  rtc_display_tod();
  
  // Time starts now
  rtc_disable_interrupt(MCF_RTC_RTCIENR_ALL);
  rtc_enable_interrupt(MCF_RTC_RTCIENR_SW);
  printf("Wait for %d minute to get stop watch interrupt...\n",count);
  rtc_intr_serviced = 0;
  while(!rtc_intr_serviced);
  rtc_disable_interrupt(MCF_RTC_RTCIENR_ALL);
  rtc_display_tod();
  
  // Check time after stop watch interrupt
  if((MCF_RTC_HOURMIN & MCF_RTC_HOURMIN_MINUTES(0x0000003F)) != MCF_RTC_HOURMIN_MINUTES(count+1))
  {
    printf("Waited %d minute\n",count);
  }
  else
  {
    printf("Check with some reference clock source that %d minutes have passed\n",(count+1));	
  }
}


void
rtc_display_tod (void)
{
    int day, hr, min, sec;
    day = MCF_RTC_DAYS;
    hr = (MCF_RTC_HOURMIN >> 8) & 0x1F;
    min = MCF_RTC_HOURMIN & 0x3F;
    sec = MCF_RTC_SECONDS & 0x3F;
    printf("Time: %02d:%02d:%02d:%02d\n", day, hr, min, sec);
}

void
rtc_set_tod (int day, int hr, int min, int sec)
{
    MCF_RTC_DAYS = day;
    MCF_RTC_SECONDS = sec;
    MCF_RTC_HOURMIN = 0
        | MCF_RTC_HOURMIN_HOURS(hr)
        | MCF_RTC_HOURMIN_MINUTES(min);
}

void 
rtc_enable_interrupt (int interrupt_num)
{
  // Following are the legal values for input
  // argument to this function
  // MCF_RTC_RTCIENR_SW
  // MCF_RTC_RTCIENR_MIN
  // MCF_RTC_RTCIENR_ALM
  // MCF_RTC_RTCIENR_DAY
  // MCF_RTC_RTCIENR_1HZ
  // MCF_RTC_RTCIENR_HR
  // MCF_RTC_RTCIENR_2HZ
  // MCF_RTC_RTCIENR_SAM0
  // MCF_RTC_RTCIENR_SAM1
  // MCF_RTC_RTCIENR_SAM2
  // MCF_RTC_RTCIENR_SAM3
  // MCF_RTC_RTCIENR_SAM4
  // MCF_RTC_RTCIENR_SAM5
  // MCF_RTC_RTCIENR_SAM6
  // MCF_RTC_RTCIENR_SAM7
  // MCF_RTC_RTCIENR_ALL
  MCF_RTC_RTCIENR |= interrupt_num;
}

void
rtc_disable_interrupt (int interrupt_num)
{
  // Following are the legal values for input
  // argument to this function
  // MCF_RTC_RTCIENR_SW
  // MCF_RTC_RTCIENR_MIN
  // MCF_RTC_RTCIENR_ALM
  // MCF_RTC_RTCIENR_DAY
  // MCF_RTC_RTCIENR_1HZ
  // MCF_RTC_RTCIENR_HR
  // MCF_RTC_RTCIENR_2HZ
  // MCF_RTC_RTCIENR_SAM0
  // MCF_RTC_RTCIENR_SAM1
  // MCF_RTC_RTCIENR_SAM2
  // MCF_RTC_RTCIENR_SAM3
  // MCF_RTC_RTCIENR_SAM4
  // MCF_RTC_RTCIENR_SAM5
  // MCF_RTC_RTCIENR_SAM6
  // MCF_RTC_RTCIENR_SAM7
  // MCF_RTC_RTCIENR_ALL
  MCF_RTC_RTCIENR &= (~interrupt_num);
}

void
rtc_clear_interrupt(int interrupt_num)
{
  // Following are the legal values for input
  // argument to this function
  // MCF_RTC_RTCISR_SW
  // MCF_RTC_RTCISR_MIN
  // MCF_RTC_RTCISR_ALM
  // MCF_RTC_RTCISR_DAY
  // MCF_RTC_RTCISR_1HZ
  // MCF_RTC_RTCISR_HR
  // MCF_RTC_RTCISR_2HZ
  // MCF_RTC_RTCISR_SAM0
  // MCF_RTC_RTCISR_SAM1
  // MCF_RTC_RTCISR_SAM2
  // MCF_RTC_RTCISR_SAM3
  // MCF_RTC_RTCISR_SAM4
  // MCF_RTC_RTCISR_SAM5
  // MCF_RTC_RTCISR_SAM6
  // MCF_RTC_RTCISR_SAM7
  // MCF_RTC_RTCISR_ALL
  MCF_RTC_RTCISR = interrupt_num;
	
}

void 
rtc_set_alarm(int day, int hr, int min, int sec)
{
    MCF_RTC_ALRM_DAY = day;
    MCF_RTC_ALRM_SEC = sec;
    MCF_RTC_ALRM_HM = 0
        | MCF_RTC_ALRM_HM_HOURS(hr)
        | MCF_RTC_ALRM_HM_MINUTES(min);	
}


