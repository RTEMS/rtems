/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2013 embedded brains GmbH & Co. KG
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

#define MORE_CONTEXT_SIZE \
  (ARM_EXCEPTION_FRAME_SIZE - ARM_EXCEPTION_FRAME_REGISTER_SP_OFFSET)

.extern _ARM_Exception_default

.globl _ARMV4_Exception_undef_default
.globl _ARMV4_Exception_swi_default
.globl _ARMV4_Exception_data_abort_default
.globl _ARMV4_Exception_pref_abort_default
.globl _ARMV4_Exception_reserved_default
.globl _ARMV4_Exception_irq_default
.globl _ARMV4_Exception_fiq_default

.section ".text"

.arm

_ARMV4_Exception_undef_default:

	/* Save context and load vector */
	sub	sp, #MORE_CONTEXT_SIZE
	stmdb	sp!, {r0-r12}
	mov	r4, #1

	b	save_more_context

_ARMV4_Exception_swi_default:

	/* Save context and load vector */
	sub	sp, #MORE_CONTEXT_SIZE
	stmdb	sp!, {r0-r12}
	mov	r4, #2

	b	save_more_context

_ARMV4_Exception_pref_abort_default:

	/* Save context and load vector */
	sub	sp, #MORE_CONTEXT_SIZE
	stmdb	sp!, {r0-r12}
	mov	r4, #3

	b	save_more_context

_ARMV4_Exception_data_abort_default:

	/* Save context and load vector */
	sub	sp, #MORE_CONTEXT_SIZE
	stmdb	sp!, {r0-r12}
	mov	r4, #4

	b	save_more_context

_ARMV4_Exception_reserved_default:

	/* Save context and load vector */
	sub	sp, #MORE_CONTEXT_SIZE
	stmdb	sp!, {r0-r12}
	mov	r4, #5

	b	save_more_context

_ARMV4_Exception_irq_default:

	/* Save context and load vector */
	sub	sp, #MORE_CONTEXT_SIZE
	stmdb	sp!, {r0-r12}
	mov	r4, #6

	b	save_more_context

_ARMV4_Exception_fiq_default:

	/* Save context and load vector */
	sub	sp, #MORE_CONTEXT_SIZE
	stmdb	sp!, {r0-r12}
	mov	r4, #7

	/*
	 * Don't enable FIQs yet. Set the FIQ disable bit in the SPSR
	 * (which we'll load into the CPSR in save_more_context).
	 */
	mrs r2, spsr
	orr r2, #ARM_PSR_F
	msr spsr_c, r2

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
	mov	r5, #0
	add	r6, sp, #ARM_EXCEPTION_FRAME_REGISTER_SP_OFFSET
	stm	r6, {r0-r5}

	/* Argument for high level handler */
	mov	r0, sp

	/* Clear VFP context pointer */
	add	r3, sp, #ARM_EXCEPTION_FRAME_VFP_CONTEXT_OFFSET
	mov	r1, #0
	str	r1, [r3]

#ifdef ARM_MULTILIB_VFP
	/* Ensure that the FPU is enabled */
	vmrs	r1, FPEXC
	tst	r1, #(1 << 30)
	beq	1f

	/* Save VFP context */
	sub	sp, #(ARM_VFP_CONTEXT_SIZE + 8)
	add	r4, sp, #8
	bic	r4, r4, #7
	str	r4, [r3]
	vmrs	r2, FPSCR
	stmia	r4!, {r1-r2}
	vstmia	r4!, {d0-d15}
#ifdef ARM_MULTILIB_VFP_D32
	vstmia	r4!, {d16-d31}
#else
	mov	r1, #0
	mov	r2, #0
	adds	r3, r4, #128
2:
	stmia	r4!, {r1-r2}
	cmp	r4, r3
	bne	2b
#endif
1:
#endif /* ARM_MULTILIB_VFP */

	/* Call high level handler */
	SWITCH_FROM_ARM_TO_THUMB	r1
	bl	_ARM_Exception_default

	/* Just in case */
twiddle:
	b	twiddle

#endif /* ARM_MULTILIB_ARCH_V4 */
