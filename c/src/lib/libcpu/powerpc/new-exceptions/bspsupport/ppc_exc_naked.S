/**
 * @file
 *
 * @ingroup ppc_exc
 *
 * @brief PowerPC Exceptions implementation.
 */

/*
 * Copyright (c) 2009
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include "ppc_exc_asm_macros.h"

	.global ppc_exc_min_prolog_tmpl_naked

ppc_exc_min_prolog_tmpl_naked:

	stwu	r1, -EXCEPTION_FRAME_END(r1)
	stw	VECTOR_REGISTER, VECTOR_OFFSET(r1)
	li	VECTOR_REGISTER, 0

	/*
	 * We store the absolute branch target address here.  It will be used
	 * to generate the branch operation in ppc_exc_make_prologue().
	 */
	.int	ppc_exc_wrap_naked

        .global ppc_exc_wrap_naked
ppc_exc_wrap_naked:

	/* Save scratch registers */
	stw	SCRATCH_REGISTER_0, SCRATCH_REGISTER_0_OFFSET(r1)
	stw	SCRATCH_REGISTER_1, SCRATCH_REGISTER_1_OFFSET(r1)
	stw	SCRATCH_REGISTER_2, SCRATCH_REGISTER_2_OFFSET(r1)

	/* Save volatile registers */
	stw	r0, GPR0_OFFSET(r1)
	stw	r3, GPR3_OFFSET(r1)
	stw	r8, GPR8_OFFSET(r1)
	stw	r9, GPR9_OFFSET(r1)
	stw	r10, GPR10_OFFSET(r1)
	stw	r11, GPR11_OFFSET(r1)
	stw	r12, GPR12_OFFSET(r1)

	/* Save CR */
	mfcr	SCRATCH_REGISTER_0
	stw	SCRATCH_REGISTER_0, EXC_CR_OFFSET(r1)

	/* Save SRR0 */
	mfspr	SCRATCH_REGISTER_0, srr0
	stw	SCRATCH_REGISTER_0, SRR0_FRAME_OFFSET(r1)

	/* Save SRR1 */
	mfspr	SCRATCH_REGISTER_0, srr1
	stw	SCRATCH_REGISTER_0, SRR1_FRAME_OFFSET(r1)

	/* Save CTR */
	mfctr	SCRATCH_REGISTER_0
	stw	SCRATCH_REGISTER_0, EXC_CTR_OFFSET(r1)

	/* Save XER */
	mfxer	SCRATCH_REGISTER_0
	stw	SCRATCH_REGISTER_0, EXC_XER_OFFSET(r1)

	/* Save LR */
	mflr	SCRATCH_REGISTER_0
	stw	SCRATCH_REGISTER_0, EXC_LR_OFFSET(r1)

	/* Load MSR bit mask */
	lwz	SCRATCH_REGISTER_0, ppc_exc_msr_bits@sdarel(r13)

	/*
	 * Change the MSR if necessary (MMU, RI), remember decision in
	 * non-volatile CR_MSR.
	 */
	cmpwi	CR_MSR, SCRATCH_REGISTER_0, 0
	bne	CR_MSR, wrap_change_msr_naked

wrap_change_msr_done_naked:

	/*
	 * Call high level exception handler
	 */

	/*
	 * Get the handler table index from the vector number.  We have to
	 * discard the exception type.  Take only the least significant five
	 * bits (= LAST_VALID_EXC + 1) from the vector register.  Multiply by
	 * four (= size of function pointer).
	 */
	rlwinm	SCRATCH_REGISTER_1, VECTOR_REGISTER, 2, 25, 29

	/* Load handler table address */
	LA	SCRATCH_REGISTER_0, ppc_exc_handler_table

	/* Load handler address */
	lwzx	SCRATCH_REGISTER_0, SCRATCH_REGISTER_0, SCRATCH_REGISTER_1

	/*
	 * First parameter = exception frame pointer + FRAME_LINK_SPACE
	 *
	 * We add FRAME_LINK_SPACE to the frame pointer because the high level
	 * handler expects a BSP_Exception_frame structure.
	 */
	addi	r3, r1, FRAME_LINK_SPACE

	/*
	 * Second parameter = vector number (r4 is the VECTOR_REGISTER)
	 *
	 * Discard the exception type and store the vector number
	 * in the vector register.  Take only the least significant
	 * five bits (= LAST_VALID_EXC + 1).
	 */
	rlwinm	VECTOR_REGISTER, VECTOR_REGISTER, 0, 27, 31

	/* Call handler */
	mtctr	SCRATCH_REGISTER_0
	bctrl

	/* Restore MSR? */
	bne	CR_MSR, wrap_restore_msr_naked

wrap_restore_msr_done_naked:

	/* Restore XER and CTR */
	lwz	SCRATCH_REGISTER_0, EXC_XER_OFFSET(r1)
	lwz	SCRATCH_REGISTER_1, EXC_CTR_OFFSET(r1)
	mtxer	SCRATCH_REGISTER_0
	mtctr	SCRATCH_REGISTER_1

	/* Restore CR and LR */
	lwz	SCRATCH_REGISTER_0, EXC_CR_OFFSET(r1)
	lwz	SCRATCH_REGISTER_1, EXC_LR_OFFSET(r1)
	mtcr	SCRATCH_REGISTER_0
	mtlr	SCRATCH_REGISTER_1

	/* Restore volatile registers */
	lwz	r0, GPR0_OFFSET(r1)
	lwz	r3, GPR3_OFFSET(r1)
	lwz	r8, GPR8_OFFSET(r1)
	lwz	r9, GPR9_OFFSET(r1)
	lwz	r10, GPR10_OFFSET(r1)
	lwz	r11, GPR11_OFFSET(r1)
	lwz	r12, GPR12_OFFSET(r1)

	/* Restore vector register */
	lwz	VECTOR_REGISTER, VECTOR_OFFSET(r1)

	/* Restore scratch registers and SRRs */
	lwz	SCRATCH_REGISTER_0, SRR0_FRAME_OFFSET(r1)
	lwz	SCRATCH_REGISTER_1, SRR1_FRAME_OFFSET(r1)
	lwz	SCRATCH_REGISTER_2, SCRATCH_REGISTER_2_OFFSET(r1)
	mtspr	srr0, SCRATCH_REGISTER_0
	lwz	SCRATCH_REGISTER_0, SCRATCH_REGISTER_0_OFFSET(r1)
	mtspr	srr1, SCRATCH_REGISTER_1
	lwz	SCRATCH_REGISTER_1, SCRATCH_REGISTER_1_OFFSET(r1)

	/*
	 * We restore r1 from the frame rather than just popping (adding to
	 * current r1) since the exception handler might have done strange
	 * things (e.g. a debugger moving and relocating the stack).
	 */
	lwz	r1, 0(r1)

	/* Return */
	rfi

wrap_change_msr_naked:

	mfmsr	SCRATCH_REGISTER_1
	or	SCRATCH_REGISTER_1, SCRATCH_REGISTER_1, SCRATCH_REGISTER_0
	mtmsr	SCRATCH_REGISTER_1
	sync
	isync
	b	wrap_change_msr_done_naked

wrap_restore_msr_naked:

	lwz	SCRATCH_REGISTER_0, ppc_exc_msr_bits@sdarel(r13)
	mfmsr	SCRATCH_REGISTER_1
	andc	SCRATCH_REGISTER_1, SCRATCH_REGISTER_1, SCRATCH_REGISTER_0
	mtmsr	SCRATCH_REGISTER_1
	sync
	isync
	b	wrap_restore_msr_done_naked
