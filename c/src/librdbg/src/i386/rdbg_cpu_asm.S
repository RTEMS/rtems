/*  cpu_asm.s
 *
 *  This file contains all assembly code for the Intel i386 implementation
 *  of RDBG.
 *
 */

#include <asm.h>

        BEGIN_CODE

/*
 *  void copyback_data_cache_and_invalidate_instr_cache()
 *
 *  This routine performs a copy of the data cache 
 *  and invalidate the instruction cache
 */

        .p2align  1
        PUBLIC (copyback_data_cache_and_invalidate_instr_cache)

SYM (copyback_data_cache_and_invalidate_instr_cache):
        wbinvd
        ret


	
/*
 * void enterRdbg(void)
 *	
 * This function perform a call to the exception 19
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

        PUBLIC (enterRdbg)

SYM (enterRdbg):
        int $50
        ret


/*
 * void rtems_exception_prologue_50(void)
 *	
 * Exception 50 is used to enter Rdbg
 *
 */
	
        .p2align 4
	
        PUBLIC (rtems_exception_prologue_50)
        PUBLIC (_Exception_Handler)

SYM (rtems_exception_prologue_50):	
	pushl	$ 0
	pushl	$ 50	
        jmp   SYM(_Exception_Handler) ;


END_CODE

END
