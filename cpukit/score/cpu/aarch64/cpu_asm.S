/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreCPUAArch64
 *
 * @brief AArch64 architecture context switch implementation.
 */

/*
 * Copyright (C) 2020 On-Line Applications Research Corporation (OAR)
 * Written by Kinsey Moore <kinsey.moore@oarcorp.com>
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

        .text

/*
 *  void _CPU_Context_switch( run_context, heir_context )
 *  void _CPU_Context_restore( run_context, heir_context )
 *
 *  This routine performs a normal non-FP context.
 *
 *  X0 = run_context    X1 = heir_context
 *
 *  This function copies the current registers to where x0 points, then
 *  restores the ones from where x1 points.
 *
 */

DEFINE_FUNCTION_AARCH64(_CPU_Context_switch)
	.globl	_CPU_Context_switch_no_return
	.set	_CPU_Context_switch_no_return, _CPU_Context_switch
#ifdef AARCH64_MULTILIB_ARCH_V8_ILP32
/* Sanitize inputs for ILP32 ABI */
	mov w0, w0
	mov w1, w1
  #ifdef RTEMS_SMP
    #define reg_2 x2
  #else
    #define reg_2 w2
  #endif
#else
#define reg_2 x2
#endif

/* Start saving context */
	GET_SELF_CPU_CONTROL	reg_2
	ldr	w3, [x2, #PER_CPU_ISR_DISPATCH_DISABLE]

	stp x19, x20, [x0]
	stp x21, x22, [x0, #0x10]
	stp x23, x24, [x0, #0x20]
	stp x25, x26, [x0, #0x30]
	stp x27, x28, [x0, #0x40]
	stp fp,  lr,  [x0, #0x50]
	mov x4,  sp
	str x4,  [x0, #0x60]

#ifdef AARCH64_MULTILIB_VFP
	add	x5, x0, #AARCH64_CONTEXT_CONTROL_D8_OFFSET
	stp d8,  d9,  [x5]
	stp d10, d11, [x5, #0x10]
	stp d12, d13, [x5, #0x20]
	stp d14, d15, [x5, #0x30]
#endif

	str	x3, [x0, #AARCH64_CONTEXT_CONTROL_ISR_DISPATCH_DISABLE]

#ifdef RTEMS_SMP
	/*
	 * The executing thread no longer executes on this processor.  Switch
	 * the stack to the temporary interrupt stack of this processor.  Mark
	 * the context of the executing thread as not executing.
	 */
	dmb	SY
	add	sp, x2, #(PER_CPU_INTERRUPT_FRAME_AREA + CPU_INTERRUPT_FRAME_SIZE)
	mov	x3, #0
	strb	w3, [x0, #AARCH64_CONTEXT_CONTROL_IS_EXECUTING_OFFSET]

.L_check_is_executing:

	/* Check the is executing indicator of the heir context */
	add	x3, x1, #AARCH64_CONTEXT_CONTROL_IS_EXECUTING_OFFSET
	ldaxrb	w4, [x3]
	cmp	x4, #0
	bne	.L_get_potential_new_heir

	/* Try to update the is executing indicator of the heir context */
	mov	x4, #1
	stlxrb	w5, w4, [x3]
	cmp	x5, #0
	bne	.L_get_potential_new_heir
	dmb	SY
#endif

/* Start restoring context */
.L_restore:
#if !defined(RTEMS_SMP) && defined(AARCH64_MULTILIB_HAS_LOAD_STORE_EXCLUSIVE)
	clrex
#endif

	ldr	x3, [x1, #AARCH64_CONTEXT_CONTROL_THREAD_ID_OFFSET]

	ldr	x4, [x1, #AARCH64_CONTEXT_CONTROL_ISR_DISPATCH_DISABLE]

#ifdef AARCH64_MULTILIB_VFP
	add	x5, x1, #AARCH64_CONTEXT_CONTROL_D8_OFFSET
	ldp d8,  d9,  [x5]
	ldp d10, d11, [x5, #0x10]
	ldp d12, d13, [x5, #0x20]
	ldp d14, d15, [x5, #0x30]
#endif

	msr	TPIDR_EL0, x3

	str	w4, [x2, #PER_CPU_ISR_DISPATCH_DISABLE]

	ldp x19, x20, [x1]
	ldp x21, x22, [x1, #0x10]
	ldp x23, x24, [x1, #0x20]
	ldp x25, x26, [x1, #0x30]
	ldp x27, x28, [x1, #0x40]
	ldp fp,  lr,  [x1, #0x50]
	ldr x4,  [x1, #0x60]
	mov sp,  x4
	ret

/*
 *  void _CPU_Context_restore( new_context )
 *
 *  This function restores the registers from where x0 points.
 *  It must match _CPU_Context_switch()
 *
 */
DEFINE_FUNCTION_AARCH64(_CPU_Context_restore)
#ifdef AARCH64_MULTILIB_ARCH_V8_ILP32
/* Sanitize input for ILP32 ABI */
	mov w0, w0
#endif

        mov     x1, x0
	GET_SELF_CPU_CONTROL	reg_2
        b       .L_restore

#ifdef RTEMS_SMP
.L_get_potential_new_heir:

	/* We may have a new heir */

	/* Read the executing and heir */
#ifdef AARCH64_MULTILIB_ARCH_V8_ILP32
	ldr	w4, [x2, #PER_CPU_OFFSET_EXECUTING]
	ldr	w5, [x2, #PER_CPU_OFFSET_HEIR]
#else
	ldr	x4, [x2, #PER_CPU_OFFSET_EXECUTING]
	ldr	x5, [x2, #PER_CPU_OFFSET_HEIR]
#endif

	/*
	 * Update the executing only if necessary to avoid cache line
	 * monopolization.
	 */
	cmp	x4, x5
	beq	.L_check_is_executing

	/* Calculate the heir context pointer */
	sub	x4, x1, x4
	add	x1, x5, x4

	/* Update the executing */
#ifdef AARCH64_MULTILIB_ARCH_V8_ILP32
	str	w5, [x2, #PER_CPU_OFFSET_EXECUTING]
#else
	str	x5, [x2, #PER_CPU_OFFSET_EXECUTING]
#endif

	b	.L_check_is_executing

DEFINE_FUNCTION_AARCH64(_AArch64_Start_multitasking)
#ifdef AARCH64_MULTILIB_ARCH_V8_ILP32
	/* Sanitize input for ILP32 ABI */
	mov	w0, w0
#endif

	mov	x1, x0
	GET_SELF_CPU_CONTROL	reg_2

	/* Switch the stack to the temporary interrupt stack of this processor */
	add	sp, x2, #(PER_CPU_INTERRUPT_FRAME_AREA + CPU_INTERRUPT_FRAME_SIZE)

	/* Enable interrupts */
	msr	DAIFClr, #0x2

	b	.L_check_is_executing
#endif
