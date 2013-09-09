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
#include <rtems/score/cpu.h>

#define LR_OFFSET 8
#define CR_OFFSET 12
#define OFFSET(i) ((i) * PPC_GPR_SIZE + 16)
#define GPR14_OFFSET OFFSET(0)
#define GPR15_OFFSET OFFSET(1)
#define GPR16_OFFSET OFFSET(2)
#define GPR17_OFFSET OFFSET(3)
#define GPR18_OFFSET OFFSET(4)
#define GPR19_OFFSET OFFSET(5)
#define GPR20_OFFSET OFFSET(6)
#define GPR21_OFFSET OFFSET(7)
#define GPR22_OFFSET OFFSET(8)
#define GPR23_OFFSET OFFSET(9)
#define GPR24_OFFSET OFFSET(10)
#define GPR25_OFFSET OFFSET(11)
#define GPR26_OFFSET OFFSET(12)
#define GPR27_OFFSET OFFSET(13)
#define GPR28_OFFSET OFFSET(14)
#define GPR29_OFFSET OFFSET(15)
#define GPR30_OFFSET OFFSET(16)
#define GPR31_OFFSET OFFSET(17)
#define FRAME_SIZE \
  ((OFFSET(18) + CPU_STACK_ALIGNMENT - 1) & ~(CPU_STACK_ALIGNMENT - 1))

	.global _CPU_Context_validate

_CPU_Context_validate:

	/* Save */
	stwu	r1, -FRAME_SIZE(r1)
	mflr	r4
	stw	r4, LR_OFFSET(r1)
	mfcr	r4
	stw	r4, CR_OFFSET(r1)
	stw	r14, GPR14_OFFSET(r1)
	stw	r15, GPR15_OFFSET(r1)
	stw	r16, GPR16_OFFSET(r1)
	stw	r17, GPR17_OFFSET(r1)
	stw	r18, GPR18_OFFSET(r1)
	stw	r19, GPR19_OFFSET(r1)
	stw	r20, GPR20_OFFSET(r1)
	stw	r21, GPR21_OFFSET(r1)
	stw	r22, GPR22_OFFSET(r1)
	stw	r23, GPR23_OFFSET(r1)
	stw	r24, GPR24_OFFSET(r1)
	stw	r25, GPR25_OFFSET(r1)
	stw	r26, GPR26_OFFSET(r1)
	stw	r27, GPR27_OFFSET(r1)
	stw	r28, GPR28_OFFSET(r1)
	stw	r29, GPR29_OFFSET(r1)
	stw	r30, GPR30_OFFSET(r1)
	stw	r31, GPR31_OFFSET(r1)

	/* Fill */

	/* CR and GPR29 are equal most of the time */
	addi	r4, r3, 24
	mtcr	r4

	addi	r4, r3, 25
	mtlr	r4
	addi	r4, r3, 26
	mtctr	r4
	addi	r4, r3, 27
	mtxer	r4
	addi	r0, r3, 28

	/* GPR4 is used for temporary values */

	addi	r5, r3, 1
	addi	r6, r3, 2
	addi	r7, r3, 3
	addi	r8, r3, 4
	addi	r9, r3, 5
	addi	r10, r3, 6
	addi	r11, r3, 7
	addi	r12, r3, 8
	addi	r14, r3, 9
	addi	r15, r3, 10
	addi	r16, r3, 11
	addi	r17, r3, 12
	addi	r18, r3, 13
	addi	r19, r3, 14
	addi	r20, r3, 15
	addi	r21, r3, 16
	addi	r22, r3, 17
	addi	r23, r3, 18
	addi	r24, r3, 19
	addi	r25, r3, 20
	addi	r26, r3, 21
	addi	r27, r3, 22
	addi	r28, r3, 23

	/* GPR29 and CR are equal most of the time */
	addi	r29, r3, 24

	/* GPR30 contains the MSR pattern */
	mfmsr	r30
	xor	r30, r30, r3

	/* GPR31 contains the stack pointer */
	mr	r31, r1

	/* Check */
check:
	mfcr	r4
	cmpw	r4, r29
	bne	restore
	addi	r4, r3, 1
	cmpw	r4, r5
	bne	restore
	addi	r4, r3, 2
	cmpw	r4, r6
	bne	restore
	addi	r4, r3, 3
	cmpw	r4, r7
	bne	restore
	addi	r4, r3, 4
	cmpw	r4, r8
	bne	restore
	addi	r4, r3, 5
	cmpw	r4, r9
	bne	restore
	addi	r4, r3, 6
	cmpw	r4, r10
	bne	restore
	addi	r4, r3, 7
	cmpw	r4, r11
	bne	restore
	addi	r4, r3, 8
	cmpw	r4, r12
	bne	restore
	lis	r4, _SDA_BASE_@h
	ori	r4, r4, _SDA_BASE_@l
	cmpw	r4, r13
	bne	restore
	addi	r4, r3, 9
	cmpw	r4, r14
	bne	restore
	addi	r4, r3, 10
	cmpw	r4, r15
	bne	restore
	addi	r4, r3, 11
	cmpw	r4, r16
	bne	restore
	addi	r4, r3, 12
	cmpw	r4, r17
	bne	restore
	addi	r4, r3, 13
	cmpw	r4, r18
	bne	restore
	addi	r4, r3, 14
	cmpw	r4, r19
	bne	restore
	addi	r4, r3, 15
	cmpw	r4, r20
	bne	restore
	addi	r4, r3, 16
	cmpw	r4, r21
	bne	restore
	addi	r4, r3, 17
	cmpw	r4, r22
	bne	restore
	addi	r4, r3, 18
	cmpw	r4, r23
	bne	restore
	addi	r4, r3, 19
	cmpw	r4, r24
	bne	restore
	addi	r4, r3, 20
	cmpw	r4, r25
	bne	restore
	addi	r4, r3, 21
	cmpw	r4, r26
	bne	restore
	addi	r4, r3, 22
	cmpw	r4, r27
	bne	restore
	addi	r4, r3, 23
	cmpw	r4, r28
	bne	restore
	addi	r4, r3, 24
	cmpw	r4, r29
	bne	restore
	mfmsr	r4
	xor	r4, r4, r3
	cmpw	r4, r30
	bne	restore
	addi	r4, r3, 25
	mflr	r5
	cmpw	r4, r5
	bne	restore
	addi	r4, r3, 26
	mfctr	r5
	cmpw	r4, r5
	bne	restore
	addi	r4, r3, 27
	mfxer	r5
	cmpw	r4, r5
	bne	restore
	addi	r4, r3, 28
	cmpw	r4, r0
	bne	restore
	cmpw	r31, r1
	bne	restore
#if 0
	/* This is only valid if we use the EABI */
	lis	r4, _SDA2_BASE_@h
	ori	r4, r4, _SDA2_BASE_@l
	cmpw	r4, r2
	bne	restore
#endif
	mtcr	r29
	addi	r5, r3, 1
	b	check

	/* Restore */
restore:
	lwz	r31, GPR31_OFFSET(r1)
	lwz	r30, GPR30_OFFSET(r1)
	lwz	r29, GPR29_OFFSET(r1)
	lwz	r28, GPR28_OFFSET(r1)
	lwz	r27, GPR27_OFFSET(r1)
	lwz	r26, GPR26_OFFSET(r1)
	lwz	r25, GPR25_OFFSET(r1)
	lwz	r24, GPR24_OFFSET(r1)
	lwz	r23, GPR23_OFFSET(r1)
	lwz	r22, GPR22_OFFSET(r1)
	lwz	r21, GPR21_OFFSET(r1)
	lwz	r20, GPR20_OFFSET(r1)
	lwz	r19, GPR19_OFFSET(r1)
	lwz	r18, GPR18_OFFSET(r1)
	lwz	r17, GPR17_OFFSET(r1)
	lwz	r16, GPR16_OFFSET(r1)
	lwz	r15, GPR15_OFFSET(r1)
	lwz	r14, GPR14_OFFSET(r1)
	lwz	r4, CR_OFFSET(r1)
	mtcr	r4
	lwz	r4, LR_OFFSET(r1)
	mtlr	r4
	addi	r1, r1, FRAME_SIZE
	blr
