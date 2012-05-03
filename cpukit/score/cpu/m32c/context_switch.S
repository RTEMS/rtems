/*
 *  Context switch for the Reneas M32C
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define ARG_EXECUTING 8
#define ARG_HEIR      12

#define CTXT_SP  0
#define CTXT_FB  4

	.file	"context_switch.S"
	.text
	.global	__CPU_Context_switch
	.type	__CPU_Context_switch, @function
__CPU_Context_switch:
	enter	#0
	pushm 	a0,a1,r0,r1,r2,r3
	pushc	flg
	pushc	sb

#if defined(__r8c_cpu__)
  #warning "not implemented on R8C"
#else
	mov.l	ARG_EXECUTING[fb],a0	; a0 = executing
	stc  	fb,a1
	mov.l	a1,CTXT_FB[a0]		; save fb
	stc  	sp,a1
	mov.l	a1,CTXT_SP[a0]		; save sp

	mov.l	ARG_HEIR[fb],a0		; a0 = heir

restore:
	mov.l	CTXT_SP[a0],a1
	ldc  	a1,sp			; restore sp
	mov.l	CTXT_FB[a0],a1
	ldc  	a1,fb			; restore fb
#endif
	popc	sb
	popc	flg
	popm 	a0,a1,r0,r1,r2,r3
	exitd
	.size	__CPU_Context_switch, .-__CPU_Context_switch

#define ARG_RESTART 8

	.global	__CPU_Context_Restart_self
	.type	__CPU_Context_Restart_self, @function
__CPU_Context_Restart_self:
	enter	#0
#if defined(__r8c_cpu__)
  #warning "__CPU_Context_Restart_self: not implemented on R8C"
#else
	mov.l	ARG_RESTART[fb],a0	; a0 = heir
	jmp.s	restore
#endif
