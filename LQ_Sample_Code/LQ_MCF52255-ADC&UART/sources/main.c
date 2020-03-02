/********************************************************
龙丘MCF52255多功能开发板V1.0
编写:龙丘
Designed by Chiu Sir
E-mail:chiusir@aliyun.com
软件版本:V1.1
最后更新:2013年4月19日凌晨
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
杜邦线一端接GND，另一端分别接核心板的右下角AN0--AN7，可以看到接线到哪个通道，
那个通道的输出就是0.000V，而其余通道则为随机电压值。
数据如下：
Ch[0]: 00.000V  LONGQIU: UART1 OK!
Ch[1]: 00.667V  LONGQIU: UART1 OK!
Ch[2]: 01.025V  LONGQIU: UART1 OK!
Ch[3]: 01.040V  LONGQIU: UART1 OK!
Ch[4]: 02.004V  LONGQIU: UART1 OK!
Ch[5]: 01.682V  LONGQIU: UART1 OK!
Ch[6]: 01.415V  LONGQIU: UART1 OK!
Ch[7]: 01.419V  LONGQIU: UART1 OK!

********************************************************/
#include "support_common.h" /* include peripheral declarations and more */
#include "MCF52255_UART_driver.h"
#include <stdio.h>
#include <math.h>

void memcpy(int8* des,int8* src,int num);

char *str1="abcdefghijklmn";
char *str2="0123456789"; 
void delayms(int ms);

void delayms(int ms)
{
   int ii,jj;
   if (ms<1) ms=1;
   for(ii=0;ii<ms;ii++)     
     for(jj=0;jj<6676;jj++) asm{ nop};  //80MHz--1ms  
}
uint16 ADvalue;
float Vout;

void ADinit (void); //AD转换模块初始化
uint16 ADonce(uint16 ANn); //12位精度采集某路模拟量的AD值
void Cvt_16bit_Str(uint8 ch,char str[],uint16 V2,uint8 st);
//函数名:ADinit                                                            *
//功  能:AD转换模块初始化                                                  *
// 	     (1)循环顺序扫描模式,AN0-7设为单端输入,禁止中断,停止转换           *
// 	     (2)ADC时钟频率3MHz     		 	                               *
// 	     (3)上电延时为4个ADC时钟周期                                       *
//       (4)使用内部参考电压VRH和VRL                                       *
// 	     (5)开放所有8个采样通道                                            *
//       (6)启动转换                                                       *
//参  数:无                                                                *
//返  回:无                                                                *
//-------------------------------------------------------------------------*
void ADinit (void) //AD转换模块初始化
{

    //1.将引脚AN(0--7)配置为ADC转换功能                           
        //MCF_GPIO_PANPAR = 0xFFFF;
    	MCF_GPIO_PANPAR=MCF_GPIO_PANPAR_AN0_AN0
				   |MCF_GPIO_PANPAR_AN1_AN1
				   |MCF_GPIO_PANPAR_AN2_AN2
				   |MCF_GPIO_PANPAR_AN3_AN3
				   |MCF_GPIO_PANPAR_AN4_AN4
				   |MCF_GPIO_PANPAR_AN5_AN5
			   	   |MCF_GPIO_PANPAR_AN6_AN6
				   |MCF_GPIO_PANPAR_AN7_AN7;

    //2.循环顺序扫描模式,AN0-7设为单端输入,禁止中断,停止转换
    MCF_ADC_CTRL1 =  0b0101000000000010;
                  //    ||||||||||| |||___SMODE=2,循环顺序扫描
                  //    |||||||||||_______CHNCFG=0,8路ANn都设为单端输入  
                  //    |||||||___________HLMTIE=0,禁止超上限中断
                  //    ||||||____________LLMTIE=0,禁止超下限中断
                  //    |||||_____________ZCIE=0,禁止过零中断
                  //    ||||______________EOSIE0=0,禁止转换完成中断
                  //    |||_______________SYNC0=1,由SYNC0输入或写START0启动扫描
                  //    ||________________START0=0,未启动扫描
                  //    |_________________STOP0=1,停止转换
    //3.ADC时钟频率2MHz
    MCF_ADC_CTRL2 = 0b0000000000010011;
                  //             |||||____DIV=40,ADC时钟频率2MHz 
                  //                      (fBUS=80MHz,fAD=fBUS/((DIV+1)*2=2MHz)
    //4.转换器A和B和参考电压电路上电,上电延时10个ADC时钟周期
    MCF_ADC_POWER = 0b0000000010100100;
                  //  |     ||||||||||____PD0=0,转换器A上电
                  //  |     |||||||||_____PD1=0,转换器B上电
                  //  |     ||||||||______PD2=1,参考电压电路上电依赖于转换器A/B
                  //  |     |||||||_______APD=0,禁止自动掉电
                  //  |     ||||||________PUDELAY=10,上电延时为10个ADC时钟周期     
                  //  |___________________ASB=0,禁止自动低功耗模式
    //5.使用内部参考电压VRH和VRL
    MCF_ADC_CAL   = 0b0000000000000000;
                  //  ||__________________SEL_VREFH=0,SEL_VREFL=0
                  //                      使用内部参考电压VRH和VRL
    
    //6.清停止0位,STOP0=0
    MCF_ADC_CTRL1 &= 0xBFFF;
    
    //7.过零控制禁止 即转换结果和上一次转换结果有符号变化
	MCF_ADC_ADZCC=0;
	
	//8.使能SAMPLEn采样槽			  
	MCF_ADC_ADSDIS=0;
	
	//9.偏移量寄存器为0  结果寄存器为无符号型
	MCF_ADC_ADOFS(0)=0X0000;
	
    //10.等待转换器A上电完成
    while (MCF_ADC_POWER & MCF_ADC_POWER_PSTS0) asm{ nop};
    //11.等待转换器B上电完成
    while (MCF_ADC_POWER & MCF_ADC_POWER_PSTS1) asm{ nop};
    //12.向START0写1,启动扫描
    MCF_ADC_CTRL1 |= 0x2000; 
    
    return;
    
}

//-------------------------------------------------------------------------*
//函数名:ADonce                                                            *
//功  能:采集一次AN0-AN7中任意一路的模拟量的AD值                           *
//参  数:ANn:模拟量输入端口号(范围:0-7)                                    *
//返  回:无符号结果值(范围:0-4095)                                         *
//-------------------------------------------------------------------------*
uint16 ADonce(uint16 ANn) //12位精度采集某路模拟量的AD值
{
    uint16 result;
    //等待数据准备好
    while ((MCF_ADC_ADSTAT & (1 << ANn))==0) asm{ nop};
    //取得结果
    result = (MCF_ADC_ADRSLT(ANn) & 0x7FF8) >> 3;
    return result;
}
//龙丘科技自己写的函数，把数据格式化输出，供大家参考
//                 数组        待转变量   数组中的起始位置
void Cvt_16bit_Str(uint8 ch,char str[],uint16 V1,uint8 st)
{
  char characters[17]="0123456789ABCDEF";  
  str[st++] ='C';
  str[st++] ='h'; 
  str[st++] ='[';  
  str[st++] = characters[ch]; 
  str[st++] =']'; 
  str[st++] =':'; 
  str[st++] =' '; 
  str[st++] = characters[(uint8)(V1/10000)];    
  str[st++] = characters[(uint8)((((uint16)V1)%10000)/1000)];  
  str[st++] ='.'; //小数点
  str[st++] = characters[(uint8)((((uint16)V1)%1000)/100)];
  str[st++] = characters[(uint8)((((uint16)V1)%100)/10)];
  str[st++] = characters[(uint8)(( (uint16)V1)%10)];  
  str[st++] ='V';
  str[st] ='\0';// end 
}  
int main(void)
{
	uint8 ii=0,txtbuf[16];
	
	MCF_GPIO_PTCPAR =0X00;	//开启GPIO功能
	MCF_GPIO_DDRTC =0xff;//输出
	MCF_GPIO_PORTTC=0x00;//初始值
	uart1_init(9600);		
	uart0_init(9600);
	ADinit (); 
	//uart2_init(9600);
	while(1)
	{
		for(ii=0;ii<8;ii++) 
		{	
			ADvalue=ADonce(ii);	
	        Vout=(uint32)ADvalue*220/273;//3300/4095;
	        Cvt_16bit_Str(ii,(char*)txtbuf,Vout,0);
			uart1_putstr((char*)txtbuf);
			uart1_putstr("  LONGQIU: UART1 OK!\n");
			uart0_putstr((char*)txtbuf);
			uart0_putstr("  LONGQIU: UART0 OK!\n");
			MCF_GPIO_PORTTC=~MCF_GPIO_PORTTC;		
		    delayms(50);
		}
		uart0_putstr("\n");
		uart1_putstr("\n");
		delayms(2000);
	}

}
