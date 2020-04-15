/**
 *  @file
 *
 *  @brief Scheduler CBS Get Remaining Budget
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
#include <rtems/score/threadimpl.h>

int _Scheduler_CBS_Get_remaining_budget (
  Scheduler_CBS_Server_id  server_id,
  time_t                  *remaining_budget
)
{
  Scheduler_CBS_Server *server;
  ISR_lock_Context      lock_context;
  Thread_Control       *the_thread;

  if ( server_id >= _Scheduler_CBS_Maximum_servers ) {
    return SCHEDULER_CBS_ERROR_INVALID_PARAMETER;
  }

  server = &_Scheduler_CBS_Server_list[ server_id ];

  if ( !server->initialized ) {
    return SCHEDULER_CBS_ERROR_NOSERVER;
  }

  if ( server->task_id == -1 ) {
    *remaining_budget = server->parameters.budget;
    return SCHEDULER_CBS_OK;
  }

  the_thread = _Thread_Get( server->task_id, &lock_context );

  if ( the_thread != NULL ) {
    *remaining_budget = the_thread->cpu_time_budget;
    _ISR_lock_ISR_enable( &lock_context );
  } else {
    *remaining_budget = 0;
  }

  return SCHEDULER_CBS_OK;
}
