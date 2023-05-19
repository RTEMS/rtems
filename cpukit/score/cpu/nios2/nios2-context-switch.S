/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2011 embedded brains GmbH & Co. KG
 *
 * Copyright (c) 2006 Kolja Waschk (rtemsdev/ixo.de)
 *
 * Derived from no_cpu/cpu_asm.S, copyright (c) 1989-1999,
 *   On-Line Applications Research Corporation (OAR).
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

#include <rtems/score/percpu.h>
#include <rtems/score/nios2-utility.h>

	.extern	_Per_CPU_Information

	.globl	_CPU_Context_switch
	.globl	_CPU_Context_restore

_CPU_Context_switch:

	ldw	r9, %gprel(_Per_CPU_Information + PER_CPU_ISR_DISPATCH_DISABLE)(gp)
	rdctl	r8, status
	stw	r16, NIOS2_CONTEXT_OFFSET_R16(r4)
	stw	r17, NIOS2_CONTEXT_OFFSET_R17(r4)
	stw	r18, NIOS2_CONTEXT_OFFSET_R18(r4)
	stw	r19, NIOS2_CONTEXT_OFFSET_R19(r4)
	stw	r20, NIOS2_CONTEXT_OFFSET_R20(r4)
	stw	r21, NIOS2_CONTEXT_OFFSET_R21(r4)
	stw	r22, NIOS2_CONTEXT_OFFSET_R22(r4)
	stw	r23, NIOS2_CONTEXT_OFFSET_R23(r4)
	stw	fp, NIOS2_CONTEXT_OFFSET_FP(r4)
	stw	r8, NIOS2_CONTEXT_OFFSET_STATUS(r4)
	stw	sp, NIOS2_CONTEXT_OFFSET_SP(r4)
	stw	ra, NIOS2_CONTEXT_OFFSET_RA(r4)
	stw	r9, NIOS2_CONTEXT_OFFSET_ISR_DISPATCH_DISABLE(r4)

restore:

	ldw	r10, NIOS2_CONTEXT_OFFSET_ISR_DISPATCH_DISABLE(r5)
	ldw	r16, NIOS2_CONTEXT_OFFSET_R16(r5)
	ldw	r17, NIOS2_CONTEXT_OFFSET_R17(r5)
	ldw	r18, NIOS2_CONTEXT_OFFSET_R18(r5)
	ldw	r19, NIOS2_CONTEXT_OFFSET_R19(r5)
	ldw	r20, NIOS2_CONTEXT_OFFSET_R20(r5)
	ldw	r21, NIOS2_CONTEXT_OFFSET_R21(r5)
	ldw	r22, NIOS2_CONTEXT_OFFSET_R22(r5)
	ldw	r23, NIOS2_CONTEXT_OFFSET_R23(r5)
	ldw	fp, NIOS2_CONTEXT_OFFSET_FP(r5)
	stw	r10, %gprel(_Per_CPU_Information + PER_CPU_ISR_DISPATCH_DISABLE)(gp)
	ldw	r11, NIOS2_CONTEXT_OFFSET_STATUS(r5)
	ldw	sp, NIOS2_CONTEXT_OFFSET_SP(r5)
	ldw	ra, NIOS2_CONTEXT_OFFSET_RA(r5)
	wrctl	status, r11

	ret

_CPU_Context_restore:

	mov	r5, r4
	br	restore
