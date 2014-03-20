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
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/asm.h>

#define FRAME_OFFSET_R16 0
#define FRAME_OFFSET_R17 4
#define FRAME_OFFSET_R18 8
#define FRAME_OFFSET_R19 12
#define FRAME_OFFSET_R20 16
#define FRAME_OFFSET_R21 20
#define FRAME_OFFSET_R22 24
#define FRAME_OFFSET_R23 28
#define FRAME_OFFSET_FP 32
#define FRAME_OFFSET_RA 36

#define FRAME_SIZE (FRAME_OFFSET_RA + 4)

	.set	noat
	.section	.text

	.global _CPU_Context_validate

_CPU_Context_validate:

	/* Save */
	subi	sp, sp, FRAME_SIZE
	stw	r16, FRAME_OFFSET_R16(sp)
	stw	r17, FRAME_OFFSET_R17(sp)
	stw	r18, FRAME_OFFSET_R18(sp)
	stw	r19, FRAME_OFFSET_R19(sp)
	stw	r20, FRAME_OFFSET_R20(sp)
	stw	r21, FRAME_OFFSET_R21(sp)
	stw	r22, FRAME_OFFSET_R22(sp)
	stw	r23, FRAME_OFFSET_R23(sp)
	stw	fp, FRAME_OFFSET_FP(sp)
	stw	ra, FRAME_OFFSET_RA(sp)

	/* Fill */

	/* AT contains the stack pointer */
	mov	at, sp

	/* R2 is used for temporary values */

	subi	r3, r4, 1
	addi	r5, r4, 1
	addi	r6, r4, 2
	addi	r7, r4, 3
	addi	r8, r4, 4
	addi	r9, r4, 5
	addi	r10, r4, 6
	addi	r11, r4, 7
	addi	r12, r4, 8
	addi	r13, r4, 9
	addi	r14, r4, 10
	addi	r15, r4, 11
	addi	r16, r4, 12
	addi	r17, r4, 13
	addi	r18, r4, 14
	addi	r19, r4, 15
	addi	r20, r4, 16
	addi	r21, r4, 17
	addi	r22, r4, 18
	addi	r23, r4, 19
	addi	fp, r4, 20
	addi	ra, r4, 21

	/* Check */
check:
	bne	sp, at, restore
	subi	r2, r4, 1
	bne	r2, r3, restore
	addi	r2, r4, 1
	bne	r2, r5, restore
	addi	r2, r4, 2
	bne	r2, r6, restore
	addi	r2, r4, 3
	bne	r2, r7, restore
	addi	r2, r4, 4
	bne	r2, r8, restore
	addi	r2, r4, 5
	bne	r2, r9, restore
	addi	r2, r4, 6
	bne	r2, r10, restore
	addi	r2, r4, 7
	bne	r2, r11, restore
	addi	r2, r4, 8
	bne	r2, r12, restore
	addi	r2, r4, 9
	bne	r2, r13, restore
	addi	r2, r4, 10
	bne	r2, r14, restore
	addi	r2, r4, 11
	bne	r2, r15, restore
	addi	r2, r4, 12
	bne	r2, r16, restore
	addi	r2, r4, 13
	bne	r2, r17, restore
	addi	r2, r4, 14
	bne	r2, r18, restore
	addi	r2, r4, 15
	bne	r2, r19, restore
	addi	r2, r4, 16
	bne	r2, r20, restore
	addi	r2, r4, 17
	bne	r2, r21, restore
	addi	r2, r4, 18
	bne	r2, r22, restore
	addi	r2, r4, 19
	bne	r2, r23, restore
	addi	r2, r4, 20
	bne	r2, fp, restore
	addi	r2, r4, 21
	bne	r2, ra, restore
	br	check

	/* Restore */
restore:
	ldw	ra, FRAME_OFFSET_RA(sp)
	ldw	fp, FRAME_OFFSET_FP(sp)
	ldw	r23, FRAME_OFFSET_R23(sp)
	ldw	r22, FRAME_OFFSET_R22(sp)
	ldw	r21, FRAME_OFFSET_R21(sp)
	ldw	r20, FRAME_OFFSET_R20(sp)
	ldw	r19, FRAME_OFFSET_R19(sp)
	ldw	r18, FRAME_OFFSET_R18(sp)
	ldw	r17, FRAME_OFFSET_R17(sp)
	ldw	r16, FRAME_OFFSET_R16(sp)
	addi	sp, sp, FRAME_SIZE
	ret
