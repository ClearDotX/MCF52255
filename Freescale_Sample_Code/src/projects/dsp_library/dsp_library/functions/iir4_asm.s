/*********************************************
 Freescale Semiconductor 2007
 (c) COPYRIGHT Freescale Semiconductor 2007
 ALL RIGHTS RESERVED.
**********************************************/

/*----------------------------------------------------------------------
Description: 4th order IIR filter. All filter coefficients must be
16-bit twos-complement with fixed-point scaling values defined in the
configuration register.  The scaling shift value can be different for
the numerator and demoninator coefficients, but the numerator shift
value must be equal or larger than that of the denominator.

Evaluates the following difference equation:
y(n) = (2^-b) * (B4*x(n-4) + B3*x(n-3) + B2*x(n-2) + B1*x(n-1) + B0*x(n)) +
       (2^-a) * (A4*y(n-4) + A3*y(n-3) + A2*y(n-2) + A1*y(n-1))

----------------------------------------------------------------------
COEFFICIENTS (10x2 Bytes):
offset  {upper, lower}
0       {B4   , B3   }
4       {B2   , B1   }
8       {B0   , A4   }
12      {A3   , A2   }
16      {A1   , A0   }
	
{B4,B3,B2,B1,B0} are 16-bit signed fixed-point numerator coefficients
scaled by b-bits.
{A4,A3,A2,A1,A0} are 16-bit signed fixed-point denominator
coefficients scaled by a-bits. Note that A0 is not used and assumed to
equal negative one (i.e., filter coefficients are normalized to A0).

It is required that a and b are positive, and b is greater than or
equal to a.

----------------------------------------------------------------------
INPUT/OUPUT BUFFER (8x2 Bytes):
offset  {upper , lower }
0       {x(n-1), y(n-1)}
4       {x(n-2), y(n-2)}
8       {x(n-3), y(n-3)}
12      {x(n-4), y(n-4)}

{x(n), x(n-1), x(n-2), x(n-3), x(n-4)} are current and previous 16-bit inputs.
{y(n), y(n-1), y(n-2), y(n-3), y(n-4)} are current and previous 16-bit outputs.

----------------------------------------------------------------------
CONFIGURATION REGISTER (1x2 Bytes):
{b-a,a}

Field    Bits   Description
b-a      8      difference between num and den scaling bits
a        8      denominator scaling bits

----------------------------------------------------------------------*/

.global	iir4_asm
.global	_iir4_asm

#include <dsp_library_defines.h>
#include <dsp_library_asm_macros.h>
	
.text

iir4_asm:
_iir4_asm:
	FUNCTION_INIT_MACRO

 	movea.l	IIR_INPUT_OFFSET(A0),A1		; pntr to input data
 	movea.l	IIR_COEF_OFFSET(A0),A2		; addr of coef.start
 	move.l	#0x00000080,MACSR		; load MACSR for Sat|signed|Integer|round
 	move.l	#0,ACC				; clear the acc
	move.w	(A1),D0				; load current input
 	movem.l	IIR_BUFFER_OFFSET(A0),D1-D4	; load all previous inputs and outputs
 	movem.l	(A2),D5-D7			; load {B4, B3} {B2, B1} {B0, A4}
	swap.w	D0				; swap current input into upper half of reg

 	mac.w	D4.u,D5.u			; acc + x(n-4)*B4
 	mac.w	D3.u,D5.l			; acc + x(n-3)*B3
 	mac.w	D2.u,D6.u			; acc + x(n-2)*B2
 	mac.w	D1.u,D6.l			; acc + x(n-1)*B1
 	mac.w	D0.u,D7.u			; acc + x(n)*B0	

 	move.b	IIR_NUM_SF_OFFSET(A0),D0	; get the num shift value (difference between num and den sf)
 	move.l	ACC,D7				; get the acc
 	asr.l	D0,D7				; shift the numerator sum
 	addx.l	D0,D7				; add CCR(X) and D0 (for rounding)
 	sub.l	D0,D7				; subtract back D0
 	move.l	D7,ACC				; move back to acc
 	movem.l	10(A2),D5-D6			; load {A4, A3} {A2, A1}

 	mac.w	D4.l,D5.u			; acc + y(n-4)*A4
 	mac.w	D3.l,D5.l			; acc + y(n-3)*A3
 	mac.w	D2.l,D6.u			; acc + y(n-2)*A2
 	mac.w	D1.l,D6.l			; acc + y(n-1)*A1
	
 	move.b	IIR_DEN_SF_OFFSET(A0),D0	; get the den shift value
 	move.l	ACC,D7				; get the acc
 	asr.l	D0,D7				; shift the final sum
 	addx.l	D0,D7				; add CCR(X) and D0 (for rounding)
 	sub.l	D0,D7				; subtract back D0
 	move.w	D7,D0				; move lower half of result into D0

check_pos_sat:
_check_pos_sat:	
 	cmpi.l	#0x00007fff,D7			; check for positive saturation
 	ble	check_neg_sat			; no positive saturation if D7 <= 0x7fff
 	move.w	#0x7fff,D0			; positive saturation value
 	bra	write_data_out
	
check_neg_sat:
_check_neg_sat:
 	cmpi.l	#0xffff8000,D7			; check for negative saturation
 	bge	write_data_out			; no negative saturation if D7 >= 0x8000
 	move.w	#0x8000,D0			; negative saturation value
	
write_data_out:
_write_data_out:
 	move.w	D0,IIR_OUTPUT_OFFSET(A0)	; write data out
 	movem.l	D0-D3,IIR_BUFFER_OFFSET(A0)	; advance buffer
		
	FUNCTION_DONE_MACRO
