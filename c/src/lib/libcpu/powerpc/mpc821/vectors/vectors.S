/*  vectors.s	1.1 - 95/12/04
 *
 *  This file contains the assembly code for the PowerPC MPC821
 *  interrupt veneers for RTEMS.
 * 
 *  Author: Jay Monkman (jmonkman@frasca.com)
 *
 *  Copyright (C) 1998 by Frasca International, Inc.
 *
 *  Derived from c/src/lib/libcpu/ppc/ppc403/vectors/vectors.s:
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
 *  will be either 0x0000 or 0xfff0.
 *
 *  $Id$
 */

#include "asm.h"
#include <mpc821.h>

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
	.set    begin_vector,     0x0000
	.set    reset_vector,     0x0100
	.set    mach_vector,      0x0200
	.set    dsi_vector,       0x0300
	.set    isi_vector,       0x0400
	.set    ext_vector,       0x0500
	.set    align_vector,     0x0600
	.set    prog_vector,      0x0700
	.set    float_vector,     0x0800
	.set    dec_vector,       0x0900
	.set    sys_vector,       0x0C00
	.set    trace_vector,     0x0d00
	.set	syscall_vector,   0x0c00
	.set	fpassist_vector,  0x0e00
	.set	software_vector,  0x1000
	.set	itlbm_vector,     0x1100
	.set	dtlbm_vector,     0x1200
	.set	itlbe_vector,     0x1300
	.set	dtlbe_vector,     0x1400
	.set	databkpt_vector,  0x1c00
	.set	insbkpt_vector,   0x1d00
	.set	perbkpt_vector,   0x1e00
	.set	dev_vector,       0x1f00
	.set    siu_vector,       0x2000
	.set    cpm_vector,       0x2400

/* Go to the right section */
#if PPC_ASM == PPC_ASM_ELF
	.section .vectors,"awx",@progbits
#elif PPC_ASM == PPC_ASM_XCOFF
	.csect	.text[PR]
#endif

	PUBLIC_VAR (__vectors)
SYM (__vectors):
	
/* Critical error handling */
	.org	reset_vector - file_base 
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

	li      r0, PPC_IRQ_SYSTEM_RESET
	b       PROC (_ISR_Handler)
	
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
	b       PROC (_ISR_Handler)
	
/* Protection exception */
	.org	dsi_vector - file_base
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

/* Instruction Storage exception */
	.org	isi_vector - file_base
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

	li      r0, PPC_IRQ_ISI
	b       PROC (_ISR_Handler)

/* External interrupt */
/* When an external interrupt occurs, we must find out what caused it */
/*  before calling the RTEMS handler. First we use SIVEC to decide    */
/*  what signalled the interrupt to the SIU.                          */
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
        stw     r9, IP_9(r1)  	/* r9 will be restored in the next level */
        stw     r10, IP_10(r1)

	lis  	r9, m821@ha
        addi 	r9, r9, m821@l
	lbz   	r10, 0x1c(r9)		/* SIVEC */
        rlwinm	r10, r10, 4, 0, 27	/* each psuedo vector will have */
        				/* room for 16 instructions */
	addis	r10, r10, siu_vectors@ha
	addi 	r10, r10, siu_vectors@l
	mflr	r0
	mtlr	r10
	lwz 	r10, IP_10(r1)
	blr

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

/* Float exception */
	.org    float_vector - file_base
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

	li      r0, PPC_IRQ_NOFP
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
			
/* Trace interrupt */
	.org	trace_vector - file_base
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

	li      r0, PPC_IRQ_TRACE
	b       PROC (_ISR_Handler)

	.org	itlbm_vector - file_base
itlbm_vectors:
	mfspr	r2, 784      /* MI_CTR */
	mfspr	r3, 792      /* MD_CTR */
	mfspr	r4, 787      /* MI_EPN */
	mfspr	r5, 789      /* MI_TWC */
	mfspr	r6, 797      /* MD_TWC */
	mfspr	r7, 789      /* MI_TWC */
	mfspr	r8, 790      /* MI_RPN */
	mfspr	r9, 798      /* MD_RPN */
	mfspr	r10, 796      /* M_TWB */
	mfspr	r11, 793      /* M_CASID */
	mfspr	r12, 786      /* MI_AP */
	mfspr	r13, 794      /* MD_AP */
	mfspr	r14, 799      /* M_TW */
	mfspr	r15, 816      /* MI_CAM */
	mfspr	r16, 817      /* MI_RAM0 */
	mfspr	r17, 818      /* MI_RAM1 */
	mfspr	r18, 824      /* MD_CAM */
	mfspr	r19, 825      /* M_RAM0 */
	mfspr	r20, 826      /* M_RAM1 */
	.long   0

	.org	dtlbm_vector - file_base
dtlbm_vectors:
	mfspr	r1, 0x1a
	mfspr	r2, 784      /* MI_CTR */
	mfspr	r3, 792      /* MD_CTR */
	lis	r3, 0x400
	mtspr	792, r3
	mfspr	r4, 787      /* MI_EPN */
	mfspr	r5, 789      /* MI_TWC */
	mfspr	r6, 797      /* MD_TWC */
	mfspr	r7, 789      /* MI_TWC */
	mfspr	r8, 790      /* MI_RPN */
	mfspr	r9, 798      /* MD_RPN */
	mfspr	r10, 796      /* M_TWB */
	mfspr	r11, 793      /* M_CASID */
	mfspr	r12, 786      /* MI_AP */
	mfspr	r13, 794      /* MD_AP */
	mfspr	r14, 799      /* M_TW */
	mfspr	r15, 816      /* MI_CAM */
	mfspr	r16, 817      /* MI_RAM0 */
	mfspr	r17, 818      /* MI_RAM1 */
	mtspr	824, r18 
	mfspr	r18, 824      /* MD_CAM */
	mfspr	r19, 825      /* M_RAM0 */
	mfspr	r20, 826      /* M_RAM1 */
	.long   0

	.org	itlbe_vector - file_base
itlbe_vectors:
	mfspr	r2, 784      /* MI_CTR */
	mfspr	r3, 792      /* MD_CTR */
	mfspr	r4, 787      /* MI_EPN */
	mfspr	r5, 789      /* MI_TWC */
	mfspr	r6, 797      /* MD_TWC */
	mfspr	r7, 789      /* MI_TWC */
	mfspr	r8, 790      /* MI_RPN */
	mfspr	r9, 798      /* MD_RPN */
	mfspr	r10, 796      /* M_TWB */
	mfspr	r11, 793      /* M_CASID */
	mfspr	r12, 786      /* MI_AP */
	mfspr	r13, 794      /* MD_AP */
	mfspr	r14, 799      /* M_TW */
	mfspr	r15, 816      /* MI_CAM */
	mfspr	r16, 817      /* MI_RAM0 */
	mfspr	r17, 818      /* MI_RAM1 */
	mfspr	r18, 824      /* MD_CAM */
	mfspr	r19, 825      /* M_RAM0 */
	mfspr	r20, 826      /* M_RAM1 */
	.long   0

	.org	dtlbe_vector - file_base
dtlbe_vectors:
	mfspr	r2, 784      /* MI_CTR */
	mfspr	r3, 792      /* MD_CTR */
	mfspr	r4, 787      /* MI_EPN */
	mfspr	r5, 789      /* MI_TWC */
	mfspr	r6, 797      /* MD_TWC */
	mfspr	r7, 789      /* MI_TWC */
	mfspr	r8, 790      /* MI_RPN */
	mfspr	r9, 798      /* MD_RPN */
	mfspr	r10, 796      /* M_TWB */
	mfspr	r11, 793      /* M_CASID */
	mfspr	r12, 786      /* MI_AP */
	mfspr	r13, 794      /* MD_AP */
	mfspr	r14, 799      /* M_TW */
	mfspr	r15, 816      /* MI_CAM */
	mfspr	r16, 817      /* MI_RAM0 */
	mfspr	r17, 818      /* MI_RAM1 */
	mfspr	r18, 824      /* MD_CAM */
	mfspr	r19, 825      /* M_RAM0 */
	mfspr	r20, 826      /* M_RAM1 */
	.long   0


	
/* Now we look at what signaled the interrupt to the SIU. */
/*   I needed to do this in order to decode the CPM interrupts before  */
/*    calling _ISR_Handler                                             */

/* *IRQ0 */
	.org    siu_vector - file_base
siu_vectors:
	mtlr  	r0
	lwz 	r9, IP_9(r1)
	li      r0, PPC_IRQ_IRQ0
	b       PROC (_ISR_Handler)

	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop

/* Level 0 */
	mtlr	r0
	lwz	r9, IP_9(r1)
	li      r0, PPC_IRQ_LVL0
	b       PROC (_ISR_Handler)

	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop

/* *IRQ1 */
	mtlr  	r0
	lwz	r9, IP_9(r1)
	li      r0, PPC_IRQ_IRQ1
	b       PROC (_ISR_Handler)

	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop

/* This is probably not the "correct" way to do this. I need to have a 
 *  way of calling _ISR_Handler for the CPM interrupts and this is the
 *  simplest way I can think of. Since I have the CPM interrupt mapped
 *  to the SIU interrupt level 1 on the eth-comm board, I put it here.
 *  It would probably be ok if I moved this directory to under libbsp
 *  instead of libcpu. For now, deal with it.
*/
/* Level 1 - CPM */
/* Now we need to get the CPM interrupt vector */
	/* Registers:                                 */
	/*   R0 - has stored value of LR 	      */
	/*   R9 - pointer to m821 struct 	      */
	/*   R10 has already been saved and restored  */
	li	r10, 1
	sth	r10, 0x930(r9)	     /* CIVR */
	lbz	r10, 0x930(r9)	     /* if we use this as an offset into a */
	rlwinm	r10, r10, 1, 0, 31   /*  table, each entry will have room  */
				     /*  4 instructions.                   */
	addis	r10, r10, cpm_vectors@ha
	addi	r10, r10, cpm_vectors@l

	mtlr	r10
	lwz	r10, IP_10(r1)
	blr

	nop
	nop
	nop
	nop
	nop
	nop
	nop

#if 0
/* Level 1 */
	mtlr	r0
	lwz	r9, IP_9(r1)
	li      r0, PPC_IRQ_LVL1
	b       PROC (_ISR_Handler)

	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
#endif

/* *IRQ2 */
	mtlr  	r0
	lwz	r9, IP_9(r1)
	li      r0, PPC_IRQ_IRQ2
	b       PROC (_ISR_Handler)

	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop

/* Level 2 */
	mtlr	r0
	lwz	r9, IP_9(r1)
	li      r0, PPC_IRQ_LVL2
	b       PROC (_ISR_Handler)

	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop

/* *IRQ3 */
	mtlr  	r0
	lwz	r9, IP_9(r1)
	li      r0, PPC_IRQ_IRQ3
	b       PROC (_ISR_Handler)

	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop

/* Level 3 */
	mtlr	r0
	lwz	r9, IP_9(r1)
	li      r0, PPC_IRQ_LVL3
	b       PROC (_ISR_Handler)

	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop

/* *IRQ4 */
	mtlr  	r0
	lwz	r9, IP_9(r1)
	li      r0, PPC_IRQ_IRQ4
	b       PROC (_ISR_Handler)

	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop

/* Level 4 */
	mtlr	r0
	lwz	r9, IP_9(r1)
	li      r0, PPC_IRQ_LVL4
	b       PROC (_ISR_Handler)

	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop

/* *IRQ5 */
	mtlr  	r0
	lwz	r9, IP_9(r1)
	li      r0, PPC_IRQ_IRQ5
	b       PROC (_ISR_Handler)

	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop

/* Level 5 */
	mtlr	r0
	lwz	r9, IP_9(r1)
	li      r0, PPC_IRQ_LVL5
	b       PROC (_ISR_Handler)

	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop

/* *IRQ6 */
	mtlr  	r0
	lwz	r9, IP_9(r1)
	li      r0, PPC_IRQ_IRQ6
	b       PROC (_ISR_Handler)

	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop

/* Level 6 */
	mtlr	r0
	lwz	r9, IP_9(r1)
	li      r0, PPC_IRQ_LVL6
	b       PROC (_ISR_Handler)

	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop

/* *IRQ7 */
	mtlr  	r0
	lwz	r9, IP_9(r1)
	li      r0, PPC_IRQ_IRQ7
	b       PROC (_ISR_Handler)

	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop

/* Level 7 */
	mtlr	r0
	lwz	r9, IP_9(r1)
	li      r0, PPC_IRQ_LVL7
	b       PROC (_ISR_Handler)

	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop

	
/*	.org	cpm_vector - file_base*/
cpm_vectors:		
	mtlr	r0
	lwz	r9, IP_9(r1)
	li	r0, PPC_IRQ_CPM_RESERVED_0
        .long   0

	/* PC4 */
	mtlr	r0
	lwz	r9, IP_9(r1)
	li	r0, PPC_IRQ_CPM_PC4
	b	PROC (_ISR_Handler)

	/* PC5 */
	mtlr	r0
	lwz	r9, IP_9(r1)
	li	r0, PPC_IRQ_CPM_PC5
	b	PROC (_ISR_Handler)

	/* SMC2 / PIP */
	mtlr	r0
	lwz	r9, IP_9(r1)
	li	r0, PPC_IRQ_CPM_SMC2
	b	PROC (_ISR_Handler)

	/* SMC1 */
	mtlr	r0
	lwz	r9, IP_9(r1)
	li	r0, PPC_IRQ_CPM_SMC1
	b	PROC (_ISR_Handler)

	/* SPI */
	mtlr	r0
	lwz	r9, IP_9(r1)
	li	r0, PPC_IRQ_CPM_SPI
	b	PROC (_ISR_Handler)

	/* PC6 */
	mtlr	r0
	lwz	r9, IP_9(r1)
	li	r0, PPC_IRQ_CPM_PC6
	b	PROC (_ISR_Handler)

	/* Timer 4 */
	mtlr	r0
	lwz	r9, IP_9(r1)
	li	r0, PPC_IRQ_CPM_TIMER4
	b	PROC (_ISR_Handler)

	/* Reserved - we should never see this */
	mtlr	r0
	lwz	r9, IP_9(r1)
	li	r0, PPC_IRQ_CPM_RESERVED_8
        .long   0

	/* PC7 */
	mtlr	r0
	lwz	r9, IP_9(r1)
	li	r0, PPC_IRQ_CPM_PC7
	b	PROC (_ISR_Handler)

	/* PC8 */
	mtlr	r0
	lwz	r9, IP_9(r1)
	li	r0, PPC_IRQ_CPM_PC8
	b	PROC (_ISR_Handler)

	/* PC9 */
	mtlr	r0
	lwz	r9, IP_9(r1)
	li	r0, PPC_IRQ_CPM_PC9
	b	PROC (_ISR_Handler)

	/* Timer 3 */
	mtlr	r0
	lwz	r9, IP_9(r1)
	li	r0, PPC_IRQ_CPM_TIMER3
	b	PROC (_ISR_Handler)

	/* Reserved - we should never get here */
	mtlr	r0
	lwz	r9, IP_9(r1)
	li	r0, PPC_IRQ_CPM_RESERVED_D
        .long   0

	/* PC10 */
	mtlr	r0
	lwz	r9, IP_9(r1)
	li	r0, PPC_IRQ_CPM_PC10
	b	PROC (_ISR_Handler)

	/* PC11 */
	mtlr	r0
	lwz	r9, IP_9(r1)
	li	r0, PPC_IRQ_CPM_PC11
	b	PROC (_ISR_Handler)

	/* I2C */
	mtlr	r0
	lwz	r9, IP_9(r1)
	li	r0, PPC_IRQ_CPM_I2C
	b	PROC (_ISR_Handler)

	/* RISC Timer Table */
	mtlr	r0
	lwz	r9, IP_9(r1)
	li	r0, PPC_IRQ_CPM_RISC_TIMER
	b	PROC (_ISR_Handler)

	/* Timer 2 */
	mtlr	r0
	lwz	r9, IP_9(r1)
	li	r0, PPC_IRQ_CPM_TIMER2
	b	PROC (_ISR_Handler)

	/* Reserved - we should never get here */
	mtlr	r0
	lwz	r9, IP_9(r1)
	li	r0, PPC_IRQ_CPM_RESERVED_13
        .long   0

	/* IDMA2 */
	mtlr	r0
	lwz	r9, IP_9(r1)
	li	r0, PPC_IRQ_CPM_IDMA2
	b	PROC (_ISR_Handler)

	/* IDMA1 */
	mtlr	r0
	lwz	r9, IP_9(r1)
	li	r0, PPC_IRQ_CPM_IDMA1
	b	PROC (_ISR_Handler)

	/* SDMA Channel Bus Error */
	mtlr	r0
	lwz	r9, IP_9(r1)
	li	r0, PPC_IRQ_CPM_SDMA_ERROR
	b	PROC (_ISR_Handler)

	/* PC12 */
	mtlr	r0
	lwz	r9, IP_9(r1)
	li	r0, PPC_IRQ_CPM_PC12
	b	PROC (_ISR_Handler)

	/* PC13 */
	mtlr	r0
	lwz	r9, IP_9(r1)
	li	r0, PPC_IRQ_CPM_PC13
	b	PROC (_ISR_Handler)

	/* Timer 1 */
	mtlr	r0
	lwz	r9, IP_9(r1)
	li	r0, PPC_IRQ_CPM_TIMER1
	b	PROC (_ISR_Handler)

	/* PC14 */
	mtlr	r0
	lwz	r9, IP_9(r1)
	li	r0, PPC_IRQ_CPM_PC14
	b	PROC (_ISR_Handler)

	/* SCC4 */
	mtlr	r0
	lwz	r9, IP_9(r1)
	li	r0, PPC_IRQ_CPM_SCC4
	b	PROC (_ISR_Handler)

	/* SCC3 */
	mtlr	r0
	lwz	r9, IP_9(r1)
	li	r0, PPC_IRQ_CPM_SCC3
	b	PROC (_ISR_Handler)

	/* SCC2 */
	mtlr	r0
	lwz	r9, IP_9(r1)
	li	r0, PPC_IRQ_CPM_SCC2
	b	PROC (_ISR_Handler)

	/* SCC1 */
	mtlr	r0
	lwz	r9, IP_9(r1)
	li	r0, PPC_IRQ_CPM_SCC1
	b	PROC (_ISR_Handler)

	/* PC15 */
	mtlr	r0
	lwz	r9, IP_9(r1)
	li	r0, PPC_IRQ_CPM_PC15
	b	PROC (_ISR_Handler)



