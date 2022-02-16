/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreSchedulerCBS
 *
 * @brief This source file contains the implementation of
 *   _Scheduler_CBS_Attach_thread().
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

  the_thread->is_preemptible = true;
  the_thread->CPU_budget.operations = &_Scheduler_CBS_Budget;

  _ISR_lock_ISR_enable( &lock_context );
  return SCHEDULER_CBS_OK;
}
