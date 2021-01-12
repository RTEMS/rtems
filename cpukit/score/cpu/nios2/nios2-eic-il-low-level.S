/*
 * Copyright (c) 2011-2015 embedded brains GmbH.  All rights reserved.
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

#include <rtems/score/percpu.h>
#include <rtems/score/nios2-utility.h>

#define FRAME_OFFSET_RA 0
#define FRAME_OFFSET_AT 4
#define FRAME_OFFSET_R2 8
#define FRAME_OFFSET_R3 12
#define FRAME_OFFSET_R4 16
#define FRAME_OFFSET_R5 20
#define FRAME_OFFSET_R6 24
#define FRAME_OFFSET_R7 28
#define FRAME_OFFSET_R8 32
#define FRAME_OFFSET_R9 36
#define FRAME_OFFSET_R10 40
#define FRAME_OFFSET_R11 44
#define FRAME_OFFSET_R12 48
#define FRAME_OFFSET_R13 52
#define FRAME_OFFSET_R14 56
#define FRAME_OFFSET_R15 60
#define FRAME_OFFSET_STATUS 64
#define FRAME_OFFSET_EA  68

#define FRAME_SIZE (FRAME_OFFSET_EA + 4)

	.set	noat
	.set	nobreak
	.section	.text

	.extern	_Per_CPU_Information
	.extern	_Nios2_ISR_Status_interrupts_disabled

	.globl	_Nios2_ISR_Dispatch_with_shadow_register_set

_Nios2_ISR_Dispatch_with_shadow_register_set:

	/* Load thread dispatch disable level */
	ldw	r16, %gprel(_Per_CPU_Information + PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL)(gp)

	/* Read status */
	rdctl	r18, status

	/* Load high level handler address and argument */
	ldw	r8, 4(et)
	ldw	r4, 8(et)

	/* Increment and store thread dispatch disable level */
	addi	r17, r16, 1
	stw	r17, %gprel(_Per_CPU_Information + PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL)(gp)

	/*
	 * Enable higher level interrupts.  This is safe since status.RSIE is
	 * always 0 and thread dispatching is disabled right above.  Higher
	 * priority interrupts shall not share shadow register sets with lower
	 * priority interrupts.
	 */
	ori	r5, r18, 1
	wrctl	status, r5

	/* Call high level handler with argument */
	callr	r8

	/* Load the thread dispatch necessary indicator */
	ldb	r12, %gprel(_Per_CPU_Information + PER_CPU_DISPATCH_NEEDED)(gp)

	/* Load the thread dispatch after ISR disable indicator */
	ldw	r13, %gprel(_Per_CPU_Information + PER_CPU_ISR_DISPATCH_DISABLE)(gp)

	/* Fix return address */
	subi	ea, ea, 4

	/*
	 * If the current thread dispatch disable level (r17) is one, then
	 * negate the thread dispatch necessary indicator, otherwise the value
	 * is irrelevant.  Or it with the previous thread dispatch disable
	 * level value (r16).  The r15 which will be used as a status to
	 * determine if a thread dispatch is necessary and allowed.
	 */
	xor	r12, r17, r12
	or	r15, r12, r16

	/*
	 * Get the previous register set from r18.  If it is zero, then this is
	 * the outermost interrupt.  Or it to the thread dispatch status (r15).
	 */
	andhi	r12, r18, 0x3f
	or	r15, r12, r15

	/*
	 * Or the thread dispatch after ISR disable indicator (r13) to the
	 * thread dispatch status (r15).
	 */
	or	r15, r13, r15

	/* Is a thread dispatch necessary and allowed? */
	bne	r15, zero, no_thread_dispatch

	/* Obtain stack frame in normal register set */
	rdprs	r15, sp, -FRAME_SIZE

	/* Disable thread dispatch after ISR */
	stw	r17, %gprel(_Per_CPU_Information + PER_CPU_ISR_DISPATCH_DISABLE)(gp)

	/* Save context */
	stw	sstatus, FRAME_OFFSET_STATUS(r15)
	stw	ea, FRAME_OFFSET_EA(r15)

	/* Set thread dispatch helper address */
	movhi	ea, %hiadj(thread_dispatch_helper)
	addi	ea, ea, %lo(thread_dispatch_helper)

	/* Update stack pointer in normal register set */
	wrprs	sp, r15

	/* Jump to thread dispatch helper */
	eret

no_thread_dispatch:

	/* Restore the thread dispatch disable level */
	stw	r16, %gprel(_Per_CPU_Information + PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL)(gp)

	/* Return to interrupted context */
	eret

thread_dispatch_helper:

	/* This code executes in the context of the interrupted thread */

	/* Save volatile registers */
	stw	ra, FRAME_OFFSET_RA(sp)
	stw	at, FRAME_OFFSET_AT(sp)
	stw	r2, FRAME_OFFSET_R2(sp)
	stw	r3, FRAME_OFFSET_R3(sp)
	stw	r4, FRAME_OFFSET_R4(sp)
	stw	r5, FRAME_OFFSET_R5(sp)
	stw	r6, FRAME_OFFSET_R6(sp)
	stw	r7, FRAME_OFFSET_R7(sp)
	stw	r8, FRAME_OFFSET_R8(sp)
	stw	r9, FRAME_OFFSET_R9(sp)
	stw	r10, FRAME_OFFSET_R10(sp)
	stw	r11, FRAME_OFFSET_R11(sp)
	stw	r12, FRAME_OFFSET_R12(sp)
	stw	r13, FRAME_OFFSET_R13(sp)
	stw	r14, FRAME_OFFSET_R14(sp)
	stw	r15, FRAME_OFFSET_R15(sp)

	/*
	 * Disable interrupts (1).
	 *
	 * We have the following invariants:
	 *   1. status.RSIE == 0: thread context initialization
	 *   2. status.CRS == 0: thread context initialization
	 *   3. status.PRS: arbitrary
	 *   4. status.IL < interrupt disable IL: else we would not be here
	 *   5. status.IH == 0: thread context initialization
	 *   6. status.U == 0: thread context initialization
	 *   7. status.PIE == 1: thread context initialization
	 * Thus we can use a constant to disable interrupts.
	 */
	movi	r5, %lo(_Nios2_ISR_Status_interrupts_disabled)
	wrctl	status, r5

do_thread_dispatch:

	addi	r4, gp, %gprel(_Per_CPU_Information)
	call	_Thread_Do_dispatch

	/* Restore some volatile registers */
	ldw	ra, FRAME_OFFSET_RA(sp)
	ldw	at, FRAME_OFFSET_AT(sp)
	ldw	r2, FRAME_OFFSET_R2(sp)
	ldw	r3, FRAME_OFFSET_R3(sp)
	ldw	r4, FRAME_OFFSET_R4(sp)
	ldw	r5, FRAME_OFFSET_R5(sp)
	ldw	r6, FRAME_OFFSET_R6(sp)
	ldw	r7, FRAME_OFFSET_R7(sp)
	ldw	r8, FRAME_OFFSET_R8(sp)
	ldw	r9, FRAME_OFFSET_R9(sp)
	ldw	r10, FRAME_OFFSET_R10(sp)
	ldw	r11, FRAME_OFFSET_R11(sp)
	ldw	r12, FRAME_OFFSET_R12(sp)

	/* Disable interrupts, see (1) */
	rdctl	r14, status
	movi	r15, %lo(_Nios2_ISR_Status_interrupts_disabled)
	wrctl	status, r15

	/* Load thread dispatch necessary */
	ldb	r13, %gprel(_Per_CPU_Information + PER_CPU_DISPATCH_NEEDED)(gp)

	/* Is thread dispatch necessary? */
	bne	r13, zero, prepare_thread_dispatch

	/* Enable thread dispatch after ISR */
	stw	zero, %gprel(_Per_CPU_Information + PER_CPU_ISR_DISPATCH_DISABLE)(gp)

	/* Restore remaining volatile register */
	ldw	r13, FRAME_OFFSET_R13(sp)
	ldw	r14, FRAME_OFFSET_R14(sp)
	ldw	r15, FRAME_OFFSET_R15(sp)

	/* Restore context */
	ldw	et, FRAME_OFFSET_STATUS(sp)
	ldw	ea, FRAME_OFFSET_EA(sp)

	/* Release stack frame */
	addi	sp, sp, FRAME_SIZE

	/* Restore context */
	wrctl	estatus, et

	/* Return to interrupted thread */
	eret

prepare_thread_dispatch:

	/* Disable thread dispatching */
	movi	r4, 1
	stw	r4, %gprel(_Per_CPU_Information + PER_CPU_ISR_DISPATCH_DISABLE)(gp)
	stw	r4, %gprel(_Per_CPU_Information + PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL)(gp)

	/* Set interrupt level argument for _Thread_Do_dispatch() */
	mov	r5, r15

	br	do_thread_dispatch
