/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreCPUARM
 *
 * @brief ARM data and prefetch abort exception prologue and epilogue.
 */

/*
 * Copyright (c) 2009 embedded brains GmbH & Co. KG
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/asm.h>

#ifdef ARM_MULTILIB_ARCH_V4

.extern _ARM_Exception_default

.globl _ARMV4_Exception_data_abort_set_handler
.globl _ARMV4_Exception_data_abort

.globl _ARMV4_Exception_prefetch_abort_set_handler
.globl _ARMV4_Exception_prefetch_abort

.section ".bss"

data_abort_handler:
.long 0

prefetch_abort_handler:
.long 0

.section ".text"

#ifdef __thumb__
	.thumb_func
#endif

_ARMV4_Exception_data_abort_set_handler:
	ldr	r1, =data_abort_handler
	str	r0, [r1]
#ifdef __thumb__
	bx	lr
#else
	mov	pc, lr
#endif

#ifdef __thumb__
	.thumb_func
#endif

_ARMV4_Exception_prefetch_abort_set_handler:
	ldr	r1, =prefetch_abort_handler
	str	r0, [r1]
#ifdef __thumb__
	bx	lr
#else
	mov	pc, lr
#endif

.arm

_ARMV4_Exception_prefetch_abort:

	/* Save context and load handler */
	sub	sp, #20
	stmdb	sp!, {r0-r12}
	mov	r4, #3
	ldr	r6, =prefetch_abort_handler

	b	save_more_context

_ARMV4_Exception_data_abort:

	/* Save context and load handler */
	sub	sp, #20
	stmdb	sp!, {r0-r12}
	mov	r4, #4
	ldr	r6, =data_abort_handler

save_more_context:

	/* Save more context */
	mov	r2, lr
	mrs	r3, spsr
	mrs	r7, cpsr
	orr	r5, r3, #ARM_PSR_I
	bic	r5, #ARM_PSR_T
	msr	cpsr, r5
	mov	r0, sp
	mov	r1, lr
	msr	cpsr, r7
	add	r5, sp, #72
	stmdb	r5!, {r0-r4}

	/* Call high level handler */
	ldr	r2, [r6]
	cmp	r2, #0
	ldreq	r2, =_ARM_Exception_default
	mov	r0, sp
#ifndef __thumb__
	mov	lr, pc
	mov	pc, r2
#else /* __thumb__ */
	SWITCH_FROM_ARM_TO_THUMB	r1
	bl	call_handler
	SWITCH_FROM_THUMB_TO_ARM
#endif /* __thumb__ */

	/* Restore context */
	ldmia	r5!, {r0-r4}
	mov	lr, r2
	msr	spsr, r3
	ldmia	sp!, {r0-r12}
	add	sp, #20

	/* Return from interrupt */
	subs	pc, lr, #8

#ifdef __thumb__
.thumb
call_handler:
	bx	r2
#endif /* __thumb__ */

#endif /* ARM_MULTILIB_ARCH_V4 */
