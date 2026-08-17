/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2026 embedded brains GmbH & Co. KG
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
#include <rtems/mips/iregdef.h>

/*
 * The pattern arrives in a0 and stays there, AT is the temporary of the check.
 * The zero register, the stack pointer, the global pointer and the k0 and k1
 * of the exception handler are not part of the pattern.  The thread pointer of
 * this port lives in the context and not in a register.  The stack pointer is
 * compared through the frame, since no register is left for it.
 *
 * A hard float build saves the registers of the FPU in the floating point
 * context of a thread.  They are not covered here, since that needs the FPU
 * enabled in the status register of the coprocessor and no BSP of the test
 * programme has one.
 */

#define FRAME_OFFSET_S0 0
#define FRAME_OFFSET_S1 4
#define FRAME_OFFSET_S2 8
#define FRAME_OFFSET_S3 12
#define FRAME_OFFSET_S4 16
#define FRAME_OFFSET_S5 20
#define FRAME_OFFSET_S6 24
#define FRAME_OFFSET_S7 28
#define FRAME_OFFSET_FP 32
#define FRAME_OFFSET_RA 36
#define FRAME_OFFSET_SP_XOR 40
#define FRAME_SIZE 48

	.set noreorder
	.set noat

	.text

FRAME(_CPU_Context_validate,sp,0,ra)

	addiu	sp, sp, -FRAME_SIZE

	sw	s0, FRAME_OFFSET_S0(sp)
	sw	s1, FRAME_OFFSET_S1(sp)
	sw	s2, FRAME_OFFSET_S2(sp)
	sw	s3, FRAME_OFFSET_S3(sp)
	sw	s4, FRAME_OFFSET_S4(sp)
	sw	s5, FRAME_OFFSET_S5(sp)
	sw	s6, FRAME_OFFSET_S6(sp)
	sw	s7, FRAME_OFFSET_S7(sp)
	sw	fp, FRAME_OFFSET_FP(sp)
	sw	ra, FRAME_OFFSET_RA(sp)

	xor	AT, sp, a0
	sw	AT, FRAME_OFFSET_SP_XOR(sp)

	/* Fill */

.macro fill_register reg, inc
	addiu	\reg, a0, \inc
.endm

	fill_register	v0, 1
	fill_register	v1, 2
	fill_register	a1, 3
	fill_register	a2, 4
	fill_register	a3, 5
	fill_register	t0, 6
	fill_register	t1, 7
	fill_register	t2, 8
	fill_register	t3, 9
	fill_register	t4, 10
	fill_register	t5, 11
	fill_register	t6, 12
	fill_register	t7, 13
	fill_register	t8, 14
	fill_register	t9, 15
	fill_register	s0, 16
	fill_register	s1, 17
	fill_register	s2, 18
	fill_register	s3, 19
	fill_register	s4, 20
	fill_register	s5, 21
	fill_register	s6, 22
	fill_register	s7, 23
	fill_register	fp, 24
	fill_register	ra, 25

	/* Check */

.macro check_register reg, inc
	addiu	AT, a0, \inc
	bne	\reg, AT, .Lrestore
	 nop
.endm

.Lcheck:
	check_register	v0, 1
	check_register	v1, 2
	check_register	a1, 3
	check_register	a2, 4
	check_register	a3, 5
	check_register	t0, 6
	check_register	t1, 7
	check_register	t2, 8
	check_register	t3, 9
	check_register	t4, 10
	check_register	t5, 11
	check_register	t6, 12
	check_register	t7, 13
	check_register	t8, 14
	check_register	t9, 15
	check_register	s0, 16
	check_register	s1, 17
	check_register	s2, 18
	check_register	s3, 19
	check_register	s4, 20
	check_register	s5, 21
	check_register	s6, 22
	check_register	s7, 23
	check_register	fp, 24
	check_register	ra, 25

	lw	AT, FRAME_OFFSET_SP_XOR(sp)
	xor	AT, AT, a0
	bne	AT, sp, .Lrestore
	 nop

	b	.Lcheck
	 nop

	/* Restore */

.Lrestore:
	lw	s0, FRAME_OFFSET_S0(sp)
	lw	s1, FRAME_OFFSET_S1(sp)
	lw	s2, FRAME_OFFSET_S2(sp)
	lw	s3, FRAME_OFFSET_S3(sp)
	lw	s4, FRAME_OFFSET_S4(sp)
	lw	s5, FRAME_OFFSET_S5(sp)
	lw	s6, FRAME_OFFSET_S6(sp)
	lw	s7, FRAME_OFFSET_S7(sp)
	lw	fp, FRAME_OFFSET_FP(sp)
	lw	ra, FRAME_OFFSET_RA(sp)

	j	ra
	 addiu	sp, sp, FRAME_SIZE

ENDFRAME(_CPU_Context_validate)
