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

#include <rtems/score/assert.h>
#include <rtems/score/schedulerimpl.h>
#include <rtems/score/threaddispatch.h>
#include <rtems/score/todimpl.h>
#include <rtems/score/watchdogimpl.h>

#if HAVE_CONFIG_H
#include "config.h"
#endif

void _Watchdog_Tick( void )
{
  _Assert( !_Thread_Dispatch_is_enabled() );

  _TOD_Tickle_ticks();

  _Watchdog_Tickle_ticks();

  _Scheduler_Tick();
}
