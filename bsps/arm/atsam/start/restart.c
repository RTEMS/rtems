/*
 * Copyright (c) 2017 embedded brains GmbH.  All rights reserved.
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
#include <bsp.h>

static void ARMV7M_Systick_cleanup(void)
{
  volatile ARMV7M_Systick *systick = _ARMV7M_Systick;
  systick->csr = 0;
}

void bsp_restart( const void *const addr )
{
  rtems_interrupt_level level;
  void(*start)(void) = (void(*)(void))(addr);

  rtems_interrupt_disable(level);
  (void)level;
  rtems_cache_disable_instruction();
  rtems_cache_disable_data();
  rtems_cache_invalidate_entire_instruction();
  rtems_cache_invalidate_entire_data();
  ARMV7M_Systick_cleanup();

  start();
}
