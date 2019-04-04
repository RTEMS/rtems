/**
 * @file
 *
 * @brief Scheduler CBS Release Job
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

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/schedulercbsimpl.h>

void _Scheduler_CBS_Release_job(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Priority_Node           *priority_node,
  uint64_t                 deadline,
  Thread_queue_Context    *queue_context
)
{
  Scheduler_CBS_Node   *node;
  Scheduler_CBS_Server *serv_info;

  node = _Scheduler_CBS_Thread_get_node( the_thread );
  serv_info = node->cbs_server;

  /* Budget replenishment for the next job. */
  if ( serv_info != NULL ) {
    the_thread->cpu_time_budget = serv_info->parameters.budget;
  }

  node->deadline_node = priority_node;

  _Scheduler_EDF_Release_job(
    scheduler,
    the_thread,
    priority_node,
    deadline,
    queue_context
  );
}

void _Scheduler_CBS_Cancel_job(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Priority_Node           *priority_node,
  Thread_queue_Context    *queue_context
)
{
  Scheduler_CBS_Node *node;

  node = _Scheduler_CBS_Thread_get_node( the_thread );

  if ( node->deadline_node != NULL ) {
    _Assert( node->deadline_node == priority_node );
    node->deadline_node = NULL;

    _Scheduler_EDF_Cancel_job(
      scheduler,
      the_thread,
      priority_node,
      queue_context
    );
  }
}
