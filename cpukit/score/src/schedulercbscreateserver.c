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

int _Scheduler_CBS_Create_server (
  Scheduler_CBS_Parameters     *params,
  Scheduler_CBS_Budget_overrun  budget_overrun_callback,
  rtems_id                     *server_id
)
{
  unsigned int i;
  Scheduler_CBS_Server *the_server;

  if ( params->budget <= 0 ||
       params->deadline <= 0 ||
       params->budget >= SCHEDULER_EDF_PRIO_MSB ||
       params->deadline >= SCHEDULER_EDF_PRIO_MSB )
    return SCHEDULER_CBS_ERROR_INVALID_PARAMETER;

  for ( i = 0; i<_Scheduler_CBS_Maximum_servers; i++ ) {
    if ( !_Scheduler_CBS_Server_list[i] )
      break;
  }

  if ( i == _Scheduler_CBS_Maximum_servers )
    return SCHEDULER_CBS_ERROR_FULL;

  *server_id = i;
  _Scheduler_CBS_Server_list[*server_id] = (Scheduler_CBS_Server *)
    _Workspace_Allocate( sizeof(Scheduler_CBS_Server) );
  the_server = _Scheduler_CBS_Server_list[*server_id];
  if ( !the_server )
    return SCHEDULER_CBS_ERROR_NO_MEMORY;

  the_server->parameters = *params;
  the_server->task_id = -1;
  the_server->cbs_budget_overrun = budget_overrun_callback;
  return SCHEDULER_CBS_OK;
}
