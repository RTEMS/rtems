/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  @ingroup RTEMSScoreCPUMicroBlaze
 *
 *  @brief MicroBlaze context validate implementation
 */

/*
 * COPYRIGHT (C) 2021 On-Line Applications Research Corporation (OAR).
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

#define FRAME_OFFSET_R19 0
#define FRAME_OFFSET_R20 4
#define FRAME_OFFSET_R21 8
#define FRAME_OFFSET_R22 12
#define FRAME_OFFSET_R23 16
#define FRAME_OFFSET_R24 20
#define FRAME_OFFSET_R25 24
#define FRAME_OFFSET_R26 28
#define FRAME_OFFSET_R27 32
#define FRAME_OFFSET_R28 36
#define FRAME_OFFSET_R29 40
#define FRAME_OFFSET_R30 44
#define FRAME_OFFSET_R31 48

#define FRAME_SIZE (FRAME_OFFSET_R31 + 4)

.text
.align 4

PUBLIC(_CPU_Context_validate)

SYM(_CPU_Context_validate):

	/* Save */
	addik r1, r1, -FRAME_SIZE
	swi r19, r1, FRAME_OFFSET_R19
	swi r20, r1, FRAME_OFFSET_R20
	swi r21, r1, FRAME_OFFSET_R21
	swi r22, r1, FRAME_OFFSET_R22
	swi r23, r1, FRAME_OFFSET_R23
	swi r24, r1, FRAME_OFFSET_R24
	swi r25, r1, FRAME_OFFSET_R25
	swi r26, r1, FRAME_OFFSET_R26
	swi r27, r1, FRAME_OFFSET_R27
	swi r28, r1, FRAME_OFFSET_R28
	swi r29, r1, FRAME_OFFSET_R29
	swi r30, r1, FRAME_OFFSET_R30
	swi r31, r1, FRAME_OFFSET_R31

	/* Fill */

	add r4, r0, r3

	/* r7 contains the stack pointer */
	add r7, r0, r1

.macro fill_register reg
	addi r4, r4, 1
	add \reg, r0, r4
.endm

	fill_register r21
	fill_register r22
	fill_register r23
	fill_register r24
	fill_register r25
	fill_register r26
	fill_register r27
	fill_register r28
	fill_register r29
	fill_register r30
	fill_register r31

	/* Check */
check:

.macro check_register reg
	addi r4, r4, 1
	cmp r6, \reg, r4
	bnei r6, restore
.endm

	cmp r6, r7, r1
	bnei r6, restore

	add r4, r0, r3

	check_register r21
	check_register r22
	check_register r23
	check_register r24
	check_register r25
	check_register r26
	check_register r27
	check_register r28
	check_register r29
	check_register r30
	check_register r31

	brai check

	/* Restore */
restore:

	lwi r19, r1, FRAME_OFFSET_R19
	lwi r20, r1, FRAME_OFFSET_R20
	lwi r21, r1, FRAME_OFFSET_R21
	lwi r22, r1, FRAME_OFFSET_R22
	lwi r23, r1, FRAME_OFFSET_R23
	lwi r24, r1, FRAME_OFFSET_R24
	lwi r25, r1, FRAME_OFFSET_R25
	lwi r26, r1, FRAME_OFFSET_R26
	lwi r27, r1, FRAME_OFFSET_R27
	lwi r28, r1, FRAME_OFFSET_R28
	lwi r29, r1, FRAME_OFFSET_R29
	lwi r30, r1, FRAME_OFFSET_R30
	lwi r31, r1, FRAME_OFFSET_R31

	addik r1, r1, FRAME_SIZE

	bra r15
