/*  cpu_asm.s
 *
 *  This file contains all assembly code for the Intel i386 implementation
 *  of RDBG.
 *
 * $Id$
 *
 */

#include <asm.h>
#include <rtems/score/cpu.h>
#include <libcpu/io.h>

        BEGIN_CODE

/*
 *  void copyback_data_cache_and_invalidate_instr_cache(addr, size)
 *
 *  This routine performs a copy of the data cache 
 *  and invalidate the instruction cache
 */

        .p2align  5
        PUBLIC_VAR (copyback_data_cache_and_invalidate_instr_cache)

SYM (copyback_data_cache_and_invalidate_instr_cache):
	/* make sure the data changed is in the cache */
	sync
	/* r3 address to handle, r4 length in bytes */
	addi	r6, r0, PPC_CACHE_ALIGNMENT
	/* r5 = last address to handle */
	add	r5,r3,r4
	/* r3 = cache_align(r3, PPC_CACHE_ALIGNMENT) */
	subi	r0,r6,1
	andc	r3,r3,r0
	/* R4 = R3 = copy of first address */
	mr	r4,r3
	/*
	 * Copyback data cache
	 */
1:	cmplw	r4,r5	 /* r4 >= r5 then done */
	dcbst	0,r4	 /* flush (data cache bloc store) */
	add	r4,r4,r6 /* r4 = next cache line addr */
	blt	1b	 /* end r4 >= r5 then done */
	sync		/* Wait for all dcbst to complete on bus */
	/*
	 * invalidate instruction cache
	 */
	/* R4 = fisrt address */
	mr	r4,r3
2:	cmplw	r4,r5	 /* r4 >= r5 then done */
	icbi	0,r4	 /* invalidate (instruction cache bloc invalidate) */
	add	r4,r4,r6 /* r4 = next cache line addr */
	blt	2b	 /* end r4 >= r5 then done */
	sync		 /* Wait for all icbi to complete on bus */
	isync
	blr

	
/*
 * void enterRdbg(void)
 *	
 * This function perform a call to the exception SYSTEM call
 * It is used :
 *   1 - in the user code, to simulate a Breakpoint.
 *       (with justSaveContext = 0)
 *   2 - in the RDBG code, to push a ctx in the list.
 *       (with justSaveContext = 1)
 *
 * In most of case, it will be use as described in 1.
 * The 2nd possibility will be used by RDBG to obtain
 * its own ctx
 */

        PUBLIC_VAR (enterRdbg)

SYM (enterRdbg):
	sc
	blr
	
END_CODE

END
