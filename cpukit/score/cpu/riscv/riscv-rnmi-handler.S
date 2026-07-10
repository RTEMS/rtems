/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @addtogroup RTEMSScoreCPURISCV
 *
 * @brief RISC-V resumable NMI support implementation.
 */

/*
 * Copyright (C) 2026 Intel Corporation
 *
 * Copyright (c) 2018 embedded brains GmbH & Co. KG
 *
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

#include "riscv-exception-frame.h"

PUBLIC(_RISCV_RNMI_handler)

	/* An input section of its own lets the BSP place the RNMI vector */
	.section	.text.rnmi, "ax", @progbits
	.align	2
	.option	arch, +zicsr, +smrnmi

TYPE_FUNC(_RISCV_RNMI_handler)
SYM(_RISCV_RNMI_handler):
	/* Switch to the RNMI stack, mnscratch gets the interrupted sp */
	csrrw	sp, mnscratch, sp
	/* A full exception frame, so that the fatal path fills it in place */
	addi	sp, sp, -RISCV_EXCEPTION_FRAME_SIZE
	SREG	a0, RISCV_INTERRUPT_FRAME_A0(sp)
	csrr	a0, mnscratch
	SREG	a0, RISCV_EXCEPTION_FRAME_SP(sp)

	/* An exception in RNMI context uses the stack below this frame */
	csrw	mnscratch, sp

	/* Save */
	SREG	a1, RISCV_INTERRUPT_FRAME_A1(sp)
	SREG	s0, RISCV_INTERRUPT_FRAME_S0(sp)

	/* Check if this is a synchronous exception during RNMI */
	csrrw	a0, mncause, zero
	beqz	a0, .Lsynchronous_exception

	GET_SELF_CPU_CONTROL	a1
	lw	s0, RISCV_PER_CPU_RNMI_DEFERRED(a1)
	beqz	s0, .Lstandard_path

	/* Drop the RNMI while the deferred work of the previous one runs */
	LREG	s0, RISCV_PER_CPU_RNMI_MNCAUSE(a1)
	bnez	s0, .Ldeferred_busy

	/* Save deferred context */
	csrr	s0, mnepc
	SREG	s0, RISCV_PER_CPU_RNMI_MNEPC(a1)
	SREG	a0, RISCV_PER_CPU_RNMI_MNCAUSE(a1)

	/* Trigger a software interrupt */
	LREG	a1, RISCV_PER_CPU_CLINT_MSIP(a1)
	li	a0, 1
	sw	a0, 0(a1)

.Ldeferred_restore:
	/* Restore */
	addi	a0, sp, RISCV_EXCEPTION_FRAME_SIZE
	csrw	mnscratch, a0
	LREG	s0, RISCV_INTERRUPT_FRAME_S0(sp)
	LREG	a1, RISCV_INTERRUPT_FRAME_A1(sp)
	LREG	a0, RISCV_INTERRUPT_FRAME_A0(sp)
	LREG	sp, RISCV_EXCEPTION_FRAME_SP(sp)
	mnret

.Ldeferred_busy:
	LREG	s0, RISCV_PER_CPU_RNMI_DROPPED(a1)
	addi	s0, s0, 1
	SREG	s0, RISCV_PER_CPU_RNMI_DROPPED(a1)
	j	.Ldeferred_restore

.Lstandard_path:
	/* Save */
	SREG	a2, RISCV_INTERRUPT_FRAME_A2(sp)
	mv	s0, a1
	csrr	a1, mnstatus
	csrr	a2, mnepc
	SREG	s1, RISCV_INTERRUPT_FRAME_S1(sp)
#if __riscv_flen > 0
	frcsr	s1
#endif
	SAVE_INTERRUPT_FRAME_GPRS	sp
	SREG	a1, RISCV_INTERRUPT_FRAME_STATUS(sp)
	SREG	a2, RISCV_INTERRUPT_FRAME_EPC(sp)
#if __riscv_flen > 0
	sw	s1, RISCV_INTERRUPT_FRAME_FCSR(sp)
	SAVE_INTERRUPT_FRAME_FPRS	sp
#endif

	/* Increment interrupt nest and thread dispatch disable level */
	lw	t0, PER_CPU_ISR_NEST_LEVEL(s0)
	lw	t1, PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL(s0)
	addi	t2, t0, 1
	addi	t1, t1, 1
	sw	t2, PER_CPU_ISR_NEST_LEVEL(s0)
	sw	t1, PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL(s0)

	CLEAR_RESERVATIONS	s0

	mv	a1, s0
	call	_RISCV_RNMI_dispatch

	/* Load some per-CPU variables */
	lw	t0, PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL(s0)
	lw	t3, PER_CPU_ISR_NEST_LEVEL(s0)

	/* Decrement levels */
	addi	t0, t0, -1
	addi	t3, t3, -1

	/* Store thread dispatch disable and ISR nest levels */
	sw	t0, PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL(s0)
	sw	t3, PER_CPU_ISR_NEST_LEVEL(s0)

	/* Restore */
	LREG	a0, RISCV_INTERRUPT_FRAME_STATUS(sp)
	LREG	a1, RISCV_INTERRUPT_FRAME_EPC(sp)
	LREG	a2, RISCV_INTERRUPT_FRAME_A2(sp)
	LREG	s0, RISCV_INTERRUPT_FRAME_S0(sp)
	LREG	s1, RISCV_INTERRUPT_FRAME_S1(sp)
	RESTORE_INTERRUPT_FRAME_GPRS	sp
	csrw	mnstatus, a0
	csrw	mnepc, a1
#if __riscv_flen > 0
	lw	a0, RISCV_INTERRUPT_FRAME_FCSR(sp)
	RESTORE_INTERRUPT_FRAME_FPRS	sp
	fscsr	a0
#endif
	addi	a0, sp, RISCV_EXCEPTION_FRAME_SIZE
	csrw	mnscratch, a0
	LREG	a0, RISCV_INTERRUPT_FRAME_A0(sp)
	LREG	a1, RISCV_INTERRUPT_FRAME_A1(sp)
	LREG	sp, RISCV_EXCEPTION_FRAME_SP(sp)

	mnret

.Lsynchronous_exception:
	/* Save */
	SREG	a2, RISCV_INTERRUPT_FRAME_A2(sp)
	csrr	a0, mcause
	csrr	a1, mstatus
	csrr	a2, mepc
	SREG	s1, RISCV_INTERRUPT_FRAME_S1(sp)
#if __riscv_flen > 0
	frcsr	s1
#endif
	SAVE_INTERRUPT_FRAME_GPRS	sp
	SREG	a1, RISCV_INTERRUPT_FRAME_STATUS(sp)
	SREG	a2, RISCV_INTERRUPT_FRAME_EPC(sp)
#if __riscv_flen > 0
	sw	s1, RISCV_INTERRUPT_FRAME_FCSR(sp)
	SAVE_INTERRUPT_FRAME_FPRS	sp
#endif
	SREG	a0, RISCV_EXCEPTION_FRAME_CAUSE(sp)
	SAVE_EXCEPTION_FRAME	sp

	li	a0, 9
	mv	a1, sp
	call	_Terminate
