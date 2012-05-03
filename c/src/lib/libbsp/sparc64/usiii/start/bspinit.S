/*
 *  BSP specific initialization for Sparc64 RTEMS -- sun4u BSP
 *
 *  This code defines start code specific to the sun4u BSP
 */

/*
 *
 *  COPYRIGHT (c) 2010 Gedare Bloom.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
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

