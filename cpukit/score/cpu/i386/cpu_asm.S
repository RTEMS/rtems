/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  This file contains all assembly code for the Intel i386 implementation
 *  of RTEMS.
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/asm.h>
#include <rtems/score/cpu.h>

#ifndef CPU_STACK_ALIGNMENT
#error "Missing header? CPU_STACK_ALIGNMENT not defined"
#endif

/*
 * Format of i386 Register structure
 */

.set REG_EFLAGS,  I386_CONTEXT_CONTROL_EFLAGS_OFFSET
.set REG_ESP,     I386_CONTEXT_CONTROL_ESP_OFFSET
.set REG_EBP,     I386_CONTEXT_CONTROL_EBP_OFFSET
.set REG_EBX,     I386_CONTEXT_CONTROL_EBX_OFFSET
.set REG_ESI,     I386_CONTEXT_CONTROL_ESI_OFFSET
.set REG_EDI,     I386_CONTEXT_CONTROL_EDI_OFFSET
.set REG_GS_0,    I386_CONTEXT_CONTROL_GS_0_OFFSET
.set REG_GS_1,    I386_CONTEXT_CONTROL_GS_1_OFFSET

        BEGIN_CODE

/*
 *  void _CPU_Context_switch( run_context, heir_context )
 *
 *  This routine performs a normal non-FP context.
 */

        .p2align  1
        PUBLIC (_CPU_Context_switch)
        PUBLIC (_CPU_Context_switch_no_return)

.set RUNCONTEXT_ARG,   4                   /* save context argument */
.set HEIRCONTEXT_ARG,  8                   /* restore context argument */

SYM (_CPU_Context_switch):
SYM (_CPU_Context_switch_no_return):
        movl      RUNCONTEXT_ARG(esp),eax  /* eax = running threads context */
        GET_SELF_CPU_CONTROL edx           /* edx has address for per_CPU information */
        movl      PER_CPU_ISR_DISPATCH_DISABLE(edx),ecx
        pushf                              /* push eflags */
        popl      REG_EFLAGS(eax)          /* save eflags */
        movl      esp,REG_ESP(eax)         /* save stack pointer */
        movl      ebp,REG_EBP(eax)         /* save base pointer */
        movl      ebx,REG_EBX(eax)         /* save ebx */
        movl      esi,REG_ESI(eax)         /* save source register */
        movl      edi,REG_EDI(eax)         /* save destination register */
        movl      ecx, I386_CONTEXT_CONTROL_ISR_DISPATCH_DISABLE(eax)

        movl      eax,ecx                  /* ecx = running threads context */
        movl      HEIRCONTEXT_ARG(esp),eax /* eax = heir threads context */

#ifdef RTEMS_SMP
      /*
       * The executing thread no longer executes on this processor.  Switch
       * the stack to the temporary interrupt stack of this processor.  Mark
       * the context of the executing thread as not executing.
       */
        leal      PER_CPU_INTERRUPT_FRAME_AREA + CPU_INTERRUPT_FRAME_SIZE(edx),esp
        movb      $0, I386_CONTEXT_CONTROL_IS_EXECUTING_OFFSET(ecx)

.L_check_is_executing:
        lock bts  $0,I386_CONTEXT_CONTROL_IS_EXECUTING_OFFSET(eax)  /* Indicator in carry flag */
        jc        .L_get_potential_new_heir
#endif

/* Start restoring context */
.L_restore:
        movl      I386_CONTEXT_CONTROL_ISR_DISPATCH_DISABLE(eax),ecx
        movl      ecx,PER_CPU_ISR_DISPATCH_DISABLE(edx)
        movl      REG_ESP(eax),esp         /* restore stack pointer */
        pushl     REG_EFLAGS(eax)          /* push eflags */
        popf                               /* restore eflags */
        movl      REG_EBP(eax),ebp         /* restore base pointer */
        movl      REG_EBX(eax),ebx         /* restore ebx */
        movl      REG_ESI(eax),esi         /* restore source register */
        movl      REG_EDI(eax),edi         /* restore destination register */
        GET_CPU_ID ecx
        movl      REG_GS_0(eax), edx       /* restore gs segment */
        movl      edx, _Global_descriptor_table+24(,ecx,8)
        movl      REG_GS_1(eax), edx
        movl      edx, _Global_descriptor_table+28(,ecx,8)
        leal      24(,ecx,8), edx
        movl      edx, gs
        ret

/*
 *  NOTE: May be unnecessary to reload some registers.
 */

/*
 *  void _CPU_Context_restore( new_context )
 *
 *  This routine performs a normal non-FP context.
 */

        PUBLIC (_CPU_Context_restore)

.set NEWCONTEXT_ARG,   4                   /* context to restore argument */

SYM (_CPU_Context_restore):
        movl      NEWCONTEXT_ARG(esp),eax  /* eax = running threads context */
        GET_SELF_CPU_CONTROL edx           /* edx has address for per_CPU information */
        jmp       .L_restore

#ifdef RTEMS_SMP

.L_get_potential_new_heir:

        /* We may have a new heir */

        /* Read the executing and heir */
        movl    PER_CPU_OFFSET_EXECUTING(edx),ebx
        movl    PER_CPU_OFFSET_HEIR(edx),esi

        /*
         * Update the executing only if necessary to avoid cache line
         * monopolization.
         */
        cmp     esi,ebx
        je      .L_check_is_executing

        /* Calculate the heir context pointer */
        addl    esi,eax
        subl    ebx,eax

        /* Update the executing */
        movl    esi,PER_CPU_OFFSET_EXECUTING(edx)

        jmp     .L_check_is_executing
#endif

/*void _CPU_Context_save_fp_context( &fp_context_ptr )
 *  void _CPU_Context_restore_fp_context( &fp_context_ptr )
 *
 *  This section is used to context switch an i80287, i80387,
 *  the built-in coprocessor or the i80486 or compatible.
 */

.set FPCONTEXT_ARG,   4                    /* FP context argument */

#ifndef __SSE__
        .p2align  1
        PUBLIC (_CPU_Context_save_fp)
SYM (_CPU_Context_save_fp):
        movl      FPCONTEXT_ARG(esp),eax   /* eax = &ptr to FP context area */
        movl      (eax),eax                /* eax = FP context area */
        fsave     (eax)                    /* save FP context */
        ret

        .p2align  1
        PUBLIC (_CPU_Context_restore_fp)
SYM (_CPU_Context_restore_fp):
        movl      FPCONTEXT_ARG(esp),eax   /* eax = &ptr to FP context area */
        movl      (eax),eax                /* eax = FP context area */
        frstor    (eax)                    /* restore FP context */
        ret
#endif

#ifdef __SSE__
#define SSE_OFF  16
#endif

        PUBLIC (_Exception_Handler)
SYM (_Exception_Handler):
	pusha                  /* Push general purpose registers    */
	pushl   $0             /* Null pointer to SSE area          */
	movl    esp,  ebp      /* Save original SP                  */
#ifndef __SSE__
	subl    $4,   esp      /* Reserve space for argument        */
                           /* Align stack (courtesy for C/gcc)  */
	andl    $ - CPU_STACK_ALIGNMENT, esp
#else
	subl    $512, esp      /* Space for SSE area                */
                           /* Align stack (courtesy for C/gcc)  */
	andl    $ - CPU_STACK_ALIGNMENT, esp
/* Doing fwait here will re-throw an already pending FP exception!
	fwait
 */
	fxsave  0(esp)
	fninit                 /* Clean-slate FPU                   */
	movl    $0x1f80, 0(ebp)
	ldmxcsr 0(ebp)         /* Clean-slate MXCSR                 */
	movl    esp, 0(ebp)    /* Store pointer to SSE area         */
	subl    $SSE_OFF, esp  /* Aligned space for argument        */
#endif
	movl    ebp, (esp)     /* Store argument                    */
	movl	_currentExcHandler, eax	   /* Call function stored in _currentExcHandler */
	call	* eax
#ifdef __SSE__
	fwait
	fxrstor 16(esp)
#endif
	movl    ebp,  esp      /* Restore original SP               */
	addl    $4,   esp      /* Skill pointer to SSE area         */
	popa                   /* Restore general purpose registers */
	addl	$8, esp        /* Skill vector number and faultCode */
	iret

#define DISTINCT_EXCEPTION_WITH_FAULTCODE_ENTRY(_vector) \
        .p2align 4                         ; \
        PUBLIC (rtems_exception_prologue_ ## _vector ) ; \
SYM (rtems_exception_prologue_ ## _vector ):             \
	pushl	$ _vector	; \
        jmp   SYM (_Exception_Handler) ;

#define DISTINCT_EXCEPTION_WITHOUT_FAULTCODE_ENTRY(_vector) \
        .p2align 4                         ; \
        PUBLIC (rtems_exception_prologue_ ## _vector ) ; \
SYM (rtems_exception_prologue_ ## _vector ):             \
	pushl	$ 0		; \
	pushl	$ _vector	; \
        jmp   SYM (_Exception_Handler) ;

/*
 * Divide Error
 */
DISTINCT_EXCEPTION_WITHOUT_FAULTCODE_ENTRY (0)
/*
 * Debug Exception
 */
DISTINCT_EXCEPTION_WITHOUT_FAULTCODE_ENTRY (1)
/*
 * NMI
 */
DISTINCT_EXCEPTION_WITHOUT_FAULTCODE_ENTRY (2)
/*
 * Breakpoint
 */
DISTINCT_EXCEPTION_WITHOUT_FAULTCODE_ENTRY (3)
/*
 * Overflow
 */
DISTINCT_EXCEPTION_WITHOUT_FAULTCODE_ENTRY (4)
/*
 * Bound Range Exceeded
 */
DISTINCT_EXCEPTION_WITHOUT_FAULTCODE_ENTRY (5)
/*
 * Invalid Opcode
 */
DISTINCT_EXCEPTION_WITHOUT_FAULTCODE_ENTRY (6)
/*
 * No Math Coproc
 */
DISTINCT_EXCEPTION_WITHOUT_FAULTCODE_ENTRY (7)
/*
 * Double Fault
 */
DISTINCT_EXCEPTION_WITH_FAULTCODE_ENTRY (8)
/*
 * Coprocessor segment overrun
 */
DISTINCT_EXCEPTION_WITHOUT_FAULTCODE_ENTRY (9)
/*
 * Invalid TSS
 */
DISTINCT_EXCEPTION_WITH_FAULTCODE_ENTRY (10)
/*
 * Segment Not Present
 */
DISTINCT_EXCEPTION_WITH_FAULTCODE_ENTRY (11)
/*
 * Stack segment Fault
 */
DISTINCT_EXCEPTION_WITH_FAULTCODE_ENTRY (12)
/*
 * General Protection Fault
 */
DISTINCT_EXCEPTION_WITH_FAULTCODE_ENTRY (13)
/*
 * Page Fault
 */
DISTINCT_EXCEPTION_WITH_FAULTCODE_ENTRY (14)
/*
 * Floating point error (NB 15 is reserved it is therefor skipped)
 */
DISTINCT_EXCEPTION_WITHOUT_FAULTCODE_ENTRY (16)
/*
 * Aligment Check
 */
DISTINCT_EXCEPTION_WITH_FAULTCODE_ENTRY (17)
/*
 * Machine Check
 */
DISTINCT_EXCEPTION_WITH_FAULTCODE_ENTRY (18)

#ifdef __SSE__
/*
 * SIMD FP Exception
 */
DISTINCT_EXCEPTION_WITHOUT_FAULTCODE_ENTRY (19)
#endif


/*
 *  void *i386_Logical_to_physical(
 *     uint16_t    segment,
 *     void             *address
 *  );
 *
 *  Returns thirty-two bit physical address for segment:address.
 */

.set SEGMENT_ARG, 4
.set ADDRESS_ARG, 8

             PUBLIC (i386_Logical_to_physical)

SYM (i386_Logical_to_physical):

        xorl    eax,eax                /* clear eax */
        movzwl  SEGMENT_ARG(esp),ecx   /* ecx = segment value */
        movl    $ SYM (_Global_descriptor_table),edx
                                       /* edx = address of our GDT */
        addl    ecx,edx                /* edx = address of desired entry */
        movb    7(edx),ah              /* ah = base 31:24 */
        movb    4(edx),al              /* al = base 23:16 */
        shll    $16,eax                /* move ax into correct bits */
        movw    2(edx),ax              /* ax = base 0:15 */
        movl    ADDRESS_ARG(esp),ecx   /* ecx = address to convert */
        addl    eax,ecx                /* ecx = physical address equivalent */
        movl    ecx,eax                /* eax = ecx */
        ret

/*
 *  void *i386_Physical_to_logical(
 *     uint16_t    segment,
 *     void             *address
 *  );
 *
 *  Returns thirty-two bit physical address for segment:address.
 */

/*
 *.set SEGMENT_ARG, 4
 *.set ADDRESS_ARG, 8   -- use sets from above
 */

       PUBLIC (i386_Physical_to_logical)

SYM (i386_Physical_to_logical):
        xorl    eax,eax                /* clear eax */
        movzwl  SEGMENT_ARG(esp),ecx   /* ecx = segment value */
        movl    $ SYM (_Global_descriptor_table),edx
                                       /* edx = address of our GDT */
        addl    ecx,edx                /* edx = address of desired entry */
        movb    7(edx),ah              /* ah = base 31:24 */
        movb    4(edx),al              /* al = base 23:16 */
        shll    $16,eax                /* move ax into correct bits */
        movw    2(edx),ax              /* ax = base 0:15 */
        movl    ADDRESS_ARG(esp),ecx   /* ecx = address to convert */
        subl    eax,ecx                /* ecx = logical address equivalent */
        movl    ecx,eax                /* eax = ecx */
        ret

/*
 *  int i386_Physical_to_real(
 *     void     *address,
 *     uint16_t *segment,
 *     uint16_t *offset
 *  );
 *
 *  Fills segment:offest real mode pointer counted from thirty-two bit physical
 *  address.
 *  Returns 0 if inconvertible, 1 if successfuly converted.
 */

.set PHYS_PTR_ARG,   4
.set RM_PTR_SEG_ARG, 8
.set RM_PTR_OFF_ARG, 12

       PUBLIC (i386_Physical_to_real)

SYM (i386_Physical_to_real):
        movl    PHYS_PTR_ARG(esp),eax
        cmpl    $0x10FFF0, eax
        js      1f
        movl    $0, eax
        ret
1:  	cmpl    $0x100000, eax
        js      2f
        subl    $0xFFFF0, eax
        movl    RM_PTR_OFF_ARG(esp), ecx
        movw    ax, (ecx)
        movl    RM_PTR_SEG_ARG(esp), ecx
        movw    $0xFFFF, (ecx)
        movl    $1, eax
        ret
2: 	movl    eax, edx
        and     $0xF, ax
        movl    RM_PTR_OFF_ARG(esp), ecx
        movw    ax, (ecx)
        shrl    $4, edx
        movl    RM_PTR_SEG_ARG(esp), ecx
        movw    dx, (ecx)
        movl    $1, eax
        ret

END_CODE

END
