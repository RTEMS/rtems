/*  
 *  $Id$
 *
 *  Re-written the gen68302 start-up code.
 *  
 *  Uses gas syntax only, removed the OAR asm.h.
 *  
 *  Supplies a complete vector table in ROM.
 *  
 *  Manages all vectors with seperate handlers to trap unhandled
 *  execptions.
 *  
 *  Uses the target specific header file to get the runtime 
 *  configuration
 *  
 *  COPYRIGHT (c) 1996
 *  Objective Design Systems Pty Ltd (ODS)
 *        
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 */

|
| Entry from debugger
|
        .sect   .text

|
| Start
|
| Entered from a hardware reset.
|
        
        .global start                           | Default entry point for GNU
start:

        move.w  #0x2700,%sr                     | Disable all interrupts

        |
        | zero out uninitialized data area
        |

zerobss:
        moveal  #end,%a0                        | find end of .bss
        moveal  #bss_start,%a1                  | find beginning of .bss
        moveq   #0,%d0
        
zerobss_loop:
        
        movel   %d0,%a1@+                       | to zero out uninitialized
        cmpal   %a0,%a1
        jlt     zerobss_loop                    | loop until _end reached

        movel   #end,%d0                        | d0 = end of bss/start of heap
        addl    #heap_size,%d0                  | d0 = end of heap

        movel   %d0,stack_start                 | Save for brk() routine
        addl    #stack_size,%d0                 | make room for stack
        andl    #0xffffffc0,%d0                 | align it on 16 byte boundary

        movw    #0x3700,%sr                     | SUPV MODE,INTERRUPTS OFF!!!
        movel   %d0,%a7                         | set master stack pointer
        movel   %d0,%a6                         | set base pointer

        jsr     boot_phase_3

|                
| Initialised data
|

        .sect   .data
        
        .global start_frame
        
start_frame:
        .space  4,0

        .global stack_start
        
stack_start:
        .space  4,0

|
| Uninitialised data
|

        .sect   .bss
        
        .global environ
        .align  2
        
environ:
        .long   0

        .global heap_size
        .set    heap_size,0x2000

        .global stack_size
        .set    stack_size,0x1000


