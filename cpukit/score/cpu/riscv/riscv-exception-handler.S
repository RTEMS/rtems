/**
 * @file
 *
 * @ingroup ScoreCPU
 *
 * @brief RISC-V exception support implementation.
 */

/*
 * Copyright (c) 2018 embedded brains GmbH

 * Copyright (c) 2015 University of York.
 * Hesham Almatary <hesham@alumni.york.ac.uk>
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/asm.h>
#include <rtems/score/percpu.h>

EXTERN(bsp_start_vector_table_begin)
EXTERN(_Thread_Dispatch)
PUBLIC(ISR_Handler)

	.section	.text, "ax", @progbits
	.align	2

TYPE_FUNC(ISR_Handler)
SYM(ISR_Handler):
	addi	sp, sp, -1 * 36 * CPU_SIZEOF_POINTER

	SREG	x1, (1 * CPU_SIZEOF_POINTER)(sp)
	/* Skip x2/sp */
	SREG	x3, (3 * CPU_SIZEOF_POINTER)(sp)
	SREG	x4, (4 * CPU_SIZEOF_POINTER)(sp)
	SREG	x5, (5 * CPU_SIZEOF_POINTER)(sp)
	SREG	x6, (6 * CPU_SIZEOF_POINTER)(sp)
	SREG	x7, (7 * CPU_SIZEOF_POINTER)(sp)
	SREG	x8, (8 * CPU_SIZEOF_POINTER)(sp)
	SREG	x9, (9 * CPU_SIZEOF_POINTER)(sp)
	SREG	x10, (10 * CPU_SIZEOF_POINTER)(sp)
	SREG	x11, (11 * CPU_SIZEOF_POINTER)(sp)
	SREG	x12, (12 * CPU_SIZEOF_POINTER)(sp)
	SREG	x13, (13 * CPU_SIZEOF_POINTER)(sp)
	SREG	x14, (14 * CPU_SIZEOF_POINTER)(sp)
	SREG	x15, (15 * CPU_SIZEOF_POINTER)(sp)
	SREG	x16, (16 * CPU_SIZEOF_POINTER)(sp)
	SREG	x17, (17 * CPU_SIZEOF_POINTER)(sp)
	SREG	x18, (18 * CPU_SIZEOF_POINTER)(sp)
	SREG	x19, (19 * CPU_SIZEOF_POINTER)(sp)
	SREG	x20, (20 * CPU_SIZEOF_POINTER)(sp)
	SREG	x21, (21 * CPU_SIZEOF_POINTER)(sp)
	SREG	x22, (22 * CPU_SIZEOF_POINTER)(sp)
	SREG	x23, (23 * CPU_SIZEOF_POINTER)(sp)
	SREG	x24, (24 * CPU_SIZEOF_POINTER)(sp)
	SREG	x25, (25 * CPU_SIZEOF_POINTER)(sp)
	SREG	x26, (26 * CPU_SIZEOF_POINTER)(sp)
	SREG	x27, (27 * CPU_SIZEOF_POINTER)(sp)
	SREG	x28, (28 * CPU_SIZEOF_POINTER)(sp)
	SREG	x29, (28 * CPU_SIZEOF_POINTER)(sp)
	SREG	x30, (30 * CPU_SIZEOF_POINTER)(sp)
	SREG	x31, (31 * CPU_SIZEOF_POINTER)(sp)

	/* Exception level related registers */
	csrr	a0, mstatus
	SREG	a0, (32 * CPU_SIZEOF_POINTER)(sp)
	csrr	a0, mcause
	SREG	a0, (33 * CPU_SIZEOF_POINTER)(sp)
	csrr	a1, mepc
	SREG	a1, (34 * CPU_SIZEOF_POINTER)(sp)

	/* FIXME Only handle interrupts for now (MSB = 1) */
	andi	a0, a0, 0xf

	/* Get per-CPU control of current processor */
	GET_SELF_CPU_CONTROL	s0

	/* Increment interrupt nest and thread dispatch disable level */
	lw	t0, PER_CPU_ISR_NEST_LEVEL(s0)
	lw	t1, PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL(s0)
	addi	t2, t0, 1
	addi	t1, t1, 1
	sw	t2, PER_CPU_ISR_NEST_LEVEL(s0)
	sw	t1, PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL(s0)

	/* Save interrupted task stack pointer */
	addi	t4, sp, 36 * CPU_SIZEOF_POINTER
	SREG	t4, (2 * CPU_SIZEOF_POINTER)(sp)

	/* Keep sp (Exception frame address) in s1 */
	mv	s1, sp

	/* Call the exception handler from vector table */

	/* First function arg for C handler is vector number,
		* and the second is a pointer to exception frame.
		* a0/mcause/vector number is already loaded above */
	mv	a1, sp

	/* calculate the offset */
	la	t5, bsp_start_vector_table_begin
#if	__riscv_xlen == 32
	slli	t6, a0, 2
#else	/* xlen = 64 */
	slli	t6, a0, 3
#endif
	add	t5, t5, t6
	LREG	t5, (t5)

	/* Switch to interrupt stack if necessary */
	bnez	t0, .Linterrupt_stack_switch_done
	LREG	sp, PER_CPU_INTERRUPT_STACK_HIGH(s0)
.Linterrupt_stack_switch_done:

	jalr	t5

	/* Load some per-CPU variables */
	lw	t0, PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL(s0)
	lbu	t1, PER_CPU_DISPATCH_NEEDED(s0)
	lw	t2, PER_CPU_ISR_DISPATCH_DISABLE(s0)
	lw	t3, PER_CPU_ISR_NEST_LEVEL(s0)

	/* Restore stack pointer */
	mv	sp, s1

	/* Decrement levels and determine thread dispatch state */
	xor	t1, t1, t0
	addi	t0, t0, -1
	or	t1, t1, t0
	or	t1, t1, t2
	addi	t3, t3, -1

	/* Store thread dispatch disable and ISR nest levels */
	sw	t0, PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL(s0)
	sw	t3, PER_CPU_ISR_NEST_LEVEL(s0)

	/*
	 * Check thread dispatch necessary, ISR dispatch disable and thread
	 * dispatch disable level.
	 */
	bnez	t1, .Lthread_dispatch_done

.Ldo_thread_dispatch:

	/* Set ISR dispatch disable and thread dispatch disable level to one */
	li	t0, 1
	sw	t0, PER_CPU_ISR_DISPATCH_DISABLE(s0)
	sw	t0, PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL(s0)

	/* Call _Thread_Do_dispatch(), this function will enable interrupts */
	mv	a0, s0
	li	a1, RISCV_MSTATUS_MIE
	call	_Thread_Do_dispatch

	/* Disable interrupts */
	csrrc	zero, mstatus, RISCV_MSTATUS_MIE

#ifdef RTEMS_SMP
	GET_SELF_CPU_CONTROL	s0
#endif

	/* Check if we have to do the thread dispatch again */
	lbu	t0, PER_CPU_DISPATCH_NEEDED(s0)
	bnez	t0, .Ldo_thread_dispatch

	/* We are done with thread dispatching */
	sw	zero, PER_CPU_ISR_DISPATCH_DISABLE(s0)

.Lthread_dispatch_done:

	LREG	x1, (1 * CPU_SIZEOF_POINTER)(sp)
	/* Skip sp/x2 */
	LREG	x3, (3 * CPU_SIZEOF_POINTER)(sp)
	LREG	x4, (4 * CPU_SIZEOF_POINTER)(sp)
	LREG	x5, (5 * CPU_SIZEOF_POINTER)(sp)
	LREG	x6, (6 * CPU_SIZEOF_POINTER)(sp)
	LREG	x7, (7 * CPU_SIZEOF_POINTER)(sp)
	LREG	x8, (8 * CPU_SIZEOF_POINTER)(sp)
	LREG	x9, (9 * CPU_SIZEOF_POINTER)(sp)
	LREG	x10, (10 * CPU_SIZEOF_POINTER)(sp)
	LREG	x11, (11 * CPU_SIZEOF_POINTER)(sp)
	LREG	x12, (12 * CPU_SIZEOF_POINTER)(sp)
	LREG	x13, (13 * CPU_SIZEOF_POINTER)(sp)
	LREG	x14, (14 * CPU_SIZEOF_POINTER)(sp)
	LREG	x15, (15 * CPU_SIZEOF_POINTER)(sp)
	LREG	x16, (16 * CPU_SIZEOF_POINTER)(sp)
	LREG	x17, (17 * CPU_SIZEOF_POINTER)(sp)
	LREG	x18, (18 * CPU_SIZEOF_POINTER)(sp)
	LREG	x19, (19 * CPU_SIZEOF_POINTER)(sp)
	LREG	x20, (20 * CPU_SIZEOF_POINTER)(sp)
	LREG	x21, (21 * CPU_SIZEOF_POINTER)(sp)
	LREG	x22, (22 * CPU_SIZEOF_POINTER)(sp)
	LREG	x23, (23 * CPU_SIZEOF_POINTER)(sp)
	LREG	x24, (24 * CPU_SIZEOF_POINTER)(sp)
	LREG	x25, (25 * CPU_SIZEOF_POINTER)(sp)
	LREG	x26, (26 * CPU_SIZEOF_POINTER)(sp)
	LREG	x27, (27 * CPU_SIZEOF_POINTER)(sp)
	LREG	x28, (28 * CPU_SIZEOF_POINTER)(sp)
	LREG	x29, (29 * CPU_SIZEOF_POINTER)(sp)
	LREG	x30, (30 * CPU_SIZEOF_POINTER)(sp)

	/* Load mstatus */
	LREG	x31, (32 * CPU_SIZEOF_POINTER)(sp)
	csrw	mstatus, x31
	/* Load mepc */
	LREG	x31, (34 * CPU_SIZEOF_POINTER)(sp)
	csrw	mepc, x31

	LREG	x31, (31 * CPU_SIZEOF_POINTER)(sp)

	/* Unwind exception frame */
	addi	sp, sp, 36 * CPU_SIZEOF_POINTER

	mret
