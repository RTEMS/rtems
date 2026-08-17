/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsX8664AMD64
 *
 * @brief Exception prologues
 */

/*
 * Copyright (C) 2026 embedded brains GmbH & Co. KG
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

#include <rtems/asm.h>

BEGIN_CODE

/*
 * A distinct entry per vector, so that the common part gets the vector number
 * without reading it back from the interrupt descriptor table.  The processor
 * pushes an error code for some vectors only.  The entries of the other
 * vectors push a zero, so that the frame has the same shape everywhere.
 */
#define EXCEPTION_ENTRY(vector)              \
  .p2align 4                               ; \
  PUBLIC(amd64_exception_prologue_##vector); \
SYM(amd64_exception_prologue_##vector):    ; \
  pushq $0                                 ; \
  pushq $vector                            ; \
  jmp   SYM(amd64_exception_common)

#define EXCEPTION_ENTRY_WITH_CODE(vector)    \
  .p2align 4                               ; \
  PUBLIC(amd64_exception_prologue_##vector); \
SYM(amd64_exception_prologue_##vector):    ; \
  pushq $vector                            ; \
  jmp   SYM(amd64_exception_common)

EXCEPTION_ENTRY(0)
EXCEPTION_ENTRY(1)
EXCEPTION_ENTRY(2)
EXCEPTION_ENTRY(3)
EXCEPTION_ENTRY(4)
EXCEPTION_ENTRY(5)
EXCEPTION_ENTRY(6)
EXCEPTION_ENTRY(7)
EXCEPTION_ENTRY_WITH_CODE(8)
EXCEPTION_ENTRY(9)
EXCEPTION_ENTRY_WITH_CODE(10)
EXCEPTION_ENTRY_WITH_CODE(11)
EXCEPTION_ENTRY_WITH_CODE(12)
EXCEPTION_ENTRY_WITH_CODE(13)
EXCEPTION_ENTRY_WITH_CODE(14)
EXCEPTION_ENTRY(15)
EXCEPTION_ENTRY(16)
EXCEPTION_ENTRY_WITH_CODE(17)
EXCEPTION_ENTRY(18)
EXCEPTION_ENTRY(19)
EXCEPTION_ENTRY(20)
EXCEPTION_ENTRY_WITH_CODE(21)
EXCEPTION_ENTRY(22)
EXCEPTION_ENTRY(23)
EXCEPTION_ENTRY(24)
EXCEPTION_ENTRY(25)
EXCEPTION_ENTRY(26)
EXCEPTION_ENTRY(27)
EXCEPTION_ENTRY(28)
EXCEPTION_ENTRY_WITH_CODE(29)
EXCEPTION_ENTRY_WITH_CODE(30)
EXCEPTION_ENTRY(31)

/*
 * Complete the CPU_Exception_frame and hand it to the C code, which does not
 * return.  The registers are pushed in the reverse order of the frame, since
 * the stack grows towards lower addresses.
 */
SYM(amd64_exception_common):
  pushq r15
  pushq r14
  pushq r13
  pushq r12
  pushq r11
  pushq r10
  pushq r9
  pushq r8
  pushq rbp
  pushq rdi
  pushq rsi
  pushq rdx
  pushq rcx
  pushq rbx
  pushq rax

  movq  rsp, REG_ARG0
  /* The ABI wants the stack pointer aligned to 16 bytes before a call */
  andq  $~15, rsp
  call  SYM(amd64_exception_handler)

END_CODE

END
