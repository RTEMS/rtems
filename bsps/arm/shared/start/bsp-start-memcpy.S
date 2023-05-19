/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief bsp_start_memcpy() implementation.
 */

/*
 * Copyright (C) 2008, 2011 embedded brains GmbH & Co. KG
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
#include <rtems/score/cpu.h>

	.section	".bsp_start_text", "ax"

#if defined(ARM_MULTILIB_ARCH_V4)

DEFINE_FUNCTION_ARM(bsp_start_memcpy)

	/* Return if dest == src */
	cmp	r0, r1
#ifdef __thumb__
	bxeq	lr
#else
	moveq	pc, lr
#endif

	/* Return if length is zero */
	mov	r3, #0
	cmp	r3, r2
#ifdef __thumb__
	bxeq	lr
#else
	moveq	pc, lr
#endif

	/* Save non-volatile registers */
	push	{r4-r8, lr}

	/* Copy worker routine to stack */
	adr	ip, worker_begin
	ldm	ip, {r3-r8}
	push	{r3-r8}
#ifdef ARM_MULTILIB_HAS_BARRIER_INSTRUCTIONS
	dsb
	isb
#endif

	/* Execute worker routine */
	mov	r3, #0
	mov	ip, sp
	mov	lr, pc
#ifdef __thumb__
	bx	ip
#else
	mov	pc, ip
#endif

	/* Restore stack and non-volatile registers */
	add	sp, sp, #24
	pop	{r4-r8, lr}

	/* Return */
#ifdef __thumb__
	bx	lr
#else
	mov	pc, lr
#endif

worker_begin:

	/* Worker routine */
	ldr	ip, [r1, r3]
	str	ip, [r0, r3]
	add	r3, r3, #4
	cmp	r3, r2
	bcc	worker_begin
#ifdef __thumb__
	bx	lr
#else
	mov	pc, lr
#endif

#elif defined(ARM_MULTILIB_ARCH_V7M)

	.syntax	unified

	.align	2
	.globl	bsp_start_memcpy
	.thumb
	.thumb_func
	.type	bsp_start_memcpy, %function

bsp_start_memcpy:

	/* Return if dest == src */
	cmp	r0, r1
	beq	return

	/* Return if length is zero */
	movs	r3, #0
	cmp	r3, r2
	beq	return

	/* Save non-volatile registers */
	push	{r4-r7, lr}

	/* Copy worker routine to stack */
	adr	r3, worker_begin
	ldm	r3, {r4-r7}
	push	{r4-r7}

	/* Execute worker routine */
	add	r3, sp, #1
	adds	r2, r2, #3
	bic	r2, r2, #3
	adds	r2, r2, r1
	blx	r3

	/* Restore stack and non-volatile registers */
	add	sp, sp, #16
	pop	{r4-r7, lr}

return:

	/* Return */
	bx	lr

	.align	2

worker_begin:

	/* Worker routine */
	ldr	r3, [r1], #4
	cmp	r2, r1
	str	r3, [r0], #4
	bne	worker_begin
	bx	lr

#endif /* defined(ARM_MULTILIB_ARCH_V7M) */
