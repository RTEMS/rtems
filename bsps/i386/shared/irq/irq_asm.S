/*
 *  This file contains the implementation of the function described in irq.h
 */

/*
 *  Copyright (C) 1998 valette@crf.canon.fr
 *
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <rtems/asm.h>
#include <bspopts.h>
#include <rtems/score/cpu.h>
#include <rtems/score/percpu.h>

#include <bsp.h> /* to establish dependency on prototype */

#ifndef CPU_STACK_ALIGNMENT
#error "Missing header? CPU_STACK_ALIGNMENT is not defined here"
#endif

/* Stack frame we use for intermediate storage               */
#define ARG_OFF	0
#define MSK_OFF 4        /* not used any more                */
#define EBX_OFF 8        /* ebx                              */
#define EBP_OFF 12       /* code restoring ebp/esp relies on */
#define ESP_OFF 16       /* esp being on top of ebp!         */
#ifdef __SSE__
/* need to be on 16 byte boundary for SSE, add 12 to do that */
#define FRM_SIZ (20+12+512)
#define SSE_OFF 32
#else
#define FRM_SIZ 20
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
	 *   original sp
         *   saved ebx
	 *   saved ebp
	 *   saved irq mask
	 *   vector arg to BSP_dispatch_isr   <- aligned SP
	 */
	movl      esp, eax
	subl      $FRM_SIZ, esp
	andl      $ - CPU_STACK_ALIGNMENT, esp
	movl      ebx, EBX_OFF(esp)
	movl      eax, ESP_OFF(esp)
	movl      ebp, EBP_OFF(esp)

	/*
	 * GCC versions starting with 4.3 no longer place the cld
	 * instruction before string operations.  We  need to ensure
	 * it is set correctly for ISR handlers.
	 */
	cld

#ifdef __SSE__
	/* NOTE: SSE only is supported if the BSP enables fxsave/fxrstor
	 * to save/restore SSE context! This is so far only implemented
	 * for pc386!.
	 */

	/* We save SSE here (on the task stack) because we possibly
	 * call other C-code (besides the ISR, namely _Thread_Dispatch())
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
	 *  Now switch stacks if necessary
	 */

PUBLIC (ISR_STOP)
ISR_STOP:
.check_stack_switch:
	movl      esp, ebp                  /* ebp = previous stack pointer */

#ifdef RTEMS_SMP
	call      SYM(_CPU_SMP_Get_current_processor)
	sall      $PER_CPU_CONTROL_SIZE_LOG2, eax
	addl      $SYM(_Per_CPU_Information), eax
	movl      eax, ebx
#else
	movl      $SYM(_Per_CPU_Information), ebx
#endif

	/* is this the outermost interrupt? */
	cmpl      $0, PER_CPU_ISR_NEST_LEVEL(ebx)
	jne       nested                    /* No, then continue */
	movl      PER_CPU_INTERRUPT_STACK_HIGH(ebx), esp

	/*
	 *  We want to insure that the old stack pointer is in ebp
	 *  By saving it on every interrupt, all we have to do is
	 *  movl ebp->esp near the end of every interrupt.
	 */

nested:
	incl      PER_CPU_ISR_NEST_LEVEL(ebx)  /* one nest level deeper */
	incl      PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL(ebx) /* disable
	                                                        multitasking */
	/*
	 *  ECX is preloaded with the vector number; store as arg
	 *  on top of stack. Note that _CPU_Interrupt_stack_high
	 *  was adjusted in _CPU_Interrupt_stack_setup() (score/rtems/cpu.h)
	 *  to make sure there is space.
	 */

	movl      ecx, ARG_OFF(esp)  /* store vector arg in stack */
	call      BSP_dispatch_isr

	movl      ARG_OFF(esp), ecx     /* grab vector arg from stack */

	/*
	 * Restore stack. This moves back to the task stack
	 * when all interrupts are unnested.
	 */
	movl      ebp, esp

	decl      PER_CPU_ISR_NEST_LEVEL(ebx)  /* one less ISR nest level */
	                                    /* If interrupts are nested, */
	                                    /*   then dispatching is disabled */

	decl      PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL(ebx)
	                                    /* unnest multitasking */
	                                    /* Is dispatch disabled */
	jne       .exit                     /* Yes, then exit */

	cmpb      $0, PER_CPU_DISPATCH_NEEDED(ebx)
	                                    /* Is task switch necessary? */
	jne       .schedule                 /* Yes, then call the scheduler */
	jmp       .exit                     /* No, exit */

.schedule:
	/*
	 * the scratch registers have already been saved and we are already
	 * back on the thread system stack. So we can call _Thread_Dispatch
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

	/* restore ebx, ebp and original esp */
	addl      $EBX_OFF, esp
	popl      ebx
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
DISTINCT_INTERRUPT_ENTRY(16)

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
