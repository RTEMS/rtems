/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsMicroBlaze
 *
 * @brief MicroBlaze debug trap handler implementation
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

#include <rtems/score/cpu.h>

	.text
	.globl _debug_sw_break_handler    # HW Exception Handler Label
	.extern _MicroBlaze_Debug_handle
	.align 2

	_debug_sw_break_handler:
	/* The stack used here does not matter since debug cannot recurse */

	/* Subtract exception frame */
	addik r1, r1, -(CPU_EXCEPTION_FRAME_SIZE)

	/* Store program state */
	swi  r2, r1, MICROBLAZE_EXCEPTION_FRAME_R2
	swi  r3, r1, MICROBLAZE_EXCEPTION_FRAME_R3
	swi  r4, r1, MICROBLAZE_EXCEPTION_FRAME_R4
	swi  r5, r1, MICROBLAZE_EXCEPTION_FRAME_R5
	swi  r6, r1, MICROBLAZE_EXCEPTION_FRAME_R6
	swi  r7, r1, MICROBLAZE_EXCEPTION_FRAME_R7
	swi  r8, r1, MICROBLAZE_EXCEPTION_FRAME_R8
	swi  r9, r1, MICROBLAZE_EXCEPTION_FRAME_R9
	swi r10, r1, MICROBLAZE_EXCEPTION_FRAME_R10
	swi r11, r1, MICROBLAZE_EXCEPTION_FRAME_R11
	swi r12, r1, MICROBLAZE_EXCEPTION_FRAME_R12
	swi r13, r1, MICROBLAZE_EXCEPTION_FRAME_R13
	swi r14, r1, MICROBLAZE_EXCEPTION_FRAME_R14
	swi r15, r1, MICROBLAZE_EXCEPTION_FRAME_R15
	swi r16, r1, MICROBLAZE_EXCEPTION_FRAME_R16
	swi r17, r1, MICROBLAZE_EXCEPTION_FRAME_R17
	swi r18, r1, MICROBLAZE_EXCEPTION_FRAME_R18
	swi r19, r1, MICROBLAZE_EXCEPTION_FRAME_R19
	swi r20, r1, MICROBLAZE_EXCEPTION_FRAME_R20
	swi r21, r1, MICROBLAZE_EXCEPTION_FRAME_R21
	swi r22, r1, MICROBLAZE_EXCEPTION_FRAME_R22
	swi r23, r1, MICROBLAZE_EXCEPTION_FRAME_R23
	swi r24, r1, MICROBLAZE_EXCEPTION_FRAME_R24
	swi r25, r1, MICROBLAZE_EXCEPTION_FRAME_R25
	swi r26, r1, MICROBLAZE_EXCEPTION_FRAME_R26
	swi r27, r1, MICROBLAZE_EXCEPTION_FRAME_R27
	swi r28, r1, MICROBLAZE_EXCEPTION_FRAME_R28
	swi r29, r1, MICROBLAZE_EXCEPTION_FRAME_R29
	swi r30, r1, MICROBLAZE_EXCEPTION_FRAME_R30
	swi r31, r1, MICROBLAZE_EXCEPTION_FRAME_R31

	/* Retrieve and store MSR */
	mfs r3, rmsr
	swi r3, r1, MICROBLAZE_EXCEPTION_FRAME_MSR

	/* Retrieve and store EAR */
	mfs r3, rear
	swi r3, r1, MICROBLAZE_EXCEPTION_FRAME_EAR

	/* Retrieve and store ESR */
	mfs r3, resr
	swi r3, r1, MICROBLAZE_EXCEPTION_FRAME_ESR

	/* Retrieve and store BTR */
	mfs r3, rbtr
	swi r3, r1, MICROBLAZE_EXCEPTION_FRAME_BTR

	/* Calculate and store original stack pointer */
	addik r3, r1, CPU_EXCEPTION_FRAME_SIZE
	swi r3, r1, MICROBLAZE_EXCEPTION_FRAME_R1

	/* set parameter 1 to CPU Exception frame */
	addi r5, r1, 0

	/* call into the debug framework */
	braid _MicroBlaze_Debug_handle
	nop
