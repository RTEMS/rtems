/*
 * Copyright (c) 2008-2012 embedded brains GmbH.  All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>

void bsp_restart(void *addr)
{
  rtems_interrupt_level level;
  void (*start)(void) = addr;

  (void) level;
  rtems_interrupt_disable(level);
  (*start)();
}
