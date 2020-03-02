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
------------------------------------
�����������������
TC����
��TA3�ӵ�TC0��Ȼ��ͨ������������Բ鿴maichong��ֵ
********************************************************/
 
#include "support_common.h" /* include peripheral declarations and more */
//.�궨��
//ʹ�ܶ�ʱ������
#define EnableGPT()   { (MCF_GPT_GPTSCR1 |= MCF_GPT_GPTSCR1_GPTEN); }
//��ֹ��ʱ������
#define DisableGPT()  { (MCF_GPT_GPTSCR1 &= ~MCF_GPT_GPTSCR1_GPTEN);}
void delay(int ms);
void GPTPA_Init(void);
uint16 maichong=0;       //���������

int main(void)
{		
	MCF_GPIO_PTCPAR =0X00;	//����GPIO����
	MCF_GPIO_DDRTC =0xff;//���
	MCF_GPIO_PORTTC=0x00;//��ʼֵ
	GPTPA_Init();
	for(;;) 
	{	
		MCF_GPIO_PORTTC=0x00;//��		
		delay(200);		
		MCF_GPIO_PORTTC=0xff;//��		
		delay(200);
		maichong=MCF_GPT_GPTPACNT;//�������ֵ	
	}
}

///-------------------------------------------------------------------------*
//������: GPTPA_Init                                                       *
//��  ��: ��ʼ�������ۼ���,�½����ۼ�                                      * 
//��  ��: ��                                                               *
//��  ��: ��                                                               * 
//˵  ��: ��ʼ��������,��ʱ�������ǽ�ֹ��,�����ۼ����ж��ǽ�ֹ��           *
//-------------------------------------------------------------------------*
void GPTPA_Init(void)
{
    //1.����TA.3����ΪGPT����
    MCF_GPIO_PTAPAR |= 0x40;
    //2.��ֹGPT
    DisableGPT();
    //3.����GPTʱ��
    MCF_GPT_GPTSCR2 |= MCF_GPT_GPTSCR2_PR(6);
    //4.����GPTͨ��3����Ϊ�����ۼӹ���,�½����ۼ�,�ر������ۼ��ж�
    MCF_GPT_GPTPACTL = 0x40; 
    //5.���������ۼ��жϼ���������λ
    MCF_INTC0_ICR(42) = MCF_INTC_ICR_IL(3) 
                        | MCF_INTC_ICR_IP(3);
    MCF_INTC0_IMRH &= ~MCF_INTC_IMRH_INT_MASK42;
    MCF_INTC0_IMRL &= ~MCF_INTC_IMRL_MASKALL; 
    //6.�������ۼ������־
    MCF_GPT_GPTPAFLG |= MCF_GPT_GPTPAFLG_PAIF;
    //7.�������ۼӼ�����
    MCF_GPT_GPTPACNT = 0x0000;
    //8.��GPT������
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
 