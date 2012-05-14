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

#define CONTEXT_R8       0x00
#define CONTEXT_R9       0x04
#define CONTEXT_R10      0x08
#define CONTEXT_R11      0x0C
#define CONTEXT_R12      0x10
#define CONTEXT_R13_FP   0x14
#define CONTEXT_R14_LR   0x18
#define CONTEXT_R15_SP   0x1C
#define CONTEXT_ACC_LOW  0x20
#define CONTEXT_ACC_HIGH 0x24

	.file	"context_switch.S"
	.text
	.global	_CPU_Context_switch
	.type	_CPU_Context_switch, @function
_CPU_Context_switch:
	st      r8,  @(CONTEXT_R8,r0)
	st      r9,  @(CONTEXT_R9,r0)
	st      r10, @(CONTEXT_R10,r0)
	st      r11, @(CONTEXT_R11,r0)
	st      r12, @(CONTEXT_R12,r0)
	st      r13, @(CONTEXT_R13_FP,r0)
	st      r14, @(CONTEXT_R14_LR,r0)
	st      r15, @(CONTEXT_R15_SP,r0)
        mvfaclo r2
	st      r2,  @(CONTEXT_ACC_LOW,r0)
        mvfachi r2
	st      r2,  @(CONTEXT_ACC_HIGH,r0)

restore:
	ld      r8,  @(CONTEXT_R8,r1)
	ld      r9,  @(CONTEXT_R9,r1)
	ld      r10, @(CONTEXT_R10,r1)
	ld      r11, @(CONTEXT_R11,r1)
	ld      r12, @(CONTEXT_R12,r1)
	ld      r13, @(CONTEXT_R13_FP,r1)
	ld      r14, @(CONTEXT_R14_LR,r1)
	ld      r15, @(CONTEXT_R15_SP,r1)
	ld      r2,  @(CONTEXT_ACC_LOW,r1)
        mvtaclo r2
	ld      r2,  @(CONTEXT_ACC_HIGH,r1)
        mvtachi r2
        jmp     lr	

	.global	_CPU_Context_Restart_self
	.type	_CPU_Context_Restart_self, @function
_CPU_Context_Restart_self:
	mv	r1, r0
	bra	restore
