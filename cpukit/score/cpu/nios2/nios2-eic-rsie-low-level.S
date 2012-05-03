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

#define FRAME_OFFSET_AT 0
#define FRAME_OFFSET_R2 4
#define FRAME_OFFSET_R3 8
#define FRAME_OFFSET_R4 12
#define FRAME_OFFSET_R5 16
#define FRAME_OFFSET_R6 20
#define FRAME_OFFSET_R7 24
#define FRAME_OFFSET_R8 28
#define FRAME_OFFSET_R9 32
#define FRAME_OFFSET_R10 36
#define FRAME_OFFSET_R11 40
#define FRAME_OFFSET_R12 44
#define FRAME_OFFSET_R13 48
#define FRAME_OFFSET_R14 52
#define FRAME_OFFSET_R15 56
#define FRAME_OFFSET_RA  60
#define FRAME_OFFSET_EA  64
#define FRAME_OFFSET_ESTATUS 68
#define FRAME_OFFSET_R16 72

#define FRAME_SIZE (FRAME_OFFSET_R16 + 4)

	.set	noat
	.section	.text

	.extern	_Per_CPU_Information
	.extern	_Thread_Dispatch_disable_level

	.globl	_Nios2_ISR_Dispatch_with_shadow_preemptive

_Nios2_ISR_Dispatch_with_shadow_preemptive:

	/* Obtain stack frame */
	subi	sp, sp, FRAME_SIZE

	/* Save volatile registers */
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

	/* Save context */
	rdctl	r2, estatus
	subi	ea, ea, 4
	stw	ra, FRAME_OFFSET_RA(sp)
	stw	ea, FRAME_OFFSET_EA(sp)
	stw	r2, FRAME_OFFSET_ESTATUS(sp)

	/* Save one non-volatile register for further usage */
	stw	r16, FRAME_OFFSET_R16(sp)

	/* Save stack pointer */
	mov	r16, sp

	/* Increment ISR nest level and thread dispatch disable level */
	ldw	r9, %gprel(_Per_CPU_Information + PER_CPU_ISR_NEST_LEVEL)(gp)
	ldw	r10, %gprel(_Thread_Dispatch_disable_level)(gp)
	addi	r11, r9, 1
	addi	r10, r10, 1
	stw	r11, %gprel(_Per_CPU_Information + PER_CPU_ISR_NEST_LEVEL)(gp)
	stw	r10, %gprel(_Thread_Dispatch_disable_level)(gp)

	/* Switch to interrupt stack if necessary */
	bne	r9, zero, switch_to_interrupt_stack_done
	ldw	sp, %gprel(_Per_CPU_Information + PER_CPU_INTERRUPT_STACK_HIGH)(gp)

switch_to_interrupt_stack_done:

	/* Load high level handler address and argument */
	ldw	r12, 4(et)
	ldw	r4, 8(et)

	/* Enable interrupts */
	rdctl	r13, status
	orhi	r13, r13, 0x0080
	wrctl	status, r13

	/* Call high level handler with argument */
	callr	r12

	/* Disable interrupts */
	rdctl	r12, status
	movhi	r13, 0xff80
	subi	r13, r13, 1
	and	r12, r12, r13
	wrctl	status, r12

	/* Decrement ISR nest level and thread dispatch disable level */
	ldw	r9, %gprel(_Per_CPU_Information + PER_CPU_ISR_NEST_LEVEL)(gp)
	ldw	r10, %gprel(_Thread_Dispatch_disable_level)(gp)
	subi	r9, r9, 1
	subi	r10, r10, 1
	stw	r9, %gprel(_Per_CPU_Information + PER_CPU_ISR_NEST_LEVEL)(gp)
	stw	r10, %gprel(_Thread_Dispatch_disable_level)(gp)

	/*
	 * Restore stack pointer.  If the ISR nest level is greater than one,
	 * then this is a nop, else we switch back to the thread stack.
	 */
	mov	sp, r16

	/* Thread dispatch */
	bne	r10, zero, thread_dispatch_done
	call	_Thread_Dispatch

thread_dispatch_done:

	/* Restore volatile registers */
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
	ldw	r14, FRAME_OFFSET_R14(sp)
	ldw	r15, FRAME_OFFSET_R15(sp)

	/* Restore context */
	ldw	ra, FRAME_OFFSET_RA(sp)
	ldw	ea, FRAME_OFFSET_EA(sp)
	ldw	et, FRAME_OFFSET_ESTATUS(sp)

	/* Restore the non-volatile register */
	ldw	r16, FRAME_OFFSET_R16(sp)

	/* Release stack frame */
	addi	sp, sp, FRAME_SIZE

	/* Restore context */
	wrctl	estatus, et

	/* Return */
	eret
