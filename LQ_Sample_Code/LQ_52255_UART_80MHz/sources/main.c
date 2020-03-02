/********************************************************
龙丘MCF52255多功能开发板V1.0
编写:龙丘
Designed by Chiu Sir
E-mail:chiusir@aliyun.com
软件版本:V1.1
最后更新:2013年8月19日凌晨
相关信息参考下列地址：
网站：  http://www.lqist.cn
淘宝店：http://shop36265907.taobao.com
------------------------------------
Code Warrior 7.2
Target : MCF52255
Crystal: 48.000Mhz
PLL:80M
------------------------------------
串口0,1演示程序。
UART0 9600,8,N,1
UART1 9600,8,N,1
下面两处修改必须一致才能正确输出。
MCF52255_UART_driver.c 第54行	SYS_CLOCK=80000000; 
MCF52255_UART_driver.c 第215行	SYS_CLOCK=80000000; 

端口对应关系：
参考：MCF52255核心板资料打包\MCF52255RM.pdf第187页

8路ADC演示方法:
将核心板的UA0或者UB0接到USB转TTL的TXD管脚，通过串口调试助手可以查看数据。

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
//fsys 计数公式：fsys = fref × 2 × (MFD + 2) ÷ (2 RFD);
//fref 计算公式  fref= MHz/(CCHR+1) ；
//fbus 计算公式  fbus=fsys/2;
//该程序中，CCHR是4(复位后默认值),MFD=4,RFD=0。系统频率计算：
//为8MHz/(CCHR+1) × 2 × (MFD + 2) ÷ (2^RFD) = 80Mhz.
	MCF_CLOCK_CCHR=5;	
	MCF_CLOCK_SYNCR = (MCF_CLOCK_SYNCR_MFD(3)
					| MCF_CLOCK_SYNCR_RFD(0)
					| MCF_CLOCK_SYNCR_CLKSRC
					| MCF_CLOCK_SYNCR_PLLMODE
					| MCF_CLOCK_SYNCR_PLLEN) ;
//等待时钟稳定
	while (!(MCF_CLOCK_SYNSR & MCF_CLOCK_SYNSR_LOCK))
	{
	}		
}
void PLL_144M()
{
//fsys 计数公式：fsys = fref × 2 × (MFD + 2) ÷ (2 RFD);
//fref 计算公式  fref= MHz/(CCHR+1) 
//fbus 计算公式  fbus=fsys/2;
//该程序中，CCHR是4(复位后默认值),MFD=4,RFD=0。系统频率计算：
//为8MHz/(CCHR+1) × 2 × (MFD + 2) ÷ (2^RFD) = 144Mhz.
	MCF_CLOCK_CCHR=5;	
	MCF_CLOCK_SYNCR = (MCF_CLOCK_SYNCR_MFD(7)
					| MCF_CLOCK_SYNCR_RFD(0)
					| MCF_CLOCK_SYNCR_CLKSRC
					| MCF_CLOCK_SYNCR_PLLMODE
					| MCF_CLOCK_SYNCR_PLLEN) ;
//等待时钟稳定
	while (!(MCF_CLOCK_SYNSR & MCF_CLOCK_SYNSR_LOCK))
	{
	}		
}
void PLL_153d6M()
{
//fsys 计数公式：fsys = fref × 2 × (MFD + 2) ÷ (2 RFD);
//fref 计算公式  fref= MHz/(CCHR+1) 
//fbus 计算公式  fbus=fsys/2;
//该程序中，CCHR是4(复位后默认值),MFD=4,RFD=0。系统频率计算：
//为8MHz/(CCHR+1) × 2 × (MFD + 2) ÷ (2^RFD) = 172.8Mhz.
//48/(4+1) × 2 × (6 + 2) ÷ (2^RFD) = 153.6Mhz. 
	MCF_CLOCK_CCHR=4;	
	MCF_CLOCK_SYNCR = (MCF_CLOCK_SYNCR_MFD(6)
					| MCF_CLOCK_SYNCR_RFD(0)
					| MCF_CLOCK_SYNCR_CLKSRC
					| MCF_CLOCK_SYNCR_PLLMODE
					| MCF_CLOCK_SYNCR_PLLEN) ;
//等待时钟稳定
	while (!(MCF_CLOCK_SYNSR & MCF_CLOCK_SYNSR_LOCK))
	{
	}		
}

int main(void)
{	
	PLL_80M();                      //超频，已有函数80M,144M,153.6M
	uart0_init(80000000,9600);    //系统频率和波特率设定，如果波特率越大，误码率越大，需要处理下原始函数
	
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
