/**
 * @file
 *
 * @brief bsp_start_memcpy() implementation.
 */

/*
 * Copyright (c) 2008-2011 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <rtems/asm.h>
#include <rtems/system.h>
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
