/********************************************************   
��ƽ    ̨������ColdFire V2�๦�ܿ�����
����    д������
��Designed��by Chiu Sir
��E-mail  ��chiusir@aliyun.com
������汾��V1.0
�������¡�2013��4��20��
�������Ϣ�ο����е�ַ��
����    վ��http://www.lqist.cn
���Ա����̡�http://shop36265907.taobao.com
------------------------------------------------
��dev.env.��CW7.2
��Target  ��MCF52255
��Crystal ��48.000Mhz
��busclock��80.000MHz
��pllclock��?MHz    
------------------------------------
GPIOtest
TC����
LED�ӵ�TC2,3������
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
	MCF_GPIO_PTCPAR =0X00;	//����GPIO����
	MCF_GPIO_DDRTC =0x03;//���
	MCF_GPIO_PORTTC=0x00;//��ʼֵ
	MCF_GPIO_PTIPAR =0X00;	//����GPIO����
	MCF_GPIO_DDRTI =0x01;//���
	MCF_GPIO_PORTTI=0x00;//��ʼֵ
	for(;;) 
	{	
		MCF_GPIO_PORTTC=0x00;//��
		MCF_GPIO_PORTTI=0xff;//��		
		delay(200);		
		MCF_GPIO_PORTTC=0xff;//��
		MCF_GPIO_PORTTI=0x00;//��		
		delay(200);	
	}
}
