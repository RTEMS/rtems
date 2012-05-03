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

int _Scheduler_CBS_Get_execution_time (
  Scheduler_CBS_Server_id   server_id,
  time_t                   *exec_time,
  time_t                   *abs_time
)
{
  Objects_Locations location;
  Thread_Control *the_thread;

  if ( server_id >= _Scheduler_CBS_Maximum_servers )
    return SCHEDULER_CBS_ERROR_INVALID_PARAMETER;
  if ( !_Scheduler_CBS_Server_list[server_id] )
    return SCHEDULER_CBS_ERROR_NOSERVER;
  if ( _Scheduler_CBS_Server_list[server_id]->task_id == -1 ) {
    *exec_time = 0;
    return SCHEDULER_CBS_OK;
  }

  the_thread = _Thread_Get(
                 _Scheduler_CBS_Server_list[server_id]->task_id,
                 &location
               );
  /* The routine _Thread_Get may disable dispatch and not enable again. */
  if ( the_thread ) {
    _Thread_Enable_dispatch();
    *exec_time = _Scheduler_CBS_Server_list[server_id]->parameters.budget -
      the_thread->cpu_time_budget;
  }
  else {
    *exec_time = _Scheduler_CBS_Server_list[server_id]->parameters.budget;
  }
  return SCHEDULER_CBS_OK;
}
