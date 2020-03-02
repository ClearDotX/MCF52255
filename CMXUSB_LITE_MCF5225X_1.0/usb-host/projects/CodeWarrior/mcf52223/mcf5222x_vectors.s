/******************************************************************************
 This file contains the vector table and the startup code executed out of 
 reset.
 *****************************************************************************/
	.global VECTOR_TABLE
	.global _VECTOR_TABLE
	.global start
	.global d0_reset
	.global _d0_reset
	.global d1_reset
	.global _d1_reset

	.extern __IPSBAR
	.extern __SRAM
    .extern __FLASH
	.extern __SP_INIT

    .extern _irq_handler02
    .extern _irq_handler03
    .extern _irq_handler04
    .extern _irq_handler05
    .extern _irq_handler06
    .extern _irq_handler07
    .extern _irq_handler08
    .extern _irq_handler09
    .extern _irq_handler0a
    .extern _irq_handler0b
    .extern _irq_handler0c
    .extern _irq_handler0d
    .extern _irq_handler0e
    .extern _irq_handler0f
    .extern _irq_handler10
    .extern _irq_handler11
    .extern _irq_handler12
    .extern _irq_handler13
    .extern _irq_handler14
    .extern _irq_handler15
    .extern _irq_handler16
    .extern _irq_handler17
    .extern _irq_handler18
    .extern _irq_handler19
    .extern _irq_handler1a
    .extern _irq_handler1b
    .extern _irq_handler1c
    .extern _irq_handler1d
    .extern _irq_handler1e
    .extern _irq_handler1f
    .extern _irq_handler20
    .extern _irq_handler21
    .extern _irq_handler22
    .extern _irq_handler23
    .extern _irq_handler24
    .extern _irq_handler25
    .extern _irq_handler26
    .extern _irq_handler27
    .extern _irq_handler28
    .extern _irq_handler29
    .extern _irq_handler2a
    .extern _irq_handler2b
    .extern _irq_handler2c
    .extern _irq_handler2d
    .extern _irq_handler2e
    .extern _irq_handler2f
    .extern _irq_handler30
    .extern _irq_handler31
    .extern _irq_handler32
    .extern _irq_handler33
    .extern _irq_handler34
    .extern _irq_handler35
    .extern _irq_handler36
    .extern _irq_handler37
    .extern _irq_handler38
    .extern _irq_handler39
    .extern _irq_handler3a
    .extern _irq_handler3b
    .extern _irq_handler3c
    .extern _irq_handler3d
    .extern _irq_handler3e
    .extern _irq_handler3f
    .extern _irq_handler40
    .extern _irq_handler41
    .extern _irq_handler42
    .extern _irq_handler43
    .extern _irq_handler44
    .extern _irq_handler45
    .extern _irq_handler46
    .extern _irq_handler47
    .extern _irq_handler48
    .extern _irq_handler49
    .extern _irq_handler4a
    .extern _irq_handler4b
    .extern _irq_handler4c
    .extern _irq_handler4d
    .extern _irq_handler4e
    .extern _irq_handler4f
    .extern _irq_handler50
    .extern _irq_handler51
    .extern _irq_handler52
    .extern _irq_handler53
    .extern _irq_handler54
    .extern _irq_handler55
    .extern _irq_handler56
    .extern _irq_handler57
    .extern _irq_handler58
    .extern _irq_handler59
    .extern _irq_handler5a
    .extern _irq_handler5b
    .extern _irq_handler5c
    .extern _irq_handler5d
    .extern _irq_handler5e
    .extern _irq_handler5f
    .extern _irq_handler60
    .extern _irq_handler61
    .extern _irq_handler62
    .extern _irq_handler63
    .extern _irq_handler64
    .extern _irq_handler65
    .extern _irq_handler66
    .extern _irq_handler67
    .extern _irq_handler68
    .extern _irq_handler69
    .extern _irq_handler6a
    .extern _irq_handler6b
    .extern _irq_handler6c
    .extern _irq_handler6d
    .extern _irq_handler6e
    .extern _irq_handler6f
    .extern _irq_handler70
    .extern _irq_handler71
    .extern _irq_handler72
    .extern _irq_handler73    
    .extern _irq_handler74
    .extern _irq_handler75
    .extern _irq_handler76
    .extern _irq_handler77
    .extern _irq_handler78
    .extern _irq_handler79
    .extern _irq_handler7a
    .extern _irq_handler7b
    .extern _irq_handler7c
    .extern _irq_handler7d
    .extern _irq_handler7e
    .extern _irq_handler7f
	.extern _low_level_init
    .extern _usb_it_handler
    .extern _uart_it_handler
	.extern _main
	.bss
d0_reset:
_d0_reset:	.long	0
d1_reset:
_d1_reset:	.long	0

	.text

/*
 * Exception Vector Table
 */
VECTOR_TABLE:
_VECTOR_TABLE:
INITSP:		.long	__SP_INIT		/* Initial SP			*/
INITPC:		.long	start			/* Initial PC			*/
vector02:	.long	_irq_handler02	/* Access Error			*/
vector03:	.long	_irq_handler03	/* Address Error		*/
vector04:	.long	_irq_handler04	/* Illegal Instruction	*/
vector05:	.long	_irq_handler05	/* Divide by 0  		*/
vector06:	.long	_irq_handler06	/* Reserved				*/
vector07:	.long	_irq_handler07	/* Reserved				*/
vector08:	.long	_irq_handler08	/* Privilege Violation	*/
vector09:	.long	_irq_handler09	/* Trace				*/
vector0a:	.long	_irq_handler0a	/* Unimplemented A-Line	*/
vector0b:	.long	_irq_handler0b	/* Unimplemented F-Line	*/
vector0c:	.long	_irq_handler0c	/* Debug interrupt		*/
vector0d:	.long	_irq_handler0d	/* Reserved				*/
vector0e:	.long	_irq_handler0e	/* Format Error			*/
vector0f:	.long	_irq_handler0f	/* Reserved             */
vector10:	.long	_irq_handler10	/* Reserved				*/
vector11:	.long	_irq_handler11	/* Reserved				*/
vector12:	.long	_irq_handler12	/* Reserved				*/
vector13:	.long	_irq_handler13	/* Reserved				*/
vector14:	.long	_irq_handler14	/* Reserved				*/
vector15:	.long	_irq_handler15	/* Reserved				*/
vector16:	.long	_irq_handler16	/* Reserved				*/
vector17:	.long	_irq_handler17	/* Reserved				*/
vector18:	.long	_irq_handler18	/* Spurious interrupt	*/
vector19:	.long	_irq_handler19	/* Reserved             */
vector1a:	.long	_irq_handler1a	/* Reserved             */
vector1b:	.long	_irq_handler1b	/* Reserved             */
vector1c:	.long	_irq_handler1c	/* Reserved             */
vector1d:	.long	_irq_handler1d	/* Reserved             */
vector1e:	.long	_irq_handler1e	/* Reserved             */
vector1f:	.long	_irq_handler1f	/* Reserved             */
vector20:	.long	_irq_handler20	/* TRAP #0				*/
vector21:	.long	_irq_handler21	/* TRAP #1				*/
vector22:	.long	_irq_handler22	/* TRAP #2				*/
vector23:	.long	_irq_handler23	/* TRAP #3				*/
vector24:	.long	_irq_handler24	/* TRAP #4				*/
vector25:	.long	_irq_handler25	/* TRAP #5				*/
vector26:	.long	_irq_handler26	/* TRAP #6				*/
vector27:	.long	_irq_handler27	/* TRAP #7				*/
vector28:	.long	_irq_handler28	/* TRAP #8				*/
vector29:	.long	_irq_handler29	/* TRAP #9				*/
vector2a:	.long	_irq_handler2a	/* TRAP #10				*/
vector2b:	.long	_irq_handler2b	/* TRAP #11				*/
vector2c:	.long	_irq_handler2c	/* TRAP #12				*/
vector2d:	.long	_irq_handler2d	/* TRAP #13				*/
vector2e:	.long	_irq_handler2e	/* TRAP #14				*/
vector2f:	.long	_irq_handler2f	/* TRAP #15				*/
vector30:	.long	_irq_handler30	/* Reserved				*/
vector31:	.long	_irq_handler31	/* Reserved				*/
vector32:	.long	_irq_handler32	/* Reserved				*/
vector33:	.long	_irq_handler33	/* Reserved				*/
vector34:	.long	_irq_handler34	/* Reserved				*/
vector35:	.long	_irq_handler35	/* Reserved				*/
vector36:	.long	_irq_handler36	/* Reserved				*/
vector37:	.long	_irq_handler37	/* Reserved				*/
vector38:	.long	_irq_handler38	/* Reserved				*/
vector39:	.long	_irq_handler39	/* Reserved				*/
vector3a:	.long	_irq_handler3a	/* Reserved				*/
vector3b:	.long	_irq_handler3b	/* Reserved				*/
vector3c:	.long	_irq_handler3c	/* Reserved				*/
vector3d:	.long	_irq_handler3d	/* Reserved				*/
vector3e:	.long	_irq_handler3e	/* Reserved				*/
vector3f:	.long	_irq_handler3f	/* Reserved				*/
vector40:	.long	_irq_handler40  /* EPF1 */
vector41:	.long	_irq_handler41  /* EPF2 */
vector42:	.long	_irq_handler42  /* EPF3 */
vector43:	.long	_irq_handler43  /* EPF4 */
vector44:	.long	_irq_handler44  /* EPF5 */
vector45:	.long	_irq_handler45  /* EPF6 */
vector46:	.long	_irq_handler46  /* EPF7 */
vector47:	.long	_irq_handler47  /* SWTI */
vector48:	.long	_irq_handler48  /* DMA0 */
vector49:	.long	_irq_handler49  /* DMA1 */
vector4a:	.long	_irq_handler4a  /* DMA2 */
vector4b:	.long	_irq_handler4b  /* DMA3 */
vector4c:	.long	_irq_handler4c  /* UART0 */
vector4d:	.long	_irq_handler4d  /* UART1 */
vector4e:	.long	_irq_handler4e  /* UART2 */
vector4f:	.long	_irq_handler4f  /* Reserved				*/
vector50:	.long	_irq_handler50  /* I2C */
vector51:	.long	_irq_handler51  /* QSPI */
vector52:	.long	_irq_handler52  /* DTIM0 */
vector53:	.long	_irq_handler53  /* DTIM1 */
vector54:	.long	_irq_handler54  /* DTIM2 */
vector55:	.long	_irq_handler55  /* DTIM3 */
vector56:	.long	_irq_handler56  /* FLEXCAN buf0 */
vector57:	.long	_irq_handler57  /* FLEXCAN buf1 */
vector58:	.long	_irq_handler58  /* FLEXCAN buf2 */
vector59:	.long	_irq_handler59  /* FLEXCAN buf3 */
vector5a:	.long	_irq_handler5a  /* FLEXCAN buf4 */
vector5b:	.long	_irq_handler5b  /* FLEXCAN buf5 */
vector5c:	.long	_irq_handler5c  /* FLEXCAN buf6 */
vector5d:	.long	_irq_handler5d  /* FLEXCAN buf7 */
vector5e:	.long	_irq_handler5e  /* FLEXCAN buf8 */
vector5f:	.long	_irq_handler5f  /* FLEXCAN buf9 */
vector60:	.long	_irq_handler60  /* FLEXCAN buf10 */
vector61:	.long	_irq_handler61  /* FLEXCAN buf11 */
vector62:	.long	_irq_handler62  /* FLEXCAN buf12 */
vector63:	.long	_irq_handler63  /* FLEXCAN buf13 */
vector64:	.long	_irq_handler64  /* FLEXCAN buf14 */
vector65:	.long	_irq_handler65  /* FLEXCAN buf15 */
vector66:	.long	_irq_handler66  /* FLEXCAN ERR */
vector67:	.long	_irq_handler67  /* FLEXCAN BOF */
vector68:	.long	_irq_handler68  /* GTP TOF */
vector69:	.long	_irq_handler69  /* GTP PAIF */
vector6a:	.long	_irq_handler6a  /* GTP PAOVF */
vector6b:	.long	_irq_handler6b  /* GTP C0F */
vector6c:	.long	_irq_handler6c  /* GTP C1F */
vector6d:	.long	_irq_handler6d  /* GTP C2F */
vector6e:	.long	_irq_handler6e  /* GTP C3F */
vector6f:	.long	_irq_handler6f  /* PMM LVDF */
vector70:	.long	_irq_handler70  /* ADC ADCA */
vector71:	.long	_irq_handler71  /* ADC ADCB */
vector72:	.long	_irq_handler72  /* ADC ADCINT */
vector73:	.long	_irq_handler73  /* reserved ?usb?*/
vector74:	.long	_irq_handler74  /* reserved */
vector75:   .long   _usb_it_handler /* USB */
vector76:   .long   _irq_handler76  /* reserved */
vector77:   .long   _irq_handler77  /* PIT0 PIF V55*/
vector78:   .long   _irq_handler78  /* PIT1 PIF V56*/
vector79:   .long   _irq_handler79  /* reserved */
vector7A:   .long   _irq_handler7a  /* reserved */
vector7B:   .long   _irq_handler7b  /* CMF CBEIF */
vector7C:   .long   _irq_handler7c  /* CMF CCIF */
vector7D:   .long   _irq_handler7d  /* CMF PVIF */
vector7E:   .long   _irq_handler7e  /* CMF AEIF */
vector7F:   .long   _irq_handler7f  /* RTC interrupt */


    .org 0x400
KEY_UPPER:  .long   0x00000000
KEY_LOWER:  .long   0x00000000
CFMPROT:    .long   0x00000000
CFMSACC:    .long   0x00000000
CFMDACC:    .long   0x00000000
CFMSEC:     .long   0x00000000

/********************************************************************/
start:
	/* Save off reset values of D0 and D1 */
	move.l	d0,d6
	move.l	d1,d7

	/* Initialize RAMBAR1: locate SRAM and validate it */
	move.l	#__SRAM,d0
	add.l	#0x21,d0
	movec	d0,RAMBAR1

	/* Locate Stack Pointer */
	move.l	#__SP_INIT,sp

	/* Initialize IPSBAR */
	move.l	#__IPSBAR,d0
	add.l	#0x1,d0
	move.l	d0,0x40000000

	/* Initialize FLASHBAR */
	/* Flash at address 0x0. */
	move.l	 #0,d0
	add.l	#0x61,d0
	movec	d0,RAMBAR0
	
	/* Locate Stack Pointer */
	move.l	#__SP_INIT,sp

	/* Save off intial D0 and D1 to RAM */
	move.l	d6,d0_reset
	move.l	d7,d1_reset

	/* Common startup code */
	jsr 	_low_level_init

	/* Jump to the main process */
	jsr		_main

	bra		.
	nop
	nop
	halt

	.end
