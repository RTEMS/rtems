/*  dlentry.s
 *
 *  $Id$
 *
 *  This file contains the entry veneer for RTEMS programs
 *  downloaded to the eth-comm board.
 *
 * This file was written by Jay Monkman (jmonkman@fracsa.com)
 * It was based on the dlentry.s file for the Papyrus BSP,
 * written by:
 *
 *  Author:     Andrew Bray <andy@i-cubed.co.uk>
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

#include "asm.h"

/*
 *  The initial stack is set to run BELOW the code base address.
 *  (between the vectors and text sections)
 *
 *  All the entry veneer has to do is to clear the BSS.
 */

/*
 *  GDB likes to have debugging information for the entry veneer.
 *  Here is some DWARF information.
 */
/* 
 * There was some debugging info here, but I removed it because I
 * couldn't get it to work. It isn't really necessary as far as I
 * can tell. It should still be in the papyrus BSP. -Jay
 */



/* 
 * On entry to download_entry, R3 will hold a pointer to a Board Info
 *  Block (boardinfo_t). This should be copied as soon as possible
 *  to the global M860_binfo. (The block should be copied, _NOT_
 *  the pointer)
 */

        .section ".entry"  /* This might have to be the first thing in the 
                            * text section. At one time, it had to be
                            * first, but I don't believe it is true
                            * andy more. */
        PUBLIC_VAR (start)
SYM(start):
        bl      .startup
base_addr:      

/*
 * Parameters from linker
 */
toc_pointer:    
        .long   s.got
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
 * Initialization code 
 */
.startup:
        /* Get start address */
        mflr    r1
        
        /* clear the bss section */
        bl      bssclr

/* 
 * Copy the Board Info Block
 */
        .extern SYM(M860_binfo)
        lis     r6, SYM(M860_binfo)@ha
        addi    r6, r6, SYM(M860_binfo)@l

        lhz     r4, 0(r3)                /* Load the size of the block */
        rlwinm. r4, r4, 30, 0x3fffffff   /* get number of words */

        mtctr   r4

cpy_b:  lwz     r5, 0(r3)                /* In with the old ...  */
        stw     r5, 0(r6)                /* ... Out with the new */
        addi    r6, r6, 0x4              /* Go to the next word */
        addi    r3, r3, 0x4
        bdnz    cpy_b                    /* decrement counter and loop */

/*
 * C_setup.
 */

        /* set toc */
        lwz r2, toc_pointer-base_addr(r1)

        /* Set up stack pointer = beginning of text section - 56 */
        addi    r1, r1, -56-4

        lis     r9, 0xff00
        addi    r10, 0, -16384
        sth     r10, 0x950(r9)

        lis     r9, 0x0000
        addi    r9, r9, 0x0007
        mtspr   0x9e, r9

        /* clear argc and argv */
        xor     r3, r3, r3
        xor     r4, r4, r4
        
        .extern SYM (boot_card)
        bl      SYM (boot_card)                  /* call the first C routine */
loopstop:
        b       loopstop

/*
 * bssclr - zero out bss
 */
bssclr:
        lwz     r4, bss_addr-base_addr(r1)      /* Start of bss */
        lwz     r5, bss_length-base_addr(r1)    /* Length of bss */

        rlwinm. r5,r5,30,0x3FFFFFFF             /* form length/4 */
        beqlr                                   /* no bss */
        mtctr   r5                              /* set ctr reg */
        xor     r6,r6,r6                        /* r6 = 0 */
clear_bss:
        stswi   r6,r4,0x4                       /* store r6 */
        addi    r4,r4,0x4                       /* update r2 */

        bdnz    clear_bss                       /* dec counter and loop */
        blr                                     /* return */
