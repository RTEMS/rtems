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
 * The caller saved registers of the ABI are RAX, RCX, RDX, RSI, RDI and R8 to
 * R11.  RDI arrives with the pattern and is clobbered last.  The control word
 * of the x87 unit and the control and status register of the SSE unit belong
 * to the floating point context, which the interrupt prologue saves and
 * restores, so they are clobbered here as well.
 */

#define FPUCW_DEFAULT 0x037f
#define MXCSR_DEFAULT 0x1f80

	BEGIN_CODE

	.p2align 4
	PUBLIC (_CPU_Context_volatile_clobber)

SYM (_CPU_Context_volatile_clobber):

.macro clobber_register reg, inc
	leaq	\inc(rdi), \reg
.endm

	subq	$8, rsp

	movq	rdi, rax
	andq	$3, rax
	shlq	$10, rax
	orq	$FPUCW_DEFAULT, rax
	movq	rax, (rsp)
	fldcw	(rsp)

	movq	rdi, rax
	andq	$3, rax
	shlq	$13, rax
	orq	$MXCSR_DEFAULT, rax
	movq	rax, (rsp)
	ldmxcsr	(rsp)

	addq	$8, rsp

	clobber_register	rax, 1
	clobber_register	rcx, 2
	clobber_register	rdx, 3
	clobber_register	rsi, 4
	clobber_register	r8, 5
	clobber_register	r9, 6
	clobber_register	r10, 7
	clobber_register	r11, 8
	clobber_register	rdi, 9
	ret

END_CODE

END
