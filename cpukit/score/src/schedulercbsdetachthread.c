/**
 * @file
 *
 * @brief Detach from the CBS Server
 *
 * @ingroup RTEMSScoreScheduler
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

#include <rtems/score/schedulercbsimpl.h>
#include <rtems/score/threadimpl.h>

int _Scheduler_CBS_Detach_thread (
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

  if ( server->task_id != task_id ) {
    return SCHEDULER_CBS_ERROR_INVALID_PARAMETER;
  }

  the_thread = _Thread_Get( task_id, &lock_context );

  if ( the_thread == NULL ) {
    return SCHEDULER_CBS_ERROR_INVALID_PARAMETER;
  }

  node = _Scheduler_CBS_Thread_get_node( the_thread );
  node->cbs_server = NULL;

  server->task_id = -1;

  the_thread->budget_algorithm = the_thread->Start.budget_algorithm;
  the_thread->budget_callout   = the_thread->Start.budget_callout;
  the_thread->is_preemptible   = the_thread->Start.is_preemptible;

  _ISR_lock_ISR_enable( &lock_context );
  return SCHEDULER_CBS_OK;
}
