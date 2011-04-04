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

#include <rtems/asm.h>
#include <bsp/irq_asm.h>
#include <rtems/score/cpu.h>

#ifndef CPU_STACK_ALIGNMENT
#error "Missing header? CPU_STACK_ALIGNMENT is not defined here"
#endif

/* Stack frame we use for intermediate storage               */
#define ARG_OFF	0
#define MSK_OFF 4
#define EBP_OFF 8        /* code restoring ebp/esp relies on */
#define ESP_OFF 12       /* esp being on top of ebp!         */
#ifdef __SSE__
#define FRM_SIZ (16+512)
#define SSE_OFF 16
#else
#define FRM_SIZ 16
#endif

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
	 * Establish an aligned stack frame
	 *   original-sp
	 *   saved-bp
	 *   saved-irq-mask
	 *   vector-arg-to-C_dispatch_isr   <- aligned SP
	 */
	movl      esp, eax
	subl      $FRM_SIZ, esp
	andl      $ - CPU_STACK_ALIGNMENT, esp
	movl      eax, ESP_OFF(esp)
	movl      ebp, EBP_OFF(esp)

#ifdef __SSE__
	/* NOTE: SSE only is supported if the BSP enables fxsave/fxrstor
	 * to save/restore SSE context! This is so far only implemented
	 * for pc386!.
	 */

	/* We save SSE here (on the task stack) because we possibly
	 * call other C-code (besides the ISR, namely _Thread_Dispatch()
	 * or _ThreadProcessSignalsFromIrq()).
	 */
    /*  don't wait here; a possible exception condition will eventually be
     *  detected when the task resumes control and executes a FP instruction
	fwait
     */
	fxsave SSE_OFF(esp)
	fninit                          /* clean-slate FPU                */
	movl   $0x1f80, ARG_OFF(esp)	/* use ARG_OFF as scratch space   */
	ldmxcsr ARG_OFF(esp)            /* clean-slate MXCSR              */
#endif


	/*
	 * acknowledge the interrupt
	 *
	 */
	movw      SYM (i8259s_cache), ax /* move current i8259 interrupt mask in ax */
	movl      eax, MSK_OFF(esp)      /* save in stack frame */
	/*
	 * compute the new PIC mask:
	 *
	 * <new mask> = <old mask> | irq_mask_or_tbl[<intr number aka ecx>]
	 */
	movw      SYM (irq_mask_or_tbl) (,ecx,2), dx
	orw       dx, ax
	/*
	 * Install new computed value on the i8259 and update cache
	 * accordingly
	 */
	movw      ax, SYM (i8259s_cache)
	outb      $PIC_MASTER_IMR_IO_PORT
	movb      ah, al
	outb      $PIC_SLAVE_IMR_IO_PORT

	movb      $PIC_EOI, al
	cmpl      $7, ecx
	jbe      .master
	outb      $PIC_SLAVE_COMMAND_IO_PORT
.master:
	outb      $PIC_MASTER_COMMAND_IO_PORT

	/*
	 *  Now switch stacks if necessary
	 */

.check_stack_switch:
	movl      esp, ebp                  /* ebp = previous stack pointer */
	cmpl      $0, SYM (_ISR_Nest_level) /* is this the outermost interrupt? */
	jne       nested                    /* No, then continue */
	movl      SYM (_CPU_Interrupt_stack_high), esp

	/*
	 *  We want to insure that the old stack pointer is in ebp
	 *  By saving it on every interrupt, all we have to do is
	 *  movl ebp->esp near the end of every interrupt.
	 */

nested:
	incl      SYM (_ISR_Nest_level)     /* one nest level deeper */
	incl      SYM (_Thread_Dispatch_disable_level) /* disable multitasking */

        /*
         * GCC versions starting with 4.3 no longer place the cld
         * instruction before string operations.  We  need to ensure
         * it is set correctly for ISR handlers.
         */
        cld

	/*
	 * re-enable interrupts at processor level as the current
	 * interrupt source is now masked via i8259
	 */
	sti

    /*
	 *  ECX is preloaded with the vector number; store as arg
	 *  on top of stack. Note that _CPU_Interrupt_stack_high
	 *  was adjusted in _CPU_Interrupt_stack_setup() (score/rtems/cpu.h)
	 *  to make sure there is space.
	 */

	movl      ecx, ARG_OFF(esp)  /* store vector arg in stack */
	call      C_dispatch_isr

	/*
	 * disable interrupts_again
	 */
	cli

	/*
	 * Restore stack. This moves back to the task stack
	 * when all interrupts are unnested.
	 */
	movl      ebp, esp

	/*
	 * restore the original i8259 masks
	 */
	movl      MSK_OFF(esp), eax
	movw      ax, SYM (i8259s_cache)
	outb	  $PIC_MASTER_IMR_IO_PORT
	movb	  ah, al
	outb	  $PIC_SLAVE_IMR_IO_PORT

	decl      SYM (_ISR_Nest_level)     /* one less ISR nest level */
	                                    /* If interrupts are nested, */
	                                    /*   then dispatching is disabled */

	decl      SYM (_Thread_Dispatch_disable_level)
	                                    /* unnest multitasking */
	                                    /* Is dispatch disabled */
	jne       .exit                     /* Yes, then exit */

	cmpb      $0, SYM (_Context_Switch_necessary)
	                                    /* Is task switch necessary? */
	jne       .schedule                 /* Yes, then call the scheduler */

	cmpb      $0, SYM (_ISR_Signals_to_thread_executing)
	                                    /* signals sent to Run_thread */
	                                    /*   while in interrupt handler? */
	je        .exit                     /* No, exit */

.bframe:
	movb      $0, SYM (_ISR_Signals_to_thread_executing)
	/*
	 * This code is the less critical path. In order to have a single
	 * Thread Context, we take the same frame than the one pushed on
	 * exceptions. This makes sense because Signal is a software
	 * exception.
	 */
	call      _ThreadProcessSignalsFromIrq

	jmp       .exit

.schedule:
	/*
	 * the scratch registers have already been saved and we are already
	 * back on the thread system stack. So we can call _Thread_Displatch
	 * directly
	 */
	call      _Thread_Dispatch
	/*
	 * fall through exit to restore complete contex (scratch registers
	 * eip, CS, Flags).
	 */
.exit:

#ifdef __SSE__
	fwait
	fxrstor   SSE_OFF(esp)
#endif

	/* restore ebp and original esp */
	addl      $EBP_OFF, esp
	popl      ebp
	popl      esp
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
	popl      edx
	popl      ecx
	popl      eax
	iret

#define DISTINCT_INTERRUPT_ENTRY(_vector) \
	.p2align 4                         ; \
	PUBLIC (rtems_irq_prologue_ ## _vector ) ; \
SYM (rtems_irq_prologue_ ## _vector ):             \
	pushl     eax                ; \
	pushl     ecx                ; \
	pushl     edx                ; \
	movl      $ _vector, ecx     ; \
	jmp       SYM (_ISR_Handler) ;

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
	mov       esp, ebp
	andl     $ - CPU_STACK_ALIGNMENT, esp
	call	  raw_idt_notify
	mov       ebp, esp
	popa
	iret

END_CODE

END
