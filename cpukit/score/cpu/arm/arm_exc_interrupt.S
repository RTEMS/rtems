/**
 * @file
 *
 * @ingroup ScoreCPU
 *
 * @brief ARM interrupt exception prologue and epilogue.
 */

/*
 * Copyright (c) 2009-2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
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
#include <rtems/score/percpu.h>

#ifdef ARM_MULTILIB_ARCH_V4

#define EXCHANGE_LR r4
#define EXCHANGE_SPSR r5
#define EXCHANGE_CPSR r6
#define EXCHANGE_INT_SP r7

#define EXCHANGE_LIST {EXCHANGE_LR, EXCHANGE_SPSR, EXCHANGE_CPSR, EXCHANGE_INT_SP}
#define EXCHANGE_SIZE 16

#define CONTEXT_LIST {r0, r1, r2, r3, EXCHANGE_LR, EXCHANGE_SPSR, r12}
#define CONTEXT_SIZE 28

#ifdef ARM_MULTILIB_VFP_D32
  #define VFP_CONTEXT_WITH_ALIGNMENT_SPACE (24 * 8 + 4 + 4)
#endif

.extern _Thread_Dispatch_disable_level

.extern bsp_interrupt_dispatch

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
	 * Save context.  We save the LR separately because it has to be
	 * restored in SVC mode.  The other registers can be restored in INT
	 * mode.
	 */
	stmdb	sp!, CONTEXT_LIST
	stmdb	sp!, {lr}

#ifdef ARM_MULTILIB_VFP_D32
	/* Save VFP context */
	sub	sp, #VFP_CONTEXT_WITH_ALIGNMENT_SPACE
	add	r1, sp, #4
	vmrs	r0, FPSCR
	bic	r1, r1, #7
	vstmia	r1!, {d0-d7}
	vstmia	r1!, {d16-d31}
	str	r0, [r1]
#endif

#ifdef RTEMS_SMP
	/* ISR enter */
	blx	_ISR_SMP_Enter

	/* Remember INT stack pointer */
	mov	r1, EXCHANGE_INT_SP

	/* Restore exchange registers from exchange area */
	ldmia	r1, EXCHANGE_LIST

	/* Switch stack if necessary and save original stack pointer */
	mov	r2, sp
	cmp	r0, #0
	moveq	sp, r1
	stmdb	sp!, {r2}

	/* Call BSP dependent interrupt dispatcher */
	blx	bsp_interrupt_dispatch

	/* Restore stack pointer */
	ldr	sp, [sp]

	/* ISR exit */
	blx	_ISR_SMP_Exit
	cmp	r0, #0
	beq	thread_dispatch_done

	/* Thread dispatch */
	blx	_Thread_Dispatch

thread_dispatch_done:
#else /* RTEMS_SMP */
	/* Remember INT stack pointer */
	mov	r1, EXCHANGE_INT_SP

	/* Restore exchange registers from exchange area */
	ldmia	r1, EXCHANGE_LIST

	/* Get interrupt nest level */
	ldr	r0, =ISR_NEST_LEVEL
	ldr	r2, [r0]

	/* Switch stack if necessary and save original stack pointer */
	mov	r3, sp
	cmp	r2, #0
	moveq	sp, r1
	stmdb	sp!, {r3}

	/* Switch to THUMB instructions if necessary */
	SWITCH_FROM_ARM_TO_THUMB        r1

	/* Increment interrupt nest and thread dispatch disable level */
	ldr	r1, =_Thread_Dispatch_disable_level
	ldr	r3, [r1]
	add	r2, #1
	add	r3, #1
	str	r2, [r0]
	str	r3, [r1]

	/* Call BSP dependent interrupt dispatcher */
	bl	bsp_interrupt_dispatch

	/* Decrement interrupt nest and thread dispatch disable level */
	ldr	r0, =ISR_NEST_LEVEL
	ldr	r1, =_Thread_Dispatch_disable_level
	ldr	r2, [r0]
	ldr	r3, [r1]
	sub	r2, #1
	sub	r3, #1
	str	r2, [r0]
	str	r3, [r1]

	/* Restore stack pointer */
	SWITCH_FROM_THUMB_TO_ARM
	ldr	sp, [sp]
	SWITCH_FROM_ARM_TO_THUMB	r0

	/* Check thread dispatch disable level */
	cmp	r3, #0
	bne	thread_dispatch_done

	/* Check context switch necessary */
	ldr	r0, =DISPATCH_NEEDED
	ldrb	r1, [r0]
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
#endif /* RTEMS_SMP */

#ifdef ARM_MULTILIB_VFP_D32
	/* Restore VFP context */
	add	r1, sp, #4
	bic	r1, r1, #7
	vldmia	r1!, {d0-d7}
	vldmia	r1!, {d16-d31}
	ldr	r0, [r1]
	add	sp, #VFP_CONTEXT_WITH_ALIGNMENT_SPACE
	vmsr	FPSCR, r0
#endif

	/* Restore link register */
	ldmia	sp!, {lr}

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
