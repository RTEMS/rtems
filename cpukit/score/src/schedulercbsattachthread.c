/**
 *  @file
 *
 *  @brief Attach Scheduler CBS Thread
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

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/schedulercbsimpl.h>
#include <rtems/score/threadimpl.h>

int _Scheduler_CBS_Attach_thread (
  Scheduler_CBS_Server_id server_id,
  rtems_id                task_id
)
{
  Scheduler_CBS_Server *server;
  ISR_lock_Context      lock_context;
  Thread_Control       *the_thread;
  Scheduler_CBS_Node   *node;

  if ( server_id >= _Scheduler_CBS_Maximum_servers ) {
    return SCHEDULER_CBS_ERROR_INVALID_PARAMETER;
  }

  server = &_Scheduler_CBS_Server_list[ server_id ];

  if ( !server->initialized ) {
    return SCHEDULER_CBS_ERROR_NOSERVER;
  }

  if ( server->task_id != -1 ) {
    return SCHEDULER_CBS_ERROR_FULL;
  }

  the_thread = _Thread_Get( task_id, &lock_context );

  if ( the_thread == NULL ) {
    return SCHEDULER_CBS_ERROR_INVALID_PARAMETER;
  }

  node = _Scheduler_CBS_Thread_get_node( the_thread );

  if ( node->cbs_server != NULL ) {
    _ISR_lock_ISR_enable( &lock_context );
    return SCHEDULER_CBS_ERROR_FULL;
  }

  node->cbs_server = server;

  server->task_id = task_id;

  the_thread->budget_callout   = _Scheduler_CBS_Budget_callout;
  the_thread->budget_algorithm = THREAD_CPU_BUDGET_ALGORITHM_CALLOUT;
  the_thread->is_preemptible   = true;

  _ISR_lock_ISR_enable( &lock_context );
  return SCHEDULER_CBS_OK;
}
