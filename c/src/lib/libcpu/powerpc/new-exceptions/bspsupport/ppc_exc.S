/*
 * (c) 1999, Eric Valette valette@crf.canon.fr
 *
 * Modified and partially rewritten by Till Straumann, 2007
 *
 * Modified by Sebastian Huber <sebastian.huber@embedded-brains.de>, 2008.
 *
 * Low-level assembly code for PPC exceptions.
 *
 * This file was written with the goal to eliminate
 * ALL #ifdef <cpu_flavor> conditionals -- please do not
 * reintroduce such statements.
 */

/* Load macro definitions */
#include <rtems/asm.h>
#include <rtems/system.h>
#include <rtems/score/percpu.h>

#include "ppc_exc_asm_macros.h"

/******************************************************/
/*  PROLOGUES                                         */
/******************************************************/

	/*
	 * Expand prologue snippets for classic, ppc405-critical, bookE-critical
	 * and E500 machine-check, synchronous and asynchronous exceptions
	 */
	PPC_EXC_MIN_PROLOG_SYNC _NAME=tmpl_std        _VEC=0 _PRI=std  _FLVR=std
	PPC_EXC_MIN_PROLOG_SYNC _NAME=tmpl_p405_crit  _VEC=0 _PRI=crit _FLVR=p405_crit
	PPC_EXC_MIN_PROLOG_SYNC _NAME=tmpl_bookE_crit _VEC=0 _PRI=crit _FLVR=bookE_crit
	PPC_EXC_MIN_PROLOG_SYNC _NAME=tmpl_e500_mchk  _VEC=0 _PRI=mchk _FLVR=e500_mchk

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
	stw	VECTOR_REGISTER, VECTOR_OFFSET(r1)
	mflr	VECTOR_REGISTER

	/*
	 * We store the absolute branch target address here.  It will be used
	 * to generate the branch operation in ppc_exc_make_prologue().
	 *
	 * We add one to request the link in the generated branch instruction.
	 */
	.int	ppc_exc_wrap_auto + 1

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
	stw	r1, ppc_exc_lock_std@sdarel(r13)
	stw	VECTOR_REGISTER, ppc_exc_vector_register_std@sdarel(r13)
	mflr	VECTOR_REGISTER

	/*
	 * We store the absolute branch target address here.  It will be used
	 * to generate the branch operation in ppc_exc_make_prologue().
	 *
	 * We add one to request the link in the generated branch instruction.
	 */
	.int	ppc_exc_wrap_auto_async + 1

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
	WRAP	_FLVR=std _PRI=std _SRR0=srr0 _SRR1=srr1 _RFI=rfi

	/* ppc405 has a critical exception using srr2/srr3 */
	WRAP	_FLVR=p405_crit _PRI=crit _SRR0=srr2 _SRR1=srr3 _RFI=rfci

	/* bookE has critical exception using csrr0 cssr1 */
	WRAP	_FLVR=bookE_crit _PRI=crit _SRR0=csrr0 _SRR1=csrr1 _RFI=rfci

	/* e500 has machine-check exception using mcsrr0 mcssr1 */
	WRAP	_FLVR=e500_mchk _PRI=mchk _SRR0=mcsrr0 _SRR1=mcsrr1 _RFI=rfmci

	/* LR holds vector, VECTOR_REGISTER holds orig. LR */
        .global ppc_exc_wrap_auto
ppc_exc_wrap_auto:
	stw	FRAME_REGISTER, FRAME_OFFSET(r1)

	/* Find address where we jumped from */
	mflr	FRAME_REGISTER

	/* Restore LR */
	mtlr	VECTOR_REGISTER

	/* Compute vector into R3 */
	rlwinm	VECTOR_REGISTER, FRAME_REGISTER, 24, 26, 31

	/*
	 * We're now in almost the same state as if called by
	 * min_prolog_std but we must skip saving FRAME_REGISTER
	 * since that's done already
	 */
	b	wrap_no_save_frame_register_std

        .global ppc_exc_wrap_auto_async
ppc_exc_wrap_auto_async:
	stwu	r1, -EXCEPTION_FRAME_END(r1)
	stw	FRAME_REGISTER, FRAME_OFFSET(r1)
	/* find address where we jumped from */
	mflr	FRAME_REGISTER
	/* restore LR     */
	mtlr	VECTOR_REGISTER
	/* set upper bits to indicate that non-volatile
	 * registers should not be saved/restored.
	 */
	li	VECTOR_REGISTER, 0xffff8000
	/* compute vector into R3 */
	rlwimi	VECTOR_REGISTER, FRAME_REGISTER, 24, 26, 31
	/* we're now in almost the same state as if called by
	 * min_prolog_std but we must skip saving FRAME_REGISTER
	 * since that's done already
	 */
	b	wrap_no_save_frame_register_std

	.global	__ppc_exc_wrappers_end
__ppc_exc_wrappers_end = .
