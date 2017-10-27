/**
 * @file
 *
 * @ingroup ScoreCPU
 *
 * @brief RISC-V exception support implementation.
 */

/*
 * Copyright (c) 2015 University of York.
 * Hesham Almatary <hesham@alumni.york.ac.uk>
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

#include <rtems/score/cpu.h>

#include <rtems/asm.h>
#include <rtems/score/percpu.h>

EXTERN(bsp_start_vector_table_begin)
EXTERN(_Thread_Dispatch)
PUBLIC(ISR_Handler)

.section .text, "ax"
.align 4
TYPE_FUNC(ISR_Handler)
SYM(ISR_Handler):
  addi sp, sp, -1 * 36 * CPU_SIZEOF_POINTER

  SREG x1, (1 * CPU_SIZEOF_POINTER)(sp)
  /* Skip x2/sp */
  SREG x3, (3 * CPU_SIZEOF_POINTER)(sp)
  SREG x4, (4 * CPU_SIZEOF_POINTER)(sp)
  SREG x5, (5 * CPU_SIZEOF_POINTER)(sp)
  SREG x6, (6 * CPU_SIZEOF_POINTER)(sp)
  SREG x7, (7 * CPU_SIZEOF_POINTER)(sp)
  SREG x8, (8 * CPU_SIZEOF_POINTER)(sp)
  SREG x9, (9 * CPU_SIZEOF_POINTER)(sp)
  SREG x10, (10 * CPU_SIZEOF_POINTER)(sp)
  SREG x11, (11 * CPU_SIZEOF_POINTER)(sp)
  SREG x12, (12 * CPU_SIZEOF_POINTER)(sp)
  SREG x13, (13 * CPU_SIZEOF_POINTER)(sp)
  SREG x14, (14 * CPU_SIZEOF_POINTER)(sp)
  SREG x15, (15 * CPU_SIZEOF_POINTER)(sp)
  SREG x16, (16 * CPU_SIZEOF_POINTER)(sp)
  SREG x17, (17 * CPU_SIZEOF_POINTER)(sp)
  SREG x18, (18 * CPU_SIZEOF_POINTER)(sp)
  SREG x19, (19 * CPU_SIZEOF_POINTER)(sp)
  SREG x20, (20 * CPU_SIZEOF_POINTER)(sp)
  SREG x21, (21 * CPU_SIZEOF_POINTER)(sp)
  SREG x22, (22 * CPU_SIZEOF_POINTER)(sp)
  SREG x23, (23 * CPU_SIZEOF_POINTER)(sp)
  SREG x24, (24 * CPU_SIZEOF_POINTER)(sp)
  SREG x25, (25 * CPU_SIZEOF_POINTER)(sp)
  SREG x26, (26 * CPU_SIZEOF_POINTER)(sp)
  SREG x27, (27 * CPU_SIZEOF_POINTER)(sp)
  SREG x28, (28 * CPU_SIZEOF_POINTER)(sp)
  SREG x29, (28 * CPU_SIZEOF_POINTER)(sp)
  SREG x30, (30 * CPU_SIZEOF_POINTER)(sp)
  SREG x31, (31 * CPU_SIZEOF_POINTER)(sp)

  /* Exception level related registers */
  csrr a0, mstatus
  SREG a0, (32 * CPU_SIZEOF_POINTER)(sp)
  csrr a0, mcause
  SREG a0, (33 * CPU_SIZEOF_POINTER)(sp)
  csrr a1, mepc
  SREG a1, (34 * CPU_SIZEOF_POINTER)(sp)

  /* FIXME Only handle interrupts for now (MSB = 1) */
  andi a0, a0, 0xf

  /* Increment nesting level */
  la t0, ISR_NEST_LEVEL

  /* Disable multitasking */
  la t1, THREAD_DISPATCH_DISABLE_LEVEL

  lw t2, (t0)
  lw t3, (t1)
  addi t2, t2, 1
  addi t3, t3, 1
  sw t2, (t0)
  sw t3, (t1)

  /* Save interrupted task stack pointer */
  addi t4, sp, 36 * CPU_SIZEOF_POINTER
  SREG t4, (2 * CPU_SIZEOF_POINTER)(sp)

  /* Keep sp (Exception frame address) in s1 */
  mv   s1, sp

  /* Call the exception handler from vector table */

  /* First function arg for C handler is vector number,
   * and the second is a pointer to exception frame.
   * a0/mcause/vector number is already loaded above */
  mv a1, sp

  /* calculate the offset */
  la   t5, bsp_start_vector_table_begin
#if __riscv_xlen == 32
  slli t6, a0, 2
#else /* xlen = 64 */
  slli t6, a0, 3
#endif
  add  t5, t5, t6
  LREG t5, (t5)

  /* Do not switch stacks if we are in a nested interrupt. At
   * this point t2 should be holding ISR_NEST_LEVEL value.
   */
  li   s0, 1
  bgtu t2, s0, jump_to_c_handler

  /* Switch to RTEMS dedicated interrupt stack */
  la     sp, INTERRUPT_STACK_HIGH
  LREG   sp, (sp)

jump_to_c_handler:
  jalr t5

  /* Switch back to the interrupted task stack */
  mv sp, s1

  /* Decrement nesting level */
  la t0, ISR_NEST_LEVEL

  /* Enable multitasking */
  la t1, THREAD_DISPATCH_DISABLE_LEVEL

  Lw t2, (t0)
  lw t3, (t1)
  addi t2, t2, -1
  addi t3, t3, -1
  sw t2, (t0)
  sw t3, (t1)

  /* Check if _ISR_Nest_level > 0 */
  bgtz t2, exception_frame_restore

  /* Check if _Thread_Dispatch_disable_level > 0 */
  bgtz t3, exception_frame_restore

  /* Check if dispatch needed */
  la   x31, DISPATCH_NEEDED
  lw x31, (x31)
  beqz x31, exception_frame_restore

  la x31, _Thread_Dispatch
  jalr x31

  SYM(exception_frame_restore):
  LREG x1, (1 * CPU_SIZEOF_POINTER)(sp)
  /* Skip sp/x2 */
  LREG x3, (3 * CPU_SIZEOF_POINTER)(sp)
  LREG x4, (4 * CPU_SIZEOF_POINTER)(sp)
  LREG x5, (5 * CPU_SIZEOF_POINTER)(sp)
  LREG x6, (6 * CPU_SIZEOF_POINTER)(sp)
  LREG x7, (7 * CPU_SIZEOF_POINTER)(sp)
  LREG x8, (8 * CPU_SIZEOF_POINTER)(sp)
  LREG x9, (9 * CPU_SIZEOF_POINTER)(sp)
  LREG x10, (10 * CPU_SIZEOF_POINTER)(sp)
  LREG x11, (11 * CPU_SIZEOF_POINTER)(sp)
  LREG x12, (12 * CPU_SIZEOF_POINTER)(sp)
  LREG x13, (13 * CPU_SIZEOF_POINTER)(sp)
  LREG x14, (14 * CPU_SIZEOF_POINTER)(sp)
  LREG x15, (15 * CPU_SIZEOF_POINTER)(sp)
  LREG x16, (16 * CPU_SIZEOF_POINTER)(sp)
  LREG x17, (17 * CPU_SIZEOF_POINTER)(sp)
  LREG x18, (18 * CPU_SIZEOF_POINTER)(sp)
  LREG x19, (19 * CPU_SIZEOF_POINTER)(sp)
  LREG x20, (20 * CPU_SIZEOF_POINTER)(sp)
  LREG x21, (21 * CPU_SIZEOF_POINTER)(sp)
  LREG x22, (22 * CPU_SIZEOF_POINTER)(sp)
  LREG x23, (23 * CPU_SIZEOF_POINTER)(sp)
  LREG x24, (24 * CPU_SIZEOF_POINTER)(sp)
  LREG x25, (25 * CPU_SIZEOF_POINTER)(sp)
  LREG x26, (26 * CPU_SIZEOF_POINTER)(sp)
  LREG x27, (27 * CPU_SIZEOF_POINTER)(sp)
  LREG x28, (28 * CPU_SIZEOF_POINTER)(sp)
  LREG x29, (29 * CPU_SIZEOF_POINTER)(sp)
  LREG x30, (30 * CPU_SIZEOF_POINTER)(sp)

  /* Load mstatus */
  LREG x31, (32 * CPU_SIZEOF_POINTER)(sp)
  csrw mstatus, x31
  /* Load mepc */
  LREG x31, (34 * CPU_SIZEOF_POINTER)(sp)
  csrw mepc, x31

  LREG x31, (31 * CPU_SIZEOF_POINTER)(sp)

  /* Unwind exception frame */
  addi sp, sp, 36 * CPU_SIZEOF_POINTER

  mret
