/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  BSP specific initialization for Sparc64 RTEMS -- sun4u BSP
 *
 *  This code defines start code specific to the sun4u BSP
 */

/*
 *  COPYRIGHT (c) 2010 Gedare Bloom.
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
#include <rtems/score/cpu.h>

#include <traptable.h>

#define LSU_CR_IM_MASK  (0x0004)  /* bit 2 */
#define LSU_CR_DM_MASK  (0x0008)  /* bit 3 */

#define STACK_WINDOW_SAVE_AREA_SIZE (16*8)

.register %g2, #scratch
.register %g3, #scratch

.section .text

PUBLIC(_BSP_init)
.global _BSP_init
  SYM(_BSP_init):
  save %sp, -STACK_WINDOW_SAVE_AREA_SIZE, %sp

  ! copy the tba
  sethi %hi(real_trap_table), %o0
  rdpr  %tba, %o2
  stx   %o2, [%o0 + %lo(real_trap_table)]

!  /* copy firmware trap table so that RTEMS can install ISR handlers */
!  setx  SYM(trap_table), %l0, %o0
!  rdpr  %tba, %o1
!  set TABLE_SIZE, %o2
!  call memcpy
!  nop

  mov %g0, %o0
  call _take_mmu
  nop

  ret
  restore

