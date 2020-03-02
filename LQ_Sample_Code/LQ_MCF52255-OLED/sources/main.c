/********************************************************
��ƽ    ̨������MCF52110������V2.0
����    д������
��Designed��by Chiu Sir
��E-mail  ��chiusir@aliyun.com
������汾��V1.0
�������¡�2012��3��28��
�������Ϣ�ο����е�ַ��
����    վ��http://www.lqist.cn
���Ա����̡�http://shop36265907.taobao.com
------------------------------------------------
��dev.env.��Code Warrior 7.2
��Target  ��MCF52255
��Crystal ��48.000Mhz
��busclock��80.000MHz
��pllclock��80.000MHz

�����Ѿ��ӵأ�CS  Ƭѡ�ܽ�
UB3:DC  ��������ѡ��ܽ�
UB2:RES ģ�鸴λ�ܽ� 
UB1:D1��MOSI�� ����������������ݽţ���MCU����
UB0:D0��SCLK�� ��ʱ�ӽţ���MCU����
3.3V:VCC
GND:GND

���س�����ȷ���߼��ɡ�
/********************************************************/
#include "support_common.h" /* include peripheral declarations and more */
#include "LQ12864.h" 

extern uint8 longqiu96x64[];
void Dly_ms(uint16 ms);
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
void led_init();
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

    led_init();
    MCF_GPIO_PUCPAR=0;
    MCF_GPIO_DDRUC=0XFF; 
    MCF_GPIO_PORTUC&=0X0F; 
	LCD_Init();	
	for(;;) 
	{	   
	  LCD_Fill(0xff); 
      Dly_ms(1000); 
      LCD_Fill(0x00);
      Dly_ms(1000);      
      LCD_Print(8,0,(uint8 *)"�����������ܿƼ�");
      LCD_Print(15,2,(uint8 *)"���ܳ���ѡҺ��");
      LCD_P8x16Str(48,4,(uint8 *)"OLED");
      LCD_P6x8Str(16,6,(uint8 *)"chiusir@aliyun.com"); 
      LCD_P6x8Str(34,7,(uint8 *)"2012-03-03");    
      Dly_ms(2000);       
      LCD_CLS();
      Draw_BMP(16,0,112,7,longqiu96x64); 
      Dly_ms(2000);
      MCF_GPIO_PORTTC =~MCF_GPIO_PORTTC;
	  	
	}
}
