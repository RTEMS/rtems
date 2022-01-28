/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreCPUMicroBlaze
 *
 * @brief MicroBlaze interrupt handler implementation
 */

/*
 * Copyright (C) 2021 On-Line Applications Research Corporation (OAR)
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
	.globl _ISR_Handler
	.align 2

_ISR_Handler:
	/* Save stack frame */
	swi  r3, r1, MICROBLAZE_INTERRUPT_FRAME_R3
	swi  r4, r1, MICROBLAZE_INTERRUPT_FRAME_R4
	swi  r6, r1, MICROBLAZE_INTERRUPT_FRAME_R6
	swi  r7, r1, MICROBLAZE_INTERRUPT_FRAME_R7
	swi  r8, r1, MICROBLAZE_INTERRUPT_FRAME_R8
	swi  r9, r1, MICROBLAZE_INTERRUPT_FRAME_R9
	swi r10, r1, MICROBLAZE_INTERRUPT_FRAME_R10
	swi r11, r1, MICROBLAZE_INTERRUPT_FRAME_R11
	swi r12, r1, MICROBLAZE_INTERRUPT_FRAME_R12
	swi r15, r1, MICROBLAZE_INTERRUPT_FRAME_R15
	swi r18, r1, MICROBLAZE_INTERRUPT_FRAME_R18
	mfs r3, rmsr
	swi  r3, r1, MICROBLAZE_INTERRUPT_FRAME_MSR

	/* Disable dispatching */
	lwi r3, r0, _Per_CPU_Information + 16
	addik r3, r3, 1
	swi r3, r0, _Per_CPU_Information + 16

	swi r14, r1, MICROBLAZE_INTERRUPT_FRAME_R14

	/* Is SP < INTERRUPT_STACK_LOW? */
	lwi r4, r0, _Per_CPU_Information
	rsubk r3, r4, r1
	blei r3, switch_to_interrupt_stack

	/* Is SP > INTERRUPT_STACK_HIGH? */
	lwi r4, r0, _Per_CPU_Information + 4
	rsubk r3, r4, r1
	bgei r3, switch_to_interrupt_stack

	bri on_interrupt_stack

switch_to_interrupt_stack:
	add r4, r0, r1
	lwi r1, r0, _Per_CPU_Information + 4
	addik r1, r1, -(CPU_INTERRUPT_FRAME_SIZE)
	swi r4, r1, 0

on_interrupt_stack:
	/* Add 1 to ISR_NEST_LEVEL */
	lwi r3, r0, _Per_CPU_Information + 8
	addik r3, r3, 1
	swi r3, r0, _Per_CPU_Information + 8

	bralid r15, bsp_interrupt_dispatch
	nop

	/* Subtract 1 from ISR_NEST_LEVEL */
	lwi r3, r0, _Per_CPU_Information + 8
	addik r3, r3, -1
	swi r3, r0, _Per_CPU_Information + 8

	/* Is ISR_NEST_LEVEL > 0? */
	bgti r3, after_stack_switch

	/* Switch back to interrupted thread stack */
	lwi r1, r1, 0

after_stack_switch:
	/* Subtract 1 from THREAD_DISPATCH_DISABLE_LEVEL */
	lwi r3, r0, _Per_CPU_Information + 16
	addik r3, r3, -1
	swi r3, r0, _Per_CPU_Information + 16

	/* Is THREAD_DISPATCH_DISABLE_LEVEL != 0? */
	bnei r3, quick_exit

	/* Is DISPATCH_NEEDED == 0? */
	lwi r3, r0, _Per_CPU_Information + 20
	beqi r3, quick_exit

	/* Return to interrupted thread and make it do a dispatch */
	bralid r15, _Thread_Dispatch
	nop
	/* Fall through to quick exit */

quick_exit:
	/* Simple return from nested interrupt */
	/* Restore registers */
	lwi  r3, r1, MICROBLAZE_INTERRUPT_FRAME_MSR
	mts  rmsr, r3
	lwi  r3, r1, MICROBLAZE_INTERRUPT_FRAME_R3
	lwi  r4, r1, MICROBLAZE_INTERRUPT_FRAME_R4
	lwi  r5, r1, MICROBLAZE_INTERRUPT_FRAME_R5
	lwi  r6, r1, MICROBLAZE_INTERRUPT_FRAME_R6
	lwi  r7, r1, MICROBLAZE_INTERRUPT_FRAME_R7
	lwi  r8, r1, MICROBLAZE_INTERRUPT_FRAME_R8
	lwi  r9, r1, MICROBLAZE_INTERRUPT_FRAME_R9
	lwi r10, r1, MICROBLAZE_INTERRUPT_FRAME_R10
	lwi r11, r1, MICROBLAZE_INTERRUPT_FRAME_R11
	lwi r12, r1, MICROBLAZE_INTERRUPT_FRAME_R12
	lwi r14, r1, MICROBLAZE_INTERRUPT_FRAME_R14
	lwi r15, r1, MICROBLAZE_INTERRUPT_FRAME_R15
	lwi r18, r1, MICROBLAZE_INTERRUPT_FRAME_R18

	/* Remove stack frame */
	addik r1, r1, CPU_INTERRUPT_FRAME_SIZE

	rtid r14, 0
	nop
