/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  This file contains the raw entry points for the exceptions.
 *
 *  COPYRIGHT (c) 1989-2000.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
