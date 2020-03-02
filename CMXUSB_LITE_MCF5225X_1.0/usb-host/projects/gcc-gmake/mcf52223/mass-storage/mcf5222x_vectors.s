/******************************************************************************
 This file contains the vector table and the startup code executed out of
 reset.
 *****************************************************************************/
	.extern __IPSBAR
	.extern __SRAM
    .extern __FLASH
	.extern __SP_INIT

    .extern irq_handler02
    .extern irq_handler03
    .extern irq_handler04
    .extern irq_handler05
    .extern irq_handler06
    .extern irq_handler07
    .extern irq_handler08
    .extern irq_handler09
    .extern irq_handler0a
    .extern irq_handler0b
    .extern irq_handler0c
    .extern irq_handler0d
    .extern irq_handler0e
    .extern irq_handler0f
    .extern irq_handler10
    .extern irq_handler11
    .extern irq_handler12
    .extern irq_handler13
    .extern irq_handler14
    .extern irq_handler15
    .extern irq_handler16
    .extern irq_handler17
    .extern irq_handler18
    .extern irq_handler19
    .extern irq_handler1a
    .extern irq_handler1b
    .extern irq_handler1c
    .extern irq_handler1d
    .extern irq_handler1e
    .extern irq_handler1f
    .extern irq_handler20
    .extern irq_handler21
    .extern irq_handler22
    .extern irq_handler23
    .extern irq_handler24
    .extern irq_handler25
    .extern irq_handler26
    .extern irq_handler27
    .extern irq_handler28
    .extern irq_handler29
    .extern irq_handler2a
    .extern irq_handler2b
    .extern irq_handler2c
    .extern irq_handler2d
    .extern irq_handler2e
    .extern irq_handler2f
    .extern irq_handler30
    .extern irq_handler31
    .extern irq_handler32
    .extern irq_handler33
    .extern irq_handler34
    .extern irq_handler35
    .extern irq_handler36
    .extern irq_handler37
    .extern irq_handler38
    .extern irq_handler39
    .extern irq_handler3a
    .extern irq_handler3b
    .extern irq_handler3c
    .extern irq_handler3d
    .extern irq_handler3e
    .extern irq_handler3f
    .extern irq_handler40
    .extern irq_handler41
    .extern irq_handler42
    .extern irq_handler43
    .extern irq_handler44
    .extern irq_handler45
    .extern irq_handler46
    .extern irq_handler47
    .extern irq_handler48
    .extern irq_handler49
    .extern irq_handler4a
    .extern irq_handler4b
    .extern irq_handler4c
    .extern irq_handler4d
    .extern irq_handler4e
    .extern irq_handler4f
    .extern irq_handler50
    .extern irq_handler51
    .extern irq_handler52
    .extern irq_handler53
    .extern irq_handler54
    .extern irq_handler55
    .extern irq_handler56
    .extern irq_handler57
    .extern irq_handler58
    .extern irq_handler59
    .extern irq_handler5a
    .extern irq_handler5b
    .extern irq_handler5c
    .extern irq_handler5d
    .extern irq_handler5e
    .extern irq_handler5f
    .extern irq_handler60
    .extern irq_handler61
    .extern irq_handler62
    .extern irq_handler63
    .extern irq_handler64
    .extern irq_handler65
    .extern irq_handler66
    .extern irq_handler67
    .extern irq_handler68
    .extern irq_handler69
    .extern irq_handler6a
    .extern irq_handler6b
    .extern irq_handler6c
    .extern irq_handler6d
    .extern irq_handler6e
    .extern irq_handler6f
    .extern irq_handler70
    .extern irq_handler71
    .extern irq_handler72
    .extern irq_handler73
    .extern irq_handler74
    .extern irq_handler75
    .extern irq_handler76
    .extern irq_handler77
    .extern irq_handler78
    .extern irq_handler79
    .extern irq_handler7a
    .extern irq_handler7b
    .extern irq_handler7c
    .extern irq_handler7d
    .extern irq_handler7e
    .extern irq_handler7f
	.extern low_level_init
    .extern usb_it_handler
    .extern uart_it_handler
	.extern main

	.bss
.globl d0_reset
d0_reset:
     .long	0

.globl d1_reset
d1_reset:
     .long	0

	.section .vectors, "x"
	.= 0
/*
 * Exception Vector Table
 */
.globl VECTOR_TABLE
VECTOR_TABLE:

INITSP:		.long	__SP_INIT		/* Initial SP			*/
INITPC:		.long	__start			/* Initial PC			*/
vector02:	.long	irq_handler02	/* Access Error			*/
vector03:	.long	irq_handler03	/* Address Error		*/
vector04:	.long	irq_handler04	/* Illegal Instruction	*/
vector05:	.long	irq_handler05	/* Divide by 0  		*/
vector06:	.long	irq_handler06	/* Reserved				*/
vector07:	.long	irq_handler07	/* Reserved				*/
vector08:	.long	irq_handler08	/* Privilege Violation	*/
vector09:	.long	irq_handler09	/* Trace				*/
vector0a:	.long	irq_handler0a	/* Unimplemented A-Line	*/
vector0b:	.long	irq_handler0b	/* Unimplemented F-Line	*/
vector0c:	.long	irq_handler0c	/* Debug Interrupt		*/
vector0d:	.long	irq_handler0d	/* Reserved				*/
vector0e:	.long	irq_handler0e	/* Format Error			*/
vector0f:	.long	irq_handler0f	/* Reserved             */
vector10:	.long	irq_handler10	/* Reserved				*/
vector11:	.long	irq_handler11	/* Reserved				*/
vector12:	.long	irq_handler12	/* Reserved				*/
vector13:	.long	irq_handler13	/* Reserved				*/
vector14:	.long	irq_handler14	/* Reserved				*/
vector15:	.long	irq_handler15	/* Reserved				*/
vector16:	.long	irq_handler16	/* Reserved				*/
vector17:	.long	irq_handler17	/* Reserved				*/
vector18:	.long	irq_handler18	/* Spurious Interrupt	*/
vector19:	.long	irq_handler19	/* Reserved             */
vector1a:	.long	irq_handler1a	/* Reserved             */
vector1b:	.long	irq_handler1b	/* Reserved             */
vector1c:	.long	irq_handler1c	/* Reserved             */
vector1d:	.long	irq_handler1d	/* Reserved             */
vector1e:	.long	irq_handler1e	/* Reserved             */
vector1f:	.long	irq_handler1f	/* Reserved             */
vector20:	.long	irq_handler20	/* TRAP #0				*/
vector21:	.long	irq_handler21	/* TRAP #1				*/
vector22:	.long	irq_handler22	/* TRAP #2				*/
vector23:	.long	irq_handler23	/* TRAP #3				*/
vector24:	.long	irq_handler24	/* TRAP #4				*/
vector25:	.long	irq_handler25	/* TRAP #5				*/
vector26:	.long	irq_handler26	/* TRAP #6				*/
vector27:	.long	irq_handler27	/* TRAP #7				*/
vector28:	.long	irq_handler28	/* TRAP #8				*/
vector29:	.long	irq_handler29	/* TRAP #9				*/
vector2a:	.long	irq_handler2a	/* TRAP #10				*/
vector2b:	.long	irq_handler2b	/* TRAP #11				*/
vector2c:	.long	irq_handler2c	/* TRAP #12				*/
vector2d:	.long	irq_handler2d	/* TRAP #13				*/
vector2e:	.long	irq_handler2e	/* TRAP #14				*/
vector2f:	.long	irq_handler2f	/* TRAP #15				*/
vector30:	.long	irq_handler30	/* Reserved				*/
vector31:	.long	irq_handler31	/* Reserved				*/
vector32:	.long	irq_handler32	/* Reserved				*/
vector33:	.long	irq_handler33	/* Reserved				*/
vector34:	.long	irq_handler34	/* Reserved				*/
vector35:	.long	irq_handler35	/* Reserved				*/
vector36:	.long	irq_handler36	/* Reserved				*/
vector37:	.long	irq_handler37	/* Reserved				*/
vector38:	.long	irq_handler38	/* Reserved				*/
vector39:	.long	irq_handler39	/* Reserved				*/
vector3a:	.long	irq_handler3a	/* Reserved				*/
vector3b:	.long	irq_handler3b	/* Reserved				*/
vector3c:	.long	irq_handler3c	/* Reserved				*/
vector3d:	.long	irq_handler3d	/* Reserved				*/
vector3e:	.long	irq_handler3e	/* Reserved				*/
vector3f:	.long	irq_handler3f	/* Reserved				*/
vector40:	.long	irq_handler40  /* EPF1 */
vector41:	.long	irq_handler41  /* EPF2 */
vector42:	.long	irq_handler42  /* EPF3 */
vector43:	.long	irq_handler43  /* EPF4 */
vector44:	.long	irq_handler44  /* EPF5 */
vector45:	.long	irq_handler45  /* EPF6 */
vector46:	.long	irq_handler46  /* EPF7 */
vector47:	.long	irq_handler47  /* SWTI */
vector48:	.long	irq_handler48  /* DMA0 */
vector49:	.long	irq_handler49  /* DMA1 */
vector4a:	.long	irq_handler4a  /* DMA2 */
vector4b:	.long	irq_handler4b  /* DMA3 */
vector4c:	.long	irq_handler4c  /* UART0 */
vector4d:	.long	irq_handler4d  /* UART1 */
vector4e:	.long	irq_handler4e  /* UART2 */
vector4f:	.long	irq_handler4f  /* Reserved				*/
vector50:	.long	irq_handler50  /* I2C */
vector51:	.long	irq_handler51  /* QSPI */
vector52:	.long	irq_handler52  /* DTIM0 */
vector53:	.long	irq_handler53  /* DTIM1 */
vector54:	.long	irq_handler54  /* DTIM2 */
vector55:	.long	irq_handler55  /* DTIM3 */
vector56:	.long	irq_handler56  /* FLEXCAN buf0 */
vector57:	.long	irq_handler57  /* FLEXCAN buf1 */
vector58:	.long	irq_handler58  /* FLEXCAN buf2 */
vector59:	.long	irq_handler59  /* FLEXCAN buf3 */
vector5a:	.long	irq_handler5a  /* FLEXCAN buf4 */
vector5b:	.long	irq_handler5b  /* FLEXCAN buf5 */
vector5c:	.long	irq_handler5c  /* FLEXCAN buf6 */
vector5d:	.long	irq_handler5d  /* FLEXCAN buf7 */
vector5e:	.long	irq_handler5e  /* FLEXCAN buf8 */
vector5f:	.long	irq_handler5f  /* FLEXCAN buf9 */
vector60:	.long	irq_handler60  /* FLEXCAN buf10 */
vector61:	.long	irq_handler61  /* FLEXCAN buf11 */
vector62:	.long	irq_handler62  /* FLEXCAN buf12 */
vector63:	.long	irq_handler63  /* FLEXCAN buf13 */
vector64:	.long	irq_handler64  /* FLEXCAN buf14 */
vector65:	.long	irq_handler65  /* FLEXCAN buf15 */
vector66:	.long	irq_handler66  /* FLEXCAN ERR */
vector67:	.long	irq_handler67  /* FLEXCAN BOF */
vector68:	.long	irq_handler68  /* GTP TOF */
vector69:	.long	irq_handler69  /* GTP PAIF */
vector6a:	.long	irq_handler6a  /* GTP PAOVF */
vector6b:	.long	irq_handler6b  /* GTP C0F */
vector6c:	.long	irq_handler6c  /* GTP C1F */
vector6d:	.long	irq_handler6d  /* GTP C2F */
vector6e:	.long	irq_handler6e  /* GTP C3F */
vector6f:	.long	irq_handler6f  /* PMM LVDF */
vector70:	.long	irq_handler70  /* ADC ADCA */
vector71:	.long	irq_handler71  /* ADC ADCB */
vector72:	.long	irq_handler72  /* ADC ADCINT */
vector73:	.long	irq_handler73  /* reserved ?usb?*/
vector74:	.long	irq_handler74  /* reserved */
vector75:   .long   usb_it_handler /* USB */
vector76:   .long   irq_handler76  /* reserved */
vector77:   .long   irq_handler77  /* PIT0 PIF V55*/
vector78:   .long   irq_handler78  /* PIT1 PIF V56*/
vector79:   .long   irq_handler79  /* reserved */
vector7A:   .long   irq_handler7a  /* reserved */
vector7B:   .long   irq_handler7b  /* CMF CBEIF */
vector7C:   .long   irq_handler7c  /* CMF CCIF */
vector7D:   .long   irq_handler7d  /* CMF PVIF */
vector7E:   .long   irq_handler7e  /* CMF AEIF */
vector7F:   .long   irq_handler7f  /* RTC Interrupt */


    .org 0x400
KEY_UPPER:  .long   0x00000000
KEY_LOWER:  .long   0x00000000
CFMPROT:    .long   0x00000000
CFMSACC:    .long   0x00000000
CFMDACC:    .long   0x00000000
CFMSEC:     .long   0x00000000

.set RAMBAR1,0xc05
.set RAMBAR0,0xc04
/********************************************************************/
.globl  __start
__start:
	/* Save off reset values of D0 and D1 */
	move.l	%d0,%d6
	move.l	%d1,%d7

	/* Initialize RAMBAR1: locate SRAM and validate it */
	move.l	#__SRAM,%d0
	add.l	#0x21,%d0
	movec	%d0, #RAMBAR1

	/* Locate Stack Pointer */
	move.l	#__SP_INIT, %sp

	/* Initialize IPSBAR */
	move.l	#__IPSBAR, %d0
	add.l	#0x1, %d0
	move.l	%d0,0x40000000

	/* Initialize FLASHBAR */
	/* Flash at address 0x0. */
	move.l	 #0,%d0
	add.l	#0x61,%d0
	movec	%d0,#RAMBAR0

	/* Locate Stack Pointer */
	move.l	#__SP_INIT,%sp

	/* Save off intial D0 and D1 to RAM */
	move.l	%d6,d0_reset
	move.l	%d7,d1_reset

	/* Common startup code */
	jsr 	low_level_init

	/* Jump to the main process */
	jsr		main

	bra		.
	nop
	nop
	halt

	.end
