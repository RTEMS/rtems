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
PUBLIC(_CPU_Context_restore_fp)
PUBLIC(_CPU_Context_save_fp)

SYM(_CPU_Context_switch):
	GET_SELF_CPU_CONTROL	a2
	lw	a3, PER_CPU_ISR_DISPATCH_DISABLE(a2)

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

	sw	a3, RISCV_CONTEXT_ISR_DISPATCH_DISABLE(a0)

.Lrestore:
	lw	a3, RISCV_CONTEXT_ISR_DISPATCH_DISABLE(a1)

	LREG	ra, RISCV_CONTEXT_RA(a1)
	LREG	sp, RISCV_CONTEXT_SP(a1)
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

	sw	a3, PER_CPU_ISR_DISPATCH_DISABLE(a2)

	ret

SYM(_CPU_Context_restore):
	mv	a1, a0
	GET_SELF_CPU_CONTROL	a2
	j	.Lrestore

	/* TODO no FP support for riscv32 yet */
	SYM(_CPU_Context_restore_fp):
	nop

	SYM(_CPU_Context_save_fp):
	nop
