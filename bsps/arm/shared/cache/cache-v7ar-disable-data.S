/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2018 embedded brains GmbH
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <rtems/asm.h>

#if __ARM_ARCH >= 7 && (__ARM_ARCH_PROFILE == 65 || __ARM_ARCH_PROFILE == 82)
	.globl	rtems_cache_disable_data

	.syntax	unified
	.section	.text
	.arm

	/*
	 * This function disables the data cache on an ARMv7-AR compatible
	 * processor.
	 */
FUNCTION_ENTRY(rtems_cache_disable_data)
	/* Disable interrupts */
	mrs	r0, CPSR
	orr	r1, r0, #0x80
	msr	CPSR_fc, r1

	stmdb	sp!, {r4 - r11, lr}
	dmb

	/* Disable data cache in SCTLR */
	mrc	p15, 0, r1, c1, c0, 0
	bic	r1, r1, #0x4
	mcr	p15, 0, r1, c1, c0, 0
	isb

	/* Get cache levels (LoC) from CLIDR */
	mrc	p15, 1, r1, c0, c0, 1
	mov	r2, r1, lsr #24
	ands	r2, r2, #0x7
	beq	.Ldone

	/* Start with level 0 */
	mov	r3, #0

.Lflush_level:
	/* Flush level specified by r3 */

	/* Check cache type */
	add	r4, r3, r3, lsl #1
	lsr	r5, r1, r4
	and	r5, r5, #0x7
	cmp	r5, #2
	blt	.Lno_data_cache

	/* Read CCSIDR */
	lsl	r4, r3, #1
	mcr	p15, 2, r4, c0, c0, 0
	isb
	mrc	p15, 1, r5, c0, c0, 0

	/* Get cache line power */
	and	r6, r5, #0x7
	add	r6, r6, #4

	/* Get ways minus one */
	mov	r7, #0x3ff
	ands	r7, r7, r5, lsr #3

	/* Get way shift */
	clz	r8, r7

	/* Get sets minus one */
	mov	r9, #0x7fff
	ands	r9, r9, r5, lsr #13

.Lloop_over_ways:
	mov	r10, r9

.Lloop_over_sets:
	orr	r11, r4, r7, lsl r8
	orr	r11, r11, r10, lsl r6

	/* Clean and invalidate by set and way */
	mcr	p15, 0, r11, c7, c14, 2

	subs	r10, r10, #1
	bge	.Lloop_over_sets
	subs	r7, r7, #1
	bge	.Lloop_over_ways

.Lno_data_cache:
	/* Next level */
	add	r3, r3, #1
	cmp	r2, r3
	bgt	.Lflush_level

.Ldone:
	/* Restore interrupts */
	msr	CPSR_fc, r0

	ldmia	sp!, {r4 - r11, pc}
#endif
