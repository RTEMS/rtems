/*
 * (c) 1999, Eric Valette valette@crf.canon.fr
 *
 *
 *  This file contains the assembly code for the PowerPC
 *  exception veneers for RTEMS.
 *
 * $Id$
 */

#include <rtems/asm.h>
#include <rtems/score/cpu.h>
#include <bsp/vectors.h>
#include <libcpu/raw_exception.h>
		
#define SYNC \
	sync; \
	isync

	.text
	/* 603e shadows GPR0..GPR3 for certain exceptions. We must switch
	 * that off before we can use the stack pointer. Note that this is
	 * ONLY safe if the shadowing is actually active -- otherwise, r1
         * is destroyed. We deliberately use r1 so problems become obvious
	 * if this is abused!
	 */
PUBLIC_VAR(tgpr_clr_exception_vector_code_prolog)
SYM (tgpr_clr_exception_vector_code_prolog):
	mfmsr	r1
	rlwinm	r1,r1,0,15,13
	mtmsr	r1
	isync
	/* fall thru */
PUBLIC_VAR(default_exception_vector_code_prolog)
SYM (default_exception_vector_code_prolog):
	/*
	 * let room for exception frame
	 */
	stw	r3, GPR3_OFFSET-EXCEPTION_FRAME_END(r1)
	mflr	r3
	stw	r3, EXC_LR_OFFSET-EXCEPTION_FRAME_END(r1)
	bla	push_normalized_frame

	/* IMPORTANT: prologue size MUST be < 32 bytes; 'altivec unavailable' exception
	 *            is already at 0xf20 :-(
	 */

	PUBLIC_VAR (default_exception_vector_code_prolog_size)
	PUBLIC_VAR (tgpr_clr_exception_vector_code_prolog_size)

	default_exception_vector_code_prolog_size = . - default_exception_vector_code_prolog
	tgpr_clr_exception_vector_code_prolog_size= . - tgpr_clr_exception_vector_code_prolog

	.p2align 5
PUBLIC_VAR (push_normalized_frame)
SYM (push_normalized_frame):
	stwu    r1, - (EXCEPTION_FRAME_END)(r1)
	mfcr	r3
	stw	r3,  EXC_CR_OFFSET(r1)
	/*
	 * r3 = exception vector entry point
	 * (256 * vector number) + few instructions
	 */
	/*
	 * FIXME: vectors should distingish
	 * all bits in mask 0x00003ff0
	 * and keep in mind that the LR/R3 contains the 
	 * address BEHIND the entry code 
	 */
	mflr	r3
	/* mask upper bits in case vectors are in the high area (psim) */
	rlwinm  r3, r3, 32-5, 20, 31
#if defined(ASM_60X_VEC_VECTOR)
	/*
	 * Remap altivec unavaliable (0xf20) to its vector number...
	 */
	cmplwi  r3,(ASM_60X_VEC_VECTOR_OFFSET>>5)
	bne     1f
	li		r3,ASM_60X_VEC_VECTOR<<3
1:
#endif
	/*
	 * r3 = r3 >> 8 = vector #
	 */
	srwi	r3,r3,3
	stw	r3, EXCEPTION_NUMBER_OFFSET(r1)
	stw	r0, GPR0_OFFSET(r1)
	/* R2 should never change (EABI: pointer to .sdata2) - we
	 * save it nevertheless..
	 */
	stw	r2, GPR2_OFFSET(r1)
#if defined(ASM_VECTORS_CRITICAL)
	lis	r0,ASM_VECTORS_CRITICAL@h
	ori	r0,r0,ASM_VECTORS_CRITICAL@l
	rlwnm.  r0,r0,r3,0,0
	beq	1f
	/*
	 * NOTE: srr2/3 are stored in slots SRR0/1
	 * for critical exceptions
	 */ 
	mfsrr2  r3
	stw	r3, SRR0_FRAME_OFFSET(r1)
	mfsrr3  r3
	stw	r3, SRR1_FRAME_OFFSET(r1)
	b	2f
1:		
#endif	
	mfsrr0  r3
	stw	r3, SRR0_FRAME_OFFSET(r1)
	mfsrr1  r3
	stw	r3, SRR1_FRAME_OFFSET(r1)
2:	
	/*
	 * Save general purpose registers
	 * Already saved in prolog : R1, R3, LR.
	 * Saved a few line above  : R0, R2
	 *
	 * Manual says that "stmw" instruction may be slower than
	 * series of individual "stw" but who cares about performance
	 * for the DEFAULT exception handler?
	 */
	stmw	r4, GPR4_OFFSET(r1)	/* save R4->R31 */

	mfctr	r30
	stw	r30,  EXC_CTR_OFFSET(r1)
	mfxer	r28
	stw	r28,  EXC_XER_OFFSET(r1)
	mfmsr	r28
	stw	r28,  EXC_MSR_OFFSET(r1)
	mfdar	r28
	stw	r28,  EXC_DAR_OFFSET(r1)
	/*
	 * compute SP at exception entry
	 */
	addi    r3, r1, EXCEPTION_FRAME_END
	/*
	 * store it at the right place
	 */
	stw     r3, GPR1_OFFSET(r1)

#if defined(PPC_MSR_EXC_BITS)
	/*
	 * Enable data and instruction address translation, exception nesting
	 */
	mfmsr	r3
	ori     r3,r3, PPC_MSR_EXC_BITS
	mtmsr   r3
	SYNC
#endif
	/*
	 * Call C exception handler
	 */
	/*
	 * store the execption frame address in r3 (first param)
	 */
	addi	r3, r1, 0x8
	/* clear CR[6] to make sure no varargs fn callee assumes there are FP args passed */
	crxor	6,6,6
	/*
	 * globalExceptHdl(r3)
	 */
	addis	r4, 0, globalExceptHdl@ha
	lwz	r5, globalExceptHdl@l(r4)
	mtlr	r5
	blrl
	/*
	 * Restore registers status
	 */
	lwz	r31,  EXC_CR_OFFSET(r1)
	mtcr	r31
	lwz	r30,  EXC_CTR_OFFSET(r1)
	mtctr	r30
	lwz	r29,  EXC_LR_OFFSET(r1)
	mtlr	r29
	lwz	r28,  EXC_XER_OFFSET(r1)
	mtxer	r28

	lmw	r4, GPR4_OFFSET(r1)
	lwz	r2, GPR2_OFFSET(r1)

	/*
	 * Disable data and instruction translation. Mark path non recoverable
	 */
#if defined(PPC_MSR_EXC_BITS)
	mfmsr	r3
	xori	r3, r3, PPC_MSR_EXC_BITS
	mtmsr   r3
	SYNC
#endif
#if defined(ASM_VECTORS_CRITICAL)
	/*
	 * determine, whether to restore from
	 * srr0/1 or srr2/3
	 */
	lis	r0,ASM_VECTORS_CRITICAL@h
	lwz	r3,EXCEPTION_NUMBER_OFFSET(r1)
	ori	r0,r0,ASM_VECTORS_CRITICAL@l
	rlwnm.  r0,r0,r3,0,0
	beq	1f
	/*
	 * NOTE: srr2/3 are stored in slots SRR0/1
	 * for critical exceptions
	 */ 
	lwz	r3, SRR1_FRAME_OFFSET(r1)
	mtsrr3	r3
	lwz	r3, SRR0_FRAME_OFFSET(r1)
	mtsrr2	r3
	lwz	r3, GPR3_OFFSET(r1)
	lwz	r0, GPR0_OFFSET(r1)
	/* DONT add back the frame size but reload the value
	 * stored in the frame -- maybe the exception handler
	 * changed it with good reason (e.g., gdb pushed a dummy frame)
	 */
	lwz r1, GPR1_OFFSET(r1)
	SYNC
	rfci
1:		
#endif	
	/*
	 * Restore rfi related settings
	 */

	lwz	r3, SRR1_FRAME_OFFSET(r1)
	mtsrr1	r3
	lwz	r3, SRR0_FRAME_OFFSET(r1)
	mtsrr0	r3

	lwz	r3, GPR3_OFFSET(r1)
	lwz	r0, GPR0_OFFSET(r1)
	/* DONT add back the frame size but reload the value
	 * stored in the frame -- maybe the exception handler
	 * changed it with good reason (e.g., gdb pushed a dummy frame)
	 */
	lwz r1, GPR1_OFFSET(r1)
	SYNC
	rfi
