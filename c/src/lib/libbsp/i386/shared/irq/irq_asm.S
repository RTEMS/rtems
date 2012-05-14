/* irq.c
 *
 *  This file contains the implementation of the function described in irq.h
 *
 *  Copyright (C) 1998 valette@crf.canon.fr
 *
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems/asm.h>
#include <rtems/system.h>
#include <bspopts.h>
#include <bsp/irq_asm.h>
#include <rtems/score/cpu.h>
#include <rtems/score/percpu.h>

#ifndef CPU_STACK_ALIGNMENT
#error "Missing header? CPU_STACK_ALIGNMENT is not defined here"
#endif

/* Stack frame we use for intermediate storage               */
#define ARG_OFF	0
#define MSK_OFF 4
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
	 *     1. saved scratch registers registers eax edx ecx
	 *     2. put the vector number in ecx.
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
	 *   vector arg to C_dispatch_isr   <- aligned SP
	 */
	movl      esp, eax
	subl      $FRM_SIZ, esp
	andl      $ - CPU_STACK_ALIGNMENT, esp
	movl      ebx, EBX_OFF(esp)
	movl      eax, ESP_OFF(esp)
	movl      ebp, EBP_OFF(esp)
	movw      SYM (i8259s_cache), ax /* save current i8259 interrupt mask */
	movl      eax, MSK_OFF(esp)      /* save in stack frame */

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

.check_stack_switch:
	movl      esp, ebp                  /* ebp = previous stack pointer */
#if defined(RTEMS_SMP) && defined(BSP_HAS_SMP)
	movl     $SYM(_Per_CPU_Information_p), ebx
	call     SYM(bsp_smp_processor_id)
	mov      (ebx,eax,4), ebx
        pushl    ecx
        call     SYM(_ISR_SMP_Enter)
        popl     ecx
        cmpl     $0, eax
        jne      .i8259
        movl     PER_CPU_INTERRUPT_STACK_HIGH(ebx), esp
       
#else
	movl     $SYM(_Per_CPU_Information), ebx

	/*
         *  Is this the outermost interrupt?
	 *  Switch stacks if necessary
	 */
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
	incl      SYM (_Thread_Dispatch_disable_level) /* disable multitasking */
#endif
        /*
         *  i8259 Management
         */

.i8259:
        /* Do not disable any 8259 interrupts if this isn't from one */
	cmp       ecx, 16               /* is this a PIC IRQ? */
	jge       .end_of_i8259

	/*
	 * acknowledge the interrupt
	 */
	movw      SYM (i8259s_cache), ax /* fetch current i8259 interrupt mask */

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
.end_of_i8259:

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

	movl      ARG_OFF(esp), ecx     /* grab vector arg from stack */

	/*
	 * Restore stack. This moves back to the task stack
	 * when all interrupts are unnested.
	 */
	movl      ebp, esp

	/*
	 * restore the original i8259 masks
	 */
        /* Do not touch 8259 interrupts if this isn't from one */
	cmp       ecx, 16               /* is this a PIC IRQ? */
	jge       .dont_restore_i8259

	movl      MSK_OFF(esp), eax
	movw      ax, SYM (i8259s_cache)
	outb	  $PIC_MASTER_IMR_IO_PORT
	movb	  ah, al
	outb	  $PIC_SLAVE_IMR_IO_PORT
.dont_restore_i8259:


#if defined(RTEMS_SMP) && defined(BSP_HAS_SMP)
        call    SYM(_ISR_SMP_Exit)
	testl	eax, eax
	je	.exit
#else
	decl      PER_CPU_ISR_NEST_LEVEL(ebx)  /* one less ISR nest level */
	                                    /* If interrupts are nested, */
	                                    /*   then dispatching is disabled */

	decl      SYM (_Thread_Dispatch_disable_level)
	                                    /* unnest multitasking */
	                                    /* Is dispatch disabled */
	jne       .exit                     /* Yes, then exit */

	cmpb      $0, PER_CPU_DISPATCH_NEEDED(ebx)
	                                    /* Is task switch necessary? */
	jne       .schedule                 /* Yes, then call the scheduler */
	jmp       .exit                     /* No, exit */
#endif

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
