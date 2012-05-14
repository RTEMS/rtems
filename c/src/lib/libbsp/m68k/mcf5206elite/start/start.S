/*
 *  MCF5206eLITE startup code
 *
 *  This file contains the entry point for the application.
 *  The name of this entry point is compiler dependent.
 *  It jumps to the BSP which is responsible for performing
 *  all initialization.
 *
 *  Copyright (C) 2000 OKTET Ltd., St.-Petersburg, Russia
 *  Author: Victor V. Vengerov <vvv@oktet.ru>
 *
 *  Based on work:
 *  Author:
 *    David Fiddes, D.J@fiddes.surfaid.org
 *    http://www.calm.hw.ac.uk/davidf/coldfire/
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems/asm.h>
#include "bsp.h"

BEGIN_CODE

/* Initial stack situated in on-chip static memory */
#define INITIAL_STACK BSP_MEM_ADDR_SRAM+BSP_MEM_SIZE_SRAM-4

	PUBLIC	(INTERRUPT_VECTOR)
SYM(INTERRUPT_VECTOR):
	.long   INITIAL_STACK		| 00: initial SSP
	.long	start			| 01: Initial PC
	.long	_unexp_exception	| 02: Access Error
	.long	_unexp_exception	| 03: Address Error
	.long	_unexp_exception	| 04: Illegal Instruction
	.long	_reserved_int		| 05: Reserved
	.long	_reserved_int		| 06: Reserved
	.long	_reserved_int		| 07: Reserved
	.long	_unexp_exception	| 08: Priveledge Violation
	.long	_unexp_exception	| 09: Trace
	.long	_unexp_exception	| 0A: Unimplemented A-Line
	.long	_unexp_exception	| 0B: Unimplemented F-Line
	.long	_unexp_exception	| 0C: Debug interrupt
	.long	_reserved_int		| 0D: Reserved
	.long	_unexp_exception	| 0E: Format error
	.long	_unexp_exception	| 0F: Uninitialized interrupt
	.long	_reserved_int		| 10: Reserved
	.long	_reserved_int		| 11: Reserved
	.long	_reserved_int		| 12: Reserved
	.long	_reserved_int		| 13: Reserved
	.long	_reserved_int		| 14: Reserved
	.long	_reserved_int		| 15: Reserved
	.long	_reserved_int		| 16: Reserved
	.long	_reserved_int		| 17: Reserved
	.long	_spurious_int		| 18: Spurious interrupt
	.long	_avec1_int		| 19: Autovector Level 1
	.long	_avec2_int		| 1A: Autovector Level 2
	.long	_avec3_int		| 1B: Autovector Level 3
	.long	_avec4_int		| 1C: Autovector Level 4
	.long	_avec5_int		| 1D: Autovector Level 5
	.long	_avec6_int		| 1E: Autovector Level 6
	.long	_avec7_int		| 1F: Autovector Level 7
	.long	_unexp_exception	| 20: TRAP #0
	.long	_unexp_exception	| 21: TRAP #1
	.long	_unexp_exception	| 22: TRAP #2
	.long	_unexp_exception	| 23: TRAP #3
	.long	_unexp_exception	| 24: TRAP #4
	.long	_unexp_exception	| 25: TRAP #5
	.long	_unexp_exception	| 26: TRAP #6
	.long	_unexp_exception	| 27: TRAP #7
	.long	_unexp_exception	| 28: TRAP #8
	.long	_unexp_exception	| 29: TRAP #9
	.long	_unexp_exception	| 2A: TRAP #10
	.long	_unexp_exception	| 2B: TRAP #11
	.long	_unexp_exception	| 2C: TRAP #12
	.long	_unexp_exception	| 2D: TRAP #13
	.long	_unexp_exception	| 2E: TRAP #14
	.long	_unexp_exception	| 2F: TRAP #15
	.long	_reserved_int		| 30: Reserved
	.long	_reserved_int		| 31: Reserved
	.long	_reserved_int		| 32: Reserved
	.long	_reserved_int		| 33: Reserved
	.long	_reserved_int		| 34: Reserved
	.long	_reserved_int		| 35: Reserved
	.long	_reserved_int		| 36: Reserved
	.long	_reserved_int		| 37: Reserved
	.long	_reserved_int		| 38: Reserved
	.long	_reserved_int		| 39: Reserved
	.long	_reserved_int		| 3A: Reserved
	.long	_reserved_int		| 3B: Reserved
	.long	_reserved_int		| 3C: Reserved
	.long	_reserved_int		| 3D: Reserved
	.long	_reserved_int		| 3E: Reserved
	.long	_reserved_int		| 3F: Reserved

	.long	_unexp_int		| 40-FF: User defined interrupts
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int

	.long	_unexp_int		| 50:
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int

	.long	_unexp_int		| 60:
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int

	.long	_unexp_int		| 70:
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int

	.long	_unexp_int		| 80:
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int

	.long	_unexp_int		| 90:
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int

	.long	_unexp_int		| A0:
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int

	.long	_unexp_int		| B0:
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int

	.long	_unexp_int		| C0:
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int

	.long	_unexp_int		| D0:
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int

	.long	_unexp_int		| E0:
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int

	.long	_unexp_int		| F0:
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int
	.long	_unexp_int

	PUBLIC(start)
SYM(start):
	move.w	#0x2700,sr		| First turn off all interrupts!

	move.l	#(BSP_MEM_ADDR_SRAM + MCF5206E_RAMBAR_V),d0
	movec	d0,rambar0		| ...so we have a stack

	move.l	#(INITIAL_STACK),sp	| Set up stack again (may be we are
					| going here from monitor or with
					| BDM interface assistance)

	/*
	 * Remainder of the startup code is handled by C code
	 */
	jmp	SYM(Init5206e)		| Start C code (which never returns)

/***************************************************************************
   Function : CopyDataClearBSSAndStart

   Description : Copy DATA segment, clear BSS segment, initialize heap,
   initialise real stack, start C program. Assume that DATA and BSS sizes
   are multiples of 4.
 ***************************************************************************/
	PUBLIC (CopyDataClearBSSAndStart)
SYM(CopyDataClearBSSAndStart):
	lea	copy_start,a0		| Get start of DATA in RAM
	lea	SYM(etext),a2		| Get start of DATA in ROM
	cmpl	a0,a2			| Are they the same?
	beq.s	NOCOPY			| Yes, no copy necessary
	lea	copy_end,a1		| Get end of DATA in RAM
	bra.s	COPYLOOPTEST		| Branch into copy loop
COPYLOOP:
	movel	a2@+,a0@+		| Copy word from ROM to RAM
COPYLOOPTEST:
	cmpl	a1,a0			| Done?
	bcs.s	COPYLOOP		| No, skip
NOCOPY:

	lea	clear_start,a0		| Get start of BSS
	lea	clear_end,a1		| Get end of BSS
	clrl	d0			| Value to set
	bra.s	ZEROLOOPTEST		| Branch into clear loop
ZEROLOOP:
	movel	d0,a0@+			| Clear a word
ZEROLOOPTEST:
	cmpl	a1,a0			| Done?
	bcs.s	ZEROLOOP		| No, skip
	move	4(a7),d0

	/*
	 * Right : Now we're ready to boot RTEMS
	 */
	clrl	d0			| Pass in null to all boot_card() params
	movel	d0,a7@-			| command line
	jsr	SYM(boot_card)		| Call C boot_card function to startup RTEMS

# Wait forever
_stop:
	nop
	stop	#0x2700
	jmp	_stop

# The following labelled nops is a placeholders for breakpoints
_unexp_exception:
	nop
	jmp	_stop

_unexp_int:
	nop
	jmp	_stop

_reserved_int:
	nop
	jmp	_stop

_spurious_int:
	nop
	jmp	_stop

_avec1_int:
	nop
	jmp	_unexp_int

_avec2_int:
	nop
	jmp	_unexp_int

_avec3_int:
	nop
	jmp	_unexp_int

_avec4_int:
	nop
	jmp	_unexp_int

_avec5_int:
	nop
	jmp	_unexp_int

_avec6_int:
	nop
	jmp	_unexp_int

_avec7_int:
	nop
	jmp	_unexp_int

END_CODE

END
