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
MCF52255_UART_driver.c 第32行	SYS_CLOCK=80000000; 
MCF52255_UART_driver.c 第114行	SYS_CLOCK=80000000; 
MCF52255_sysinit.c     第204行	PLL_80M(); //龙丘提示：修改主频，看上面的函数定以即可

8路ADC演示方法:
将核心板的TXD0或者TXD1接到USB转TTL的TXD管脚，通过串口调试助手可以查看数据。
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
#include "MCF52255_UART_driver.h"

char receive[30]=
{
	'B',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};
extern uint8 status;

/* 
*Initialization of UART0
*/
void uart0_init(uint16 baudrate)
{	
	uint32 div,SYS_CLOCK;
	SYS_CLOCK=80000000;   //龙丘提示：修改主频，跟系统频率一致才行
	
	MCF_GPIO_PUAPAR=MCF_GPIO_PUAPAR_UTXD0_UTXD0
				   |MCF_GPIO_PUAPAR_URXD0_URXD0;
	
//Reset Transmitter Receiver Mode Register
	MCF_UART0_UCR|=(0
				  |MCF_UART_UCR_RESET_TX
	              |MCF_UART_UCR_RESET_RX                                     
	              |MCF_UART_UCR_RESET_MR);             

//No parity,8bit data
	MCF_UART0_UMR1=(0
		          |MCF_UART_UMR_PM_NONE
	              |MCF_UART_UMR_BC_8);
		
//1bit stop
	MCF_UART0_UMR2|=MCF_UART_UMR_CM_NORMAL
				  |MCF_UART_UMR_SB_STOP_BITS_1;
		                
//Set Rx and Tx buad by SYSTERM CLOCK
	MCF_UART0_UCSR=(0
				  |MCF_UART_UCSR_RCS_SYS_CLK
		  		  |MCF_UART_UCSR_TCS_SYS_CLK);
	
//Set	UART interrupts	
	MCF_UART0_UIMR=0;//MCF_UART_UISR_FFULL_RXRDY;
				  //|MCF_UART_UISR_TXRDY;
				  //|MCF_UART_UISR_DB
				  //|MCF_UART_UISR_COS
	
	MCF_INTC0_IMRL&=~MCF_INTC_IMRL_MASKALL;
	MCF_INTC0_IMRL&=~MCF_INTC_IMRL_INT_MASK13;//UART0
	MCF_INTC0_ICR13=MCF_INTC_ICR_IP(6)+MCF_INTC_ICR_IL(2);//UART0	
	
	
//set buad rate
	div=(SYS_CLOCK/32/baudrate);
	MCF_UART0_UBG1=(unsigned char)(div>>8);
	MCF_UART0_UBG2=(unsigned char)(div&0x00ff);
	
//Enable Tx/Rx
	MCF_UART0_UCR=(0
				 |MCF_UART_UCR_TX_ENABLED
				 |MCF_UART_UCR_RX_ENABLED);
}

void uart0_putchar(char c)
{
//Wait until space is available in the FIFO 
    while (!(MCF_UART0_USR&MCF_UART_USR_TXRDY)) 
    {
    	;
    }

    //Send the character 
    MCF_UART0_UTB = (unsigned char)c;
}

unsigned char uart0_getchar()
{
//Wait until character has been received 
    while (!(MCF_UART0_USR & MCF_UART_USR_RXRDY)) 
    {
    	
    };

    return MCF_UART0_URB;	
}

void uart0_putstr(char *str)
{
	while(*str!=0)
	{
		uart0_putchar(*str++);
	}
}


void uart1_init(unsigned long baudrate)
{
    unsigned int div;
    uint32 sysclk=80000000;
    MCF_GPIO_PUBPAR=MCF_GPIO_PUBPAR_UTXD1_UTXD1
    			   |MCF_GPIO_PUBPAR_URXD1_URXD1;
    			   //|MCF_GPIO_PUBPAR_URTS1_URTS1
    			   //|MCF_GPIO_PUBPAR_UCTS1_UCTS1;
	
// Reset Transmitter/Receiver/Mode Register
	MCF_UART1_UCR =MCF_UART_UCR_RESET_TX
				  |MCF_UART_UCR_RESET_RX
				  |MCF_UART_UCR_RESET_MR;


//No parity, 8bit data
	MCF_UART1_UMR1=MCF_UART_UMR_PM_NONE
				  |MCF_UART_UMR_BC_8;
	 

//1bit stop
	MCF_UART1_UMR2=MCF_UART_UMR_SB_STOP_BITS_1;

//Set Rx and Tx baud by SYSTEM CLOCK
	MCF_UART1_UCSR =(0
		| MCF_UART_UCSR_RCS_SYS_CLK
		| MCF_UART_UCSR_TCS_SYS_CLK);


//Mask all UART interrupts
	MCF_UART1_UIMR =0;
	
//set baude rate
	div=sysclk/32/baudrate;
	MCF_UART1_UBG1=(unsigned char)(div>>8);
	MCF_UART1_UBG2=(unsigned char)(div&0x00ff);
	
	
//Set	UART interrupts	
	MCF_UART1_UIMR=0;//MCF_UART_UISR_FFULL_RXRDY;
				  //|MCF_UART_UISR_TXRDY;
				  //|MCF_UART_UISR_DB
				  //|MCF_UART_UISR_COS

// Enable Tx/Rx
	MCF_UART1_UCR = (0
		| MCF_UART_UCR_TX_ENABLED
		| MCF_UART_UCR_RX_ENABLED);
}

void uart1_putchar(char c)
{
    while (!(MCF_UART1_USR & MCF_UART_USR_TXRDY)) 
    {
    	
    };
    MCF_UART1_UTB =(unsigned char) c;	
}

unsigned char uart1_getchar()
{
    while (!(MCF_UART1_USR & MCF_UART_USR_RXRDY)) 
    {
    	
    };
    return MCF_UART1_URB;
}


void uart1_putstr(char *str)
{
	while(*str!=0)
	{
		uart1_putchar(*str++);
	}
}



void uart2_init(unsigned long baudrate)
{
    unsigned int div;
    uint32 sysclk=80000000;    //龙丘提示：修改主频，跟系统频率一致才行
    MCF_GPIO_PUBPAR=MCF_GPIO_PUBPAR_URTS1_UTXD2
    			   |MCF_GPIO_PUBPAR_UCTS1_URXD2;
    			   //|MCF_GPIO_PUBPAR_URTS1_URTS1
    			   //|MCF_GPIO_PUBPAR_UCTS1_UCTS1;
	
// Reset Transmitter/Receiver/Mode Register
	MCF_UART2_UCR =MCF_UART_UCR_RESET_TX
				  |MCF_UART_UCR_RESET_RX
				  |MCF_UART_UCR_RESET_MR;


//No parity, 8bit data
	MCF_UART2_UMR1=MCF_UART_UMR_PM_NONE
				  |MCF_UART_UMR_BC_8;
	 

//1bit stop
	MCF_UART2_UMR2=MCF_UART_UMR_SB_STOP_BITS_1;

//Set Rx and Tx baud by SYSTEM CLOCK
	MCF_UART2_UCSR =(0
				   |MCF_UART_UCSR_RCS_SYS_CLK
				   |MCF_UART_UCSR_TCS_SYS_CLK);


//Mask all UART interrupts
	MCF_UART2_UIMR =0;
	
//set baude rate
	div=sysclk/32/baudrate;
	MCF_UART2_UBG1=(unsigned char)(div>>8);
	MCF_UART2_UBG2=(unsigned char)(div&0x00ff);
	
	
//Set	UART interrupts	
	MCF_UART2_UIMR=0;//MCF_UART_UISR_FFULL_RXRDY;
				  //|MCF_UART_UISR_TXRDY;
				  //|MCF_UART_UISR_DB
				  //|MCF_UART_UISR_COS

// Enable Tx/Rx
	MCF_UART2_UCR =(0
				  |MCF_UART_UCR_TX_ENABLED
				  |MCF_UART_UCR_RX_ENABLED);
}

void uart2_putchar(char c)
{
    while (!(MCF_UART2_USR & MCF_UART_USR_TXRDY)) 
    {
    	
    };
    MCF_UART2_UTB =(unsigned char) c;	
}

unsigned char uart2_getchar()
{
    while (!(MCF_UART2_USR & MCF_UART_USR_RXRDY)) 
    {
    	
    };
    return MCF_UART2_URB;
}


void uart2_putstr(char *str)
{
	while(*str!=0)
	{
		uart2_putchar(*str++);
	}
}

void uart_init(uint8 channel,int32 baudrate)
{
	uint32 div,SYS_CLOCK;
	SYS_CLOCK=80000000;
	
	if(channel==0)
	{
		MCF_GPIO_PUAPAR&=0XF0;
		MCF_GPIO_PUAPAR|=0x05;	
	}
	else if(channel==1)
	{	
		MCF_GPIO_PUBPAR&=0XF0;
		MCF_GPIO_PUBPAR|=0X05;
	}
	else if(channel==2)
	{
		MCF_GPIO_PUCPAR&=0XFC;
		MCF_GPIO_PUCPAR|=0X03;	
	}
	
	//Reset Transmitter Receiver Mode Register
	MCF_UART_UCR(channel)|=(0
	|MCF_UART_UCR_RESET_TX
	|MCF_UART_UCR_RESET_RX
	|MCF_UART_UCR_RESET_MR);

	//No parity,8bit data
	MCF_UART_UMR(channel)=(0
		|MCF_UART_UMR_PM_NONE
		|MCF_UART_UMR_BC_8);
		
	//1bit stop
	MCF_UART_UMR(channel)=(0
		|MCF_UART_UMR_CM_NORMAL
		|MCF_UART_UMR_SB_STOP_BITS_1);
		
	//Set Rx and Tx buad by SYSTERM CLOCK
	MCF_UART_UCSR(channel)=(0
		|MCF_UART_UCSR_RCS_SYS_CLK
		|MCF_UART_UCSR_TCS_SYS_CLK);
	
	//Mask all UART interrupts	
	MCF_UART_UIMR(channel)=0;
	
	//set buad rate
	div=(SYS_CLOCK/32/baudrate);
	
	MCF_UART_UBG1(channel)=(unsigned char)(div>>8);
	MCF_UART_UBG2(channel)=(unsigned char)(div&0x00ff);
	
	//Enable Tx/Rx
	MCF_UART_UCR(channel)=(0
		|MCF_UART_UCR_TX_ENABLED
		|MCF_UART_UCR_RX_ENABLED);
}

void uart_putchar(uint8 channel,unsigned char c)
{
    while (!(MCF_UART_USR(channel) & MCF_UART_USR_TXRDY)) 
    {
    	
    };
    MCF_UART_UTB(channel) = c;	
}

void uart_putstr(uint8 channel,char *str)
{
	while(*str!=0)
	{
		while(!(MCF_UART_USR(channel)&MCF_UART_USR_TXRDY))
		{
		
		};
			MCF_UART_UTB(channel)=(unsigned char)*str++;
	}
}
unsigned char uart_getchar(uint8 channel)
{
    while (!(MCF_UART_USR(channel) & MCF_UART_USR_RXRDY)) 
    {
    	
    };
    return MCF_UART_URB(channel);	
}


void uart0_printf(char* str,uint32 data)
{
	uint8  result[12];
	result[10]='\n';
	result[11]=0;
	uart0_putstr(str);
	result[0]=(uint8)(data/1000000000+48);
	result[1]=(uint8)((data%1000000000)/100000000+48);
	result[2]=(uint8)((data%100000000)/10000000+48);
	result[3]=(uint8)((data%10000000)/1000000+48);
	result[4]=(uint8)((data%1000000)/100000+48);
	result[5]=(uint8)((data%100000)/10000+48);
	result[6]=(uint8)((data%10000)/1000+48);
	result[7]=(uint8)((data%1000)/100+48);
	result[8]=(uint8)((data%100)/10+48);
	result[9]=(uint8)(data%10+48);
	uart0_putstr((char*)result);
}

void uart0_printfs(char *str,uint8 data[] ,uint32 num)
{
	uint8  result[5];
	uint32 i;
	result[3]=',';
	result[4]=0;
	uart0_putstr(str);
	for(i=0;i<num;i++)
	{
		result[0]=(uint8)(data[i]/100+48);
		result[1]=(uint8)((data[i]%100)/10+48);
		result[2]=(uint8)(data[i]%10+48);
		uart0_putstr((char*)result);
	}
	uart0_putchar('\n');
}


//init as UART0 receive
void DMA_init(void)
{
	char *decadd=receive;
	
	MCF_SCM_DMAREQC=MCF_SCM_DMAREQC_DMAC0(0x8);//UART0 receive
	
	//source address register
	MCF_DMA0_SAR= 0x4000020C;	    
	//destination address register
	MCF_DMA_DAR(0)=(uint32)decadd;	           
	//byte count
	MCF_DMA0_BCR=30;
	MCF_DMA0_DCR=MCF_DMA_DCR_INT   //interrupt enable
				  |MCF_DMA_DCR_EEXT  //enable external request
				  |MCF_DMA_DCR_CS	 //force a single read/write transfer per request
				  |MCF_DMA_DCR_SSIZE(MCF_DMA_DCR_SSIZE_BYTE)
				  |MCF_DMA_DCR_DINC
				  |MCF_DMA_DCR_DSIZE(MCF_DMA_DCR_DSIZE_BYTE)
				  |MCF_DMA_DCR_D_REQ;
				  //|MCF_DMA_DCR_AA   //auto aligne
				  //|MCF_DMA_DCR_SINC
				  //|MCF_DMA_DCR_LCH1_CH3
				  //|MCF_DMA_DCR_LCH2_CH2;
	
	//UART0 read
	MCF_SCM_PACR2=MCF_SCM_PACR_ACCESS_CTRL1(5);
	//read/write
//	MCF_SCM_GPACR0=MCF_SCM_GPACR_ACCESS_CTRL(6);
	
//interrupt enable	
	MCF_INTC0_IMRL&=~MCF_INTC_IMRL_MASKALL;
	MCF_INTC0_IMRL&=~MCF_INTC_IMRL_INT_MASK9;//channel 0
	MCF_INTC0_ICR09=MCF_INTC_ICR_IP(3)+MCF_INTC_ICR_IL(2);		
}

__declspec(interrupt:0) void DMA0_handler(void)//9
{
	uint8 int_status = MCF_DMA0_DSR;
	if(int_status & MCF_DMA_DSR_DONE)
	{
		if(int_status & MCF_DMA_DSR_CE)
		{
			uart0_putstr("configuration error\n");
		}
		else if(int_status & MCF_DMA_DSR_BED)
		{
			uart0_putstr("destination bus error\n");
		}
		else if(int_status & MCF_DMA_DSR_BES)
		{
			uart0_putstr("source bus error\n");
		}
		else
		{	
			uart0_putstr("dma0 transfer done\n");
			uart0_putstr(receive);
		}
		//clear DMA0 interrupt
	//	MCF_DMA0_DCR&=~MCF_DMA_DCR_EEXT;
		uart0_putstr("DMA\n");
		MCF_DMA0_DSR |= MCF_DMA_DSR_DONE;
		MCF_DMA_DAR(0)=(uint32)receive;
		MCF_DMA0_BCR=30;	
		MCF_DMA0_DCR|=MCF_DMA_DCR_EEXT;
	}
	if(int_status & MCF_DMA_DSR_BSY)
	{
		uart0_putstr("busy\n");
	}
	if(int_status & MCF_DMA_DSR_REQ)
	{
		uart0_putstr("transfer remaining but channel not selected\n");
	}
}



__declspec(interrupt:0) void UART0_interrupt(void)//13
{
	static uint8 i=0;
	static char rec_temp,rec_buffer[20];
	if(MCF_UART0_USR&MCF_UART_USR_RXRDY)
	{
		rec_temp=(char)MCF_UART0_URB;
		if((rec_temp!='\n')&&(i<20))
		{
			rec_buffer[i]=rec_temp;
			i++;
			rec_buffer[i]=0;
			
		}
		else
		{	
			uart0_putstr(rec_buffer);
			uart0_putchar('\n');
			i=0;
			status=over;
		}
	}
	else uart0_putstr("error");
	
}
__declspec(interrupt:0) void UART1_interrupt(void)//14
{
	
}
__declspec(interrupt:0) void UART2_interrupt(void)//15
{
	
}
