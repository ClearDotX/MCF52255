/********************************************************   
【平    台】龙丘ColdFire V2多功能开发板
【编    写】龙丘
【Designed】by Chiu Sir
【E-mail  】chiusir@aliyun.com
【软件版本】V1.0
【最后更新】2013年4月20日
【相关信息参考下列地址】
【网    站】http://www.lqist.cn
【淘宝店铺】http://shop36265907.taobao.com
------------------------------------------------
【dev.env.】CW7.2
【Target  】MCF52255
【Crystal 】48.000Mhz
【busclock】80.000MHz
【pllclock】?MHz    
------------------------------------
GPIOtest
TC闪灯
LED接到TC2,3两个口
********************************************************/
 
#include "support_common.h" /* include peripheral declarations and more */
void delay(int ms);

void delay(int ms) 
{
   int ii,jj;
   if (ms<1) ms=1;
   for(ii=0;ii<ms;ii++)     
     for(jj=0;jj<6676;jj++)
     {} 
}
 
int main(void)
{
	MCF_GPIO_PTCPAR =0X00;	//开启GPIO功能
	MCF_GPIO_DDRTC =0x03;//输出
	MCF_GPIO_PORTTC=0x00;//初始值
	MCF_GPIO_PTIPAR =0X00;	//开启GPIO功能
	MCF_GPIO_DDRTI =0x01;//输出
	MCF_GPIO_PORTTI=0x00;//初始值
	for(;;) 
	{	
		MCF_GPIO_PORTTC=0x00;//亮
		MCF_GPIO_PORTTI=0xff;//灭		
		delay(200);		
		MCF_GPIO_PORTTC=0xff;//灭
		MCF_GPIO_PORTTI=0x00;//亮		
		delay(200);	
	}
}
