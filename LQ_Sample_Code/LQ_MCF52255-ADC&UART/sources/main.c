/********************************************************
����MCF52255�๦�ܿ�����V1.0
��д:����
Designed by Chiu Sir
E-mail:chiusir@aliyun.com
����汾:V1.1
������:2013��4��19���賿
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
�Ű���һ�˽�GND����һ�˷ֱ�Ӻ��İ�����½�AN0--AN7�����Կ������ߵ��ĸ�ͨ����
�Ǹ�ͨ�����������0.000V��������ͨ����Ϊ�����ѹֵ��
�������£�
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

void ADinit (void); //ADת��ģ���ʼ��
uint16 ADonce(uint16 ANn); //12λ���Ȳɼ�ĳ·ģ������ADֵ
void Cvt_16bit_Str(uint8 ch,char str[],uint16 V2,uint8 st);
//������:ADinit                                                            *
//��  ��:ADת��ģ���ʼ��                                                  *
// 	     (1)ѭ��˳��ɨ��ģʽ,AN0-7��Ϊ��������,��ֹ�ж�,ֹͣת��           *
// 	     (2)ADCʱ��Ƶ��3MHz     		 	                               *
// 	     (3)�ϵ���ʱΪ4��ADCʱ������                                       *
//       (4)ʹ���ڲ��ο���ѹVRH��VRL                                       *
// 	     (5)��������8������ͨ��                                            *
//       (6)����ת��                                                       *
//��  ��:��                                                                *
//��  ��:��                                                                *
//-------------------------------------------------------------------------*
void ADinit (void) //ADת��ģ���ʼ��
{

    //1.������AN(0--7)����ΪADCת������                           
        //MCF_GPIO_PANPAR = 0xFFFF;
    	MCF_GPIO_PANPAR=MCF_GPIO_PANPAR_AN0_AN0
				   |MCF_GPIO_PANPAR_AN1_AN1
				   |MCF_GPIO_PANPAR_AN2_AN2
				   |MCF_GPIO_PANPAR_AN3_AN3
				   |MCF_GPIO_PANPAR_AN4_AN4
				   |MCF_GPIO_PANPAR_AN5_AN5
			   	   |MCF_GPIO_PANPAR_AN6_AN6
				   |MCF_GPIO_PANPAR_AN7_AN7;

    //2.ѭ��˳��ɨ��ģʽ,AN0-7��Ϊ��������,��ֹ�ж�,ֹͣת��
    MCF_ADC_CTRL1 =  0b0101000000000010;
                  //    ||||||||||| |||___SMODE=2,ѭ��˳��ɨ��
                  //    |||||||||||_______CHNCFG=0,8·ANn����Ϊ��������  
                  //    |||||||___________HLMTIE=0,��ֹ�������ж�
                  //    ||||||____________LLMTIE=0,��ֹ�������ж�
                  //    |||||_____________ZCIE=0,��ֹ�����ж�
                  //    ||||______________EOSIE0=0,��ֹת������ж�
                  //    |||_______________SYNC0=1,��SYNC0�����дSTART0����ɨ��
                  //    ||________________START0=0,δ����ɨ��
                  //    |_________________STOP0=1,ֹͣת��
    //3.ADCʱ��Ƶ��2MHz
    MCF_ADC_CTRL2 = 0b0000000000010011;
                  //             |||||____DIV=40,ADCʱ��Ƶ��2MHz 
                  //                      (fBUS=80MHz,fAD=fBUS/((DIV+1)*2=2MHz)
    //4.ת����A��B�Ͳο���ѹ��·�ϵ�,�ϵ���ʱ10��ADCʱ������
    MCF_ADC_POWER = 0b0000000010100100;
                  //  |     ||||||||||____PD0=0,ת����A�ϵ�
                  //  |     |||||||||_____PD1=0,ת����B�ϵ�
                  //  |     ||||||||______PD2=1,�ο���ѹ��·�ϵ�������ת����A/B
                  //  |     |||||||_______APD=0,��ֹ�Զ�����
                  //  |     ||||||________PUDELAY=10,�ϵ���ʱΪ10��ADCʱ������     
                  //  |___________________ASB=0,��ֹ�Զ��͹���ģʽ
    //5.ʹ���ڲ��ο���ѹVRH��VRL
    MCF_ADC_CAL   = 0b0000000000000000;
                  //  ||__________________SEL_VREFH=0,SEL_VREFL=0
                  //                      ʹ���ڲ��ο���ѹVRH��VRL
    
    //6.��ֹͣ0λ,STOP0=0
    MCF_ADC_CTRL1 &= 0xBFFF;
    
    //7.������ƽ�ֹ ��ת���������һ��ת������з��ű仯
	MCF_ADC_ADZCC=0;
	
	//8.ʹ��SAMPLEn������			  
	MCF_ADC_ADSDIS=0;
	
	//9.ƫ�����Ĵ���Ϊ0  ����Ĵ���Ϊ�޷�����
	MCF_ADC_ADOFS(0)=0X0000;
	
    //10.�ȴ�ת����A�ϵ����
    while (MCF_ADC_POWER & MCF_ADC_POWER_PSTS0) asm{ nop};
    //11.�ȴ�ת����B�ϵ����
    while (MCF_ADC_POWER & MCF_ADC_POWER_PSTS1) asm{ nop};
    //12.��START0д1,����ɨ��
    MCF_ADC_CTRL1 |= 0x2000; 
    
    return;
    
}

//-------------------------------------------------------------------------*
//������:ADonce                                                            *
//��  ��:�ɼ�һ��AN0-AN7������һ·��ģ������ADֵ                           *
//��  ��:ANn:ģ��������˿ں�(��Χ:0-7)                                    *
//��  ��:�޷��Ž��ֵ(��Χ:0-4095)                                         *
//-------------------------------------------------------------------------*
uint16 ADonce(uint16 ANn) //12λ���Ȳɼ�ĳ·ģ������ADֵ
{
    uint16 result;
    //�ȴ�����׼����
    while ((MCF_ADC_ADSTAT & (1 << ANn))==0) asm{ nop};
    //ȡ�ý��
    result = (MCF_ADC_ADRSLT(ANn) & 0x7FF8) >> 3;
    return result;
}
//����Ƽ��Լ�д�ĺ����������ݸ�ʽ�����������Ҳο�
//                 ����        ��ת����   �����е���ʼλ��
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
  str[st++] ='.'; //С����
  str[st++] = characters[(uint8)((((uint16)V1)%1000)/100)];
  str[st++] = characters[(uint8)((((uint16)V1)%100)/10)];
  str[st++] = characters[(uint8)(( (uint16)V1)%10)];  
  str[st++] ='V';
  str[st] ='\0';// end 
}  
int main(void)
{
	uint8 ii=0,txtbuf[16];
	
	MCF_GPIO_PTCPAR =0X00;	//����GPIO����
	MCF_GPIO_DDRTC =0xff;//���
	MCF_GPIO_PORTTC=0x00;//��ʼֵ
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
