/********************************************************   
��ƽ    ̨������ColdFire V2�๦�ܿ�����
����    д������
��Designed��by Chiu Sir
��E-mail  ��chiusir@aliyun.com
������汾��V1.0
�������¡�2013��7��25��
�������Ϣ�ο����е�ַ��
����    վ��http://www.lqist.cn
���Ա����̡�http://shop36265907.taobao.com
------------------------------------------------
��dev.env.��CW7.2
��Target  ��MCF52255
��Crystal ��48.000Mhz
��busclock��80.000MHz
��pllclock��?MHz    
***************************
------------------------------------
���ܳ��ۺϳ����
EPORT�ⲿ�ж���ʾ
���س���
���г���
������İ���IRQ7��νӵأ������ⲿ�жϣ�TC0(PWM0)�ڻ���˸


�������̣�
1.��ʼ��EPROT                              --������ԭʼmain.c�У�75������
2.����жϷ�����                         --������ԭʼmain.c�У�167������
3.���ж����������ļ���ǰ�������жϷ����� --������ԭʼmain.c�У�39������
4.�������������жϷ�����                 --������ԭʼexceptions.c�У�423������

********************************************************/
#include "support_common.h" /* include peripheral declarations and more */

int i;
#define EnableInterrupts asm { move.w SR,D0; andi.l #0xF8FF,D0; move.w D0,SR;  }

void EPORT_init(void);
__declspec(interrupt:0) void EPORT_irq1_handler(void);
__declspec(interrupt:0) void EPORT_irq2_handler(void);
__declspec(interrupt:0) void EPORT_irq3_handler(void);
__declspec(interrupt:0) void EPORT_irq4_handler(void);
__declspec(interrupt:0) void EPORT_irq5_handler(void);
__declspec(interrupt:0) void EPORT_irq6_handler(void);
__declspec(interrupt:0) void EPORT_irq7_handler(void);

int main(void)
{
	int counter = 0;
	
	
	MCF_GPIO_PTCPAR = MCF_GPIO_PTCPAR_DTIN0_GPIO
				     |MCF_GPIO_PTCPAR_DTIN1_GPIO
				     |MCF_GPIO_PTCPAR_DTIN2_GPIO
				     |MCF_GPIO_PTCPAR_DTIN3_GPIO;
				   
	MCF_GPIO_DDRTC = MCF_GPIO_DDRTC_DDRTC0
				    |MCF_GPIO_DDRTC_DDRTC1
				    |MCF_GPIO_DDRTC_DDRTC2
				    |MCF_GPIO_DDRTC_DDRTC3;
				  
	MCF_GPIO_PORTTC = MCF_GPIO_PORTTC_PORTTC0;
	
	EPORT_init();
	EnableInterrupts;

	for(;;) {	   
	   	counter++;
	   	
	}
}



void EPORT_init(void)
{
	MCF_GPIO_PNQPAR = MCF_GPIO_PNQPAR_IRQ1_IRQ1
				     |MCF_GPIO_PNQPAR_IRQ3_IRQ3
				     |MCF_GPIO_PNQPAR_IRQ5_IRQ5
				     |MCF_GPIO_PNQPAR_IRQ7_IRQ7;
//pin assignment register:FALLING LEVEL RISING BOTH			   
	MCF_EPORT_EPPAR = MCF_EPORT_EPPAR_EPPA1_FALLING
				 	 |MCF_EPORT_EPPAR_EPPA3_FALLING
				 	 |MCF_EPORT_EPPAR_EPPA5_FALLING
					 |MCF_EPORT_EPPAR_EPPA7_FALLING;
//data direction register
	MCF_EPORT_EPDDR =(uint8)~MCF_EPORT_EPDDR_EPDD1
					 &~MCF_EPORT_EPDDR_EPDD3
					 &~MCF_EPORT_EPDDR_EPDD5
					 &~MCF_EPORT_EPDDR_EPDD7;
//interrupt enable register
	MCF_EPORT_EPIER =// MCF_EPORT_EPIER_EPIE1	//EPORT interrupt source 1
					 //|MCF_EPORT_EPIER_EPIE3	//EPORT interrupt source 3
				 	 //|MCF_EPORT_EPIER_EPIE5	//EPORT interrupt source 5
					  MCF_EPORT_EPIER_EPIE7;//EPORT interrupt source 7
//data register:hold the data to be driven to the pins
	MCF_EPORT_EPDR = MCF_EPORT_EPDR_EPD1
	  			    |MCF_EPORT_EPDR_EPD3
	  			    |MCF_EPORT_EPDR_EPD5
	  			    |MCF_EPORT_EPDR_EPD7;
//data register:reflects the current state of the pins READ ONLY
//	MCF_EPORT_EPPDR
//flag register:individually latches EPORT edge events
	MCF_EPORT_EPFR = MCF_EPORT_EPFR_EPF1
				    |MCF_EPORT_EPFR_EPF2
				    |MCF_EPORT_EPFR_EPF3
				    |MCF_EPORT_EPFR_EPF4
				    |MCF_EPORT_EPFR_EPF5
				    |MCF_EPORT_EPFR_EPF6
				    |MCF_EPORT_EPFR_EPF7;
				   
//set interrupt enable				   
	MCF_INTC0_IMRL&=~MCF_INTC_IMRL_MASKALL;
	MCF_INTC0_IMRL&=~MCF_INTC_IMRL_INT_MASK1 //EPORT interrupt source 1
				   &~MCF_INTC_IMRL_INT_MASK3 //EPORT interrupt source 3
				   &~MCF_INTC_IMRL_INT_MASK5 //EPORT interrupt source 5
				   &~MCF_INTC_IMRL_INT_MASK7;//EPORT interrupt source 7
				   
	MCF_INTC0_ICR01=MCF_INTC_ICR_IP(1)+MCF_INTC_ICR_IL(3);
	MCF_INTC0_ICR03=MCF_INTC_ICR_IP(2)+MCF_INTC_ICR_IL(3);
	MCF_INTC0_ICR05=MCF_INTC_ICR_IP(3)+MCF_INTC_ICR_IL(3);
	MCF_INTC0_ICR07=MCF_INTC_ICR_IP(4)+MCF_INTC_ICR_IL(3);
	
}


__declspec(interrupt:0) void EPORT_irq1_handler(void)//source 1
{
	MCF_EPORT_EPFR|=MCF_EPORT_EPFR_EPF1;

/******************************
	asm
	{
		stop #0x2700;
	}
	//	power_off(35);
	//	Pit_status=OFF;
******************************/	
}

__declspec(interrupt:0) void EPORT_irq2_handler(void)
{
	MCF_EPORT_EPFR|=MCF_EPORT_EPFR_EPF2;
	
}

__declspec(interrupt:0) void EPORT_irq3_handler(void)
{
	MCF_EPORT_EPFR|=MCF_EPORT_EPFR_EPF3;	
}

__declspec(interrupt:0) void EPORT_irq4_handler(void)
{
	MCF_EPORT_EPFR|=MCF_EPORT_EPFR_EPF4;	
}

__declspec(interrupt:0) void EPORT_irq5_handler(void)
{
	MCF_EPORT_EPFR|=MCF_EPORT_EPFR_EPF5;	
}

__declspec(interrupt:0) void EPORT_irq6_handler(void)
{
	MCF_EPORT_EPFR|=MCF_EPORT_EPFR_EPF6;	
}

__declspec(interrupt:0) void EPORT_irq7_handler(void)
{
	MCF_EPORT_EPFR|=MCF_EPORT_EPFR_EPF7;
			if(++i%2)
	   	{
	   		MCF_GPIO_PORTTC=0x00;	
	   	}
	   	else
	   		MCF_GPIO_PORTTC=0xFF;
}