/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreCPUMicroBlaze
 *
 * @brief MicroBlaze exception extensions ASM implementation
 */

/*
 * Copyright (C) 2022 On-Line Applications Research Corporation (OAR)
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

#include <rtems/asm.h>
#include <rtems/score/percpu.h>

	.text
        .globl _CPU_Exception_dispatch_and_resume
        .globl _MicroBlaze_Exception_resume_from_exception
        .globl _MicroBlaze_Exception_resume_from_break
	.align 2

_CPU_Exception_dispatch_and_resume:
	/* Subtract 1 from ISR_NEST_LEVEL */
	lwi r3, r0, _Per_CPU_Information + 8
	addik r3, r3, -1
	swi r3, r0, _Per_CPU_Information + 8

	/* Subtract 1 from THREAD_DISPATCH_DISABLE_LEVEL */
	lwi r3, r0, _Per_CPU_Information + 16
	addik r3, r3, -1
	swi r3, r0, _Per_CPU_Information + 16

	/* Is THREAD_DISPATCH_DISABLE_LEVEL != 0? */
	bnei r3, _MicroBlaze_Exception_resume_from_exception

	/* Is DISPATCH_NEEDED == 0? */
	lwi r3, r0, _Per_CPU_Information + 20
	beqi r3, _MicroBlaze_Exception_resume_from_exception

	bralid r15, _Thread_Dispatch
	nop
/* Fall through to restore exception frame */

_MicroBlaze_Exception_resume_from_exception:
	/* Move argument to stack pointer */
	addi r1, r5, 0

	/* Retrieve and store MSR */
	lwi r3, r1, MICROBLAZE_EXCEPTION_FRAME_MSR
	mts rmsr, r3

	/* Retrieve and store EAR */
	lwi r3, r1, MICROBLAZE_EXCEPTION_FRAME_EAR
	mts rear, r3

	/* Retrieve and store ESR */
	lwi r3, r1, MICROBLAZE_EXCEPTION_FRAME_ESR
	mts resr, r3

	/* Restore program state */
	lwi  r2, r1, MICROBLAZE_EXCEPTION_FRAME_R2
	lwi  r3, r1, MICROBLAZE_EXCEPTION_FRAME_R3
	lwi  r4, r1, MICROBLAZE_EXCEPTION_FRAME_R4
	lwi  r5, r1, MICROBLAZE_EXCEPTION_FRAME_R5
	lwi  r6, r1, MICROBLAZE_EXCEPTION_FRAME_R6
	lwi  r7, r1, MICROBLAZE_EXCEPTION_FRAME_R7
	lwi  r8, r1, MICROBLAZE_EXCEPTION_FRAME_R8
	lwi  r9, r1, MICROBLAZE_EXCEPTION_FRAME_R9
	lwi r10, r1, MICROBLAZE_EXCEPTION_FRAME_R10
	lwi r11, r1, MICROBLAZE_EXCEPTION_FRAME_R11
	lwi r12, r1, MICROBLAZE_EXCEPTION_FRAME_R12
	lwi r13, r1, MICROBLAZE_EXCEPTION_FRAME_R13
	lwi r14, r1, MICROBLAZE_EXCEPTION_FRAME_R14
	lwi r15, r1, MICROBLAZE_EXCEPTION_FRAME_R15
	lwi r16, r1, MICROBLAZE_EXCEPTION_FRAME_R16
	lwi r17, r1, MICROBLAZE_EXCEPTION_FRAME_R17
	lwi r18, r1, MICROBLAZE_EXCEPTION_FRAME_R18
	lwi r19, r1, MICROBLAZE_EXCEPTION_FRAME_R19
	lwi r20, r1, MICROBLAZE_EXCEPTION_FRAME_R20
	lwi r21, r1, MICROBLAZE_EXCEPTION_FRAME_R21
	lwi r22, r1, MICROBLAZE_EXCEPTION_FRAME_R22
	lwi r23, r1, MICROBLAZE_EXCEPTION_FRAME_R23
	lwi r24, r1, MICROBLAZE_EXCEPTION_FRAME_R24
	lwi r25, r1, MICROBLAZE_EXCEPTION_FRAME_R25
	lwi r26, r1, MICROBLAZE_EXCEPTION_FRAME_R26
	lwi r27, r1, MICROBLAZE_EXCEPTION_FRAME_R27
	lwi r28, r1, MICROBLAZE_EXCEPTION_FRAME_R28
	lwi r29, r1, MICROBLAZE_EXCEPTION_FRAME_R29
	lwi r30, r1, MICROBLAZE_EXCEPTION_FRAME_R30
	lwi r31, r1, MICROBLAZE_EXCEPTION_FRAME_R31

	/* Free stack space */
	addik r1, r1, CPU_EXCEPTION_FRAME_SIZE

	/* Return from exception mode */
	/* Branch to BTR is handled by upper layers */
	rted r17, 0
	nop

/* There is no dispatch version of resume from break */
_MicroBlaze_Exception_resume_from_break:
	/* Move argument to stack pointer */
	addi r1, r5, 0

	/* Retrieve and store MSR */
	lwi r3, r1, MICROBLAZE_EXCEPTION_FRAME_MSR
	mts rmsr, r3

	/* Retrieve and store EAR */
	lwi r3, r1, MICROBLAZE_EXCEPTION_FRAME_EAR
	mts rear, r3

	/* Retrieve and store ESR */
	lwi r3, r1, MICROBLAZE_EXCEPTION_FRAME_ESR
	mts resr, r3

	/* Restore program state */
	lwi  r2, r1, MICROBLAZE_EXCEPTION_FRAME_R2
	lwi  r3, r1, MICROBLAZE_EXCEPTION_FRAME_R3
	lwi  r4, r1, MICROBLAZE_EXCEPTION_FRAME_R4
	lwi  r5, r1, MICROBLAZE_EXCEPTION_FRAME_R5
	lwi  r6, r1, MICROBLAZE_EXCEPTION_FRAME_R6
	lwi  r7, r1, MICROBLAZE_EXCEPTION_FRAME_R7
	lwi  r8, r1, MICROBLAZE_EXCEPTION_FRAME_R8
	lwi  r9, r1, MICROBLAZE_EXCEPTION_FRAME_R9
	lwi r10, r1, MICROBLAZE_EXCEPTION_FRAME_R10
	lwi r11, r1, MICROBLAZE_EXCEPTION_FRAME_R11
	lwi r12, r1, MICROBLAZE_EXCEPTION_FRAME_R12
	lwi r13, r1, MICROBLAZE_EXCEPTION_FRAME_R13
	lwi r14, r1, MICROBLAZE_EXCEPTION_FRAME_R14
	lwi r15, r1, MICROBLAZE_EXCEPTION_FRAME_R15
	lwi r16, r1, MICROBLAZE_EXCEPTION_FRAME_R16
	lwi r17, r1, MICROBLAZE_EXCEPTION_FRAME_R17
	lwi r18, r1, MICROBLAZE_EXCEPTION_FRAME_R18
	lwi r19, r1, MICROBLAZE_EXCEPTION_FRAME_R19
	lwi r20, r1, MICROBLAZE_EXCEPTION_FRAME_R20
	lwi r21, r1, MICROBLAZE_EXCEPTION_FRAME_R21
	lwi r22, r1, MICROBLAZE_EXCEPTION_FRAME_R22
	lwi r23, r1, MICROBLAZE_EXCEPTION_FRAME_R23
	lwi r24, r1, MICROBLAZE_EXCEPTION_FRAME_R24
	lwi r25, r1, MICROBLAZE_EXCEPTION_FRAME_R25
	lwi r26, r1, MICROBLAZE_EXCEPTION_FRAME_R26
	lwi r27, r1, MICROBLAZE_EXCEPTION_FRAME_R27
	lwi r28, r1, MICROBLAZE_EXCEPTION_FRAME_R28
	lwi r29, r1, MICROBLAZE_EXCEPTION_FRAME_R29
	lwi r30, r1, MICROBLAZE_EXCEPTION_FRAME_R30
	lwi r31, r1, MICROBLAZE_EXCEPTION_FRAME_R31

	/* Free stack space */
	addik r1, r1, CPU_EXCEPTION_FRAME_SIZE

	/* Return from debug mode */
	rtbd r16, 0
	nop
