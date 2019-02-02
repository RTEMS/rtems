/*
 * Copyright (c) 2018 embedded brains GmbH
 * Copyright (c) 2015 Hesham Almatary <hesham@alumni.york.ac.uk>
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
#include <rtems/score/cpu.h>

#define OFFSET(i) ((i) * CPU_SIZEOF_POINTER)

#define RA_OFFSET OFFSET(0)
#define T0_OFFSET OFFSET(1)
#define T1_OFFSET OFFSET(2)
#define T2_OFFSET OFFSET(3)
#define S0_OFFSET OFFSET(4)
#define S1_OFFSET OFFSET(5)
#define A0_OFFSET OFFSET(6)
#define A1_OFFSET OFFSET(7)
#define A2_OFFSET OFFSET(8)
#define A3_OFFSET OFFSET(9)
#define A4_OFFSET OFFSET(10)
#define A5_OFFSET OFFSET(11)
#define A6_OFFSET OFFSET(12)
#define A7_OFFSET OFFSET(13)
#define S2_OFFSET OFFSET(14)
#define S3_OFFSET OFFSET(15)
#define S4_OFFSET OFFSET(16)
#define S5_OFFSET OFFSET(17)
#define S6_OFFSET OFFSET(18)
#define S7_OFFSET OFFSET(19)
#define S8_OFFSET OFFSET(20)
#define S9_OFFSET OFFSET(21)
#define S10_OFFSET OFFSET(22)
#define S11_OFFSET OFFSET(23)
#define T3_OFFSET OFFSET(24)
#define T4_OFFSET OFFSET(25)
#define T5_OFFSET OFFSET(26)
#define T6_OFFSET OFFSET(27)
#define TMP_OFFSET OFFSET(28)

#if __riscv_flen == 32
#define FOFFSET(i) (OFFSET(29) + (i) * 4)
#elif __riscv_flen == 64
#define FOFFSET(i) (OFFSET(30) + (i) * 8)
#else
#define FOFFSET(i) OFFSET(29)
#endif /* __riscv_flen */

#define FRAME_SIZE \
  ((FOFFSET(32) + CPU_STACK_ALIGNMENT - 1) & ~(CPU_STACK_ALIGNMENT - 1))

	.section	.text, "ax", @progbits
	.align	2

PUBLIC(_CPU_Context_validate)
SYM(_CPU_Context_validate):
	addi	sp, sp, -FRAME_SIZE

	/* Save */

	SREG	ra, RA_OFFSET(sp)
	SREG	t0, T0_OFFSET(sp)
	SREG	t1, T1_OFFSET(sp)
	SREG	t2, T2_OFFSET(sp)
	SREG	s0, S0_OFFSET(sp)
	SREG	s1, S1_OFFSET(sp)
	SREG	a0, A0_OFFSET(sp)
	SREG	a1, A1_OFFSET(sp)
	SREG	a2, A2_OFFSET(sp)
	SREG	a3, A3_OFFSET(sp)
	SREG	a4, A4_OFFSET(sp)
	SREG	a5, A5_OFFSET(sp)
	SREG	a6, A6_OFFSET(sp)
	SREG	a7, A7_OFFSET(sp)
	SREG	s2, S2_OFFSET(sp)
	SREG	s3, S3_OFFSET(sp)
	SREG	s4, S4_OFFSET(sp)
	SREG	s5, S5_OFFSET(sp)
	SREG	s6, S6_OFFSET(sp)
	SREG	s7, S7_OFFSET(sp)
	SREG	s8, S8_OFFSET(sp)
	SREG	s9, S9_OFFSET(sp)
	SREG	s10, S10_OFFSET(sp)
	SREG	s11, S11_OFFSET(sp)
	SREG	t3, T3_OFFSET(sp)
	SREG	t4, T4_OFFSET(sp)
	SREG	t5, T5_OFFSET(sp)
	SREG	t6, T6_OFFSET(sp)

#if __riscv_flen > 0
	FSREG	f0, FOFFSET(0)(sp)
	FSREG	f1, FOFFSET(1)(sp)
	FSREG	f2, FOFFSET(2)(sp)
	FSREG	f3, FOFFSET(3)(sp)
	FSREG	f4, FOFFSET(4)(sp)
	FSREG	f5, FOFFSET(5)(sp)
	FSREG	f6, FOFFSET(6)(sp)
	FSREG	f7, FOFFSET(7)(sp)
	FSREG	f8, FOFFSET(8)(sp)
	FSREG	f9, FOFFSET(9)(sp)
	FSREG	f10, FOFFSET(10)(sp)
	FSREG	f11, FOFFSET(11)(sp)
	FSREG	f12, FOFFSET(12)(sp)
	FSREG	f13, FOFFSET(13)(sp)
	FSREG	f14, FOFFSET(14)(sp)
	FSREG	f15, FOFFSET(15)(sp)
	FSREG	f16, FOFFSET(16)(sp)
	FSREG	f17, FOFFSET(17)(sp)
	FSREG	f18, FOFFSET(18)(sp)
	FSREG	f19, FOFFSET(19)(sp)
	FSREG	f20, FOFFSET(20)(sp)
	FSREG	f21, FOFFSET(21)(sp)
	FSREG	f22, FOFFSET(22)(sp)
	FSREG	f23, FOFFSET(23)(sp)
	FSREG	f24, FOFFSET(24)(sp)
	FSREG	f25, FOFFSET(25)(sp)
	FSREG	f26, FOFFSET(26)(sp)
	FSREG	f27, FOFFSET(27)(sp)
	FSREG	f28, FOFFSET(28)(sp)
	FSREG	f29, FOFFSET(29)(sp)
	FSREG	f30, FOFFSET(30)(sp)
	FSREG	f31, FOFFSET(31)(sp)
#endif /* __riscv_flen */

	/* Fill */

	addi	ra, a0, 1
	/* sp must remain as is */
	/* gp must remain as is */
	/* tp must remain as is */
	/* t0 is used for temporary values */
	addi	t1, a0, 2
	addi	t2, a0, 3
	addi	s0, a0, 4
	addi	s1, a0, 5
	/* a0 is the pattern */
	addi	a1, a0, 6
	addi	a2, a0, 7
	addi	a3, a0, 8
	addi	a4, a0, 9
	addi	a5, a0, 10
	addi	a6, a0, 11
	addi	a7, a0, 12
	addi	s2, a0, 13
	addi	s3, a0, 14
	addi	s4, a0, 15
	addi	s5, a0, 16
	addi	s6, a0, 17
	addi	s7, a0, 18
	addi	s8, a0, 19
	addi	s9, a0, 20
	addi	s10, a0, 21
	addi	s11, a0, 22
	addi	t3, a0, 23

	xor	t4, sp, a0
	xor	t5, gp, a0
	xor	t6, tp, a0

#if __riscv_flen > 0
	andi	t0, a0, 0x1f
	fsflags	t0

	.macro	fill_f reg, inc
	addi	t0, a0, 24 + \inc
	FMVYX	\reg, t0
	.endm

	fill_f	f0, 0
	fill_f	f1, 1
	fill_f	f2, 2
	fill_f	f3, 3
	fill_f	f4, 4
	fill_f	f5, 5
	fill_f	f6, 6
	fill_f	f7, 7
	fill_f	f8, 8
	fill_f	f9, 9
	fill_f	f10, 10
	fill_f	f11, 11
	fill_f	f12, 12
	fill_f	f13, 13
	fill_f	f14, 14
	fill_f	f15, 15
	fill_f	f16, 16
	fill_f	f17, 17
	fill_f	f18, 18
	fill_f	f19, 19
	fill_f	f20, 20
	fill_f	f21, 21
	fill_f	f22, 22
	fill_f	f23, 23
	fill_f	f24, 24
	fill_f	f25, 25
	fill_f	f26, 26
	fill_f	f27, 27
	fill_f	f28, 28
	fill_f	f29, 29
	fill_f	f30, 30
	fill_f	f31, 31
#endif /* __riscv_flen */

	/* Check */

.Lcheck:
	.macro	check_register reg, inc
	addi	t0, a0, \inc
	bne	\reg, t0, .Lrestore
	.endm

	check_register	ra, 1
	check_register	t1, 2
	check_register	t2, 3
	check_register	s0, 4
	check_register	s1, 5
	check_register	a1, 6
	check_register	a2, 7
	check_register	a3, 8
	check_register	a4, 9
	check_register	a5, 10
	check_register	a6, 11
	check_register	a7, 12
	check_register	s2, 13
	check_register	s3, 14
	check_register	s4, 15
	check_register	s5, 16
	check_register	s6, 17
	check_register	s7, 18
	check_register	s8, 19
	check_register	s9, 20
	check_register	s10, 21
	check_register	s11, 22
	check_register	t3, 23

	xor	t0, sp, a0
	bne	t4, t0, .Lrestore

	xor	t0, gp, a0
	bne	t5, t0, .Lrestore

	xor	t0, tp, a0
	bne	t6, t0, .Lrestore

#if __riscv_flen > 0
	SREG	t1, TMP_OFFSET(sp)
	frflags	t0
	andi	t1, a0, 0x1f
	xor	t0, t1, t0
	LREG	t1, TMP_OFFSET(sp)
	bnez	t0, .Lrestore


	.macro	check_f reg, inc
	FMVXY	t0, \reg
	addi	t0, t0, -24 - \inc
	bne	t0, a0, .Lrestore
	.endm

	check_f	f0, 0
	check_f	f1, 1
	check_f	f2, 2
	check_f	f3, 3
	check_f	f4, 4
	check_f	f5, 5
	check_f	f6, 6
	check_f	f7, 7
	check_f	f8, 8
	check_f	f9, 9
	check_f	f10, 10
	check_f	f11, 11
	check_f	f12, 12
	check_f	f13, 13
	check_f	f14, 14
	check_f	f15, 15
	check_f	f16, 16
	check_f	f17, 17
	check_f	f18, 18
	check_f	f19, 19
	check_f	f20, 20
	check_f	f21, 21
	check_f	f22, 22
	check_f	f23, 23
	check_f	f24, 24
	check_f	f25, 25
	check_f	f26, 26
	check_f	f27, 27
	check_f	f28, 28
	check_f	f29, 29
	check_f	f30, 30
	check_f	f31, 31
#endif /* __riscv_flen */

	j	.Lcheck

	/* Restore */

.Lrestore:

	LREG	ra, RA_OFFSET(sp)
	LREG	t0, T0_OFFSET(sp)
	LREG	t1, T1_OFFSET(sp)
	LREG	t2, T2_OFFSET(sp)
	LREG	s0, S0_OFFSET(sp)
	LREG	s1, S1_OFFSET(sp)
	LREG	a0, A0_OFFSET(sp)
	LREG	a1, A1_OFFSET(sp)
	LREG	a2, A2_OFFSET(sp)
	LREG	a3, A3_OFFSET(sp)
	LREG	a4, A4_OFFSET(sp)
	LREG	a5, A5_OFFSET(sp)
	LREG	a6, A6_OFFSET(sp)
	LREG	a7, A7_OFFSET(sp)
	LREG	s2, S2_OFFSET(sp)
	LREG	s3, S3_OFFSET(sp)
	LREG	s4, S4_OFFSET(sp)
	LREG	s5, S5_OFFSET(sp)
	LREG	s6, S6_OFFSET(sp)
	LREG	s7, S7_OFFSET(sp)
	LREG	s8, S8_OFFSET(sp)
	LREG	s9, S9_OFFSET(sp)
	LREG	s10, S10_OFFSET(sp)
	LREG	s11, S11_OFFSET(sp)
	LREG	t3, T3_OFFSET(sp)
	LREG	t4, T4_OFFSET(sp)
	LREG	t5, T5_OFFSET(sp)
	LREG	t6, T6_OFFSET(sp)

#if __riscv_flen > 0
	FLREG	f0, FOFFSET(0)(sp)
	FLREG	f1, FOFFSET(1)(sp)
	FLREG	f2, FOFFSET(2)(sp)
	FLREG	f3, FOFFSET(3)(sp)
	FLREG	f4, FOFFSET(4)(sp)
	FLREG	f5, FOFFSET(5)(sp)
	FLREG	f6, FOFFSET(6)(sp)
	FLREG	f7, FOFFSET(7)(sp)
	FLREG	f8, FOFFSET(8)(sp)
	FLREG	f9, FOFFSET(9)(sp)
	FLREG	f10, FOFFSET(10)(sp)
	FLREG	f11, FOFFSET(11)(sp)
	FLREG	f12, FOFFSET(12)(sp)
	FLREG	f13, FOFFSET(13)(sp)
	FLREG	f14, FOFFSET(14)(sp)
	FLREG	f15, FOFFSET(15)(sp)
	FLREG	f16, FOFFSET(16)(sp)
	FLREG	f17, FOFFSET(17)(sp)
	FLREG	f18, FOFFSET(18)(sp)
	FLREG	f19, FOFFSET(19)(sp)
	FLREG	f20, FOFFSET(20)(sp)
	FLREG	f21, FOFFSET(21)(sp)
	FLREG	f22, FOFFSET(22)(sp)
	FLREG	f23, FOFFSET(23)(sp)
	FLREG	f24, FOFFSET(24)(sp)
	FLREG	f25, FOFFSET(25)(sp)
	FLREG	f26, FOFFSET(26)(sp)
	FLREG	f27, FOFFSET(27)(sp)
	FLREG	f28, FOFFSET(28)(sp)
	FLREG	f29, FOFFSET(29)(sp)
	FLREG	f30, FOFFSET(30)(sp)
	FLREG	f31, FOFFSET(31)(sp)
#endif /* __riscv_flen */

	addi	sp, sp, FRAME_SIZE
	ret
