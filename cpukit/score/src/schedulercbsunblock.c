/**
 * @file
 *
 * @brief Unblocks a Thread from the Queue
 *
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
#include <rtems/score/scheduleredfimpl.h>
#include <rtems/score/schedulerimpl.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/watchdogimpl.h>

Scheduler_Void_or_thread _Scheduler_CBS_Unblock(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread
)
{
  Scheduler_EDF_Context *context;
  Scheduler_CBS_Node    *node;
  Scheduler_CBS_Server  *serv_info;
  Priority_Control       priority;
  bool                   prepend_it;

  context = _Scheduler_EDF_Get_context( scheduler );
  node = _Scheduler_CBS_Thread_get_node( the_thread );
  serv_info = node->cbs_server;
  priority = _Scheduler_Node_get_priority( &node->Base.Base, &prepend_it );
  (void) prepend_it;

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
      /* Put late unblocked task to background until the end of period. */

      priority = node->Base.background_priority;
      the_thread->real_priority = priority;

      if (
        _Thread_Priority_less_than( the_thread->current_priority, priority )
          || !_Thread_Owns_resources( the_thread )
      ) {
        the_thread->current_priority = priority;
      }
    }
  }

  node->Base.current_priority = priority;
  _Scheduler_EDF_Enqueue( context, &node->Base, priority );

  /*
   *  If the thread that was unblocked is more important than the heir,
   *  then we have a new heir.  This may or may not result in a
   *  context switch.
   *
   *  Normal case:
   *    If the current thread is preemptible, then we need to do
   *    a context switch.
   *  Pseudo-ISR case:
   *    Even if the thread isn't preemptible, if the new heir is
   *    a pseudo-ISR system task, we need to do a context switch.
   */
  if ( priority < _Thread_Heir->current_priority ) {
    _Scheduler_Update_heir( the_thread, priority == PRIORITY_PSEUDO_ISR );
  }

  SCHEDULER_RETURN_VOID_OR_NULL;
}
