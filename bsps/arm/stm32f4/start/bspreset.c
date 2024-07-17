/*
 * Copyright (c) 2012 Sebastian Huber.  All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <rtems.h>

#include <bsp/bootcard.h>

void bsp_reset( rtems_fatal_source source, rtems_fatal_code code )
{
  rtems_interrupt_level level;

  (void) source;
  (void) code;

  rtems_interrupt_disable(level);
  (void) level;

  while (1);
}
