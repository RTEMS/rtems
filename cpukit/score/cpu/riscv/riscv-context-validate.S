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

#define FRAME_SIZE \
  ((OFFSET(28) + CPU_STACK_ALIGNMENT - 1) & ~(CPU_STACK_ALIGNMENT - 1))

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

	addi	sp, sp, FRAME_SIZE
	ret
