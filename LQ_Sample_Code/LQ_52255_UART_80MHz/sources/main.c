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

�˿ڶ�Ӧ��ϵ��
�ο���MCF52255���İ����ϴ��\MCF52255RM.pdf��187ҳ

8·ADC��ʾ����:
�����İ��UA0����UB0�ӵ�USBתTTL��TXD�ܽţ�ͨ�����ڵ������ֿ��Բ鿴���ݡ�

********************************************************/
#include "support_common.h" /* include peripheral declarations and more */
#include "MCF52255_UART_driver.h"

void delay(int ms);

void PLL_80M();
void PLL_144M();
void PLL_153d6M();
/********************************************************************/
void delay(int ms) 
{
   int ii,jj;
   if (ms<1) ms=1;
   for(ii=0;ii<ms;ii++)     
     for(jj=0;jj<6676;jj++)
     {} 
}

/********************************************************************/
void PLL_80M()
{
//fsys ������ʽ��fsys = fref �� 2 �� (MFD + 2) �� (2 RFD);
//fref ���㹫ʽ  fref= MHz/(CCHR+1) ��
//fbus ���㹫ʽ  fbus=fsys/2;
//�ó����У�CCHR��4(��λ��Ĭ��ֵ),MFD=4,RFD=0��ϵͳƵ�ʼ��㣺
//Ϊ8MHz/(CCHR+1) �� 2 �� (MFD + 2) �� (2^RFD) = 80Mhz.
	MCF_CLOCK_CCHR=5;	
	MCF_CLOCK_SYNCR = (MCF_CLOCK_SYNCR_MFD(3)
					| MCF_CLOCK_SYNCR_RFD(0)
					| MCF_CLOCK_SYNCR_CLKSRC
					| MCF_CLOCK_SYNCR_PLLMODE
					| MCF_CLOCK_SYNCR_PLLEN) ;
//�ȴ�ʱ���ȶ�
	while (!(MCF_CLOCK_SYNSR & MCF_CLOCK_SYNSR_LOCK))
	{
	}		
}
void PLL_144M()
{
//fsys ������ʽ��fsys = fref �� 2 �� (MFD + 2) �� (2 RFD);
//fref ���㹫ʽ  fref= MHz/(CCHR+1) 
//fbus ���㹫ʽ  fbus=fsys/2;
//�ó����У�CCHR��4(��λ��Ĭ��ֵ),MFD=4,RFD=0��ϵͳƵ�ʼ��㣺
//Ϊ8MHz/(CCHR+1) �� 2 �� (MFD + 2) �� (2^RFD) = 144Mhz.
	MCF_CLOCK_CCHR=5;	
	MCF_CLOCK_SYNCR = (MCF_CLOCK_SYNCR_MFD(7)
					| MCF_CLOCK_SYNCR_RFD(0)
					| MCF_CLOCK_SYNCR_CLKSRC
					| MCF_CLOCK_SYNCR_PLLMODE
					| MCF_CLOCK_SYNCR_PLLEN) ;
//�ȴ�ʱ���ȶ�
	while (!(MCF_CLOCK_SYNSR & MCF_CLOCK_SYNSR_LOCK))
	{
	}		
}
void PLL_153d6M()
{
//fsys ������ʽ��fsys = fref �� 2 �� (MFD + 2) �� (2 RFD);
//fref ���㹫ʽ  fref= MHz/(CCHR+1) 
//fbus ���㹫ʽ  fbus=fsys/2;
//�ó����У�CCHR��4(��λ��Ĭ��ֵ),MFD=4,RFD=0��ϵͳƵ�ʼ��㣺
//Ϊ8MHz/(CCHR+1) �� 2 �� (MFD + 2) �� (2^RFD) = 172.8Mhz.
//48/(4+1) �� 2 �� (6 + 2) �� (2^RFD) = 153.6Mhz. 
	MCF_CLOCK_CCHR=4;	
	MCF_CLOCK_SYNCR = (MCF_CLOCK_SYNCR_MFD(6)
					| MCF_CLOCK_SYNCR_RFD(0)
					| MCF_CLOCK_SYNCR_CLKSRC
					| MCF_CLOCK_SYNCR_PLLMODE
					| MCF_CLOCK_SYNCR_PLLEN) ;
//�ȴ�ʱ���ȶ�
	while (!(MCF_CLOCK_SYNSR & MCF_CLOCK_SYNSR_LOCK))
	{
	}		
}

int main(void)
{	
	PLL_80M();                      //��Ƶ�����к���80M,144M,153.6M
	uart0_init(80000000,9600);    //ϵͳƵ�ʺͲ������趨�����������Խ��������Խ����Ҫ������ԭʼ����
	
	MCF_GPIO_PTCPAR|=MCF_GPIO_PTCPAR_DTIN1_GPIO
				    |MCF_GPIO_PTCPAR_DTIN2_GPIO;
	MCF_GPIO_DDRTC|=MCF_GPIO_DDRTC_DDRTC1
				   |MCF_GPIO_DDRTC_DDRTC2;
	MCF_GPIO_SETTC|=MCF_GPIO_SETTC_SETTC1
				   |MCF_GPIO_SETTC_SETTC2;		
	while(1)
	{	
		uart0_putstr("LONGQIU: UART0 OK!\n");
		MCF_GPIO_PORTTC=(uint8)~MCF_GPIO_PORTTC;		
		delay(500);	
	}

}
