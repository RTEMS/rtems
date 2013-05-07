/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/asm.h>

#define FRAME_OFFSET_R4 0
#define FRAME_OFFSET_R5 4
#define FRAME_OFFSET_R6 8
#define FRAME_OFFSET_R7 12
#define FRAME_OFFSET_R8 16
#define FRAME_OFFSET_R9 20
#define FRAME_OFFSET_R10 24
#define FRAME_OFFSET_R11 28
#define FRAME_OFFSET_LR 32

#define FRAME_SIZE (FRAME_OFFSET_LR + 4)

	.section	.text

FUNCTION_THUMB_ENTRY(_CPU_Context_validate)

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

	/* Fill */

	/* R1 is used for temporary values */
	mov	r1, r0

	/* R2 contains the stack pointer */
	mov	r2, sp

.macro fill_register reg
	add	r1, r1, #1
	mov	\reg, r1
.endm

	fill_register	r3
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

	check_register	r3
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

	add	sp, sp, #FRAME_SIZE

	bx	lr

FUNCTION_END(_CPU_Context_validate)
