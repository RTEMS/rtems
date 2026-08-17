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
 * The pattern stays in EAX and ECX is the temporary of the check, so the
 * registers of the pattern are the callee saved EBX, ESI, EDI and EBP and the
 * caller saved EDX.  The stack pointer belongs to the environment of the
 * thread and is compared through the frame, since no register is left for it.
 *
 * The floating point context of this port is the control word of the x87 unit
 * and the control and status register of the SSE unit, and it exists only in
 * an SSE build.  The values of the vector registers are not part of it, since
 * the ABI lets a function clobber them.  Only the rounding mode of the two
 * control words carries the pattern, the remaining fields keep their defaults,
 * so that no exception is unmasked and the precision does not change.
 */

#define FRAME_OFFSET_SP_XOR 0
#define FRAME_OFFSET_FPUCW 4
#define FRAME_OFFSET_MXCSR 8
#define FRAME_OFFSET_FPUCW_SAVED 12
#define FRAME_OFFSET_MXCSR_SAVED 16
#define FRAME_OFFSET_TMP 20
#define FRAME_SIZE 24
#define FRAME_OFFSET_PATTERN ( FRAME_SIZE + 20 )

#define FPUCW_DEFAULT 0x037f
#define MXCSR_DEFAULT 0x1f80

        BEGIN_CODE

	.p2align 4
	PUBLIC (_CPU_Context_validate)

SYM (_CPU_Context_validate):
	pushl	ebp
	pushl	ebx
	pushl	esi
	pushl	edi
	subl	$FRAME_SIZE, esp

	movl	FRAME_OFFSET_PATTERN(esp), eax

	movl	esp, ecx
	xorl	eax, ecx
	movl	ecx, FRAME_OFFSET_SP_XOR(esp)

#ifdef __SSE__
	movl	$0, FRAME_OFFSET_TMP(esp)
	fnstcw	FRAME_OFFSET_TMP(esp)
	movl	FRAME_OFFSET_TMP(esp), ecx
	movl	ecx, FRAME_OFFSET_FPUCW_SAVED(esp)
	stmxcsr	FRAME_OFFSET_TMP(esp)
	movl	FRAME_OFFSET_TMP(esp), ecx
	movl	ecx, FRAME_OFFSET_MXCSR_SAVED(esp)
#endif

	/* Fill */

	leal	1(eax), ebx
	leal	2(eax), edx
	leal	3(eax), esi
	leal	4(eax), edi
	leal	5(eax), ebp

#ifdef __SSE__
	movl	eax, ecx
	andl	$3, ecx
	shll	$10, ecx
	orl	$FPUCW_DEFAULT, ecx
	movl	ecx, FRAME_OFFSET_FPUCW(esp)
	movl	ecx, FRAME_OFFSET_TMP(esp)
	fldcw	FRAME_OFFSET_TMP(esp)

	movl	eax, ecx
	andl	$3, ecx
	shll	$13, ecx
	orl	$MXCSR_DEFAULT, ecx
	movl	ecx, FRAME_OFFSET_MXCSR(esp)
	movl	ecx, FRAME_OFFSET_TMP(esp)
	ldmxcsr	FRAME_OFFSET_TMP(esp)
#endif

	/* Check */

.macro check_register reg, inc
	leal	\inc(eax), ecx
	cmpl	ecx, \reg
	jne	.Lrestore
.endm

.Lcheck:
	check_register	ebx, 1
	check_register	edx, 2
	check_register	esi, 3
	check_register	edi, 4
	check_register	ebp, 5

	movl	esp, ecx
	xorl	eax, ecx
	cmpl	FRAME_OFFSET_SP_XOR(esp), ecx
	jne	.Lrestore

#ifdef __SSE__
	movl	$0, FRAME_OFFSET_TMP(esp)
	fnstcw	FRAME_OFFSET_TMP(esp)
	movl	FRAME_OFFSET_TMP(esp), ecx
	cmpl	FRAME_OFFSET_FPUCW(esp), ecx
	jne	.Lrestore

	stmxcsr	FRAME_OFFSET_TMP(esp)
	movl	FRAME_OFFSET_TMP(esp), ecx
	cmpl	FRAME_OFFSET_MXCSR(esp), ecx
	jne	.Lrestore
#endif

	jmp	.Lcheck

	/* Restore */

.Lrestore:
#ifdef __SSE__
	movl	FRAME_OFFSET_FPUCW_SAVED(esp), ecx
	movl	ecx, FRAME_OFFSET_TMP(esp)
	fldcw	FRAME_OFFSET_TMP(esp)
	movl	FRAME_OFFSET_MXCSR_SAVED(esp), ecx
	movl	ecx, FRAME_OFFSET_TMP(esp)
	ldmxcsr	FRAME_OFFSET_TMP(esp)
#endif

	addl	$FRAME_SIZE, esp
	popl	edi
	popl	esi
	popl	ebx
	popl	ebp
	ret

END_CODE

END
