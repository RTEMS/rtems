/*
 *  This file contains the raw entry points for the exceptions.
 *
 *  COPYRIGHT (c) 1989-2000.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems/asm.h>
#include <rtems/mips/iregdef.h>
#include <rtems/mips/idtcpu.h>

/*
 *  MIPS ISA Level 1 entries
 */

#if __mips == 1

FRAME(exc_norm_code,sp,0,ra)
	la	k0, _ISR_Handler /* generic external int hndlr */
	j	k0
	nop
ENDFRAME(exc_norm_code)

FRAME(exc_dbg_code,sp,0,ra)
        la      k0, _DBG_Handler /* debug interrupt */
        j       k0
	nop
ENDFRAME(exc_dbg_code)

/* XXX this is dependent on IDT/SIM and needs to be addressed */
FRAME(exc_utlb_code,sp,0,ra)
        la      k0, (R_VEC+((48)*8))
        j       k0
	nop
ENDFRAME(exc_utlb_code)

/*
 * MIPS ISA Level 32
 * XXX Again, reliance on SIM. Not good.??????????
 */
#elif __mips == 32
FRAME(exc_tlb_code,sp,0,ra)
        la      k0, _ISR_Handler
        j       k0
	nop
ENDFRAME(exc_tlb_code)

FRAME(exc_xtlb_code,sp,0,ra)
        la      k0, _ISR_Handler
        j       k0
	nop

ENDFRAME(exc_xtlb_code)

FRAME(exc_cache_code,sp,0,ra)
        la      k0, _ISR_Handler
        j       k0
	nop
ENDFRAME(exc_cache_code)

FRAME(exc_norm_code,sp,0,ra)
	la	k0, _ISR_Handler /* generic external int hndlr */
	j	k0
	nop
ENDFRAME(exc_norm_code)

/*
 * MIPS ISA Level 3
 * XXX Again, reliance on SIM. Not good.
 */
#elif __mips == 3

FRAME(exc_tlb_code,sp,0,ra)
        la      k0, (R_VEC+((112)*8)) /* R4000 Sim location */
        j       k0
	nop
ENDFRAME(exc_tlb_code)

FRAME(exc_xtlb_code,sp,0,ra)
        la      k0, (R_VEC+((112)*8)) /* R4000 Sim location */
        j       k0
	nop

ENDFRAME(exc_xtlb_code)

FRAME(exc_cache_code,sp,0,ra)
        la      k0, (R_VEC+((112)*8)) /* R4000 Sim location */
        j       k0
	nop
ENDFRAME(exc_cache_code)

FRAME(exc_norm_code,sp,0,ra)
	la	k0, _ISR_Handler /* generic external int hndlr */
	j	k0
	nop
ENDFRAME(exc_norm_code)

#else

#error "isr_entries.S: ISA support problem"

#endif
