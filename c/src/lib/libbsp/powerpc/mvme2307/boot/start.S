/*
 * This is based on the mvme-crt0.S file from libgloss/rs6000.
 * crt0.S -- startup file for PowerPC systems.
 *
 * Copyright (c) 1995 Cygnus Support
 *
 * The authors hereby grant permission to use, copy, modify, distribute,
 * and license this software and its documentation for any purpose, provided
 * that existing copyright notices are retained in all copies and that this
 * notice is included verbatim in any distributions. No written agreement,
 * license, or royalty fee is required for any of the authorized uses.
 * Modifications to this software may be copyrighted by their authors
 * and need not follow the licensing terms described here, provided that
 * the new terms are clearly indicated on the first page of each file where
 * they apply.
 *
 *  $Id$
 */

#include "ppc-asm.h"

        .file   "start.S"
        .section ".got2","aw"
        .align  2

.LCTOC1 = .+32768

        .extern FUNC_NAME(atexit)
        .globl  FUNC_NAME(__atexit)
        .section ".sdata","aw"
        .align  2
FUNC_NAME(__atexit):                    /* tell C's eabi-ctor's we have an atexit function */
        .long   FUNC_NAME(atexit)@fixup /* and that it is to register __do_global_dtors */

        .section ".fixup","aw"
        .align  2
        .long   FUNC_NAME(__atexit)

        .section ".got2","aw"
.Ltable = .-.LCTOC1
        .long   .LCTOC1                 /* address we think .LCTOC1 is loaded at */

.Lbss_start = .-.LCTOC1
        .long   __sbss_start            /* need to include sbss and bss sections */

.Lend = .-.LCTOC1
        .long   _end

.Lstack = .-.LCTOC1                     /* stack address if set by user */
        .long   __stack

.Ltext_start = .-.LCTOC1                /* beginning of text segment */
        .long   __text_start

        .text
        /* PPCBUG ROM bootstrap record */
        .ascii  "BOOT"                  /* "magic number" */
        .long   0x10                    /* offset to first instruction */
        .long   0x16                    /* length of record, including checksum  */
        .ascii  "BOOT"                  /* routine name */
        b       _start                  /* code of routine */
        .short  0x6b79                  /* checksum */
        .short  0x0000                  /* space filler */
.Lptr:
        .long .LCTOC1-.Laddr

        .globl  _start
        .type   _start,@function
_start:
        bl      .Laddr                  /* get current address */
.Laddr:
        mflr    r4                      /* real address of .Laddr */
        lwz     r5,(.Lptr-.Laddr)(r4)   /* linker generated address of .LCTOC1 */
        add     r5,r5,r4                /* correct to real pointer */
        lwz     r4,.Ltable(r5)          /* get linker's idea of where .Laddr is */
        subf    r4,r4,r5                /* calculate difference between where linked and current */

        /* copy text and data to RAM */
        lwz     r6,.Ltext_start(r5)     /* calculate beginning of the text */
        lwz     r7,.Lbss_start(r5)      /* calculate end of the init data */

        cmplw   1,r6,r7
        bc      4,4,.Ldone1

        subf    r8,r6,r7                /* number of bytes to copy */
        addi    r8,r8,3                 /* round up to next word boundary */
        srwi    r9,r8,2                 /* number of words to copy */
        mtctr   r9
        addi    r6,r6,-4                /* adjust so we can use stwu */
        add     r8,r6,r4                /* get source address in ROM */
.Lloop1:
        lwzu    r0,4(r8)                /* load word from ROM */
        stwu    r0,4(r6)                /* store word to RAM */
        bdnz    .Lloop1

.Ldone1:


        /* clear bss */
        lwz     r6,.Lbss_start(r5)      /* calculate beginning of the BSS */
        lwz     r7,.Lend(r5)            /* calculate end of the BSS */

        cmplw   1,r6,r7
        bc      4,4,.Ldone

        subf    r8,r6,r7                /* number of bytes to zero */
        srwi    r9,r8,2                 /* number of words to zero */
        mtctr   r9
        li      r0,0                    /* zero to clear memory */
        addi    r6,r6,-4                /* adjust so we can use stwu */
.Lloop:
        stwu    r0,4(r6)                /* zero bss */
        bdnz    .Lloop

.Ldone:

        lwz     r0,.Lstack(r5)          /* stack address or 0 */
        cmplwi  1,r0,0                  /* equal to 0? */
        bc      12,6,.Lnostack          /* use default stack if == 0 */
        mr      sp,r0                   /* use user defined stack */

.Lnostack:
        /* set up initial stack frame */
        addi    sp,sp,-4                /* make sure we don't overwrite debug mem */
        lis     r0,0
        stw     r0,0(sp)                /* clear back chain */
        stwu    sp,-56(sp)              /* push another stack frame */

        /* jump to RAM and continue */
        ba      .Lcontinue
.Lcontinue:

        /* invalidate instruction cache */
        mfspr   r4,1008                 /* read hid0 */
        ori     r5,r4,0x00000800        /* set icfi bit */
        mtspr   1008,r5
        sync
        isync
        mtspr   1008,r4                 /* restore hid0 */
        sync
        isync

        /* Let her rip */
        bl      FUNC_NAME(boot_card)

        /* return value from boot_card is argument to exit */
        bl      FUNC_NAME(exit)
        trap
.Lstart:
        .size   _start,.Lstart-_start
