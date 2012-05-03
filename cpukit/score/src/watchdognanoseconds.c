/*
 * Copyright (c) 2010 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/watchdog.h>

Watchdog_Nanoseconds_since_last_tick_routine
  _Watchdog_Nanoseconds_since_tick_handler =
    _Watchdog_Nanoseconds_since_tick_default_handler;

uint32_t _Watchdog_Nanoseconds_since_tick_default_handler( void )
{
  return 0;
}
