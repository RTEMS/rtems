/*
 * Copyrigh (c) 2015 Hesham Almatary <hesham@alumni.york.ac.uk>
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

.section	.text


PUBLIC(_CPU_Context_validate)
SYM(_CPU_Context_validate):
  /* RISC-V/RTEMS context has 36 registers of CPU_SIZEOF_POINTER size */
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

  /* Fill */

  /* t0 is used for temporary values */
  mv t0, x0

  /* x31 contains the stack pointer */
  mv x31, sp

  .macro fill_register reg
  addi	t0, t0,  1
  mv	\reg, t0
  .endm

  fill_register	x1
  fill_register	x2
  fill_register	x3
  fill_register	x4
  fill_register	x5
  fill_register	x6
  fill_register	x7
  fill_register	x8
  fill_register	x9
  fill_register	x10
  fill_register	x11
  fill_register	x12
  fill_register	x13
  fill_register	x14
  fill_register	x15
  fill_register	x16
  fill_register	x17
  fill_register	x18
  fill_register	x19
  fill_register	x20
  fill_register	x21
  fill_register	x22
  fill_register	x23
  fill_register	x24
  fill_register	x25
  fill_register	x26
  fill_register	x27
  fill_register	x28
  fill_register	x29
  fill_register	x30
  fill_register	x31

  /* Check */
check:

  .macro check_register reg
  addi t0, t0, 1
  bne \reg, t0, restore
  .endm

  bne   x31, sp, restore

  mv t0, x0

  check_register	x1
  check_register	x2
  check_register	x3
  check_register	x4
  check_register	x5
  check_register	x6
  check_register	x7
  check_register	x8
  check_register	x9
  check_register	x10
  check_register	x11
  check_register	x12
  check_register	x13
  check_register	x14
  check_register	x15
  check_register	x16
  check_register	x17
  check_register	x18
  check_register	x19
  check_register	x20
  check_register	x21
  check_register	x22
  check_register	x23
  check_register	x24
  check_register	x25
  check_register	x26
  check_register	x27
  check_register	x28
  check_register	x29
  check_register	x30
  check_register	x31

  j check

  /* Restore */
restore:
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

  LREG x31, (31 * CPU_SIZEOF_POINTER)(sp)

  addi sp, sp, 36 * CPU_SIZEOF_POINTER
  ret
