/*  start.S
 *
 *  This file contains the entry veneer for RTEMS programs
 *  on the MBX8xx board.
 *  It jumps to the BSP which is responsible for performing
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
 * Modifications (for MBX8xx) of respective RTEMS files:
 * Copyright (c) 1999, National Research Council of Canada
 */

#include <rtems/asm.h>

/*
 *  The initial stack is set to run BELOW the code base address.
 *  (between the vectors and text sections)
 *
 *  All the entry veneer has to do is to clear the BSS.
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
	.byte "rtems/c/src/lib/libbsp/powerpc/mbx8xx/startup/"
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
	.byte "rtems/c/src/lib/libbsp/powerpc/mbx8xx/startup/"
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
toc_pointer:
        .long   __GOT_START__
bss_length:
        .long   bss.size
bss_addr:
        .long   bss.start

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
 * test function:	 blink orange led once
 */
#define LEDBLINK_DELAY (5*1000*1000)
#define LEDPORT 0xFA100001
#define LEDMASK 0xf0
#define LEDON   0x00
#define LEDOFF  0x08

	PUBLIC_VAR(ledblink)
SYM(ledblink):
	lis	r3,LEDBLINK_DELAY>>16
ledblink1:
	subi	r3,r3,1
	cmpi	0,1,r3,0
	bne	ledblink1
	/*
	 * turn orange led off
	 */
	lis	r3,LEDPORT@ha
	lbz	r0,LEDPORT@l(r3)
	andi.	r0,r0,LEDMASK
	ori	r0,r0,LEDOFF
	stb	r0,LEDPORT@l(r3)

	lis	r3,LEDBLINK_DELAY>>16
ledblink2:
	subi	r3,r3,1
	cmpi	0,1,r3,0
	bne	ledblink2
	/*
	 * turn orange led on
	 */
	lis	r3,LEDPORT@ha
	lbz	r0,LEDPORT@l(r3)
	andi.	r0,r0,LEDMASK
	ori	r0,r0,LEDON
	stb	r0,LEDPORT@l(r3)

	blr
/*
 * #define LOADED_BY_EPPCBUG
 */
#define LOADED_BY_EPPCBUG
#define EARLY_CONSOLE
/*
 * Initialization code
 */
.startup:
        /* Get the start address. */
        mflr    r1
#ifdef LOADED_BY_EPPCBUG
	/* Save pointer to residual/board data */
	lis	r9,eppcbugInfo@ha
	stw	r3,eppcbugInfo@l(r9)
#endif
        /* Initialize essential registers. */
        bl      initregs
        nop

	/*
	 * C_setup.
	 */

        /* set toc */
        lwz r2, toc_pointer-base_addr(r1)

        /* Set up stack pointer = beginning of text section - 56 */
        addi    r1, r1, -56-4

        /* Initialize the memory mapped MPC821 registers (done in C). */
        EXTERN_PROC (_InitMBX8xx)
        bl	PROC (_InitMBX8xx)
        nop

        /* Clear the bss section. */
        bl      bssclr
        nop
#if defined(EARLY_CONSOLE) && defined(LOADED_BY_EPPCBUG)
	EXTERN_PROC (serial_init)
	bl	PROC (serial_init)
#endif
	lis	r5,environ@ha
        la	r5,environ@l(r5)		/* environp */
        /* clear argc command line */
        xor     r3, r3, r3

        EXTERN_PROC (boot_card)
        bl       PROC (boot_card)	/* call the first C routine */
        nop

        /* we should never return from boot_card, but in case we do ... */
	/* The next instructions are dependent on your runtime environment */

	/* Return to EPPCBug */
	lis  	r10, 0x0400		/* Data cache disable */
	mtspr 	568, r10
	isync

	mtspr   560, r10		/* Instruction cache disable */
	isync

stop_here:
	li	r10, 0x0F00		/* .RETURN */
	sc

	b	stop_here
        nop

/*
 * bssclr - zero out bss
 */
bssclr:
	lis	r3, base_addr@ha
	addi	r3, r3, base_addr@l
        lwz     r4, bss_addr-base_addr(r3)      /* Start of bss */
        lwz     r5, bss_length-base_addr(r3)    /* Length of bss */

        rlwinm. r5,r5,30,0x3FFFFFFF             /* form length/4 */
        beqlr                                   /* no bss - return */
        mtctr   r5                              /* set ctr reg */

        li      r5,0x0000                       /* r5 = 0 */
clear_bss:
        stw     r5,0(r4)                        /* store r6 */
        addi    r4,r4,0x4                       /* update r4 */
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
	 * Disable address translation. We should already be running in real space,
	 * so this should be a no-op, i.e. no need to switch instruction stream
	 * addresses from virtual space to real space. Other bits set the processor
	 * for big-endian mode, exceptions vectored to 0x000n_nnnn (vectors are
	 * already in low memory!), no execution tracing, machine check exceptions
	 * enabled, floating-point not available (MPC8xx has none), supervisor
	 * priviledge level, external interrupts disabled, power management
	 * disabled (normal operation mode).
	 */
	li	r0, 0x1000	/* MSR_ME */
	mtmsr	r0		/* Context-synchronizing */
	isync

	/*
	 * Clear the exception handling registers.
	 * Note SPRG3 is reserved for use by EPPCBug on the MBX8xx.
	 */
	li	r0, 0x0000
	mtdar   r0
	mtspr	sprg0, r0
	mtspr	sprg1, r0
	mtspr	sprg2, r0
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
