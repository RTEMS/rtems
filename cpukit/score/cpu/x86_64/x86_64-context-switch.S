/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2024 Matheus Pecoraro
 * Copyright (c) 2018 Amaan Cheval <amaan.cheval@gmail.com>
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/asm.h>
#include <rtems/score/cpu.h>

#ifndef CPU_STACK_ALIGNMENT
#error "Missing header? CPU_STACK_ALIGNMENT not defined"
#endif

BEGIN_CODE

/*
 *  void _CPU_Context_switch( run_context, heir_context )
 *
 *  This routine performs a normal non-FP context.
 */

.p2align  1
PUBLIC(_CPU_Context_switch)
PUBLIC(_CPU_Context_switch_no_return)

/* save context argument */
.set RUNCONTEXT_ARG,   REG_ARG0
/* restore context argument */
.set HEIRCONTEXT_ARG,  REG_ARG1

SYM(_CPU_Context_switch):
SYM(_CPU_Context_switch_no_return):
  movq    RUNCONTEXT_ARG, r10   /* r10 = running threads context */
  GET_SELF_CPU_CONTROL_R11      /* r11 = per CPU information */

  /* Fill up Context_Control struct */
  pushf
  popq               CPU_CONTEXT_CONTROL_EFLAGS(r10) /* pop rflags into context */
  movq    rbx,       CPU_CONTEXT_CONTROL_RBX(r10)
  movq    rsp,       CPU_CONTEXT_CONTROL_RSP(r10)
  movq    rbp,       CPU_CONTEXT_CONTROL_RBP(r10)
  movq    r12,       CPU_CONTEXT_CONTROL_R12(r10)
  movq    r13,       CPU_CONTEXT_CONTROL_R13(r10)
  movq    r14,       CPU_CONTEXT_CONTROL_R14(r10)
  movq    r15,       CPU_CONTEXT_CONTROL_R15(r10)

  movl    PER_CPU_ISR_DISPATCH_DISABLE(r11), %edx
  movl    %edx, CPU_CONTEXT_CONTROL_ISR_DISPATCH_DISABLE(r10)

  movq    r10, r8              /* r8  = running threads context */
  movq    HEIRCONTEXT_ARG, r10 /* r10 = heir threads context */

#ifdef RTEMS_SMP
  /*
   * The executing thread no longer executes on this processor.  Switch
   * the stack to the temporary interrupt stack of this processor.  Mark
   * the context of the executing thread as not executing.
   */
  leaq    PER_CPU_INTERRUPT_FRAME_AREA + CPU_INTERRUPT_FRAME_SIZE(r11), rsp
  movw    $0, CPU_CONTEXT_CONTROL_IS_EXECUTING(r8)

.check_is_executing:
  lock btsw $0, CPU_CONTEXT_CONTROL_IS_EXECUTING(r10) /* Indicator in carry flag */
  jnc       .restore

.get_potential_new_heir:
  /* We may have a new heir */

  /* Read the executing and heir */
  movq    PER_CPU_OFFSET_EXECUTING(r11), r8
  movq    PER_CPU_OFFSET_HEIR(r11), r9

  /*
   * Update the executing only if necessary to avoid cache line
   * monopolization.
   */
  cmpq    r8, r9
  je      .check_is_executing

  /* Calculate the heir context pointer */
  addq    r9, r10
  subq    r8, r10

  /* Update the executing */
  movq    r9, PER_CPU_OFFSET_EXECUTING(r11)

  jmp     .check_is_executing
#endif

.restore:
  movl  CPU_CONTEXT_CONTROL_ISR_DISPATCH_DISABLE(r10), %edx
  movl  %edx, PER_CPU_ISR_DISPATCH_DISABLE(r11)

  movq  CPU_CONTEXT_CONTROL_RBX(r10), rbx
  movq  CPU_CONTEXT_CONTROL_RSP(r10), rsp

  /*
   * We need to load rflags after rsp to avoid an interrupt while the ISR stack
   * is still being used during the initialization process
   */
  pushq CPU_CONTEXT_CONTROL_EFLAGS(r10)       /* push rflags */
  popf                                        /* restore rflags */

  movq  CPU_CONTEXT_CONTROL_RBP(r10), rbp
  movq  CPU_CONTEXT_CONTROL_R12(r10), r12
  movq  CPU_CONTEXT_CONTROL_R13(r10), r13
  movq  CPU_CONTEXT_CONTROL_R14(r10), r14
  movq  CPU_CONTEXT_CONTROL_R15(r10), r15

  movq CPU_CONTEXT_CONTROL_FS(r10), rax
  /* High bits in %edx and low bits in %eax */
  movq rax, rdx
  shrq $32, rdx
  movl $FSBASE_MSR, %ecx
  wrmsr

  ret

/*
 *  void _CPU_Context_restore( new_context )
 *
 *  This routine performs a normal non-FP context restore.
 */

PUBLIC(_CPU_Context_restore)

.set NEWCONTEXT_ARG,   REG_ARG0       /* context to restore argument */

SYM(_CPU_Context_restore):
  movq      NEWCONTEXT_ARG, r10  /* r10 = running threads context */
  GET_SELF_CPU_CONTROL_R11       /* r11 = per CPU information */
  jmp       .restore

END_CODE
END
