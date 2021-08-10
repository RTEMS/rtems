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
.globl	_CPU_Exception_dispatch_and_resume
.globl	_CPU_Exception_resume

#ifdef AARCH64_MULTILIB_ARCH_V8_ILP32
  #ifdef RTEMS_SMP
    #define SELF_CPU_CONTROL_GET_REG x19
  #else
    #define SELF_CPU_CONTROL_GET_REG w19
  #endif
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

/*
 * This function is expected to resume execution using the CPU_Exception_frame
 * provided in x0. This function  does not adhere to the AAPCS64 calling
 * convention because all necessary state is contained within the exception
 * frame.
 */
_CPU_Exception_resume:
/* Reset stack pointer */
	mov	sp, x0

/* call CEF restore routine (doesn't restore lr) */
	bl .pop_exception_context

/* get lr from CEF */
	ldr lr, [sp, #AARCH64_EXCEPTION_FRAME_REGISTER_LR_OFFSET]

/* drop space reserved for CEF */
	add sp, sp, #AARCH64_EXCEPTION_FRAME_SIZE

/* switch to thread stack */
	msr spsel, #1
	eret

/*
 * This function is expected to undo dispatch disabling, perform dispatch, and
 * resume execution using the CPU_Exception_frame provided in x0. This function
 * does not adhere to the AAPCS64 calling convention because all necessary
 * state is contained within the exception frame.
 */
_CPU_Exception_dispatch_and_resume:
/* Get per-CPU control of current processor */
	GET_SELF_CPU_CONTROL	SELF_CPU_CONTROL_GET_REG

/* Reset stack pointer */
	mov	sp, x0

/* Check dispatch disable and perform dispatch if necessary */
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

/* store should_skip_thread_dispatch in x22 */
	mov x22, x1

/*
 * It is now safe to assume that the source of the exception has been resolved.
 * Copy the exception frame to the thread stack to be compatible with thread
 * dispatch. This may arbitrarily clobber corruptible registers since all
 * important state is contained in the exception frame.
 *
 * No need to save current LR since this will never return to the caller.
 */
	bl .move_exception_frame_and_switch_to_thread_stack

/*
 * Check thread dispatch necessary, ISR dispatch disable and thread dispatch
 * disable level.
 */
	cmp     x22, #0
	bne     .Lno_need_thread_dispatch_resume
	bl .AArch64_Perform_Thread_Dispatch
.Lno_need_thread_dispatch_resume:
/* call CEF restore routine (doesn't restore lr) */
	bl .pop_exception_context

/* get lr from CEF */
	ldr lr, [sp, #AARCH64_EXCEPTION_FRAME_REGISTER_LR_OFFSET]

/* drop space reserved for CEF */
	add sp, sp, #AARCH64_EXCEPTION_FRAME_SIZE
	eret

/* Assumes sp currently points to the EF on the exception stack and SPSel is 0 */
.move_exception_frame_and_switch_to_thread_stack:
	mov x1, sp                                                      /* Set x1 to the current exception frame */
	msr spsel, #1                                                   /* switch to thread stack */
	ldr x0, [x1, #AARCH64_EXCEPTION_FRAME_REGISTER_SP_OFFSET]       /* Get thread SP from exception frame since it may have been updated */
	mov sp, x0
	sub sp, sp, #AARCH64_EXCEPTION_FRAME_SIZE                       /* reserve space for CEF */
	mov x0, sp                                                      /* Set x0 to the new exception frame */
	mov x20, lr                                                     /* Save LR */
	bl _AArch64_Exception_frame_copy                                /* Copy exception frame to reserved thread stack space */
	mov lr, x20                                                     /* Restore LR */
	msr spsel, #0							/* switch to exception stack */
	add sp, sp, #AARCH64_EXCEPTION_FRAME_SIZE			/* release space for CEF on exception stack */
	msr spsel, #1							/* switch to thread stack */
	ret

/*
 * Apply the exception frame to the current register status, SP points to the EF
 */
.pop_exception_context:
/* Pop daif and spsr */
	ldp x2, x3, [sp, #AARCH64_EXCEPTION_FRAME_REGISTER_DAIF_OFFSET]
/* Restore daif and spsr */
	msr DAIF, x2
	msr SPSR_EL1, x3
/* Pop FAR and ESR */
	ldp x2, x3, [sp, #AARCH64_EXCEPTION_FRAME_REGISTER_SYNDROME_OFFSET]
/* Restore ESR and FAR */
	msr ESR_EL1, x2
	msr FAR_EL1, x3
/* Pop fpcr and fpsr */
	ldp x2, x3, [sp, #AARCH64_EXCEPTION_FRAME_REGISTER_FPSR_OFFSET]
/* Restore fpcr and fpsr */
	msr FPSR, x2
	msr FPCR, x3
/* Pop VFP registers */
	ldp q0,  q1,  [sp, #(AARCH64_EXCEPTION_FRAME_REGISTER_Q0_OFFSET + 0x000)]
	ldp q2,  q3,  [sp, #(AARCH64_EXCEPTION_FRAME_REGISTER_Q0_OFFSET + 0x020)]
	ldp q4,  q5,  [sp, #(AARCH64_EXCEPTION_FRAME_REGISTER_Q0_OFFSET + 0x040)]
	ldp q6,  q7,  [sp, #(AARCH64_EXCEPTION_FRAME_REGISTER_Q0_OFFSET + 0x060)]
	ldp q8,  q9,  [sp, #(AARCH64_EXCEPTION_FRAME_REGISTER_Q0_OFFSET + 0x080)]
	ldp q10, q11, [sp, #(AARCH64_EXCEPTION_FRAME_REGISTER_Q0_OFFSET + 0x0a0)]
	ldp q12, q13, [sp, #(AARCH64_EXCEPTION_FRAME_REGISTER_Q0_OFFSET + 0x0c0)]
	ldp q14, q15, [sp, #(AARCH64_EXCEPTION_FRAME_REGISTER_Q0_OFFSET + 0x0e0)]
	ldp q16, q17, [sp, #(AARCH64_EXCEPTION_FRAME_REGISTER_Q0_OFFSET + 0x100)]
	ldp q18, q19, [sp, #(AARCH64_EXCEPTION_FRAME_REGISTER_Q0_OFFSET + 0x120)]
	ldp q20, q21, [sp, #(AARCH64_EXCEPTION_FRAME_REGISTER_Q0_OFFSET + 0x140)]
	ldp q22, q23, [sp, #(AARCH64_EXCEPTION_FRAME_REGISTER_Q0_OFFSET + 0x160)]
	ldp q24, q25, [sp, #(AARCH64_EXCEPTION_FRAME_REGISTER_Q0_OFFSET + 0x180)]
	ldp q26, q27, [sp, #(AARCH64_EXCEPTION_FRAME_REGISTER_Q0_OFFSET + 0x1a0)]
	ldp q28, q29, [sp, #(AARCH64_EXCEPTION_FRAME_REGISTER_Q0_OFFSET + 0x1c0)]
	ldp q30, q31, [sp, #(AARCH64_EXCEPTION_FRAME_REGISTER_Q0_OFFSET + 0x1e0)]
/* Pop x0-x29(fp) */
	ldp x2,  x3,  [sp, #0x10]
	ldp x4,  x5,  [sp, #0x20]
	ldp x6,  x7,  [sp, #0x30]
	ldp x8,  x9,  [sp, #0x40]
	ldp x10, x11, [sp, #0x50]
	ldp x12, x13, [sp, #0x60]
	ldp x14, x15, [sp, #0x70]
	ldp x16, x17, [sp, #0x80]
	ldp x18, x19, [sp, #0x90]
	ldp x20, x21, [sp, #0xa0]
	ldp x22, x23, [sp, #0xb0]
	ldp x24, x25, [sp, #0xc0]
	ldp x26, x27, [sp, #0xd0]
	ldp x28, x29, [sp, #0xe0]
/* Pop ELR, SP already popped */
	ldr x1, [sp, #(AARCH64_EXCEPTION_FRAME_REGISTER_SP_OFFSET + 0x8)]
/* Restore exception LR */
	msr ELR_EL1, x1
	ldp x0,  x1,  [sp, #0x00]

/* We must clear reservations to ensure consistency with atomic operations */
	clrex

	ret
