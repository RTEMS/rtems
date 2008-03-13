/*
 * (c) 1999, Eric Valette valette@crf.canon.fr
 *
 * Modified and partially rewritten by Till Straumann, 2007
 *
 * Low-level assembly code for PPC exceptions.
 *
 * This file was written with the goal to eliminate
 * ALL #ifdef <cpu_flavor> conditionals -- please do not
 * reintroduce such statements.
 */

/* Load macro definitions */
#include "ppc_exc_asm_macros.h"

/******************************************************/
/*  PROLOGUES                                         */
/******************************************************/

	/*
	 * Expand prologue snippets for classic, ppc405-critical, bookE-critical
	 * and E500 machine-check, synchronous and asynchronous exceptions
	 */
	PPC_EXC_MIN_PROLOG_SYNC  _NAME=tmpl_std        _VEC=0 _PRI=std  _FLVR=std
	PPC_EXC_MIN_PROLOG_SYNC  _NAME=tmpl_p405_crit  _VEC=0 _PRI=crit _FLVR=p405_crit
	PPC_EXC_MIN_PROLOG_SYNC  _NAME=tmpl_bookE_crit _VEC=0 _PRI=crit _FLVR=bookE_crit
	PPC_EXC_MIN_PROLOG_SYNC  _NAME=tmpl_e500_mchk  _VEC=0 _PRI=mchk _FLVR=e500_mchk

	PPC_EXC_MIN_PROLOG_ASYNC _NAME=tmpl_std        _VEC=0 _PRI=std  _FLVR=std
	PPC_EXC_MIN_PROLOG_ASYNC _NAME=tmpl_p405_crit  _VEC=0 _PRI=crit _FLVR=p405_crit
	PPC_EXC_MIN_PROLOG_ASYNC _NAME=tmpl_bookE_crit _VEC=0 _PRI=crit _FLVR=bookE_crit
	PPC_EXC_MIN_PROLOG_ASYNC _NAME=tmpl_e500_mchk  _VEC=0 _PRI=mchk _FLVR=e500_mchk

	.global ppc_exc_min_prolog_size
ppc_exc_min_prolog_size      = 4 * 4

    /* Special prologue for 603e-style CPUs.
	 *
	 * 603e shadows GPR0..GPR3 for certain exceptions. We must switch
     * that off before we can use the stack pointer. Note that this is
     * ONLY safe if the shadowing is actually active -- otherwise, r1
     * is destroyed. We deliberately use r1 so problems become obvious
     * if this is misused!
     */
	.global ppc_exc_tgpr_clr_prolog
ppc_exc_tgpr_clr_prolog:
    mfmsr   r1
    rlwinm  r1,r1,0,15,13
    mtmsr   r1
    isync
	/* FALL THRU TO 'auto' PROLOG */

/* Determine vector dynamically/automatically
 *
 * BUT: - only standard exceptions (no critical ones)
 *      - vector offset must be on 256 Byte boundary.
 */
	.global ppc_exc_min_prolog_auto
ppc_exc_min_prolog_auto:
	stwu	r1, -EXCEPTION_FRAME_END(r1)
	stw		r3, GPR3_OFFSET(r1)
	mflr    r3
	bla  wrap_auto

	.global ppc_exc_tgpr_clr_prolog_size
ppc_exc_tgpr_clr_prolog_size = . - ppc_exc_tgpr_clr_prolog

/*
 * Automatic vector, asynchronous exception; however,
 * automatic vector calculation is less efficient than
 * using an explicit vector in a minimal prolog snippet.
 * The latter method is preferable since there usually
 * are few asynchronous exceptions.
 *
 * For generic exceptions (which are the bulk) using
 * the 'auto' prologue is OK since performance is not
 * really an issue.
 */
	.global ppc_exc_min_prolog_auto_async
ppc_exc_min_prolog_auto_async:
	stw		r1, ppc_exc_lock_std@sdarel(r13)
	stw		r3, ppc_exc_gpr3_std@sdarel(r13)
	mflr    r3
	bla  wrap_auto_async

/******************************************************/
/*  WRAPPERS                                          */
/******************************************************/

	/* Tag start and end of the wrappers.
	 * If exceptions are installed farther removed
	 * from the text area than 32M then the wrappers
	 * must be moved to an area that is reachable
	 * from where the prologues reside. Branches into
	 * C-code are far.
	 */

	.global	__ppc_exc_wrappers_start
__ppc_exc_wrappers_start = .

	/* Expand wrappers for different exception flavors */

	/* Standard/classic powerpc */
	WRAP _FLVR=std _PRI=std _SRR0=srr0 _SRR1=srr1 _RFI=rfi

	/* ppc405 has a critical exception using srr2/srr3 */
	WRAP _FLVR=p405_crit _PRI=crit _SRR0=srr2 _SRR1=srr3 _RFI=rfci

	/* bookE has critical exception using csrr0 cssr1 */
	WRAP _FLVR=bookE_crit _PRI=crit _SRR0=csrr0 _SRR1=csrr1 _RFI=rfci

	/* e500 has machine-check exception using mcsrr0 mcssr1 */
	WRAP _FLVR=e500_mchk _PRI=mchk _SRR0=mcsrr0 _SRR1=mcsrr1 _RFI=rfmci


	/* LR holds vector, r3 holds orig. LR */
wrap_auto:
	stw		r14, GPR14_OFFSET(r1)
	/* find address where we jumped from */
	mflr	r14
	/* restore LR     */
	mtlr    r3
	/* compute vector into R3 */
	rlwinm  r3, r14, 24, 26, 31
	/* we're now in almost the same state as if called by
	 * min_prolog_std but we must skip saving r14
	 * since that's done already
	 */
	b       wrap_no_save_r14_std

wrap_auto_async:
	stwu	r1, -EXCEPTION_FRAME_END(r1)
	stw		r14, GPR14_OFFSET(r1)
	/* find address where we jumped from */
	mflr	r14
	/* restore LR     */
	mtlr    r3
	/* set upper bits to indicate that non-volatile
	 * registers should not be saved/restored.
	 */
	li      r3, 0xffff8000
	/* compute vector into R3 */
	rlwimi  r3, r14, 24, 26, 31
	/* we're now in almost the same state as if called by
	 * min_prolog_std but we must skip saving r14
	 * since that's done already
	 */
	b       wrap_no_save_r14_std

/*
 * Common code for all flavors of exception and whether
 * they are synchronous or asynchronous.
 *
 * Call with
 *  r3 : vector
 *  r4 : srr0
 *  r5 : srr1
 *  r14: exception frame
 *  cr4: OR of lower-priority locks
 *  cr2: exception type (asyn/isr [<0] or synchronous [>=0])
 *  lr : is updated by 'bl'
 * all others: original state
 *
 * If this is an asynchronous exception ( cr2 < 0 ):
 *   - save volatile registers only, 
 *   - disable thread dispatching,
 *   - switch to interrupt stack (if necessary),
 *   - call the C-dispatcher,
 *   - switch back the stack,
 *   - decrement the dispatch-disable level 
 *   - check if it is safe to dispatch (disable-level must be 0
 *     AND no lower-priority asynchronous exception must be under
 *     way (as indicated by the lock variables).
 *   - If it would be OK to dispatch, call the C-wrapup code.
 *   - restore volatile registers
 *
 * Otherwise, i.e., if we are dealing with a synchronous exception
 * then:
 *   - save all registers
 *   - call the C-dispatcher
 *   - restore registers
 */

wrap_common:
	stw		r4, SRR0_FRAME_OFFSET(r14)
	stw		r5, SRR1_FRAME_OFFSET(r14)

	/* prepare for calling C code; */

	/* use non-volatile r15 for remembering lr */
	stw		r15, GPR15_OFFSET(r14)

	/* save vector; negative if only scratch regs. are valid */
	stw		r3,  EXCEPTION_NUMBER_OFFSET(r14)

	/* save scratch registers */

	/* r2 should be unused or fixed anyways (eabi sdata2) */
	stw		r0,  GPR0_OFFSET(r14)
	stw		r2,  GPR2_OFFSET(r14)
	stw		r6,  GPR6_OFFSET(r14)
	stw		r7,  GPR7_OFFSET(r14)
	stw		r8,  GPR8_OFFSET(r14)
	stw		r9,  GPR9_OFFSET(r14)
	stw		r10, GPR10_OFFSET(r14)
	stw		r11, GPR11_OFFSET(r14)
	stw		r12, GPR12_OFFSET(r14)
	/* r13 must be fixed anyways (sysv sdata) */

	/* save LR */
	mflr    r15

	mfctr	r4
	mfxer   r5
	stw		r4,  EXC_CTR_OFFSET(r14)
	stw		r5,  EXC_XER_OFFSET(r14)

	/*
	 * Switch MMU / RI on if necessary;
	 * remember decision in cr3
	 */
	lwz		r4,  ppc_exc_msr_bits@sdarel(r13)
	cmpwi	cr3, r4, 0
	beq		cr3, 1f
	mfmsr	r5
	or		r5, r5, r4
	mtmsr	r5
	sync
	isync
1:

	/* If this is a asynchronous exception we skip ahead */
	blt		cr2, skip_save_nonvolatile_regs

	/* YES; they want everything ('normal exception') */

	/* save original stack pointer */
	lwz		r5,  EXC_MIN_GPR1(r14)
	stw		r5,  GPR1_OFFSET(r14)

	stw		r13, GPR13_OFFSET(r14)

	/* store r16..r31 into the exception frame */
	stmw	r16, GPR16_OFFSET(r14)

skip_save_nonvolatile_regs:
	/* store address of exception frame in r4; vector is in r3 */
	addi  r4, r14, FRAME_LINK_SPACE 

	/* load hi-halfword of C wrapper address */
	lis		r5, ppc_exc_C_wrapper@h
	/* clear CR[6] to make sure no vararg callee assumes that
	 * there are any valid FP regs
	 */
	crxor 6,6,6
	/* merge lo-halfword of C wrapper address */
	ori		r5, r5, ppc_exc_C_wrapper@l
	/* Far branch to ppc_C_wrapper */
	mtlr	r5
	blrl

	/* do not clobber r3 since we pass the return value
	 * of ppc_exc_C_wrapper on to ppc_exc_wrapup
	 */

	/* skip decrementing the thread-dispatch disable level
	 * and calling ppc_exc_wrapup if this is a synchronous
	 * exception.
	 */
	bge		cr2, restore_nonvolatile_regs

	/* decrement ISR nest level;
	 * disable all interrupts.
	 */
	lwz		r4,  ppc_exc_msr_irq_mask@sdarel(r13)
	mfmsr	r5
	andc	r4, r5, r4
	mtmsr	r4
	lwz		r4, _ISR_Nest_level@sdarel(r13)
	addi	r4, r4, -1
	stw		r4, _ISR_Nest_level@sdarel(r13)

	/*
	 * switch back to original stack (r14 == r1 if we are
	 * still on the IRQ stack).
	 */
	mr		r1, r14

	/* restore interrupt mask */
	mtmsr	r5

	/* decrement thread_dispatch level and check
	 * if we have to run the dispatcher.
	 */
	lwz		r5,  _Thread_Dispatch_disable_level@sdarel(r13)
	addic.	r5,  r5, -1
	stw		r5,  _Thread_Dispatch_disable_level@sdarel(r13)
	
	/* test _Thread_Dispatch_disable nesting level AND
	 * lower priority locks (in cr4); ONLY if
	 * _Thread_Dispatch_disable_level == 0 AND no lock is set
	 * then call ppc_exc_wrapup which may do a context switch.
	 */
	crand	EQ(cr0), EQ(cr0), EQ(cr4)
	bne		2f
	crxor	6,6,6
	/* Far branch to ppc_exc_wrapup */
	lis		r5, ppc_exc_wrapup@h
	addi    r4, r14, FRAME_LINK_SPACE 
	ori		r5, r5, ppc_exc_wrapup@l
	mtlr	r5
	blrl
2:
	lwz		r14, GPR14_OFFSET(r1)

	/* we can skip restoring r16..r31 */
	b		skip_restore_nonvolatile_regs

restore_nonvolatile_regs:
	/* synchronous exc: restore everything from the exception frame */
	lwz		r14, GPR14_OFFSET(r1)

	/* restore stack pointer */
	lwz		r5,  GPR1_OFFSET(r1)
	stw		r5,  EXC_MIN_GPR1(r1)

	/* restore non-volatile regs */
	lwz		r13, GPR13_OFFSET(r1)
	lmw		r16, GPR16_OFFSET(r1)

skip_restore_nonvolatile_regs:
	lwz		r3,  EXC_XER_OFFSET(r1) 
	lwz		r4,  EXC_CTR_OFFSET(r1)
	mtxer	r3
	mtctr	r4

	/* restore lr, r15 */ 
	mtlr	r15
	lwz		r15, GPR15_OFFSET(r1)

	/* restore scratch regs */
	lwz		r12, GPR12_OFFSET(r1)
	lwz		r11, GPR11_OFFSET(r1)
	lwz		r10, GPR10_OFFSET(r1)
	lwz		r9,  GPR9_OFFSET(r1)
	lwz		r8,  GPR8_OFFSET(r1)
	lwz		r7,  GPR7_OFFSET(r1)
	lwz		r6,  GPR6_OFFSET(r1)
	/* r4, r5 are eventually restored by caller */
	lwz		r3,  GPR3_OFFSET(r1)
	lwz		r2,  GPR2_OFFSET(r1)
	/* r1, is eventually restored by caller */
	lwz		r0,  GPR0_OFFSET(r1)

	beq  cr3, 2f
	/* restore MSR settings */
	lwz		r5,  ppc_exc_msr_bits@sdarel(r13)
	mfmsr	r4
	andc	r4, r4, r5
	mtmsr	r4
	sync
	isync
2:

	lwz		r4,  EXC_CR_OFFSET(r1)
	mtcr	r4

	/* restore SRR and stack */
	lwz		r4,  SRR0_FRAME_OFFSET(r1)
	lwz		r5,  SRR1_FRAME_OFFSET(r1)
	blr
	
	.global	__ppc_exc_wrappers_end
__ppc_exc_wrappers_end = .
