/**
 * @file
 *
 * @ingroup RTEMSScoreScheduler
 *
 * @brief CBS Scheduler Budget Handler
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

void _Scheduler_CBS_Budget_callout(
  Thread_Control *the_thread
)
{
  Scheduler_CBS_Node      *node;
  Scheduler_CBS_Server_id  server_id;
  Thread_queue_Context     queue_context;

  node = _Scheduler_CBS_Thread_get_node( the_thread );

  /* Put violating task to background until the end of period. */
  _Thread_queue_Context_clear_priority_updates( &queue_context );
  _Scheduler_CBS_Cancel_job(
    NULL,
    the_thread,
    node->deadline_node,
    &queue_context
  );
  _Thread_Priority_update( &queue_context );

  /* Invoke callback function if any. */
  if ( node->cbs_server->cbs_budget_overrun ) {
    _Scheduler_CBS_Get_server_id(
        node->cbs_server->task_id,
        &server_id
    );
    node->cbs_server->cbs_budget_overrun( server_id );
  }
}

int _Scheduler_CBS_Initialize(void)
{
  return SCHEDULER_CBS_OK;
}
