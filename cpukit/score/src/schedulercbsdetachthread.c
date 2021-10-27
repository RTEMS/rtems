/**
 * @file
 *
 * @ingroup RTEMSScoreSchedulerCBS
 *
 * @brief This source file contains the implementation of
 *   _Scheduler_CBS_Detach_thread().
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
  Scheduler_CBS_Server               *server;
  ISR_lock_Context                    lock_context;
  Thread_Control                     *the_thread;
  Scheduler_CBS_Node                 *node;
  const Thread_CPU_budget_operations *cpu_budget_operations;

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

  the_thread->is_preemptible = the_thread->Start.is_preemptible;

  cpu_budget_operations = the_thread->Start.cpu_budget_operations;
  the_thread->CPU_budget.operations = cpu_budget_operations;

  if ( cpu_budget_operations != NULL ) {
    ( *cpu_budget_operations->initialize )( the_thread );
  }

  _ISR_lock_ISR_enable( &lock_context );
  return SCHEDULER_CBS_OK;
}
