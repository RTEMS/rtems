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
#include <rtems/mips/idtcpu.h>
#include <rtems/score/mips.h>

#if __mips == 3
#define LDREG ld
#define MTCO dmtc0
#define R_SZ 8
#if __mips_fpr == 32
#define LDREGC1 lwc1
#else
#define LDREGC1 ldc1
#endif
#elif __mips == 1 || __mips == 32
#define LDREG lw
#define LDREGC1 lwc1
#define MTCO mtc0
#define R_SZ 4
#else
#error "unknown MIPS ISA"
#endif

#define EXCP_STACK_SIZE (NREGS*R_SZ)

/*
 *  void _CPU_Exception_resume( CPU_Exception_frame *frame )
 *
 *  This routine restores the register set of the frame and continues the
 *  execution at the exception program counter of the frame.
 *
 *  _ISR_Handler() builds the frame on the stack of the interrupted context.
 *  The exception path of it stores every integer register and every floating
 *  point register.
 *
 *  The stack pointer of the frame holds the address of the frame, because the
 *  prologue stores the register after it allocates the frame.  The interrupted
 *  value is that address plus the size of the frame, which this routine
 *  computes.  The exit of the handler adds the same size to its own stack
 *  pointer.
 *
 *  This routine restores neither the stack pointer, which it computes, nor k0
 *  and k1, which carry its own state.  It restores neither the cause nor the
 *  bad virtual address, which no write of the processor accepts.
 *
 *  k0 and k1 belong to the operating system, so they carry the frame and the
 *  scratch value of this routine.
 */

FRAME(_CPU_Exception_resume,sp,0,ra)

	.set noreorder

	move	k0,a0

	/*
	 * k1 keeps the exception program counter, because the mips1 return
	 * jumps to it rather than to a control register.
	 */
	LDREG	k1,R_EPC*R_SZ(k0)

#if MIPS_HAS_FPU == 1
	/*
	 * The floating point registers go before the write of the status
	 * register, so the write of the control and status register of
	 * coprocessor 1 meets the coprocessor enable of the handler.
	 */
	LDREG	t8,R_FCSR*R_SZ(k0)
	NOP
	ctc1	t8,$31
	NOP
	LDREGC1	$f0,R_F0*R_SZ(k0)
	LDREGC1	$f1,R_F1*R_SZ(k0)
	LDREGC1	$f2,R_F2*R_SZ(k0)
	LDREGC1	$f3,R_F3*R_SZ(k0)
	LDREGC1	$f4,R_F4*R_SZ(k0)
	LDREGC1	$f5,R_F5*R_SZ(k0)
	LDREGC1	$f6,R_F6*R_SZ(k0)
	LDREGC1	$f7,R_F7*R_SZ(k0)
	LDREGC1	$f8,R_F8*R_SZ(k0)
	LDREGC1	$f9,R_F9*R_SZ(k0)
	LDREGC1	$f10,R_F10*R_SZ(k0)
	LDREGC1	$f11,R_F11*R_SZ(k0)
	LDREGC1	$f12,R_F12*R_SZ(k0)
	LDREGC1	$f13,R_F13*R_SZ(k0)
	LDREGC1	$f14,R_F14*R_SZ(k0)
	LDREGC1	$f15,R_F15*R_SZ(k0)
	LDREGC1	$f16,R_F16*R_SZ(k0)
	LDREGC1	$f17,R_F17*R_SZ(k0)
	LDREGC1	$f18,R_F18*R_SZ(k0)
	LDREGC1	$f19,R_F19*R_SZ(k0)
	LDREGC1	$f20,R_F20*R_SZ(k0)
	LDREGC1	$f21,R_F21*R_SZ(k0)
	LDREGC1	$f22,R_F22*R_SZ(k0)
	LDREGC1	$f23,R_F23*R_SZ(k0)
	LDREGC1	$f24,R_F24*R_SZ(k0)
	LDREGC1	$f25,R_F25*R_SZ(k0)
	LDREGC1	$f26,R_F26*R_SZ(k0)
	LDREGC1	$f27,R_F27*R_SZ(k0)
	LDREGC1	$f28,R_F28*R_SZ(k0)
	LDREGC1	$f29,R_F29*R_SZ(k0)
	LDREGC1	$f30,R_F30*R_SZ(k0)
	LDREGC1	$f31,R_F31*R_SZ(k0)
#endif

#if (__mips == 3) || (__mips == 32)
	MTCO	k1,C0_EPC
	NOP

	/*
	 * _Terminate() clears the interrupt enable before it calls the fatal
	 * error extensions, so the status register of the frame is the only
	 * source of the level which the interrupted context had.  The eret
	 * needs the exception level, which it clears again.
	 */
	LDREG	t8,R_SR*R_SZ(k0)
	NOP
	or	t8,SR_EXL
	mtc0	t8,C0_SR
	NOP
#endif


	LDREG	t8,R_MDLO*R_SZ(k0)
	mtlo	t8
	LDREG	t8,R_MDHI*R_SZ(k0)
	mthi	t8

	/*
	 * The chain which reaches this routine ends in a fatal error, so no
	 * function of it returns and no function of it puts a call-saved
	 * register back.  The exception path of _ISR_Handler() stores s0 to
	 * s7, so the frame is the only source of them.
	 */
	LDREG	s0,R_S0*R_SZ(k0)
	LDREG	s1,R_S1*R_SZ(k0)
	LDREG	s2,R_S2*R_SZ(k0)
	LDREG	s3,R_S3*R_SZ(k0)
	LDREG	s4,R_S4*R_SZ(k0)
	LDREG	s5,R_S5*R_SZ(k0)
	LDREG	s6,R_S6*R_SZ(k0)
	LDREG	s7,R_S7*R_SZ(k0)

	LDREG	v0,R_V0*R_SZ(k0)
	LDREG	v1,R_V1*R_SZ(k0)
	LDREG	a1,R_A1*R_SZ(k0)
	LDREG	a2,R_A2*R_SZ(k0)
	LDREG	a3,R_A3*R_SZ(k0)
	LDREG	t0,R_T0*R_SZ(k0)
	LDREG	t1,R_T1*R_SZ(k0)
	LDREG	t2,R_T2*R_SZ(k0)
	LDREG	t3,R_T3*R_SZ(k0)
	LDREG	t4,R_T4*R_SZ(k0)
	LDREG	t5,R_T5*R_SZ(k0)
	LDREG	t6,R_T6*R_SZ(k0)
	LDREG	t7,R_T7*R_SZ(k0)
	LDREG	t8,R_T8*R_SZ(k0)
	LDREG	t9,R_T9*R_SZ(k0)
	LDREG	gp,R_GP*R_SZ(k0)
	LDREG	fp,R_FP*R_SZ(k0)
	LDREG	ra,R_RA*R_SZ(k0)

	.set noat
	LDREG	AT,R_AT*R_SZ(k0)
	.set at

	/* a0 holds the frame, so it goes after every other load */
	LDREG	a0,R_A0*R_SZ(k0)

	addiu	sp,k0,EXCP_STACK_SIZE

#if (__mips == 3) || (__mips == 32)
	eret
#elif __mips == 1
	j	k1
	rfe
#endif
	NOP

	.set reorder

ENDFRAME(_CPU_Exception_resume)
