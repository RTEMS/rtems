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
#include <rtems/score/schedulercbs.h>

void _Scheduler_CBS_Release_job(
  Thread_Control    *the_thread,
  uint32_t           deadline
)
{
  Priority_Control new_priority;
  Scheduler_CBS_Per_thread *sched_info =
    (Scheduler_CBS_Per_thread *) the_thread->scheduler_info;
  Scheduler_CBS_Server *serv_info =
    (Scheduler_CBS_Server *) sched_info->cbs_server;

  if (deadline) {
    /* Initializing or shifting deadline. */
    if (serv_info)
      new_priority = (_Watchdog_Ticks_since_boot + serv_info->parameters.deadline)
        & ~SCHEDULER_EDF_PRIO_MSB;
    else
      new_priority = (_Watchdog_Ticks_since_boot + deadline)
        & ~SCHEDULER_EDF_PRIO_MSB;
  }
  else {
    /* Switch back to background priority. */
    new_priority = the_thread->Start.initial_priority;
  }

  /* Budget replenishment for the next job. */
  if (serv_info)
    the_thread->cpu_time_budget = serv_info->parameters.budget;

  the_thread->real_priority = new_priority;
  _Thread_Change_priority(the_thread, new_priority, true);
}
