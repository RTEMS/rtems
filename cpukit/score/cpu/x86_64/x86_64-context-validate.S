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

/*
 * The pattern arrives in RDI and stays there, RAX is the temporary of the
 * check, so the registers of the pattern are the callee saved RBX, RBP and R12
 * to R15 and the caller saved RCX, RDX, RSI and R8 to R11.  The stack pointer
 * belongs to the environment of the thread and is compared through the frame,
 * since no register is left for it.
 *
 * The floating point context of this port is the control word of the x87 unit
 * and the control and status register of the SSE unit.  The values of the
 * vector registers are not part of it, since the ABI lets a function clobber
 * them.  Only the rounding mode of the two control words carries the pattern,
 * the remaining fields keep their defaults, so that no exception is unmasked
 * and the precision does not change.
 */

#define FRAME_OFFSET_SP_XOR 0
#define FRAME_OFFSET_FPUCW 8
#define FRAME_OFFSET_MXCSR 16
#define FRAME_OFFSET_FPUCW_SAVED 24
#define FRAME_OFFSET_MXCSR_SAVED 32
#define FRAME_OFFSET_TMP 40
#define FRAME_SIZE 56

#define FPUCW_DEFAULT 0x037f
#define MXCSR_DEFAULT 0x1f80

	BEGIN_CODE

	.p2align 4
	PUBLIC (_CPU_Context_validate)

SYM (_CPU_Context_validate):
	pushq	rbp
	pushq	rbx
	pushq	r12
	pushq	r13
	pushq	r14
	pushq	r15
	subq	$FRAME_SIZE, rsp

	movq	rsp, rax
	xorq	rdi, rax
	movq	rax, FRAME_OFFSET_SP_XOR(rsp)

	movq	$0, FRAME_OFFSET_TMP(rsp)
	fnstcw	FRAME_OFFSET_TMP(rsp)
	movq	FRAME_OFFSET_TMP(rsp), rax
	movq	rax, FRAME_OFFSET_FPUCW_SAVED(rsp)
	stmxcsr	FRAME_OFFSET_TMP(rsp)
	movq	FRAME_OFFSET_TMP(rsp), rax
	movq	rax, FRAME_OFFSET_MXCSR_SAVED(rsp)

	/* Fill */

.macro fill_register reg, inc
	leaq	\inc(rdi), \reg
.endm

	fill_register	rbx, 1
	fill_register	rcx, 2
	fill_register	rdx, 3
	fill_register	rsi, 4
	fill_register	rbp, 5
	fill_register	r8, 6
	fill_register	r9, 7
	fill_register	r10, 8
	fill_register	r11, 9
	fill_register	r12, 10
	fill_register	r13, 11
	fill_register	r14, 12
	fill_register	r15, 13

	movq	rdi, rax
	andq	$3, rax
	shlq	$10, rax
	orq	$FPUCW_DEFAULT, rax
	movq	rax, FRAME_OFFSET_FPUCW(rsp)
	movq	rax, FRAME_OFFSET_TMP(rsp)
	fldcw	FRAME_OFFSET_TMP(rsp)

	movq	rdi, rax
	andq	$3, rax
	shlq	$13, rax
	orq	$MXCSR_DEFAULT, rax
	movq	rax, FRAME_OFFSET_MXCSR(rsp)
	movq	rax, FRAME_OFFSET_TMP(rsp)
	ldmxcsr	FRAME_OFFSET_TMP(rsp)

	/* Check */

.macro check_register reg, inc
	leaq	\inc(rdi), rax
	cmpq	rax, \reg
	jne	.Lrestore
.endm

.Lcheck:
	check_register	rbx, 1
	check_register	rcx, 2
	check_register	rdx, 3
	check_register	rsi, 4
	check_register	rbp, 5
	check_register	r8, 6
	check_register	r9, 7
	check_register	r10, 8
	check_register	r11, 9
	check_register	r12, 10
	check_register	r13, 11
	check_register	r14, 12
	check_register	r15, 13

	movq	rsp, rax
	xorq	rdi, rax
	cmpq	FRAME_OFFSET_SP_XOR(rsp), rax
	jne	.Lrestore

	movq	$0, FRAME_OFFSET_TMP(rsp)
	fnstcw	FRAME_OFFSET_TMP(rsp)
	movq	FRAME_OFFSET_TMP(rsp), rax
	cmpq	FRAME_OFFSET_FPUCW(rsp), rax
	jne	.Lrestore

	stmxcsr	FRAME_OFFSET_TMP(rsp)
	movq	FRAME_OFFSET_TMP(rsp), rax
	cmpq	FRAME_OFFSET_MXCSR(rsp), rax
	jne	.Lrestore

	jmp	.Lcheck

	/* Restore */

.Lrestore:
	movq	FRAME_OFFSET_FPUCW_SAVED(rsp), rax
	movq	rax, FRAME_OFFSET_TMP(rsp)
	fldcw	FRAME_OFFSET_TMP(rsp)
	movq	FRAME_OFFSET_MXCSR_SAVED(rsp), rax
	movq	rax, FRAME_OFFSET_TMP(rsp)
	ldmxcsr	FRAME_OFFSET_TMP(rsp)

	addq	$FRAME_SIZE, rsp
	popq	r15
	popq	r14
	popq	r13
	popq	r12
	popq	rbx
	popq	rbp
	ret

END_CODE

END
