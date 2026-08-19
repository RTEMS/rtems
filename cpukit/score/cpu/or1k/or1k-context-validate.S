/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreCPUor1k
 *
 * @brief This source file contains the implementation of
 *   _CPU_Context_validate() for the OpenRISC 1000 architecture.
 */

/*
 * COPYRIGHT (c) 2014 Hesham ALMatary <heshamelmatary@gmail.com>
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
#include <rtems/score/cpu.h>

.section	.text

/*
 * The general purpose registers of the ABI are the stack pointer r1, the frame
 * pointer r2, the arguments r3 to r8, the return address r9, the thread
 * pointer r10, the return values r11 and r12, and r13 to r31.  The stack
 * pointer and the thread pointer belong to the environment of the thread and
 * are not part of the pattern.  r3 keeps the pattern and r13 is the temporary
 * of the check, so both are left out as well.
 */

#define FRAME_OFFSET(reg) ((reg) * 4)
#define FRAME_OFFSET_SP_XOR FRAME_OFFSET(32)
#define FRAME_SIZE (FRAME_OFFSET_SP_XOR + 8)

.macro	save_register reg
	l.sw	FRAME_OFFSET(\reg)(r1), r\reg
.endm

.macro	load_register reg
	l.lwz	r\reg, FRAME_OFFSET(\reg)(r1)
.endm

.macro	fill_register reg, inc
	l.addi	r\reg, r3, \inc
.endm

.macro	check_register reg, inc
	l.addi	r13, r3, \inc
	l.sfne	r\reg, r13
	l.bf	restore
	 l.nop
.endm

PUBLIC(_CPU_Context_validate)
SYM(_CPU_Context_validate):

	l.addi	r1, r1, -FRAME_SIZE

	/* Save */

	save_register	2
	save_register	9
	save_register	11
	save_register	12
	save_register	14
	save_register	15
	save_register	16
	save_register	17
	save_register	18
	save_register	19
	save_register	20
	save_register	21
	save_register	22
	save_register	23
	save_register	24
	save_register	25
	save_register	26
	save_register	27
	save_register	28
	save_register	29
	save_register	30
	save_register	31

	/*
	 * There is no register left to keep the stack pointer, so the check
	 * compares it through the stack.
	 */
	l.xor	r13, r1, r3
	l.sw	FRAME_OFFSET_SP_XOR(r1), r13

	/* Fill */

	fill_register	2, 1
	fill_register	4, 2
	fill_register	5, 3
	fill_register	6, 4
	fill_register	7, 5
	fill_register	8, 6
	fill_register	9, 7
	fill_register	11, 8
	fill_register	12, 9
	fill_register	14, 10
	fill_register	15, 11
	fill_register	16, 12
	fill_register	17, 13
	fill_register	18, 14
	fill_register	19, 15
	fill_register	20, 16
	fill_register	21, 17
	fill_register	22, 18
	fill_register	23, 19
	fill_register	24, 20
	fill_register	25, 21
	fill_register	26, 22
	fill_register	27, 23
	fill_register	28, 24
	fill_register	29, 25
	fill_register	30, 26
	fill_register	31, 27

	/* Check */

check:

	check_register	2, 1
	check_register	4, 2
	check_register	5, 3
	check_register	6, 4
	check_register	7, 5
	check_register	8, 6
	check_register	9, 7
	check_register	11, 8
	check_register	12, 9
	check_register	14, 10
	check_register	15, 11
	check_register	16, 12
	check_register	17, 13
	check_register	18, 14
	check_register	19, 15
	check_register	20, 16
	check_register	21, 17
	check_register	22, 18
	check_register	23, 19
	check_register	24, 20
	check_register	25, 21
	check_register	26, 22
	check_register	27, 23
	check_register	28, 24
	check_register	29, 25
	check_register	30, 26
	check_register	31, 27

	l.lwz	r13, FRAME_OFFSET_SP_XOR(r1)
	l.xor	r13, r13, r3
	l.sfne	r13, r1
	l.bf	restore
	 l.nop

	l.j	check
	 l.nop

	/* Restore */

restore:

	load_register	2
	load_register	9
	load_register	11
	load_register	12
	load_register	14
	load_register	15
	load_register	16
	load_register	17
	load_register	18
	load_register	19
	load_register	20
	load_register	21
	load_register	22
	load_register	23
	load_register	24
	load_register	25
	load_register	26
	load_register	27
	load_register	28
	load_register	29
	load_register	30
	load_register	31

	l.addi	r1, r1, FRAME_SIZE

	l.jr	r9
	 l.nop
