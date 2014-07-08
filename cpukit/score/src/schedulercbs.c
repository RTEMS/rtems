/**
 * @file
 *
 * @brief CBS Scheduler Budget Handler
 * @ingroup ScoreScheduler
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
#include <rtems/score/wkspace.h>

void _Scheduler_CBS_Budget_callout(
  Thread_Control *the_thread
)
{
  Priority_Control          new_priority;
  Scheduler_CBS_Node       *node;
  Scheduler_CBS_Server_id   server_id;

  /* Put violating task to background until the end of period. */
  new_priority = the_thread->Start.initial_priority;
  if ( the_thread->real_priority != new_priority )
    the_thread->real_priority = new_priority;
  if ( the_thread->current_priority != new_priority )
    _Thread_Change_priority(the_thread, new_priority, true);

  /* Invoke callback function if any. */
  node = _Scheduler_CBS_Thread_get_node( the_thread );
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
