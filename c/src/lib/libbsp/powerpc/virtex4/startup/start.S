/*!@file      start.S
*
*  @brief     Initialization code to set up the CPU and call boot_card()
*
*  This "BSP" targets the Xilinx Virtex XC4VFX60 and related parts.  This
*  BSP makes no assumptions on what firmware is loaded into the FPGA.
*
*  Provides the .entry section code. This is the first code to run in
*  the PPC after download to RAM.   Excecution in this case starts at
*  'download_entry'.
*
*  The entrypoint 'start' is provided for the case where a bootloader has
*  initialized the CPU, and all that remains to do is to set up a C
*  environment and call boot_card.
*
*  Derived from virtex dlentry and others.
*
*  IBM refers to the version of the processor as PPC405F5.
*  The processor version register returns 0x20011470.
*  References:
*      PowerPC Processor Reference Guide UG011 (v1.3)
*      http://www.xilinx.com/support/documentation/user_guides/ug011.pdf
*
*      PowerPC Block Reference Guide
*      http://www.xilinx.com/support/documentation/user_guides/ug018.pdf
*
*      PowerPC errata
*      ftp://ftp.xilinx.com/pub/documentation/misc/ppc405f6v5_2_0.pdf
*
*      PowerPC 405-S Embedded Processor Core User's Manual (Version 1.2)
*      https://www-01.ibm.com/chips/techlib/techlib.nsf/products/PowerPC_405_Embedded_Cores
*
*  @author    Richard Claus <claus@SLAC.Stanford.edu>
*
*  @date      March 4, 2011 -- Created
*
*  $Revision: 674 $
*
*  @verbatim                    Copyright 2011
*                                      by
*                         The Board of Trustees of the
*                       Leland Stanford Junior University.
*                              All rights reserved.
*
*         Work supported by the U.S. Department of Energy under contract
*       DE-AC03-76SF00515.
*
*                               Disclaimer Notice
*
*        The items furnished herewith were developed under the sponsorship
*   of the U.S. Government.  Neither the U.S., nor the U.S. D.O.E., nor the
*   Leland Stanford Junior University, nor their employees, makes any war-
*   ranty, express or implied, or assumes any liability or responsibility
*   for accuracy, completeness or usefulness of any information, apparatus,
*   product or process disclosed, or represents that its use will not in-
*   fringe privately-owned rights.  Mention of any product, its manufactur-
*   er, or suppliers shall not, nor is it intended to, imply approval, dis-
*   approval, or fitness for any particular use.  The U.S. and the Univer-
*   sity at all times retain the right to use and disseminate the furnished
*   items for any purpose whatsoever.                       Notice 91 02 01
*
*  @endverbatim
*/

#include <rtems/asm.h>
#include <rtems/powerpc/powerpc.h>

/*
 *  The virtex ELF link scripts support some special sections:
 *    .entry    The actual entry point
 *    .vectors  The section containing the interrupt entry veneers.
 */

/*
 *  Downloaded code loads the vectors separately to 0x00000100,
 *  so .entry can be over 256 bytes.
 *
 *  The other sections are linked in the following order:
 *    .entry
 *    .text
 *    .data
 *    .bss
 *  see linker command file for section placement
 *
 *  The initial stack is set to __stack_base.
 *
 */

        .section .entry

        PUBLIC_VAR (download_entry)
        PUBLIC_VAR (__rtems_entry_point)
SYM(download_entry):
SYM(__rtems_entry_point):
        b       startupDow        /* Entry point used by xmd dow command */

        PUBLIC_VAR (start)
SYM(start):
        b       startupBL         /* Entry point used by bootLoader */

base_addr:
       /*-------------------------------------------------------------------
        * Parameters from linker
        *-----------------------------------------------------------------*/
toc_pointer:
        .long   __got_start
bss_length:
        .long   __bss_size
bss_addr:
        .long   __bss_start
stack_top:
        .long   __stack_base
dccr_contents:
        .long   __dccr
iccr_contents:
        .long   __iccr
sgr_contents:
        .long   __sgr

       /*-------------------------------------------------------------------
        * Setup iccr, sgr, msr, cccr0, dcwr, dccr and clear bss
        *-----------------------------------------------------------------*/

startupDow:
       /*-------------------------------------------------------------------
        * Load the parameter table base address
        *------------------------------------------------------------------*/
        lis     r1,   base_addr@h
        ori     r1,r1,base_addr@l

       /* -------------------------------------------------------------------
        * Clear the Machine State Register's Critical and External
        * interrupt enables.
        *------------------------------------------------------------------*/
        mfmsr   r3
        lis     r0,   0x00028000@h
        ori     r0,r0,0x00028000@l
        andc    r3,r3,r0
        mtmsr   r3
        sync

       /* -------------------------------------------------------------------
        * Initialize the memory system.
        *------------------------------------------------------------------*/
        li      r0,0

        /* Set the Storage Guarded Register. */
        lwz     r2,sgr_contents-base_addr(r1)
        mtsgr   r2

        /* Configure endianness, compression */
        lis     r0,0x00000000@h   // Endianess value
        mtsler  r0
        lis     r0,0x00000000@h   // Compression value
        mtsu0r  r0

        /* Invalidate the entire instruction cache. */
        iccci   r0,r0

        /* Set the Instruction Cache Cacheability Register. */
        lwz     r2,iccr_contents-base_addr(r1)
        mticcr  r2
        isync

       /*-------------------------------------------------------------------
        * Tell the processor where the exception vector table will be.
        *------------------------------------------------------------------*/
        .extern SYM(__vectors)
        lis     r0, __vectors@h    /* set EVPR exc. vector prefix */
        mtspr   evpr,r0

       /*-------------------------------------------------------------------
        * Set up the debug register to freeze timers on debug events.
        *------------------------------------------------------------------*/
        mfdbcr0 r0
        ori     r0,r0,0x0001
        mtdbcr0 r0
        isync

        /* Select whether APU, Wait Enable, interrupts/exceptions and address
           translation should be enabled when application starts */
        lis     r0,0x00000000@h   /* SRR1 value */
        mtsrr1  r0                /* Potentially: 0x80000000 >> 6 is APU */

        /* Clear out stale values in certain registers to avoid confusion */
        mtxer   r0                /* Fixed-point exception register */
        mtesr   r0                /* Exception syndrome register */
        mtdear  r0                /* Data exception address register */
        mtmcsr  r0                /* Machine check syndrome register */
        mtpit   r0                /* Programmable interval timer */
        li      r0,-1             /* -1 to clear TSR */
        mttsr   r0                /* Timer status register */

        /* Invalidate the data cache */
        li      r2,0              /* Start address */
        li      r3,0x100          /* Number of cache lines */
        mtctr   r3                /* Transfer data cache congruence class count to CTR */
1:      dccci   0,r2              /* Invalidate this congruence class */
        addi    r2,r2,0x20        /* Point to next congruence class */
        bdnz    1b                /* Decrement counter and loop whilst not zero */

       /* -------------------------------------------------------------------
        * Set Core Configuration Register 0 as follows:
        * sum: 0x02700E00
        * bit 1 off:   as told by ppc405 errata to avoid CPU_213 ppc bug
        * bit 3 off:   as told by ppc405 errata to avoid CPU_213 ppc bug
                       (Note added later: PPC405F6 is not subject to CPU_213.)
        * bit 1 on:    Xilinx: CR 203746 Patch for PPC405 errata (RiC 12/8/11)
        * bit 2 on:    Xilinx: CR 203746 Patch for PPC405 errata (RiC 12/8/11)
        * bit 6 on:    load word as line
        * bit 7 off:   load  misses allocate cache line
        * bit 8 off:   store misses allocate cache line
        * bit 9-11 on: default settings to do with plb priority
        * bit 20 on:   prefetching for cacheable regions
        * bit 21 on:   prefetching for non-cacheable regions
        * bit 22 on:   request size of non-cacheable inst fetches is 8 words
        * bit 23 off:  fetch misses allocate cache line
        *------------------------------------------------------------------*/
        lis     r5,   0x52700E00@h
        ori     r5,r5,0x52700E00@l

       /* -------------------------------------------------------------------
        * To change CCR0 we make sure the code writing to it is
        *  running from the I-cache. This is needed because changing some
        * CCR0 fields will cause a hang if the processor is trying to
        * access memory at the same time.
        *------------------------------------------------------------------*/
        lis     r4,   2f@h
        ori     r4,r4,2f@l
        icbt    r0,r4
        b       2f

        .align  5 /* New cache line (32 bytes each) */
2:
        icbt    r0,r4       /* Put this line into the I-cache. */
        isync
        mtccr0  r5
        isync
        b       3f

        .align  5
3:
        /* Set the Data Cache Write-Through Register for no write-through, i.e., for write-back. */
        li      r0,0
        mtdcwr  r0

        /* Set the Data Cache Cacheablility Register. */
        lwz     r0,dccr_contents-base_addr(r1)
        mtdccr  r0
        isync

       /* Fall through */


       /* -------------------------------------------------------------------
        * If a bootloader has run that has already performed some
        * initialization, which among other things has loaded
        * this code into memory and jumped to start above, the initialization
        * above does not need to be done.  Execution thus resumes here.
        *------------------------------------------------------------------*/

startupBL:
       /* -------------------------------------------------------------------
        * Note that some initialization has already been performed by the
        * bootloader code in Block RAM, which among other things has loaded
        * this code into memory and jumped to start above.
        *------------------------------------------------------------------*/

       /*-------------------------------------------------------------------
        * Load the parameter table base address
        *------------------------------------------------------------------*/
        lis     r1,   base_addr@h
        ori     r1,r1,base_addr@l

       /*-------------------------------------------------------------------
        * Setup stack for RTEMS and call boot_card(). From this
        * point forward registers will be used in accordance with the
        * PowerPC EABI.
        *
        * boot_card() supervises the initialization of RTEMS and the C
        * library.  It calls bsp_start(), bsp_pretasking_hook(), etc.
        *------------------------------------------------------------------*/
        lwz     r2,toc_pointer-base_addr(r1)        /* set r2 to toc */
        lwz     r1,stack_top-base_addr(r1)          /* set r1 to stack_top */

        /* Align as required by ABI */
        li      r3,PPC_STACK_ALIGNMENT-1
        andc    r1,r1,r3

       /*-------------------------------------------------------------------
        * Set up r2 and r13. Upon entry r1 must have a nonzero value
        *  as it will be stored in an "init done" flag. Stupid but true.
        *  r1 must also be set up as a stack pointer as __eabi() jumps
        *  to __init() which has a standard function prolog.
        *------------------------------------------------------------------*/
        bl      __eabi

       /*-------------------------------------------------------------------
        * Zero the .bss, .sbss and .sbss2 sections.
        * Must have r2 and r13 properly set.
        *------------------------------------------------------------------*/
        bl      zero_bss

       /*-------------------------------------------------------------------
        * Create a minimal stack frame for this code, the caller of boot_card().
        *------------------------------------------------------------------*/
        addi    r1,r1, -PPC_MINIMUM_STACK_FRAME_SIZE

        xor     r3,r3,r3
        stw     r3,0(r1)            /* Terminate the chain of stack frames. */
        stw     r3,4(r1)
        stw     r3,8(r1)
        stw     r3,12(r1)
        lis     r5,environ@ha
        la      r5,environ@l(r5)    /* environp */

       /*-------------------------------------------------------------------
        * Call boot_card() with its arguments, the command-line pointer and
        * the argument count, set to NULL.
        *------------------------------------------------------------------*/
        li      r4,0                /* argv */
        li      r3,0                /* argc */
        .extern SYM (boot_card)
        b       SYM (boot_card)
