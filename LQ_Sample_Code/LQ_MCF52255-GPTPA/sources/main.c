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
------------------------------------
脉冲检测输入脉冲测试
TC闪灯
把TA3接到TC0，然后通过单步仿真可以查看maichong数值
********************************************************/
 
#include "support_common.h" /* include peripheral declarations and more */
//.宏定义
//使能定时器计数
#define EnableGPT()   { (MCF_GPT_GPTSCR1 |= MCF_GPT_GPTSCR1_GPTEN); }
//禁止定时器计数
#define DisableGPT()  { (MCF_GPT_GPTSCR1 &= ~MCF_GPT_GPTSCR1_GPTEN);}
void delay(int ms);
void GPTPA_Init(void);
uint16 maichong=0;       //存放脉冲数

int main(void)
{		
	MCF_GPIO_PTCPAR =0X00;	//开启GPIO功能
	MCF_GPIO_DDRTC =0xff;//输出
	MCF_GPIO_PORTTC=0x00;//初始值
	GPTPA_Init();
	for(;;) 
	{	
		MCF_GPIO_PORTTC=0x00;//亮		
		delay(200);		
		MCF_GPIO_PORTTC=0xff;//灭		
		delay(200);
		maichong=MCF_GPT_GPTPACNT;//脉冲计数值	
	}
}

///-------------------------------------------------------------------------*
//函数名: GPTPA_Init                                                       *
//功  能: 初始化脉冲累加器,下降沿累加                                      * 
//参  数: 无                                                               *
//返  回: 无                                                               * 
//说  明: 初始化结束后,定时器计数是禁止的,脉冲累加器中断是禁止的           *
//-------------------------------------------------------------------------*
void GPTPA_Init(void)
{
    //1.配置TA.3引脚为GPT功能
    MCF_GPIO_PTAPAR |= 0x40;
    //2.禁止GPT
    DisableGPT();
    //3.设置GPT时钟
    MCF_GPT_GPTSCR2 |= MCF_GPT_GPTSCR2_PR(6);
    //4.设置GPT通道3功能为脉冲累加功能,下降沿累加,关闭脉冲累加中断
    MCF_GPT_GPTPACTL = 0x40; 
    //5.设置脉冲累加中断级别并清屏蔽位
    MCF_INTC0_ICR(42) = MCF_INTC_ICR_IL(3) 
                        | MCF_INTC_ICR_IP(3);
    MCF_INTC0_IMRH &= ~MCF_INTC_IMRH_INT_MASK42;
    MCF_INTC0_IMRL &= ~MCF_INTC_IMRL_MASKALL; 
    //6.清脉冲累加输入标志
    MCF_GPT_GPTPAFLG |= MCF_GPT_GPTPAFLG_PAIF;
    //7.清脉冲累加计数器
    MCF_GPT_GPTPACNT = 0x0000;
    //8.清GPT计数器
    MCF_GPT_GPTCNT = 0x0000;
}

void delay(int ms) 
{
   int ii,jj;
   if (ms<1) ms=1;
   for(ii=0;ii<ms;ii++)     
     for(jj=0;jj<6676;jj++)
     {} 
}
 