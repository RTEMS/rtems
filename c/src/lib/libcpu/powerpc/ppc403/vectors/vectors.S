/*  vectors.s	1.1 - 95/12/04
 *
 *  This file contains the assembly code for the PowerPC 403
 *  interrupt veneers for RTEMS.
 *
 *  Author:	Andrew Bray <andy@i-cubed.co.uk>
 *
 *  COPYRIGHT (c) 1995 by i-cubed ltd.
 *
 *  To anyone who acknowledges that this file is provided "AS IS"
 *  without any express or implied warranty:
 *      permission to use, copy, modify, and distribute this file
 *      for any purpose is hereby granted without fee, provided that
 *      the above copyright notice and this notice appears in all
 *      copies, and that the name of i-cubed limited not be used in
 *      advertising or publicity pertaining to distribution of the
 *      software without specific, written prior permission.
 *      i-cubed limited makes no representations about the suitability
 *      of this software for any purpose.
 *
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
 *
 *  $Id$
 */

#include <bsp.h> /* PPC_VECTOR_FILE_BASE */
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
	.set    begin_vector,0x0000
	.set    crit_vector,0x0100
	.set    mach_vector,0x0200
	.set    prot_vector,0x0300
	.set    ext_vector,0x0500
	.set    align_vector,0x0600
	.set    prog_vector,0x0700
	.set    dec_vector,0x0900
	.set    sys_vector,0x0C00
	.set    pit_vector,0x1000
	.set    fit_vector,0x1010
	.set    wadt_vector,0x1020
	.set    debug_vector,0x2000

/* Go to the right section */
#if PPC_ASM == PPC_ASM_ELF
	.section .vectors,"awx",@progbits
#elif PPC_ASM == PPC_ASM_XCOFF
	.csect	.text[PR]
#endif

	PUBLIC_VAR (__vectors)
SYM (__vectors):
	
/* Critical error handling */
	.org	crit_vector - file_base
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

	li      r0, PPC_IRQ_CRIT
	b       PROC (_ISR_HandlerC)
	
/* Machine check exception */
	.org	mach_vector - file_base
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

	li      r0, PPC_IRQ_MCHECK
	b       PROC (_ISR_HandlerC)
	
/* Protection exception */
	.org	prot_vector - file_base
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

	li      r0, PPC_IRQ_PROTECT
	b       PROC (_ISR_Handler)

/* External interrupt */
	.org	ext_vector - file_base
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

	li      r0, PPC_IRQ_EXTERNAL
	b       PROC (_ISR_Handler)
	
/* Align exception */
	.org	align_vector - file_base
	.extern	align_h
	b	align_h
	
/* Program exception */
	.org	prog_vector - file_base
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

	li      r0, PPC_IRQ_PROGRAM
	b       PROC (_ISR_Handler)
	
/* Decrementer exception */
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

	li      r0, PPC_IRQ_PROGRAM
	b       PROC (_ISR_Handler)
	
/* System call */
	.org	sys_vector - file_base
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

	li      r0, PPC_IRQ_SCALL
	b       PROC (_ISR_Handler)
			
/* PIT interrupt */
	.org	pit_vector - file_base
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

	li      r0, PPC_IRQ_PIT
	b       PROC (_ISR_Handler)
			
/* FIT interrupt */
	.org	fit_vector - file_base
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

	li      r0, PPC_IRQ_FIT
	b       PROC (_ISR_Handler)
			
/* Watchdog interrupt */
	.org	wadt_vector - file_base
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

	li      r0, PPC_IRQ_WATCHDOG
	b       PROC (_ISR_HandlerC)
	
/* Debug exception */
debug:
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

	li      r0, PPC_IRQ_DEBUG
	b       PROC (_ISR_HandlerC)

/* Debug exception */
	.org	debug_vector - file_base
	b	debug
