/*  dlentry.s	1.0 - 95/08/08
 *
 *  This file contains the entry veneer for RTEMS programs
 *  downloaded to Papyrus.
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
 *  $Id$
 */

#include "asm.h"

/*
 *  The Papyrus ELF link scripts support three special sections:
 *    .entry    The actual entry point, this must contain less
 *              than 256 bytes of code/data to fit below the
 *              .vectors section.  This always preceeds any other
 *              code or data.
 *    .vectors  The section containing the interrupt entry veneers.
 *    .entry2   Any code overflowing from .entry
 *    .descriptors The PowerOpen function indirection blocks.
 */

/*
 *  Downloaded code loads the vectors separately to 0x00000100,
 *  so .entry can be over 256 bytes.
 *
 *  The other sections are linked in the following order:
 *    .entry
 *    .entry2
 *    .text
 *    .descriptors
 *    .data
 *    .bss
 *  usually starting from 0x00020000.
 *
 *  The initial stack is set to run BELOW the code base address.
 *
 *  All the entry veneer has to do is to clear the BSS.
 */

/*
 *  GDB likes to have debugging information for the entry veneer.
 *  Here is some DWARF information.
 */

#if PPC_ASM == PPC_ASM_ELF
        .section .entry,"ax",@progbits
.L_text_b:
.L_LC1:
	.previous

.section	.debug_sfnames
.L_sfnames_b:
	.byte "/home/andy/powerpc/rtems-3.2.0/c/src/lib/libcpu/ppc/ppc403/dlentry/"
	.byte 0
.L_F0:
	.byte "dlentry.s"
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
	.byte "download_entry"
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
	.byte "dlentry.s"
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
	.byte "/home/andy/powerpc/rtems-3.2.0/c/src/lib/libcpu/ppc/ppc403/dlentry/"
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
	.byte "download_entry"
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
#endif
				
/*-------------------------------------------------------------------------------
 * ROM Vector area.
 *------------------------------------------------------------------------------*/
#if PPC_ASM == PPC_ASM_ELF
        .section .entry
#else
	.csect .text[PR]
#endif

	PUBLIC_VAR (download_entry)
SYM(download_entry):
	bl	.startup
base_addr:	

/*-------------------------------------------------------------------------------
 * Parameters from linker
 *------------------------------------------------------------------------------*/
toc_pointer:	
#if PPC_ASM == PPC_ASM_ELF
	.long	s.got
#else
	.long	TOC[tc0]
#endif
bss_length:	
	.long	bss.size
bss_addr:	
	.long	bss.start
/*-------------------------------------------------------------------------------
 * Reset_entry.
 *------------------------------------------------------------------------------*/
.startup:
	/* Get start address, stack grows down from here... */
	mflr	r1
	
	/* Assume Bank regs set up..., cache etc. */
        bl      bssclr

        /*-----------------------------------------------------------------------
         * C_setup.
         *----------------------------------------------------------------------*/
        lwz     r2,toc_pointer-base_addr(r1)       /* set r2 to toc */
        addi    r1,r1,-56-4              /* start stack at text_addr - 56 */
        addi    r3,r0,0x0                /* clear r3 */
	stw	r3, 0(r1)		 /* Clear stack chain */
	stw	r3, 4(r1)
	stw	r3, 8(r1)
	stw	r3, 12(r1)
        .extern SYM (main)
        b       SYM (main)          		 /* call the first C routine */

/*-------------------------------------------------------------------------------
 * bssclr.
 *------------------------------------------------------------------------------*/
bssclr:
        /*-----------------------------------------------------------------------
         * Data move finished, zero out bss.
         *----------------------------------------------------------------------*/
        lwz     r2,bss_addr-base_addr(r1)         /* start of bss set by loader */
        lwz     r3,bss_length-base_addr(r1)       /* bss length */
        rlwinm. r3,r3,30,0x3FFFFFFF     /* form length/4 */
        beqlr                           /* no bss */
        mtctr   r3                      /* set ctr reg */
        xor     r6,r6,r6                /* r6 = 0 */
clear_bss:
        stswi   r6,r2,0x4               /* store r6 */
        addi    r3,r2,0x4               /* update r2 */
        bdnz    clear_bss               /* decrement counter and loop */
        blr                             /* return */
.L_text_e:

#if PPC_ABI == PPC_ABI_POWEROPEN
	DESCRIPTOR (startup)
#endif
