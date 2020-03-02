/*********************************************
 Freescale Semiconductor 2007
 (c) COPYRIGHT Freescale Semiconductor 2007
 ALL RIGHTS RESERVED.
**********************************************/

/*----------------------------------------------------------------------
Description: 6th order IIR filter. All filter coefficients must be
16-bit twos-complement with fixed-point scaling values defined in the
configuration register.  The scaling shift value can be different for
the numerator and demoninator coefficients, but the numerator shift
value must be equal or larger than that of the denominator.

Evaluates the following difference equation:
y(n) = (2^-b) * (B6*x(n-6) + B5*x(n-5) + B4*x(n-4) + B3*x(n-3) + B2*x(n-2) + B1*x(n-1) + B0*x(n)) +
       (2^-a) * (A6*y(n-6) + A5*y(n-5) + A4*y(n-4) + A3*y(n-3) + A2*y(n-2) + A1*y(n-1))

----------------------------------------------------------------------
COEFFICIENTS (14x2 Bytes):
offset  {upper, lower}
0       {B6   , B5   }
4       {B4   , B3   }
8       {B2   , B1   }
12      {B0   , A6   }
16      {A5   , A4   }
20      {A3   , A2   }
24      {A1   , A0   }
	
{B6,B5,B4,B3,B2,B1,B0} are 16-bit signed fixed-point numerator
coefficients scaled by b-bits.
{A6,A5,A4,A3,A2,A1,A0} are 16-bit signed fixed-point denominator
coefficients scaled by a-bits. Note that A0 is not used and assumed to
equal negative one (i.e., filter coefficients are normalized to A0).

It is required that a and b are positive, and b is greater than or
equal to a.

----------------------------------------------------------------------
INPUT/OUPUT BUFFER (12x2 Bytes):
offset  {upper , lower }
0       {x(n-1), y(n-1)}
4       {x(n-2), y(n-2)}
8       {x(n-3), y(n-3)}
12      {x(n-4), y(n-4)}
16      {x(n-5), y(n-5)}
20      {x(n-6), y(n-6)}

{x(n), x(n-1), x(n-2), x(n-3), x(n-4), x(n-5), x(n-6)} are current and previous 16-bit inputs.
{y(n), y(n-1), y(n-2), y(n-3), y(n-4), y(n-5), y(n-6)} are current and previous 16-bit outputs.

----------------------------------------------------------------------
CONFIGURATION REGISTER (1x2 Bytes):
{b-a,a}

Field    Bits   Description
b-a      8      difference between num and den scaling bits
a        8      denominator scaling bits

----------------------------------------------------------------------*/

.global	iir6_asm
.global	_iir6_asm

#include <dsp_library_defines.h>
#include <dsp_library_asm_macros.h>
	
.text

iir6_asm:
_iir6_asm:
	FUNCTION_INIT_MACRO

	move.l	IIR_INPUT_OFFSET(A0),A1		; pntr to input data
	movea.l	IIR_COEF_OFFSET(A0),A2		; addr of coef.start
	move.l	#0x00000080,MACSR		; load MACSR for Sat|signed|Integer|round
	move.l	#0,ACC				; clear the acc
	move.w	(A1),D0				; load current input
	movem.l	IIR_BUFFER_OFFSET(A0),D1-D6	; load all previous inputs and outputs
	move.l	(A2)+,D7			; load {B6, B5}
	swap.w	D0				; swap current input into upper half of reg

	mac.w	D6.u,D7.u			; acc + x(n-6)*B6
	mac.w	D5.u,D7.l,(A2)+,D7		; acc + x(n-5)*B5, load {B4, B3}
	mac.w	D4.u,D7.u			; acc + x(n-4)*B4
	mac.w	D3.u,D7.l,(A2)+,D7		; acc + x(n-3)*B3, load {B2, B1}
	mac.w	D2.u,D7.u         		; acc + x(n-2)*B2
	mac.w	D1.u,D7.l,(A2),D7		; acc + x(n-1)*B1, load {B0, A6}
	mac.w	D0.u,D7.u			; acc + x(n)*B0	

	move.b	IIR_NUM_SF_OFFSET(A0),D0	; get the num shift value (difference between num and den sf)
	move.l	ACC,D7				; get the acc
	asr.l	D0,D7				; shift the numerator sum
	addx.l	D0,D7				; add CCR(X) and D0 (for rounding)
	sub.l	D0,D7				; subtract back D0
	move.l	D7,ACC				; move back to acc
	addq.l	#2,A2				; move coef ptr to A6
	move.l	(A2)+,D7			; load {A6, A5}

	mac.w	D6.l,D7.u			; acc + y(n-6)*A6
	mac.w	D5.l,D7.l,(A2)+,D7		; acc + y(n-5)*A5, load {A4, A3}
	mac.w	D4.l,D7.u			; acc + y(n-4)*A4
	mac.w	D3.l,D7.l,(A2)+,D7		; acc + y(n-3)*A3, load {A2, A1}
	mac.w	D2.l,D7.u         		; acc + y(n-2)*A2
	mac.w	D1.l,D7.l			; acc + y(n-1)*A1
	
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
	movem.l	D0-D5,IIR_BUFFER_OFFSET(A0)	; advance buffer
		
	FUNCTION_DONE_MACRO
