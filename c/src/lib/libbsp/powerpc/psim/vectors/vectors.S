/*  vectors.s	1.1 - 95/12/04
 *
 *  This file contains the assembly code for the PowerPC 
 *  interrupt vectors for RTEMS.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

/*
 *  The issue with this file is getting it loaded at the right place.
 *  The first vector MUST be at address 0x????0100.
 *  How this is achieved is dependant on the tool chain.
 *
 *  However the basic mechanism for ELF assemblers is to create a
 *  section called ".vectors", which will be loaded to an address
 *  between 0x????0000 and 0x????0100 (inclusive) via a link script.
 *
 *  The basic mechanism for XCOFF assemblers is to place it in the
 *  normal text section, and arrange for this file to be located
 *  at an appropriate position on the linker command line.
 *
 *  The variable 'PPC_VECTOR_FILE_BASE' must be defined to be the
 *  offset from 0x????0000 to the first location in the file.  This
 *  will usually be 0x0000 or 0x0100.
 */

#include <bsp.h>
#include "asm.h"

#ifndef PPC_VECTOR_FILE_BASE
#error "PPC_VECTOR_FILE_BASE is not defined."
#endif

	/* Where this file will be loaded */
	.set	file_base, PPC_VECTOR_FILE_BASE

	/* Offset to store reg 0 */

	.set	IP_LINK, 0
#if (PPC_ABI == PPC_ABI_POWEROPEN || PPC_ABI == PPC_ABI_GCC27)
	.set	IP_0, (IP_LINK + 56)
#else
	.set	IP_0, (IP_LINK + 8)
#endif
	.set	IP_2, (IP_0 + 4)

	.set	IP_3, (IP_2 + 4)
	.set	IP_4, (IP_3 + 4)
	.set	IP_5, (IP_4 + 4)
	.set	IP_6, (IP_5 + 4)
	
	.set	IP_7, (IP_6 + 4)
	.set	IP_8, (IP_7 + 4)
	.set	IP_9, (IP_8 + 4)
	.set	IP_10, (IP_9 + 4)
	
	.set	IP_11, (IP_10 + 4)
	.set	IP_12, (IP_11 + 4)
	.set	IP_13, (IP_12 + 4)
	.set	IP_28, (IP_13 + 4)
	
	.set	IP_29, (IP_28 + 4)
	.set	IP_30, (IP_29 + 4)
	.set	IP_31, (IP_30 + 4)
	.set	IP_CR, (IP_31 + 4)
	
	.set	IP_CTR, (IP_CR + 4)
	.set	IP_XER, (IP_CTR + 4)
	.set	IP_LR, (IP_XER + 4)
	.set	IP_PC, (IP_LR + 4)
	
	.set	IP_MSR, (IP_PC + 4)
	
	.set	IP_END, (IP_MSR + 16)

	/* Vector offsets			 */
	.set    begin_vector,0xFFF00000
	.set    crit_vector,0xFFF00100
	.set    mach_vector,0xFFF00200
	.set    prot_vector,0xFFF00300
	.set    ext_vector,0xFFF00500
	.set    align_vector,0xFFF00600
	.set    prog_vector,0xFFF00700
	.set    dec_vector,0xFFF00900
	.set    sys_vector,0xFFF00C00
	.set    pit_vector,0xFFF01000
	.set    fit_vector,0xFFF01010
	.set    wadt_vector,0xFFF01020
	.set    debug_vector,0xFFF02000

/* Go to the right section */
#if PPC_ASM == PPC_ASM_ELF
	.section .vectors,"awx",@progbits
#elif PPC_ASM == PPC_ASM_XCOFF
	.csect	.text[PR]
#endif

	PUBLIC_VAR (__vectors)
SYM (__vectors):
	
/* Decrementer interrupt */
	.org	dec_vector - file_base
#if (PPC_ABI == PPC_ABI_POWEROPEN || PPC_ABI == PPC_ABI_GCC27)
#if (PPC_HAS_FPU)
	stwu	r1, -(20*4 + 18*8 + IP_END)(r1)
#else
	stwu	r1, -(20*4 + IP_END)(r1)
#endif
#else
	stwu	r1, -(IP_END)(r1)
#endif
	stw	r0, IP_0(r1)

	li      r0, PPC_IRQ_DECREMENTER
	b       PROC (_ISR_Handler)

