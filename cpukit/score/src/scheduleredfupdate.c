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
#include <rtems/score/priority.h>
#include <rtems/score/scheduler.h>
#include <rtems/score/scheduleredf.h>
#include <rtems/score/thread.h>

void _Scheduler_EDF_Update(
  Thread_Control      *the_thread
)
{
  Scheduler_EDF_Per_thread *sched_info =
    (Scheduler_EDF_Per_thread*)the_thread->scheduler_info;

  if (sched_info->queue_state == SCHEDULER_EDF_QUEUE_STATE_NEVER_HAS_BEEN) {
    /* Shifts the priority to the region of background tasks. */
    the_thread->Start.initial_priority |= (SCHEDULER_EDF_PRIO_MSB);
    the_thread->real_priority    = the_thread->Start.initial_priority;
    the_thread->current_priority = the_thread->Start.initial_priority;
    sched_info->queue_state = SCHEDULER_EDF_QUEUE_STATE_NOT_PRESENTLY;
  }
}
