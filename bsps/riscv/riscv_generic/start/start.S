/*
 * Copyright (c) 2015 University of York.
 * Hesham Almatary <hesham@alumni.york.ac.uk>
 *
 * Copyright (c) 2013, The Regents of the University of California (Regents).
 * All Rights Reserved.
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
#include <bsp/linker-symbols.h>
#include <rtems/score/riscv-utility.h>
#include <rtems/score/cpu.h>
#include <rtems/asm.h>

EXTERN(bsp_section_bss_begin)
EXTERN(bsp_section_bss_end)
EXTERN(ISR_Handler)
EXTERN(bsp_start_vector_table_size)
EXTERN(bsp_vector_table_size)
EXTERN(bsp_section_stack_begin)

PUBLIC(bsp_start_vector_table_begin)
PUBLIC(bsp_start_vector_table_end)
PUBLIC(_start)

.section .start, "wax"
TYPE_FUNC(_start)
SYM(_start):
  li x2, 0
  li x3, 0
  li x4, 0
  li x5, 0
  li x6, 0
  li x7, 0
  li x8, 0
  li x9, 0
  li x10, 0
  li x11, 0
  li x12, 0
  li x13, 0
  li x14, 0
  li x15, 0
  li x16, 0
  li x17, 0
  li x18, 0
  li x19, 0
  li x20, 0
  li x21, 0
  li x22, 0
  li x23, 0
  li x24, 0
  li x25, 0
  li x26, 0
  li x27, 0
  li x28, 0
  li x29, 0
  li x30, 0
  li x31, 0

  la t0, ISR_Handler
  csrw mtvec, t0

  /* load stack and frame pointers */
  la sp, bsp_section_stack_begin

  /* Clearing .bss */
  la t0, bsp_section_bss_begin
  la t1, bsp_section_bss_end

_loop_clear_bss:
  bge   t0, t1, _end_clear_bss
  SREG    x0, 0(t0)
  addi  t0, t0, CPU_SIZEOF_POINTER
  j     _loop_clear_bss
_end_clear_bss:

  /* Init FPU unit if it's there */
  li t0, MSTATUS_FS
  csrs mstatus, t0

  j boot_card

  .align 4
bsp_start_vector_table_begin:
  .word _RISCV_Exception_default /* User int */
  .word _RISCV_Exception_default /* Supervisor int */
  .word _RISCV_Exception_default /* Reserved */
  .word _RISCV_Exception_default /* Machine int */
  .word _RISCV_Exception_default /* User timer int */
  .word _RISCV_Exception_default /* Supervisor Timer int */
  .word _RISCV_Exception_default /* Reserved */
  .word _RISCV_Exception_default /* Machine Timer int */
  .word _RISCV_Exception_default /* User external int */
  .word _RISCV_Exception_default /* Supervisor external int */
  .word _RISCV_Exception_default /* Reserved */
  .word _RISCV_Exception_default /* Machine external int */
  .word _RISCV_Exception_default
  .word _RISCV_Exception_default
  .word _RISCV_Exception_default
  .word _RISCV_Exception_default
bsp_start_vector_table_end:
