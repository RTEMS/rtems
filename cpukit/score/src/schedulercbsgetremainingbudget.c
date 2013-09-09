/**
 *  @file
 *
 *  @brief Scheduler CBS Get Remaining Budget
 *  @ingroup ScoreScheduler
 */

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

#include <rtems/score/schedulercbs.h>
#include <rtems/score/threadimpl.h>

int _Scheduler_CBS_Get_remaining_budget (
  Scheduler_CBS_Server_id  server_id,
  time_t                  *remaining_budget
)
{
  Objects_Locations location;
  Thread_Control *the_thread;

  if ( server_id >= _Scheduler_CBS_Maximum_servers )
    return SCHEDULER_CBS_ERROR_INVALID_PARAMETER;
  if ( !_Scheduler_CBS_Server_list[server_id] )
    return SCHEDULER_CBS_ERROR_NOSERVER;
  if ( _Scheduler_CBS_Server_list[server_id]->task_id == -1 ) {
    *remaining_budget = _Scheduler_CBS_Server_list[server_id]->parameters.budget;
    return SCHEDULER_CBS_OK;
  }

  the_thread = _Thread_Get(
                 _Scheduler_CBS_Server_list[server_id]->task_id,
                 &location
               );
  /* The routine _Thread_Get may disable dispatch and not enable again. */
  if ( the_thread ) {
    *remaining_budget = the_thread->cpu_time_budget;
    _Objects_Put( &the_thread->Object );
  }
  else {
    *remaining_budget = 0;
  }

  return SCHEDULER_CBS_OK;
}
