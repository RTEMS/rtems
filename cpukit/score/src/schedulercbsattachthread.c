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
#include <rtems/rtems/object.h>

int _Scheduler_CBS_Attach_thread (
  Scheduler_CBS_Server_id server_id,
  rtems_id                task_id
)
{
  Objects_Locations location;
  Thread_Control *the_thread;
  Scheduler_CBS_Per_thread *sched_info;

  if ( server_id >= _Scheduler_CBS_Maximum_servers )
    return SCHEDULER_CBS_ERROR_INVALID_PARAMETER;

  the_thread = _Thread_Get(task_id, &location);
  /* The routine _Thread_Get may disable dispatch and not enable again. */
  if ( the_thread )
    _Thread_Enable_dispatch();
  if ( !the_thread )
    return SCHEDULER_CBS_ERROR_INVALID_PARAMETER;

  sched_info = (Scheduler_CBS_Per_thread *) the_thread->scheduler_info;

  /* Thread is already attached to a server. */
  if ( sched_info->cbs_server )
    return SCHEDULER_CBS_ERROR_FULL;

  /* Server is not valid. */
  if ( !_Scheduler_CBS_Server_list[server_id] )
    return SCHEDULER_CBS_ERROR_NOSERVER;

  /* Server is already attached to a thread. */
  if ( _Scheduler_CBS_Server_list[server_id]->task_id != -1 )
    return SCHEDULER_CBS_ERROR_FULL;

  _Scheduler_CBS_Server_list[server_id]->task_id = task_id;
  sched_info->cbs_server = (void *) _Scheduler_CBS_Server_list[server_id];

  the_thread->budget_callout   = _Scheduler_CBS_Budget_callout;
  the_thread->budget_algorithm = THREAD_CPU_BUDGET_ALGORITHM_CALLOUT;
  the_thread->is_preemptible   = true;

  return SCHEDULER_CBS_OK;
}
