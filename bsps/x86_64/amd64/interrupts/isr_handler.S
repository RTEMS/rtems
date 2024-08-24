/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsX8664AMD64
 *
 * @brief Contains the _ISR_Handler that acts as the common handler for all
 * vectors to be managed by the RTEMS interrupt manager.
 */

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

#include <rtems/asm.h>
#include <rtems/score/cpu.h>
#include <rtems/score/percpu.h>

#ifndef CPU_STACK_ALIGNMENT
#error "Missing header? CPU_STACK_ALIGNMENT not defined"
#endif

BEGIN_CODE

PUBLIC(apic_spurious_handler)
SYM(apic_spurious_handler):
  iretq

/*
 * These are callee-saved registers, which means we can use them in our
 * interrupts as persistent scratch registers (i.e. calls will not destroy
 * them), as long as we also save and restore it for the interrupted task.
 */
.set SCRATCH_REG0,    rbp
.set SCRATCH_REG1,    rbx

/*
 * We need to set a distinct handler for every interrupt vector so that
 * we can pass the vector number to _ISR_Handler correctly.
 */
#define DISTINCT_INTERRUPT_ENTRY(vector)   \
  .p2align 4                             ; \
  PUBLIC(rtems_irq_prologue_ ## vector)  ; \
SYM(rtems_irq_prologue_ ## vector):      ; \
  pushq REG_ARG0                         ; \
  movq  $vector, REG_ARG0                ; \
  pushq SCRATCH_REG0                     ; \
  pushq SCRATCH_REG1                     ; \
  jmp   SYM(_ISR_Handler)

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
DISTINCT_INTERRUPT_ENTRY(17)
DISTINCT_INTERRUPT_ENTRY(18)
DISTINCT_INTERRUPT_ENTRY(19)
DISTINCT_INTERRUPT_ENTRY(20)
DISTINCT_INTERRUPT_ENTRY(21)
DISTINCT_INTERRUPT_ENTRY(22)
DISTINCT_INTERRUPT_ENTRY(23)
DISTINCT_INTERRUPT_ENTRY(24)
DISTINCT_INTERRUPT_ENTRY(25)
DISTINCT_INTERRUPT_ENTRY(26)
DISTINCT_INTERRUPT_ENTRY(27)
DISTINCT_INTERRUPT_ENTRY(28)
DISTINCT_INTERRUPT_ENTRY(29)
DISTINCT_INTERRUPT_ENTRY(30)
DISTINCT_INTERRUPT_ENTRY(31)
DISTINCT_INTERRUPT_ENTRY(32)
DISTINCT_INTERRUPT_ENTRY(33)

SYM(_ISR_Handler):
.save_cpu_interrupt_frame:
.set SAVED_RSP, SCRATCH_REG0
  movq rsp, SAVED_RSP

  /* Make space for CPU_Interrupt_frame */
  subq $CPU_INTERRUPT_FRAME_CALLER_SAVED_SIZE, rsp
.set ALIGNMENT_MASK, ~(CPU_STACK_ALIGNMENT - 1)
  andq $ALIGNMENT_MASK, rsp
  /* XXX: Save interrupt mask? (See #5122) */

  /* Save x87 FPU, MMX and SSE state */
  fwait
  fxsave64 (CPU_INTERRUPT_FRAME_SSE_STATE)(rsp)
  /* Reset to a clean state */
  fninit
  /* Use CPU_INTERRUPT_FRAME_RAX as scratch space */
  movl $0x1F80, (CPU_INTERRUPT_FRAME_RAX)(rsp)
  ldmxcsr (CPU_INTERRUPT_FRAME_RAX)(rsp)

  /* Save caller-saved registers to CPU_Interrupt_frame */
  movq rax,         (CPU_INTERRUPT_FRAME_RAX)(rsp)
  movq rcx,         (CPU_INTERRUPT_FRAME_RCX)(rsp)
  movq rdx,         (CPU_INTERRUPT_FRAME_RDX)(rsp)
  movq rsi,         (CPU_INTERRUPT_FRAME_RSI)(rsp)
  movq r8,          (CPU_INTERRUPT_FRAME_R8)(rsp)
  movq r9,          (CPU_INTERRUPT_FRAME_R9)(rsp)
  movq r10,         (CPU_INTERRUPT_FRAME_R10)(rsp)
  movq r11,         (CPU_INTERRUPT_FRAME_R11)(rsp)

  /* Save the initial rsp */
  movq SAVED_RSP,   (CPU_INTERRUPT_FRAME_RSP)(rsp)

.switch_stack_if_needed:
  /* Save current aligned rsp so we can find CPU_Interrupt_frame again later */
  movq rsp, SAVED_RSP

  /*
   * Switch to interrupt stack if necessary; it's necessary if this is the
   * outermost interrupt, which means we've been using the task's stack so far
   */

.set Per_CPU_Info, SCRATCH_REG1
  GET_SELF_CPU_CONTROL_RBX /* SCRATCH_REG1 == rbx */
  cmpl $0, PER_CPU_ISR_NEST_LEVEL(Per_CPU_Info)
  jne  .skip_switch
.switch_stack:
  movq PER_CPU_INTERRUPT_STACK_HIGH(Per_CPU_Info), rsp
.skip_switch:
  incl PER_CPU_ISR_NEST_LEVEL(Per_CPU_Info)
  incl PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL(Per_CPU_Info)

.call_isr_dispatch:
  /* REG_ARG0 already includes the vector number, so we can simply call */
  call amd64_dispatch_isr

.restore_stack:
  /* If this is the outermost stack, this restores the task stack */
  movq SAVED_RSP, rsp

  decl PER_CPU_ISR_NEST_LEVEL(Per_CPU_Info)
  decl PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL(Per_CPU_Info)
  movl PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL(Per_CPU_Info), %eax
  orl PER_CPU_ISR_DISPATCH_DISABLE(Per_CPU_Info), %eax
  /**
   * If either thread dispatch disable level or ISR dispatch disable
   * are non-zero skip scheduling the dispatch
   */
  cmpl $0, %eax
  jne  .restore_cpu_interrupt_frame
  /* Only call _Thread_Do_dispatch if dispatch needed is not 0 */
  cmpb $0, PER_CPU_DISPATCH_NEEDED(Per_CPU_Info)
  je  .restore_cpu_interrupt_frame

.schedule_dispatch:
  /* Set ISR dispatch disable and thread dispatch disable level to one */
  movl $1, PER_CPU_ISR_DISPATCH_DISABLE(Per_CPU_Info)
  movl $1, PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL(Per_CPU_Info)

  /* Call Thread_Do_dispatch(), this function will enable interrupts */
  movq Per_CPU_Info, REG_ARG0
  movq $CPU_ISR_LEVEL_ENABLED, REG_ARG1 /* Set interrupt flag manually */
  call _Thread_Do_dispatch

  /* Disable interrupts */
  cli

  /**
   * It is possible that after returning from _Thread_Do_dispatch the
   * Per_CPU_Info has changed
   */
  GET_SELF_CPU_CONTROL_RBX /* Per_CPU_Info == SCRATCH_REG1 == rbx */
  cmpb $0, PER_CPU_DISPATCH_NEEDED(Per_CPU_Info)
  jne  .schedule_dispatch

  /* Done with thread dispatching */
  movl $0, PER_CPU_ISR_DISPATCH_DISABLE(Per_CPU_Info)

.restore_cpu_interrupt_frame:
  /* Restore x87 FPU, MMX and SSE state */
  fwait
  fxrstor64 (CPU_INTERRUPT_FRAME_SSE_STATE)(rsp)

  /* Restore registers from CPU_Interrupt_frame */
  movq (CPU_INTERRUPT_FRAME_RAX)(rsp), rax
  movq (CPU_INTERRUPT_FRAME_RCX)(rsp), rcx
  movq (CPU_INTERRUPT_FRAME_RDX)(rsp), rdx
  movq (CPU_INTERRUPT_FRAME_RSI)(rsp), rsi
  movq (CPU_INTERRUPT_FRAME_R8)(rsp), r8
  movq (CPU_INTERRUPT_FRAME_R9)(rsp), r9
  movq (CPU_INTERRUPT_FRAME_R10)(rsp), r10
  movq (CPU_INTERRUPT_FRAME_R11)(rsp), r11

  /* Restore the rsp value from just before _ISR_Handler was called */
  movq (CPU_INTERRUPT_FRAME_RSP)(rsp), SAVED_RSP
  movq SAVED_RSP, rsp

  /* Restore args DISTINCT_INTERRUPT_ENTRY pushed to task stack */
  popq SCRATCH_REG1
  popq SCRATCH_REG0
  popq REG_ARG0
  iretq

END_CODE

END
