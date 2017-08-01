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

#define LR_OFFSET 8
#define CR_OFFSET 16
#define OFFSET(i) ((i) * PPC_GPR_SIZE + 32)
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

#ifdef PPC_MULTILIB_FPU
  #define FOFFSET(i) ((i) * 8 + OFFSET(18))
  #define F14_OFFSET FOFFSET(0)
  #define F15_OFFSET FOFFSET(1)
  #define F16_OFFSET FOFFSET(2)
  #define F17_OFFSET FOFFSET(3)
  #define F18_OFFSET FOFFSET(4)
  #define F19_OFFSET FOFFSET(5)
  #define F20_OFFSET FOFFSET(6)
  #define F21_OFFSET FOFFSET(7)
  #define F22_OFFSET FOFFSET(8)
  #define F23_OFFSET FOFFSET(9)
  #define F24_OFFSET FOFFSET(10)
  #define F25_OFFSET FOFFSET(11)
  #define F26_OFFSET FOFFSET(12)
  #define F27_OFFSET FOFFSET(13)
  #define F28_OFFSET FOFFSET(14)
  #define F29_OFFSET FOFFSET(15)
  #define F30_OFFSET FOFFSET(16)
  #define F31_OFFSET FOFFSET(17)
  #define FPSCR_OFFSET FOFFSET(18)
  #define FTMP_OFFSET FOFFSET(19)
  #define FTMP2_OFFSET FOFFSET(20)
  #define FPUEND FOFFSET(21)
#else
  #define FPUEND OFFSET(18)
#endif

#ifdef PPC_MULTILIB_ALTIVEC
  #define VOFFSET(i) ((i) * 16 + ((FPUEND + 16 - 1) & ~(16 - 1)))
  #define V20_OFFSET VOFFSET(0)
  #define V21_OFFSET VOFFSET(1)
  #define V22_OFFSET VOFFSET(2)
  #define V23_OFFSET VOFFSET(3)
  #define V24_OFFSET VOFFSET(4)
  #define V25_OFFSET VOFFSET(5)
  #define V26_OFFSET VOFFSET(6)
  #define V27_OFFSET VOFFSET(7)
  #define V28_OFFSET VOFFSET(8)
  #define V29_OFFSET VOFFSET(9)
  #define V30_OFFSET VOFFSET(10)
  #define V31_OFFSET VOFFSET(11)
  #define VTMP_OFFSET VOFFSET(12)
  #define VTMP2_OFFSET VOFFSET(13)
  #define VRSAVE_OFFSET VOFFSET(14)
  #define VSCR_OFFSET (VOFFSET(14) + 12)
  #define ALTIVECEND VOFFSET(15)
#else
  #define ALTIVECEND FPUEND
#endif

#define FRAME_SIZE \
  ((ALTIVECEND + CPU_STACK_ALIGNMENT - 1) & ~(CPU_STACK_ALIGNMENT - 1))

	.global _CPU_Context_validate

_CPU_Context_validate:

	/* Save */
	PPC_REG_STORE_UPDATE	r1, -FRAME_SIZE(r1)
	mflr	r4
	PPC_REG_STORE	r4, LR_OFFSET(r1)
	mfcr	r4
	stw	r4, CR_OFFSET(r1)
	PPC_REG_STORE	r14, GPR14_OFFSET(r1)
	PPC_REG_STORE	r15, GPR15_OFFSET(r1)
	PPC_REG_STORE	r16, GPR16_OFFSET(r1)
	PPC_REG_STORE	r17, GPR17_OFFSET(r1)
	PPC_REG_STORE	r18, GPR18_OFFSET(r1)
	PPC_REG_STORE	r19, GPR19_OFFSET(r1)
	PPC_REG_STORE	r20, GPR20_OFFSET(r1)
	PPC_REG_STORE	r21, GPR21_OFFSET(r1)
	PPC_REG_STORE	r22, GPR22_OFFSET(r1)
	PPC_REG_STORE	r23, GPR23_OFFSET(r1)
	PPC_REG_STORE	r24, GPR24_OFFSET(r1)
	PPC_REG_STORE	r25, GPR25_OFFSET(r1)
	PPC_REG_STORE	r26, GPR26_OFFSET(r1)
	PPC_REG_STORE	r27, GPR27_OFFSET(r1)
	PPC_REG_STORE	r28, GPR28_OFFSET(r1)
	PPC_REG_STORE	r29, GPR29_OFFSET(r1)
	PPC_REG_STORE	r30, GPR30_OFFSET(r1)
	PPC_REG_STORE	r31, GPR31_OFFSET(r1)

#ifdef PPC_MULTILIB_FPU
	stfd	f14, F14_OFFSET(r1)
	stfd	f15, F15_OFFSET(r1)
	stfd	f16, F16_OFFSET(r1)
	stfd	f17, F17_OFFSET(r1)
	stfd	f18, F18_OFFSET(r1)
	stfd	f19, F19_OFFSET(r1)
	stfd	f20, F20_OFFSET(r1)
	stfd	f21, F21_OFFSET(r1)
	stfd	f22, F22_OFFSET(r1)
	stfd	f23, F23_OFFSET(r1)
	stfd	f24, F24_OFFSET(r1)
	stfd	f25, F25_OFFSET(r1)
	stfd	f26, F26_OFFSET(r1)
	stfd	f27, F27_OFFSET(r1)
	stfd	f28, F28_OFFSET(r1)
	stfd	f29, F29_OFFSET(r1)
	stfd	f30, F30_OFFSET(r1)
	stfd	f31, F31_OFFSET(r1)
	mffs	f0
	stfd	f0, FPSCR_OFFSET(r1)
#endif

#ifdef PPC_MULTILIB_ALTIVEC
	li	r0, V20_OFFSET
	stvx	v20, r1, r0
	li	r0, V21_OFFSET
	stvx	v21, r1, r0
	li	r0, V22_OFFSET
	stvx	v22, r1, r0
	li	r0, V23_OFFSET
	stvx	v23, r1, r0
	li	r0, V24_OFFSET
	stvx	v24, r1, r0
	li	r0, V25_OFFSET
	stvx	v25, r1, r0
	li	r0, V26_OFFSET
	stvx	v26, r1, r0
	li	r0, V27_OFFSET
	stvx	v27, r1, r0
	li	r0, V28_OFFSET
	stvx	v28, r1, r0
	li	r0, V29_OFFSET
	stvx	v29, r1, r0
	li	r0, V30_OFFSET
	stvx	v30, r1, r0
	li	r0, V31_OFFSET
	stvx	v31, r1, r0
	mfvscr	v0
	li	r0, VSCR_OFFSET
	stvewx	v0, r1, r0
	mfvrsave	r0
	stw	r0, VRSAVE_OFFSET(r1)
#endif

	/* Fill */

	/* CR and GPR29 are equal most of the time */
	addi	r4, r3, 24
	mtcr	r4

	addi	r4, r3, 25
	mtlr	r4
	addi	r4, r3, 26
	mtctr	r4
	rlwinm	r4, r3, 0, 25, 2
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

	/* GPR28 contains the TP pattern */
#ifdef __powerpc64__
	xor	r28, r13, r3
#else
	xor	r28, r2, r3
#endif

	/* GPR29 and CR are equal most of the time */
	addi	r29, r3, 24

	/* GPR30 contains the MSR pattern */
	mfmsr	r30
	xor	r30, r30, r3

	/* GPR31 contains the stack pointer */
	mr	r31, r1

#ifdef PPC_MULTILIB_FPU
.macro FILL_F i
	addi	r4, r3, 0x100 + \i
	stw	r4, FTMP_OFFSET(r1)
	addi	r4, r3, 0x200 + \i
	stw	r4, FTMP_OFFSET + 4(r1)
	lfd	\i, FTMP_OFFSET(r1)
.endm

	FILL_F 0
	FILL_F 1
	FILL_F 2
	FILL_F 3
	FILL_F 4
	FILL_F 5
	FILL_F 6
	FILL_F 7
	FILL_F 8
	FILL_F 9
	FILL_F 10
	FILL_F 11
	FILL_F 12
	FILL_F 13
	FILL_F 14
	FILL_F 15
	FILL_F 16
	FILL_F 17
	FILL_F 18
	FILL_F 19
	FILL_F 20
	FILL_F 21
	FILL_F 22
	FILL_F 23
	FILL_F 24
	FILL_F 25
	FILL_F 26
	FILL_F 27
	FILL_F 28
	FILL_F 29
	FILL_F 30
	FILL_F 31
#endif

#ifdef PPC_MULTILIB_ALTIVEC
.macro FILL_V i
	addi	r4, r3, 0x300 + \i
	stw	r4, VTMP_OFFSET(r1)
	addi	r4, r3, 0x400 + \i
	stw	r4, VTMP_OFFSET + 4(r1)
	addi	r4, r3, 0x500 + \i
	stw	r4, VTMP_OFFSET + 8(r1)
	addi	r4, r3, 0x600 + \i
	stw	r4, VTMP_OFFSET + 12(r1)
	li	r4, VTMP_OFFSET
	lvx	\i, r1, r4
.endm

	FILL_V 0
	FILL_V 1
	FILL_V 2
	FILL_V 3
	FILL_V 4
	FILL_V 5
	FILL_V 6
	FILL_V 7
	FILL_V 8
	FILL_V 9
	FILL_V 10
	FILL_V 11
	FILL_V 12
	FILL_V 13
	FILL_V 14
	FILL_V 15
	FILL_V 16
	FILL_V 17
	FILL_V 18
	FILL_V 19
	FILL_V 20
	FILL_V 21
	FILL_V 22
	FILL_V 23
	FILL_V 24
	FILL_V 25
	FILL_V 26
	FILL_V 27
	FILL_V 28
	FILL_V 29
	FILL_V 30
	FILL_V 31
	addi	r4, r3, 0x700
	mtvrsave	r4
#endif

	/* Check */
check:
	mfcr	r4
	cmpw	r4, r29
	bne	restore
	addi	r4, r3, 1
	PPC_REG_CMP	r4, r5
	bne	restore
	addi	r4, r3, 2
	PPC_REG_CMP	r4, r6
	bne	restore
	addi	r4, r3, 3
	PPC_REG_CMP	r4, r7
	bne	restore
	addi	r4, r3, 4
	PPC_REG_CMP	r4, r8
	bne	restore
	addi	r4, r3, 5
	PPC_REG_CMP	r4, r9
	bne	restore
	addi	r4, r3, 6
	PPC_REG_CMP	r4, r10
	bne	restore
	addi	r4, r3, 7
	PPC_REG_CMP	r4, r11
	bne	restore
	addi	r4, r3, 8
	PPC_REG_CMP	r4, r12
	bne	restore
#ifdef __powerpc64__
	lis	r4, .TOC.@highest
	ori	r4, r4, .TOC.@higher
	rldicr	r4, r4, 32, 31
	oris	r4, r4, .TOC.@h
	ori	r4, r4, .TOC.@l
	PPC_REG_CMP	r4, r2
#else
	lis	r4, _SDA_BASE_@h
	ori	r4, r4, _SDA_BASE_@l
	PPC_REG_CMP	r4, r13
#endif
	bne	restore
	addi	r4, r3, 9
	PPC_REG_CMP	r4, r14
	bne	restore
	addi	r4, r3, 10
	PPC_REG_CMP	r4, r15
	bne	restore
	addi	r4, r3, 11
	PPC_REG_CMP	r4, r16
	bne	restore
	addi	r4, r3, 12
	PPC_REG_CMP	r4, r17
	bne	restore
	addi	r4, r3, 13
	PPC_REG_CMP	r4, r18
	bne	restore
	addi	r4, r3, 14
	PPC_REG_CMP	r4, r19
	bne	restore
	addi	r4, r3, 15
	PPC_REG_CMP	r4, r20
	bne	restore
	addi	r4, r3, 16
	PPC_REG_CMP	r4, r21
	bne	restore
	addi	r4, r3, 17
	PPC_REG_CMP	r4, r22
	bne	restore
	addi	r4, r3, 18
	PPC_REG_CMP	r4, r23
	bne	restore
	addi	r4, r3, 19
	PPC_REG_CMP	r4, r24
	bne	restore
	addi	r4, r3, 20
	PPC_REG_CMP	r4, r25
	bne	restore
	addi	r4, r3, 21
	PPC_REG_CMP	r4, r26
	bne	restore
	addi	r4, r3, 22
	PPC_REG_CMP	r4, r27
	bne	restore
#ifdef __powerpc64__
	xor	r4, r13, r3
#else
	xor	r4, r2, r3
#endif
	PPC_REG_CMP	r4, r28
	bne	restore
	addi	r4, r3, 24
	PPC_REG_CMP	r4, r29
	bne	restore
	mfmsr	r4
	xor	r4, r4, r3
	PPC_REG_CMP	r4, r30
	bne	restore
	addi	r4, r3, 25
	mflr	r5
	PPC_REG_CMP	r4, r5
	bne	restore
	addi	r4, r3, 26
	mfctr	r5
	PPC_REG_CMP	r4, r5
	bne	restore
	rlwinm	r4, r3, 0, 25, 2
	mfxer	r5
	cmpw	r4, r5
	bne	restore
	addi	r4, r3, 28
	PPC_REG_CMP	r4, r0
	bne	restore
	PPC_REG_CMP	r31, r1
	bne	restore

#ifdef PPC_MULTILIB_FPU
.macro CHECK_F i
	stfd	\i, FTMP_OFFSET(r1)
	lwz	r5, FTMP_OFFSET(r1)
	addi	r4, r3, 0x100 + \i
	cmpw	r5, r4
	bne	restore
	lwz	r5, FTMP_OFFSET + 4(r1)
	addi	r4, r3, 0x200 + \i
	cmpw	r5, r4
	bne	restore
.endm

	/* Check FPSCR */
	stfd	f0, FTMP_OFFSET(r1)
	mffs	f0
	stfd	f0, FTMP2_OFFSET(r1)
	lwz	r4, FTMP2_OFFSET + 4(r1)
	lwz	r5, FPSCR_OFFSET + 4(r1)
	cmpw	r5, r4
	bne	restore
	lfd	f0, FTMP_OFFSET(r1)

	CHECK_F 0
	CHECK_F 1
	CHECK_F 2
	CHECK_F 3
	CHECK_F 4
	CHECK_F 5
	CHECK_F 6
	CHECK_F 7
	CHECK_F 8
	CHECK_F 9
	CHECK_F 10
	CHECK_F 11
	CHECK_F 12
	CHECK_F 13
	CHECK_F 14
	CHECK_F 15
	CHECK_F 16
	CHECK_F 17
	CHECK_F 18
	CHECK_F 19
	CHECK_F 20
	CHECK_F 21
	CHECK_F 22
	CHECK_F 23
	CHECK_F 24
	CHECK_F 25
	CHECK_F 26
	CHECK_F 27
	CHECK_F 28
	CHECK_F 29
	CHECK_F 30
	CHECK_F 31
#endif

#ifdef PPC_MULTILIB_ALTIVEC
.macro CHECK_V i
	li	r4, VTMP_OFFSET
	stvx	\i, r1, r4
	lwz	r5, VTMP_OFFSET(r1)
	addi	r4, r3, 0x300 + \i
	cmpw	r5, r4
	bne	restore
	lwz	r5, VTMP_OFFSET + 4(r1)
	addi	r4, r3, 0x400 + \i
	cmpw	r5, r4
	bne	restore
	lwz	r5, VTMP_OFFSET + 8(r1)
	addi	r4, r3, 0x500 + \i
	cmpw	r5, r4
	bne	restore
	lwz	r5, VTMP_OFFSET + 12(r1)
	addi	r4, r3, 0x600 + \i
	cmpw	r5, r4
	bne	restore
.endm

	/* Check VSCR */
	li	r4, VTMP_OFFSET
	stvx	v0, r1, r4
	mfvscr	v0
	li	r4, VTMP2_OFFSET + 12
	stvewx	v0, r1, r4
	lwz	r4, VTMP2_OFFSET + 12(r1)
	lwz	r5, VSCR_OFFSET(r1)
	cmpw	r5, r4
	bne	restore
	li	r4, VTMP_OFFSET
	lvx	v0, r1, r4

	CHECK_V 0
	CHECK_V 1
	CHECK_V 2
	CHECK_V 3
	CHECK_V 4
	CHECK_V 5
	CHECK_V 6
	CHECK_V 7
	CHECK_V 8
	CHECK_V 9
	CHECK_V 10
	CHECK_V 11
	CHECK_V 12
	CHECK_V 13
	CHECK_V 14
	CHECK_V 15
	CHECK_V 16
	CHECK_V 17
	CHECK_V 18
	CHECK_V 19
	CHECK_V 20
	CHECK_V 21
	CHECK_V 22
	CHECK_V 23
	CHECK_V 24
	CHECK_V 25
	CHECK_V 26
	CHECK_V 27
	CHECK_V 28
	CHECK_V 29
	CHECK_V 30
	CHECK_V 31
	mfvrsave	r5
	addi	r4, r3, 0x700
	cmpw	r5, r4
	bne	restore
#endif

	mtcr	r29
	addi	r5, r3, 1
	b	check

	/* Restore */
restore:

#ifdef PPC_MULTILIB_ALTIVEC
	lwz	r0, VRSAVE_OFFSET(r1)
	mtvrsave	r0
	li	r0, V31_OFFSET
	lvx	v31, r1, r0
	li	r0, V30_OFFSET
	lvx	v30, r1, r0
	li	r0, V29_OFFSET
	lvx	v29, r1, r0
	li	r0, V28_OFFSET
	lvx	v28, r1, r0
	li	r0, V27_OFFSET
	lvx	v27, r1, r0
	li	r0, V26_OFFSET
	lvx	v26, r1, r0
	li	r0, V25_OFFSET
	lvx	v25, r1, r0
	li	r0, V24_OFFSET
	lvx	v24, r1, r0
	li	r0, V23_OFFSET
	lvx	v23, r1, r0
	li	r0, V22_OFFSET
	lvx	v22, r1, r0
	li	r0, V21_OFFSET
	lvx	v21, r1, r0
	li	r0, V20_OFFSET
	lvx	v20, r1, r0
#endif

#ifdef PPC_MULTILIB_FPU
	lfd	f31, F31_OFFSET(r1)
	lfd	f30, F30_OFFSET(r1)
	lfd	f29, F29_OFFSET(r1)
	lfd	f28, F28_OFFSET(r1)
	lfd	f27, F27_OFFSET(r1)
	lfd	f26, F26_OFFSET(r1)
	lfd	f25, F25_OFFSET(r1)
	lfd	f24, F24_OFFSET(r1)
	lfd	f23, F23_OFFSET(r1)
	lfd	f22, F22_OFFSET(r1)
	lfd	f21, F21_OFFSET(r1)
	lfd	f20, F20_OFFSET(r1)
	lfd	f19, F19_OFFSET(r1)
	lfd	f18, F18_OFFSET(r1)
	lfd	f17, F17_OFFSET(r1)
	lfd	f16, F16_OFFSET(r1)
	lfd	f15, F15_OFFSET(r1)
	lfd	f14, F14_OFFSET(r1)
#endif

	PPC_REG_LOAD	r31, GPR31_OFFSET(r1)
	PPC_REG_LOAD	r30, GPR30_OFFSET(r1)
	PPC_REG_LOAD	r29, GPR29_OFFSET(r1)
	PPC_REG_LOAD	r28, GPR28_OFFSET(r1)
	PPC_REG_LOAD	r27, GPR27_OFFSET(r1)
	PPC_REG_LOAD	r26, GPR26_OFFSET(r1)
	PPC_REG_LOAD	r25, GPR25_OFFSET(r1)
	PPC_REG_LOAD	r24, GPR24_OFFSET(r1)
	PPC_REG_LOAD	r23, GPR23_OFFSET(r1)
	PPC_REG_LOAD	r22, GPR22_OFFSET(r1)
	PPC_REG_LOAD	r21, GPR21_OFFSET(r1)
	PPC_REG_LOAD	r20, GPR20_OFFSET(r1)
	PPC_REG_LOAD	r19, GPR19_OFFSET(r1)
	PPC_REG_LOAD	r18, GPR18_OFFSET(r1)
	PPC_REG_LOAD	r17, GPR17_OFFSET(r1)
	PPC_REG_LOAD	r16, GPR16_OFFSET(r1)
	PPC_REG_LOAD	r15, GPR15_OFFSET(r1)
	PPC_REG_LOAD	r14, GPR14_OFFSET(r1)
	lwz	r4, CR_OFFSET(r1)
	mtcr	r4
	PPC_REG_LOAD	r4, LR_OFFSET(r1)
	mtlr	r4
	addi	r1, r1, FRAME_SIZE
	blr
