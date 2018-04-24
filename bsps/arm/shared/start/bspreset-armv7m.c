/*
 * Copyright (c) 2015 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <rtems.h>
#include <rtems/score/armv7m.h>

#include <bsp/bootcard.h>
#include <bsp/start.h>

#ifdef ARM_MULTILIB_ARCH_V7M

BSP_START_TEXT_SECTION __attribute__((flatten)) void bsp_reset(void)
{
  rtems_interrupt_level level;

  rtems_interrupt_local_disable(level);
  (void) level;

  _ARMV7M_SCB->aircr = ARMV7M_SCB_AIRCR_VECTKEY
    | ARMV7M_SCB_AIRCR_SYSRESETREQ;

  while (true) {
    /* Do nothing */
  }
}

#endif /* ARM_MULTILIB_ARCH_V7M */
