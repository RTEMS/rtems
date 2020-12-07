/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreCPUAArch64
 *
 * @brief Implementation of _CPU_Context_validate
 *
 * This file implements _CPU_Context_validate for use in spcontext01.
 */

/*
 * Copyright (C) 2020 On-Line Applications Research Corporation (OAR)
 * Written by Kinsey Moore <kinsey.moore@oarcorp.com>
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

#define FRAME_OFFSET_X4  0x00
#define FRAME_OFFSET_X5  0x08
#define FRAME_OFFSET_X6  0x10
#define FRAME_OFFSET_X7  0x18
#define FRAME_OFFSET_X8  0x20
#define FRAME_OFFSET_X9  0x28
#define FRAME_OFFSET_X10 0x30
#define FRAME_OFFSET_X11 0x38
#define FRAME_OFFSET_LR  0x40

#ifdef AARCH64_MULTILIB_VFP
  #define FRAME_OFFSET_V8  0x48
  #define FRAME_OFFSET_V9  0x58
  #define FRAME_OFFSET_V10 0x68
  #define FRAME_OFFSET_V11 0x78
  #define FRAME_OFFSET_V12 0x88
  #define FRAME_OFFSET_V13 0x98
  #define FRAME_OFFSET_V14 0xA8
  #define FRAME_OFFSET_V15 0xB8

  #define FRAME_SIZE (FRAME_OFFSET_V15 + 0x10)
#else
  #define FRAME_SIZE (FRAME_OFFSET_LR + 0x08)
#endif

	.section	.text

FUNCTION_ENTRY(_CPU_Context_validate)

	/* Save */

	sub	sp, sp, #FRAME_SIZE

	str	x4, [sp, #FRAME_OFFSET_X4]
	str	x5, [sp, #FRAME_OFFSET_X5]
	str	x6, [sp, #FRAME_OFFSET_X6]
	str	x7, [sp, #FRAME_OFFSET_X7]
	str	x8, [sp, #FRAME_OFFSET_X8]
	str	x9, [sp, #FRAME_OFFSET_X9]
	str	x10, [sp, #FRAME_OFFSET_X10]
	str	x11, [sp, #FRAME_OFFSET_X11]
	str	lr, [sp, #FRAME_OFFSET_LR]

#ifdef AARCH64_MULTILIB_VFP
	str	d8, [sp, #FRAME_OFFSET_V8]
	str	d9, [sp, #FRAME_OFFSET_V9]
	str	d10, [sp, #FRAME_OFFSET_V10]
	str	d11, [sp, #FRAME_OFFSET_V11]
	str	d12, [sp, #FRAME_OFFSET_V12]
	str	d13, [sp, #FRAME_OFFSET_V13]
	str	d14, [sp, #FRAME_OFFSET_V14]
	str	d15, [sp, #FRAME_OFFSET_V15]
#endif

	/* Fill */

	/* R1 is used for temporary values */
	mov	x1, x0

	/* R2 contains the stack pointer */
	mov	x2, sp

.macro fill_register reg
	add	x1, x1, #1
	mov	\reg, x1
.endm


#ifdef AARCH64_MULTILIB_VFP
	/* X3 contains the FPSCR */
	mrs	x3, FPSR
	ldr	x4, =0xf000001f
	bic	x3, x3, x4
	and	x4, x4, x0
	orr	x3, x3, x4
	msr	FPSR, x3
#else
	fill_register	x3
#endif

	fill_register	x4
	fill_register	x5
	fill_register	x6
	fill_register	x7
	fill_register	x8
	fill_register	x9
	fill_register	x10
	fill_register	x11
	fill_register	x12
	fill_register	lr

#ifdef AARCH64_MULTILIB_VFP
.macro fill_vfp_register regnum
	add	x1, x1, #1
	fmov	d\regnum\(), x1
	fmov	v\regnum\().D[1], x1
.endm

	fill_vfp_register	0
	fill_vfp_register	1
	fill_vfp_register	2
	fill_vfp_register	3
	fill_vfp_register	4
	fill_vfp_register	5
	fill_vfp_register	6
	fill_vfp_register	7
	fill_vfp_register	8
	fill_vfp_register	9
	fill_vfp_register	10
	fill_vfp_register	11
	fill_vfp_register	12
	fill_vfp_register	13
	fill_vfp_register	14
	fill_vfp_register	15
	fill_vfp_register	16
	fill_vfp_register	17
	fill_vfp_register	18
	fill_vfp_register	19
	fill_vfp_register	20
	fill_vfp_register	21
	fill_vfp_register	22
	fill_vfp_register	23
	fill_vfp_register	24
	fill_vfp_register	25
	fill_vfp_register	26
	fill_vfp_register	27
	fill_vfp_register	28
	fill_vfp_register	29
	fill_vfp_register	30
	fill_vfp_register	31
#endif /* AARCH64_MULTILIB_VFP */

	/* Check */
check:

.macro check_register reg
	add	x1, x1, #1
	cmp	\reg, x1
	bne	restore
.endm

	/* A compare involving the stack pointer is deprecated */
	mov	x1, sp
	cmp	x2, x1
	bne	restore

	mov	x1, x0

#ifndef AARCH64_MULTILIB_VFP
	check_register	x3
#endif

	check_register	x4
	check_register	x5
	check_register	x6
	check_register	x7
	check_register	x8
	check_register	x9
	check_register	x10
	check_register	x11
	check_register	x12
	check_register	lr

#ifdef AARCH64_MULTILIB_VFP
	b	check_vfp
#endif

	b	check

	/* Restore */
restore:

	ldr	x4, [sp, #FRAME_OFFSET_X4]
	ldr	x5, [sp, #FRAME_OFFSET_X5]
	ldr	x6, [sp, #FRAME_OFFSET_X6]
	ldr	x7, [sp, #FRAME_OFFSET_X7]
	ldr	x8, [sp, #FRAME_OFFSET_X8]
	ldr	x9, [sp, #FRAME_OFFSET_X9]
	ldr	x10, [sp, #FRAME_OFFSET_X10]
	ldr	x11, [sp, #FRAME_OFFSET_X11]
	ldr	lr, [sp, #FRAME_OFFSET_LR]

#ifdef AARCH64_MULTILIB_VFP
	ldr	d8, [sp, #FRAME_OFFSET_V8]
	ldr	d9, [sp, #FRAME_OFFSET_V9]
	ldr	d10, [sp, #FRAME_OFFSET_V10]
	ldr	d11, [sp, #FRAME_OFFSET_V11]
	ldr	d12, [sp, #FRAME_OFFSET_V12]
	ldr	d13, [sp, #FRAME_OFFSET_V13]
	ldr	d14, [sp, #FRAME_OFFSET_V14]
	ldr	d15, [sp, #FRAME_OFFSET_V15]
#endif

	add	sp, sp, #FRAME_SIZE

	ret

FUNCTION_END(_CPU_Context_validate)

#ifdef AARCH64_MULTILIB_VFP
check_vfp:

.macro check_vfp_register regnum
	add	x1, x1, #1
	fmov	x4, d\regnum
	fmov	x5, v\regnum\().D[1]
	cmp	x5, x4
	bne	1f
	cmp	x1, x4
	bne	1f
	b	2f
1:
	b	restore
2:
.endm

	mrs	x4, FPSR
	cmp	x4, x3
	bne	restore

	check_vfp_register	0
	check_vfp_register	1
	check_vfp_register	2
	check_vfp_register	3
	check_vfp_register	4
	check_vfp_register	5
	check_vfp_register	6
	check_vfp_register	7
	check_vfp_register	8
	check_vfp_register	9
	check_vfp_register	10
	check_vfp_register	11
	check_vfp_register	12
	check_vfp_register	13
	check_vfp_register	14
	check_vfp_register	15
	check_vfp_register	16
	check_vfp_register	17
	check_vfp_register	18
	check_vfp_register	19
	check_vfp_register	20
	check_vfp_register	21
	check_vfp_register	22
	check_vfp_register	23
	check_vfp_register	24
	check_vfp_register	25
	check_vfp_register	26
	check_vfp_register	27
	check_vfp_register	28
	check_vfp_register	29
	check_vfp_register	30
	check_vfp_register	31

	/* Restore x4 and x5 */
	mov	x1, x0
	fill_register	x4
	fill_register	x5

	b	check
#endif /* AARCH64_MULTILIB_VFP */
