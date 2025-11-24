/* SPDX-License-Identifier: BSD-3-Clause */

/* Copyright (c) 2001, 2009 Xilinx, Inc.  All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:

   1.  Redistributions source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

   2.  Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

   3.  Neither the name of Xilinx nor the names of its contributors may be
   used to endorse or promote products derived from this software without
   specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS "AS
   IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
   TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
   PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
   HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
   TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include <rtems/score/cpuimpl.h>

	.text
	.globl _interrupt_handler    # Interrupt Handler Label
	.align 2

	_interrupt_handler:
#ifndef __rtems__
	rtid    r14, 0
	nop
#else /* __rtems__ */
	/* Subtract stack frame */
	addik r1, r1, -(CPU_INTERRUPT_FRAME_SIZE)

	/* Save stack frame */
	swi  r3, r1, MICROBLAZE_INTERRUPT_FRAME_R3
	swi  r4, r1, MICROBLAZE_INTERRUPT_FRAME_R4
	swi  r5, r1, MICROBLAZE_INTERRUPT_FRAME_R5
	swi  r6, r1, MICROBLAZE_INTERRUPT_FRAME_R6
	swi  r7, r1, MICROBLAZE_INTERRUPT_FRAME_R7
	swi  r8, r1, MICROBLAZE_INTERRUPT_FRAME_R8
	swi  r9, r1, MICROBLAZE_INTERRUPT_FRAME_R9
	swi r10, r1, MICROBLAZE_INTERRUPT_FRAME_R10
	swi r11, r1, MICROBLAZE_INTERRUPT_FRAME_R11
	swi r12, r1, MICROBLAZE_INTERRUPT_FRAME_R12
	swi r14, r1, MICROBLAZE_INTERRUPT_FRAME_R14
	swi r15, r1, MICROBLAZE_INTERRUPT_FRAME_R15
	swi r18, r1, MICROBLAZE_INTERRUPT_FRAME_R18
	mfs r3, rmsr
	swi  r3, r1, MICROBLAZE_INTERRUPT_FRAME_MSR

	/* Indicate unknown interrupt source; 0xFF tells _ISR_Handler to query the INTC. */
	/* Use addik so MSR flags stay unchanged before calling the handler. */
	/* braid has a delay slot; addik will occur prior to the branch. */
	braid _ISR_Handler
	addik r5, r0, 0xFF
#endif /* __rtems__ */
