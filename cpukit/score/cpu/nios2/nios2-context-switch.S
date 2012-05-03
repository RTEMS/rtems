/*
 * Copyright (c) 2011 embedded brains GmbH
 *
 * Copyright (c) 2006 Kolja Waschk (rtemsdev/ixo.de)
 *
 * Derived from no_cpu/cpu_asm.S, copyright (c) 1989-1999,
 *   On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/cpu.h>
#include <rtems/score/nios2-utility.h>

	.extern	_Nios2_Thread_dispatch_disabled

	.globl	_CPU_Context_switch
	.globl	_CPU_Context_restore

_CPU_Context_switch:

	ldw	r9, %gprel(_Nios2_Thread_dispatch_disabled)(gp)
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
	stw	r9, NIOS2_CONTEXT_OFFSET_THREAD_DISPATCH_DISABLED(r4)

restore:

	ldw	r10, NIOS2_CONTEXT_OFFSET_THREAD_DISPATCH_DISABLED(r5)
	ldw	r16, NIOS2_CONTEXT_OFFSET_R16(r5)
	ldw	r17, NIOS2_CONTEXT_OFFSET_R17(r5)
	ldw	r18, NIOS2_CONTEXT_OFFSET_R18(r5)
	ldw	r19, NIOS2_CONTEXT_OFFSET_R19(r5)
	ldw	r20, NIOS2_CONTEXT_OFFSET_R20(r5)
	ldw	r21, NIOS2_CONTEXT_OFFSET_R21(r5)
	ldw	r22, NIOS2_CONTEXT_OFFSET_R22(r5)
	ldw	r23, NIOS2_CONTEXT_OFFSET_R23(r5)
	ldw	fp, NIOS2_CONTEXT_OFFSET_FP(r5)
	stw	r10, %gprel(_Nios2_Thread_dispatch_disabled)(gp)
	ldw	r11, NIOS2_CONTEXT_OFFSET_STATUS(r5)
	ldw	sp, NIOS2_CONTEXT_OFFSET_SP(r5)
	ldw	ra, NIOS2_CONTEXT_OFFSET_RA(r5)
	wrctl	status, r11

	ret

_CPU_Context_restore:

	mov	r5, r4
	br	restore
