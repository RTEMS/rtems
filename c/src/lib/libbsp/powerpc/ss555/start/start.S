/*  start.S
 *
 *  This file contains the entry veneer for RTEMS programs on the Intec
 *  SS555 board.  It jumps to the BSP which is responsible for performing
 *  all remaining initialization.
 *
 * This file is based on several others:
 *
 * (1) start360.s from the gen68360 BSP by
 *     W. Eric Norum (eric@skatter.usask.ca)
 *     with the following copyright and license:
 *
 *     COPYRIGHT (c) 1989-1998.
 *     On-Line Applications Research Corporation (OAR).
 *
 *     The license and distribution terms for this file may in
 *     the file LICENSE in this distribution or at
 *     http://www.rtems.com/license/LICENSE.
 *
 * (2) start.s for the eth_comm port by
 *     Jay Monkman (jmonkman@fracsa.com),
 *     which itself is based on the
 *
 * (3) dlentry.s for the Papyrus BSP, written by:
 *     Andrew Bray <andy@i-cubed.co.uk>
 *     with the following copyright and license:
 *
 *     COPYRIGHT (c) 1995 by i-cubed ltd.
 *
 * (4) start860.S for the MBX821/MBX860, written by:
 *     Darlene A. Stewart <darlene.stewart@iit.nrc.ca>
 *     Copyright (c) 1999, National Research Council of Canada
 *
 *     To anyone who acknowledges that this file is provided "AS IS"
 *     without any express or implied warranty:
 *         permission to use, copy, modify, and distribute this file
 *         for any purpose is hereby granted without fee, provided that
 *         the above copyright notice and this notice appears in all
 *         copies, and that the name of i-cubed limited not be used in
 *         advertising or publicity pertaining to distribution of the
 *         software without specific, written prior permission.
 *         i-cubed limited makes no representations about the suitability
 *         of this software for any purpose.
 *
 * (5) Modifications (for MBX8xx) of respective RTEMS files:
 *     Copyright (c) 1999, National Research Council of Canada
 *
 * SS555 port sponsored by Defence Research and Development Canada - Suffield
 * Copyright (C) 2004, Real-Time Systems Inc. (querbach@realtime.bc.ca)
 */

#include <rtems/asm.h>
#include <rtems/powerpc/registers.h>

/*
 *  The initial stack is set to the top of the internal RAM.
 *
 *  All the entry veneer has to do is to clear the .bss section and copy the
 *  initializers into the .data section.
 */

/*
 *  GDB likes to have debugging information for the entry veneer.
 *  Play compiler and provide some DWARF information.
 *
 *  CHANGE TO SUIT YOUR SETUP!
 */

        .section .entry,"ax",@progbits
.L_text_b:
.L_LC1:
	.previous

.section 	.debug_sfnames
.L_sfnames_b:
	.byte "rtems/c/src/lib/libbsp/powerpc/ss555/startup/"
	.byte 0
.L_F0:
	.byte "start.S"
	.byte 0
	.previous

.section	.line
.L_line_b:
	.4byte	.L_line_e-.L_line_b
	.4byte	.L_text_b
.L_LE1:
.L_line_last:
	.4byte	0x0
	.2byte	0xffff
	.4byte	.L_text_e-.L_text_b
.L_line_e:
	.previous

.section	.debug_srcinfo
.L_srcinfo_b:
	.4byte	.L_line_b
	.4byte	.L_sfnames_b
	.4byte	.L_text_b
	.4byte	.L_text_e
	.4byte	0xffffffff
	.4byte	.L_LE1-.L_line_b
	.4byte	.L_F0-.L_sfnames_b
	.4byte	.L_line_last-.L_line_b
	.4byte	0xffffffff
	.previous

.section	.debug_pubnames
	.4byte	.L_debug_b
	.4byte	.L_P0
	.byte "start"
	.byte 0
	.4byte	0x0
	.byte 0
	.previous

.section	.debug_aranges
	.4byte	.L_debug_b
	.4byte	.L_text_b
	.4byte	.L_text_e-.L_text_b
	.4byte	0
	.4byte	0
	.4byte	0
	.4byte	0
	.4byte	0
	.4byte	0
	.4byte	0x0
	.4byte	0x0
	.previous

.section	.debug
.L_debug_b:
.L_D1:
	.4byte	.L_D1_e-.L_D1
	.2byte	0x11	/* TAG_compile_unit */
	.2byte	0x12	/* AT_sibling */
	.4byte	.L_D2
	.2byte	0x38	/* AT_name */
	.byte "start.S"
	.byte 0
	.2byte	0x258	/* AT_producer */
	.byte "GAS 2.5.2"
	.byte 0
	.2byte	0x111	/* AT_low_pc */
	.4byte	.L_text_b
	.2byte	0x121	/* AT_high_pc */
	.4byte	.L_text_e
	.2byte	0x106	/* AT_stmt_list */
	.4byte	.L_line_b
	.2byte	0x1b8	/* AT_comp_dir */
	.byte "rtems/c/src/lib/libbsp/powerpc/ss555/startup/"
	.byte 0
	.2byte	0x8006	/* AT_sf_names */
	.4byte	.L_sfnames_b
	.2byte	0x8016	/* AT_src_info */
	.4byte	.L_srcinfo_b
.L_D1_e:
.L_P0:
.L_D3:
	.4byte	.L_D3_e-.L_D3
	.2byte	0x6	/* TAG_global_subroutine */
	.2byte	0x12	/* AT_sibling */
	.4byte	.L_D4
	.2byte	0x38	/* AT_name */
	.byte "start"
	.byte 0
	.2byte	0x278	/* AT_prototyped */
	.byte 0
	.2byte	0x111	/* AT_low_pc */
	.4byte	.L_text_b
	.2byte	0x121	/* AT_high_pc */
	.4byte	.L_text_e
	.2byte	0x8041	/* AT_body_begin */
	.4byte	.L_text_b
	.2byte	0x8051	/* AT_body_end */
	.4byte	.L_text_e
.L_D3_e:

.L_D4:
	.4byte	.L_D4_e-.L_D4
	.align 2
.L_D4_e:
.L_D2:
	.previous

/*
 * Tell C's eabi-ctor's that we have an atexit function,
 * and that it is to register __do_global_dtors.
 */
	EXTERN_PROC(atexit)
	PUBLIC_VAR(__atexit)
	.section ".sdata","aw"
	.align	2
SYM(__atexit):
	EXT_PROC_REF(atexit)@fixup
	.previous

	.section ".fixup","aw"
	.align	2
	EXT_SYM_REF(__atexit)
	.previous

/* That should do it */

/*
 *  Put the entry point in its own section. That way, we can guarantee
 *  to put it first in the .text section in the linker script.
 */
        .section .entry

        PUBLIC_VAR (start)
SYM(start):
        bl      .startup	/* or bl .spin */
base_addr:

/*
 * Parameters from linker
 */
stack_top:
	.long	initStackPtr

toc_pointer:
        .long   __GOT_START__

bss_length:
        .long   bss.size
bss_addr:
        .long   bss.start

data_length:
        .long   data.size
data_addr:
        .long   data.start
contents_addr:
        .long   data.contents.start

PUBLIC_VAR (text_addr)
text_addr:
        .long   text.start

PUBLIC_VAR (text_length)
text_length:
        .long   text.size

/*
 * Spin, if necessary, to acquire control from debugger (CodeWarrior).
 */
spin:
	.long	0x0001
.spin:
	lis	r3, spin@ha
	lwz	r3, spin@l(r3)
	cmpwi	r3, 0x1
	beq	.spin

/*
 * Initialization code
 */
.startup:
        /* Capture address of linker parameters. */
        mflr    r3

	/* Set initial stack pointer to end of internal RAM - 56. */
	lwz	r1, stack_top-base_addr(r3)
        addi    r1, r1, -56

        /* Initialize essential registers. */
        bl      initregs

        /* Set TOC pointer */
        lwz r2, toc_pointer-base_addr(r3)

        /* Initialize the memory mapped MPC555 registers (done in C). */
        EXTERN_PROC (_InitSS555)
        bl	PROC (_InitSS555)

        /* Clear the .bss section. */
        bl      bssclr

	/* Copy initializers into the .data section */
	bl	datacopy

	/* Enable floating point, since gcc sometimes uses the floating
	 * point registers for data moves, even if the C source code doesn't
	 * include floating point operations.
	 */
	mfmsr	r0
	ori	r0, r0, MSR_FP
	mtmsr	r0

	/* Start system. */
	li	r3, 0			/* command line */
        EXTERN_PROC (boot_card)
        bl       PROC (boot_card)	/* call the first C routine */

        /* We should never return from boot_card, but in case we do ... */
	/* The next instructions are dependent on your runtime environment. */

stop_here:
	b	stop_here

/*
 * datacopy - copy initializers into .data section
 */
datacopy:
	lis	r3, base_addr@ha		/* point to linker data */
	addi	r3, r3, base_addr@l

        lwz     r4, contents_addr-base_addr(r3) /* .data contents in ROM */
        lwz     r5, data_addr-base_addr(r3)     /* .data section in RAM */
        lwz     r6, data_length-base_addr(r3)   /* length of .data */

        rlwinm. r6, r6, 30, 0x3FFFFFFF          /* form length / 4 */
        beqlr                                   /* no .data - return */

        mtctr   r6                              /* set ctr reg */
dc1:
	lwz	r6, 0(r4)			/* get word */
        stw     r6, 0(r5)                       /* store word */
        addi    r4, r4, 0x4                     /* next source */
        addi    r5, r5, 0x4                     /* next target */
        bdnz    dc1                             /* dec counter and loop */

        blr                                     /* return */

/*
 * bssclr - zero out bss
 */
bssclr:
	lis	r3, base_addr@ha		/* point to linker data */
	addi	r3, r3, base_addr@l

        lwz     r4, bss_addr-base_addr(r3)      /* Start of bss */
        lwz     r5, bss_length-base_addr(r3)    /* Length of bss */

        rlwinm. r5, r5, 30, 0x3FFFFFFF          /* form length/4 */
        beqlr                                   /* no bss - return */

        mtctr   r5                              /* set ctr reg */
        li      r5, 0x0000                      /* r5 = 0 */
clear_bss:
        stw     r5, 0(r4)                       /* store r6 */
        addi    r4, r4, 0x4                     /* update r4 */
        bdnz    clear_bss                       /* dec counter and loop */

        blr                                     /* return */

/*
 * initregs
 *	Initialize the MSR and basic core PowerPC registers
 *
 * Register usage:
 *	r0 - scratch
 */
initregs:
	/*
	 * Set the processor for big-endian mode, exceptions vectored to
	 * 0x000n_nnnn, no execution tracing, machine check exceptions
	 * enabled, floating-point not available, supervisor priviledge
	 * level, external interrupts disabled, power management disabled
	 * (normal operation mode).
	 */
	li	r0, 0x1000	/* MSR_ME */
	mtmsr	r0		/* Context-synchronizing */
	isync

	/*
	 * Clear the exception handling registers.
	 */
	li	r0, 0x0000
	mtdar   r0
	mtspr	sprg0, r0
	mtspr	sprg1, r0
	mtspr	sprg2, r0
	mtspr	sprg3, r0
	mtspr	srr0, r0
	mtspr	srr1, r0

	mr	r6, r0
	mr	r7, r0
	mr	r8, r0
	mr	r9, r0
	mr	r10, r0
	mr	r11, r0
	mr	r12, r0
	mr	r13, r0
	mr	r14, r0
	mr	r15, r0
	mr	r16, r0
	mr	r17, r0
	mr	r18, r0
	mr	r19, r0
	mr	r20, r0
	mr	r21, r0
	mr	r22, r0
	mr	r23, r0
	mr	r24, r0
	mr	r25, r0
	mr	r26, r0
	mr	r27, r0
	mr	r28, r0
	mr	r29, r0
	mr	r30, r0
	mr	r31, r0

	blr			/* return */

.L_text_e:
