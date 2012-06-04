/*
 * Copyright (c) 2011 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <bspopts.h>
#include <rtems/score/percpu.h>
#include <bsp/vectors.h>

#define VECTOR_REGISTER r4
#define ISR_NEST_HADDR_REGISTER r5
#define ISR_NEST_REGISTER r6
#define DISPATCH_LEVEL_REGISTER r7
#define HANDLER_REGISTER r8
#define SCRATCH_0_REGISTER r0
#define SCRATCH_1_REGISTER r3
#define SCRATCH_2_REGISTER r9
#define SCRATCH_3_REGISTER r10
#define SCRATCH_4_REGISTER r11
#define SCRATCH_5_REGISTER r12
#define FRAME_REGISTER r14

#define VECTOR_OFFSET(reg) GPR4_OFFSET(reg)
#define ISR_NEST_HADDR_OFFSET(reg) GPR5_OFFSET(reg)
#define ISR_NEST_OFFSET(reg) GPR6_OFFSET(reg)
#define DISPATCH_LEVEL_OFFSET(reg) GPR7_OFFSET(reg)
#define HANDLER_OFFSET(reg) GPR8_OFFSET(reg)
#define SCRATCH_0_OFFSET(reg) GPR0_OFFSET(reg)
#define SCRATCH_1_OFFSET(reg) GPR3_OFFSET(reg)
#define SCRATCH_2_OFFSET(reg) GPR9_OFFSET(reg)
#define SCRATCH_3_OFFSET(reg) GPR10_OFFSET(reg)
#define SCRATCH_4_OFFSET(reg) GPR11_OFFSET(reg)
#define SCRATCH_5_OFFSET(reg) GPR12_OFFSET(reg)

/*
 * The register 2 slot is free, since this is the read-only small data anchor.
 */
#define FRAME_OFFSET(reg) GPR2_OFFSET(reg)

	.global	ppc_exc_min_prolog_async_tmpl_normal
	.global ppc_exc_wrap_async_normal

ppc_exc_min_prolog_async_tmpl_normal:

	stwu	r1, -PPC_EXC_MINIMAL_FRAME_SIZE(r1)
	stw	VECTOR_REGISTER, PPC_EXC_VECTOR_PROLOGUE_OFFSET(r1)
	li	VECTOR_REGISTER, 0xffff8000

	/*
	 * We store the absolute branch target address here.  It will be used
	 * to generate the branch operation in ppc_exc_make_prologue().
	 */
	.int	ppc_exc_wrap_async_normal

ppc_exc_wrap_async_normal:

	/* Save non-volatile FRAME_REGISTER */
	stw	FRAME_REGISTER, FRAME_OFFSET(r1)

#ifdef __SPE__
	/* Enable SPE */
	mfmsr	FRAME_REGISTER
	oris	FRAME_REGISTER, FRAME_REGISTER, MSR_SPE >> 16
	mtmsr	FRAME_REGISTER
	isync
#endif

	/* Move frame pointer to non-volatile FRAME_REGISTER */
	mr	FRAME_REGISTER, r1

	/* Load ISR nest level and thread dispatch disable level */
	PPC_GPR_STORE	ISR_NEST_HADDR_REGISTER, ISR_NEST_HADDR_OFFSET(r1)
	lis	ISR_NEST_HADDR_REGISTER, ISR_NEST_LEVEL@ha
	PPC_GPR_STORE	ISR_NEST_REGISTER, ISR_NEST_OFFSET(r1)
	lwz	ISR_NEST_REGISTER, ISR_NEST_LEVEL@l(ISR_NEST_HADDR_REGISTER)
	PPC_GPR_STORE	DISPATCH_LEVEL_REGISTER, DISPATCH_LEVEL_OFFSET(r1)
	lwz	DISPATCH_LEVEL_REGISTER, _Thread_Dispatch_disable_level@sdarel(r13)

	PPC_GPR_STORE	SCRATCH_0_REGISTER, SCRATCH_0_OFFSET(r1)

#ifdef __SPE__
	/*
	 * Save high order part of VECTOR_REGISTER here.  The low order part
	 * was saved in the minimal prologue.
	 */
	evmergehi	SCRATCH_0_REGISTER, SCRATCH_0_REGISTER, VECTOR_REGISTER
	stw	SCRATCH_0_REGISTER, VECTOR_OFFSET(r1)
#endif

	PPC_GPR_STORE	HANDLER_REGISTER, HANDLER_OFFSET(r1)

	/*
	 * Load the handler address.  Get the handler table index from the
	 * vector number.  We have to discard the exception type.  Take only
	 * the least significant five bits (= LAST_VALID_EXC + 1) from the
	 * vector register.  Multiply by four (= size of function pointer).
	 */
	rlwinm	SCRATCH_0_REGISTER, VECTOR_REGISTER, 2, 25, 29
	lis	HANDLER_REGISTER, ppc_exc_handler_table@h
	ori	HANDLER_REGISTER, HANDLER_REGISTER, ppc_exc_handler_table@l
	lwzx	HANDLER_REGISTER, HANDLER_REGISTER, SCRATCH_0_REGISTER

	PPC_GPR_STORE	SCRATCH_1_REGISTER, SCRATCH_1_OFFSET(r1)
	PPC_GPR_STORE	SCRATCH_2_REGISTER, SCRATCH_2_OFFSET(r1)
	PPC_GPR_STORE	SCRATCH_3_REGISTER, SCRATCH_3_OFFSET(r1)
	PPC_GPR_STORE	SCRATCH_4_REGISTER, SCRATCH_4_OFFSET(r1)
	PPC_GPR_STORE	SCRATCH_5_REGISTER, SCRATCH_5_OFFSET(r1)

	/* Save SRR0, SRR1, CR, CTR, XER, and LR */
	mfsrr0	SCRATCH_0_REGISTER
	mfsrr1	SCRATCH_1_REGISTER
	mfcr	SCRATCH_2_REGISTER
	mfctr	SCRATCH_3_REGISTER
	mfxer	SCRATCH_4_REGISTER
	mflr	SCRATCH_5_REGISTER
	stw	SCRATCH_0_REGISTER, SRR0_FRAME_OFFSET(r1)
	stw	SCRATCH_1_REGISTER, SRR1_FRAME_OFFSET(r1)
	stw	SCRATCH_2_REGISTER, EXC_CR_OFFSET(r1)
	stw	SCRATCH_3_REGISTER, EXC_CTR_OFFSET(r1)
	stw	SCRATCH_4_REGISTER, EXC_XER_OFFSET(r1)
	stw	SCRATCH_5_REGISTER, EXC_LR_OFFSET(r1)

#ifdef __SPE__
	/* Save SPEFSCR and ACC */
	mfspr	SCRATCH_0_REGISTER, FSL_EIS_SPEFSCR
	evxor	SCRATCH_1_REGISTER, SCRATCH_1_REGISTER, SCRATCH_1_REGISTER
	evmwumiaa	SCRATCH_1_REGISTER, SCRATCH_1_REGISTER, SCRATCH_1_REGISTER
	stw	SCRATCH_0_REGISTER, PPC_EXC_SPEFSCR_OFFSET(r1)
	evstdd	SCRATCH_1_REGISTER, PPC_EXC_ACC_OFFSET(r1)
#endif

	/* Increment ISR nest level and thread dispatch disable level */
	cmpwi	ISR_NEST_REGISTER, 0
	addi	ISR_NEST_REGISTER, ISR_NEST_REGISTER, 1
	addi	DISPATCH_LEVEL_REGISTER, DISPATCH_LEVEL_REGISTER, 1
	stw	ISR_NEST_REGISTER, ISR_NEST_LEVEL@l(ISR_NEST_HADDR_REGISTER)
	stw	DISPATCH_LEVEL_REGISTER, _Thread_Dispatch_disable_level@sdarel(r13)

	/* Switch stack if necessary */
	mfspr	SCRATCH_0_REGISTER, SPRG1
	iselgt	r1, r1, SCRATCH_0_REGISTER

	/*
	 * Call high level exception handler.
	 *
	 * First parameter = exception frame pointer + FRAME_LINK_SPACE
	 * Second parameter = vector number (r4 is the VECTOR_REGISTER)
	 */
	addi	r3, FRAME_REGISTER, FRAME_LINK_SPACE
	rlwinm	VECTOR_REGISTER, VECTOR_REGISTER, 0, 27, 31
	mtctr	HANDLER_REGISTER
	bctrl

	/* Load ISR nest level and thread dispatch disable level */
	lis	ISR_NEST_HADDR_REGISTER, ISR_NEST_LEVEL@ha
	lwz	ISR_NEST_REGISTER, ISR_NEST_LEVEL@l(ISR_NEST_HADDR_REGISTER)
	lwz	DISPATCH_LEVEL_REGISTER, _Thread_Dispatch_disable_level@sdarel(r13)

	/*
	 * Switch back to original stack (FRAME_REGISTER == r1 if we are still
	 * on the IRQ stack) and restore FRAME_REGISTER.
	 */
	mr	r1, FRAME_REGISTER
	lwz	FRAME_REGISTER, FRAME_OFFSET(r1)

	/* Decrement ISR nest level and thread dispatch disable level */
	subi	ISR_NEST_REGISTER, ISR_NEST_REGISTER, 1
	subic.	DISPATCH_LEVEL_REGISTER, DISPATCH_LEVEL_REGISTER, 1
	stw	ISR_NEST_REGISTER, ISR_NEST_LEVEL@l(ISR_NEST_HADDR_REGISTER)
	stw	DISPATCH_LEVEL_REGISTER, _Thread_Dispatch_disable_level@sdarel(r13)

	/* Call thread dispatcher if necessary */
	bne	thread_dispatching_done
	bl	_Thread_Dispatch
thread_dispatching_done:

#ifdef __SPE__
	/* Load SPEFSCR and ACC */
	lwz	DISPATCH_LEVEL_REGISTER, PPC_EXC_SPEFSCR_OFFSET(r1)
	evldd	HANDLER_REGISTER, PPC_EXC_ACC_OFFSET(r1)
#endif

	/* Load SRR0, SRR1, CR, CTR, XER, and LR */
	lwz	SCRATCH_0_REGISTER, SRR0_FRAME_OFFSET(r1)
	lwz	SCRATCH_1_REGISTER, SRR1_FRAME_OFFSET(r1)
	lwz	SCRATCH_2_REGISTER, EXC_CR_OFFSET(r1)
	lwz	SCRATCH_3_REGISTER, EXC_CTR_OFFSET(r1)
	lwz	SCRATCH_4_REGISTER, EXC_XER_OFFSET(r1)
	lwz	SCRATCH_5_REGISTER, EXC_LR_OFFSET(r1)

	PPC_GPR_LOAD	VECTOR_REGISTER, VECTOR_OFFSET(r1)
	PPC_GPR_LOAD	ISR_NEST_HADDR_REGISTER, ISR_NEST_HADDR_OFFSET(r1)
	PPC_GPR_LOAD	ISR_NEST_REGISTER, ISR_NEST_OFFSET(r1)

#ifdef __SPE__
	/* Restore SPEFSCR */
	mtspr	FSL_EIS_SPEFSCR, DISPATCH_LEVEL_REGISTER
#endif
	PPC_GPR_LOAD	DISPATCH_LEVEL_REGISTER, DISPATCH_LEVEL_OFFSET(r1)

#ifdef __SPE__
	/* Restore ACC */
	evmra	HANDLER_REGISTER, HANDLER_REGISTER
#endif
	PPC_GPR_LOAD	HANDLER_REGISTER, HANDLER_OFFSET(r1)

	/* Restore SRR0, SRR1, CR, CTR, XER, and LR */
	mtsrr0	SCRATCH_0_REGISTER
	PPC_GPR_LOAD	SCRATCH_0_REGISTER, SCRATCH_0_OFFSET(r1)
	mtsrr1	SCRATCH_1_REGISTER
	PPC_GPR_LOAD	SCRATCH_1_REGISTER, SCRATCH_1_OFFSET(r1)
	mtcr	SCRATCH_2_REGISTER
	PPC_GPR_LOAD	SCRATCH_2_REGISTER, SCRATCH_2_OFFSET(r1)
	mtctr	SCRATCH_3_REGISTER
	PPC_GPR_LOAD	SCRATCH_3_REGISTER, SCRATCH_3_OFFSET(r1)
	mtxer	SCRATCH_4_REGISTER
	PPC_GPR_LOAD	SCRATCH_4_REGISTER, SCRATCH_4_OFFSET(r1)
	mtlr	SCRATCH_5_REGISTER
	PPC_GPR_LOAD	SCRATCH_5_REGISTER, SCRATCH_5_OFFSET(r1)

	/* Pop stack */
	addi	r1, r1, PPC_EXC_MINIMAL_FRAME_SIZE

	/* Return */
	rfi
