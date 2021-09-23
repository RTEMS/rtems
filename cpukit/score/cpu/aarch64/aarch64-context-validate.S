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
#include <rtems/score/basedefs.h>

/*
 * This register size applies to X (integer) registers as well as the D (lower
 * half floating point) registers. It does not apply to V (full size floating
 * point) registers or W (lower half integer) registers.
 */
#define AARCH64_REGISTER_SIZE 8

/* According to the AAPCS64, X19-X28 are callee-saved registers */
#define FRAME_OFFSET_X19  0x00
#define FRAME_OFFSET_X20  0x08
#define FRAME_OFFSET_X21  0x10
#define FRAME_OFFSET_X22  0x18
#define FRAME_OFFSET_X23  0x20
#define FRAME_OFFSET_X24  0x28
#define FRAME_OFFSET_X25  0x30
#define FRAME_OFFSET_X26  0x38
#define FRAME_OFFSET_X27  0x40
#define FRAME_OFFSET_X28  0x48
#define FRAME_OFFSET_LR   0x50

#ifdef AARCH64_MULTILIB_VFP
  /*
   * According to the AAPCS64, V8-V15 are callee-saved registers, but only the
   * bottom 8 bytes are required to be saved which correspond to D8-D15.
   */
  #define FRAME_OFFSET_D8  0x58
  #define FRAME_OFFSET_D9  0x60
  #define FRAME_OFFSET_D10 0x68
  #define FRAME_OFFSET_D11 0x70
  #define FRAME_OFFSET_D12 0x78
  #define FRAME_OFFSET_D13 0x80
  #define FRAME_OFFSET_D14 0x88
  #define FRAME_OFFSET_D15 0x90

  /*
   * Force 16 byte alignment of the frame size to avoid stack pointer alignment
   * exceptions.
   */
  #define FRAME_SIZE RTEMS_ALIGN_UP( FRAME_OFFSET_D15 + AARCH64_REGISTER_SIZE, 16 )
#else
  #define FRAME_SIZE RTEMS_ALIGN_UP( FRAME_OFFSET_LR + AARCH64_REGISTER_SIZE, 16 )
#endif

	.section	.text

FUNCTION_ENTRY(_CPU_Context_validate)

	/* Save */

	sub	sp, sp, #FRAME_SIZE

	str	x19, [sp, #FRAME_OFFSET_X19]
	str	x20, [sp, #FRAME_OFFSET_X20]
	str	x21, [sp, #FRAME_OFFSET_X21]
	str	x22, [sp, #FRAME_OFFSET_X22]
	str	x23, [sp, #FRAME_OFFSET_X23]
	str	x24, [sp, #FRAME_OFFSET_X24]
	str	x25, [sp, #FRAME_OFFSET_X25]
	str	x26, [sp, #FRAME_OFFSET_X26]
	str	x27, [sp, #FRAME_OFFSET_X27]
	str	x28, [sp, #FRAME_OFFSET_X28]
	str	lr, [sp, #FRAME_OFFSET_LR]

#ifdef AARCH64_MULTILIB_VFP
	str	d8, [sp, #FRAME_OFFSET_D8]
	str	d9, [sp, #FRAME_OFFSET_D9]
	str	d10, [sp, #FRAME_OFFSET_D10]
	str	d11, [sp, #FRAME_OFFSET_D11]
	str	d12, [sp, #FRAME_OFFSET_D12]
	str	d13, [sp, #FRAME_OFFSET_D13]
	str	d14, [sp, #FRAME_OFFSET_D14]
	str	d15, [sp, #FRAME_OFFSET_D15]
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
	/* X3 contains the FPSR */
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
	fill_register	x13
	fill_register	x14
	fill_register	x15
	fill_register	x16
	fill_register	x17
	fill_register	x18
	fill_register	x19
	fill_register	x20
	fill_register	x21
	fill_register	x22
	fill_register	x23
	fill_register	x24
	fill_register	x25
	fill_register	x26
	fill_register	x27
	fill_register	x28
	fill_register	x29
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
	check_register	x13
	check_register	x14
	check_register	x15
	check_register	x16
	check_register	x17
	check_register	x18
	check_register	x19
	check_register	x20
	check_register	x21
	check_register	x22
	check_register	x23
	check_register	x24
	check_register	x25
	check_register	x26
	check_register	x27
	check_register	x28
	check_register	x29
	check_register	lr

#ifdef AARCH64_MULTILIB_VFP
	b	check_vfp
#endif

	b	check

	/* Restore */
restore:

	ldr	x19, [sp, #FRAME_OFFSET_X19]
	ldr	x20, [sp, #FRAME_OFFSET_X20]
	ldr	x21, [sp, #FRAME_OFFSET_X21]
	ldr	x22, [sp, #FRAME_OFFSET_X22]
	ldr	x23, [sp, #FRAME_OFFSET_X23]
	ldr	x24, [sp, #FRAME_OFFSET_X24]
	ldr	x25, [sp, #FRAME_OFFSET_X25]
	ldr	x26, [sp, #FRAME_OFFSET_X26]
	ldr	x27, [sp, #FRAME_OFFSET_X27]
	ldr	x28, [sp, #FRAME_OFFSET_X28]
	ldr	lr, [sp, #FRAME_OFFSET_LR]

#ifdef AARCH64_MULTILIB_VFP
	ldr	d8, [sp, #FRAME_OFFSET_D8]
	ldr	d9, [sp, #FRAME_OFFSET_D9]
	ldr	d10, [sp, #FRAME_OFFSET_D10]
	ldr	d11, [sp, #FRAME_OFFSET_D11]
	ldr	d12, [sp, #FRAME_OFFSET_D12]
	ldr	d13, [sp, #FRAME_OFFSET_D13]
	ldr	d14, [sp, #FRAME_OFFSET_D14]
	ldr	d15, [sp, #FRAME_OFFSET_D15]
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
