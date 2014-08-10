/**
 * @file
 *
 * @ingroup ScoreCPU
 *
 * @brief ARM interrupt exception prologue and epilogue.
 */

/*
 * Copyright (c) 2009-2014 embedded brains GmbH.  All rights reserved.
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
#define SP_OF_INTERRUPTED_CONTEXT r9

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
	stmdb	sp!, {SP_OF_INTERRUPTED_CONTEXT, lr}

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
	GET_SELF_CPU_CONTROL	SELF_CPU_CONTROL, r1

	/* Remember INT stack pointer */
	mov	r1, EXCHANGE_INT_SP

	/* Restore exchange registers from exchange area */
	ldmia	r1, EXCHANGE_LIST

	/* Get interrupt nest level */
	ldr	r2, [SELF_CPU_CONTROL, #PER_CPU_ISR_NEST_LEVEL]

	/* Switch stack if necessary and save original stack pointer */
	mov	SP_OF_INTERRUPTED_CONTEXT, sp
	cmp	r2, #0
	moveq	sp, r1

	/* Switch to THUMB instructions if necessary */
	SWITCH_FROM_ARM_TO_THUMB	r1

	/* Increment interrupt nest and thread dispatch disable level */
	ldr	r3, [SELF_CPU_CONTROL, #PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL]
	add	r2, #1
	add	r3, #1
	str	r2, [SELF_CPU_CONTROL, #PER_CPU_ISR_NEST_LEVEL]
	str	r3, [SELF_CPU_CONTROL, #PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL]

#ifdef RTEMS_PROFILING
	cmp	r2, #1
	bne	profiling_entry_done
	bl	_CPU_Counter_read
	push	{r0, r1}
profiling_entry_done:
#endif

	/* Call BSP dependent interrupt dispatcher */
	bl	bsp_interrupt_dispatch

	/* Decrement interrupt nest and thread dispatch disable level */
	ldr	r2, [SELF_CPU_CONTROL, #PER_CPU_ISR_NEST_LEVEL]
	ldr	r3, [SELF_CPU_CONTROL, #PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL]
	sub	r2, #1
	sub	r3, #1
	str	r2, [SELF_CPU_CONTROL, #PER_CPU_ISR_NEST_LEVEL]
	str	r3, [SELF_CPU_CONTROL, #PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL]

#ifdef RTEMS_PROFILING
	cmp	r2, #0
	bne	profiling_exit_done
	bl	_CPU_Counter_read
	pop	{r1, r3}
	mov	r2, r0
	mov	r0, SELF_CPU_CONTROL
	bl	_Profiling_Outer_most_interrupt_entry_and_exit
	ldr	r3, [SELF_CPU_CONTROL, #PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL]
profiling_exit_done:
#endif

	/* Restore stack pointer */
	mov	sp, SP_OF_INTERRUPTED_CONTEXT

	/* Check thread dispatch disable level */
	cmp	r3, #0
	bne	thread_dispatch_done

	/* Check context switch necessary */
	ldrb	r1, [SELF_CPU_CONTROL, #PER_CPU_DISPATCH_NEEDED]
	cmp	r1, #0
	beq	thread_dispatch_done

        /* This aligns thread_dispatch_done on a 4 byte boundary */
#ifdef __thumb__
	nop
#endif /* __thumb__ */

	/* Thread dispatch */
	bl	_Thread_Dispatch

thread_dispatch_done:

	/* Switch to ARM instructions if necessary */
	SWITCH_FROM_THUMB_TO_ARM

#ifdef ARM_MULTILIB_VFP
	/* Restore VFP context */
	ldmia	sp!, {r0, r1}
#ifdef ARM_MULTILIB_VFP_D32
	vldmia	sp!, {d16-d31}
#endif
	vldmia	sp!, {d0-d7}
	vmsr	FPSCR, r0
#endif /* ARM_MULTILIB_VFP */

	/* Restore SP_OF_INTERRUPTED_CONTEXT register and link register */
	ldmia	sp!, {SP_OF_INTERRUPTED_CONTEXT, lr}

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

	/* Return from interrupt */
	subs	pc, lr, #4

#endif /* ARM_MULTILIB_ARCH_V4 */
