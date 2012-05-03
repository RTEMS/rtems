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
#include <rtems/score/scheduler.h>
#include <rtems/score/scheduleredf.h>

void _Scheduler_EDF_Release_job(
  Thread_Control    *the_thread,
  uint32_t           deadline
)
{
  Priority_Control new_priority;

  if (deadline) {
    /* Initializing or shifting deadline. */
    new_priority = (_Watchdog_Ticks_since_boot + deadline)
                   & ~SCHEDULER_EDF_PRIO_MSB;
  }
  else {
    /* Switch back to background priority. */
    new_priority = the_thread->Start.initial_priority;
  }

  the_thread->real_priority = new_priority;
  _Thread_Change_priority(the_thread, new_priority, true);
}
