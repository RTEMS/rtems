/* SPDX-License-Identifier: GPL-2.0+-with-RTEMS-exception */

/*
 *  Copyright (c) 2002 by Jay Monkman <jtm@smoothsmoothie.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/bootcard.h>

void bsp_reset( rtems_fatal_source source, rtems_fatal_code code )
{
  (void) source;
  (void) code;

#if ON_SKYEYE == 1
  #define SKYEYE_MAGIC_ADDRESS (*(volatile unsigned int *)(0xb0000000))

  SKYEYE_MAGIC_ADDRESS = 0xff;
#else
  /* TODO: This code was initially copied from the gp32 BSP. That BSP has
   * been obsoleted and removed but this code may still benefit from being
   * in a shared place.
   */
  rtems_interrupt_level level;

  rtems_interrupt_disable(level);
  (void) level;
  /* disable mmu, invalide i-cache and call swi #4 */
  __asm__ volatile(""
    "mrc    p15,0,r0,c1,c0,0  \n"
    "bic    r0,r0,#1          \n"
    "mcr    p15,0,r0,c1,c0,0  \n"
    "nop                      \n"
    "nop                      \n"
    "nop                      \n"
    "nop                      \n"
    "nop                      \n"
    "mov    r0,#0             \n"
    "MCR    p15,0,r0,c7,c5,0  \n"
    "nop                      \n"
    "nop                      \n"
    "nop                      \n"
    "nop                      \n"
    "nop                      \n"
    "swi    #4                "
    :
    :
    : "r0"
  );
  /* we should be back in bios now */
#endif

  RTEMS_UNREACHABLE();
}
