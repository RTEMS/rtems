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

.macro	ARM_DATA_CACHE_LOOP_SET_WAY CRM

	/* Get cache levels (LoC) from CLIDR */
	mrc	p15, 1, r1, c0, c0, 1
	mov	r2, r1, lsr #24
	ands	r2, r2, #0x7
	beq	5f

	/* Start with level 0 */
	mov	r3, #0

	/* Flush level specified by r3 */
1:

	/* Check cache type and skip this level if there is no data cache */
	add	r4, r3, r3, lsl #1
	lsr	r5, r1, r4
	and	r5, r5, #0x7
	cmp	r5, #2
	blt	4f

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

	/* Loop over ways */
2:
	mov	r10, r9

	/* Loop over sets */
3:
	orr	r11, r4, r7, lsl r8
	orr	r11, r11, r10, lsl r6

	/* Cache operation by set and way */
	mcr	p15, 0, r11, c7, \CRM, 2

	subs	r10, r10, #1
	bge	3b
	subs	r7, r7, #1
	bge	2b

	/* Next level */
4:
	add	r3, r3, #1
	cmp	r2, r3
	bgt	1b

	/* Done */
5:

.endm
