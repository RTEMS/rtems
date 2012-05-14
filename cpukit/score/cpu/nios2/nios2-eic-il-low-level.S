/*
 * Copyright (c) 2011 embedded brains GmbH.  All rights reserved.
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
	.section	.text

	.extern	_Per_CPU_Information
	.extern	_Thread_Dispatch_disable_level
	.extern	_Nios2_Thread_dispatch_disabled
	.extern	_Nios2_ISR_Status_interrupts_disabled

	.globl	_Nios2_ISR_Dispatch_with_shadow_non_preemptive

_Nios2_ISR_Dispatch_with_shadow_non_preemptive:

	/* Load thread dispatch disable level */
	ldw	r16, %gprel(_Thread_Dispatch_disable_level)(gp)

	/* Load high level handler address and argument */
	ldw	r8, 4(et)
	ldw	r4, 8(et)

	/* Increment and store thread dispatch disable level */
	addi	r9, r16, 1
	stw	r9, %gprel(_Thread_Dispatch_disable_level)(gp)

	/* Call high level handler with argument */
	callr	r8

	/* Load thread dispatch necessary */
	ldb	r12, %gprel(_Per_CPU_Information + PER_CPU_DISPATCH_NEEDED)(gp)

	/* Load Nios II specific thread dispatch disabled */
	ldw	r13, %gprel(_Nios2_Thread_dispatch_disabled)(gp)

	/* Read status */
	rdctl	r14, status

	/* Fix return address */
	subi	ea, ea, 4

	/*
	 * Restore the thread dispatch disable level.  We must do this before
	 * we return to the normal register set, because otherwise we have
	 * problems if someone deletes or restarts the interrupted thread while
	 * we are in the thread dispatch helper.
	 */
	stw	r16, %gprel(_Thread_Dispatch_disable_level)(gp)

	/* Is thread dispatch allowed? */
	bne	r16, zero, no_thread_dispatch

	/* Is thread dispatch necessary? */
	beq	r12, zero, no_thread_dispatch

	/* Is outermost interrupt? */
	andhi	r14, r14, 0x3f
	bne	r14, zero, no_thread_dispatch

	/* Is Nios II specific thread dispatch allowed? */
	bne	r13, zero, no_thread_dispatch

	/* Obtain stack frame in normal register set */
	rdprs	r15, sp, -FRAME_SIZE

	/* Disable Nios II specific thread dispatch */
	stw	r12, %gprel(_Nios2_Thread_dispatch_disabled)(gp)

	/* Save context */
	stw	sstatus, FRAME_OFFSET_STATUS(r15)
	stw	ea, FRAME_OFFSET_EA(r15)

	/* Set thread dispatch helper address */
	movhi	ea, %hiadj(thread_dispatch_helper)
	addi	ea, ea, %lo(thread_dispatch_helper)

	/* Update stack pointer in normal register set */
	wrprs	sp, r15

no_thread_dispatch:

	/*
	 * Return to thread dispatch helper, interrupted thread or interrupted
	 * lower level interrupt service routine.
	 */
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

do_thread_dispatch:

	call	_Thread_Dispatch

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
	ldw	r13, FRAME_OFFSET_R13(sp)

	/*
	 * Disable interrupts.
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
	rdctl	r14, status
	movi	r15, %lo(_Nios2_ISR_Status_interrupts_disabled)
	wrctl	status, r15

	/* Load thread dispatch necessary */
	ldb	r12, %gprel(_Per_CPU_Information + PER_CPU_DISPATCH_NEEDED)(gp)

	/* Is thread dispatch necessary? */
	bne	r12, zero, enable_interrupts_before_thread_dispatch

	/* Enable Nios II specific thread dispatch */
	stw	zero, %gprel(_Nios2_Thread_dispatch_disabled)(gp)

	/* Restore remaining volatile register */
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

enable_interrupts_before_thread_dispatch:

	/* Restore status */
	wrctl	status, r14

	br	do_thread_dispatch
