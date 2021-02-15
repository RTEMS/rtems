/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreCPUAArch64
 *
 * @brief Implementation of AArch64 exception vector table.
 *
 * This file implements the AArch64 exception vector table and its embedded
 * jump handlers along with the code necessary to call higher level C handlers.
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

.extern _AArch64_Exception_default

.globl	bsp_start_vector_table_begin
.globl	bsp_start_vector_table_end
.globl	bsp_start_vector_table_size
.globl	bsp_vector_table_size

.section ".text"

/*
 * This is the exception vector table and the pointers to the default
 * exceptions handlers. Each vector in the table has space for up to 32
 * instructions. The space of the last two instructions in each vector is used
 * for the exception handler pointer.
 *
 * The operation of all exceptions is as follows:
 * * An exception occurs
 * * A vector is chosen based on the exception type and machine state
 * * Execution begins at the chosen vector
 * * X0 and LR are pushed onto the current stack
 * * An unconditional branch and link is taken to the next instruction to get
 *   the PC
 * * The exception handler pointer (EHP) is retrieved from the current vector using
 *   the PC
 * * Branch and link to the EHP
 * * X0 and LR are popped from the current stack after returning from the EHP
 * * The exception returns to the previous execution state
 */

/*
 * TODO(kmoore) The current implementation here assumes that SP is not
 * misaligned.
 */
	.macro	JUMP_HANDLER_SHORT
/* Mask to use in BIC, lower 7 bits */
	mov x0, #0x7f
/* LR contains PC, mask off to the base of the current vector */
	bic x0,	lr,	x0
/* Load address from the last word in the vector */
	ldr x0,	[x0,	#0x78]
/*
 * Branch and link to the address in x0. There is no reason to save the current
 * LR since it has already been saved and the current contents are junk.
 */
	blr x0
/* Pop x0,lr from stack */
	ldp x0,	lr,	[sp],	#0x10
/* Return from exception */
	eret
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	.endm

	.macro	JUMP_HANDLER
	JUMP_HANDLER_SHORT
	nop
	.endm

	.macro	JUMP_TARGET_SP0
/* Takes up the space of 2 instructions */
#ifdef AARCH64_MULTILIB_ARCH_V8_ILP32
	.word .print_exception_dump_sp0
	.word 0x0
#else
	.dword .print_exception_dump_sp0
#endif
	.endm

	.macro	JUMP_TARGET_SPx
/* Takes up the space of 2 instructions */
#ifdef AARCH64_MULTILIB_ARCH_V8_ILP32
	.word .print_exception_dump_spx
	.word 0x0
#else
	.dword .print_exception_dump_spx
#endif
	.endm

bsp_start_vector_table_begin:
.balign 0x800
Vector_table_el3:
/*
 * The exception handler for synchronous exceptions from the current EL
 * using SP0.
 */
curr_el_sp0_sync:
	stp x0,	lr,	[sp, #-0x10]!	/* Push x0,lr on to the stack */
	bl curr_el_sp0_sync_get_pc	/* Get current execution address */
curr_el_sp0_sync_get_pc:		/* The current PC is now in LR */
	JUMP_HANDLER
	JUMP_TARGET_SP0
.balign 0x80
/* The exception handler for IRQ exceptions from the current EL using SP0. */
curr_el_sp0_irq:
	stp x0,	lr,	[sp, #-0x10]!	/* Push x0,lr on to the stack */
	bl curr_el_sp0_irq_get_pc	/* Get current execution address */
curr_el_sp0_irq_get_pc:			/* The current PC is now in LR */
	JUMP_HANDLER
	JUMP_TARGET_SP0
.balign 0x80
/* The exception handler for FIQ exceptions from the current EL using SP0. */
curr_el_sp0_fiq:
	stp x0,	lr,	[sp, #-0x10]!	/* Push x0,lr on to the stack */
	bl curr_el_sp0_fiq_get_pc	/* Get current execution address */
curr_el_sp0_fiq_get_pc:			/* The current PC is now in LR */
	JUMP_HANDLER
	JUMP_TARGET_SP0
.balign 0x80
/*
 * The exception handler for system error exceptions from the current EL using
 * SP0.
 */
curr_el_sp0_serror:
	stp x0,	lr,	[sp, #-0x10]!	/* Push x0,lr on to the stack */
	bl curr_el_sp0_serror_get_pc	/* Get current execution address */
curr_el_sp0_serror_get_pc:		/* The current PC is now in LR */
	JUMP_HANDLER
	JUMP_TARGET_SP0
.balign 0x80
/*
 * The exception handler for synchronous exceptions from the current EL using
 * the current SP.
 */
curr_el_spx_sync:
	msr SCTLR_EL1, XZR
	stp x0,	lr,	[sp, #-0x10]!	/* Push x0,lr on to the stack */
	bl curr_el_spx_sync_get_pc	/* Get current execution address */
curr_el_spx_sync_get_pc:		/* The current PC is now in LR */
/* Use short jump handler since this has an extra instruction to clear SCTLR */
	JUMP_HANDLER_SHORT
	JUMP_TARGET_SPx
.balign 0x80
/*
 * The exception handler for IRQ exceptions from the current EL using the
 * current SP.
 */
curr_el_spx_irq:
	stp x0,	lr,	[sp, #-0x10]!	/* Push x0,lr on to the stack */
	bl curr_el_spx_irq_get_pc	/* Get current execution address */
curr_el_spx_irq_get_pc:			/* The current PC is now in LR */
	JUMP_HANDLER
	JUMP_TARGET_SPx
.balign 0x80
/*
 * The exception handler for FIQ exceptions from the current EL using the
 * current SP.
 */
curr_el_spx_fiq:
	stp x0,	lr,	[sp, #-0x10]!	/* Push x0,lr on to the stack */
	bl curr_el_spx_fiq_get_pc	/* Get current execution address */
curr_el_spx_fiq_get_pc:			/* The current PC is now in LR */
	JUMP_HANDLER
	JUMP_TARGET_SPx
.balign 0x80
/*
 * The exception handler for system error exceptions from the current EL using
 * the current SP.
 */
curr_el_spx_serror:
	stp x0,	lr,	[sp, #-0x10]!	/* Push x0,lr on to the stack */
	bl curr_el_spx_serror_get_pc	/* Get current execution address */
curr_el_spx_serror_get_pc:		/* The current PC is now in LR */
	JUMP_HANDLER
	JUMP_TARGET_SPx
.balign 0x80
/*
 * The exception handler for synchronous exceptions from a lower EL (AArch64).
 */
lower_el_aarch64_sync:
	stp x0,	lr,	[sp, #-0x10]!	/* Push x0,lr on to the stack */
	bl lower_el_aarch64_sync_get_pc	/* Get current execution address */
lower_el_aarch64_sync_get_pc:		/* The current PC is now in LR */
	JUMP_HANDLER
	JUMP_TARGET_SPx
.balign 0x80
/* The exception handler for IRQ exceptions from a lower EL (AArch64). */
lower_el_aarch64_irq:
	stp x0,	lr,	[sp, #-0x10]!	/* Push x0,lr on to the stack */
	bl lower_el_aarch64_irq_get_pc	/* Get current execution address */
lower_el_aarch64_irq_get_pc:		/* The current PC is now in LR */
	JUMP_HANDLER
	JUMP_TARGET_SPx
.balign 0x80
/* The exception handler for FIQ exceptions from a lower EL (AArch64). */
lower_el_aarch64_fiq:
	stp x0,	lr,	[sp, #-0x10]!	/* Push x0,lr on to the stack */
	bl lower_el_aarch64_fiq_get_pc	/* Get current execution address */
lower_el_aarch64_fiq_get_pc:		/* The current PC is now in LR */
	JUMP_HANDLER
	JUMP_TARGET_SPx
.balign 0x80
/*
 * The exception handler for system error exceptions from a lower EL(AArch64).
 */
lower_el_aarch64_serror:
/* Push x0,lr on to the stack */
	stp x0,	lr,	[sp, #-0x10]!
/* Get current execution address */
	bl lower_el_aarch64_serror_get_pc
lower_el_aarch64_serror_get_pc:		/* The current PC is now in LR */
	JUMP_HANDLER
	JUMP_TARGET_SPx
.balign 0x80
/*
 * The exception handler for the synchronous exception from a lower EL(AArch32).
 */
lower_el_aarch32_sync:
	stp x0,	lr,	[sp, #-0x10]!	/* Push x0,lr on to the stack */
	bl lower_el_aarch32_sync_get_pc	/* Get current execution address */
lower_el_aarch32_sync_get_pc:		/* The current PC is now in LR */
	JUMP_HANDLER
	JUMP_TARGET_SPx
.balign 0x80
/* The exception handler for the IRQ exception from a lower EL (AArch32). */
lower_el_aarch32_irq:
	stp x0,	lr,	[sp, #-0x10]!	/* Push x0,lr on to the stack */
	bl lower_el_aarch32_irq_get_pc	/* Get current execution address */
lower_el_aarch32_irq_get_pc:		/* The current PC is now in LR */
	JUMP_HANDLER
	JUMP_TARGET_SPx
.balign 0x80
/* The exception handler for the FIQ exception from a lower EL (AArch32). */
lower_el_aarch32_fiq:
	stp x0,	lr,	[sp, #-0x10]!	/* Push x0,lr on to the stack */
	bl lower_el_aarch32_fiq_get_pc	/* Get current execution address */
lower_el_aarch32_fiq_get_pc:		/* The current PC is now in LR */
	JUMP_HANDLER
	JUMP_TARGET_SPx
.balign 0x80
/*
 * The exception handler for the system error exception from a lower EL
 * (AArch32).
 */
lower_el_aarch32_serror:
/* Push x0,lr on to the stack */
	stp x0,	lr,	[sp, #-0x10]!
/* Get current execution address */
	bl lower_el_aarch32_serror_get_pc
lower_el_aarch32_serror_get_pc	:		/* The current PC is now in LR */
	JUMP_HANDLER
	JUMP_TARGET_SPx

bsp_start_vector_table_end:

	.set	bsp_start_vector_table_size, bsp_start_vector_table_end - bsp_start_vector_table_begin
	.set	bsp_vector_table_size, bsp_start_vector_table_size

/*
 * This involves switching a few things around. the real x0 and lr are on SPx
 * and need to be retrieved while the lr upon entry contains the pointer into
 * the AArch64 vector table
 */
.print_exception_dump_spx:
/* Switch to exception stack (SP0) */
	msr spsel, #0
/* Save space for exception context */
	sub sp, sp, #AARCH64_EXCEPTION_FRAME_SIZE
/*
 * Push exception vector, LR currently points into the actual exception vector
 * table
 */
	and lr, lr, #0x780
	lsr lr, lr, #7
	str lr, [sp, #AARCH64_EXCEPTION_FRAME_REGISTER_VECTOR_OFFSET]
/* Pop x0,lr from stack, saved by generic handler */
/*
 * This modifies the stack pointer back to the pre-vector-handler value which is
 * safe because this will never return
 */
	msr spsel, #1
	ldp x0,	lr, [sp], #0x10
	msr spsel, #0
/* Save LR */
	str lr, [sp, #AARCH64_EXCEPTION_FRAME_REGISTER_LR_OFFSET]
/* Push the start of the context */
	bl .push_exception_context_start
/* Save original sp in x0 for .push_exception_context_finish */
	msr spsel, #1
	mov x0, sp
	msr spsel, #0
/* Push the remainder of the context */
	bl .push_exception_context_finish
/* Save sp into x0 for handler */
	mov x0, sp
/* Jump into the handler */
	bl _AArch64_Exception_default

	/* Just in case */
	b	twiddle

.print_exception_dump_sp0:
/* Save space for exception context */
	sub sp, sp, #AARCH64_EXCEPTION_FRAME_SIZE
/*
 * Push exception vector, LR currently points into the actual exception vector
 */
	and lr, lr, #0x780
	lsr lr, lr, #7
	str lr, [sp, #AARCH64_EXCEPTION_FRAME_REGISTER_VECTOR_OFFSET]
/* Get x0,lr from stack, saved by generic handler */
	add sp, sp, #AARCH64_EXCEPTION_FRAME_SIZE
	ldp x0,	lr, [sp]
	sub sp, sp, #AARCH64_EXCEPTION_FRAME_SIZE
/* Save LR */
	str lr, [sp, #AARCH64_EXCEPTION_FRAME_REGISTER_LR_OFFSET]
/* Push the start of the context */
	bl .push_exception_context_start
/* Save original sp in x0 for .push_exception_context_finish */
	add x0, sp, #(AARCH64_EXCEPTION_FRAME_SIZE + 0x10)
/* Push the remainder of the context */
	bl .push_exception_context_finish
/* Save sp (exception frame) into x0 for handler */
	mov x0, sp
/* Jump into the handler */
	bl _AArch64_Exception_default

	/* Just in case */
twiddle:
	b	twiddle

/* Assumes SP is at the base of the context and LR has already been pushed */
.push_exception_context_start:
/* Push x0-x29(fp) */
	stp x0,  x1,  [sp, #0x00]
	stp x2,  x3,  [sp, #0x10]
	stp x4,  x5,  [sp, #0x20]
	stp x6,  x7,  [sp, #0x30]
	stp x8,  x9,  [sp, #0x40]
	stp x10, x11, [sp, #0x50]
	stp x12, x13, [sp, #0x60]
	stp x14, x15, [sp, #0x70]
	stp x16, x17, [sp, #0x80]
	stp x18, x19, [sp, #0x90]
	stp x20, x21, [sp, #0xa0]
	stp x22, x23, [sp, #0xb0]
	stp x24, x25, [sp, #0xc0]
	stp x26, x27, [sp, #0xd0]
	stp x28, x29, [sp, #0xe0]
	ret

/* Expects original SP to be stored in x0 */
.push_exception_context_finish:
/* Get exception LR for PC */
	mrs x1, ELR_EL1
/* Push sp and pc */
	stp x0, x1, [sp, #AARCH64_EXCEPTION_FRAME_REGISTER_SP_OFFSET]
/* Get daif and spsr */
	mrs x0, DAIF
	mrs x1, SPSR_EL1
/* Push daif and spsr */
	stp x0, x1, [sp, #AARCH64_EXCEPTION_FRAME_REGISTER_DAIF_OFFSET]
/* Get ESR and FAR */
	mrs x0, ESR_EL1
	mrs x1, FAR_EL1
/* Push FAR and ESR */
	stp x0, x1, [sp, #AARCH64_EXCEPTION_FRAME_REGISTER_SYNDROME_OFFSET]
/* Get fpcr and fpsr */
	mrs x0, FPSR
	mrs x1, FPCR
/* Push fpcr and fpsr */
	stp x0, x1, [sp, #AARCH64_EXCEPTION_FRAME_REGISTER_FPSR_OFFSET]
/* Push VFP registers */
	stp q0,  q1,  [sp, #(AARCH64_EXCEPTION_FRAME_REGISTER_Q0_OFFSET + 0x000)]
	stp q2,  q3,  [sp, #(AARCH64_EXCEPTION_FRAME_REGISTER_Q0_OFFSET + 0x020)]
	stp q4,  q5,  [sp, #(AARCH64_EXCEPTION_FRAME_REGISTER_Q0_OFFSET + 0x040)]
	stp q6,  q7,  [sp, #(AARCH64_EXCEPTION_FRAME_REGISTER_Q0_OFFSET + 0x060)]
	stp q8,  q9,  [sp, #(AARCH64_EXCEPTION_FRAME_REGISTER_Q0_OFFSET + 0x080)]
	stp q10, q11, [sp, #(AARCH64_EXCEPTION_FRAME_REGISTER_Q0_OFFSET + 0x0a0)]
	stp q12, q13, [sp, #(AARCH64_EXCEPTION_FRAME_REGISTER_Q0_OFFSET + 0x0c0)]
	stp q14, q15, [sp, #(AARCH64_EXCEPTION_FRAME_REGISTER_Q0_OFFSET + 0x0e0)]
	stp q16, q17, [sp, #(AARCH64_EXCEPTION_FRAME_REGISTER_Q0_OFFSET + 0x100)]
	stp q18, q19, [sp, #(AARCH64_EXCEPTION_FRAME_REGISTER_Q0_OFFSET + 0x120)]
	stp q20, q21, [sp, #(AARCH64_EXCEPTION_FRAME_REGISTER_Q0_OFFSET + 0x140)]
	stp q22, q23, [sp, #(AARCH64_EXCEPTION_FRAME_REGISTER_Q0_OFFSET + 0x160)]
	stp q24, q25, [sp, #(AARCH64_EXCEPTION_FRAME_REGISTER_Q0_OFFSET + 0x180)]
	stp q26, q27, [sp, #(AARCH64_EXCEPTION_FRAME_REGISTER_Q0_OFFSET + 0x1a0)]
	stp q28, q29, [sp, #(AARCH64_EXCEPTION_FRAME_REGISTER_Q0_OFFSET + 0x1c0)]
	stp q30, q31, [sp, #(AARCH64_EXCEPTION_FRAME_REGISTER_Q0_OFFSET + 0x1e0)]
/* Done, return to exception handler */
	ret

/*
 * Apply the exception frame to the current register status, SP points to the EF
 */
.pop_exception_context_and_ret:
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
/* Restore LR */
	ldr lr, [sp, #AARCH64_EXCEPTION_FRAME_REGISTER_LR_OFFSET]
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
/* Pop sp (ignored since sp should be shortly restored anyway) and ELR */
	ldp x0, x1, [sp, #AARCH64_EXCEPTION_FRAME_REGISTER_SP_OFFSET]
/* Restore exception LR */
	msr ELR_EL1, x1
	ldp x0,  x1,  [sp, #0x00]
	add sp, sp, #AARCH64_EXCEPTION_FRAME_SIZE

/* We must clear reservations to ensure consistency with atomic operations */
	clrex

	ret
