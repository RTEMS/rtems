/**
 * @file
 *
 * @ingroup RTEMSScoreCPUARM
 *
 * @brief ARM interrupt exception prologue and epilogue.
 */

/*
 * Copyright (c) 2009, 2016 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
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

#define EXCHANGE_LR r4
#define EXCHANGE_SPSR r5
#define EXCHANGE_CPSR r6
#define EXCHANGE_INT_SP r8

#define EXCHANGE_LIST {EXCHANGE_LR, EXCHANGE_SPSR, EXCHANGE_CPSR, EXCHANGE_INT_SP}
#define EXCHANGE_SIZE 16

#define SELF_CPU_CONTROL r7
#define NON_VOLATILE_SCRATCH r9

#define CONTEXT_LIST {r0, r1, r2, r3, EXCHANGE_LR, EXCHANGE_SPSR, SELF_CPU_CONTROL, r12}
#define CONTEXT_SIZE 32

.arm
.globl _ARMV4_Exception_interrupt
_ARMV4_Exception_interrupt:

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
	 * INT mode.  Ensure that stack remains 8 byte aligned.  Use register
	 * necessary for the stack alignment for the stack pointer of the
	 * interrupted context.
	 */
	stmdb	sp!, CONTEXT_LIST
	stmdb	sp!, {NON_VOLATILE_SCRATCH, lr}

#ifdef ARM_MULTILIB_VFP
	/* Save VFP context */
	vmrs	r0, FPSCR
	vstmdb	sp!, {d0-d7}
#ifdef ARM_MULTILIB_VFP_D32
	vstmdb	sp!, {d16-d31}
#endif
	stmdb	sp!, {r0, r1}
#endif /* ARM_MULTILIB_VFP */

	/* Get per-CPU control of current processor */
	GET_SELF_CPU_CONTROL	SELF_CPU_CONTROL

	/* Remember INT stack pointer */
	mov	r1, EXCHANGE_INT_SP

	/* Restore exchange registers from exchange area */
	ldmia	r1, EXCHANGE_LIST

	/* Get interrupt nest level */
	ldr	r2, [SELF_CPU_CONTROL, #PER_CPU_ISR_NEST_LEVEL]

	/* Switch stack if necessary and save original stack pointer */
	mov	NON_VOLATILE_SCRATCH, sp
	cmp	r2, #0
	moveq	sp, r1

	/* Switch to Thumb-2 instructions if necessary */
	SWITCH_FROM_ARM_TO_THUMB_2	r1

	/* Increment interrupt nest and thread dispatch disable level */
	ldr	r3, [SELF_CPU_CONTROL, #PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL]
	add	r2, #1
	add	r3, #1
	str	r2, [SELF_CPU_CONTROL, #PER_CPU_ISR_NEST_LEVEL]
	str	r3, [SELF_CPU_CONTROL, #PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL]

	/* Call BSP dependent interrupt dispatcher */
#ifdef RTEMS_PROFILING
	cmp	r2, #1
	bne	.Lskip_profiling
	BLX_TO_THUMB_1	_CPU_Counter_read
	mov	SELF_CPU_CONTROL, r0
	BLX_TO_THUMB_1	bsp_interrupt_dispatch
	BLX_TO_THUMB_1	_CPU_Counter_read
	mov	r2, r0
	mov	r1, SELF_CPU_CONTROL
	GET_SELF_CPU_CONTROL	r0
	mov	SELF_CPU_CONTROL, r0
	BLX_TO_THUMB_1	_Profiling_Outer_most_interrupt_entry_and_exit
.Lprofiling_done:
#else
	BLX_TO_THUMB_1	bsp_interrupt_dispatch
#endif

	/* Load some per-CPU variables */
	ldr	r0, [SELF_CPU_CONTROL, #PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL]
	ldrb	r1, [SELF_CPU_CONTROL, #PER_CPU_DISPATCH_NEEDED]
	ldr	r2, [SELF_CPU_CONTROL, #PER_CPU_ISR_DISPATCH_DISABLE]
	ldr	r3, [SELF_CPU_CONTROL, #PER_CPU_ISR_NEST_LEVEL]

	/* Restore stack pointer */
	mov	sp, NON_VOLATILE_SCRATCH

	/* Save CPSR in non-volatile register */
	mrs	NON_VOLATILE_SCRATCH, CPSR

	/* Decrement levels and determine thread dispatch state */
	eor	r1, r0
	sub	r0, #1
	orr	r1, r0
	orr	r1, r2
	sub	r3, #1

	/* Store thread dispatch disable and ISR nest levels */
	str	r0, [SELF_CPU_CONTROL, #PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL]
	str	r3, [SELF_CPU_CONTROL, #PER_CPU_ISR_NEST_LEVEL]

	/*
	 * Check thread dispatch necessary, ISR dispatch disable and thread
	 * dispatch disable level.
	 */
	cmp	r1, #0
	bne	.Lthread_dispatch_done

	/* Thread dispatch */
	mrs	NON_VOLATILE_SCRATCH, CPSR

.Ldo_thread_dispatch:

	/* Set ISR dispatch disable and thread dispatch disable level to one */
	mov	r0, #1
	str	r0, [SELF_CPU_CONTROL, #PER_CPU_ISR_DISPATCH_DISABLE]
	str	r0, [SELF_CPU_CONTROL, #PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL]

	/* Call _Thread_Do_dispatch(), this function will enable interrupts */
	mov	r0, SELF_CPU_CONTROL
	mov	r1, NON_VOLATILE_SCRATCH
	mov	r2, #0x80
	bic	r1, r2
	BLX_TO_THUMB_1	_Thread_Do_dispatch

	/* Disable interrupts */
	msr	CPSR, NON_VOLATILE_SCRATCH

#ifdef RTEMS_SMP
	GET_SELF_CPU_CONTROL	SELF_CPU_CONTROL
#endif

	/* Check if we have to do the thread dispatch again */
	ldrb	r0, [SELF_CPU_CONTROL, #PER_CPU_DISPATCH_NEEDED]
	cmp	r0, #0
	bne	.Ldo_thread_dispatch

	/* We are done with thread dispatching */
	mov	r0, #0
	str	r0, [SELF_CPU_CONTROL, #PER_CPU_ISR_DISPATCH_DISABLE]

.Lthread_dispatch_done:

	/* Switch to ARM instructions if necessary */
	SWITCH_FROM_THUMB_2_TO_ARM

#ifdef ARM_MULTILIB_VFP
	/* Restore VFP context */
	ldmia	sp!, {r0, r1}
#ifdef ARM_MULTILIB_VFP_D32
	vldmia	sp!, {d16-d31}
#endif
	vldmia	sp!, {d0-d7}
	vmsr	FPSCR, r0
#endif /* ARM_MULTILIB_VFP */

	/* Restore NON_VOLATILE_SCRATCH register and link register */
	ldmia	sp!, {NON_VOLATILE_SCRATCH, lr}

	/*
	 * XXX: Remember and restore stack pointer.  The data on the stack is
	 * still in use.  So the stack is now in an inconsistent state.  The
	 * FIQ handler implementation must not use this area.
	 */
	mov	r0, sp
	add	sp, #CONTEXT_SIZE

	/* Get INT mode program status register */
	mrs	r1, CPSR
	bic	r1, r1, #0x1

	/* Switch to INT mode */
	msr	CPSR_c, r1

	/* Save EXCHANGE_LR and EXCHANGE_SPSR registers to exchange area */
	stmdb	sp!, {EXCHANGE_LR, EXCHANGE_SPSR}

	/* Restore context */
	ldmia	r0, CONTEXT_LIST

	/* Set return address and program status */
	mov	lr, EXCHANGE_LR
	msr	SPSR_fsxc, EXCHANGE_SPSR

	/* Restore EXCHANGE_LR and EXCHANGE_SPSR registers from exchange area */
	ldmia	sp!, {EXCHANGE_LR, EXCHANGE_SPSR}

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
	subs	pc, lr, #4

#ifdef RTEMS_PROFILING
#ifdef __thumb2__
.thumb
#else
.arm
#endif
.Lskip_profiling:
	BLX_TO_THUMB_1	bsp_interrupt_dispatch
	b	.Lprofiling_done
#endif

#endif /* ARM_MULTILIB_ARCH_V4 */
