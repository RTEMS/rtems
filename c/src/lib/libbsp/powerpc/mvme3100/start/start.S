/*
 *  start.S :	  RTEMS entry point
 *
 *  Copyright (C) 1999 Eric Valette. valette@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 * 
 *  Modified for mvme3100 by T. Straumann, 2007.
 *
 *  $Id$
 *
 */

#include <rtems/asm.h>
#include <rtems/score/cpu.h>
#include <rtems/powerpc/powerpc.h>

#include <bspopts.h>

#define SYNC \
	sync; \
	isync

#define KERNELBASE	0x0

	.text
	.globl	__rtems_entry_point
	.type	__rtems_entry_point,@function
__rtems_entry_point:
	mr	r31,r3
	mr	r30,r4
	mr	r29,r5
	mr	r28,r6
	mr	r27,r7

	/* Use MotLoad's TLB setup for now; caches are on already */
	bl  __eabi	/* setup EABI and SYSV environment */
	bl	zero_bss
	/*
	 * restore original args
	 */
	mr	r3,r31
	mr	r4,r30
	mr	r5,r29
	mr	r6,r28
	mr	r7,r27
	bl	save_boot_params
	/*
	 * stack = &__rtems_end + 4096
	 */
	addis	r9,r0, __rtems_end+(4096-PPC_MINIMUM_STACK_FRAME_SIZE)@ha
	addi	r9,r9, __rtems_end+(4096-PPC_MINIMUM_STACK_FRAME_SIZE)@l
	/* align down to 16-bytes */
	li  r5, (CPU_STACK_ALIGNMENT - 1)
	andc r1, r9, r5
	/*
	 * We are now in a environment that is totally independent from 
	 * bootloader setup.
	 */
	lis	r5,environ@ha
	la	r5,environ@l(r5)	/* environp */
	li	r4, 0			/* argv */
	li	r3, 0			/* argc */
	bl	boot_card
	/* point of no return: reset board here ? */
