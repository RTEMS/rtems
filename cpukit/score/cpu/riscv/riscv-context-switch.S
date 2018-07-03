/*
 * Copyright (c) 2018 embedded brains GmbH
 *
 * Copyright (c) 2015 University of York.
 * Hesham ALmatary <hesham@alumni.york.ac.uk>
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

	.section	.text, "ax", @progbits
	.align	2

PUBLIC(_CPU_Context_switch)
PUBLIC(_CPU_Context_restore)

SYM(_CPU_Context_switch):
	GET_SELF_CPU_CONTROL	a2
	lw	a3, PER_CPU_ISR_DISPATCH_DISABLE(a2)

#if __riscv_flen > 0
	frcsr	a4
#endif

	SREG	ra, RISCV_CONTEXT_RA(a0)
	SREG	sp, RISCV_CONTEXT_SP(a0)
	SREG	s0, RISCV_CONTEXT_S0(a0)
	SREG	s1, RISCV_CONTEXT_S1(a0)
	SREG	s2, RISCV_CONTEXT_S2(a0)
	SREG	s3, RISCV_CONTEXT_S3(a0)
	SREG	s4, RISCV_CONTEXT_S4(a0)
	SREG	s5, RISCV_CONTEXT_S5(a0)
	SREG	s6, RISCV_CONTEXT_S6(a0)
	SREG	s7, RISCV_CONTEXT_S7(a0)
	SREG	s8, RISCV_CONTEXT_S8(a0)
	SREG	s9, RISCV_CONTEXT_S9(a0)
	SREG	s10, RISCV_CONTEXT_S10(a0)
	SREG	s11, RISCV_CONTEXT_S11(a0)

#if __riscv_flen > 0
	sw	a4, RISCV_CONTEXT_FCSR(a0)
	FSREG	fs0, RISCV_CONTEXT_FS0(a0)
	FSREG	fs1, RISCV_CONTEXT_FS1(a0)
	FSREG	fs2, RISCV_CONTEXT_FS2(a0)
	FSREG	fs3, RISCV_CONTEXT_FS3(a0)
	FSREG	fs4, RISCV_CONTEXT_FS4(a0)
	FSREG	fs5, RISCV_CONTEXT_FS5(a0)
	FSREG	fs6, RISCV_CONTEXT_FS6(a0)
	FSREG	fs7, RISCV_CONTEXT_FS7(a0)
	FSREG	fs8, RISCV_CONTEXT_FS8(a0)
	FSREG	fs9, RISCV_CONTEXT_FS9(a0)
	FSREG	fs10, RISCV_CONTEXT_FS10(a0)
	FSREG	fs11, RISCV_CONTEXT_FS11(a0)
#endif

	sw	a3, RISCV_CONTEXT_ISR_DISPATCH_DISABLE(a0)

#ifdef RTEMS_SMP
	/*
	 * The executing thread no longer executes on this processor.  Switch
	 * the stack to the temporary interrupt stack of this processor.  Mark
	 * the context of the executing thread as not executing.
	 */
	addi	sp, a2, PER_CPU_INTERRUPT_FRAME_AREA + CPU_INTERRUPT_FRAME_SIZE
	amoswap.w.rl	zero, zero, RISCV_CONTEXT_IS_EXECUTING(a0)

.Ltry_update_is_executing:

	/* Try to update the is executing indicator of the heir context */
	li	a3, 1
	amoswap.w.aq	a3, a3, RISCV_CONTEXT_IS_EXECUTING(a1)
	bnez	a3, .Lcheck_is_executing
#endif

.Lrestore:

	lw	a3, RISCV_CONTEXT_ISR_DISPATCH_DISABLE(a1)

	LREG	ra, RISCV_CONTEXT_RA(a1)
	LREG	sp, RISCV_CONTEXT_SP(a1)
	LREG	tp, RISCV_CONTEXT_TP(a1)
	LREG	s0, RISCV_CONTEXT_S0(a1)
	LREG	s1, RISCV_CONTEXT_S1(a1)
	LREG	s2, RISCV_CONTEXT_S2(a1)
	LREG	s3, RISCV_CONTEXT_S3(a1)
	LREG	s4, RISCV_CONTEXT_S4(a1)
	LREG	s5, RISCV_CONTEXT_S5(a1)
	LREG	s6, RISCV_CONTEXT_S6(a1)
	LREG	s7, RISCV_CONTEXT_S7(a1)
	LREG	s8, RISCV_CONTEXT_S8(a1)
	LREG	s9, RISCV_CONTEXT_S9(a1)
	LREG	s10, RISCV_CONTEXT_S10(a1)
	LREG	s11, RISCV_CONTEXT_S11(a1)

#if __riscv_flen > 0
	lw	a4, RISCV_CONTEXT_FCSR(a1)
	FLREG	fs0, RISCV_CONTEXT_FS0(a1)
	FLREG	fs1, RISCV_CONTEXT_FS1(a1)
	FLREG	fs2, RISCV_CONTEXT_FS2(a1)
	FLREG	fs3, RISCV_CONTEXT_FS3(a1)
	FLREG	fs4, RISCV_CONTEXT_FS4(a1)
	FLREG	fs5, RISCV_CONTEXT_FS5(a1)
	FLREG	fs6, RISCV_CONTEXT_FS6(a1)
	FLREG	fs7, RISCV_CONTEXT_FS7(a1)
	FLREG	fs8, RISCV_CONTEXT_FS8(a1)
	FLREG	fs9, RISCV_CONTEXT_FS9(a1)
	FLREG	fs10, RISCV_CONTEXT_FS10(a1)
	FLREG	fs11, RISCV_CONTEXT_FS11(a1)
	fscsr	a4
#endif

	sw	a3, PER_CPU_ISR_DISPATCH_DISABLE(a2)

	CLEAR_RESERVATIONS	a2

	ret

SYM(_CPU_Context_restore):
	mv	a1, a0
	GET_SELF_CPU_CONTROL	a2
	j	.Lrestore

#ifdef RTEMS_SMP
.Lcheck_is_executing:

	/* Check the is executing indicator of the heir context */
	lw	a3, RISCV_CONTEXT_IS_EXECUTING(a1)
	beqz	a3, .Ltry_update_is_executing

	/* We may have a new heir */

	/* Read the executing and heir */
	LREG	a4, PER_CPU_OFFSET_EXECUTING(a2)
	LREG	a5, PER_CPU_OFFSET_HEIR(a2)

	/*
	 * Update the executing only if necessary to avoid cache line
	 * monopolization.
	 */
	beq	a4, a5, .Ltry_update_is_executing

	/* Calculate the heir context pointer */
	sub	a4, a1, a4
	add	a1, a5, a4

	/* Update the executing */
	sw	a5, PER_CPU_OFFSET_EXECUTING(a2)

	j	.Ltry_update_is_executing
#endif
