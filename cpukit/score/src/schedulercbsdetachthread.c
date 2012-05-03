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

int _Scheduler_CBS_Detach_thread (
  Scheduler_CBS_Server_id server_id,
  rtems_id                task_id
)
{
  Objects_Locations location;
  Thread_Control *the_thread;
  Scheduler_CBS_Per_thread *sched_info;

  the_thread = _Thread_Get(task_id, &location);
  /* The routine _Thread_Get may disable dispatch and not enable again. */
  if ( the_thread ) {
    _Thread_Enable_dispatch();
  }

  if ( server_id >= _Scheduler_CBS_Maximum_servers )
    return SCHEDULER_CBS_ERROR_INVALID_PARAMETER;
  if ( !the_thread )
    return SCHEDULER_CBS_ERROR_INVALID_PARAMETER;
  /* Server is not valid. */
  if ( !_Scheduler_CBS_Server_list[server_id] )
    return SCHEDULER_CBS_ERROR_NOSERVER;
  /* Thread and server are not attached. */
  if ( _Scheduler_CBS_Server_list[server_id]->task_id != task_id )
    return SCHEDULER_CBS_ERROR_INVALID_PARAMETER;

  _Scheduler_CBS_Server_list[server_id]->task_id = -1;
  sched_info = (Scheduler_CBS_Per_thread *) the_thread->scheduler_info;
  sched_info->cbs_server = NULL;

  the_thread->budget_algorithm = the_thread->Start.budget_algorithm;
  the_thread->budget_callout   = the_thread->Start.budget_callout;
  the_thread->is_preemptible   = the_thread->Start.is_preemptible;

  return SCHEDULER_CBS_OK;
}
