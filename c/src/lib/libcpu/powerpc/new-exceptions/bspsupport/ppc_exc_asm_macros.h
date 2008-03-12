/*
 * (c) 1999, Eric Valette valette@crf.canon.fr
 *
 * Modified and partially rewritten by Till Straumann, 2007
 *
 * Low-level assembly code for PPC exceptions (macros).
 *
 * This file was written with the goal to eliminate
 * ALL #ifdef <cpu_flavor> conditionals -- please do not
 * reintroduce such statements.
 */

#include <rtems/score/cpu.h>
#include <bsp/vectors.h>
#include <libcpu/raw_exception.h>

#define EXC_MIN_GPR1		0
#define FRAME_LINK_SPACE	8

#define r0	0
#define r1	1
#define r2	2
#define r3	3
#define r4	4
#define r5	5
#define r6	6
#define r7	7
#define r8	8
#define r9	9
#define r10	10
#define r11	11
#define r12	12
#define r13	13
#define r14	14
#define r15	15
#define r16	16
#define r17	17
#define r18	18
#define r19	19
#define r20	20
#define r21	21
#define r22	22
#define r23	23
#define r24	24
#define r25	25
#define r26	26
#define r27	27
#define r28	28
#define r29	29
#define r30	30
#define r31	31

#define cr0 0
#define cr1 1
#define cr4 4

#define LT(cr)	((cr)*4+0)
#define GT(cr)	((cr)*4+1)
#define EQ(cr)	((cr)*4+2)

#define	NOFRAME	0xffff8000

/* Switch r1 to interrupt stack if not already there.
 *
 * USES:    RA, RB
 * ON EXIT: RA, RB available, r1 points into interrupt
 *          stack.
 *
 * NOTES:
 *   - NEVER store stuff in a frame before
 *     reserving it (stwu r1) - otherwise
 *     higher-priority exception may overwrite.
 *   - algorithm should allow nesting of higher
 *     priority exceptions (HPE) (by disabling
 *     them while the stack is switched).
 */
#if 0
	.macro	SWITCH_STACK RA RB FLVR
	mfspr	\RB, SPRG1
	cmplw	cr0, r1, \RB
	bgt		do_r1_reload_\FLVR
	lwz     \RA, ppc_exc_intr_stack_size@sdarel(r13)
	subf	\RB, \RB, \RA
	cmplw	cr0, r1, \RB
	bge		no_r1_reload_\FLVR
do_r1_reload_\FLVR:
	mfspr	r1, SPRG1
no_r1_reload_\FLVR:
	.endm
#else
	.macro	SWITCH_STACK RA RB FLVR
	/* disable interrupts */
	lwz		\RA, ppc_exc_msr_irq_mask@sdarel(r13)
	mfmsr	\RB
	andc	\RA, \RB, \RA
	mtmsr	\RA
	/* increment nest level */
	lwz		\RA, _ISR_Nest_level@sdarel(r13)
	cmplwi	cr0, \RA, 0
	bne	no_r1_reload_\FLVR
	/* reload r1            */
	mfspr	r1, SPRG1
no_r1_reload_\FLVR:
	addi	\RA, \RA, 1
	stw		\RA, _ISR_Nest_level@sdarel(r13)
	/* restore IRQ mask     */
	mtmsr	\RB
	.endm
#endif

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
	.macro	PPC_EXC_MIN_PROLOG_ASYNC _NAME _VEC _PRI _FLVR
	.global	ppc_exc_min_prolog_async_\_NAME
ppc_exc_min_prolog_async_\_NAME:
	/* Atomically write lock variable in 1st instruction with non-zero value
	 * (r1 is always nonzero; r13 could also be used)
	 */
	stw		r1, ppc_exc_lock_\_PRI@sdarel(r13)
	/* We have no stack frame yet; store r3 in special area;
	 * a higher-priority (critical) interrupt uses a different area
	 * (hence the different prologue snippets) (\PRI) 
	 */
	stw		r3, ppc_exc_gpr3_\_PRI@sdarel(r13)
	/* Load vector.
	 */
	li		r3, ( \_VEC | 0xffff8000 )
	/* Branch (must be within 32MB)
	 */
	ba 		wrap_\_FLVR
	.endm

	.macro PPC_EXC_MIN_PROLOG_SYNC _NAME _VEC _PRI _FLVR
	.global ppc_exc_min_prolog_sync_\_NAME
ppc_exc_min_prolog_sync_\_NAME:
	stwu	r1, -EXCEPTION_FRAME_END(r1)
	stw		r3, GPR3_OFFSET(r1)
	li		r3, \_VEC
	ba		wrap_nopush_\_FLVR
	.endm
		
	.macro TEST_LOCK_std
	/* 'std' is lowest level, i.e., can not be locked -> EQ(cr4) = 1 */
	creqv EQ(cr4), EQ(cr4), EQ(cr4)
	.endm

	/* critical-exception wrapper has to check 'std' lock: */
	.macro TEST_LOCK_crit
	lwz r5, ppc_exc_lock_std@sdarel(r13)
	cmpli	cr4, r5, 0
	.endm

	/* machine-check wrapper has to check 'std' and 'crit' locks */
	.macro TEST_LOCK_mchk
	lwz r5, ppc_exc_lock_std@sdarel(r13)
	cmpli	cr4, r5, 0
	lwz r5, ppc_exc_lock_crit@sdarel(r13)
	cmpli	cr0, r5, 0
	crand	EQ(cr4), EQ(cr4), EQ(cr0)
	.endm

	/* Minimal prologue snippets jump into WRAP
	 * which prepares calling code common to all
	 * flavors of exceptions.
	 * We must have this macro instantiated for
	 * each possible flavor of exception so that
	 * we use the proper lock variable, SRR register pair and
	 * RFI instruction.
	 */
	.macro	WRAP _FLVR _PRI _SRR0 _SRR1 _RFI
wrap_\_FLVR:
	stwu    r1,  -EXCEPTION_FRAME_END(r1)
wrap_nopush_\_FLVR:
	stw		r14, GPR14_OFFSET(r1)
wrap_no_save_r14_\_FLVR:

	/* Save r4 r5 and CR; we want CR soon */
	mfcr	r14
	stw		r4,  GPR4_OFFSET(r1)
	stw		r5,  GPR5_OFFSET(r1)
	stw		r14, EXC_CR_OFFSET(r1)

	/* Check if this is an 'interrupt-type' exception
	 * (MSB vector is set).
	 * 'interrupt-type' exceptions disable thread dispatching
	 * and switch to a private stack.
	 * The type of exception is kept in (non-volatile) cr2
	 *  < 0 -> interrupt-type
	 *  > 0 -> 'normal' exception; always on task stack,
	 *         may switch context at any time.
	 */
	cmpwi	cr2, r3, 0

	/* 
	 * Save frame address in r14
	 */
	mr      r14, r1

	bge     cr2, no_thread_dispatch_disable_\_FLVR

	/* first thing we need to
	 * increment the thread-dispatch disable level
	 * in case a higher priority exception occurs
	 * we don't want it to run the scheduler.
	 */
	lwz		r5,  _Thread_Dispatch_disable_level@sdarel(r13)
	addi	r5,  r5, 1
	stw		r5,  _Thread_Dispatch_disable_level@sdarel(r13)

	/* clear lock; no higher-priority interrupt occurring after
	 * this point can cause a context switch.
	 */
	li		r5,  0
	stw		r5,  ppc_exc_lock_\_PRI@sdarel(r13)

	/* test lower-priority locks; result in (non-volatile) cr4 */
	TEST_LOCK_\_PRI

	/* Peform stack switch if necessary */
	SWITCH_STACK RA=r4 RB=r5 FLVR=\_FLVR

	/* save r3, in exception frame */
	lwz		r5,  ppc_exc_gpr3_\_PRI@sdarel(r13)
	stw		r5,  GPR3_OFFSET(r14)

no_thread_dispatch_disable_\_FLVR:

	/* save lr into exception frame */
	mflr	r4
	stw     r4,  EXC_LR_OFFSET(r14)

	/* we now have r4,r5,lr,cr available;
	 * r3  still holds the vector,
	 * r14 a pointer to the exception frame (always on
	 *     task stack)
	 * r1  is the stack pointer, either on the task stack
	 *     or on the IRQ stack
	 */

	/* retrieve SRR0/SRR1 */
	mf\_SRR0	r4
	mf\_SRR1	r5

	/* branch to common routine */
	bl		wrap_common

	/* restore SRR, r4, r5, r1 (stack pointer) and lr */
	mt\_SRR0	r4
	mt\_SRR1	r5
	/* restore lr */
	lwz		r5,  EXC_LR_OFFSET(r1)
	lwz		r4,  GPR4_OFFSET(r1)
	mtlr	r5
	lwz		r5,  GPR5_OFFSET(r1)
	lwz		r1,  EXC_MIN_GPR1(r1)
	\_RFI
	.endm
