/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreSchedulerCBS
 *
 * @brief This source file contains the implementation of
 *   _Scheduler_CBS_Budget_callout() and _Scheduler_CBS_Initialize().
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
#include <rtems/score/statesimpl.h>
#include <rtems/score/threadcpubudget.h>

/**
 * @brief Invoked when a limited time quantum is exceeded.
 *
 * This routine is invoked when a limited time quantum is exceeded.
 *
 * @param the_thread The thread that exceeded a limited time quantum.
 */
static void _Scheduler_CBS_Budget_callout( Thread_Control *the_thread )
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

static void _Scheduler_CBS_Budget_at_tick( Thread_Control *the_thread )
{
  uint32_t budget_available;

  if ( !the_thread->is_preemptible ) {
    return;
  }

  if ( !_States_Is_ready( the_thread->current_state ) ) {
    return;
  }

  budget_available = the_thread->CPU_budget.available;

  if ( budget_available == 1 ) {
    the_thread->CPU_budget.available = 0;
    _Scheduler_CBS_Budget_callout ( the_thread );
  } else {
    the_thread->CPU_budget.available = budget_available - 1;
  }
}

const Thread_CPU_budget_operations _Scheduler_CBS_Budget = {
  .at_tick = _Scheduler_CBS_Budget_at_tick,
  .at_context_switch = _Thread_CPU_budget_do_nothing,
  .initialize = _Thread_CPU_budget_do_nothing
};

int _Scheduler_CBS_Initialize(void)
{
  return SCHEDULER_CBS_OK;
}
