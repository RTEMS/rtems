/*  cpu_asm.s
 *
 *  This file contains all assembly code for the MC68020 implementation
 *  of RTEMS.
 *
 *  ATTENTION: Modified for benchmarks
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */


#include <asm.h>

        .text

/*PAGE
 *  void _Debug_ISR_Handler_Console()
 *
 *  This routine provides the RTEMS interrupt management.
 *
 *  NOTE:
 *    Upon entry, the master stack will contain an interrupt stack frame
 *    back to the interrupted thread and the interrupt stack will contain
 *    a throwaway interrupt stack frame.  If dispatching is enabled, this
 *    is the outer most interrupt, and (a context switch is necessary or
 *    the current thread has signals), then set up the master stack to
 *    transfer control to the interrupt dispatcher.
 *  NOTE:
 *    USED TO MESURE THE TIME SPENT IN THE INTERRUPT SUBROUTINE
 *    CS5 - CS8 are linked to an oscilloscope so that you can mesure
 *    RTEMS overhead (BTW it's very short :) )
 */

/*
 *  With this approach, lower priority interrupts may
 *  execute twice if a higher priority interrupt is
 *  acknowledged before _Thread_Dispatch_disable is
 *  increamented and the higher priority interrupt
 *  preforms a context switch after executing. The lower
 *  priority intterrupt will execute (1) at the end of the
 *  higher priority interrupt in the new context if
 *  permitted by the new interrupt level mask, and (2) when
 *  the original context regains the cpu.
 */
 
#if ( M68K_HAS_VBR == 1)
.set SR_OFFSET,    0                     | Status register offset
.set PC_OFFSET,    2                     | Program Counter offset
.set FVO_OFFSET,   6                     | Format/vector offset
#else
.set SR_OFFSET,    2                     | Status register offset
.set PC_OFFSET,    4                     | Program Counter offset
.set FVO_OFFSET,   0                     | Format/vector offset placed in the stack
#endif /* M68K_HAS_VBR */
 
.set SAVED,        16                    | space for saved registers

        .align  4
        .global SYM (_Debug_ISR_Handler_Console)

SYM (_Debug_ISR_Handler_Console):

					 |
	tst.w	0x14000000		 | ALLUME CS5 
					 |

        addql   #1,SYM (_Thread_Dispatch_disable_level) | disable multitasking
        moveml  d0-d1/a0-a1,a7@-         | save d0-d1,a0-a1
        movew   a7@(SAVED+FVO_OFFSET),d0 | d0 = F/VO
        andl    #0x0fff,d0               | d0 = vector offset in vbr


#if ( CPU_HAS_SOFTWARE_INTERRUPT_STACK == 1 )
	movew	sr,d1			| Save status register
	oriw	#0x700,sr		| Disable interrupts
	tstl	SYM (_ISR_Nest_level)	| Interrupting an interrupt handler?
	bne	1f			| Yes, just skip over stack switch code
	movel	SYM(_CPU_Interrupt_stack_high),a0	| End of interrupt stack
	movel	a7,a0@-			| Save task stack pointer
	movel	a0,a7			| Switch to interrupt stack
1:
	addql   #1,SYM(_ISR_Nest_level)	| one nest level deeper
	movew	d1,sr			| Restore status register
#else
	addql   #1,SYM (_ISR_Nest_level) | one nest level deeper
#endif /* CPU_HAS_SOFTWARE_INTERRUPT_STACK == 1 */

#if ( M68K_HAS_PREINDEXING == 1 )
        movel   @( SYM (_ISR_Vector_table),d0:w:1),a0| fetch the ISR
#else
        movel   # SYM (_ISR_Vector_table),a0   | a0 = base of RTEMS table
        addal   d0,a0                    | a0 = address of vector
        movel   (a0),a0                  | a0 = address of user routine
#endif

        lsrl    #2,d0                    | d0 = vector number
        movel   d0,a7@-                  | push vector number

					 |
	tst.w	0x18000000		 | ALLUME CS6
					 |

        jbsr    a0@                      | invoke the user ISR

					 |
	tst.w	0x18000000		 | ALLUME CS6
					 |

        addql   #4,a7                    | remove vector number

#if ( CPU_HAS_SOFTWARE_INTERRUPT_STACK == 1 )
	movew	sr,d0			| Save status register
	oriw	#0x700,sr		| Disable interrupts
	subql	#1,SYM(_ISR_Nest_level)	| Reduce interrupt-nesting count
	bne	1f			| Skip if return to interrupt
	movel	(a7),a7			| Restore task stack pointer
1:
	movew	d0,sr			| Restore status register
#else
	subql   #1,SYM (_ISR_Nest_level) | one less nest level
#endif /* CPU_HAS_SOFTWARE_INTERRUPT_STACK == 1 */

        subql   #1,SYM (_Thread_Dispatch_disable_level)
                                         | unnest multitasking
        bne     Debug_exit                     | If dispatch disabled, Debug_exit

#if ( M68K_HAS_SEPARATE_STACKS == 1 )
        movew   #0xf000,d0               | isolate format nibble
        andw    a7@(SAVED+FVO_OFFSET),d0 | get F/VO
        cmpiw   #0x1000,d0               | is it a throwaway isf?
        bne     Debug_exit                     | NOT outer level, so branch
#endif

        tstl    SYM (_Context_Switch_necessary)
                                         | Is thread switch necessary?
        bne     bframe                   | Yes, invoke dispatcher

        tstl    SYM (_ISR_Signals_to_thread_executing)
                                         | signals sent to Run_thread
                                         |   while in interrupt handler?
        beq     Debug_exit               | No, then Debug_exit


bframe: clrl    SYM (_ISR_Signals_to_thread_executing)
                                         | If sent, will be processed
#if ( M68K_HAS_SEPARATE_STACKS == 1 )
        movec   msp,a0                   | a0 = master stack pointer
        movew   #0,a0@-                  | push format word
        movel   # SYM (_ISR_Dispatch),a0@- | push return addr
        | filter out the trace bit to stop single step debugging breaking
        movew   a0@(6+SR_OFFSET),d0
        andw    #0x7FFF,d0
        movew   d0,a0@-                  | push thread sr
        movec   a0,msp                   | set master stack pointer
#else

        | filter out the trace bit to stop single step debugging breaking
        movew   a7@(16+SR_OFFSET),d0
        andw    #0x7FFF,d0
        movew   d0,sr
        jsr SYM (_Thread_Dispatch)
#endif

Debug_exit:   moveml  a7@+,d0-d1/a0-a1         | restore d0-d1,a0-a1
#if ( M68K_HAS_VBR == 0 )
        addql   #2,a7                    | pop format/id
#endif /* M68K_HAS_VBR */

					 |
	tst.w	0x1C000000		 | ALLUME CS7
					 |

        rte                              | return to thread
                                         |   OR _Isr_dispatch


