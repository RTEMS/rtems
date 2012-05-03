/*
 *  Copyright (C) 2011 Petr Benes.
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/config.h>
#include <rtems/score/scheduleredf.h>

int _Scheduler_EDF_Priority_compare (
  Priority_Control p1,
  Priority_Control p2
)
{
  Watchdog_Interval time = _Watchdog_Ticks_since_boot;

  /*
   * Reorder priorities to separate deadline driven and background tasks.
   *
   * The background tasks have p1 or p2 > SCHEDULER_EDF_PRIO_MSB.
   * The deadline driven tasks need to have subtracted current time in order
   * to see which deadline is closer wrt. current time.
   */
  if (!(p1 & SCHEDULER_EDF_PRIO_MSB))
    p1 = (p1 - time) & ~SCHEDULER_EDF_PRIO_MSB;
  if (!(p2 & SCHEDULER_EDF_PRIO_MSB))
    p2 = (p2 - time) & ~SCHEDULER_EDF_PRIO_MSB;

  return ((p1<p2) - (p1>p2));
}
