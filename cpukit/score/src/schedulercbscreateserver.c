/**
 *  @file
 *
 *  @brief Scheduler CBS Create Server
 *  @ingroup RTEMSScoreScheduler
 */

/*
 *  Copyright (C) 2011 Petr Benes.
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/schedulercbs.h>
#include <rtems/score/scheduleredfimpl.h>

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
    if ( !_Scheduler_CBS_Server_list[i].initialized )
      break;
  }

  if ( i == _Scheduler_CBS_Maximum_servers )
    return SCHEDULER_CBS_ERROR_FULL;

  *server_id = i;
  the_server = &_Scheduler_CBS_Server_list[*server_id];
  the_server->parameters = *params;
  the_server->task_id = -1;
  the_server->cbs_budget_overrun = budget_overrun_callback;
  the_server->initialized = true;
  return SCHEDULER_CBS_OK;
}
