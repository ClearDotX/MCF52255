/********************************************************
【平    台】龙丘MCF52110开发板V2.0
【编    写】龙丘
【Designed】by Chiu Sir
【E-mail  】chiusir@yahoo.cn
【软件版本】V1.0
【最后更新】2013年4月23日
【相关信息参考下列地址】
【网    站】http://www.lqist.cn
【淘宝店铺】http://shop36265907.taobao.com
------------------------------------------------
【dev.env.】Code Warrior 7.2
【Target  】MCF52110
【Crystal 】48.000Mhz
【busclock】80.000MHz
【pllclock】80.000MHz

板子已经接地：CS  片选管脚
UB3:DC  命令数据选择管脚
UB2:RES 模块复位管脚 
UB1:D1（MOSI） ，主输出从输入数据脚，由MCU控制
UB0:D0（SCLK） ，时钟脚，由MCU控制
=================================
名称            核心板标识
PORTAN-0--------A0
PORTAN-1--------A1
PORTAN-2--------A2
PORTAN-3--------A3
PORTAN-4--------A4
PORTAN-5--------A5
PORTAN-6--------A6
PORTAN-7--------A7
 
/********************************************************/
#include "support_common.h" /* include peripheral declarations and more */
#include "LQ12864.h" 
#include "stdio.h"

extern uint8 longqiu96x64[];
void Dly_ms(uint16 ms);
void led_init();
unsigned char DAT=0; 

void Dly_ms(uint16 ms)
{
   int ii,jj;
   if (ms<1) ms=1;
   for(ii=0;ii<ms;ii++)
     //for(jj=0;jj<50;jj++);     //10us
     //for(jj=0;jj<2770;jj++);    //32MHz--1ms
     for(jj=0;jj<4006;jj++){}//48MHz--1ms 
     //for(jj=0;jj<5341;jj++);  //64MHz--1ms  
}

void led_init()
{
	MCF_GPIO_PTCPAR&=0X00;
	
	MCF_GPIO_DDRTC|=MCF_GPIO_DDRTC_DDRTC2 |MCF_GPIO_DDRTC_DDRTC1
		|MCF_GPIO_DDRTC_DDRTC0|MCF_GPIO_DDRTC_DDRTC3;
	MCF_GPIO_PORTTC &=0xf0;
}


int main(void)
{
	int counter=0,i=0,j=0;
	char txt[32];
		

    led_init();
    //OLED的端口初始化为输出方向
    MCF_GPIO_PUCPAR=0;    //GPIO
    MCF_GPIO_DDRUC=0XFF;  // 输出方向
    MCF_GPIO_PORTUC&=0X0F;// 初始值
    
    //PORTAN输入口设置
    MCF_GPIO_PANPAR=0X00;//GPIO
    MCF_GPIO_DDRAN=0X00; //输入方向    
    //MCF_GPIO_PORTTE//输出数据所用所用寄存器
    //MCF_GPIO_SETAN;//输入数据读取所用寄存器
	  LCD_Init();	
	  LCD_Fill(0xff); 
      Dly_ms(1000); 
      LCD_Fill(0x00);
      Dly_ms(1000);      
      LCD_Print(8,0,(uint8 *)"北京龙丘智能科技");
      LCD_Print(15,2,(uint8 *)"智能车首选液晶"); 
      LCD_P8x16Str(48,4,(uint8 *)"OLED");
      LCD_P6x8Str(16,6,(uint8 *)"chiusir@yahoo.cn"); 
      LCD_P6x8Str(34,7,(uint8 *)"2013-01-23");    
      Dly_ms(2000);       
      LCD_CLS();
      Draw_BMP(16,0,112,7,longqiu96x64); 
      Dly_ms(2000);
	for(;;) 
	{
      DAT=MCF_GPIO_SETAN;
      sprintf(txt,"SETTAN:0x%02x",DAT);//将IO口数据转换为字符串，以便OLED显示
      LCD_P8x16Str(16,6,(unsigned char*)txt);
      Dly_ms(500);
      MCF_GPIO_PORTTC =(unsigned char)~MCF_GPIO_PORTTC;//强制类型转化可以消除警告，用户不必理会
	  	
	}
}
