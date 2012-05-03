/*
 * irq_asm.S
 *
 *  This file contains the assembly code for the PowerPC
 *  IRQ veneers for RTEMS.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *
 *  MPC5xx port sponsored by Defence Research and Development Canada - Suffield
 *  Copyright (C) 2004, Real-Time Systems Inc. (querbach@realtime.bc.ca)
 *
 *  Derived from libbsp/powerpc/mbx8xx/irq/irq_asm.S:
 *
 *  Modified to support the MCP750.
 *  Modifications Copyright (C) 1999 Eric Valette. valette@crf.canon.fr
 *
 *  Till Straumann <strauman@slac.stanford.edu>, 2003/7:
 *    - store isr nesting level in _ISR_Nest_level rather than
 *      SPRG0 - RTEMS relies on that variable.
 */

#include <rtems/asm.h>
#include <rtems/score/cpu.h>
#include <rtems/score/percpu.h>
#include <libcpu/vectors.h>
#include <libcpu/raw_exception.h>


#define SYNC \
	sync; \
	isync

/*
 * Common handler for interrupt exceptions.
 *
 * The function CPU_rtems_irq_mng_init() initializes the decrementer and
 * external interrupt entries in the exception handler table with pointers
 * to this routine, which saves the remainder of the interrupted code's
 * state, then calls C_dispatch_irq_handler().
 *
 * On entry, R1 points to a new exception stack frame in which R3, R4, and
 * LR have been saved.  R4 holds the exception number.
 */
	PUBLIC_VAR(C_dispatch_irq_handler)

	PUBLIC_VAR(dispatch_irq_handler)
SYM (dispatch_irq_handler):
	/*
	 * Save SRR0/SRR1 As soon As possible as it is the minimal needed
	 * to re-enable exception processing.
	 *
	 * Note that R2 should never change (it's the EABI pointer to
	 * .sdata2), but we save it just in case.
         */
	stw	r0, GPR0_OFFSET(r1)
	stw	r2, GPR2_OFFSET(r1)

	mfsrr0  r0
	mfsrr1  r3

	stw	r0, SRR0_FRAME_OFFSET(r1)
	stw	r3, SRR1_FRAME_OFFSET(r1)

	/*
	 * Enable exception recovery.  Also enable FP so that FP context
	 * can be saved and restored (using FP instructions).
	 */
	mfmsr	r3
	ori     r3, r3, MSR_RI | MSR_FP
	mtmsr   r3
	SYNC

	/*
	 * Push C scratch registers on the current stack. It may actually be
	 * the thread stack or the interrupt stack.  Anyway we have to make
	 * it in order to be able to call C/C++ functions. Depending on the
	 * nesting interrupt level, we will switch to the right stack later.
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

	stw	r5,  EXC_CR_OFFSET(r1)
	stw	r6,  EXC_CTR_OFFSET(r1)
	stw	r7,  EXC_XER_OFFSET(r1)

	/*
	 * Add some non volatile registers to store information that will be
	 * used when returning from C handler.
	 */
	stw	r14, GPR14_OFFSET(r1)
	stw	r15, GPR15_OFFSET(r1)

	/*
	 * Save current stack pointer location in R14.
	 */
	addi	r14, r1, 0

	/*
	 * store part of _Thread_Dispatch_disable_level address in R15
	 */
	addis r15, 0, _Thread_Dispatch_disable_level@ha

	/*
	 * Retrieve current nesting level from _ISR_Nest_level
	 */
	lis		r7, ISR_NEST_LEVEL@ha
	lwz		r3, ISR_NEST_LEVEL@l(r7)

	/*
	 * Check if stack switch is necessary
	 */
	cmpwi	r3, 0
	bne	nested

	mfspr	r1, SPRG1		/* switch to interrupt stack */
nested:

	/*
	 * Start Incrementing nesting level in R3
	 */
	addi	r3, r3, 1

	/*
	 * Start Incrementing _Thread_Dispatch_disable_level R4 = _Thread_Dispatch_disable_level
	 */
	lwz	r6, _Thread_Dispatch_disable_level@l(r15)

	/* store new nesting level in _ISR_Nest_level */
	stw	r3, ISR_NEST_LEVEL@l(r7)

	addi	r6, r6, 1

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
	 * Call C exception handler for decrementer or external interrupt.
	 * Pass frame along just in case..
	 *
	 * C_dispatch_irq_handler(cpu_interrupt_frame* r3, vector r4)
	 */
	addi	r3, r14, 0x8
	bl	C_dispatch_irq_handler

	/*
	 * start decrementing nesting level. Note : do not test result against 0
	 * value as an easy exit condition because if interrupt nesting level > 1
	 * then _Thread_Dispatch_disable_level > 1
	 */
	lis		r7, ISR_NEST_LEVEL@ha
	lwz		r4, ISR_NEST_LEVEL@l(r7)

	/*
	 * start decrementing _Thread_Dispatch_disable_level
	 */
	lwz	r3,_Thread_Dispatch_disable_level@l(r15)

	addi	r4, r4, -1	/* Continue decrementing nesting level */
	addi	r3, r3, -1	/* Continue decrementing _Thread_Dispatch_disable_level */

	stw	r4, ISR_NEST_LEVEL@l(r7) /* End decrementing nesting level */
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
	addis	r4, 0, DISPATCH_NEEDED@ha
	lbz	r5, DISPATCH_NEEDED@l(r4)
	cmpwi	r5, 0
	beq	easy_exit

	/*
	 * going to call _Thread_Dispatch
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
	bl	_Thread_Dispatch

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
	 * Make path non recoverable...
	 */
	mtspr	nri, r0
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
	 * Disable nested exception processing.
	 */
	mtspr	nri, r0
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
