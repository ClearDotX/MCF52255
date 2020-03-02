/*********************************************
 Freescale Semiconductor 2007
 (c) COPYRIGHT Freescale Semiconductor 2007
 ALL RIGHTS RESERVED.
**********************************************/

/* assembly macros */

/* initialization macro: use at the beginning of every assembly function */
FUNCTION_INIT_MACRO: .macro
	#ifdef USE_A6_STACK_FRAME
		link	A6,#0
	#endif

	#ifdef SAVE_RESTORE_STATE
		lea	-60(A7),A7
		movem.l	D0-D7/A0-A5,(A7)
	#endif

	/* get function argument from stack */
	#ifdef USE_STACK_ARG
		#ifdef USE_A6_STACK_FRAME
			#define base	A6
			#define offset	8
		#else
			#define base	A7
		#endif

		#ifdef SAVE_RESTORE_STATE
			#define offset	64
		#else
			#define offset	4
		#endif

		movea.l	offset(base),A0	; load A0
	#endif

	/* otherwise function argument is already loaded into A0 */

.endm

/* cleanup macro: use at the end of every assembly function */
FUNCTION_DONE_MACRO: .macro
	#ifdef SAVE_RESTORE_STATE
		movem.l	(A7),D0-D7/A0-A5
		lea	60(A7),A7
	#endif

	#ifdef USE_A6_STACK_FRAME
		unlk	A6
	#endif

	rts

.endm

/* macro to start DMA timer */
START_TIMER_MACRO: .macro timer,Ax
	movea.l	#(0x40000400+timer*0x40),Ax	; address to DTMR
   	move.w	#0x0000,(Ax)			; reset timer
 	move.w	#0x0003,(Ax)			; start timer
.endm

/* macro to read DMA timer */
READ_TIMER_MACRO: .macro timer,Dx
	move.l	(0x4000040C+timer*0x40).l,Dx	; get timer value
.endm

/*
example calls:	
START_TIMER_MACRO 1,A3
	"clear DTMR1, use A3 for address calc"
READ_TIMER_MACRO 1,D3
	"read DTMR1 into D3"
*/
