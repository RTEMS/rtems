/*
 * bspinit.S
 *
 * COPYRIGHT (c) 2010 Gedare Bloom.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

/*
 *
 * BSP specific initialization for Sparc64 RTEMS -- sun4v BSP
 *
 * This code defines start code specific to the sun4v BSP
 */

#include <rtems/asm.h>
#include <rtems/score/cpu.h>

#define STACK_WINDOW_SAVE_AREA_SIZE (16*8)

.section .text

PUBLIC(_BSP_init)
  .global _BSP_init
  SYM(_BSP_init):

  save %sp, -STACK_WINDOW_SAVE_AREA_SIZE, %sp  



  ret
  restore

