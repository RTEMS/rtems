/*!@file      start.S
*
*  @brief     Initialization code to set up the CPU and call boot_card()
*
*  This "BSP" targets the Xilinx Virtex XC5VFX70T and related parts.  This
*  BSP makes no assumptions on what firmware is loaded into the FPGA.
*
*  Provides the .entry section code.  This is the first code to run in
*  the PPC after download to RAM.  Excecution in this case starts at
*  'download_entry'.
*
*  The entrypoint 'start' is provided for the case where a bootloader has
*  initialized the CPU, and all that remains to do is to set up a C
*  environment and call boot_card.
*
*  Derived from virtex dlentry and others.
*
*  Some portions of this code follow section 3.4 of the PPC440x5 CPU Core User's
*  Manual v7.1 from IBM.  Other parts were derived from examples provided
*  by Xilinx in their ML510 Reference Designs, e.g., ml510_bsb1_design_ppc440.
*  See boot.S in standalone/, for example.
*
*  References:
*      Embedded Processor Block in Virtex-5 FPGAs Reference Guide UG200 (v1.8)
*      http://www.xilinx.com/support/documentation/user_guides/ug200.pdf
*
*      PowerPC 440x5 Embedded Processor Core User's Manual (Version 7.1)
*      https://www-01.ibm.com/chips/techlib/techlib.nsf/products/PowerPC_440_Embedded_Core
*
*  @author    Richard Claus <claus@SLAC.Stanford.edu>
*
*  @date      March 4, 2011 -- Created
*
*  $Revision: 675 $
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

#define   V_TS_SZ_I    0x0290      // V,TS=0(Inst),SIZE=9,TID=0
#define   V_TS_SZ_D    0x0390      // V,TS=1(Data),SIZE=9,TID=0
#define   WIMG_U_S_0   0x043F      // !(U0-3),!W, I,!M,!G,!E,UX,UW,UR,SX,SW,SR
#define   WIMG_U_S_1   0x003F      // !(U0-3),!W,!I,!M,!G,!E,UX,UW,UR,SX,SW,SR
#define   PAGE_SZ      0x10000000  // 256 MB

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
 *  The initial stack is set to __stack_base
 *
 *  All the entry veneer has to do is to clear the BSS.
 */

        .section .entry

        PUBLIC_VAR(download_entry)
        PUBLIC_VAR(__rtems_entry_point)
SYM(download_entry):
SYM(__rtems_entry_point):
        b       startupDL         /* Entry point used by xmd dow command */

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


        .eject

       /*------------------------------------------------------------------
        * This code follows section 3.4 of the PPC440x5 CPU Core User's
        * Manual.  The numbers in the comments refer to the step numbers
        * therein.  Some of the implementation comes from examples provided
        * by Xilinx in their ML510 Reference Designs, e.g.,
        * ml510_bsb1_design_ppc440.  See boot.S in standalone/.
        *------------------------------------------------------------------*/
       /*------------------------------------------------------------------
        * This code is designed to accomodate warm restarts, in which the
        * application software triggers the restart of the system by branching
        * to the following code (either boot or boot1) without causing
        * one of the hardware resets: core, chip, system or JTAG (section
        * 3.2,3 in the Power PC 440-S Embedded Processor Core User's Manual).
        *-----------------------------------------------------------------*/

       /* -----------------------------------------------------------------
        * Setup CPU
        *-----------------------------------------------------------------*/
first:  li      r0,0              // Clear r0

       /* -----------------------------------------------------------------
        * Initialize the memory system.
        *------------------------------------------------------------------*/
        iccci   r0,r0             // 2. Invalidate instruction cache
        dccci   r0,r0             // 3. Invalidate data cache
        msync                     // 4. Force in-progress data PLB ops to complete

        mfdbcr0 r2                // 5. Disable all debug events
        lis     r3,0x8100
        and     r2,r2,r3          // Ignore EDM,TRAP to allow XMD use
        mtdbcr0 r2
        li      r2,-1
        mtdbsr  r2                // 6. Initialize all debug event status

       /*------------------------------------------------------------------
        * Set Core Configuration Register 0 as follows:
        * sum: 0x00200000
        * bit  1    off  Parity Recovery Enable
        * bit  4    off  Cache Read Parity Enable
        * bit 10    on   Disable Store Gathering
        * bit 11    off  Disable APU Instruction Broadcast
        * bit 16    off  Disable Trace Broadcast
        * bit 17:18 off  Specifies behaviour of icbt,dcbt/dcbtst insts
        * bit 23    off  Force Load/Store Alignment
        * bit 28:29 off  Instruction Cache Speculative Line Count
        * bit 30:31 off  Instruction Cache Speculative Line Threshold
        *            NB: UG200/pg 21: Spec. prefetching must be disabled
        *------------------------------------------------------------------*/

        lis     r2,0x0020         // 7. Set CCR0: DSTG
        mtccr0  r2                // Configure CCR0

        mtspr   ccr1,r0           // 8. Clear CCR1

       /*------------------------------------------------------------------
	* 9. Configure cache regions
        *------------------------------------------------------------------*/
	mtspr   inv0,r0
	mtspr   inv1,r0
	mtspr   inv2,r0
	mtspr   inv3,r0
	mtspr   dnv0,r0
	mtspr   dnv1,r0
	mtspr   dnv2,r0
	mtspr   dnv3,r0
	mtspr   itv0,r0
	mtspr   itv1,r0
	mtspr   itv2,r0
	mtspr   itv3,r0
	mtspr   dtv0,r0
	mtspr   dtv1,r0
	mtspr   dtv2,r0
	mtspr   dtv3,r0

       /*------------------------------------------------------------------
	* Cache victim limits
	* floors 0, ceiling max to use the entire cache -- nothing locked
        *------------------------------------------------------------------*/
	lis	r2,   0x0001f800@h
	ori	r2,r2,0x0001f800@l
	mtspr   ivlim,r2
	mtspr   dvlim,r2

       /*------------------------------------------------------------------
        * Configure instruction and data cache regions:
        * Set up register constants (r6,r7), page index (r5), address
        * variable (r4), EPN_V_TS bits (r3)
        *
        * Word 0 bits:  0xX0000290, 0xX0000390
        * Bits  Field  Inst  Data  Description
        *  0:21  EPN   0-15  0-15  Effective Page Number
        *    22  V     1     1     Valid
        *    23  TS    0     1     Translation Address Space
        * 24:27  SIZE  9     9     Page Size (9 = 256 MB)
        * 38:31  TPAR  0     0     Tag Parity
        * 32:39  TID   0     0     Translation ID (in the MMUCR)
        *
        * Word 1 bits:  0x00000000, 0x00000000
        * Bits  Field  Inst  Data  Description
        *  0:21  RPN   0     0     Real Page Number
        * 22:23  PAR1  0     0     Parity for TLB word 1
        * 28:31  ERPN  0     0     Extended Real Page Number
        *
        * Word 2 bits:  0x0000043f, 0x00000c3f
        * Bits  Field  Inst  Data  Description
        *  0: 1  PAR2  0     0     Parity for TLB word 2
        *    16  U0    0     0     User-Defineable Storage Attribute 0
        *    17  U1    0     0     User-Defineable Storage Attribute 1
        *    18  U2    0     0     User-Defineable Storage Attribute 2
        *    19  U3    0     0     User-Defineable Storage Attribute 3
        *    20  W     0     0     Write-Through
        *    21  I     1     1     Caching Inhibited
        *    22  M     0     0     Memory Coherence Required
        *    23  G     0     0     Guarded
        *    24  E     0     0     Endian
        *    26  UX    1     1     User State Execute Enable
        *    27  UW    1     1     User State Write Enable
        *    28  UR    1     1     User State Read Enable
        *    29  SX    1     1     Supervisor State Execute Enable
        *    30  SW    1     1     Supervisor State Write Enable
        *    31  SR    1     1     Supervisor State Read Enable
        *------------------------------------------------------------------*/

        mtspr   mmucr,r0          // 10a. Clear MMUCR
        li      r7,WIMG_U_S_1     // Word 2: Pages are NOT cache inhibited
        lis     r6,   PAGE_SZ@h   // Page size constant
        ori     r6,r6,PAGE_SZ@l
        mr      r5,r0             // TLB entry index
        mr      r4,r0             // Initialize RPN to zero
        mflr    r28               // Save return address
        bl      tlbSetup          // 10b. Set up the TLBs
        mtlr    r28               // Restore return address

       /*------------------------------------------------------------------
        * Select whether Wait Enable, interrupts/exceptions and which address
        * spaces should be enabled when application starts
        *------------------------------------------------------------------*/
        lis     r0,   0x00000000@h // 10d. MSR[IS]=0 MSR[DS]=0
        ori     r0,r0,0x00000000@l
        mtsrr1  r0
        mtsrr0  r28               // Return address
        rfi                       // Context synchronize to invalidate shadow TLB contents


       /*-------------------------------------------------------------------
        * Entry point used when downloaded, e.g. through XMD
        *------------------------------------------------------------------*/
startupDL:
       /*-------------------------------------------------------------------
        * Do initialization up to the point where a context sync is required
        *------------------------------------------------------------------*/
        bl      first             // Do first things first

       /*-------------------------------------------------------------------
        * 11. Tell the processor where the exception vector table will be
        *------------------------------------------------------------------*/
        .extern SYM(__vectors)
        lis     r2, __vectors@h     /* set EVPR exc. vector prefix */
        mtspr   ivpr,r2

       /*------------------------------------------------------------------
        * Set up default exception and interrupt vectors
        *------------------------------------------------------------------*/
        li       r1,0x100
        mtivor0  r1
        addi     r1,r1,0x100
        mtivor1  r1
        addi     r1,r1,0x100
        mtivor2  r1
        addi     r1,r1,0x100
        mtivor3  r1
        addi     r1,r1,0x100
        mtivor4  r1
        addi     r1,r1,0x100
        mtivor5  r1
        addi     r1,r1,0x100
        mtivor6  r1
        addi     r1,r1,0x100
        mtivor7  r1
        addi     r1,r1,0x100
        mtivor8  r1
        addi     r1,r1,0x100
        mtivor9  r1
        addi     r1,r1,0x100
        mtivor10 r1
        addi     r1,r1,0x100
        mtivor11 r1
        addi     r1,r1,0x100
        mtivor12 r1
        addi     r1,r1,0x100
        mtivor13 r1
        addi     r1,r1,0x100
        mtivor14 r1
        addi     r1,r1,0x100
        mtivor15 r1

       /*------------------------------------------------------------------
        * 12. Configure debug facilities
        *------------------------------------------------------------------*/
        mtdbcr1 r0
        mtdbcr2 r0
        mtiac1  r0
        mtiac2  r0
        mtiac3  r0
        mtiac4  r0
        mtdac1  r0
        mtdac2  r0
        mtdvc1  r0
        mtdvc2  r0
        mfdbcr0 r2                // Freeze timers on debug events
        ori     r2,r2,0x0001
        mtdbcr0 r2
        isync

       /*-------------------------------------------------------------------
        * 13. Configure timer facilities
        *------------------------------------------------------------------*/
        mtdec   r0                // Clear Decrementer to prevent exception
        mttbu   r0                // Clear Timebase to prevent Fixed Interval..
        mttbl   r0                // ..timer and Watchdog Timer exceptions
        mtpit   r0                // Programmable interval timer
        li      r1,-1             // -1 to clear TSR
        mttsr   r1                // Timer status register

       /*-------------------------------------------------------------------
        * Clear out stale values in certain registers to avoid confusion
        *------------------------------------------------------------------*/
        li      r0,0
        mtcrf   0xff,r0           // Need for simulation
        mtctr   r0                // Counter register
        mtxer   r0                // Fixed-point exception register
        mtesr   r0                // Exception syndrome register
        mtdear  r0                // Data exception address register
        mtmcsr  r0                // Machine check syndrome register

       /* Fall through */

       /* -------------------------------------------------------------------
        * If a bootloader has run that has already initialized the CPU,
        * which among other things has loaded this code into memory and
        * jumped to start above, the initialization above does not need
        * to be redone.  Execution thus resumes here.
        *------------------------------------------------------------------*/

startupBL:
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
        bl      __eabi              /* setup EABI and SYSV environment */

       /*-------------------------------------------------------------------
        * Zero the .bss, .sbss and .sbss2 sections.
        * Must have r2 and r13 properly set.
        *------------------------------------------------------------------*/
        bl      zero_bss            /* Assume Bank regs set up..., cache etc. */

       /*-------------------------------------------------------------------
        * Create a minimal stack frame for this code, the caller of boot_card().
        *------------------------------------------------------------------*/
        addi    r1,r1,-PPC_MINIMUM_STACK_FRAME_SIZE

        xor     r3,r3,r3            /* Clear r3 */
        stw     r3,0(r1)            /* Clear stack chain */
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
        b       SYM (boot_card)     /* call the first C routine */


        .eject

       /*------------------------------------------------------------------
        * Set up TLB entries: 2 entries are needed for the same 256MB page
        * one for instruction memory and the other for data memory.
        * (TS bit=0 for instructions)
        *------------------------------------------------------------------*/
tlbSetup:
1:      ori     r3,r4,V_TS_SZ_I   // Fold V_TS_SZ in with EPN=RPN
        tlbwe   r3,r5,0           // Word 0: EPN_V_TS_SZ (Instructions)
        tlbwe   r4,r5,1           // Word 1: RPN_ERPN
        tlbwe   r7,r5,2           // Word 2: WIMG_U_S
        ori     r3,r4,V_TS_SZ_D   // Fold V_TS_SZ in with EPN=RPN
        addi    r5,r5,1           // Next TLB entry
        tlbwe   r3,r5,0           // Word 0: EPN_V_TS_SZ (Data)
        tlbwe   r4,r5,1           // Word 1: RPN_ERPN
        tlbwe   r7,r5,2           // Word 2: WIMG_U_S
        add     r4,r4,r6          // Increment RPN to next 256MB block
        addi    r5,r5,1           // Next TLB entry
        cmpwi   r5,32             // Done yet?
        bne     1b
        li      r0,0
2:                                // Zero out index 32-63 TLB entries
        tlbwe   r0,r5,0
        tlbwe   r0,r5,1
        tlbwe   r0,r5,2
        addi    r5,r5,1
        cmpwi   r5,64
        bne     2b

        blr
