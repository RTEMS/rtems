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
#include <rtems/rtems/signal.h>

Scheduler_CBS_Server **_Scheduler_CBS_Server_list;

void _Scheduler_CBS_Budget_callout(
  Thread_Control *the_thread
)
{
  Priority_Control          new_priority;
  Scheduler_CBS_Per_thread *sched_info;
  Scheduler_CBS_Server_id   server_id;

  /* Put violating task to background until the end of period. */
  new_priority = the_thread->Start.initial_priority;
  if ( the_thread->real_priority != new_priority )
    the_thread->real_priority = new_priority;
  if ( the_thread->current_priority != new_priority )
    _Thread_Change_priority(the_thread, new_priority, true);

  /* Invoke callback function if any. */
  sched_info = (Scheduler_CBS_Per_thread *) the_thread->scheduler_info;
  if ( sched_info->cbs_server->cbs_budget_overrun ) {
    _Scheduler_CBS_Get_server_id(
        sched_info->cbs_server->task_id,
        &server_id
    );
    sched_info->cbs_server->cbs_budget_overrun( server_id );
  }
}

int _Scheduler_CBS_Initialize(void)
{
  unsigned int i;
  _Scheduler_CBS_Server_list = (Scheduler_CBS_Server **) _Workspace_Allocate(
      _Scheduler_CBS_Maximum_servers * sizeof(Scheduler_CBS_Server*) );
  if ( !_Scheduler_CBS_Server_list )
    return SCHEDULER_CBS_ERROR_NO_MEMORY;
  for (i = 0; i<_Scheduler_CBS_Maximum_servers; i++) {
    _Scheduler_CBS_Server_list[i] = NULL;
  }
  return SCHEDULER_CBS_OK;
}
