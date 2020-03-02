/*
 * File:		dtim_example.c
 * Purpose:		DMA Timer Examples
 * 
 * License:     All software covered by license agreement in -
 *              docs/Freescale_Software_License.pdf
 */

#include "common.h"
#include "uart.h"
#include "dtim.h"
#include "dma.h"
#include "dtim_example.h"

#define WAIT_4_USER_INPUT()    in_char()

extern char __HEAP_START[];
extern char __HEAP_END[];

static void example_idle_state(void);
static uint32 dtim_setperiod (uint32 u32Periodus, uint8 u8PITModule);

static DTIM_Info info;

static void LED_Toggle(void){ 
	MCF_GPIO_PORTTC ^= 0x08;		//toggle TC3
}

static void Dummy(void){
	
}
/********************************************************************/
/* 
 * Set DMA Timer Period in microseconds
 */
static uint32 dtim_setperiod (uint32 u32Periodus, uint8 u8PITModule) 
{
	uint8 i;
	uint32 u32Modulus;
    
	for (i = 0; i < 256; i++){
		u32Modulus = (SYS_CLK_MHZ * u32Periodus)/(i+1)-1;
		if(info.u8ClkSrc == 0x2)
			u32Modulus /= 16;
		if (u32Modulus < 65535){
			/* Try to find a good prescaler */
			MCF_DTIM_DTRR(u8PITModule) = u32Modulus;
			MCF_DTIM_DTMR(u8PITModule) |= MCF_DTIM_DTMR_PS(i);
			info.u8Prescaler = i;	//clk divide, 0x0(/1), 0x1(/2), ... 0xff(/256)			
			info.u32Reference = u32Modulus;
			if(info.u8ClkSrc == 0x1)
				return ((i+1)*(u32Modulus+1)/SYS_CLK_MHZ);
			else if(info.u8ClkSrc == 0x2)
				return ((i+1)*(u32Modulus+1)*16/SYS_CLK_MHZ);
		}
	}
	return -1;  
}

/* Basic DMA Timer Test for reference reach with external clk */
int8 dtim_extclktest(void)
{
	uint8 u8Char;
	
	printf("\t***********************\t\n");
	printf("DTimer basic Test for reference reach\n");
	printf("Connect DTIN2 to O-Scope and press a key when ready\n");

	WAIT_4_USER_INPUT();
	
	MCF_GPIO_PTCPAR = 0x25; 	//DTIN0~1, DTOUT2, TC3 GPIO
	MCF_GPIO_PORTTC = 0x00;
	MCF_GPIO_DDRTC = 0x08;		//TC3 output
	
	info.u8ClkSrc = 0x1;	//u8ClkSrc = 1, sysclk; u8ClkSrc = 2, sysclk/16; u8ClkSrc = 3, DTIN
	info.u8CapEdge = 0;		//u8CapEdge = 1, rising; u8CapEdge = 2, falling; u8CapEdge = 3, both
	info.u8DMAEn = 0;		//enable DMA request
	info.u8Halt = 0;		//timer stop in halt
	info.u8Mode16 = 0;		
	info.u8Restart = 1;		//u8Restart = 0, freerun; u8Restart = 1, restart
	info.u8OutputMode = 1;	//u8OutputMode = 0, active low; u8OutputMode = 1, toggle

	/* dtim2 */
	dtim_init(info,2);	
	dtim_setperiod(100,2);	//5kHz output
	DTimEnableInts(2, Dummy);
	dtim_start(2);	

	/* dtim0 */
	info.u8ClkSrc = 0x3;	//u8ClkSrc = 1, sysclk; u8ClkSrc = 2, sysclk/16; u8ClkSrc = 3, DTIN
	dtim_init(info,0);
	//set period to 
	MCF_DTIM_DTRR(0) = 100;
	MCF_DTIM_DTMR(0) |= MCF_DTIM_DTMR_PS(0);
	DTimEnableInts(0, LED_Toggle);
	dtim_start(0);				
		
	printf ("DMA Timer functioning?(y/n)\n");
	u8Char = WAIT_4_USER_INPUT();
	printf("\n");
	DTimDisableInts(0);
	DTimDisableInts(2);
	if ((u8Char != 'Y') && (u8Char != 'y'))
	{
		return -1;
	}
    
	return 0;
}

/* Basic DMA Timer Test with reference reach */
int8 dtim_reftest(void)
{
	uint8 u8Char;
	uint8 j;
	uint32 counter = 0;
	
	printf("\t***********************\t\n");
	printf("DTimer basic Test for reference reach\n");
	printf("Connect DTIN0~DTIN3 to O-Scope and press a key when ready\n");
	
	WAIT_4_USER_INPUT();
	
	MCF_GPIO_PTCPAR = 0xaa; 	//DTOUT0~DTOUT3
	
	info.u8ClkSrc = 0x1;	//u8ClkSrc = 1, sysclk; u8ClkSrc = 2, sysclk/16; u8ClkSrc = 3, DTIN
	info.u8CapEdge = 0;		//u8CapEdge = 1, rising; u8CapEdge = 2, falling; u8CapEdge = 3, both
	info.u8DMAEn = 0;		//enable DMA request
	info.u8Halt = 1;		//timer stop in halt
	info.u8Mode16 = 0;		
	info.u8Restart = 0;		//u8Restart = 0, freerun; u8Restart = 1, restart
	info.u8OutputMode = 1;	//u8OutputMode = 0, active low; u8OutputMode = 1, toggle
	
	/* Use both PWMs */
	for (j = 0; j < 4; j++)
	{
		dtim_init(info, j);
		printf("DMA Timer #%d\t", j);
		dtim_setperiod(100, j);
		DTimEnableInts(j, Dummy);
		//MCF_DTIM_DTRR(j) = 0;	//minimum setting
		//MCF_DTIM_DTRR(j)= 0xffffffff; //maximum setting
		//MCF_DTIM_DTMR(j) |= MCF_DTIM_DTMR_PS(0);
		dtim_start(j);			
		
		printf("MCF_DTIM_DTXMRis 0x%x\n", MCF_DTIM_DTXMR(j));
		while(1)
		{
			counter = MCF_DTIM_DTCN(j);
			printf("0x%x\n", counter);
		}
			
		printf ("DMA Timer functioning?(y/n)\n");
		u8Char = WAIT_4_USER_INPUT();
		printf("\n");
		DTimDisableInts(j);
		if ((u8Char != 'Y') && (u8Char != 'y'))
		{
			return -1;
		}
	}
	
	return 0;
}

/* Basic DMA Timer Test with capture */
int8 dtim_captest(void)
{
	uint8 u8Char;
	uint8 j;
	
	printf("\t***********************\t\n");
	printf("DTimer basic Test for capture\n");
	printf("Connect DTIN0~DTIN3 to O-Scope and press a key when ready\n");

	WAIT_4_USER_INPUT();
    
	MCF_GPIO_PTCPAR = 0x25; 	//DTIN0~1, DTOUT2, TC3 GPIO
	MCF_GPIO_DDRTC = 0x08; //TC3 output
	
	info.u8ClkSrc = 0x2;	//u8ClkSrc = 1, sysclk; u8ClkSrc = 2, sysclk/16; u8ClkSrc = 3, DTIN
	info.u8CapEdge = 0;		//u8CapEdge = 1, rising; u8CapEdge = 2, falling; u8CapEdge = 3, both
	info.u8DMAEn = 0;		//enable DMA request
	info.u8Halt = 0;		//timer stop in halt
	info.u8Mode16 = 0;		
	info.u8Restart = 1;		//u8Restart = 0, freerun; u8Restart = 1, restart
	info.u8OutputMode = 1;	//u8OutputMode = 0, active low; u8OutputMode = 1, toggle
	
	/* dtim2 */
	dtim_init(info,2);	
	dtim_setperiod(100,2);	//5kHz output
	DTimEnableInts(2, Dummy);
	dtim_start(2);	
	
	/* dtim0 */
	info.u8CapEdge = 1;
	dtim_init(info,0);
	MCF_DTIM_DTMR(0) |= MCF_DTIM_DTMR_PS(0);	//inclk = 5MHz
	DTimEnableInts(0, LED_Toggle);	
	dtim_start(0);	
	
	for(j = 0; j < 20; j++)
		printf("diff is %d\n", diff);		//diff should be 1000

	printf ("DMA Timer functioning?(y/n)\n");
	u8Char = WAIT_4_USER_INPUT();		
	DTimDisableInts(0);
	DTimDisableInts(2);
	
	printf("\n");
	if ((u8Char != 'Y') && (u8Char != 'y'))
	{
		return -1;
	}

	return 0;
}


/* Basic DMA Timer DMA trigger test */
int8 dtim_dmatest(void)
{
	uint8 u8Char;
	char *p, *q;
	uint16 count = 0;
	const uint16 maxcnt = 0x400;
	
	printf("\t***********************\t\n");
	printf("DTimer basic Test for reference reach\n");
	printf("Connect DTIN0~DTIN3 to O-Scope and press a key when ready\n");

    WAIT_4_USER_INPUT();

	p = (char *)__HEAP_START;
	q = (char *)__HEAP_START + maxcnt;
	
	if((uint32)q > (uint32)__HEAP_END)
	{
		printf("Too much data to transfer, exceed Heap limit\n");
		return;
	}
	printf("DMA source address is %p, dest address is %p\n", p, q);

	//Initialize src data block
	for(count = 0; count < maxcnt; count++)
	{
		p[count] = count;
	}
	//clear dest data clock
	for(count = 0; count < maxcnt; count++)
	{
		q[count] = (char)0x0;
	}
	
	MCF_GPIO_PTCPAR = 0xaa; 	//DTOUT0~DTOUT3

	/* Enable DMA master access in both supervisor and user mode */
	MCF_SCM_MPR = MCF_SCM_MPR_MPR(0x05);
	//dtim0 trigger DMA0
	MCF_SCM_DMAREQC = MCF_SCM_DMAREQC_DMAC0(0x4);
	
	MCF_DMA_SAR(0) = (uint32)p;	    
	MCF_DMA_DAR(0) = (uint32)q;	           
	MCF_DMA_BCR(0) = maxcnt;
	MCF_DMA_DCR(0) = MCF_DMA_DCR_INT
			 | MCF_DMA_DCR_SSIZE_BYTE
			 | MCF_DMA_DCR_DSIZE_BYTE
			 | MCF_DMA_DCR_DINC
			 | MCF_DMA_DCR_SINC
			 | MCF_DMA_DCR_D_REQ
			 | MCF_DMA_DCR_EEXT;	//external trigger	
	
	info.u8ClkSrc = 0x1;	//u8ClkSrc = 1, sysclk; u8ClkSrc = 2, sysclk/16; u8ClkSrc = 3, DTIN
	info.u8CapEdge = 0;		//u8CapEdge = 1, rising; u8CapEdge = 2, falling; u8CapEdge = 3, both
	info.u8DMAEn = 1;		//enable DMA request
	info.u8Halt = 0;		//timer stop in halt
	info.u8Mode16 = 0;		
	info.u8Restart = 1;		//u8Restart = 0, freerun; u8Restart = 1, restart
	info.u8OutputMode = 1;	//u8OutputMode = 0, active low; u8OutputMode = 1, toggle
	
    dtim_init(info,0);
    printf("DMA Timer #%d\t", 0);
	dtim_setperiod(100,0);
	DTimEnableInts(0, LED_Toggle);
    dtim_start(0);
	while(0x0 == (int_status&MCF_DMA_DSR_DONE))
		printf("MCF_DTIM_DTCN is 0x%x\n",MCF_DTIM_DTCN(0));

	//Check data coherence
	for(count = 0; count < maxcnt; count++)
	{
		if(p[count] != p[count+maxcnt])
		{
			printf("DMA transfer failed, data not coherent\n");
			printf("%p, %p, 0x%x, 0x%x\n", p+count, p+count+maxcnt, p[count], p[count+maxcnt]);
			break;
		}
		else
			p[count+maxcnt] = 0;	//clear destination data
	}
			
	if(count >= maxcnt) 
		printf("DMA transfer ok, data correct\n");		
		
    printf ("DMA Timer functioning?(y/n)\n");
    u8Char = WAIT_4_USER_INPUT();
    printf("\n");
	DTimDisableInts(0);
    if ((u8Char != 'Y') && (u8Char != 'y'))
    {
       return -1;
    }

    return 0;
}


