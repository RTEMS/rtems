
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
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <asm.h>

/*
 * Offsets for various Contexts
 */
	.set	GP_1, 0
	.set	GP_2, (GP_1 + 4)
	.set	GP_13, (GP_2 + 4)
	.set	GP_14, (GP_13 + 4)

	.set	GP_15, (GP_14 + 4)
	.set	GP_16, (GP_15 + 4)
	.set	GP_17, (GP_16 + 4)
	.set	GP_18, (GP_17 + 4)

	.set	GP_19, (GP_18 + 4)
	.set	GP_20, (GP_19 + 4)
	.set	GP_21, (GP_20 + 4)
	.set	GP_22, (GP_21 + 4)

	.set	GP_23, (GP_22 + 4)
	.set	GP_24, (GP_23 + 4)
	.set	GP_25, (GP_24 + 4)
	.set	GP_26, (GP_25 + 4)

	.set	GP_27, (GP_26 + 4)
	.set	GP_28, (GP_27 + 4)
	.set	GP_29, (GP_28 + 4)
	.set	GP_30, (GP_29 + 4)

	.set	GP_31, (GP_30 + 4)
	.set	GP_CR, (GP_31 + 4)
	.set	GP_PC, (GP_CR + 4)
	.set	GP_MSR, (GP_PC + 4)

#if (PPC_HAS_DOUBLE == 1)
	.set	FP_0, 0
	.set	FP_1, (FP_0 + 8)
	.set	FP_2, (FP_1 + 8)
	.set	FP_3, (FP_2 + 8)
	.set	FP_4, (FP_3 + 8)
	.set	FP_5, (FP_4 + 8)
	.set	FP_6, (FP_5 + 8)
	.set	FP_7, (FP_6 + 8)
	.set	FP_8, (FP_7 + 8)
	.set	FP_9, (FP_8 + 8)
	.set	FP_10, (FP_9 + 8)
	.set	FP_11, (FP_10 + 8)
	.set	FP_12, (FP_11 + 8)
	.set	FP_13, (FP_12 + 8)
	.set	FP_14, (FP_13 + 8)
	.set	FP_15, (FP_14 + 8)
	.set	FP_16, (FP_15 + 8)
	.set	FP_17, (FP_16 + 8)
	.set	FP_18, (FP_17 + 8)
	.set	FP_19, (FP_18 + 8)
	.set	FP_20, (FP_19 + 8)
	.set	FP_21, (FP_20 + 8)
	.set	FP_22, (FP_21 + 8)
	.set	FP_23, (FP_22 + 8)
	.set	FP_24, (FP_23 + 8)
	.set	FP_25, (FP_24 + 8)
	.set	FP_26, (FP_25 + 8)
	.set	FP_27, (FP_26 + 8)
	.set	FP_28, (FP_27 + 8)
	.set	FP_29, (FP_28 + 8)
	.set	FP_30, (FP_29 + 8)
	.set	FP_31, (FP_30 + 8)
	.set	FP_FPSCR, (FP_31 + 8)
#else
	.set	FP_0, 0
	.set	FP_1, (FP_0 + 4)
	.set	FP_2, (FP_1 + 4)
	.set	FP_3, (FP_2 + 4)
	.set	FP_4, (FP_3 + 4)
	.set	FP_5, (FP_4 + 4)
	.set	FP_6, (FP_5 + 4)
	.set	FP_7, (FP_6 + 4)
	.set	FP_8, (FP_7 + 4)
	.set	FP_9, (FP_8 + 4)
	.set	FP_10, (FP_9 + 4)
	.set	FP_11, (FP_10 + 4)
	.set	FP_12, (FP_11 + 4)
	.set	FP_13, (FP_12 + 4)
	.set	FP_14, (FP_13 + 4)
	.set	FP_15, (FP_14 + 4)
	.set	FP_16, (FP_15 + 4)
	.set	FP_17, (FP_16 + 4)
	.set	FP_18, (FP_17 + 4)
	.set	FP_19, (FP_18 + 4)
	.set	FP_20, (FP_19 + 4)
	.set	FP_21, (FP_20 + 4)
	.set	FP_22, (FP_21 + 4)
	.set	FP_23, (FP_22 + 4)
	.set	FP_24, (FP_23 + 4)
	.set	FP_25, (FP_24 + 4)
	.set	FP_26, (FP_25 + 4)
	.set	FP_27, (FP_26 + 4)
	.set	FP_28, (FP_27 + 4)
	.set	FP_29, (FP_28 + 4)
	.set	FP_30, (FP_29 + 4)
	.set	FP_31, (FP_30 + 4)
	.set	FP_FPSCR, (FP_31 + 4)
#endif
	
	.set	IP_LINK, 0
#if (PPC_ABI == PPC_ABI_POWEROPEN || PPC_ABI == PPC_ABI_GCC27)
	.set	IP_0, (IP_LINK + 56)
#else
	.set	IP_0, (IP_LINK + 8)
#endif
	.set	IP_2, (IP_0 + 4)

	.set	IP_3, (IP_2 + 4)
	.set	IP_4, (IP_3 + 4)
	.set	IP_5, (IP_4 + 4)
	.set	IP_6, (IP_5 + 4)
	
	.set	IP_7, (IP_6 + 4)
	.set	IP_8, (IP_7 + 4)
	.set	IP_9, (IP_8 + 4)
	.set	IP_10, (IP_9 + 4)
	
	.set	IP_11, (IP_10 + 4)
	.set	IP_12, (IP_11 + 4)
	.set	IP_13, (IP_12 + 4)
	.set	IP_28, (IP_13 + 4)

	.set	IP_29, (IP_28 + 4)
	.set	IP_30, (IP_29 + 4)
	.set	IP_31, (IP_30 + 4)
	.set	IP_CR, (IP_31 + 4)
	
	.set	IP_CTR, (IP_CR + 4)
	.set	IP_XER, (IP_CTR + 4)
	.set	IP_LR, (IP_XER + 4)
	.set	IP_PC, (IP_LR + 4)
	
	.set	IP_MSR, (IP_PC + 4)
	.set	IP_END, (IP_MSR + 16)
	
	/* _CPU_IRQ_info offsets */
	/* These must be in this order */
	.set	Nest_level, 0
	.set	Disable_level, 4
	.set	Vector_table, 8
	.set	Stack, 12
#if (PPC_ABI == PPC_ABI_POWEROPEN)
	.set	Dispatch_r2, 16
	.set	Switch_necessary, 20
#else
	.set	Default_r2, 16
#if (PPC_ABI != PPC_ABI_GCC27)
	.set	Default_r13, 20
	.set	Switch_necessary, 24
#else
	.set	Switch_necessary, 20
#endif
#endif
	.set	Signal, Switch_necessary + 4

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
	lwz	r3, 0(r3)
#if (PPC_HAS_DOUBLE == 1)
	stfd	f0, FP_0(r3)
	stfd	f1, FP_1(r3)
	stfd	f2, FP_2(r3)
	stfd	f3, FP_3(r3)
	stfd	f4, FP_4(r3)
	stfd	f5, FP_5(r3)
	stfd	f6, FP_6(r3)
	stfd	f7, FP_7(r3)
	stfd	f8, FP_8(r3)
	stfd	f9, FP_9(r3)
	stfd	f10, FP_10(r3)
	stfd	f11, FP_11(r3)
	stfd	f12, FP_12(r3)
	stfd	f13, FP_13(r3)
	stfd	f14, FP_14(r3)
	stfd	f15, FP_15(r3)
	stfd	f16, FP_16(r3)
	stfd	f17, FP_17(r3)
	stfd	f18, FP_18(r3)
	stfd	f19, FP_19(r3)
	stfd	f20, FP_20(r3)
	stfd	f21, FP_21(r3)
	stfd	f22, FP_22(r3)
	stfd	f23, FP_23(r3)
	stfd	f24, FP_24(r3)
	stfd	f25, FP_25(r3)
	stfd	f26, FP_26(r3)
	stfd	f27, FP_27(r3)
	stfd	f28, FP_28(r3)
	stfd	f29, FP_29(r3)
	stfd	f30, FP_30(r3)
	stfd	f31, FP_31(r3)
	mffs	f2
	stfd	f2, FP_FPSCR(r3)
#else
	stfs	f0, FP_0(r3)
	stfs	f1, FP_1(r3)
	stfs	f2, FP_2(r3)
	stfs	f3, FP_3(r3)
	stfs	f4, FP_4(r3)
	stfs	f5, FP_5(r3)
	stfs	f6, FP_6(r3)
	stfs	f7, FP_7(r3)
	stfs	f8, FP_8(r3)
	stfs	f9, FP_9(r3)
	stfs	f10, FP_10(r3)
	stfs	f11, FP_11(r3)
	stfs	f12, FP_12(r3)
	stfs	f13, FP_13(r3)
	stfs	f14, FP_14(r3)
	stfs	f15, FP_15(r3)
	stfs	f16, FP_16(r3)
	stfs	f17, FP_17(r3)
	stfs	f18, FP_18(r3)
	stfs	f19, FP_19(r3)
	stfs	f20, FP_20(r3)
	stfs	f21, FP_21(r3)
	stfs	f22, FP_22(r3)
	stfs	f23, FP_23(r3)
	stfs	f24, FP_24(r3)
	stfs	f25, FP_25(r3)
	stfs	f26, FP_26(r3)
	stfs	f27, FP_27(r3)
	stfs	f28, FP_28(r3)
	stfs	f29, FP_29(r3)
	stfs	f30, FP_30(r3)
	stfs	f31, FP_31(r3)
	mffs	f2
	stfs	f2, FP_FPSCR(r3)
#endif
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
#if (PPC_HAS_DOUBLE == 1)
	lfd	f2, FP_FPSCR(r3)
	mtfsf	255, f2
	lfd	f0, FP_0(r3)
	lfd	f1, FP_1(r3)
	lfd	f2, FP_2(r3)
	lfd	f3, FP_3(r3)
	lfd	f4, FP_4(r3)
	lfd	f5, FP_5(r3)
	lfd	f6, FP_6(r3)
	lfd	f7, FP_7(r3)
	lfd	f8, FP_8(r3)
	lfd	f9, FP_9(r3)
	lfd	f10, FP_10(r3)
	lfd	f11, FP_11(r3)
	lfd	f12, FP_12(r3)
	lfd	f13, FP_13(r3)
	lfd	f14, FP_14(r3)
	lfd	f15, FP_15(r3)
	lfd	f16, FP_16(r3)
	lfd	f17, FP_17(r3)
	lfd	f18, FP_18(r3)
	lfd	f19, FP_19(r3)
	lfd	f20, FP_20(r3)
	lfd	f21, FP_21(r3)
	lfd	f22, FP_22(r3)
	lfd	f23, FP_23(r3)
	lfd	f24, FP_24(r3)
	lfd	f25, FP_25(r3)
	lfd	f26, FP_26(r3)
	lfd	f27, FP_27(r3)
	lfd	f28, FP_28(r3)
	lfd	f29, FP_29(r3)
	lfd	f30, FP_30(r3)
	lfd	f31, FP_31(r3)
#else
	lfs	f2, FP_FPSCR(r3)
	mtfsf	255, f2
	lfs	f0, FP_0(r3)
	lfs	f1, FP_1(r3)
	lfs	f2, FP_2(r3)
	lfs	f3, FP_3(r3)
	lfs	f4, FP_4(r3)
	lfs	f5, FP_5(r3)
	lfs	f6, FP_6(r3)
	lfs	f7, FP_7(r3)
	lfs	f8, FP_8(r3)
	lfs	f9, FP_9(r3)
	lfs	f10, FP_10(r3)
	lfs	f11, FP_11(r3)
	lfs	f12, FP_12(r3)
	lfs	f13, FP_13(r3)
	lfs	f14, FP_14(r3)
	lfs	f15, FP_15(r3)
	lfs	f16, FP_16(r3)
	lfs	f17, FP_17(r3)
	lfs	f18, FP_18(r3)
	lfs	f19, FP_19(r3)
	lfs	f20, FP_20(r3)
	lfs	f21, FP_21(r3)
	lfs	f22, FP_22(r3)
	lfs	f23, FP_23(r3)
	lfs	f24, FP_24(r3)
	lfs	f25, FP_25(r3)
	lfs	f26, FP_26(r3)
	lfs	f27, FP_27(r3)
	lfs	f28, FP_28(r3)
	lfs	f29, FP_29(r3)
	lfs	f30, FP_30(r3)
	lfs	f31, FP_31(r3)
#endif
#endif
	blr


/*  _CPU_Context_switch
 *
 *  This routine performs a normal non-FP context switch.
 */
	ALIGN (PPC_CACHE_ALIGNMENT, PPC_CACHE_ALIGN_POWER)
	PUBLIC_PROC (_CPU_Context_switch)
PROC (_CPU_Context_switch):
	sync
	isync
#if (PPC_CACHE_ALIGNMENT == 4) /* No cache */
	stw	r1, GP_1(r3)
	lwz	r1, GP_1(r4)
	stw	r2, GP_2(r3)
	lwz	r2, GP_2(r4)
#if (PPC_USE_MULTIPLE == 1)
	stmw	r13, GP_13(r3)
	lmw	r13, GP_13(r4)
#else
	stw	r13, GP_13(r3)
	lwz	r13, GP_13(r4)
	stw	r14, GP_14(r3)
	lwz	r14, GP_14(r4)
	stw	r15, GP_15(r3)
	lwz	r15, GP_15(r4)
	stw	r16, GP_16(r3)
	lwz	r16, GP_16(r4)
	stw	r17, GP_17(r3)
	lwz	r17, GP_17(r4)
	stw	r18, GP_18(r3)
	lwz	r18, GP_18(r4)
	stw	r19, GP_19(r3)
	lwz	r19, GP_19(r4)
	stw	r20, GP_20(r3)
	lwz	r20, GP_20(r4)
	stw	r21, GP_21(r3)
	lwz	r21, GP_21(r4)
	stw	r22, GP_22(r3)
	lwz	r22, GP_22(r4)
	stw	r23, GP_23(r3)
	lwz	r23, GP_23(r4)
	stw	r24, GP_24(r3)
	lwz	r24, GP_24(r4)
	stw	r25, GP_25(r3)
	lwz	r25, GP_25(r4)
	stw	r26, GP_26(r3)
	lwz	r26, GP_26(r4)
	stw	r27, GP_27(r3)
	lwz	r27, GP_27(r4)
	stw	r28, GP_28(r3)
	lwz	r28, GP_28(r4)
	stw	r29, GP_29(r3)
	lwz	r29, GP_29(r4)
	stw	r30, GP_30(r3)
	lwz	r30, GP_30(r4)
	stw	r31, GP_31(r3)
	lwz	r31, GP_31(r4)
#endif
	mfcr	r5
	stw	r5, GP_CR(r3)
	lwz	r5, GP_CR(r4)
	mflr	r6
	mtcrf	255, r5
	stw	r6, GP_PC(r3)
	lwz	r6, GP_PC(r4)
	mfmsr	r7
	mtlr	r6
	stw	r7, GP_MSR(r3)
	lwz	r7, GP_MSR(r4)
	mtmsr	r7
#endif
#if (PPC_CACHE_ALIGNMENT == 16)
	/* This assumes that all the registers are in the given order */
	li	r5, 16
	addi	r3,r3,-4
	dcbz	r5, r3
	stw	r1, GP_1+4(r3)
	stw	r2, GP_2+4(r3)
#if (PPC_USE_MULTIPLE == 1)
	addi	r3, r3, GP_14+4
	dcbz	r5, r3
	addi	r3, r3, GP_18-GP_14
	dcbz	r5, r3
	addi	r3, r3, GP_22-GP_18
	dcbz	r5, r3
	addi	r3, r3, GP_26-GP_22
	dcbz	r5, r3
	stmw	r13, GP_13-GP_26(r3)
#else
	stw	r13, GP_13+4(r3)
	stwu	r14, GP_14+4(r3)
	dcbz	r5, r3
	stw	r15, GP_15-GP_14(r3)
	stw	r16, GP_16-GP_14(r3)
	stw	r17, GP_17-GP_14(r3)
	stwu	r18, GP_18-GP_14(r3)
	dcbz	r5, r3
	stw	r19, GP_19-GP_18(r3)
	stw	r20, GP_20-GP_18(r3)
	stw	r21, GP_21-GP_18(r3)
	stwu	r22, GP_22-GP_18(r3)
	dcbz	r5, r3
	stw	r23, GP_23-GP_22(r3)
	stw	r24, GP_24-GP_22(r3)
	stw	r25, GP_25-GP_22(r3)
	stwu	r26, GP_26-GP_22(r3)
	dcbz	r5, r3
	stw	r27, GP_27-GP_26(r3)
	stw	r28, GP_28-GP_26(r3)
	stw	r29, GP_29-GP_26(r3)
	stw	r30, GP_30-GP_26(r3)
	stw	r31, GP_31-GP_26(r3)
#endif
	dcbt	r0, r4
	mfcr	r6
	stw	r6, GP_CR-GP_26(r3)
	mflr	r7
	stw	r7, GP_PC-GP_26(r3)
	mfmsr	r8
	stw	r8, GP_MSR-GP_26(r3)
	
	dcbt	r5, r4
	lwz	r1, GP_1(r4)
	lwz	r2, GP_2(r4)
#if (PPC_USE_MULTIPLE == 1)
	addi	r4, r4, GP_15
	dcbt	r5, r4
	addi	r4, r4, GP_19-GP_15
	dcbt	r5, r4
	addi	r4, r4, GP_23-GP_19
	dcbt	r5, r4
	addi	r4, r4, GP_27-GP_23
	dcbt	r5, r4
	lmw	r13, GP_13-GP_27(r4)
#else
	lwz	r13, GP_13(r4)
	lwz	r14, GP_14(r4)
	lwzu	r15, GP_15(r4)
	dcbt	r5, r4
	lwz	r16, GP_16-GP_15(r4)
	lwz	r17, GP_17-GP_15(r4)
	lwz	r18, GP_18-GP_15(r4)
	lwzu	r19, GP_19-GP_15(r4)
	dcbt	r5, r4
	lwz	r20, GP_20-GP_19(r4)
	lwz	r21, GP_21-GP_19(r4)
	lwz	r22, GP_22-GP_19(r4)
	lwzu	r23, GP_23-GP_19(r4)
	dcbt	r5, r4
	lwz	r24, GP_24-GP_23(r4)
	lwz	r25, GP_25-GP_23(r4)
	lwz	r26, GP_26-GP_23(r4)
	lwzu	r27, GP_27-GP_23(r4)
	dcbt	r5, r4
	lwz	r28, GP_28-GP_27(r4)
	lwz	r29, GP_29-GP_27(r4)
	lwz	r30, GP_30-GP_27(r4)
	lwz	r31, GP_31-GP_27(r4)
#endif
	lwz	r6, GP_CR-GP_27(r4)
	lwz	r7, GP_PC-GP_27(r4)
	lwz	r8, GP_MSR-GP_27(r4)
	mtcrf	255, r6
	mtlr	r7
	mtmsr	r8
#endif
#if (PPC_CACHE_ALIGNMENT == 32)
	/* This assumes that all the registers are in the given order */
	li	r5, 32
	addi	r3,r3,-4
	dcbz	r5, r3
	stw	r1, GP_1+4(r3)
	stw	r2, GP_2+4(r3)
#if (PPC_USE_MULTIPLE == 1)
	addi	r3, r3, GP_18+4
	dcbz	r5, r3
	stmw	r13, GP_13-GP_18(r3)
#else
	stw	r13, GP_13+4(r3)
	stw	r14, GP_14+4(r3)
	stw	r15, GP_15+4(r3)
	stw	r16, GP_16+4(r3)
	stw	r17, GP_17+4(r3)
	stwu	r18, GP_18+4(r3)
	dcbz	r5, r3
	stw	r19, GP_19-GP_18(r3)
	stw	r20, GP_20-GP_18(r3)
	stw	r21, GP_21-GP_18(r3)
	stw	r22, GP_22-GP_18(r3)
	stw	r23, GP_23-GP_18(r3)
	stw	r24, GP_24-GP_18(r3)
	stw	r25, GP_25-GP_18(r3)
	stw	r26, GP_26-GP_18(r3)
	stw	r27, GP_27-GP_18(r3)
	stw	r28, GP_28-GP_18(r3)
	stw	r29, GP_29-GP_18(r3)
	stw	r30, GP_30-GP_18(r3)
	stw	r31, GP_31-GP_18(r3)
#endif
	dcbt	r0, r4
	mfcr	r6
	stw	r6, GP_CR-GP_18(r3)
	mflr	r7
	stw	r7, GP_PC-GP_18(r3)
	mfmsr	r8
	stw	r8, GP_MSR-GP_18(r3)

	dcbt	r5, r4
	lwz	r1, GP_1(r4)
	lwz	r2, GP_2(r4)
#if (PPC_USE_MULTIPLE == 1)
	addi	r4, r4, GP_19
	dcbt	r5, r4
	lmw	r13, GP_13-GP_19(r4)
#else
	lwz	r13, GP_13(r4)
	lwz	r14, GP_14(r4)
	lwz	r15, GP_15(r4)
	lwz	r16, GP_16(r4)
	lwz	r17, GP_17(r4)
	lwz	r18, GP_18(r4)
	lwzu	r19, GP_19(r4)
	dcbt	r5, r4
	lwz	r20, GP_20-GP_19(r4)
	lwz	r21, GP_21-GP_19(r4)
	lwz	r22, GP_22-GP_19(r4)
	lwz	r23, GP_23-GP_19(r4)
	lwz	r24, GP_24-GP_19(r4)
	lwz	r25, GP_25-GP_19(r4)
	lwz	r26, GP_26-GP_19(r4)
	lwz	r27, GP_27-GP_19(r4)
	lwz	r28, GP_28-GP_19(r4)
	lwz	r29, GP_29-GP_19(r4)
	lwz	r30, GP_30-GP_19(r4)
	lwz	r31, GP_31-GP_19(r4)
#endif
	lwz	r6, GP_CR-GP_19(r4)
	lwz	r7, GP_PC-GP_19(r4)
	lwz	r8, GP_MSR-GP_19(r4)
	mtcrf	255, r6
	mtlr	r7
	mtmsr	r8
#endif
	blr

/*
 *  _CPU_Context_restore
 *
 *  This routine is generally used only to restart self in an
 *  efficient manner.  It may simply be a label in _CPU_Context_switch.
 *
 *  NOTE: May be unnecessary to reload some registers.
 */
/*
 * ACB: Don't worry about cache optimisation here - this is not THAT critical.
 */
	ALIGN (PPC_CACHE_ALIGNMENT, PPC_CACHE_ALIGN_POWER)
	PUBLIC_PROC (_CPU_Context_restore)
PROC (_CPU_Context_restore):
	lwz	r5, GP_CR(r3)
	lwz	r6, GP_PC(r3)
	lwz	r7, GP_MSR(r3)
	mtcrf	255, r5
	mtlr	r6
	mtmsr	r7
	lwz	r1, GP_1(r3)
	lwz	r2, GP_2(r3)
#if (PPC_USE_MULTIPLE == 1)
	lmw	r13, GP_13(r3)
#else
	lwz	r13, GP_13(r3)
	lwz	r14, GP_14(r3)
	lwz	r15, GP_15(r3)
	lwz	r16, GP_16(r3)
	lwz	r17, GP_17(r3)
	lwz	r18, GP_18(r3)
	lwz	r19, GP_19(r3)
	lwz	r20, GP_20(r3)
	lwz	r21, GP_21(r3)
	lwz	r22, GP_22(r3)
	lwz	r23, GP_23(r3)
	lwz	r24, GP_24(r3)
	lwz	r25, GP_25(r3)
	lwz	r26, GP_26(r3)
	lwz	r27, GP_27(r3)
	lwz	r28, GP_28(r3)
	lwz	r29, GP_29(r3)
	lwz	r30, GP_30(r3)
	lwz	r31, GP_31(r3)
#endif

	blr

/*  Individual interrupt prologues look like this:
 * #if (PPC_ABI == PPC_ABI_POWEROPEN || PPC_ABI == PPC_ABI_GCC27)
 * #if (PPC_HAS_FPU)
 *	stwu	r1, -(20*4 + 18*8 + IP_END)(r1)
 * #else
 *	stwu	r1, -(20*4 + IP_END)(r1)
 * #endif
 * #else
 *	stwu	r1, -(IP_END)(r1)
 * #endif
 *	stw	r0, IP_0(r1)
 *
 *	li      r0, vectornum
 *	b       PROC (_ISR_Handler{,C})
 */

/*  void __ISR_Handler()
 *
 *  This routine provides the RTEMS interrupt management.
 *  The vector number is in r0. R0 has already been stacked.
 *
 */
	ALIGN (PPC_CACHE_ALIGNMENT, PPC_CACHE_ALIGN_POWER)
	PUBLIC_PROC (_ISR_Handler)
PROC (_ISR_Handler):
#define LABEL(x)	x
#define MTSAVE(x)	mtspr	sprg0, x
#define MFSAVE(x)	mfspr	x, sprg0
#define MTPC(x)		mtspr	srr0, x
#define MFPC(x)		mfspr	x, srr0
#define MTMSR(x)	mtspr	srr1, x
#define MFMSR(x)	mfspr	x, srr1
	#include	"irq_stub.s"
	rfi

#if (PPC_HAS_RFCI == 1)
/*  void __ISR_HandlerC()
 *
 *  This routine provides the RTEMS interrupt management.
 *  For critical interrupts
 *
 */
	ALIGN (PPC_CACHE_ALIGNMENT, PPC_CACHE_ALIGN_POWER)
	PUBLIC_PROC (_ISR_HandlerC)
PROC (_ISR_HandlerC):
#undef	LABEL
#undef	MTSAVE
#undef	MFSAVE
#undef	MTPC
#undef	MFPC
#undef	MTMSR
#undef	MFMSR
#define LABEL(x)	x##_C
#define MTSAVE(x)	mtspr	sprg1, x
#define MFSAVE(x)	mfspr	x, sprg1
#define MTPC(x)		mtspr	srr2, x
#define MFPC(x)		mfspr	x, srr2
#define MTMSR(x)	mtspr	srr3, x
#define MFMSR(x)	mfspr	x, srr3
	#include	"irq_stub.s"
	rfci
#endif

/*  PowerOpen descriptors for indirect function calls.
 */

#if (PPC_ABI == PPC_ABI_POWEROPEN)
	DESCRIPTOR (_CPU_Context_save_fp)
	DESCRIPTOR (_CPU_Context_restore_fp)
	DESCRIPTOR (_CPU_Context_switch)
	DESCRIPTOR (_CPU_Context_restore)
	DESCRIPTOR (_ISR_Handler)
#if (PPC_HAS_RFCI == 1)
	DESCRIPTOR (_ISR_HandlerC)
#endif
#endif
