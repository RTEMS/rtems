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

/*
 * The pattern arrives in $r0 and stays there, $r1 is the temporary of the
 * check, so the registers of the pattern are $r2 to $r13.  The frame pointer
 * is the stack pointer of the return instruction of this architecture and
 * belongs to the environment of the thread, like the stack pointer itself.
 * The stack pointer is compared through the frame, since no register is left
 * for it.
 */

#define FRAME_OFFSET_R6 0
#define FRAME_OFFSET_R7 4
#define FRAME_OFFSET_R8 8
#define FRAME_OFFSET_R9 12
#define FRAME_OFFSET_R10 16
#define FRAME_OFFSET_R11 20
#define FRAME_OFFSET_R12 24
#define FRAME_OFFSET_R13 28
#define FRAME_OFFSET_SP 32
#define FRAME_SIZE 40

	.text
	.align 2
	.global SYM(_CPU_Context_validate)

SYM(_CPU_Context_validate):
	dec	$sp, FRAME_SIZE

	sto.l	FRAME_OFFSET_R6($sp), $r6
	sto.l	FRAME_OFFSET_R7($sp), $r7
	sto.l	FRAME_OFFSET_R8($sp), $r8
	sto.l	FRAME_OFFSET_R9($sp), $r9
	sto.l	FRAME_OFFSET_R10($sp), $r10
	sto.l	FRAME_OFFSET_R11($sp), $r11
	sto.l	FRAME_OFFSET_R12($sp), $r12
	sto.l	FRAME_OFFSET_R13($sp), $r13
	sto.l	FRAME_OFFSET_SP($sp), $sp

	/* Fill */

.macro fill_register reg, inc
	mov	\reg, $r0
	inc	\reg, \inc
.endm

	fill_register	$r2, 1
	fill_register	$r3, 2
	fill_register	$r4, 3
	fill_register	$r5, 4
	fill_register	$r6, 5
	fill_register	$r7, 6
	fill_register	$r8, 7
	fill_register	$r9, 8
	fill_register	$r10, 9
	fill_register	$r11, 10
	fill_register	$r12, 11
	fill_register	$r13, 12

	/* Check */

.macro check_register reg, inc
	mov	$r1, $r0
	inc	$r1, \inc
	cmp	\reg, $r1
	bne	.Lrestore
.endm

.Lcheck:
	check_register	$r2, 1
	check_register	$r3, 2
	check_register	$r4, 3
	check_register	$r5, 4
	check_register	$r6, 5
	check_register	$r7, 6
	check_register	$r8, 7
	check_register	$r9, 8
	check_register	$r10, 9
	check_register	$r11, 10
	check_register	$r12, 11
	check_register	$r13, 12

	ldo.l	$r1, FRAME_OFFSET_SP($sp)
	cmp	$r1, $sp
	bne	.Lrestore

	jmpa	.Lcheck

	/* Restore */

.Lrestore:
	ldo.l	$r6, FRAME_OFFSET_R6($sp)
	ldo.l	$r7, FRAME_OFFSET_R7($sp)
	ldo.l	$r8, FRAME_OFFSET_R8($sp)
	ldo.l	$r9, FRAME_OFFSET_R9($sp)
	ldo.l	$r10, FRAME_OFFSET_R10($sp)
	ldo.l	$r11, FRAME_OFFSET_R11($sp)
	ldo.l	$r12, FRAME_OFFSET_R12($sp)
	ldo.l	$r13, FRAME_OFFSET_R13($sp)

	inc	$sp, FRAME_SIZE

	ret
