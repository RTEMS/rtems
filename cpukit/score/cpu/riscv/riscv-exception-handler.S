/**
 * @file
 *
 * @ingroup RTEMSScoreCPURISCV
 *
 * @brief This source file contains the exception entry of the RISC-V CPU
 *   port.
 */

/*
 * Copyright (c) 2026 Gedare Bloom
 * Copyright (c) 2018 embedded brains GmbH & Co. KG
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

#include <rtems/score/riscv-utility.h>

#include "riscv-exception-frame.h"

PUBLIC(_RISCV_Vector_table)

	.section	.text, "ax", @progbits
	/*
	 * Vectored exception/interrupt handlers can require the vector base to
	 * be aligned to 256 bytes
	 */
	.align	8
	.option	arch, +zicsr

/*
 * This could be a nop slide, but it would introduce additional latency into
 * interrupt handling depending on which interrupt was triggered
 */
TYPE_FUNC(_RISCV_Vector_table)
SYM(_RISCV_Vector_table):
	.rept 32
	j .LRISCV_Exception_handler
	.endr

.LRISCV_Exception_handler:
	addi	sp, sp, -CPU_INTERRUPT_FRAME_SIZE

	/* Save */
	SREG	a0, RISCV_INTERRUPT_FRAME_A0(sp)
	SREG	a1, RISCV_INTERRUPT_FRAME_A1(sp)
	SREG	a2, RISCV_INTERRUPT_FRAME_A2(sp)
	SREG	s0, RISCV_INTERRUPT_FRAME_S0(sp)
#ifdef RISCV_USE_S_MODE
	csrr	a0, scause
	csrr	a1, sstatus
	csrr	a2, sepc
#else
	csrr	a0, mcause
	csrr	a1, mstatus
	csrr	a2, mepc
#endif
	GET_SELF_CPU_CONTROL	s0
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

	/*
	 * Check if this is a synchronous or interrupt exception. The interrupt
	 * flag does not need to be isolated here since the interrupt flag
	 * (MSB) not being set will always evaluate to greater than zero and be
	 * registered as an exception.
	 */
	bgez	a0, .Lsynchronous_exception

	/*
	 * Interrupt exception, clear MDT bit.
	 * This is only necessary if the Smdbltrp extension is implemented.
	 * In that case not clearing the MDT bit would prevent us from setting
	 * the MIE bit later. Smdbltrp implies zicsr, so if zicsr is not present
	 * then Smdbltrp (which introduces mstatush/MDT) can not be present.
	 */
#ifdef __riscv_zicsr
#ifdef RISCV_USE_S_MODE
	li	t0, SSTATUS_SDT
	csrrc	zero, sstatus, t0
#else
#if __riscv_xlen == 64
	li	t0, MSTATUS_MDT
	csrrc	zero, mstatus, t0
#elif __riscv_xlen == 32
	li	t0, MSTATUSH_MDT
	csrrc	zero, mstatush, t0
#endif
#endif /* RISCV_USE_S_MODE */
#endif /* __riscv_zicsr */

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
#ifdef RISCV_USE_S_MODE
	li	a1, SSTATUS_SIE
#else
	li	a1, MSTATUS_MIE
#endif
	call	_Thread_Do_dispatch

	/* Disable interrupts */
#ifdef RISCV_USE_S_MODE
	csrrc	zero, sstatus, SSTATUS_SIE
#else
	csrrc	zero, mstatus, MSTATUS_MIE
#endif

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
	LREG	a0, RISCV_INTERRUPT_FRAME_STATUS(sp)
	LREG	a1, RISCV_INTERRUPT_FRAME_EPC(sp)
	LREG	s0, RISCV_INTERRUPT_FRAME_S0(sp)
	LREG	s1, RISCV_INTERRUPT_FRAME_S1(sp)
	RESTORE_INTERRUPT_FRAME_GPRS	sp

#ifdef RISCV_USE_S_MODE
	/* Clear SDT bit before restoring sstatus register */
	li	a2, SSTATUS_SDT
	not	a2, a2
	and	a0, a0, a2
	csrw	sstatus, a0
	csrw	sepc, a1
#else
	/*
	 * Clear MDT bit before restoring mstatus register.
	 * This is only necessary if the Smdbltrp extension is implemented.
	 * In that case, writing to mstatus with MDT set would clear the MIE
	 * bit, regardless of the MIE value written.
	 * On RV32, the MDT bit is in the mstatush CSR which is not restored.
	 */
#if __riscv_xlen == 64
	li	a2, MSTATUS_MDT
	not	a2, a2
	and	a0, a0, a2
#endif
	csrw	mstatus, a0
	csrw	mepc, a1
#endif /* RISCV_USE_S_MODE */

#if __riscv_flen > 0
	lw	a0, RISCV_INTERRUPT_FRAME_FCSR(sp)
	RESTORE_INTERRUPT_FRAME_FPRS	sp
	fscsr	a0
#endif
	LREG	a0, RISCV_INTERRUPT_FRAME_A0(sp)
	LREG	a1, RISCV_INTERRUPT_FRAME_A1(sp)
	LREG	a2, RISCV_INTERRUPT_FRAME_A2(sp)

	addi	sp, sp, CPU_INTERRUPT_FRAME_SIZE

#ifdef RISCV_USE_S_MODE
	sret
#else
	mret
#endif

.Lsynchronous_exception:

	SREG	a0, RISCV_EXCEPTION_FRAME_CAUSE(sp)
	addi	a0, sp, CPU_INTERRUPT_FRAME_SIZE
	SREG	a0, RISCV_EXCEPTION_FRAME_SP(sp)
	SAVE_EXCEPTION_FRAME	sp

	li	a0, 9
	mv	a1, sp
	call	_Terminate
