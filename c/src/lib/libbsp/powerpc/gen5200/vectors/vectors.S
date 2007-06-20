/*===============================================================*\
| Project: RTEMS generic MPC5200 BSP                              |
+-----------------------------------------------------------------+
| Partially based on the code references which are named below.   |
| Adaptions, modifications, enhancements and any recent parts of  |
| the code are:                                                   |
|                    Copyright (c) 2005                           |
|                    Embedded Brains GmbH                         |
|                    Obere Lagerstr. 30                           |
|                    D-82178 Puchheim                             |
|                    Germany                                      |
|                    rtems@embedded-brains.de                     |
+-----------------------------------------------------------------+
| The license and distribution terms for this file may be         |
| found in the file LICENSE in this distribution or at            |
|                                                                 |
| http://www.rtems.com/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
| this file contains the irq controller handler                   |
\*===============================================================*/
/***********************************************************************/
/*                                                                     */
/*   Module:       vectors.s                                           */
/*   Date:         07/17/2003                                          */
/*   Purpose:      RTEMS assembly code for PowerPC exception veneers   */
/*                                                                     */
/*---------------------------------------------------------------------*/
/*                                                                     */
/*   Description:                                                      */
/*                                                                     */
/*---------------------------------------------------------------------*/
/*                                                                     */
/*   Code                                                              */
/*   References:   This file contains the assembly code for the        */
/*                 PowerPC exception veneers for RTEMS.                */
/*   Module:       vectors.s                                           */
/*   Project:      RTEMS 4.6.0pre1 / MCF8260ads BSP                    */
/*   Version       1.2                                                 */
/*   Date:         04/18/2002                                          */
/*                                                                     */
/*   Author(s) / Copyright(s):                                         */
/*                                                                     */
/*   (c) 1999, Eric Valette valette@crf.canon.fr                       */
/*                                                                     */
/*---------------------------------------------------------------------*/
/*                                                                     */
/*   Partially based on the code references which are named above.     */
/*   Adaptions, modifications, enhancements and any recent parts of    */
/*   the code are under the right of                                   */
/*                                                                     */
/*         IPR Engineering, Dachauer Straße 38, D-80335 München        */
/*                        Copyright(C) 2003                            */
/*                                                                     */
/*---------------------------------------------------------------------*/
/*                                                                     */
/*   IPR Engineering makes no representation or warranties with        */
/*   respect to the performance of this computer program, and          */
/*   specifically disclaims any responsibility for any damages,        */
/*   special or consequential, connected with the use of this program. */
/*                                                                     */
/*---------------------------------------------------------------------*/
/*                                                                     */
/*   Version history:  1.0                                             */
/*                                                                     */
/***********************************************************************/

#include <rtems/asm.h>
#include <bspopts.h>
#include <rtems/score/cpu.h>
#include "vectors.h"	

#define SYNC \
	sync; \
	isync
	
	.text
	.p2align 5	
		
PUBLIC_VAR(default_exception_vector_code_prolog)
SYM (default_exception_vector_code_prolog):
	/*
	 * let room for exception frame
	 */
	stwu    r1, - (EXCEPTION_FRAME_END)(r1)
	stw	r3, GPR3_OFFSET(r1)
	stw	r2, GPR2_OFFSET(r1)
	mflr	r2
	stw	r2, EXC_LR_OFFSET(r1)
	bl	0f
0:	/*
	 * r3 = exception vector entry point 
	 * (256 * vector number) + few instructions
	 */
	mflr	r3
	/*
	 * r3 = r3 >> 8 = vector
	 */
	srwi	r3,r3,8
	ba	push_normalized_frame
	
	PUBLIC_VAR (default_exception_vector_code_prolog_size)
	
	default_exception_vector_code_prolog_size= . - default_exception_vector_code_prolog
	
	.p2align 5
PUBLIC_VAR (push_normalized_frame)	
SYM (push_normalized_frame):
	stw	r3, EXCEPTION_NUMBER_OFFSET(r1)
	stw	r0, GPR0_OFFSET(r1)
	mfsrr0  r2
	stw	r2, SRR0_FRAME_OFFSET(r1)
	mfsrr1  r3
	stw	r3, SRR1_FRAME_OFFSET(r1)
	/*
	 * Save general purpose registers
	 * Already saved in prolog : R1, R2, R3, LR.
	 * Saved a few line above  : R0
	 *
	 * Manual says that "stmw" instruction may be slower than
	 * series of individual "stw" but who cares about performance 
	 * for the DEFAULT exception handler?
	 */
	stmw	r4, GPR4_OFFSET(r1)	/* save R4->R31 */

	mfcr	r31
	stw	r31,  EXC_CR_OFFSET(r1)
	mfctr	r30
	stw	r30,  EXC_CTR_OFFSET(r1)
	mfxer	r28
	stw	r28,  EXC_XER_OFFSET(r1)
        /*
         * compute SP at exception entry
         */
        addi    r2, r1, EXCEPTION_FRAME_END
        /*
         * store it at the right place
         */
        stw     r2, GPR1_OFFSET(r1)

	/*
	 * Enable data and instruction address translation, exception nesting
	 */
	mfmsr	r3
	ori     r3,r3, MSR_RI|MSR_DR /*| MSR_IR*/
	mtmsr   r3
	SYNC
	
	/*
	 * Call C exception handler
	 */
	/*
	 * store the execption frame address in r3 (first param)
	 */
	addi	r3, r1, 0x8
	/*
	 * globalExceptHdl(r3)
	 */
	addis	r4, 0, globalExceptHdl@ha
	lwz	r5, globalExceptHdl@l(r4)
	mtlr	r5
	blrl
	/*
	 * Restore registers status
	 */
	lwz	r31,  EXC_CR_OFFSET(r1)
	mtcr	r31
	lwz	r30,  EXC_CTR_OFFSET(r1)
	mtctr	r30
	lwz	r29,  EXC_LR_OFFSET(r1)
	mtlr	r29
	lwz	r28,  EXC_XER_OFFSET(r1)
	mtxer	r28

	lmw	r4, GPR4_OFFSET(r1)
	lwz	r2, GPR2_OFFSET(r1)
	lwz	r0, GPR0_OFFSET(r1)

	/*
	 * Disable data and instruction translation. Make path non recoverable...
	 */
	mfmsr	r3
	xori	r3, r3, MSR_RI|MSR_DR /*| MSR_IR */
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

#if !defined(ep5200)
	.section .vectors,"awx",@progbits

        PUBLIC_VAR (__vectors)
        SYM (__vectors):
	bl	start
	.rep 63
	.long 0x04000400
	.endr
__vec2:	b	__vec2
	.rep 63
	.long 0x04000400
	.endr
__vec3:	b	__vec3
	.rep 63
	.long 0x04000400
	.endr
__vec4:	b	__vec4
	.rep 63
	.long 0x04000400
	.endr
__vec5:	b	__vec5
	.rep 63
	.long 0x04000400
	.endr
__vec6:	b	__vec6
	.rep 63
	.long 0x04000400
	.endr
__vec7:	b	__vec7
	.rep 63
	.long 0x04000400
	.endr
__vec8:	b	__vec8
	.rep 63
	.long 0x04000400
	.endr
__vec9:	b	__vec9
	.rep 63
	.long 0x04000400
	.endr
__veca:	b	__veca
	.rep 63
	.long 0x04000400
	.endr
__vecb:	b	__vecb
	.rep 63
	.long 0x04000400
	.endr
__vecc:	b	__vecc
	.rep 63
	.long 0x04000400
	.endr
__vecd:	b	__vecd
	.rep 63
	.long 0x04000400
	.endr
__vece:	b	__vece
	.rep 63
	.long 0x04000400
	.endr
__vecf:	b	__vecf
	.rep 63+1024
	.long 0x04000400
	.endr
#endif
