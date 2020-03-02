/*
 * File:		dma_example.c
 * Purpose:		DMA Examples
 * 
 * License:     All software covered by license agreement in -
 *              docs/Freescale_Software_License.pdf
 */


#include "common.h"
#include "dma.h"

#define WAIT_4_USER_INPUT()    in_char()

static DMA_Info info;
const uint16 maxcnt = 0x0c00;
extern char __HEAP_START[];
extern char __HEAP_END[];

int8 sram_dma_test(void)
{
	uint8 u8Char;
	uint8 j;	
	uint16 count = 0;
	char *p, *q;
	uint32 reg = 0;
	
	printf("\t***********************\t\n");
	printf("DMA sram data movement Test\n");
	printf("Press a key when ready\n");
	
	WAIT_4_USER_INPUT();

	printf("__HEAP_START is %p, __HEAP_END is %p\n", __HEAP_START, __HEAP_END);

	info.srcadd = (uint32)__HEAP_START;//&(~0x0f);
//	info.srcadd += 0x10;
	info.destadd = (uint32)(info.srcadd + maxcnt);
	info.bytecnt = maxcnt;
	p = (char *)info.srcadd;
	q = (char *)info.destadd;
	
	if(info.destadd > (uint32)__HEAP_END)
	{
		printf("Too much data to transfer, exceed Heap limit\n");
		return;
	}
	printf("DMA source address is %p, dest address is %p\n", info.srcadd, info.destadd);

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

	/*
	* DMA control, source and destination increment,
	* source and destination size is 16byte burst, auto align
	* enable internal interrupt signal
	*/	
	//note: if MCF_DMA_DCR_AA enabled, DINC and SINC will increase no matter what value they have
	info.ctrl = MCF_DMA_DCR_INT
			 | MCF_DMA_DCR_SSIZE_BYTE
			 | MCF_DMA_DCR_DSIZE_BYTE
			 | MCF_DMA_DCR_DINC
			 | MCF_DMA_DCR_SINC
		//	 | MCF_DMA_DCR_AA
			 	;
	
	/* Use both DMAs */
	for (j=0; j < 4; j++)
	{	
		reg = MCF_DMA_DCR(0);	
		DMA_init(j, &info);
		printf("DMA #%d\t", j);
		printf ("Start DMA Transfer OK?(y/n)");
		u8Char = WAIT_4_USER_INPUT();
		info.ctrl |= MCF_DMA_DCR_START;		
		DMA_start(j, &info);

		/* wait until DMA transfer done */
		while(0x00 == (int_status&MCF_DMA_DSR_DONE))
			;	
		printf("DMA status is 0x%x\n", int_status);
		
		info.ctrl &= ~MCF_DMA_DCR_START;
		int_status = 0;				

		printf("Start checking data...\n");
		//Check data coherence
		for(count = 0; count < maxcnt; count++)
		{
			//printf("0x%x, 0x%x\n", __HEAP_START[count], __HEAP_START[count+maxcnt]);
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
		
		printf("DMA Transfer OK? Continue?(y/n)");
		u8Char = WAIT_4_USER_INPUT();
		printf("\n");
		if ((u8Char != 'Y') && (u8Char != 'y'))
		{
			return -1;
		}
	}
	return 0;
}

int8 smod_dmod_test(void)
{
	uint8 u8Char;
	uint8 j;	
	uint16 count = 0;
	char *p, *q;
	 uint8 i;
	 
	printf("\t***********************\t\n");
	printf("DMA sram smod and dmod test\n");
	printf("Press a key when ready\n");
	
	WAIT_4_USER_INPUT();

	printf("__HEAP_START is %p, __HEAP_END is %p\n", __HEAP_START, __HEAP_END);

	info.srcadd = (uint32)__HEAP_START&(~0x0f);
	info.srcadd += 0x10;
	info.destadd = (uint32)(info.srcadd + maxcnt);
	info.bytecnt = maxcnt;
	p = (char *)info.srcadd;
	q = (char *)info.destadd;
	
	if(info.destadd > (uint32)__HEAP_END)
	{
		printf("Too much data to transfer, exceed Heap limit\n");
		return;
	}
	printf("DMA source address is %p, dest address is %p\n", info.srcadd, info.destadd);

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
		
	/* Use both DMAs */
	for (j=0; j < 4; j++)
	{	
		//src FIFO
		info.ctrl = MCF_DMA_DCR_INT
			 | MCF_DMA_DCR_SSIZE_WORD
			 | MCF_DMA_DCR_DSIZE_WORD
			 | MCF_DMA_DCR_DINC
			 | MCF_DMA_DCR_SINC	
			// | MCF_DMA_DCR_DMOD(0x1)
			 | MCF_DMA_DCR_SMOD(0x1)
			// | MCF_DMA_DCR_AA
			 	;		
		DMA_init(j, &info);
		printf("DMA #%d\t", j);
		printf ("Start DMA Transfer OK?(y/n)");
		u8Char = WAIT_4_USER_INPUT();
		info.ctrl |= MCF_DMA_DCR_START;
		DMA_start(j, &info);

		/* wait until DMA transfer done */
		while(0x00 == (int_status&MCF_DMA_DSR_DONE))
			;	
		//printf("MCF_DMA_SAR is 0x%x\n",MCF_DMA_SAR(j));
		//printf("MCF_DMA_DAR is 0x%x\n",MCF_DMA_DAR(j));
				
		printf("DMA status is 0x%x\n", int_status);
		int_status = 0;
		printf("Start checking data...\n");
		//Check data coherence
		for(i = 0, count = 0; count < maxcnt; count++)
		{
			if(p[i] != p[count+maxcnt])
			{
				printf("DMA transfer failed, data not coherent\n");
				printf("%p, %p, 0x%x, 0x%x\n", p+i, p+i+16, p[count], p[count+maxcnt]);
				break;
			}
			else
				p[count+maxcnt] = 0;	//clear destination data
			i++;
			if(i == 16) i = 0;
		}
		
		if(count >= maxcnt) 
			printf("DMA smod test ok, data correct\n");		
		
		//dest FIFO
		info.ctrl = MCF_DMA_DCR_INT
			 | MCF_DMA_DCR_SSIZE_WORD
			 | MCF_DMA_DCR_DSIZE_WORD
			 | MCF_DMA_DCR_DINC
			 | MCF_DMA_DCR_SINC	
			 | MCF_DMA_DCR_DMOD(0x1)
			// | MCF_DMA_DCR_SMOD(0x1)
			// | MCF_DMA_DCR_AA
			 	;		
		DMA_init(j, &info);
		printf("DMA #%d\t", j);
		printf ("Start DMA Transfer OK?(y/n)");
		u8Char = WAIT_4_USER_INPUT();
		info.ctrl |= MCF_DMA_DCR_START;
		DMA_start(j, &info);

		/* wait until DMA transfer done */
		while(0x00 == (int_status&MCF_DMA_DSR_DONE))
			;	
		//printf("MCF_DMA_SAR is 0x%x\n",MCF_DMA_SAR(j));
		//printf("MCF_DMA_DAR is 0x%x\n",MCF_DMA_DAR(j));
				
		printf("DMA status is 0x%x\n", int_status);
		int_status = 0;
		printf("Start checking data...\n");
		//Check data coherence
		for(count = maxcnt -16; count < maxcnt; count++)
		{
			if(p[count] != p[count+16])
			{
				printf("DMA transfer failed, data not coherent\n");
				printf("%p, %p, 0x%x, 0x%x\n", p+count, p+count+16, p[count], p[count+16]);
				break;
			}
			else
				p[count+maxcnt] = 0;	//clear destination data
		}
		
		if(count >= maxcnt) 
			printf("DMA smod test ok, data correct\n");		
		
		printf("DMA Transfer OK? Continue?(y/n)");
		u8Char = WAIT_4_USER_INPUT();
		printf("\n");
		if ((u8Char != 'Y') && (u8Char != 'y'))
		{
			return -1;
		}
	}
	return 0;
}

int8 dma_extreq(void)
{
	uint8 u8Char;
	char trs_buf[] = "Helloworld12345Helloworld12345Helloworld12345Helloworld12345678";
	char rev_buf[32]; 	 
	DMA_Info dma0_info,dma1_info;

	printf("\t***********************\t\n");
	printf("DMA external UART request Test\n");
	printf("Press a key when ready\n");
	
	WAIT_4_USER_INPUT();

	//UART0 transmit trigger DMA0, UART0 receive trigger DMA1
	MCF_SCM_DMAREQC = MCF_SCM_DMAREQC_DMAC0(0xc)| MCF_SCM_DMAREQC_DMAC1(0x8);

	/* DMA0 setting */
	//source address register
	dma0_info.srcadd=(uint32)&trs_buf[0];	 
	printf("MCF_DMA0_SAR is 0x%x\n",dma0_info.srcadd);
	//destination address register
	dma0_info.destadd= (uint32)&(MCF_UART_UTB(0));	 	
	printf("MCF_DMA0_DAR is 0x%x\n",dma0_info.destadd);	
	//byte count
	dma0_info.bytecnt= sizeof(trs_buf);
	printf("MCF_DMA0_BCR is 0x%x\n",dma0_info.bytecnt);
	/*
	* DMA control, source and destination increment,
	* source and destination size is one byte, auto align, cycle steal
	* enable internal interrupt signal, deasserts EEXT when BCR exhausts
	*/
	dma0_info.ctrl = MCF_DMA_DCR_INT
			 | MCF_DMA_DCR_SSIZE_BYTE
			 | MCF_DMA_DCR_DSIZE_BYTE
			 | MCF_DMA_DCR_SINC		
			 | MCF_DMA_DCR_D_REQ
			 | MCF_DMA_DCR_CS;	
	printf("MCF_DMA0_DCR is 0x%x\n",dma0_info.ctrl);
	DMA_init(0, &dma0_info);

	/* DMA1 setting */
	//source address register
	dma1_info.srcadd = (uint32)&(MCF_UART_URB(0));		
	printf("MCF_DMA1_SAR is 0x%x\n",dma1_info.srcadd);
	//destination address register
	dma1_info.destadd = (uint32)&rev_buf[0];
	printf("MCF_DMA1_DAR is 0x%x\n",dma1_info.destadd);
	//byte count
	dma1_info.bytecnt = 32;
	printf("MCF_DMA1_BCR is 0x%x\n",dma1_info.bytecnt);
	/*
	* DMA control, source and destination increment,
	* source and destination size is one byte, auto align, cycle steal
	* enable internal interrupt signal, deasserts EEXT when BCR exhausts
	*/
	dma1_info.ctrl = MCF_DMA_DCR_INT
			 | MCF_DMA_DCR_SSIZE_BYTE
			 | MCF_DMA_DCR_DSIZE_BYTE
			 | MCF_DMA_DCR_DINC
			 | MCF_DMA_DCR_SINC			 
			 | MCF_DMA_DCR_D_REQ
			 | MCF_DMA_DCR_CS;
	printf("MCF_DMA1_DCR is 0x%x\n",dma1_info.ctrl);
	DMA_init(1, &dma1_info);
		
	printf ("Start DMA Transfer OK? (y/n)\n");
	u8Char = WAIT_4_USER_INPUT();
	printf("Type a character into the terminal\n");
		
	//trigger external DMA
	dma0_info.ctrl |= MCF_DMA_DCR_EEXT;
	DMA_start(0,&dma0_info);
		
	//wait until DMA transfer complete
	while(MCF_DMA0_DCR&MCF_DMA_DCR_EEXT)
		;	
			
	dma1_info.ctrl |= MCF_DMA_DCR_EEXT;	
	DMA_start(1,&dma1_info);
	//wait until DMA transfer complete
	while(MCF_DMA1_DCR&MCF_DMA_DCR_EEXT)
		;
	for(u8Char = 0; u8Char < 32; u8Char++)	
		printf("%c", rev_buf[u8Char]);
	printf("DMA Transfer OK? Continue?(y/n)");
	u8Char = WAIT_4_USER_INPUT();
	printf("\n");
	if ((u8Char != 'Y') && (u8Char != 'y'))
	{
		return -1;
	}

	return 0;
}

int8 channel_link(void)
{
	uint8 u8Char;
	uint8 j;	
	uint16 count = 0;
	char *src0, *dest0;
	char *src1, *dest1;	 
	DMA_Info dma0_info, dma1_info;
	 	 
	printf("\t***********************\t\n");
	printf("Channel link Test\n");
	printf("Press a key when ready\n");
	
	WAIT_4_USER_INPUT();

	/* DMA channel 0 src and dest data initialization */
	dma0_info.srcadd = (uint32)__HEAP_START;
	dma0_info.destadd = (uint32)(dma0_info.srcadd + maxcnt);
	dma0_info.bytecnt = maxcnt;
	src0 = (char *)dma0_info.srcadd;
	dest0 = (char *)dma0_info.destadd;
	
	if(dma0_info.destadd > (uint32)__HEAP_END)
	{
		printf("Too much data to transfer, exceed Heap limit\n");
		return;
	}
	printf("DMA source address is %p, dest address is %p\n", dma0_info.srcadd, dma0_info.destadd);

	//Initialize src data block
	for(count = 0; count < maxcnt; count++)
	{
		src0[count] = count;
	}
	//clear dest data clock
	for(count = 0; count < maxcnt; count++)
	{
		dest0[count] = (char)0x0;
	}
	
	/* DMA channel 1 src and dest data initialization */
	dma1_info.srcadd = (uint32)(dma0_info.destadd + maxcnt);
	dma1_info.destadd = (uint32)(dma1_info.srcadd + maxcnt);
	dma1_info.bytecnt = maxcnt;
	src1 = (char *)dma1_info.srcadd;
	dest1 = (char *)dma1_info.destadd;
	
	if(dma1_info.destadd > (uint32)__HEAP_END)
	{
		printf("Too much data to transfer, exceed Heap limit\n");
		return;
	}
	printf("DMA source address is %p, dest address is %p\n", dma1_info.srcadd, dma1_info.destadd);

	//Initialize src data block
	for(count = 0; count < maxcnt; count++)
	{
		src1[count] = count+0x10;
	}
	//clear dest data clock
	for(count = 0; count < maxcnt; count++)
	{
		dest1[count] = (char)0x0;
	}
	
	/*
	* DMA0 control, source and destination increment,
	* source and destination size is 16byte burst, auto align
	* enable internal interrupt signal
	*/	
	//note: if MCF_DMA_DCR_AA enabled, DINC and SINC will increase no matter what value they have
	dma0_info.ctrl = MCF_DMA_DCR_INT
			 | MCF_DMA_DCR_SSIZE_BYTE
			 | MCF_DMA_DCR_DSIZE_BYTE
			 | MCF_DMA_DCR_DINC
			 | MCF_DMA_DCR_SINC
			 | MCF_DMA_DCR_LINKCC(0x03)
			 | MCF_DMA_DCR_LCH1(0x01)
			 | MCF_DMA_DCR_LCH2(0x02)
				;
	DMA_init(0, &dma0_info);
	
	/*
	* DMA1 control, source and destination increment,
	* source and destination size is 16byte burst, auto align
	* enable internal interrupt signal
	*/	
	//note: if MCF_DMA_DCR_AA enabled, DINC and SINC will increase no matter what value they have
	dma1_info.ctrl = MCF_DMA_DCR_INT
			 | MCF_DMA_DCR_SSIZE_BYTE
			 | MCF_DMA_DCR_DSIZE_BYTE
			 | MCF_DMA_DCR_DINC
			 | MCF_DMA_DCR_SINC;
	DMA_init(1, &dma1_info);
		
	printf ("Start DMA Transfer OK?(y/n)");
	u8Char = WAIT_4_USER_INPUT();
	dma0_info.ctrl |= MCF_DMA_DCR_START;
	DMA_start(0, &dma0_info);

	/* wait until DMA transfer done */
	while(0x00 == (int_status&MCF_DMA_DSR_DONE))
		;	
	int_status = 0;
	
	printf("Start checking data...\n");
	
	//Check data coherence
	for(count = 0; count < maxcnt; count++)
	{
		//printf("0x%x, 0x%x\n", __HEAP_START[count], __HEAP_START[count+maxcnt]);
		if(src0[count] != src0[count+maxcnt])
		{
			printf("DMA transfer failed, data not coherent\n");
			printf("%p, %p, 0x%x, 0x%x\n", src0+count, src0+count+maxcnt, src0[count], src0[count+maxcnt]);
			break;
		}
		else
			src0[count+maxcnt] = 0;	//clear destination data
	}
	
	//Check data coherence
	for(count = 0; count < maxcnt; count++)
	{
		//printf("0x%x, 0x%x\n", __HEAP_START[count], __HEAP_START[count+maxcnt]);
		if(src1[count] != src1[count+maxcnt])
		{
			printf("DMA transfer failed, data not coherent\n");
			printf("%p, %p, 0x%x, 0x%x\n", src1+count, src1+count+maxcnt, src1[count], src1[count+maxcnt]);
			break;
		}
		else
			src1[count+maxcnt] = 0;	//clear destination data
	}
			
	if(count >= maxcnt) 
		printf("DMA transfer ok, data correct\n");		
	
	printf("DMA Transfer OK? Continue?(y/n)");
	u8Char = WAIT_4_USER_INPUT();
	printf("\n");
	if ((u8Char != 'Y') && (u8Char != 'y'))
	{
		return -1;
	}
	
	return 0;
}


