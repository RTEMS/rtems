/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsX8664AMD64
 *
 * @brief Contains the trampoline code that will be executed by every
 * Application Processor when first started.
 */

/*
 * Copyright (C) 2024 Matheus Pecoraro
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

#include <gdt.h>
#include <smp.h>
#include <rtems/asm.h>
#include <rtems/score/percpu.h>

.set PM_GDT_CODE_OFFSET, 0x08       # Offset of code segment descriptor into GDT
.set PM_GDT_DATA_OFFSET, 0x10       # Offset of data segment descriptor into GDT
.set CR0_PE,             1          # Protected mode flag on CR0 register
.set CR0_PG,             0x80000000 # Paging flag on CR0 register
.set CR0_EM_BITMASK,     (~0x4)     # Bitmask for disabling x87 FPU Emulation bit
.set CR0_MP,             0x2        # Monitor co-processor flag
.set CR4_PAE,            0x20       # Physical Address Extension flag on CR4 register
.set CR4_OSFXSR,         0x200      # OS support for FXSAVE and FXRSTOR flag
.set CR4_OSXMMEXCPT,     0x400      # OS support for unmasked SIMD FP exceptions flag
.set CR4_SSEFLAGS,       (CR4_OSFXSR | CR4_OSXMMEXCPT)
.set EFER_MSR,           0xC0000080 # EFER MSR number
.set EFER_MSR_LME,       0x100      # Long Mode Enable flag on the EFER MSR

BEGIN_CODE

.code16
PUBLIC(_Trampoline_start)
SYM(_Trampoline_start):
  cli
  cld
  jmp .real_mode

.real_mode:
  lgdt (gdt_desc - _Trampoline_start) + TRAMPOLINE_ADDR

  # Enter protected mode
  movl %cr0, %eax
  orl $CR0_PE, %eax
  movl %eax, %cr0

  # Jump to protected mode
  ljmpl $PM_GDT_CODE_OFFSET, $((.protected_mode - _Trampoline_start) + TRAMPOLINE_ADDR)

.code32
.protected_mode:
  # Load data segment registers
  movw $PM_GDT_DATA_OFFSET, %ax
  movw %ax, %ds
  movw %ax, %es
  movw %ax, %ss

  # Move PML4 table address to cr3
  movl $amd64_pml4, %eax
  movl %eax, %cr3

  # Flip PAE bit in cr4
  movl %cr4, %eax
  orl $CR4_PAE, %eax
  movl %eax, %cr4

  # Set LME on the EFER MSR
  movl $EFER_MSR, %ecx
  rdmsr
  orl $EFER_MSR_LME, %eax
  wrmsr

  # Enable paging
  movl %cr0, %eax
  orl $CR0_PG, %eax
  movl %eax, %cr0

  # Update GDT for long mode
  lgdt amd64_gdt_descriptor

  # Jump to long mode
  ljmp $GDT_CODE_SEG_OFFSET, $((.long_mode - _Trampoline_start) + TRAMPOLINE_ADDR)

.code64
.long_mode:
  # Load data segment registers
  movw $GDT_DATA_SEG_OFFSET, %ax
  movw %ax, %ds
  movw %ax, %es
  movw %ax, %ss
  movw %ax, %fs

  # Acquire the processor's stack
  GET_SELF_CPU_CONTROL_RAX
  movq PER_CPU_INTERRUPT_STACK_HIGH(rax), rsp

  # Enable SSE
  movq %cr0, rax
  andq $CR0_EM_BITMASK, rax
  orq $CR0_MP, rax
  movq rax, %cr0
  movq %cr4, rax
  orq $CR4_SSEFLAGS, rax
  movq rax, %cr4

  # Exit trampoline code
	movabsq	$smp_init_ap, rax
	call *rax

/* Temporary GDT used to get to long mode */
gdt:
  /* NULL segment */
  .quad 0
  /* Code segment */
  .word 0xFFFF, 0
  .byte 0, 0x9F, 0xCF, 0
  /* Data segment */
  .word 0xFFFF, 0
  .byte 0, 0x92, 0xCF, 0
gdt_desc:
	.word (gdt_desc - gdt) - 1
	.long (gdt - _Trampoline_start) + TRAMPOLINE_ADDR

trampoline_end:
PUBLIC(_Trampoline_size)
SYM(_Trampoline_size):
.quad (trampoline_end - _Trampoline_start)
