/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreCPUAArch64
 *
 * @brief Implementation of AArch64 interrupt exception handling
 *
 * This file implements the SP0 and SPx interrupt exception handlers to
 * deal with nested and non-nested interrupts.
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

.globl	_AArch64_Exception_interrupt_no_nest
.globl	_AArch64_Exception_interrupt_nest

#ifdef AARCH64_MULTILIB_ARCH_V8_ILP32
  #define SELF_CPU_CONTROL_GET_REG w19
#else
  #define SELF_CPU_CONTROL_GET_REG x19
#endif
#define SELF_CPU_CONTROL x19
#define NON_VOLATILE_SCRATCH x20

/* It's understood that CPU state is saved prior to and restored after this */
/*
 * NOTE: This function does not follow the AArch64 procedure call specification
 * because all relevant state is known to be saved in the interrupt context,
 * hence the blind usage of x19, x20, and x21
 */
.AArch64_Interrupt_Handler:
/* Get per-CPU control of current processor */
	GET_SELF_CPU_CONTROL	SELF_CPU_CONTROL_GET_REG

/* Increment interrupt nest and thread dispatch disable level */
	ldr	w2, [SELF_CPU_CONTROL, #PER_CPU_ISR_NEST_LEVEL]
	ldr	w3, [SELF_CPU_CONTROL, #PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL]
	add	w2, w2, #1
	add	w3, w3, #1
	str	w2, [SELF_CPU_CONTROL, #PER_CPU_ISR_NEST_LEVEL]
	str	w3, [SELF_CPU_CONTROL, #PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL]

/* Save LR */
	mov	x21, LR

/* Call BSP dependent interrupt dispatcher */
	bl	bsp_interrupt_dispatch

/* Restore LR */
	mov	LR, x21

/* Load some per-CPU variables */
	ldr	w0, [SELF_CPU_CONTROL, #PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL]
	ldrb	w1, [SELF_CPU_CONTROL, #PER_CPU_DISPATCH_NEEDED]
	ldr	w2, [SELF_CPU_CONTROL, #PER_CPU_ISR_DISPATCH_DISABLE]
	ldr	w3, [SELF_CPU_CONTROL, #PER_CPU_ISR_NEST_LEVEL]

/* Decrement levels and determine thread dispatch state */
	eor	w1, w1, w0
	sub	w0, w0, #1
	orr	w1, w1, w0
	orr	w1, w1, w2
	sub	w3, w3, #1

/* Store thread dispatch disable and ISR nest levels */
	str	w0, [SELF_CPU_CONTROL, #PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL]
	str	w3, [SELF_CPU_CONTROL, #PER_CPU_ISR_NEST_LEVEL]

/* Return should_skip_thread_dispatch in x0 */
	mov	x0, x1
/* Return from handler */
	ret

/* NOTE: This function does not follow the AArch64 procedure call specification
 * because all relevant state is known to be saved in the interrupt context,
 * hence the blind usage of x19, x20, and x21 */
.AArch64_Perform_Thread_Dispatch:
/* Get per-CPU control of current processor */
	GET_SELF_CPU_CONTROL	SELF_CPU_CONTROL_GET_REG

/* Thread dispatch */
	mrs	NON_VOLATILE_SCRATCH, DAIF

.Ldo_thread_dispatch:

/* Set ISR dispatch disable and thread dispatch disable level to one */
	mov	w0, #1
	str	w0, [SELF_CPU_CONTROL, #PER_CPU_ISR_DISPATCH_DISABLE]
	str	w0, [SELF_CPU_CONTROL, #PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL]

/* Save LR */
	mov	x21, LR

/* Call _Thread_Do_dispatch(), this function will enable interrupts */
	mov	x0, SELF_CPU_CONTROL
	mov	x1, NON_VOLATILE_SCRATCH
	mov	x2, #0x80
	bic	x1, x1, x2
	bl	_Thread_Do_dispatch

/* Restore LR */
	mov	LR, x21

/* Disable interrupts */
	msr	DAIF, NON_VOLATILE_SCRATCH

#ifdef RTEMS_SMP
	GET_SELF_CPU_CONTROL	SELF_CPU_CONTROL_GET_REG
#endif

/* Check if we have to do the thread dispatch again */
	ldrb	w0, [SELF_CPU_CONTROL, #PER_CPU_DISPATCH_NEEDED]
	cmp	w0, #0
	bne	.Ldo_thread_dispatch

/* We are done with thread dispatching */
	mov	w0, #0
	str	w0, [SELF_CPU_CONTROL, #PER_CPU_ISR_DISPATCH_DISABLE]

/* Return from thread dispatch */
	ret

/*
 * Must save corruptible registers and non-corruptible registers expected to be
 * used, x0 and lr expected to be already saved on the stack
 */
.macro	push_interrupt_context
/*
 * Push x1-x21 on to the stack, need 19-21 because they're modified without
 * obeying PCS
 */
	stp lr,		x1,	[sp, #-0x10]!
	stp x2,		x3,	[sp, #-0x10]!
	stp x4,		x5,	[sp, #-0x10]!
	stp x6,		x7,	[sp, #-0x10]!
	stp x8,		x9,	[sp, #-0x10]!
	stp x10,	x11,	[sp, #-0x10]!
	stp x12,	x13,	[sp, #-0x10]!
	stp x14,	x15,	[sp, #-0x10]!
	stp x16,	x17,	[sp, #-0x10]!
	stp x18,	x19,	[sp, #-0x10]!
	stp x20,	x21,	[sp, #-0x10]!
/*
 * Push q0-q31 on to the stack, need everything because parts of every register
 * are volatile/corruptible
 */
	stp q0,		q1,	[sp, #-0x20]!
	stp q2,		q3,	[sp, #-0x20]!
	stp q4,		q5,	[sp, #-0x20]!
	stp q6,		q7,	[sp, #-0x20]!
	stp q8,		q9,	[sp, #-0x20]!
	stp q10,	q11,	[sp, #-0x20]!
	stp q12,	q13,	[sp, #-0x20]!
	stp q14,	q15,	[sp, #-0x20]!
	stp q16,	q17,	[sp, #-0x20]!
	stp q18,	q19,	[sp, #-0x20]!
	stp q20,	q21,	[sp, #-0x20]!
	stp q22,	q23,	[sp, #-0x20]!
	stp q24,	q25,	[sp, #-0x20]!
	stp q26,	q27,	[sp, #-0x20]!
	stp q28,	q29,	[sp, #-0x20]!
	stp q30,	q31,	[sp, #-0x20]!
/* Get exception LR for PC and spsr */
	mrs x0, ELR_EL1
	mrs x1, SPSR_EL1
/* Push pc and spsr */
	stp x0,		x1,	[sp, #-0x10]!
/* Get fpcr and fpsr */
	mrs x0, FPSR
	mrs x1, FPCR
/* Push fpcr and fpsr */
	stp x0,		x1,	[sp, #-0x10]!
.endm

/* Must match inverse order of .push_interrupt_context */
.macro pop_interrupt_context
/* Pop fpcr and fpsr */
	ldp x0,		x1,	[sp], #0x10
/* Restore fpcr and fpsr */
	msr FPCR, x1
	msr FPSR, x0
/* Pop pc and spsr */
	ldp x0,		x1,	[sp], #0x10
/* Restore exception LR for PC and spsr */
	msr SPSR_EL1, x1
	msr ELR_EL1, x0
/* Pop q0-q31 */
	ldp q30,	q31,	[sp], #0x20
	ldp q28,	q29,	[sp], #0x20
	ldp q26,	q27,	[sp], #0x20
	ldp q24,	q25,	[sp], #0x20
	ldp q22,	q23,	[sp], #0x20
	ldp q20,	q21,	[sp], #0x20
	ldp q18,	q19,	[sp], #0x20
	ldp q16,	q17,	[sp], #0x20
	ldp q14,	q15,	[sp], #0x20
	ldp q12,	q13,	[sp], #0x20
	ldp q10,	q11,	[sp], #0x20
	ldp q8,		q9,	[sp], #0x20
	ldp q6,		q7,	[sp], #0x20
	ldp q4,		q5,	[sp], #0x20
	ldp q2,		q3,	[sp], #0x20
	ldp q0,		q1,	[sp], #0x20
/* Pop x1-x21 */
	ldp x20,	x21,	[sp], #0x10
	ldp x18,	x19,	[sp], #0x10
	ldp x16,	x17,	[sp], #0x10
	ldp x14,	x15,	[sp], #0x10
	ldp x12,	x13,	[sp], #0x10
	ldp x10,	x11,	[sp], #0x10
	ldp x8,		x9,	[sp], #0x10
	ldp x6,		x7,	[sp], #0x10
	ldp x4,		x5,	[sp], #0x10
	ldp x2,		x3,	[sp], #0x10
	ldp lr,		x1,	[sp], #0x10
/* Must clear reservations here to ensure consistency with atomic operations */
	clrex
.endm

_AArch64_Exception_interrupt_nest:

/* Execution template:
Save volatile regs on interrupt stack
Execute irq handler
Restore volatile regs from interrupt stack
Return to embedded exception vector code
*/

/* Push interrupt context */
	push_interrupt_context

/* Jump into the handler, ignore return value */
	bl .AArch64_Interrupt_Handler

/*
 * SP should be where it was pre-handler (pointing at the exception frame)
 * or something has leaked stack space
 */
/* Pop interrupt context */
	pop_interrupt_context
/* Return to vector for final cleanup */
	ret

_AArch64_Exception_interrupt_no_nest:
/* Execution template:
Save volatile registers on thread stack(some x, all q, ELR, etc.)
Switch to interrupt stack
Execute interrupt handler
Switch to thread stack
Call thread dispatch
Restore volatile registers from thread stack
Return to embedded exception vector code
*/


/* Push interrupt context */
	push_interrupt_context

/*
 * Switch to interrupt stack, interrupt dispatch may enable interrupts causing
 * nesting
 */
	msr	spsel, #0

/* Jump into the handler */
	bl .AArch64_Interrupt_Handler

/*
 * Switch back to thread stack, interrupt dispatch should disable interrupts
 * before returning
 */
	msr	spsel, #1

/*
 * Check thread dispatch necessary, ISR dispatch disable and thread dispatch
 * disable level.
 */
	cmp	x0, #0
	bne	.Lno_need_thread_dispatch
	bl .AArch64_Perform_Thread_Dispatch

.Lno_need_thread_dispatch:
/*
 * SP should be where it was pre-handler (pointing at the exception frame)
 * or something has leaked stack space
 */
/* Pop interrupt context */
	pop_interrupt_context
/* Return to vector for final cleanup */
	ret
