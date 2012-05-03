/*  entry.s
 *
 *  This file contains the entry point for the application.
 *  The name of this entry point is compiler dependent.
 *  It jumps to the BSP which is responsible for performing
 *  all initialization.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems/asm.h>

  .set	BAR,		0xF2		| Base Address Register location
  .set	SCR,		0xF4		| System Control Register location
  .set	BAR_VAL,	0x0f7f		| BAR value
  .set	SCR_VAL,	0x00080f00	| SCR value
  .set	GIMR_VAL,	0x8780		|Global Interrupt Mode Register. (MUST BE WRITTEN).
  .set	BaseAddr,(BAR_VAL&0x0fff)<<12	| MC68302 internal base address

  .set	oSYSRAM,	0x000		| 576 bytes of internal system RAM

  .set	oGIMR,		0x812

  .set	oCS0_Base,	0x830		| 16 bits, Chip Sel 0 Base Reg
  .set	oCS0_Option,	0x832		| 16 bits, Chip Sel 0 Option Reg
  .set	oCS1_Base,	0x834		| 16 bits, Chip Sel 1 Base Reg
  .set	oCS1_Option,	0x836		| 16 bits, Chip Sel 1 Option Reg
  .set	oCS2_Base,	0x838		| 16 bits, Chip Sel 2 Base Reg
  .set	oCS2_Option,	0x83a		| 16 bits, Chip Sel 2 Option Reg
  .set	oCS3_Base,	0x83c		| 16 bits, Chip Sel 3 Base Reg
  .set	oCS3_Option,	0x83e		| 16 bits, Chip Sel 3 Option Reg

  .set	tmpSRAM_BASE,	0x400000	| start of temporary SRAM
  .set	FLASH_BASE,	0xc00000	| start of FLASH''s normal location

BEGIN_CODE
         PUBLIC (M68Kvec)               | Vector Table
SYM (M68Kvec):				| standard location for vectors
V___ISSP: .long	0x00001000		|00  0 Reset: Initial SSP
V____IPC: .long	SYM(start)-V___ISSP	|04  1 Reset: Initial PC
V_BUSERR: .long	Bad-V___ISSP		|08  2 Bus Error
V_ADRERR: .long	Bad-V___ISSP		|0c  3 Address Error
	.space	240			| reserve space for reset of vectors

#if ( M68K_HAS_SEPARATE_STACKS == 1 )
SYM (lowintstack):
        .space   4092                   | reserve for interrupt stack
SYM (hiintstack):
        .space   4                      | end of interrupt stack
#endif

	PUBLIC (start)                 | Default entry point for GNU
SYM (start):
	move.w	#0x2700,sr		| Disable all interrupts
	move.w	#BAR_VAL,BAR		| Set Base Address Register
	move.l	#SCR_VAL,SCR		| Set System Control Register
	lea	BaseAddr,a5
	move.w	#GIMR_VAL,a5@(oGIMR)	| Set Global Interrupt Mode Register

|
| Set up chip select registers for the remapping process.
|

|
|      0      X      x    x    x    x
| 0  000 0  0-- -  --- ---- ---- ----
| x  xxx x  xxx x  xx
|
	move.w	#0xc001,a5@(oCS0_Base)   | Expand CS0 to full size (FLASH)
	move.w	#0x1f82,a5@(oCS0_Option) | 000000-03ffff, R, 0 WS

|
|      X      x      x    x    x    x
| 0  100 0  0-- -  --- ---- ---- ----
| x  xxx x  xxx x  xx
|
	move.w	#0xa801,a5@(oCS1_Base)   | Set up and enable CS1 (SRAM)
	move.w	#0x1f80,a5@(oCS1_Option) | 400000-43ffff, RW, 0 WS

|
| Copy the initial boot FLASH area to the temporary SRAM location.
|
		moveq	#0,d0
		movea.l	d0,a0			| a0 -> start of FLASH
		lea	tmpSRAM_BASE,a1 	| a1 -> start of tmp SRAM
|		moveq	#(endPreBoot-V___ISSP)/4,d0	| # longs to copy
		moveq	#127,d0
cpy_flash:	move.l	(a0)+,(a1)+		| copy
		subq.l	#1,d0
		bne	cpy_flash

|
| Copy remap code to 68302''s internal system RAM.
|
		movea.w	#begRemap-V___ISSP,a0	| a0 -> remap code
		lea	a5@(oSYSRAM),a1	| a1 -> internal system RAM
|		moveq	#(endRemap-begRemap)/2-1,d0	| d0 = # words to copy
		moveq	#11,d0
cpy_remap:	move.w	(a0)+,(a1)+		| copy
		dbra	d0,cpy_remap

|
| Jump to the remap code in the 68302''s internal system RAM.
|
		jmp	a5@(oSYSRAM)		| (effectively a jmp begRemap)

|
| This remap code, when executed from the 68302''s internal system RAM
| will 1) remap CS1 so that SRAM is at 0
|      2) remap CS0 so that FLASH is at FLASH_BASE
|  and 3) jump to executable code in the remapped FLASH.
|
begRemap:	move.w	#0xa001,a5@(oCS1_Base)	| Move CS1 (SRAM)
		move.w	#0xd801,a5@(oCS0_Base)	| Move CS0 (FLASH)
		lea	FLASH_BASE,a0
		jmp	a0@(endRemap-V___ISSP.w)	| Jump back to FLASH
endRemap:
|
| Now set up the remaining chip select registers.
|

|
|      4      0      x    x    x    x
| 1  000 1  111 0  000 0--- ---- ----
| x  xxx x  xxx x  xx
|
 	move.w	#0xb1e1,a5@(oCS2_Base)		| Set up and enable CS2 (dpRAM)
 	move.w	#0x1ff0,a5@(oCS2_Option)	| 8f0000-8f07ff, RW, 0 WS

|
|      8      X      x    x    x    x
| 1  000 0  0-- -  --- ---- ---- ----
| x  xxx x  xxx x  xx
|
	move.w	#0xd001,a5@(oCS3_Base)		| Set up and enable CS3 (IO)
	move.w	#0x1f80,a5@(oCS3_Option)	| 800000-83ffff, RW, 0 WS

endPreBoot:

	move.b	#0x30,0x800001			| set status LED amber

 .set	oPIOB_Ctrl,	0x824
 .set	oPIOB_DDR,	0x826
 .set	oPIOB_Data,	0x828

 .set	oPIOA_Ctrl,	0x81e
 .set	oPIOA_DDR,	0x820
 .set	oPIOA_Data,	0x822

	move.w	#0x0ff8,a5@(oPIOB_Data)	| Make output follow resistors.
	move.w	#0x00ff,a5@(oPIOB_DDR)		| Set up PB7-PB0 for output.
	move.w	#0x0080,a5@(oPIOB_Ctrl)	| Set up WDOG* as dedicated
						| peripheral pins.

	move.w	#0x1fff,a5@(oPIOA_Data)	| Make output follow resistors.
	move.w	#0xea2a,a5@(oPIOA_DDR)		| Set up PA15-PA0 for in/output.
	move.w	#0x0003,a5@(oPIOA_Ctrl)	| Set up TXD2/RXD2 as dedicated
						| peripheral pins.

|
| Place "Bad" in all vectors from 010 thru 0ec.  Vectors 0f0 and 0f4
| are not set because they are the 68302''s BAR and SCR.
|
		movea.w	#0x010,a0
		moveq	#(0x0f0-0x010)/4-1,d0
		move.l	#Bad,d1
cpy_Bad:	move.l	d1,(a0)+
		dbra	d0,cpy_Bad

 .set	vbase,	0x0200

		lea	vbase,a0
		moveq	#31,d0
cpy_Bad1:	move.l	d1,(a0)+
		dbra	d0,cpy_Bad1

|
| Fill in special locations to configure OS
|
		move.l	#Bad,0x008		| Bus Error
		move.l	#Bad,0x00c		| Address Error
		move.l	#Bad,0x024		| Trace
|		move.l	#KE_IRET,$0b4		| pSOS+ RET_I Call

|		move.l	#_cnsl_isr,vbase+0x028	| SCC2
		move.l	#timerisr,vbase+0x018	| Timer ISR

        |
        | zero out uninitialized data area
        |
zerobss:
        moveal  # SYM (_clear_end),a0            | find end of .bss
        moveal  # SYM (_clear_start),a1          | find beginning of .bss
        moveq   #0,d0

loop:   movel   d0,a1@+                | to zero out uninitialized
        cmpal   a0,a1
        jlt     loop                    | loop until _clear_end reached

        movel   d0, SYM (_stack_init)  | Set Stack pointer
        movw    #0x3700,sr             | SUPV MODE,INTERRUPTS OFF!!!
        movel   d0,a7                  | set master stack pointer
        movel   d0,a6                  | set base pointer

      /*
       *  RTEMS should maintain a separate interrupt stack on CPUs
       *  without one in hardware.  This is currently not supported
       *  on versions of the m68k without a HW intr stack.
       */

#if ( M68K_HAS_SEPARATE_STACKS == 1 )
        lea     SYM (hiintstack),a0          | a0 = high end of intr stack
        movec   a0,isp                | set interrupt stack
#endif

        move.l  #0,a7@-               | command line
        jsr     SYM (boot_card)

	nop
Bad:	bra	Bad

	nop
END_CODE

BEGIN_DATA

	PUBLIC (start_frame)
SYM (start_frame):
        .space  4,0

	PUBLIC (stack_start)
SYM (stack_start):
        .space  4,0
END_DATA

BEGIN_BSS

	PUBLIC (environ)
        .align 2
SYM (environ):
        .long  0

	PUBLIC (heap_size)
        .set   SYM (heap_size),0x2000

        PUBLIC (stack_size)
        .set   SYM (stack_size),0x1000

END_DATA
END
