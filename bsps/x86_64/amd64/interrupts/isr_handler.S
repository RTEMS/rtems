/*
 * This file contains the _ISR_Handler that acts as the common handler for all
 * vectors to be managed by the RTEMS interrupt manager.
 */

/*
 * Copyright (c) 2018.
 * Amaan Cheval <amaan.cheval@gmail.com>
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
.set SCRATCH_REG0, rbp
.set SCRATCH_REG1, rbx

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

SYM(_ISR_Handler):
.save_cpu_interrupt_frame:
.set SAVED_RSP, SCRATCH_REG0
  movq rsp, SAVED_RSP

  /* Make space for CPU_Interrupt_frame */
  subq $CPU_INTERRUPT_FRAME_SIZE, rsp
.set ALIGNMENT_MASK, ~(CPU_STACK_ALIGNMENT - 1)
  andq $ALIGNMENT_MASK, rsp
  // XXX: Save interrupt mask?

  /* Save caller-saved registers to CPU_Interrupt_frame */
  movq rax,         (8 * CPU_SIZEOF_POINTER)(rsp)
  movq rcx,         (7 * CPU_SIZEOF_POINTER)(rsp)
  movq rdx,         (6 * CPU_SIZEOF_POINTER)(rsp)
  movq rsi,         (5 * CPU_SIZEOF_POINTER)(rsp)
  movq r8,          (4 * CPU_SIZEOF_POINTER)(rsp)
  movq r9,          (3 * CPU_SIZEOF_POINTER)(rsp)
  movq r10,         (2 * CPU_SIZEOF_POINTER)(rsp)
  movq r11,         (1 * CPU_SIZEOF_POINTER)(rsp)

  /* Save the initial rsp */
  movq SAVED_RSP,   (0 * CPU_SIZEOF_POINTER)(rsp)

.switch_stack_if_needed:
  /* Save current aligned rsp so we can find CPU_Interrupt_frame again later */
  movq rsp, SAVED_RSP

  /*
   * Switch to interrupt stack if necessary; it's necessary if this is the
   * outermost interrupt, which means we've been using the task's stack so far
   */

#ifdef RTEMS_SMP
  /* XXX: We should call _CPU_SMP_Get_current_processor here */
#endif
.set Per_CPU_Info, SCRATCH_REG1
  movq $SYM(_Per_CPU_Information), Per_CPU_Info
  cmpq $0, PER_CPU_ISR_NEST_LEVEL(Per_CPU_Info)
  jne  .skip_switch
.switch_stack:
  movq PER_CPU_INTERRUPT_STACK_HIGH(Per_CPU_Info), rsp
.skip_switch:
  incq PER_CPU_ISR_NEST_LEVEL(Per_CPU_Info)
  incq PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL(Per_CPU_Info)

.call_isr_dispatch:
  /* REG_ARG0 already includes the vector number, so we can simply call */
  call amd64_dispatch_isr

.restore_stack:
  /* If this is the outermost stack, this restores the task stack */
  movq SAVED_RSP, rsp

  decq PER_CPU_ISR_NEST_LEVEL(Per_CPU_Info)
  decq PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL(Per_CPU_Info)
  /* XXX: Bug in QEMU causing ZF to not be set by decq necessitating the cmpb */
  cmpb $0, PER_CPU_THREAD_DISPATCH_DISABLE_LEVEL(Per_CPU_Info)
  /* If dispatch is non-zero, it is disabled, so skip scheduling it */
  jne  .restore_cpu_interrupt_frame

.schedule_dispatch:
  cmpb $0, PER_CPU_DISPATCH_NEEDED(Per_CPU_Info)
  je   .restore_cpu_interrupt_frame
  call _Thread_Dispatch

.restore_cpu_interrupt_frame:
  /* Restore registers from CPU_Interrupt_frame */
  movq (8 * CPU_SIZEOF_POINTER)(rsp), rax
  movq (7 * CPU_SIZEOF_POINTER)(rsp), rcx
  movq (6 * CPU_SIZEOF_POINTER)(rsp), rdx
  movq (5 * CPU_SIZEOF_POINTER)(rsp), rsi
  movq (4 * CPU_SIZEOF_POINTER)(rsp), r8
  movq (3 * CPU_SIZEOF_POINTER)(rsp), r9
  movq (2 * CPU_SIZEOF_POINTER)(rsp), r10
  movq (1 * CPU_SIZEOF_POINTER)(rsp), r11

  /* Restore the rsp value from just before _ISR_Handler was called */
  movq (0 * CPU_SIZEOF_POINTER)(rsp), SAVED_RSP
  movq SAVED_RSP, rsp

  /* Restore args DISTINCT_INTERRUPT_ENTRY pushed to task stack */
  popq SCRATCH_REG1
  popq SCRATCH_REG0
  popq REG_ARG0
  iretq

END_CODE

END
