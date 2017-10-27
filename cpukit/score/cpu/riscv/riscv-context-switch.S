/*
 * riscv32 CPU Dependent Source
 *
 * Copyright (c) 2015 University of York.
 * Hesham ALmatary <hesham@alumni.york.ac.uk>
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

.section .text, "ax"
.align 4

PUBLIC(_CPU_Context_switch)
PUBLIC(_CPU_Context_restore)
PUBLIC(_CPU_Context_restore_fp)
PUBLIC(_CPU_Context_save_fp)
PUBLIC(restore)

SYM(_CPU_Context_switch):
  /* Disable interrupts and store all registers */
  csrr t0, mstatus
  SREG t0, (32 * CPU_SIZEOF_POINTER)(a0)

  csrci mstatus, MSTATUS_MIE

  SREG x1, (1 * CPU_SIZEOF_POINTER)(a0)
  SREG x2, (2 * CPU_SIZEOF_POINTER)(a0)
  SREG x3, (3 * CPU_SIZEOF_POINTER)(a0)
  SREG x4, (4 * CPU_SIZEOF_POINTER)(a0)
  SREG x5, (5 * CPU_SIZEOF_POINTER)(a0)
  SREG x6, (6 * CPU_SIZEOF_POINTER)(a0)
  SREG x7, (7 * CPU_SIZEOF_POINTER)(a0)
  SREG x8, (8 * CPU_SIZEOF_POINTER)(a0)
  SREG x9, (9 * CPU_SIZEOF_POINTER)(a0)
  SREG x10, (10 * CPU_SIZEOF_POINTER)(a0)
  SREG x11, (11 * CPU_SIZEOF_POINTER)(a0)
  SREG x12, (12 * CPU_SIZEOF_POINTER)(a0)
  SREG x13, (13 * CPU_SIZEOF_POINTER)(a0)
  SREG x14, (14 * CPU_SIZEOF_POINTER)(a0)
  SREG x15, (15 * CPU_SIZEOF_POINTER)(a0)
  SREG x16, (16 * CPU_SIZEOF_POINTER)(a0)
  SREG x17, (17 * CPU_SIZEOF_POINTER)(a0)
  SREG x18, (18 * CPU_SIZEOF_POINTER)(a0)
  SREG x19, (19 * CPU_SIZEOF_POINTER)(a0)
  SREG x20, (20 * CPU_SIZEOF_POINTER)(a0)
  SREG x21, (21 * CPU_SIZEOF_POINTER)(a0)
  SREG x22, (22 * CPU_SIZEOF_POINTER)(a0)
  SREG x23, (23 * CPU_SIZEOF_POINTER)(a0)
  SREG x24, (24 * CPU_SIZEOF_POINTER)(a0)
  SREG x25, (25 * CPU_SIZEOF_POINTER)(a0)
  SREG x26, (26 * CPU_SIZEOF_POINTER)(a0)
  SREG x27, (27 * CPU_SIZEOF_POINTER)(a0)
  SREG x28, (28 * CPU_SIZEOF_POINTER)(a0)
  SREG x29, (28 * CPU_SIZEOF_POINTER)(a0)
  SREG x30, (30 * CPU_SIZEOF_POINTER)(a0)
  SREG x31, (31 * CPU_SIZEOF_POINTER)(a0)

  SYM(restore):

  LREG x1, (1 * CPU_SIZEOF_POINTER)(a1)
  LREG x2, (2 * CPU_SIZEOF_POINTER)(a1)
  LREG x3, (3 * CPU_SIZEOF_POINTER)(a1)
  LREG x4, (4 * CPU_SIZEOF_POINTER)(a1)
  LREG x5, (5 * CPU_SIZEOF_POINTER)(a1)
  LREG x6, (6 * CPU_SIZEOF_POINTER)(a1)
  LREG x7, (7 * CPU_SIZEOF_POINTER)(a1)
  LREG x8, (8 * CPU_SIZEOF_POINTER)(a1)
  LREG x9, (9 * CPU_SIZEOF_POINTER)(a1)
  LREG x10, (10 * CPU_SIZEOF_POINTER)(a1)
  /* Skip a1/x11 */
  LREG x12, (12 * CPU_SIZEOF_POINTER)(a1)
  LREG x13, (13 * CPU_SIZEOF_POINTER)(a1)
  LREG x14, (14 * CPU_SIZEOF_POINTER)(a1)
  LREG x15, (15 * CPU_SIZEOF_POINTER)(a1)
  LREG x16, (16 * CPU_SIZEOF_POINTER)(a1)
  LREG x17, (17 * CPU_SIZEOF_POINTER)(a1)
  LREG x18, (18 * CPU_SIZEOF_POINTER)(a1)
  LREG x19, (19 * CPU_SIZEOF_POINTER)(a1)
  LREG x20, (20 * CPU_SIZEOF_POINTER)(a1)
  LREG x21, (21 * CPU_SIZEOF_POINTER)(a1)
  LREG x22, (22 * CPU_SIZEOF_POINTER)(a1)
  LREG x23, (23 * CPU_SIZEOF_POINTER)(a1)
  LREG x24, (24 * CPU_SIZEOF_POINTER)(a1)
  LREG x25, (25 * CPU_SIZEOF_POINTER)(a1)
  LREG x26, (26 * CPU_SIZEOF_POINTER)(a1)
  LREG x27, (27 * CPU_SIZEOF_POINTER)(a1)
  LREG x28, (28 * CPU_SIZEOF_POINTER)(a1)
  LREG x29, (29 * CPU_SIZEOF_POINTER)(a1)
  LREG x30, (30 * CPU_SIZEOF_POINTER)(a1)

  /* Load mstatus */
  LREG x31, (32 * CPU_SIZEOF_POINTER)(a1)
  csrw mstatus, x31

  LREG x30, (30 * CPU_SIZEOF_POINTER)(a1)

  LREG x11, (11 * CPU_SIZEOF_POINTER)(a1)

  ret

  SYM(_CPU_Context_restore):
  mv     a1, a0
  j      restore

  /* TODO no FP support for riscv32 yet */
  SYM(_CPU_Context_restore_fp):
  nop

  SYM(_CPU_Context_save_fp):
  nop
