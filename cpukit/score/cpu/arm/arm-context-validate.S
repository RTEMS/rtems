/*
 * Copyright (c) 2013, 2017 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/asm.h>
#include <rtems/score/cpu.h>

#define FRAME_OFFSET_R4 0
#define FRAME_OFFSET_R5 4
#define FRAME_OFFSET_R6 8
#define FRAME_OFFSET_R7 12
#define FRAME_OFFSET_R8 16
#define FRAME_OFFSET_R9 20
#define FRAME_OFFSET_R10 24
#define FRAME_OFFSET_R11 28
#define FRAME_OFFSET_LR 32

#ifdef ARM_MULTILIB_VFP
  #define FRAME_OFFSET_D8 40
  #define FRAME_OFFSET_D9 48
  #define FRAME_OFFSET_D10 56
  #define FRAME_OFFSET_D11 64
  #define FRAME_OFFSET_D12 72
  #define FRAME_OFFSET_D13 80
  #define FRAME_OFFSET_D14 88
  #define FRAME_OFFSET_D15 96

  #define FRAME_SIZE (FRAME_OFFSET_D15 + 8)
#else
  #define FRAME_SIZE (FRAME_OFFSET_LR + 4)
#endif

	.section	.text

#ifdef __thumb__
FUNCTION_THUMB_ENTRY(_CPU_Context_validate)
#else
FUNCTION_ENTRY(_CPU_Context_validate)
#endif

	/* Save */

	sub	sp, sp, #FRAME_SIZE

	mov	r1, r4
	str	r1, [sp, #FRAME_OFFSET_R4]
	mov	r1, r5
	str	r1, [sp, #FRAME_OFFSET_R5]
	mov	r1, r6
	str	r1, [sp, #FRAME_OFFSET_R6]
	mov	r1, r7
	str	r1, [sp, #FRAME_OFFSET_R7]
	mov	r1, r8
	str	r1, [sp, #FRAME_OFFSET_R8]
	mov	r1, r9
	str	r1, [sp, #FRAME_OFFSET_R9]
	mov	r1, r10
	str	r1, [sp, #FRAME_OFFSET_R10]
	mov	r1, r11
	str	r1, [sp, #FRAME_OFFSET_R11]
	mov	r1, lr
	str	r1, [sp, #FRAME_OFFSET_LR]

#ifdef ARM_MULTILIB_VFP
	vstr	d8, [sp, #FRAME_OFFSET_D8]
	vstr	d9, [sp, #FRAME_OFFSET_D9]
	vstr	d10, [sp, #FRAME_OFFSET_D10]
	vstr	d11, [sp, #FRAME_OFFSET_D11]
	vstr	d12, [sp, #FRAME_OFFSET_D12]
	vstr	d13, [sp, #FRAME_OFFSET_D13]
	vstr	d14, [sp, #FRAME_OFFSET_D14]
	vstr	d15, [sp, #FRAME_OFFSET_D15]
#endif

	/* Fill */

	/* R1 is used for temporary values */
	mov	r1, r0

	/* R2 contains the stack pointer */
	mov	r2, sp

.macro fill_register reg
	add	r1, r1, #1
	mov	\reg, r1
.endm


#ifdef ARM_MULTILIB_VFP
	/* R3 contains the FPSCR */
	vmrs	r3, FPSCR
	ldr	r4, =0xf000001f
	bic	r3, r3, r4
	and	r4, r4, r0
	orr	r3, r3, r4
	vmsr	FPSCR, r3
#else
	fill_register	r3
#endif

	fill_register	r4
	fill_register	r5
	fill_register	r6
	fill_register	r7
	fill_register	r8
	fill_register	r9
	fill_register	r10
	fill_register	r11
	fill_register	r12
	fill_register	lr

#ifdef ARM_MULTILIB_VFP
.macro fill_vfp_register reg
	add	r1, r1, #1
	vmov	\reg, r1, r1
.endm

	fill_vfp_register	d0
	fill_vfp_register	d1
	fill_vfp_register	d2
	fill_vfp_register	d3
	fill_vfp_register	d4
	fill_vfp_register	d5
	fill_vfp_register	d6
	fill_vfp_register	d7
	fill_vfp_register	d8
	fill_vfp_register	d9
	fill_vfp_register	d10
	fill_vfp_register	d11
	fill_vfp_register	d12
	fill_vfp_register	d13
	fill_vfp_register	d14
	fill_vfp_register	d15
#ifdef ARM_MULTILIB_VFP_D32
	fill_vfp_register	d16
	fill_vfp_register	d17
	fill_vfp_register	d18
	fill_vfp_register	d19
	fill_vfp_register	d20
	fill_vfp_register	d21
	fill_vfp_register	d22
	fill_vfp_register	d23
	fill_vfp_register	d24
	fill_vfp_register	d25
	fill_vfp_register	d26
	fill_vfp_register	d27
	fill_vfp_register	d28
	fill_vfp_register	d29
	fill_vfp_register	d30
	fill_vfp_register	d31
#endif /* ARM_MULTILIB_VFP_D32 */
#endif /* ARM_MULTILIB_VFP */

	/* Check */
check:

.macro check_register reg
	add	r1, r1, #1
	cmp	\reg, r1
	bne	restore
.endm

	cmp	r2, sp
	bne	restore

	mov	r1, r0

#ifndef ARM_MULTILIB_VFP
	check_register	r3
#endif

	check_register	r4
	check_register	r5
	check_register	r6
	check_register	r7
	check_register	r8
	check_register	r9
	check_register	r10
	check_register	r11
	check_register	r12
	check_register	lr

#ifdef ARM_MULTILIB_VFP
	b	check_vfp
#endif

	b	check

	/* Restore */
restore:

	ldr	r1, [sp, #FRAME_OFFSET_R4]
	mov	r4, r1
	ldr	r1, [sp, #FRAME_OFFSET_R5]
	mov	r5, r1
	ldr	r1, [sp, #FRAME_OFFSET_R6]
	mov	r6, r1
	ldr	r1, [sp, #FRAME_OFFSET_R7]
	mov	r7, r1
	ldr	r1, [sp, #FRAME_OFFSET_R8]
	mov	r8, r1
	ldr	r1, [sp, #FRAME_OFFSET_R9]
	mov	r9, r1
	ldr	r1, [sp, #FRAME_OFFSET_R10]
	mov	r10, r1
	ldr	r1, [sp, #FRAME_OFFSET_R11]
	mov	r11, r1
	ldr	r1, [sp, #FRAME_OFFSET_LR]
	mov	lr, r1

#ifdef ARM_MULTILIB_VFP
	vldr	d8, [sp, #FRAME_OFFSET_D8]
	vldr	d9, [sp, #FRAME_OFFSET_D9]
	vldr	d10, [sp, #FRAME_OFFSET_D10]
	vldr	d11, [sp, #FRAME_OFFSET_D11]
	vldr	d12, [sp, #FRAME_OFFSET_D12]
	vldr	d13, [sp, #FRAME_OFFSET_D13]
	vldr	d14, [sp, #FRAME_OFFSET_D14]
	vldr	d15, [sp, #FRAME_OFFSET_D15]
#endif

	add	sp, sp, #FRAME_SIZE

	bx	lr

FUNCTION_END(_CPU_Context_validate)

#ifdef ARM_MULTILIB_VFP
check_vfp:

.macro check_vfp_register reg
	add	r1, r1, #1
	vmov	r4, r5, \reg
	cmp	r4, r5
	bne	1f
	cmp	r1, r4
	bne	1f
	b	2f
1:
	b	restore
2:
.endm

	vmrs	r4, FPSCR
	cmp	r4, r3
	bne	restore

	check_vfp_register	d0
	check_vfp_register	d1
	check_vfp_register	d2
	check_vfp_register	d3
	check_vfp_register	d4
	check_vfp_register	d5
	check_vfp_register	d6
	check_vfp_register	d7
	check_vfp_register	d8
	check_vfp_register	d9
	check_vfp_register	d10
	check_vfp_register	d11
	check_vfp_register	d12
	check_vfp_register	d13
	check_vfp_register	d14
	check_vfp_register	d15
#ifdef ARM_MULTILIB_VFP_D32
	check_vfp_register	d16
	check_vfp_register	d17
	check_vfp_register	d18
	check_vfp_register	d19
	check_vfp_register	d20
	check_vfp_register	d21
	check_vfp_register	d22
	check_vfp_register	d23
	check_vfp_register	d24
	check_vfp_register	d25
	check_vfp_register	d26
	check_vfp_register	d27
	check_vfp_register	d28
	check_vfp_register	d29
	check_vfp_register	d30
	check_vfp_register	d31
#endif /* ARM_MULTILIB_VFP_D32 */

	/* Restore r4 and r5 */
	mov	r1, r0
	fill_register	r4
	fill_register	r5

	b	check
#endif /* ARM_MULTILIB_VFP */
