/*
 *  This file contains the assembly code for the PowerPC 
 *  IRQ veneers for RTEMS.
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  Modified to support the MCP750.
 *  Modifications Copyright (C) 1999 Eric Valette. valette@crf.canon.fr
 *
 *
 * $Id$
 */
  	
#include <asm.h>
#include <rtems/score/cpu.h>
#include <bsp/vectors.h>
#include <libcpu/raw_exception.h>
	

#define SYNC \
	sync; \
	isync
	
	.text
	.p2align 5	
		
	PUBLIC_VAR(decrementer_exception_vector_prolog_code)
	
SYM (decrementer_exception_vector_prolog_code):
	/*
	 * let room for exception frame
	 */
	stwu    r1, - (EXCEPTION_FRAME_END)(r1)
	stw	r4, GPR4_OFFSET(r1)
	li	r4, ASM_DEC_VECTOR
	ba	shared_raw_irq_code_entry

	PUBLIC_VAR (decrementer_exception_vector_prolog_code_size)
	
	decrementer_exception_vector_prolog_code_size = . - decrementer_exception_vector_prolog_code

	PUBLIC_VAR(external_exception_vector_prolog_code)
	
SYM (external_exception_vector_prolog_code):
	/*
	 * let room for exception frame
	 */
	stwu    r1, - (EXCEPTION_FRAME_END)(r1)
	stw	r4, GPR4_OFFSET(r1)
	li	r4, ASM_EXT_VECTOR
	ba	shared_raw_irq_code_entry

	PUBLIC_VAR (external_exception_vector_prolog_code_size)
	
	external_exception_vector_prolog_code_size = . - external_exception_vector_prolog_code

	PUBLIC_VAR(shared_raw_irq_code_entry)
	PUBLIC_VAR(C_dispatch_irq_handler)
	
	.p2align 5
SYM (shared_raw_irq_code_entry):
	/*
	 * Entry conditions :
	 *	Registers already saved : R1, R4
	 *	R1  :	points to a location with enough room for the
	 *	        interrupt frame
	 *	R4  :	vector number
	 */
	/*
	 * Save SRR0/SRR1 As soon As possible as it is the minimal needed 
	 * to reenable exception processing
	 */
	stw	r0, GPR0_OFFSET(r1)
	/* PPC EABI: R2 is reserved (pointer to short data .sdata2) - we won't touch it
	 * but we still save/restore it, just in case...
	 */
	stw	r2, GPR2_OFFSET(r1)
	stw	r3, GPR3_OFFSET(r1)
	
	mfsrr0  r0
	mfsrr1  r3
	
	stw	r0, SRR0_FRAME_OFFSET(r1)
	stw	r3, SRR1_FRAME_OFFSET(r1)

	mfmsr	r3
	/*
	 * Enable data and instruction address translation, exception recovery
     *
     * also, on CPUs with FP, enable FP so that FP context can be
     * saved and restored (using FP instructions)
	 */
#if (PPC_HAS_FPU == 0)
	ori     r3, r3, MSR_RI | MSR_IR | MSR_DR
#else
	ori     r3, r3, MSR_RI | MSR_IR | MSR_DR | MSR_FP
#endif
	mtmsr   r3
	SYNC
	/*
	 * Push C scratch registers on the current stack. It may
	 * actually be the thread stack or the interrupt stack.
	 * Anyway we have to make it in order to be able to call C/C++
	 * functions. Depending on the nesting interrupt level, we will
	 * switch to the right stack later.
	 */
	stw	r5, GPR5_OFFSET(r1)
	stw	r6, GPR6_OFFSET(r1)
	stw	r7, GPR7_OFFSET(r1)
	stw	r8, GPR8_OFFSET(r1)
	stw	r9, GPR9_OFFSET(r1)
	stw	r10, GPR10_OFFSET(r1)
	stw	r11, GPR11_OFFSET(r1)
	stw	r12, GPR12_OFFSET(r1)
	stw	r13, GPR13_OFFSET(r1)

	mfcr	r5
	mfctr	r6
	mfxer	r7
	mflr	r8
	
	stw	r5,  EXC_CR_OFFSET(r1)
	stw	r6,  EXC_CTR_OFFSET(r1)
	stw	r7,  EXC_XER_OFFSET(r1)
	stw	r8,  EXC_LR_OFFSET(r1)

	/*
	 * Add some non volatile registers to store information
	 * that will be used when returning from C handler
	 */
	stw	r14, GPR14_OFFSET(r1)
	stw	r15, GPR15_OFFSET(r1)
	/*
	 * save current stack pointer location in R14
	 */
	addi	r14, r1, 0
	/*
	 * store part of _Thread_Dispatch_disable_level address in R15
	 */
	addis r15,0, _Thread_Dispatch_disable_level@ha
	/*
	 * Get current nesting level in R3
	 */
	mfspr	r3, SPRG0
	/*
	 * Check if stack switch is necessary
	 */
	cmpwi	r3,0
	bne	nested
	mfspr	r1, SPRG1
	
nested:	
	/* 
	 * Start Incrementing nesting level in R3
	 */
	addi	r3,r3,1
	/*
	 * Start Incrementing _Thread_Dispatch_disable_level R4 = _Thread_Dispatch_disable_level
	 */
	lwz	r6,_Thread_Dispatch_disable_level@l(r15)
	/*
	 * store new nesting level in SPRG0
	 */
	mtspr	SPRG0, r3
	
	addi	r6, r6, 1
	mfmsr	r5
	/*
	 * store new _Thread_Dispatch_disable_level value
	 */
	stw	r6, _Thread_Dispatch_disable_level@l(r15)
	/*
	 * We are now running on the interrupt stack. External and decrementer
	 * exceptions are still disabled. I see no purpose trying to optimize
	 * further assembler code. 
	 */
	/*
	 * Call C exception handler for decrementer Interrupt frame is passed just
	 * in case...
	 */
	addi	r3, r14, 0x8
	bl	C_dispatch_irq_handler /* C_dispatch_irq_handler(cpu_interrupt_frame* r3, vector r4) */
	/*
	 * start decrementing nesting level. Note : do not test result against 0 
	 * value as an easy exit condition because if interrupt nesting level > 1
	 * then _Thread_Dispatch_disable_level > 1
	 */
	mfspr	r4, SPRG0
	/*
	 * start decrementing _Thread_Dispatch_disable_level
	 */
	lwz	r3,_Thread_Dispatch_disable_level@l(r15)
	addi	r4, r4, -1	/* Continue decrementing nesting level */
	addi	r3, r3, -1	/* Continue decrementing _Thread_Dispatch_disable_level */
	mtspr	SPRG0, r4	/* End decrementing nesting level */
	stw	r3,_Thread_Dispatch_disable_level@l(r15) /* End decrementing _Thread_Dispatch_disable_level */
	cmpwi	r3, 0
	/*
	 * switch back to original stack (done here just optimize registers 
	 * contention. Could have been done before...)
	 */
	addi	r1, r14, 0
	bne	easy_exit /* if (_Thread_Dispatch_disable_level != 0) goto easy_exit */
	/*
	 * Here we are running again on the thread system stack.
	 * We have interrupt nesting level = _Thread_Dispatch_disable_level = 0.
	 * Interrupt are still disabled. Time to check if scheduler request to 
	 * do something with the current thread...
	 */
	addis	r4, 0, _Context_Switch_necessary@ha
	lwz	r5, _Context_Switch_necessary@l(r4)
	cmpwi	r5, 0
	bne	switch
	
	addis	r6, 0, _ISR_Signals_to_thread_executing@ha
	lwz	r7, _ISR_Signals_to_thread_executing@l(r6)
	cmpwi	r7, 0
	li	r8, 0
	beq	easy_exit
	stw	r8, _ISR_Signals_to_thread_executing@l(r6)
	/*
	 * going to call _ThreadProcessSignalsFromIrq
	 * Push a complete exception like frame...
	 */
	stmw	r16, GPR16_OFFSET(r1)
	addi	r3, r1, 0x8
	/*
	 * compute SP at exception entry
	 */
	addi    r4, r1, EXCEPTION_FRAME_END
	/*
	 * store it at the right place
	 */
	stw     r4, GPR1_OFFSET(r1)
	/*
	 * Call High Level signal handling code
	 */
	bl	_ThreadProcessSignalsFromIrq
	/*
	 * start restoring exception like frame
	 */
	lwz	r31,  EXC_CTR_OFFSET(r1)
	lwz	r30,  EXC_XER_OFFSET(r1)
	lwz	r29,  EXC_CR_OFFSET(r1)
	lwz	r28,  EXC_LR_OFFSET(r1)
	
	mtctr	r31
	mtxer	r30
	mtcr	r29
	mtlr	r28
	
	lmw	r4, GPR4_OFFSET(r1)
	lwz	r2, GPR2_OFFSET(r1)
	lwz	r0, GPR0_OFFSET(r1)

	/*
	 * Disable data and instruction translation. Make path non recoverable...
	 */
	mfmsr	r3
	xori	r3, r3, MSR_RI | MSR_IR | MSR_DR
	mtmsr   r3
	SYNC
	/*
	 * Restore rfi related settings
	 */
		 
	lwz	r3, SRR1_FRAME_OFFSET(r1)
	mtsrr1	r3
	lwz	r3, SRR0_FRAME_OFFSET(r1)
	mtsrr0	r3
	
	lwz	r3, GPR3_OFFSET(r1)
	addi 	r1,r1, EXCEPTION_FRAME_END
	SYNC
	rfi
	
switch:
	bl	SYM (_Thread_Dispatch)
	
easy_exit:	
	/*
	 * start restoring interrupt frame
	 */
	lwz	r3,  EXC_CTR_OFFSET(r1)
	lwz	r4,  EXC_XER_OFFSET(r1)
	lwz	r5,  EXC_CR_OFFSET(r1)
	lwz	r6,  EXC_LR_OFFSET(r1)
	
	mtctr	r3
	mtxer	r4
	mtcr	r5
	mtlr	r6

	lwz	r15, GPR15_OFFSET(r1)
	lwz	r14, GPR14_OFFSET(r1)
	lwz	r13, GPR13_OFFSET(r1)
	lwz	r12, GPR12_OFFSET(r1)
	lwz	r11, GPR11_OFFSET(r1)
	lwz	r10, GPR10_OFFSET(r1)
	lwz	r9, GPR9_OFFSET(r1)
	lwz	r8, GPR8_OFFSET(r1)
	lwz	r7, GPR7_OFFSET(r1)
	lwz	r6, GPR6_OFFSET(r1)
	lwz	r5, GPR5_OFFSET(r1)

	/*
	 * Disable nested exception processing, data and instruction
	 * translation.
	 */
	mfmsr	r3
	xori	r3, r3, MSR_RI | MSR_IR | MSR_DR
	mtmsr   r3
	SYNC
	/*
	 * Restore rfi related settings
	 */
		 
	lwz	r4, SRR1_FRAME_OFFSET(r1)
	lwz	r3, SRR0_FRAME_OFFSET(r1)
	lwz	r2, GPR2_OFFSET(r1)
	lwz	r0, GPR0_OFFSET(r1)

	mtsrr1	r4
	mtsrr0	r3
	lwz	r4, GPR4_OFFSET(r1)
	lwz	r3, GPR3_OFFSET(r1)
	addi 	r1,r1, EXCEPTION_FRAME_END
	SYNC
	rfi

