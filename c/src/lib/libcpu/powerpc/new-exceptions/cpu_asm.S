/*  cpu_asm.s	1.1 - 95/12/04
 *
 *  This file contains the assembly code for the PowerPC implementation
 *  of RTEMS.
 *
 *  Author:	Andrew Bray <andy@i-cubed.co.uk>
 *
 *  COPYRIGHT (c) 1995 by i-cubed ltd.
 *
 *  To anyone who acknowledges that this file is provided "AS IS"
 *  without any express or implied warranty:
 *      permission to use, copy, modify, and distribute this file
 *      for any purpose is hereby granted without fee, provided that
 *      the above copyright notice and this notice appears in all
 *      copies, and that the name of i-cubed limited not be used in
 *      advertising or publicity pertaining to distribution of the
 *      software without specific, written prior permission.
 *      i-cubed limited makes no representations about the suitability
 *      of this software for any purpose.
 *
 *  Derived from c/src/exec/cpu/no_cpu/cpu_asm.c:
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (c) 2011-2012 embedded brains GmbH.
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems/asm.h>
#include <rtems/powerpc/powerpc.h>
#include <rtems/score/cpu.h>
#include <bspopts.h>

#if PPC_DEFAULT_CACHE_LINE_SIZE != 32
  #error "unexpected PPC_DEFAULT_CACHE_LINE_SIZE value"
#endif

#ifdef BSP_USE_DATA_CACHE_BLOCK_TOUCH
  #define DATA_CACHE_TOUCH(rega, regb) \
	dcbt rega, regb
#else
  #define DATA_CACHE_TOUCH(rega, regb)
#endif

#if BSP_DATA_CACHE_ENABLED && PPC_CACHE_ALIGNMENT == 32
  #define DATA_CACHE_ZERO_AND_TOUCH(reg, offset) \
	li reg, offset; dcbz reg, r3; DATA_CACHE_TOUCH(reg, r4)
#else
  #define DATA_CACHE_ZERO_AND_TOUCH(reg, offset)
#endif

#define PPC_CONTEXT_CACHE_LINE_0 32
#define PPC_CONTEXT_CACHE_LINE_1 64
#define PPC_CONTEXT_CACHE_LINE_2 96
#define PPC_CONTEXT_CACHE_LINE_3 128
#define PPC_CONTEXT_CACHE_LINE_4 160

/*
 * Offsets for various Contexts
 */

#if (PPC_HAS_DOUBLE==1)
	.set	FP_SIZE,	8
#define	LDF	lfd
#define	STF	stfd
#else
	.set	FP_SIZE,	4
#define	LDF	lfs
#define	STF	stfs
#endif

	.set	FP_0, 0
	.set	FP_1, (FP_0 + FP_SIZE)
	.set	FP_2, (FP_1 + FP_SIZE)
	.set	FP_3, (FP_2 + FP_SIZE)
	.set	FP_4, (FP_3 + FP_SIZE)
	.set	FP_5, (FP_4 + FP_SIZE)
	.set	FP_6, (FP_5 + FP_SIZE)
	.set	FP_7, (FP_6 + FP_SIZE)
	.set	FP_8, (FP_7 + FP_SIZE)
	.set	FP_9, (FP_8 + FP_SIZE)
	.set	FP_10, (FP_9 + FP_SIZE)
	.set	FP_11, (FP_10 + FP_SIZE)
	.set	FP_12, (FP_11 + FP_SIZE)
	.set	FP_13, (FP_12 + FP_SIZE)
	.set	FP_14, (FP_13 + FP_SIZE)
	.set	FP_15, (FP_14 + FP_SIZE)
	.set	FP_16, (FP_15 + FP_SIZE)
	.set	FP_17, (FP_16 + FP_SIZE)
	.set	FP_18, (FP_17 + FP_SIZE)
	.set	FP_19, (FP_18 + FP_SIZE)
	.set	FP_20, (FP_19 + FP_SIZE)
	.set	FP_21, (FP_20 + FP_SIZE)
	.set	FP_22, (FP_21 + FP_SIZE)
	.set	FP_23, (FP_22 + FP_SIZE)
	.set	FP_24, (FP_23 + FP_SIZE)
	.set	FP_25, (FP_24 + FP_SIZE)
	.set	FP_26, (FP_25 + FP_SIZE)
	.set	FP_27, (FP_26 + FP_SIZE)
	.set	FP_28, (FP_27 + FP_SIZE)
	.set	FP_29, (FP_28 + FP_SIZE)
	.set	FP_30, (FP_29 + FP_SIZE)
	.set	FP_31, (FP_30 + FP_SIZE)
	.set	FP_FPSCR, (FP_31 + FP_SIZE)

	BEGIN_CODE
/*
 *  _CPU_Context_save_fp_context
 *
 *  This routine is responsible for saving the FP context
 *  at *fp_context_ptr.  If the point to load the FP context
 *  from is changed then the pointer is modified by this routine.
 *
 *  Sometimes a macro implementation of this is in cpu.h which dereferences
 *  the ** and a similarly named routine in this file is passed something
 *  like a (Context_Control_fp *).  The general rule on making this decision
 *  is to avoid writing assembly language.
 */

	ALIGN (PPC_CACHE_ALIGNMENT, PPC_CACHE_ALIGN_POWER)
	PUBLIC_PROC (_CPU_Context_save_fp)
PROC (_CPU_Context_save_fp):
#if (PPC_HAS_FPU == 1)
/* A FP context switch may occur in an ISR or exception handler when the FPU is not
 * available. Therefore, we must explicitely enable it here!
 */
	mfmsr	r4
	andi.	r5,r4,MSR_FP
	bne	1f
	ori	r5,r4,MSR_FP
	mtmsr	r5
	isync
1:
	lwz	r3, 0(r3)
	STF	f0, FP_0(r3)
	STF	f1, FP_1(r3)
	STF	f2, FP_2(r3)
	STF	f3, FP_3(r3)
	STF	f4, FP_4(r3)
	STF	f5, FP_5(r3)
	STF	f6, FP_6(r3)
	STF	f7, FP_7(r3)
	STF	f8, FP_8(r3)
	STF	f9, FP_9(r3)
	STF	f10, FP_10(r3)
	STF	f11, FP_11(r3)
	STF	f12, FP_12(r3)
	STF	f13, FP_13(r3)
	STF	f14, FP_14(r3)
	STF	f15, FP_15(r3)
	STF	f16, FP_16(r3)
	STF	f17, FP_17(r3)
	STF	f18, FP_18(r3)
	STF	f19, FP_19(r3)
	STF	f20, FP_20(r3)
	STF	f21, FP_21(r3)
	STF	f22, FP_22(r3)
	STF	f23, FP_23(r3)
	STF	f24, FP_24(r3)
	STF	f25, FP_25(r3)
	STF	f26, FP_26(r3)
	STF	f27, FP_27(r3)
	STF	f28, FP_28(r3)
	STF	f29, FP_29(r3)
	STF	f30, FP_30(r3)
	STF	f31, FP_31(r3)
	mffs	f2
	STF	f2, FP_FPSCR(r3)
	bne	1f
	mtmsr	r4
	isync
1:
#endif
	blr

/*
 *  _CPU_Context_restore_fp_context
 *
 *  This routine is responsible for restoring the FP context
 *  at *fp_context_ptr.  If the point to load the FP context
 *  from is changed then the pointer is modified by this routine.
 *
 *  Sometimes a macro implementation of this is in cpu.h which dereferences
 *  the ** and a similarly named routine in this file is passed something
 *  like a (Context_Control_fp *).  The general rule on making this decision
 *  is to avoid writing assembly language.
 */

	ALIGN (PPC_CACHE_ALIGNMENT, PPC_CACHE_ALIGN_POWER)
	PUBLIC_PROC (_CPU_Context_restore_fp)
PROC (_CPU_Context_restore_fp):
#if (PPC_HAS_FPU == 1)
	lwz	r3, 0(r3)
/* A FP context switch may occur in an ISR or exception handler when the FPU is not
 * available. Therefore, we must explicitely enable it here!
 */
	mfmsr	r4
	andi.	r5,r4,MSR_FP
	bne	1f
	ori	r5,r4,MSR_FP
	mtmsr	r5
	isync
1:
	LDF	f2, FP_FPSCR(r3)
	mtfsf	255, f2
	LDF	f0, FP_0(r3)
	LDF	f1, FP_1(r3)
	LDF	f2, FP_2(r3)
	LDF	f3, FP_3(r3)
	LDF	f4, FP_4(r3)
	LDF	f5, FP_5(r3)
	LDF	f6, FP_6(r3)
	LDF	f7, FP_7(r3)
	LDF	f8, FP_8(r3)
	LDF	f9, FP_9(r3)
	LDF	f10, FP_10(r3)
	LDF	f11, FP_11(r3)
	LDF	f12, FP_12(r3)
	LDF	f13, FP_13(r3)
	LDF	f14, FP_14(r3)
	LDF	f15, FP_15(r3)
	LDF	f16, FP_16(r3)
	LDF	f17, FP_17(r3)
	LDF	f18, FP_18(r3)
	LDF	f19, FP_19(r3)
	LDF	f20, FP_20(r3)
	LDF	f21, FP_21(r3)
	LDF	f22, FP_22(r3)
	LDF	f23, FP_23(r3)
	LDF	f24, FP_24(r3)
	LDF	f25, FP_25(r3)
	LDF	f26, FP_26(r3)
	LDF	f27, FP_27(r3)
	LDF	f28, FP_28(r3)
	LDF	f29, FP_29(r3)
	LDF	f30, FP_30(r3)
	LDF	f31, FP_31(r3)
	bne	1f
	mtmsr	r4
	isync
1:
#endif
	blr

	ALIGN (PPC_CACHE_ALIGNMENT, PPC_CACHE_ALIGN_POWER)
	PUBLIC_PROC (_CPU_Context_switch)
PROC (_CPU_Context_switch):

#ifdef BSP_USE_SYNC_IN_CONTEXT_SWITCH
	sync
	isync
#endif

	/* Align to a cache line */
	clrrwi	r3, r3, 5
	clrrwi	r4, r4, 5

	DATA_CACHE_ZERO_AND_TOUCH(r10, PPC_CONTEXT_CACHE_LINE_0)
	DATA_CACHE_ZERO_AND_TOUCH(r11, PPC_CONTEXT_CACHE_LINE_1)

	/* Save context to r3 */

	mfmsr	r5
	mflr	r6
	mfcr	r7
#ifdef RTEMS_MULTIPROCESSING
	/*
	 * We have to clear the reservation of the executing thread.  See also
	 * Book E section 6.1.6.2 "Atomic Update Primitives".
	 */
  #if PPC_CONTEXT_OFFSET_GPR1 != PPC_CONTEXT_CACHE_LINE_0 \
    || !BSP_DATA_CACHE_ENABLED \
    || PPC_CACHE_ALIGNMENT != 32
	li	r10, PPC_CONTEXT_OFFSET_GPR1
  #endif
	stwcx.	r1, r3, r10
#endif
	stw	r1, PPC_CONTEXT_OFFSET_GPR1(r3)
	stw	r5, PPC_CONTEXT_OFFSET_MSR(r3)
	stw	r6, PPC_CONTEXT_OFFSET_LR(r3)
	stw	r7, PPC_CONTEXT_OFFSET_CR(r3)
	PPC_GPR_STORE	r14, PPC_CONTEXT_OFFSET_GPR14(r3)
	PPC_GPR_STORE	r15, PPC_CONTEXT_OFFSET_GPR15(r3)

#if PPC_CONTEXT_OFFSET_GPR20 == PPC_CONTEXT_CACHE_LINE_2
	DATA_CACHE_ZERO_AND_TOUCH(r10, PPC_CONTEXT_CACHE_LINE_2)
#endif

	PPC_GPR_STORE	r16, PPC_CONTEXT_OFFSET_GPR16(r3)
	PPC_GPR_STORE	r17, PPC_CONTEXT_OFFSET_GPR17(r3)

#if PPC_CONTEXT_OFFSET_GPR26 == PPC_CONTEXT_CACHE_LINE_2
	DATA_CACHE_ZERO_AND_TOUCH(r10, PPC_CONTEXT_CACHE_LINE_2)
#endif

	PPC_GPR_STORE	r18, PPC_CONTEXT_OFFSET_GPR18(r3)
	PPC_GPR_STORE	r19, PPC_CONTEXT_OFFSET_GPR19(r3)

#if PPC_CONTEXT_OFFSET_GPR24 == PPC_CONTEXT_CACHE_LINE_3
	DATA_CACHE_ZERO_AND_TOUCH(r10, PPC_CONTEXT_CACHE_LINE_3)
#endif

	PPC_GPR_STORE	r20, PPC_CONTEXT_OFFSET_GPR20(r3)
	PPC_GPR_STORE	r21, PPC_CONTEXT_OFFSET_GPR21(r3)
	PPC_GPR_STORE	r22, PPC_CONTEXT_OFFSET_GPR22(r3)
	PPC_GPR_STORE	r23, PPC_CONTEXT_OFFSET_GPR23(r3)

#if PPC_CONTEXT_OFFSET_GPR28 == PPC_CONTEXT_CACHE_LINE_4
	DATA_CACHE_ZERO_AND_TOUCH(r10, PPC_CONTEXT_CACHE_LINE_4)
#endif

	PPC_GPR_STORE	r24, PPC_CONTEXT_OFFSET_GPR24(r3)
	PPC_GPR_STORE	r25, PPC_CONTEXT_OFFSET_GPR25(r3)
	PPC_GPR_STORE	r26, PPC_CONTEXT_OFFSET_GPR26(r3)
	PPC_GPR_STORE	r27, PPC_CONTEXT_OFFSET_GPR27(r3)

	PPC_GPR_STORE	r28, PPC_CONTEXT_OFFSET_GPR28(r3)
	PPC_GPR_STORE	r29, PPC_CONTEXT_OFFSET_GPR29(r3)
	PPC_GPR_STORE	r30, PPC_CONTEXT_OFFSET_GPR30(r3)
	PPC_GPR_STORE	r31, PPC_CONTEXT_OFFSET_GPR31(r3)

	/* Restore context from r4 */
restore_context:

#ifdef __ALTIVEC__
	mr	r14, r4 
	.extern	_CPU_Context_switch_altivec
	bl	_CPU_Context_switch_altivec
	mr	r4, r14
#endif

	lwz	r1, PPC_CONTEXT_OFFSET_GPR1(r4)
	lwz	r5, PPC_CONTEXT_OFFSET_MSR(r4)
	lwz	r6, PPC_CONTEXT_OFFSET_LR(r4)
	lwz	r7, PPC_CONTEXT_OFFSET_CR(r4)

	PPC_GPR_LOAD	r14, PPC_CONTEXT_OFFSET_GPR14(r4)
	PPC_GPR_LOAD	r15, PPC_CONTEXT_OFFSET_GPR15(r4)

	DATA_CACHE_TOUCH(r0, r1)

	PPC_GPR_LOAD	r16, PPC_CONTEXT_OFFSET_GPR16(r4)
	PPC_GPR_LOAD	r17, PPC_CONTEXT_OFFSET_GPR17(r4)
	PPC_GPR_LOAD	r18, PPC_CONTEXT_OFFSET_GPR18(r4)
	PPC_GPR_LOAD	r19, PPC_CONTEXT_OFFSET_GPR19(r4)

	PPC_GPR_LOAD	r20, PPC_CONTEXT_OFFSET_GPR20(r4)
	PPC_GPR_LOAD	r21, PPC_CONTEXT_OFFSET_GPR21(r4)
	PPC_GPR_LOAD	r22, PPC_CONTEXT_OFFSET_GPR22(r4)
	PPC_GPR_LOAD	r23, PPC_CONTEXT_OFFSET_GPR23(r4)

	PPC_GPR_LOAD	r24, PPC_CONTEXT_OFFSET_GPR24(r4)
	PPC_GPR_LOAD	r25, PPC_CONTEXT_OFFSET_GPR25(r4)
	PPC_GPR_LOAD	r26, PPC_CONTEXT_OFFSET_GPR26(r4)
	PPC_GPR_LOAD	r27, PPC_CONTEXT_OFFSET_GPR27(r4)

	PPC_GPR_LOAD	r28, PPC_CONTEXT_OFFSET_GPR28(r4)
	PPC_GPR_LOAD	r29, PPC_CONTEXT_OFFSET_GPR29(r4)
	PPC_GPR_LOAD	r30, PPC_CONTEXT_OFFSET_GPR30(r4)
	PPC_GPR_LOAD	r31, PPC_CONTEXT_OFFSET_GPR31(r4)

	mtcr	r7
	mtlr	r6
	mtmsr	r5

#ifdef BSP_USE_SYNC_IN_CONTEXT_SWITCH
	isync
#endif

	blr

	PUBLIC_PROC (_CPU_Context_restore)
PROC (_CPU_Context_restore):
	/* Align to a cache line */
	clrrwi	r4, r3, 5

#ifdef __ALTIVEC__
	li	r3, 0
#endif

	b	restore_context
