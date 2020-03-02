/********************************************************
��ƽ    ̨������MCF52255������V2.0
����    д������
��Designed��by Chiu Sir
��E-mail  ��chiusir@aliyun.com
������汾��V1.0
�������¡�2013��1��23��
�������Ϣ�ο����е�ַ��
����    վ��http://www.lqist.cn
���Ա����̡�http://shop36265907.taobao.com
------------------------------------------------
��dev.env.��Code Warrior 7.2
��Target  ��MCF52255
��Crystal ��48.000Mhz
��busclock��80.000MHz
��pllclock��80.000MHz

=================================
����            ���İ��ʶ
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

��AN0-7����,ͬʱ��TC0-3(TC��ֻ��4bit)��� 
��AN2,3�ӵ͵�ƽ��led����
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
    //LED�Ķ˿ڳ�ʼ��Ϊ�������
	MCF_GPIO_PTCPAR =0X00;	//����GPIO����
	MCF_GPIO_DDRTC =0xff;//���
	MCF_GPIO_PORTTC=0x00;//��ʼֵ
    
    
    //PORTAN���������
    MCF_GPIO_PANPAR=0X00;//GPIO
    MCF_GPIO_DDRAN=0X00; //���뷽��    
    //MCF_GPIO_PORTTE//��������������üĴ���
    //MCF_GPIO_SETAN;//�������ݶ�ȡ���üĴ���

	for(;;) 
	{
      DAT=MCF_GPIO_SETAN;   //��ȡPORTAN����ֵ�ŵ�����DAT
      MCF_GPIO_PORTTC=DAT;  //�ѱ�������ֵ�����PORTTE�ܽ�
      delay(500);      
	  	
	}
}
