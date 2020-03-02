/******************************************************************************
 This file contains the vector table and the startup code executed out of
 reset.
 *****************************************************************************/
	.text

/********************************************************************/
.globl  mcf5xxx_wr_cacr
mcf5xxx_wr_cacr:
  move.l 4(%sp), %d0
  movec %d0, #0x002
  nop
  rts

/********************************************************************/
.globl  mcf5xxx_wr_vbr
mcf5xxx_wr_vbr:
  move.l 4(%sp), %d0
  movec %d0, #0x801
  nop
  rts

/********************************************************************/
.globl mcf5xxx_byterev
mcf5xxx_byterev:
  link %a6, #0
  move.l  8(%a6), %d0
  .short 0x02c0
  unlk   %a6
  rts
/********************************************************************/
.globl  asm_set_ipl
asm_set_ipl:
  link %A6, #-8
  movem.l %d6-%d7, (%sp)
  move.w %sr, %d7

  move.l %d7, %d0
  andi.l #0x0700, %d0
  lsr.l  #8, %d0

  move.l 8(%a6), %d6
  andi.l #0x07, %d6
  lsl.l  #8, %d6

  andi.l #0x0000f8ff, %d7
  or.l   %d6, %d7
  move.w %d7, %sr

  movem.l (%sp), %d6-%d7
  lea     8(%sp), %sp
  unlk   %A6
  rts


	.end
