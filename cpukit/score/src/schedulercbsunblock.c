/**
 * @file
 *
 * @brief Unblocks a Thread from the Queue
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
#include <rtems/score/scheduleredfimpl.h>
#include <rtems/score/schedulerimpl.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/watchdogimpl.h>

void _Scheduler_CBS_Unblock(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
)
{
  Scheduler_CBS_Node   *the_node;
  Scheduler_CBS_Server *serv_info;
  Priority_Control      priority;

  the_node = _Scheduler_CBS_Node_downcast( node );
  serv_info = the_node->cbs_server;
  priority = _Scheduler_Node_get_priority( &the_node->Base.Base );
  priority = SCHEDULER_PRIORITY_PURIFY( priority );

  /*
   * Late unblock rule for deadline-driven tasks. The remaining time to
   * deadline must be sufficient to serve the remaining computation time
   * without increased utilization of this task. It might cause a deadline
   * miss of another task.
   */
  if ( serv_info != NULL && ( priority & SCHEDULER_EDF_PRIO_MSB ) == 0 ) {
    time_t deadline = serv_info->parameters.deadline;
    time_t budget = serv_info->parameters.budget;
    uint32_t deadline_left = the_thread->cpu_time_budget;
    Priority_Control budget_left = priority - _Watchdog_Ticks_since_boot;

    if ( deadline * budget_left > budget * deadline_left ) {
      Thread_queue_Context queue_context;

      /* Put late unblocked task to background until the end of period. */
      _Thread_queue_Context_clear_priority_updates( &queue_context );
      _Scheduler_CBS_Cancel_job(
        scheduler,
        the_thread,
        the_node->deadline_node,
        &queue_context
      );
    }
  }

  _Scheduler_EDF_Unblock( scheduler, the_thread, &the_node->Base.Base );
}
