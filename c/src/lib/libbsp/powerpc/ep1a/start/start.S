/*
 * This is based on the mvme-crt0.S file from libgloss/rs6000.
 * crt0.S -- startup file for PowerPC systems.
 *
 * (c) 1998, Radstone Technology plc.
 *
 *
 * This is an unpublished work the copyright  in  which   vests
 * in  Radstone Technology plc.  All rights reserved.
 *
 * The information contained herein is the property of Radstone
 * Technology  plc.  and  is  supplied  without  liability  for
 * errors or omissions and no part may be reproduced,  used  or
 * disclosed except as authorized by contract or  other written
 * permission.     The    copyright    and    the     foregoing
 * restriction   on  reproduction, use and disclosure extend to
 * all  the   media   in   which   this  information   may   be
 * embodied.
 *
 * Copyright (c) 1995 Cygnus Support
 *
 * The authors hereby grant permission to use, copy, modify, distribute,
 * and license this software and its documentation for any purpose, provided
 * that existing copyright notices are retained in all copies and that this
 * notice is included verbatim in any distributions. No written agreement,
 * license, or royalty fee is required for any of the authorized uses.
 * Modifications to this software may be copyrighted by their authors
 * and need not follow the licensing terms described here, provided that
 * the new terms are clearly indicated on the first page of each file where
 * they apply.
 */
/*
#include <ppc-asm.h>
#include <bsp.h>
*/

#include <rtems/asm.h>
#include <rtems/score/cpu.h>
#include <libcpu/io.h>
#include <ppc-asm.h>

#define H0_60X_ICE      0x8000     /*  HID0 I-Cache Enable */
#define H0_60X_DCE      0x4000     /*  HID0 D-Cache Enable */

	.file	"start.s"

	.extern	FUNC_NAME(atexit)
	.globl	FUNC_NAME(__atexit)
	.section ".sdata","aw"
	.align	2
FUNC_NAME(__atexit):			/* tell C's eabi-ctor's we have an atexit function */
	.long	FUNC_NAME(atexit)@fixup	/* and that it is to register __do_global_dtors */

	.section ".fixup","aw"
	.align	2
	.long	FUNC_NAME(__atexit)

	.text
	.globl	__rtems_entry_point
	.type	__rtems_entry_point,@function
__rtems_entry_point:

	/* Set MSR */
	/*
	 * Enable data and instruction address translation and floating point
	 */
	li      r3,MSR_IR | MSR_DR | MSR_FP
	mtmsr   r3

        /* XXX - ADD BACK IN CACHING INSTRUCTIONS */

	/* clear bss */
	lis	r6,__bss_start@h
	ori	r6,r6,__bss_start@l
	lis	r7,__bss_end@h
	ori	r7,r7,__bss_end@l

	cmplw	1,r6,r7
	bc	4,4,.Lbss_done

	subf	r8,r6,r7		/* number of bytes to zero */
	srwi	r9,r8,2			/* number of words to zero */
	mtctr	r9
	li	r0,0			/* zero to clear memory */
	addi	r6,r6,-4		/* adjust so we can use stwu */
.Lbss_loop:
	stwu	r0,4(r6)		/* zero bss */
	bdnz	.Lbss_loop

.Lbss_done:

	/* clear sbss */
	lis	r6,__sbss_start@h
	ori	r6,r6,__sbss_start@l
	lis	r7,__sbss_end@h
	ori	r7,r7,__sbss_end@l

	cmplw	1,r6,r7
	bc	4,4,.Lsbss_done

	subf	r8,r6,r7		/* number of bytes to zero */
	srwi	r9,r8,2			/* number of words to zero */
	mtctr	r9
	li	r0,0			/* zero to clear memory */
	addi	r6,r6,-4		/* adjust so we can use stwu */
.Lsbss_loop:
	stwu	r0,4(r6)		/* zero sbss */
	bdnz	.Lsbss_loop

.Lsbss_done:

	lis	sp,__stack@h
	ori	sp,sp,__stack@l

	/* set up initial stack frame */
	addi	sp,sp,-4		/* make sure we don't overwrite debug mem */
	lis	r0,0
	stw	r0,0(sp)		/* clear back chain */
	stwu	sp,-56(sp)		/* push another stack frame */

	li	r3, 0			/* command line */

	/* Let her rip */
	bl	FUNC_NAME(boot_card)

	/*
	 * This should never get reached
	 */
	/*
 	 * Return MSR to its reset state
	 */
	li      r3,0
	mtmsr   r3
	isync

	/*
	 * Call reset entry point
	 */
	lis	r3,0xfff0
	ori	r3,r3,0x100
	mtlr	r3
	blr
.Lstart:
	.size	__rtems_entry_point,.Lstart-__rtems_entry_point

	.comm	environ,4,4
