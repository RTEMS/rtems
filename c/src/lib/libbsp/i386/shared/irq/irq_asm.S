/* irq.c
 *
 *  This file contains the implementation of the function described in irq.h
 *
 *  CopyRight (C) 1998 valette@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include "asm.h"	
#include <irq_asm.h>

	       BEGIN_CODE
	
SYM (_ISR_Handler):	
       /*
        *  Before this was point is reached the vectors unique
        *  entry point did the following:
        *
        *     1. saved scratch registers registers eax edx ecx"
        *     2. put the vector number in ecx.
        *
        * BEGINNING OF ESTABLISH SEGMENTS
        *
        *  WARNING: If an interrupt can occur when the segments are
        *           not correct, then this is where we should establish
        *           the segments.  In addition to establishing the
        *           segments, it may be necessary to establish a stack
        *           in the current data area on the outermost interrupt.
        *
        *  NOTE:  If the previous values of the segment registers are
        *         pushed, do not forget to adjust SAVED_REGS.
        *
        *  NOTE:  Make sure the exit code which restores these
        *         when this type of code is needed.
        */

       /***** ESTABLISH SEGMENTS CODE GOES HERE  ******/

       /*
        * END OF ESTABLISH SEGMENTS
        */

        /*
         *  Now switch stacks if necessary
         */

	movw	  SYM (i8259s_cache), ax # move current i8259 interrupt mask in ax
	pushl	  eax			      # push it on the stack
	/*
	 * compute the new PIC mask:
	 *
	 * <new mask> = <old mask> | irq_mask_or_tbl[<intr number aka ecx>]
	 */
	movw	  SYM (irq_mask_or_tbl) (,ecx,2), dx
	orw	  dx, ax
	/*
	 * Install new computed value on the i8259 and update cache
	 * accordingly
	 */
	movw      ax, SYM (i8259s_cache)
	outb	  $PIC_MASTER_IMR_IO_PORT
	movb	  ah, al
	outb	  $PIC_SLAVE_IMR_IO_PORT

	/*
	 * acknowledge the interrupt
	 *
	 */
	movb	$PIC_EOI, al
        cmpl    $7, ecx
	jbe	.master
	outb    $PIC_SLAVE_COMMAND_IO_PORT
.master:
        outb    $PIC_MASTER_COMMAND_IO_PORT
	
.check_stack_switch:
	pushl	  ebp
        movl      esp, ebp                  # ebp = previous stack pointer
        cmpl      $0, SYM (_ISR_Nest_level) # is this the outermost interrupt?
        jne       nested                    # No, then continue
        movl      SYM (_CPU_Interrupt_stack_high), esp

        /*
         *  We want to insure that the old stack pointer is on the
         *  stack we will be on at the end of the ISR when we restore it.
         *  By saving it on every interrupt, all we have to do is pop it
         *  near the end of every interrupt.
         */

nested:
        incl      SYM (_ISR_Nest_level)     # one nest level deeper
        incl      SYM (_Thread_Dispatch_disable_level) # disable multitasking
	/*
	 * re-enable interrupts at processor level as the current
	 * interrupt source is now masked via i8259
	 */
	sti
	
        /*
	 *  ECX is preloaded with the vector number but it is a scratch register
	 *  so we must save it again.
	 */
	
        pushl     ecx                       # push vector number
        mov       SYM (current_irq) (,ecx,4),eax
                                            # eax = Users handler
        call      *eax                      # invoke user ISR
	/*
	 * disable interrupts_again
	 */
	cli
        popl       ecx                       # ecx = vector number
	/*
	 * restore stack
	 */
	movl	  ebp, esp
	popl	  ebp

	/*
	 * restore the original i8259 masks
	 */
	popl	  eax
	movw      ax, SYM (i8259s_cache)
	outb	  $PIC_MASTER_IMR_IO_PORT
	movb	  ah, al
	outb	  $PIC_SLAVE_IMR_IO_PORT
	
	
        decl      SYM (_ISR_Nest_level)     # one less ISR nest level
                                            # If interrupts are nested,
                                            #   then dispatching is disabled

        decl      SYM (_Thread_Dispatch_disable_level)
                                            # unnest multitasking
                                            # Is dispatch disabled
        jne       .exit                     # Yes, then exit

        cmpl      $0, SYM (_Context_Switch_necessary)
                                            # Is task switch necessary?
        jne	  .schedule		    # Yes, then call the scheduler

        cmpl      $0, SYM (_ISR_Signals_to_thread_executing)
                                            # signals sent to Run_thread
                                            #   while in interrupt handler?
        je        .exit                     # No, exit

	
.bframe:
        movl      $0, SYM (_ISR_Signals_to_thread_executing)
	/*
	 * This code is the less critical path. In order to have a single
	 * Thread Context, we take the same frame than the one pushed on 
	 * exceptions. This makes sense because Signal is a software 
	 * exception.
	 */
	popl	edx
	popl	ecx
	popl	eax

	pushl	$0	# fake fault code
	pushl	$0	# fake exception number

	pusha
	pushl	esp
	call	_ThreadProcessSignalsFromIrq
	addl	$4, esp
	popa
	addl	$8, esp
	iret
			
.schedule:
	/*
	 * the scratch registers have already been saved and we are already
	 * back on the thread system stack. So we can call _Thread_Displatch 
	 * directly
	 */
	call _Thread_Dispatch
	/*
	 * fall through exit to restore complete contex (scratch registers
	 * eip, CS, Flags).
	 */
.exit:
       /*
        * BEGINNING OF DE-ESTABLISH SEGMENTS
        *
        *  NOTE:  Make sure there is code here if code is added to
        *         load the segment registers.
        *
        */

       /******* DE-ESTABLISH SEGMENTS CODE GOES HERE ********/

       /*
        * END OF DE-ESTABLISH SEGMENTS
        */
	popl	edx
	popl	ecx
	popl	eax
	iret

				
#define DISTINCT_INTERRUPT_ENTRY(_vector) \
        .p2align 4                         ; \
        PUBLIC (rtems_irq_prologue_ ## _vector ) ; \
SYM (rtems_irq_prologue_ ## _vector ):             \
        pushl	eax		; \
	pushl	ecx		; \
	pushl	edx		; \
	movl	$ _vector, ecx  ; \
        jmp   SYM (_ISR_Handler) ;

DISTINCT_INTERRUPT_ENTRY(0)
DISTINCT_INTERRUPT_ENTRY(1)
DISTINCT_INTERRUPT_ENTRY(2)
DISTINCT_INTERRUPT_ENTRY(3)
DISTINCT_INTERRUPT_ENTRY(4)
DISTINCT_INTERRUPT_ENTRY(5)
DISTINCT_INTERRUPT_ENTRY(6)
DISTINCT_INTERRUPT_ENTRY(7)
DISTINCT_INTERRUPT_ENTRY(8)
DISTINCT_INTERRUPT_ENTRY(9)
DISTINCT_INTERRUPT_ENTRY(10)
DISTINCT_INTERRUPT_ENTRY(11)
DISTINCT_INTERRUPT_ENTRY(12)
DISTINCT_INTERRUPT_ENTRY(13)
DISTINCT_INTERRUPT_ENTRY(14)
DISTINCT_INTERRUPT_ENTRY(15)
	
	/*
	 * routine used to initialize the IDT by default
	 */
		
PUBLIC (default_raw_idt_handler)
PUBLIC (raw_idt_notify)
		
SYM (default_raw_idt_handler):
	pusha
	cld
	call	raw_idt_notify
	popa
	iret
	
END_CODE

END
