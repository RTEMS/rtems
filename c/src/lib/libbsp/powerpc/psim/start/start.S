/*
 * This is based on the mvme-crt0.S file from libgloss/rs6000.
 * crt0.S -- startup file for PowerPC systems.
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

#include <rtems/asm.h>
#include <rtems/score/cpu.h>
#include <libcpu/io.h>
#include "ppc-asm.h"

	.section ".got2","aw"
	.align	2

.LCTOC1 = .+32768

	.extern	FUNC_NAME(atexit)
	.globl	FUNC_NAME(__atexit)
	.section ".sdata","aw"
	.align	2
FUNC_NAME(__atexit):			/* tell C's eabi-ctor's we have an atexit function */
	.long	FUNC_NAME(atexit)@fixup	/* and that it is to register __do_global_dtors */

	.section ".fixup","aw"
	.align	2
	.long	FUNC_NAME(__atexit)

	.section ".got2","aw"
.Ltable = .-.LCTOC1
	.long	.LCTOC1			/* address we think .LCTOC1 is loaded at */

.Lbss_start = .-.LCTOC1
	.long	__bss_start

.Lend = .-.LCTOC1
	.long	_end

.Lstack = .-.LCTOC1			/* stack address if set by user */
	.long	__stack

	.text
.Lptr:
	.long .LCTOC1-.Laddr

	.globl	__rtems_entry_point
	.type	__rtems_entry_point,@function
__rtems_entry_point:
#if 1
	.globl	_start
	.type	_start,@function
_start:
#endif
	bl	.Laddr			/* get current address */
.Laddr:
	mflr	r4			/* real address of .Laddr */
	lwz	r5,(.Lptr-.Laddr)(r4)	/* linker generated address of .LCTOC1 */
	add	r5,r5,r4		/* correct to real pointer */
	lwz	r4,.Ltable(r5)		/* get linker's idea of where .Laddr is */
	subf	r4,r4,r5		/* calculate difference between where linked and current */

	/* clear bss */
	lwz	r6,.Lbss_start(r5)	/* calculate beginning of the BSS */
	lwz	r7,.Lend(r5)		/* calculate end of the BSS */
	add	r6,r6,r4		/* adjust pointers */
	add	r7,r7,r4

	cmplw	1,r6,r7
	bc	4,4,.Ldone

	subf	r8,r6,r7		/* number of bytes to zero */
	srwi	r9,r8,2			/* number of words to zero */
	mtctr	r9
	li	r0,0			/* zero to clear memory */
	addi	r6,r6,-4		/* adjust so we can use stwu */
.Lloop:
	stwu	r0,4(r6)		/* zero bss */
	bdnz	.Lloop

.Ldone:

	lwz	r0,.Lstack(r5)		/* stack address or 0 */
	cmplwi	1,r0,0			/* equal to 0? */
	bc	12,6,.Lnostack		/* use default stack if == 0 */
	mr	sp,r0			/* use user defined stack */

.Lnostack:
#ifdef __ALTIVEC__
	/* enable altivec; this requires the ALTIVEC user
	 * extension to be installed in the user extension
	 * slot 0!
	 */
	mfmsr r0
	oris  r0, r0, (1<<(31-16-6))
	mtmsr r0
	/*
	 * set vscr and vrsave to known values
	 */
	li    r0, 0
	mtvrsave r0
	vxor   0,0,0
	mtvscr 0
#endif
	/* set up initial stack frame */
	addi	sp,sp,-4		/* make sure we don't overwrite debug mem */
	/* align */
	li  r3, CPU_STACK_ALIGNMENT-1
	andc	sp, sp, r3
	lis	r0,0
	stw	r0,0(sp)		/* clear back chain */
	stwu	sp,-CPU_STACK_ALIGNMENT(sp)		/* push another stack frame */
	bl  FUNC_NAME(__eabi)

	/* Let her rip */
	li	r3, 0			/* command line */
	bl	FUNC_NAME(boot_card)

       .globl  FUNC_NAME(bsp_reset)
FUNC_NAME(bsp_reset):
	li  10,99			/* 0x63 */
  	sc

.Lstart:
	.size	_start,.Lstart-_start
