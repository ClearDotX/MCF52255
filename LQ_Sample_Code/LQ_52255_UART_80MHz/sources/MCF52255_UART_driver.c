/********************************************************
����MCF52255�๦�ܿ�����V1.0
��д:����
Designed by Chiu Sir
E-mail:chiusir@aliyun.com
����汾:V1.1
������:2013��8��19���賿
�����Ϣ�ο����е�ַ��
��վ��  http://www.lqist.cn
�Ա��꣺http://shop36265907.taobao.com
------------------------------------
Code Warrior 7.2
Target : MCF52255
Crystal: 48.000Mhz
PLL:80M
------------------------------------
����0,1��ʾ����
UART0 9600,8,N,1
UART1 9600,8,N,1
���������޸ı���һ�²�����ȷ�����
MCF52255_UART_driver.c ��54��	SYS_CLOCK=80000000; 
MCF52255_UART_driver.c ��215��	SYS_CLOCK=80000000; 
MCF52255_sysinit.c     ��204��	PLL_80M(); //������ʾ���޸���Ƶ��������ĺ������Լ���

�˿ڶ�Ӧ��ϵ��
�ο���MCF52255���İ����ϴ��\MCF52255RM.pdf��187ҳ

8·ADC��ʾ����:
�����İ��UA0����UB0�ӵ�USBתTTL��TXD�ܽţ�ͨ�����ڵ������ֿ��Բ鿴���ݡ�

********************************************************/
#include "MCF52255_UART_driver.h"

char receive[30]=
{
	'B',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};
extern uint8 status;

/* 
*Initialization of UART0
*/
void uart0_init(uint32 SYS_CLOCK,uint16 baudrate)
{	
	uint32 div;
	
	MCF_GPIO_PUAPAR=MCF_GPIO_PUAPAR_UTXD0_UTXD0
				   |MCF_GPIO_PUAPAR_URXD0_URXD0;
	
//Reset Transmitter Receiver Mode Register
	MCF_UART0_UCR|=(0
				  |MCF_UART_UCR_RESET_TX
	              |MCF_UART_UCR_RESET_RX                                     
	              |MCF_UART_UCR_RESET_MR);             

//No parity,8bit data
	MCF_UART0_UMR1=(0
		          |MCF_UART_UMR_PM_NONE
	              |MCF_UART_UMR_BC_8);
		
//1bit stop
	MCF_UART0_UMR2|=MCF_UART_UMR_CM_NORMAL
				  |MCF_UART_UMR_SB_STOP_BITS_1;
		                
//Set Rx and Tx buad by SYSTERM CLOCK
	MCF_UART0_UCSR=(0
				  |MCF_UART_UCSR_RCS_SYS_CLK
		  		  |MCF_UART_UCSR_TCS_SYS_CLK);
	
//Set	UART interrupts	
	MCF_UART0_UIMR=0;//MCF_UART_UISR_FFULL_RXRDY;
				  //|MCF_UART_UISR_TXRDY;
				  //|MCF_UART_UISR_DB
				  //|MCF_UART_UISR_COS
	
	MCF_INTC0_IMRL&=~MCF_INTC_IMRL_MASKALL;
	MCF_INTC0_IMRL&=~MCF_INTC_IMRL_INT_MASK13;//UART0
	MCF_INTC0_ICR13=MCF_INTC_ICR_IP(6)+MCF_INTC_ICR_IL(2);//UART0	
	
	
//set buad rate
	div=(SYS_CLOCK/32/baudrate);
	MCF_UART0_UBG1=(unsigned char)(div>>8);
	MCF_UART0_UBG2=(unsigned char)(div&0x00ff);
	
//Enable Tx/Rx
	MCF_UART0_UCR=(0
				 |MCF_UART_UCR_TX_ENABLED
				 |MCF_UART_UCR_RX_ENABLED);
}

void uart0_putchar(char c)
{
//Wait until space is available in the FIFO 
    while (!(MCF_UART0_USR&MCF_UART_USR_TXRDY)) 
    {
    	;
    }

    //Send the character 
    MCF_UART0_UTB = (unsigned char)c;
}

unsigned char uart0_getchar()
{
//Wait until character has been received 
    while (!(MCF_UART0_USR & MCF_UART_USR_RXRDY)) 
    {
    	
    };

    return MCF_UART0_URB;	
}

void uart0_putstr(char *str)
{
	while(*str!=0)
	{
		uart0_putchar(*str++);
	}
}


__declspec(interrupt:0) void UART0_interrupt(void)//13
{
	static uint8 i=0;
	static char rec_temp,rec_buffer[20];
	if(MCF_UART0_USR&MCF_UART_USR_RXRDY)
	{
		rec_temp=(char)MCF_UART0_URB;
		if((rec_temp!='\n')&&(i<20))
		{
			rec_buffer[i]=rec_temp;
			i++;
			rec_buffer[i]=0;
			
		}
		else
		{	
			uart0_putstr(rec_buffer);
			uart0_putchar('\n');
			i=0;
			status=over;
		}
	}
	else uart0_putstr("error");
	
}
__declspec(interrupt:0) void UART1_interrupt(void)//14
{
	
}
__declspec(interrupt:0) void UART2_interrupt(void)//15
{
	
}
