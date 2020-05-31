/*--------------------------------------------------------------------------+
 * start16.s v1.0 - PC386 BSP - 1998/04/13
 * startAP.s 05/2019
 *--------------------------------------------------------------------------+
 * This file contains the initialization code for application processors (AP)
 * for i386 based board support packages in SMP configuration.
 * The APs start in 16 bit real mode. The goal is to:
 * 1. Initialize the CPU registers
 * 2. Load the global descriptor table
 * 3. Switch to protected mode
 * 4. Setup the stack pointers
 * 5. Switch to the higher level initialization routine
 *
 *--------------------------------------------------------------------------+
 * (C) Copyright 1997 -
 * - NavIST Group - Real-Time Distributed Systems and Industrial Automation
 *
 * http://pandora.ist.utl.pt
 *
 * Instituto Superior Tecnico * Lisboa * PORTUGAL
 *--------------------------------------------------------------------------+
 * Disclaimer:
 *
 * This file is provided "AS IS" without warranty of any kind, either
 * expressed or implied.
 *--------------------------------------------------------------------------+
 */

/*
 *  COPYRIGHT (c) 2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */


#include <bspopts.h>

/*---------------------------------------------------------------------------+
| Constants
+----------------------------------------------------------------------------*/

.set PROT_CODE_SEG, 0x08        # offset of code segment descriptor into GDT
.set PROT_DATA_SEG, 0x10        # offset of code segment descriptor into GDT
.set CR0_PE,        1           # protected mode flag on CR0 register
.set HDRSTART,      HEADERADDR  # address of start of bin2boot header
.set HDROFF,        0x24        # offset into bin2boot header of start32 addr
.set STACKOFF,      0x200-0x10  # offset to load into %esp, from start of image

/* #define NEW_GAS */
#ifdef NEW_GAS
  #define LJMPL ljmpl
#else
  #define LJMPL ljmp
#endif

/*----------------------------------------------------------------------------+
| CODE section
+----------------------------------------------------------------------------*/

.text
        .globl app_processor_start                # entry point
app_processor_start:

.code16
        cli                     # DISABLE INTERRUPTS!!!
        jmp     setup_processor
/*
 * Placeholder to copy information from boot_cpu()
 * Do NOT move or add asm instruction before
 */
.align 4
app_cpu_start:
	.long   0
app_cpu_stack:
	.long   0

setup_processor:
        movw    %cs, %ax                   # Initialize the rest of
        movw    %ax, %ds                   #   segment registers
        movw    %ax, %es
        movw    %ax, %ss

        /*---------------------------------------------------------------------+
        | Bare PC machines boot in real mode! We have to turn protected mode on.
        +---------------------------------------------------------------------*/

        lgdt    gdtptr - app_processor_start  # load Global Descriptor Table

        movl    %cr0, %eax
        orl     $CR0_PE, %eax
        movl    %eax, %cr0              # turn on protected mode
        LJMPL   $PROT_CODE_SEG, $start_32bit     # flush prefetch queue, and reload %cs

.code32
start_32bit:

        /*---------------------------------------------------------------------+
        | load the other segment registers
        +---------------------------------------------------------------------*/
        movl    $PROT_DATA_SEG, %eax
        movw    %ax, %ds
        movw    %ax, %es
        movw    %ax, %ss
        /* Prepare stack pointers */
        movl    app_cpu_stack, %esp            # stack pointer
        movl    app_cpu_stack, %ebp            # base pointer
        movl    app_cpu_start, %eax             # jump to app CPU start
        pushl   %eax
        /* Clear stack pointer to signal that the we jump to the kernel */
        movl    $0, app_cpu_stack
        /* Switch to the higher level initialization routines */
        ret

/*----------------------------------------------------------------------------+
| DATA section
+----------------------------------------------------------------------------*/

/**************************
* GLOBAL DESCRIPTOR TABLE *
**************************/

        .p2align 4
gdtptr:
        /* we use the NULL descriptor to store the GDT pointer - a trick quite
           nifty due to: Robert Collins (rcollins@x86.org) */
        .word   gdtlen - 1
        .long   gdtptr
        .word   0x0000

        /* code segment */
        .word   0xffff, 0
        .byte   0, 0x9f, 0xcf, 0

        /* data segment */
        .word   0xffff, 0
        .byte   0, 0x93, 0xcf, 0

     	/* gs segment */
        .word 0xffff, 0
        .byte 0, 0x92, 0xcf, 0

        .set    gdtlen, . - gdtptr      # length of GDT
