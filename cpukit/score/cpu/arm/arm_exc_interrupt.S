/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreCPUARM
 *
 * @brief ARM interrupt exception prologue and epilogue.
 */

/*
 * Copyright (C) 2009, 2022 embedded brains GmbH & Co. KG
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

/*
 * The upper EXCHANGE_SIZE bytes of the INT stack area are used for data
 * exchange between INT and SVC mode.  Below of this is the actual INT stack.
 * The exchange area is only accessed if INT is disabled.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/asm.h>

#ifdef ARM_MULTILIB_ARCH_V4

#define STACK_POINTER_ADJUST r7
#define NON_VOLATILE_SCRATCH r9

#ifndef ARM_MULTILIB_HAS_STORE_RETURN_STATE

#define EXCHANGE_LR r4
#define EXCHANGE_SPSR r5
#define EXCHANGE_CPSR r6
#define EXCHANGE_INT_SP r8

#define EXCHANGE_LIST {EXCHANGE_LR, EXCHANGE_SPSR, EXCHANGE_CPSR, EXCHANGE_INT_SP}
#define EXCHANGE_SIZE 16

#define CONTEXT_LIST {r0, r1, r2, r3, EXCHANGE_LR, EXCHANGE_SPSR, NON_VOLATILE_SCRATCH, r12}
#define CONTEXT_SIZE 32

#endif /* ARM_MULTILIB_HAS_STORE_RETURN_STATE */

.arm
.globl _ARMV4_Exception_interrupt
_ARMV4_Exception_interrupt:

#ifdef ARM_MULTILIB_HAS_STORE_RETURN_STATE
	/* Prepare return from interrupt */
	sub	lr, lr, #4

	/* Save LR_irq and SPSR_irq to the SVC stack */
	srsfd	sp!, #ARM_PSR_M_SVC

	/* Switch to SVC mode */
	cps	#ARM_PSR_M_SVC

	/*
	 * Save the volatile registers, two non-volatile registers used for
	 * interrupt processing, and the link register.
	 */
	push	{r0-r3, STACK_POINTER_ADJUST, NON_VOLATILE_SCRATCH, r12, lr}
#else /* ARM_MULTILIB_HAS_STORE_RETURN_STATE */
	/* Save exchange registers to exchange area */
	stmdb	sp, EXCHANGE_LIST

	/* Set exchange registers */
	mov	EXCHANGE_LR, lr
	mrs	EXCHANGE_SPSR, SPSR
	mrs	EXCHANGE_CPSR, CPSR
	sub	EXCHANGE_INT_SP, sp, #EXCHANGE_SIZE

	/* Switch to SVC mode */
	orr	EXCHANGE_CPSR, EXCHANGE_CPSR, #0x1
	msr	CPSR_c, EXCHANGE_CPSR

	/*
	 * Save context.  We save the link register separately because it has
	 * to be restored in SVC mode.  The other registers can be restored in
	 * INT mode.  Ensure that the size of the saved registers is an
	 * integral multiple of 8 bytes.  Provide a non-volatile scratch
	 * register which may be used accross function calls.
	 */
	push	CONTEXT_LIST
	push	{STACK_POINTER_ADJUST, lr}
#endif /* ARM_MULTILIB_HAS_STORE_RETURN_STATE */

	/*
	 * On a public interface, the stack pointer must be aligned on an
	 * 8-byte boundary.  However, it may temporarily be only aligned on a
	 * 4-byte boundary.  Make sure the stack pointer is aligned on an
	 * 8-byte boundary.
	 */
	and	STACK_POINTER_ADJUST, sp, #0x4
	sub	sp, sp, STACK_POINTER_ADJUST

	/* Get per-CPU control of current processor */
	GET_SELF_CPU_CONTROL	r0

#ifdef ARM_MULTILIB_VFP
	/* Save VFP context */
	vmrs	r2, FPSCR
	vpush	{d0-d7}
#ifdef ARM_MULTILIB_VFP_D32
	vpush	{d16-d31}
#endif
	push	{r2, r3}
#endif /* ARM_MULTILIB_VFP */

#ifndef ARM_MULTILIB_HAS_STORE_RETURN_STATE
	/* Remember INT stack pointer */
	mov	r1, EXCHANGE_INT_SP

	/* Restore exchange registers from exchange area */
	ldmia	r1, EXCHANGE_LIST
#endif /* ARM_MULTILIB_HAS_STORE_RETURN_STATE */

	/* Get interrupt nest level */
	ldr	r2, [r0, #PER_CPU_ISR_NEST_LEVEL]

	/* Switch stack if necessary and save original stack pointer */
	mov	NON_VOLATILE_SCRATCH, sp
	cmp	r2, #0
#ifdef ARM_MULTILIB_HAS_STORE_RETURN_STATE
	ldreq	sp, [r0, #PER_CPU_INTERRUPT_STACK_HIGH]
#else
	moveq	sp, r1
#endif

	/* Increment interrupt nest and thread dispatch disable level */
	ldr	r3, [r0, #PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL]
	add	r2, r2, #1
	add	r3, r3, #1
	str	r2, [r0, #PER_CPU_ISR_NEST_LEVEL]
	str	r3, [r0, #PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL]

	/* Call BSP dependent interrupt dispatcher */
#ifdef RTEMS_PROFILING
	cmp	r2, #1
	bne	.Lskip_profiling
	BLX_TO_THUMB_1	_CPU_Counter_read
	push	{r0, r1}
	GET_SELF_CPU_CONTROL	r0
	BLX_TO_THUMB_1	bsp_interrupt_dispatch
	BLX_TO_THUMB_1	_CPU_Counter_read
	pop	{r1, r3}
	mov	r2, r0
	GET_SELF_CPU_CONTROL	r0
	BLX_TO_THUMB_1	_Profiling_Outer_most_interrupt_entry_and_exit
.Lprofiling_done:
#else
	BLX_TO_THUMB_1	bsp_interrupt_dispatch
#endif

	/* Get per-CPU control of current processor */
	GET_SELF_CPU_CONTROL	r0

	/* Load some per-CPU variables */
	ldr	r12, [r0, #PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL]
	ldrb	r1, [r0, #PER_CPU_DISPATCH_NEEDED]
	ldr	r2, [r0, #PER_CPU_ISR_DISPATCH_DISABLE]
	ldr	r3, [r0, #PER_CPU_ISR_NEST_LEVEL]

	/* Restore stack pointer */
	mov	sp, NON_VOLATILE_SCRATCH

	/* Decrement levels and determine thread dispatch state */
	eor	r1, r1, r12
	sub	r12, r12, #1
	orr	r1, r1, r12
	orr	r1, r1, r2
	sub	r3, r3, #1

	/* Store thread dispatch disable and ISR nest levels */
	str	r12, [r0, #PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL]
	str	r3, [r0, #PER_CPU_ISR_NEST_LEVEL]

	/*
	 * Check thread dispatch necessary, ISR dispatch disable and thread
	 * dispatch disable level.
	 */
	cmp	r1, #0
	bne	.Lthread_dispatch_done

	/* Save CPSR in non-volatile register */
	mrs	NON_VOLATILE_SCRATCH, CPSR

	/* Thread dispatch */

.Ldo_thread_dispatch:

	/* Set ISR dispatch disable and thread dispatch disable level to one */
	mov	r12, #1
	str	r12, [r0, #PER_CPU_ISR_DISPATCH_DISABLE]
	str	r12, [r0, #PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL]

	/* Call _Thread_Do_dispatch(), this function will enable interrupts */
	bic	r1, NON_VOLATILE_SCRATCH, #0x80
	BLX_TO_THUMB_1	_Thread_Do_dispatch

	/* Disable interrupts */
	msr	CPSR, NON_VOLATILE_SCRATCH

	/*
	 * Get per-CPU control of current processor.  In SMP configurations, we
	 * may run on another processor after the _Thread_Do_dispatch() call.
	 */
	GET_SELF_CPU_CONTROL	r0

	/* Check if we have to do the thread dispatch again */
	ldrb	r12, [r0, #PER_CPU_DISPATCH_NEEDED]
	cmp	r12, #0
	bne	.Ldo_thread_dispatch

	/* We are done with thread dispatching */
	mov	r12, #0
	str	r12, [r0, #PER_CPU_ISR_DISPATCH_DISABLE]

.Lthread_dispatch_done:

#ifdef ARM_MULTILIB_VFP
	/* Restore VFP context */
	pop	{r2, r3}
#ifdef ARM_MULTILIB_VFP_D32
	vpop	{d16-d31}
#endif
	vpop	{d0-d7}
	vmsr	FPSCR, r2
#endif /* ARM_MULTILIB_VFP */

	/* Undo stack pointer adjustment */
	add	sp, sp, STACK_POINTER_ADJUST

#ifdef ARM_MULTILIB_HAS_STORE_RETURN_STATE
	/*
	 * Restore the volatile registers, two non-volatile registers used for
	 * interrupt processing, and the link register.
	 */
	pop	{r0-r3, STACK_POINTER_ADJUST, NON_VOLATILE_SCRATCH, r12, lr}
#else /* ARM_MULTILIB_HAS_STORE_RETURN_STATE */
	/* Restore STACK_POINTER_ADJUST register and link register */
	pop	{STACK_POINTER_ADJUST, lr}

	/*
	 * XXX: Remember and restore stack pointer.  The data on the stack is
	 * still in use.  So the stack is now in an inconsistent state.  The
	 * FIQ handler implementation must not use this area.
	 */
	mov	r12, sp
	add	sp, #CONTEXT_SIZE

	/* Get INT mode program status register */
	mrs	r1, CPSR
	bic	r1, r1, #0x1

	/* Switch to INT mode */
	msr	CPSR_c, r1

	/* Save EXCHANGE_LR and EXCHANGE_SPSR registers to exchange area */
	push	{EXCHANGE_LR, EXCHANGE_SPSR}

	/* Restore context */
	ldmia	r12, CONTEXT_LIST

	/* Set return address and program status */
	mov	lr, EXCHANGE_LR
	msr	SPSR_fsxc, EXCHANGE_SPSR

	/* Restore EXCHANGE_LR and EXCHANGE_SPSR registers from exchange area */
	pop	{EXCHANGE_LR, EXCHANGE_SPSR}
#endif /* ARM_MULTILIB_HAS_STORE_RETURN_STATE */

#ifdef ARM_MULTILIB_HAS_LOAD_STORE_EXCLUSIVE
	/*
	 * We must clear reservations here, since otherwise compare-and-swap
	 * atomic operations with interrupts enabled may yield wrong results.
	 * A compare-and-swap atomic operation is generated by the compiler
	 * like this:
	 *
	 *   .L1:
	 *     ldrex r1, [r0]
	 *     cmp   r1, r3
	 *     bne   .L2
	 *     strex r3, r2, [r0]
	 *     cmp   r3, #0
	 *     bne   .L1
	 *   .L2:
	 *
	 * Consider the following scenario.  A thread is interrupted right
	 * before the strex.  The interrupt updates the value using a
	 * compare-and-swap sequence.  Everything is fine up to this point.
	 * The interrupt performs now a compare-and-swap sequence which fails
	 * with a branch to .L2.  The current processor has now a reservation.
	 * The interrupt returns without further strex.  The thread updates the
	 * value using the unrelated reservation of the interrupt.
	 */
	clrex
#endif

	/* Return from interrupt */
#ifdef ARM_MULTILIB_HAS_STORE_RETURN_STATE
	rfefd	sp!
#else
	subs	pc, lr, #4
#endif

#ifdef RTEMS_PROFILING
.Lskip_profiling:
	BLX_TO_THUMB_1	bsp_interrupt_dispatch
	b	.Lprofiling_done
#endif

#endif /* ARM_MULTILIB_ARCH_V4 */
