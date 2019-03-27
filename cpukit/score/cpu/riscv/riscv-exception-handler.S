/**
 * @file
 *
 * @addtogroup RTEMSScoreCPURISCV
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

PUBLIC(_RISCV_Exception_handler)

	.section	.text, "ax", @progbits
	.align	2

TYPE_FUNC(_RISCV_Exception_handler)
SYM(_RISCV_Exception_handler):
	addi	sp, sp, -CPU_INTERRUPT_FRAME_SIZE

	/* Save */
	SREG	a0, RISCV_INTERRUPT_FRAME_A0(sp)
	SREG	a1, RISCV_INTERRUPT_FRAME_A1(sp)
	SREG	a2, RISCV_INTERRUPT_FRAME_A2(sp)
	SREG	s0, RISCV_INTERRUPT_FRAME_S0(sp)
	csrr	a0, mcause
	csrr	a1, mstatus
	csrr	a2, mepc
	GET_SELF_CPU_CONTROL	s0
	SREG	s1, RISCV_INTERRUPT_FRAME_S1(sp)
#if __riscv_flen > 0
	frcsr	s1
#endif
	SREG	ra, RISCV_INTERRUPT_FRAME_RA(sp)
	SREG	a3, RISCV_INTERRUPT_FRAME_A3(sp)
	SREG	a4, RISCV_INTERRUPT_FRAME_A4(sp)
	SREG	a5, RISCV_INTERRUPT_FRAME_A5(sp)
	SREG	a6, RISCV_INTERRUPT_FRAME_A6(sp)
	SREG	a7, RISCV_INTERRUPT_FRAME_A7(sp)
	SREG	t0, RISCV_INTERRUPT_FRAME_T0(sp)
	SREG	t1, RISCV_INTERRUPT_FRAME_T1(sp)
	SREG	t2, RISCV_INTERRUPT_FRAME_T2(sp)
	SREG	t3, RISCV_INTERRUPT_FRAME_T3(sp)
	SREG	t4, RISCV_INTERRUPT_FRAME_T4(sp)
	SREG	t5, RISCV_INTERRUPT_FRAME_T5(sp)
	SREG	t6, RISCV_INTERRUPT_FRAME_T6(sp)
	SREG	a1, RISCV_INTERRUPT_FRAME_MSTATUS(sp)
	SREG	a2, RISCV_INTERRUPT_FRAME_MEPC(sp)
#if __riscv_flen > 0
	sw	s1, RISCV_INTERRUPT_FRAME_FCSR(sp)
	FSREG	ft0, RISCV_INTERRUPT_FRAME_FT0(sp)
	FSREG	ft1, RISCV_INTERRUPT_FRAME_FT1(sp)
	FSREG	ft2, RISCV_INTERRUPT_FRAME_FT2(sp)
	FSREG	ft3, RISCV_INTERRUPT_FRAME_FT3(sp)
	FSREG	ft4, RISCV_INTERRUPT_FRAME_FT4(sp)
	FSREG	ft5, RISCV_INTERRUPT_FRAME_FT5(sp)
	FSREG	ft6, RISCV_INTERRUPT_FRAME_FT6(sp)
	FSREG	ft7, RISCV_INTERRUPT_FRAME_FT7(sp)
	FSREG	ft8, RISCV_INTERRUPT_FRAME_FT8(sp)
	FSREG	ft9, RISCV_INTERRUPT_FRAME_FT9(sp)
	FSREG	ft10, RISCV_INTERRUPT_FRAME_FT10(sp)
	FSREG	ft11, RISCV_INTERRUPT_FRAME_FT11(sp)
	FSREG	fa0, RISCV_INTERRUPT_FRAME_FA0(sp)
	FSREG	fa1, RISCV_INTERRUPT_FRAME_FA1(sp)
	FSREG	fa2, RISCV_INTERRUPT_FRAME_FA2(sp)
	FSREG	fa3, RISCV_INTERRUPT_FRAME_FA3(sp)
	FSREG	fa4, RISCV_INTERRUPT_FRAME_FA4(sp)
	FSREG	fa5, RISCV_INTERRUPT_FRAME_FA5(sp)
	FSREG	fa6, RISCV_INTERRUPT_FRAME_FA6(sp)
	FSREG	fa7, RISCV_INTERRUPT_FRAME_FA7(sp)
#endif

	/* Check if this is a synchronous or interrupt exception */
	bgez	a0, .Lsynchronous_exception

	/* Increment interrupt nest and thread dispatch disable level */
	lw	t0, PER_CPU_ISR_NEST_LEVEL(s0)
	lw	t1, PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL(s0)
	addi	t2, t0, 1
	addi	t1, t1, 1
	sw	t2, PER_CPU_ISR_NEST_LEVEL(s0)
	sw	t1, PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL(s0)

	CLEAR_RESERVATIONS	s0

	/*
	 * Remember current stack pointer in non-volatile register s1.  Switch
	 * to interrupt stack if necessary.
	 */
	mv	s1, sp
	bnez	t0, .Linterrupt_stack_switch_done
	LREG	sp, PER_CPU_INTERRUPT_STACK_HIGH(s0)
.Linterrupt_stack_switch_done:

	mv	a1, s0
	call	_RISCV_Interrupt_dispatch

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

	/* Restore */
	LREG	a0, RISCV_INTERRUPT_FRAME_MSTATUS(sp)
	LREG	a1, RISCV_INTERRUPT_FRAME_MEPC(sp)
	LREG	a2, RISCV_INTERRUPT_FRAME_A2(sp)
	LREG	s0, RISCV_INTERRUPT_FRAME_S0(sp)
	LREG	s1, RISCV_INTERRUPT_FRAME_S1(sp)
	LREG	ra, RISCV_INTERRUPT_FRAME_RA(sp)
	LREG	a3, RISCV_INTERRUPT_FRAME_A3(sp)
	LREG	a4, RISCV_INTERRUPT_FRAME_A4(sp)
	LREG	a5, RISCV_INTERRUPT_FRAME_A5(sp)
	LREG	a6, RISCV_INTERRUPT_FRAME_A6(sp)
	LREG	a7, RISCV_INTERRUPT_FRAME_A7(sp)
	LREG	t0, RISCV_INTERRUPT_FRAME_T0(sp)
	LREG	t1, RISCV_INTERRUPT_FRAME_T1(sp)
	LREG	t2, RISCV_INTERRUPT_FRAME_T2(sp)
	LREG	t3, RISCV_INTERRUPT_FRAME_T3(sp)
	LREG	t4, RISCV_INTERRUPT_FRAME_T4(sp)
	LREG	t5, RISCV_INTERRUPT_FRAME_T5(sp)
	LREG	t6, RISCV_INTERRUPT_FRAME_T6(sp)
	csrw	mstatus, a0
	csrw	mepc, a1
#if __riscv_flen > 0
	lw	a0, RISCV_INTERRUPT_FRAME_FCSR(sp)
	FLREG	ft0, RISCV_INTERRUPT_FRAME_FT0(sp)
	FLREG	ft1, RISCV_INTERRUPT_FRAME_FT1(sp)
	FLREG	ft2, RISCV_INTERRUPT_FRAME_FT2(sp)
	FLREG	ft3, RISCV_INTERRUPT_FRAME_FT3(sp)
	FLREG	ft4, RISCV_INTERRUPT_FRAME_FT4(sp)
	FLREG	ft5, RISCV_INTERRUPT_FRAME_FT5(sp)
	FLREG	ft6, RISCV_INTERRUPT_FRAME_FT6(sp)
	FLREG	ft7, RISCV_INTERRUPT_FRAME_FT7(sp)
	FLREG	ft8, RISCV_INTERRUPT_FRAME_FT8(sp)
	FLREG	ft9, RISCV_INTERRUPT_FRAME_FT9(sp)
	FLREG	ft10, RISCV_INTERRUPT_FRAME_FT10(sp)
	FLREG	ft11, RISCV_INTERRUPT_FRAME_FT11(sp)
	FLREG	fa0, RISCV_INTERRUPT_FRAME_FA0(sp)
	FLREG	fa1, RISCV_INTERRUPT_FRAME_FA1(sp)
	FLREG	fa2, RISCV_INTERRUPT_FRAME_FA2(sp)
	FLREG	fa3, RISCV_INTERRUPT_FRAME_FA3(sp)
	FLREG	fa4, RISCV_INTERRUPT_FRAME_FA4(sp)
	FLREG	fa5, RISCV_INTERRUPT_FRAME_FA5(sp)
	FLREG	fa6, RISCV_INTERRUPT_FRAME_FA6(sp)
	FLREG	fa7, RISCV_INTERRUPT_FRAME_FA7(sp)
	fscsr	a0
#endif
	LREG	a0, RISCV_INTERRUPT_FRAME_A0(sp)
	LREG	a1, RISCV_INTERRUPT_FRAME_A1(sp)

	addi	sp, sp, CPU_INTERRUPT_FRAME_SIZE

	mret

.Lsynchronous_exception:

	SREG	a0, RISCV_EXCEPTION_FRAME_MCAUSE(sp)
	addi	a0, sp, CPU_INTERRUPT_FRAME_SIZE
	SREG	a0, RISCV_EXCEPTION_FRAME_SP(sp)
	SREG	gp, RISCV_EXCEPTION_FRAME_GP(sp)
	SREG	tp, RISCV_EXCEPTION_FRAME_TP(sp)
	SREG	s2, RISCV_EXCEPTION_FRAME_S2(sp)
	SREG	s3, RISCV_EXCEPTION_FRAME_S3(sp)
	SREG	s4, RISCV_EXCEPTION_FRAME_S4(sp)
	SREG	s5, RISCV_EXCEPTION_FRAME_S5(sp)
	SREG	s6, RISCV_EXCEPTION_FRAME_S6(sp)
	SREG	s7, RISCV_EXCEPTION_FRAME_S7(sp)
	SREG	s8, RISCV_EXCEPTION_FRAME_S8(sp)
	SREG	s9, RISCV_EXCEPTION_FRAME_S9(sp)
	SREG	s10, RISCV_EXCEPTION_FRAME_S10(sp)
	SREG	s11, RISCV_EXCEPTION_FRAME_S11(sp)
#if __riscv_flen > 0
	FSREG	fs0, RISCV_EXCEPTION_FRAME_FS0(sp)
	FSREG	fs1, RISCV_EXCEPTION_FRAME_FS1(sp)
	FSREG	fs2, RISCV_EXCEPTION_FRAME_FS2(sp)
	FSREG	fs3, RISCV_EXCEPTION_FRAME_FS3(sp)
	FSREG	fs4, RISCV_EXCEPTION_FRAME_FS4(sp)
	FSREG	fs5, RISCV_EXCEPTION_FRAME_FS5(sp)
	FSREG	fs6, RISCV_EXCEPTION_FRAME_FS6(sp)
	FSREG	fs7, RISCV_EXCEPTION_FRAME_FS7(sp)
	FSREG	fs8, RISCV_EXCEPTION_FRAME_FS8(sp)
	FSREG	fs9, RISCV_EXCEPTION_FRAME_FS9(sp)
	FSREG	fs10, RISCV_EXCEPTION_FRAME_FS10(sp)
	FSREG	fs11, RISCV_EXCEPTION_FRAME_FS11(sp)
#endif

	li	a0, 9
	mv	a1, sp
	call	_Terminate
