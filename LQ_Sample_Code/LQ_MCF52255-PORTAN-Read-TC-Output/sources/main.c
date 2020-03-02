/********************************************************
【平    台】龙丘MCF52255开发板V2.0
【编    写】龙丘
【Designed】by Chiu Sir
【E-mail  】chiusir@aliyun.com
【软件版本】V1.0
【最后更新】2013年1月23日
【相关信息参考下列地址】
【网    站】http://www.lqist.cn
【淘宝店铺】http://shop36265907.taobao.com
------------------------------------------------
【dev.env.】Code Warrior 7.2
【Target  】MCF52255
【Crystal 】48.000Mhz
【busclock】80.000MHz
【pllclock】80.000MHz

=================================
名称            核心板标识
PORTAN-0--------AN0
PORTAN-1--------AN1
PORTAN-2--------AN2
PORTAN-3--------AN3
PORTAN-4--------AN4
PORTAN-5--------AN5
PORTAN-6--------AN6
PORTAN-7--------AN7

PORTTC-0--------TC0
PORTTC-1--------TC1
PORTTC-2--------TC2
PORTTC-3--------TC3

从AN0-7输入,同时从TC0-3(TC口只有4bit)输出 
把AN2,3接低电平，led会亮
/********************************************************/
#include "support_common.h" /* include peripheral declarations and more */ 
#include "stdio.h"
void delay(uint16 ms);

void delay(uint16 ms)
{
   int ii,jj;
   if (ms<1) ms=1;
   for(ii=0;ii<ms;ii++)     
     for(jj=0;jj<6676;jj++) asm{nop};
       
}

unsigned char DAT=0;


int main(void)
{
    //LED的端口初始化为输出方向
	MCF_GPIO_PTCPAR =0X00;	//开启GPIO功能
	MCF_GPIO_DDRTC =0xff;//输出
	MCF_GPIO_PORTTC=0x00;//初始值
    
    
    //PORTAN输入口设置
    MCF_GPIO_PANPAR=0X00;//GPIO
    MCF_GPIO_DDRAN=0X00; //输入方向    
    //MCF_GPIO_PORTTE//输出数据所用所用寄存器
    //MCF_GPIO_SETAN;//输入数据读取所用寄存器

	for(;;) 
	{
      DAT=MCF_GPIO_SETAN;   //读取PORTAN的数值放到变量DAT
      MCF_GPIO_PORTTC=DAT;  //把变量的数值输出到PORTTE管脚
      delay(500);      
	  	
	}
}
