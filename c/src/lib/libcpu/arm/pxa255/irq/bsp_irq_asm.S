/*
 * PXA255 Interrupt handler by Yang Xi <hiyangxi@gmail.com>
 * Copyright (c) 2004 by Jay Monkman <jtm@lopgindog.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#define __asm__

        .globl bsp_interrupt_dispatch
bsp_interrupt_dispatch :
/*
 * Look at interrupt status register to determine source.
 * From source, determine offset into expanded vector table
 * and load vector into r0 and handler address into r1.
 */
	ldr	r0,=0x40d00000
	ldr	r1,[r0]
	clz	r0,r1
	cmp	r0,#32
	moveq   pc,lr 	/*All zeros*/
	mov 	r2,#31
	sub	r0,r2,r0
	ldr	r2,=IRQ_table
	add	r2,r2,r0,LSL #2
	ldr	r1,[r2]
	mov	pc,r1


