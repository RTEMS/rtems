/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreSchedulerCBS
 *
 * @brief This source file contains the implementation of
 *   _Scheduler_CBS_Unblock().
 */

/*
 *  Copyright (C) 2011 Petr Benes.
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
    uint32_t deadline_left = the_thread->CPU_budget.available;
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
