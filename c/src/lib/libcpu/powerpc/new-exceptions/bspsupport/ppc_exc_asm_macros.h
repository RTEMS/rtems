/*
 * (c) 1999, Eric Valette valette@crf.canon.fr
 *
 * Modified and partially rewritten by Till Straumann, 2007-2008
 *
 * Modified by Sebastian Huber <sebastian.huber@embedded-brains.de>, 2008.
 *
 * Low-level assembly code for PPC exceptions (macros).
 *
 * This file was written with the goal to eliminate
 * ALL #ifdef <cpu_flavor> conditionals -- please do not
 * reintroduce such statements.
 */

#include <bspopts.h>
#include <bsp/vectors.h>

#define LT(cr) ((cr)*4+0)
#define GT(cr) ((cr)*4+1)
#define EQ(cr) ((cr)*4+2)

/* Opcode of 'stw r1, off(r13)' */
#define STW_R1_R13(off) ((((36<<10)|(r1<<5)|(r13))<<16) | ((off)&0xffff))

#define FRAME_REGISTER r14
#define VECTOR_REGISTER r4
#define SCRATCH_REGISTER_0 r5
#define SCRATCH_REGISTER_1 r6
#define SCRATCH_REGISTER_2 r7

#define FRAME_OFFSET( r) GPR14_OFFSET( r)
#define VECTOR_OFFSET( r) GPR4_OFFSET( r)
#define SCRATCH_REGISTER_0_OFFSET( r) GPR5_OFFSET( r)
#define SCRATCH_REGISTER_1_OFFSET( r) GPR6_OFFSET( r)
#define SCRATCH_REGISTER_2_OFFSET( r) GPR7_OFFSET( r)

#define CR_TYPE 2
#define CR_MSR 3
#define CR_LOCK 4

	/*
	 * Minimal prologue snippets:
	 *
	 * Rationale: on some PPCs the vector offsets are spaced
	 * as closely as 16 bytes.
	 *
	 * If we deal with asynchronous exceptions ('interrupts')
	 * then we can use 4 instructions to
	 *   1. atomically write lock to indicate ISR is in progress
	 *      (we cannot atomically increase the Thread_Dispatch_disable_level,
	 *      see README)
	 *   2. save a register in special area
	 *   3. load register with vector info
	 *   4. branch
	 *
	 * If we deal with a synchronous exception (no stack switch
	 * nor dispatch-disabling necessary) then it's easier:
	 *   1. push stack frame
	 *   2. save register on stack
	 *   3. load register with vector info
	 *   4. branch
	 *
	 */

/*
 *****************************************************************************
 * MACRO: PPC_EXC_MIN_PROLOG_ASYNC
 *****************************************************************************
 * USES:    VECTOR_REGISTER
 * ON EXIT: Vector in VECTOR_REGISTER
 *
 * NOTES:   VECTOR_REGISTER saved in special variable
 *          'ppc_exc_vector_register_\_PRI'.
 *
 */
	.macro	PPC_EXC_MIN_PROLOG_ASYNC _NAME _VEC _PRI _FLVR

	.global	ppc_exc_min_prolog_async_\_NAME
ppc_exc_min_prolog_async_\_NAME:
	/* Atomically write lock variable in 1st instruction with non-zero
	 * value (r1 is always nonzero; r13 could also be used)
	 *
	 * NOTE: raising an exception and executing this first instruction
	 *       of the exception handler is apparently NOT atomic, i.e., a
	 *       low-priority IRQ could set the PC to this location and a
	 *       critical IRQ could intervene just at this point.
	 *
	 *       We check against this pathological case by checking the
	 *       opcode/instruction at the interrupted PC for matching
	 *
	 *         stw r1, ppc_exc_lock_XXX@sdarel(r13)
	 *
	 *       ASSUMPTION:
	 *          1) ALL 'asynchronous' exceptions (which disable thread-
	 *             dispatching) execute THIS 'magical' instruction
	 *             FIRST.
	 *          2) This instruction (including the address offset)
	 *             is not used anywhere else (probably a safe assumption).
	 */
	stw	r1, ppc_exc_lock_\_PRI@sdarel(r13)
	/*	We have no stack frame yet; store VECTOR_REGISTER in special area;
	 * a higher-priority (critical) interrupt uses a different area
	 * (hence the different prologue snippets) (\PRI)
	 */
	stw	VECTOR_REGISTER, ppc_exc_vector_register_\_PRI@sdarel(r13)
	/*	Load vector.
	 */
	li	VECTOR_REGISTER, ( \_VEC | 0xffff8000 )

	/*
	 * We store the absolute branch target address here.  It will be used
	 * to generate the branch operation in ppc_exc_make_prologue().
	 */
	.int	ppc_exc_wrap_\_FLVR

	.endm

/*
 *****************************************************************************
 * MACRO: PPC_EXC_MIN_PROLOG_SYNC
 *****************************************************************************
 * USES:    VECTOR_REGISTER
 * ON EXIT: vector in VECTOR_REGISTER
 *
 * NOTES:   exception stack frame pushed; VECTOR_REGISTER saved in frame
 *
 */
	.macro	PPC_EXC_MIN_PROLOG_SYNC _NAME _VEC _PRI _FLVR

	.global	ppc_exc_min_prolog_sync_\_NAME
ppc_exc_min_prolog_sync_\_NAME:
	stwu	r1, -EXCEPTION_FRAME_END(r1)
	stw	VECTOR_REGISTER, VECTOR_OFFSET(r1)
	li	VECTOR_REGISTER, \_VEC

	/*
	 * We store the absolute branch target address here.  It will be used
	 * to generate the branch operation in ppc_exc_make_prologue().
	 */
	.int	ppc_exc_wrap_nopush_\_FLVR

	.endm

/*
 *****************************************************************************
 * MACRO: TEST_1ST_OPCODE_crit
 *****************************************************************************
 *
 * USES:    REG, cr0
 * ON EXIT: REG available (contains *pc - STW_R1_R13(0)),
 *          return value in cr0.
 *
 * test opcode interrupted by critical (asynchronous) exception; set CR_LOCK if
 *
 *   *SRR0 == 'stw r1, ppc_exc_lock_std@sdarel(r13)'
 *
 */
	.macro	TEST_1ST_OPCODE_crit _REG

	lwz	\_REG, SRR0_FRAME_OFFSET(FRAME_REGISTER)
	lwz	\_REG, 0(\_REG)
	/*	opcode now in REG */

	/*	subtract upper 16bits of 'stw r1, 0(r13)' instruction */
	subis	\_REG, \_REG, STW_R1_R13(0)@h
	/*
	 * if what's left compares against the 'ppc_exc_lock_std@sdarel'
	 * address offset then we have a match...
	 */
	cmplwi	cr0, \_REG, ppc_exc_lock_std@sdarel

	.endm

/*
 *****************************************************************************
 * MACRO: TEST_LOCK_std
 *****************************************************************************
 *
 * USES:    CR_LOCK
 * ON EXIT: CR_LOCK is set (indicates no lower-priority locks are engaged)
 *
 */
	.macro	TEST_LOCK_std _FLVR
	/* 'std' is lowest level, i.e., can not be locked -> EQ(CR_LOCK) = 1 */
	creqv	EQ(CR_LOCK), EQ(CR_LOCK), EQ(CR_LOCK)
	.endm

/*
 ******************************************************************************
 * MACRO: TEST_LOCK_crit
 ******************************************************************************
 *
 * USES:    CR_LOCK, cr0, SCRATCH_REGISTER_0, SCRATCH_REGISTER_1
 * ON EXIT: cr0, SCRATCH_REGISTER_0, SCRATCH_REGISTER_1 available,
 *          returns result in CR_LOCK.
 *
 * critical-exception wrapper has to check 'std' lock:
 *
 * Return CR_LOCK = (   (interrupt_mask & MSR_CE) != 0
                 &&                  ppc_lock_std == 0
 *               && * SRR0 != <write std lock instruction> )
 *
 */
	.macro	TEST_LOCK_crit _FLVR
	/* If MSR_CE is not in the IRQ mask then we must never allow
	 * thread-dispatching!
	 */
	GET_INTERRUPT_MASK mask=SCRATCH_REGISTER_1
	/* EQ(cr0) = ((interrupt_mask & MSR_CE) == 0) */
	andis.	SCRATCH_REGISTER_1, SCRATCH_REGISTER_1, MSR_CE@h
	beq	TEST_LOCK_crit_done_\_FLVR

	/* STD interrupt could have been interrupted before executing the 1st
	 * instruction which sets the lock; check this case by looking at the
	 * opcode present at the interrupted PC location.
	 */
	TEST_1ST_OPCODE_crit	_REG=SCRATCH_REGISTER_0
	/*
	 * At this point cr0 is set if
	 *
	 *   *(PC) == 'stw r1, ppc_exc_lock_std@sdarel(r13)'
	 *
	 */

	/* check lock */
	lwz	SCRATCH_REGISTER_1, ppc_exc_lock_std@sdarel(r13)
	cmplwi	CR_LOCK, SCRATCH_REGISTER_1, 0

	/* set EQ(CR_LOCK) to result */
TEST_LOCK_crit_done_\_FLVR:
	/* If we end up here because the interrupt mask did not contain
     * MSR_CE then cr0 is set and therefore the value of CR_LOCK
	 * does not matter since   x && !1 == 0:
	 *
	 *  if ( (interrupt_mask & MSR_CE) == 0 ) {
	 *      EQ(CR_LOCK) = EQ(CR_LOCK) && ! ((interrupt_mask & MSR_CE) == 0)
	 *  } else {
	 *      EQ(CR_LOCK) = (ppc_exc_lock_std == 0) && ! (*pc == <write std lock instruction>)
	 *  }
	 */
	crandc	EQ(CR_LOCK), EQ(CR_LOCK), EQ(cr0)

	.endm

/*
 ******************************************************************************
 * MACRO: TEST_LOCK_mchk
 ******************************************************************************
 *
 * USES:    CR_LOCK
 * ON EXIT: CR_LOCK is cleared.
 *
 * We never want to disable machine-check exceptions to avoid a checkstop. This
 * means that we cannot use enabling/disabling this type of exception for
 * protection of critical OS data structures.  Therefore, calling OS primitives
 * from a machine-check handler is ILLEGAL. Since machine-checks can happen
 * anytime it is not legal to perform a context switch (since the exception
 * could hit a IRQ protected section of code).  We simply let this test return
 * 0 so that ppc_exc_wrapup is never called after handling a machine-check.
 */
	.macro	TEST_LOCK_mchk _SRR0 _FLVR

	crxor	EQ(CR_LOCK), EQ(CR_LOCK), EQ(CR_LOCK)

	.endm

/*
 ******************************************************************************
 * MACRO: RECOVER_CHECK_\PRI
 ******************************************************************************
 *
 * USES:    cr0, SCRATCH_REGISTER_0, SCRATCH_REGISTER_1
 * ON EXIT: cr0, SCRATCH_REGISTER_0, SCRATCH_REGISTER_1 available
 *
 * Checks if the exception is recoverable for exceptions which need such a
 * test.
 */

/* Standard*/
	.macro	RECOVER_CHECK_std _FLVR

	/* Check if exception is recoverable */
	lwz	SCRATCH_REGISTER_0, SRR1_FRAME_OFFSET(FRAME_REGISTER)
	lwz	SCRATCH_REGISTER_1, ppc_exc_msr_bits@sdarel(r13)
	xor	SCRATCH_REGISTER_1, SCRATCH_REGISTER_1, SCRATCH_REGISTER_0
	andi.	SCRATCH_REGISTER_0, SCRATCH_REGISTER_1, MSR_RI

recover_check_twiddle_std_\_FLVR:

	/* Not recoverable? */
	bne	recover_check_twiddle_std_\_FLVR

	.endm

/* Critical */
	.macro	RECOVER_CHECK_crit _FLVR

	/* Nothing to do */

	.endm

/* Machine check */
	.macro	RECOVER_CHECK_mchk _FLVR

	/* Check if exception is recoverable */
	lwz	SCRATCH_REGISTER_0, SRR1_FRAME_OFFSET(FRAME_REGISTER)
	lwz	SCRATCH_REGISTER_1, ppc_exc_msr_bits@sdarel(r13)
	xor	SCRATCH_REGISTER_1, SCRATCH_REGISTER_1, SCRATCH_REGISTER_0
	andi.	SCRATCH_REGISTER_0, SCRATCH_REGISTER_1, MSR_RI

recover_check_twiddle_mchk_\_FLVR:

	/* Not recoverable? */
	bne	recover_check_twiddle_mchk_\_FLVR

	.endm

/*
 ******************************************************************************
 * MACRO: WRAP
 ******************************************************************************
 *
 * Minimal prologue snippets jump into WRAP which calls the high level
 * exception handler.  We must have this macro	instantiated for each possible
 * flavor of exception so that we use the proper lock variable, SRR register
 * pair and RFI instruction.
 *
 * We have two types of exceptions: synchronous and asynchronous (= interrupt
 * like).  The type is encoded in the vector register (= VECTOR_REGISTER).  For
 * interrupt like exceptions the MSB in the vector register is set.  The
 * exception type is kept in the comparison register CR_TYPE.  Normal
 * exceptions (MSB is clear) use the task stack and a context switch may happen
 * at any time.  The interrupt like exceptions disable thread dispatching and
 * switch to the interrupt stack (base address is in SPRG1).
 *
 *                                      +
 *                                      |
 *                                      | Minimal prologue
 *                                      |
 *                                      +
 *                                      |
 *                                      | o Setup frame pointer
 *                                      | o Save basic registers
 *                                      | o Determine exception type:
 *                                      |   synchronous or asynchronous
 *                                      |
 *                                +-----+
 * Synchronous exceptions:        |     | Asynchronous exceptions:
 *                                |     |
 * Save non-volatile registers    |     | o Increment thread dispatch
 *                                |     |   disable level
 *                                |     | o Increment ISR nest level
 *                                |     | o Clear lock
 *                                |     | o Switch stack if necessary
 *                                |     |
 *                                +---->+
 *                                      |
 *                                      | o Save volatile registers
 *                                      | o Change MSR if necessary
 *                                      | o Call high level handler
 *                                      | o Call global handler if necessary
 *                                      | o Check if exception is recoverable
 *                                      |
 *                                +-----+
 * Synchronous exceptions:        |     | Asynchronous exceptions:
 *                                |     |
 * Restore non-volatile registers |     | o Decrement ISR nest level
 *                                |     | o Switch stack
 *                                |     | o Decrement thread dispatch
 *                                |     |   disable level
 *                                |     | o Test lock
 *                                |     | o May do a context switch
 *                                |     |
 *                                +---->+
 *                                      |
 *                                      | o Restore MSR if necessary
 *                                      | o Restore volatile registers
 *                                      | o Restore frame pointer
 *                                      | o Return
 *                                      |
 *                                      +
 */
	.macro	WRAP _FLVR _PRI _SRR0 _SRR1 _RFI

	.global ppc_exc_wrap_\_FLVR
ppc_exc_wrap_\_FLVR:

	/* Push exception frame */
	stwu	r1, -EXCEPTION_FRAME_END(r1)

	.global ppc_exc_wrap_nopush_\_FLVR
ppc_exc_wrap_nopush_\_FLVR:

	/* Save frame register */
	stw	FRAME_REGISTER, FRAME_OFFSET(r1)

wrap_no_save_frame_register_\_FLVR:

	/*
	 * We save at first only some scratch registers
	 * and the CR.  We use a non-volatile register
	 * for the exception frame pointer (= FRAME_REGISTER).
	 */

	/* Move frame address in non-volatile FRAME_REGISTER */
	mr	FRAME_REGISTER, r1

	/* Save scratch registers */
	stw	SCRATCH_REGISTER_0, SCRATCH_REGISTER_0_OFFSET(FRAME_REGISTER)
	stw	SCRATCH_REGISTER_1, SCRATCH_REGISTER_1_OFFSET(FRAME_REGISTER)
	stw	SCRATCH_REGISTER_2, SCRATCH_REGISTER_2_OFFSET(FRAME_REGISTER)

	/* Save CR */
	mfcr	SCRATCH_REGISTER_0
	stw	SCRATCH_REGISTER_0, EXC_CR_OFFSET(FRAME_REGISTER)

	/* Check exception type and remember it in non-volatile CR_TYPE */
	cmpwi	CR_TYPE, VECTOR_REGISTER, 0

	/*
	 * Depending on the exception type we do now save the non-volatile
	 * registers or disable thread dispatching and switch to the ISR stack.
	 */

	/* Branch for synchronous exceptions */
	bge	CR_TYPE, wrap_save_non_volatile_regs_\_FLVR

	/*
	 * Increment the thread dispatch disable level in case a higher
	 * priority exception occurs we don't want it to run the scheduler.  It
	 * is safe to increment this without disabling higher priority
	 * exceptions since those will see that we wrote the lock anyways.
	 */

	/* Increment ISR nest level and thread dispatch disable level */
	lis	SCRATCH_REGISTER_2, ISR_NEST_LEVEL@ha
	lwz	SCRATCH_REGISTER_0, ISR_NEST_LEVEL@l(SCRATCH_REGISTER_2)
	lwz	SCRATCH_REGISTER_1, _Thread_Dispatch_disable_level@sdarel(r13)
	addi	SCRATCH_REGISTER_0, SCRATCH_REGISTER_0, 1
	addi	SCRATCH_REGISTER_1, SCRATCH_REGISTER_1, 1
	stw	SCRATCH_REGISTER_0, ISR_NEST_LEVEL@l(SCRATCH_REGISTER_2)
	stw	SCRATCH_REGISTER_1, _Thread_Dispatch_disable_level@sdarel(r13)

	/*
	 * No higher-priority exception occurring after this point
	 * can cause a context switch.
	 */

	/* Clear lock */
	li	SCRATCH_REGISTER_0, 0
	stw	SCRATCH_REGISTER_0, ppc_exc_lock_\_PRI@sdarel(r13)

	/* Switch stack if necessary */
	mfspr	SCRATCH_REGISTER_0, SPRG1
	cmpw	SCRATCH_REGISTER_0, r1
	blt	wrap_stack_switch_\_FLVR
	mfspr	SCRATCH_REGISTER_1, SPRG2
	cmpw	SCRATCH_REGISTER_1, r1
	blt	wrap_stack_switch_done_\_FLVR

wrap_stack_switch_\_FLVR:

	mr	r1, SCRATCH_REGISTER_0

wrap_stack_switch_done_\_FLVR:

	/*
	 * Load the pristine VECTOR_REGISTER from a special location for
	 * asynchronous exceptions.  The synchronous exceptions save the
	 * VECTOR_REGISTER in their minimal prologue.
	 */
	lwz	SCRATCH_REGISTER_2, ppc_exc_vector_register_\_PRI@sdarel(r13)

	/* Save pristine vector register */
	stw	SCRATCH_REGISTER_2, VECTOR_OFFSET(FRAME_REGISTER)

wrap_disable_thread_dispatching_done_\_FLVR:

	/*
	 * We now have SCRATCH_REGISTER_0, SCRATCH_REGISTER_1,
	 * SCRATCH_REGISTER_2 and CR available.  VECTOR_REGISTER still holds
	 * the vector (and exception type).  FRAME_REGISTER is a pointer to the
	 * exception frame (always on the stack of the interrupted context).
	 * r1 is the stack pointer, either on the task stack or on the ISR
	 * stack.  CR_TYPE holds the exception type.
	 */

	/* Save SRR0 */
	mfspr	SCRATCH_REGISTER_0, \_SRR0
	stw	SCRATCH_REGISTER_0, SRR0_FRAME_OFFSET(FRAME_REGISTER)

	/* Save SRR1 */
	mfspr	SCRATCH_REGISTER_0, \_SRR1
	stw	SCRATCH_REGISTER_0, SRR1_FRAME_OFFSET(FRAME_REGISTER)

	/* Save CTR */
	mfctr	SCRATCH_REGISTER_0
	stw	SCRATCH_REGISTER_0, EXC_CTR_OFFSET(FRAME_REGISTER)

	/* Save XER */
	mfxer	SCRATCH_REGISTER_0
	stw	SCRATCH_REGISTER_0, EXC_XER_OFFSET(FRAME_REGISTER)

	/* Save LR */
	mflr	SCRATCH_REGISTER_0
	stw	SCRATCH_REGISTER_0, EXC_LR_OFFSET(FRAME_REGISTER)

	/* Save volatile registers */
	stw	r0, GPR0_OFFSET(FRAME_REGISTER)
	stw	r3, GPR3_OFFSET(FRAME_REGISTER)
	stw	r8, GPR8_OFFSET(FRAME_REGISTER)
	stw	r9, GPR9_OFFSET(FRAME_REGISTER)
	stw	r10, GPR10_OFFSET(FRAME_REGISTER)
	stw	r11, GPR11_OFFSET(FRAME_REGISTER)
	stw	r12, GPR12_OFFSET(FRAME_REGISTER)

	/* Save read-only small data area anchor (EABI) */
	stw	r2, GPR2_OFFSET(FRAME_REGISTER)

	/* Save vector number and exception type */
	stw	VECTOR_REGISTER, EXCEPTION_NUMBER_OFFSET(FRAME_REGISTER)

	/* Load MSR bit mask */
	lwz	SCRATCH_REGISTER_0, ppc_exc_msr_bits@sdarel(r13)

	/*
	 * Change the MSR if necessary (MMU, RI),
	 * remember decision in non-volatile CR_MSR
	 */
	cmpwi	CR_MSR, SCRATCH_REGISTER_0, 0
	bne	CR_MSR, wrap_change_msr_\_FLVR

wrap_change_msr_done_\_FLVR:

#ifdef __ALTIVEC__
	LA  SCRATCH_REGISTER_0, _CPU_save_altivec_volatile
	mtctr SCRATCH_REGISTER_0
	addi r3, FRAME_REGISTER, EXC_VEC_OFFSET
	bctrl
	/*
	 * Establish defaults for vrsave and vscr
	 */
	li       SCRATCH_REGISTER_0, 0
	mtvrsave SCRATCH_REGISTER_0
	/*
	 * Use java/c9x mode; clear saturation bit 
	 */
	vxor     0, 0, 0
	mtvscr   0
	/*
	 * Reload VECTOR_REGISTER
	 */
	lwz	     VECTOR_REGISTER, EXCEPTION_NUMBER_OFFSET(FRAME_REGISTER)
#endif

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
	addi	r3, FRAME_REGISTER, FRAME_LINK_SPACE

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

	/* Check return value and call global handler if necessary */
	cmpwi   r3, 0
	bne	wrap_call_global_handler_\_FLVR

wrap_handler_done_\_FLVR:

	/* Check if exception is recoverable */
	RECOVER_CHECK_\_PRI	_FLVR=\_FLVR

	/*
	 * Depending on the exception type we do now restore the non-volatile
	 * registers or enable thread dispatching and switch back from the ISR
	 * stack.
	 */

	/* Branch for synchronous exceptions */
	bge	CR_TYPE, wrap_restore_non_volatile_regs_\_FLVR

	/*
	 * Switch back to original stack (FRAME_REGISTER == r1 if we are still
	 * on the IRQ stack).
	 */
	mr	r1, FRAME_REGISTER

	/*
	 * Check thread dispatch disable level AND lower priority locks (in
	 * CR_LOCK): ONLY if the thread dispatch disable level == 0 AND no lock
	 * is set then call ppc_exc_wrapup() which may do a context switch.  We
	 * can skip TEST_LOCK, because it has no side effects.
	 */

	/* Decrement ISR nest level and thread dispatch disable level */
	lis	SCRATCH_REGISTER_2, ISR_NEST_LEVEL@ha
	lwz	SCRATCH_REGISTER_0, ISR_NEST_LEVEL@l(SCRATCH_REGISTER_2)
	lwz	SCRATCH_REGISTER_1, _Thread_Dispatch_disable_level@sdarel(r13)
	subi	SCRATCH_REGISTER_0, SCRATCH_REGISTER_0, 1
	subic.	SCRATCH_REGISTER_1, SCRATCH_REGISTER_1, 1
	stw	SCRATCH_REGISTER_0, ISR_NEST_LEVEL@l(SCRATCH_REGISTER_2)
	stw	SCRATCH_REGISTER_1, _Thread_Dispatch_disable_level@sdarel(r13)

	/* Branch to skip thread dispatching */
	bne	wrap_thread_dispatching_done_\_FLVR

	/* Test lower-priority locks (result in non-volatile CR_LOCK) */
	TEST_LOCK_\_PRI _FLVR=\_FLVR

	/* Branch to skip thread dispatching */
	bne	CR_LOCK, wrap_thread_dispatching_done_\_FLVR

	/* Load address of ppc_exc_wrapup() */
	LA	SCRATCH_REGISTER_0, ppc_exc_wrapup

	/* First parameter = exception frame pointer + FRAME_LINK_SPACE */
	addi	r3, FRAME_REGISTER, FRAME_LINK_SPACE

	/* Call ppc_exc_wrapup() */
	mtctr	SCRATCH_REGISTER_0
	bctrl

wrap_thread_dispatching_done_\_FLVR:

#ifdef __ALTIVEC__
	LA  SCRATCH_REGISTER_0, _CPU_load_altivec_volatile
	mtctr   SCRATCH_REGISTER_0  	
	addi    r3, FRAME_REGISTER, EXC_VEC_OFFSET	
	bctrl
#endif

	/* Restore MSR? */
	bne	CR_MSR, wrap_restore_msr_\_FLVR

wrap_restore_msr_done_\_FLVR:

	/*
	 * At this point r1 is a valid exception frame pointer and
	 * FRAME_REGISTER is no longer needed.
	 */

	/* Restore frame register */
	lwz	FRAME_REGISTER, FRAME_OFFSET(r1)

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

	/* Restore read-only small data area anchor (EABI) */
	lwz	r2, GPR2_OFFSET(r1)

	/* Restore vector register */
	lwz	VECTOR_REGISTER, VECTOR_OFFSET(r1)

	/*
	 * Disable all asynchronous exceptions which can do a thread dispatch.
	 * See README.
	 */
	INTERRUPT_DISABLE	SCRATCH_REGISTER_1, SCRATCH_REGISTER_0

	/* Restore scratch registers and SRRs */
	lwz	SCRATCH_REGISTER_0, SRR0_FRAME_OFFSET(r1)
	lwz	SCRATCH_REGISTER_1, SRR1_FRAME_OFFSET(r1)
	lwz	SCRATCH_REGISTER_2, SCRATCH_REGISTER_2_OFFSET(r1)
	mtspr	\_SRR0, SCRATCH_REGISTER_0
	lwz	SCRATCH_REGISTER_0, SCRATCH_REGISTER_0_OFFSET(r1)
	mtspr	\_SRR1, SCRATCH_REGISTER_1
	lwz	SCRATCH_REGISTER_1, SCRATCH_REGISTER_1_OFFSET(r1)

	/*
	 * We restore r1 from the frame rather than just popping (adding to
	 * current r1) since the exception handler might have done strange
	 * things (e.g. a debugger moving and relocating the stack).
	 */
	lwz	r1, 0(r1)

	/* Return */
	\_RFI

wrap_change_msr_\_FLVR:

	mfmsr	SCRATCH_REGISTER_1
	or	SCRATCH_REGISTER_1, SCRATCH_REGISTER_1, SCRATCH_REGISTER_0
	mtmsr	SCRATCH_REGISTER_1
	msync
	isync
	b	wrap_change_msr_done_\_FLVR

wrap_restore_msr_\_FLVR:

	lwz	SCRATCH_REGISTER_0, ppc_exc_msr_bits@sdarel(r13)
	mfmsr	SCRATCH_REGISTER_1
	andc	SCRATCH_REGISTER_1, SCRATCH_REGISTER_1, SCRATCH_REGISTER_0
	mtmsr	SCRATCH_REGISTER_1
	msync
	isync
	b	wrap_restore_msr_done_\_FLVR

wrap_save_non_volatile_regs_\_FLVR:

	/* Load pristine stack pointer */
	lwz	SCRATCH_REGISTER_1, 0(FRAME_REGISTER)

	/* Save small data area anchor (SYSV) */
	stw	r13, GPR13_OFFSET(FRAME_REGISTER)

	/* Save pristine stack pointer */
	stw	SCRATCH_REGISTER_1, GPR1_OFFSET(FRAME_REGISTER)

	/* r14 is the FRAME_REGISTER and will be saved elsewhere */

	/* Save non-volatile registers r15 .. r31 */
#ifndef __SPE__
	stmw	r15, GPR15_OFFSET(FRAME_REGISTER)
#else
	stw	r15, GPR15_OFFSET(FRAME_REGISTER)
	stw	r16, GPR16_OFFSET(FRAME_REGISTER)
	stw	r17, GPR17_OFFSET(FRAME_REGISTER)
	stw	r18, GPR18_OFFSET(FRAME_REGISTER)
	stw	r19, GPR19_OFFSET(FRAME_REGISTER)
	stw	r20, GPR20_OFFSET(FRAME_REGISTER)
	stw	r21, GPR21_OFFSET(FRAME_REGISTER)
	stw	r22, GPR22_OFFSET(FRAME_REGISTER)
	stw	r23, GPR23_OFFSET(FRAME_REGISTER)
	stw	r24, GPR24_OFFSET(FRAME_REGISTER)
	stw	r25, GPR25_OFFSET(FRAME_REGISTER)
	stw	r26, GPR26_OFFSET(FRAME_REGISTER)
	stw	r27, GPR27_OFFSET(FRAME_REGISTER)
	stw	r28, GPR28_OFFSET(FRAME_REGISTER)
	stw	r29, GPR29_OFFSET(FRAME_REGISTER)
	stw	r30, GPR30_OFFSET(FRAME_REGISTER)
	stw	r31, GPR31_OFFSET(FRAME_REGISTER)
#endif

	b	wrap_disable_thread_dispatching_done_\_FLVR

wrap_restore_non_volatile_regs_\_FLVR:

	/* Load stack pointer */
	lwz	SCRATCH_REGISTER_0, GPR1_OFFSET(r1)

	/* Restore small data area anchor (SYSV) */
	lwz	r13, GPR13_OFFSET(r1)

	/* r14 is the FRAME_REGISTER and will be restored elsewhere */

	/* Restore non-volatile registers r15 .. r31 */
#ifndef __SPE__
	lmw	r15, GPR15_OFFSET(r1)
#else
	lwz	r15, GPR15_OFFSET(FRAME_REGISTER)
	lwz	r16, GPR16_OFFSET(FRAME_REGISTER)
	lwz	r17, GPR17_OFFSET(FRAME_REGISTER)
	lwz	r18, GPR18_OFFSET(FRAME_REGISTER)
	lwz	r19, GPR19_OFFSET(FRAME_REGISTER)
	lwz	r20, GPR20_OFFSET(FRAME_REGISTER)
	lwz	r21, GPR21_OFFSET(FRAME_REGISTER)
	lwz	r22, GPR22_OFFSET(FRAME_REGISTER)
	lwz	r23, GPR23_OFFSET(FRAME_REGISTER)
	lwz	r24, GPR24_OFFSET(FRAME_REGISTER)
	lwz	r25, GPR25_OFFSET(FRAME_REGISTER)
	lwz	r26, GPR26_OFFSET(FRAME_REGISTER)
	lwz	r27, GPR27_OFFSET(FRAME_REGISTER)
	lwz	r28, GPR28_OFFSET(FRAME_REGISTER)
	lwz	r29, GPR29_OFFSET(FRAME_REGISTER)
	lwz	r30, GPR30_OFFSET(FRAME_REGISTER)
	lwz	r31, GPR31_OFFSET(FRAME_REGISTER)
#endif

	/* Restore stack pointer */
	stw	SCRATCH_REGISTER_0, 0(r1)

	b	wrap_thread_dispatching_done_\_FLVR

wrap_call_global_handler_\_FLVR:

	/* First parameter = exception frame pointer + FRAME_LINK_SPACE */
	addi	r3, FRAME_REGISTER, FRAME_LINK_SPACE

	/* Load global handler address */
	LW	SCRATCH_REGISTER_0, globalExceptHdl

	/* Check address */
	cmpwi	SCRATCH_REGISTER_0, 0
	beq	wrap_handler_done_\_FLVR

	/* Call global handler */
	mtctr	SCRATCH_REGISTER_0
	bctrl

	b	wrap_handler_done_\_FLVR

	.endm
