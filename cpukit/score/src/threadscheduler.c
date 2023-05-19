/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreThread
 *
 * @brief This source file contains the implementation of
 *   _Thread_Scheduler_process_requests().
 */

/*
 * Copyright (c) 2016 embedded brains GmbH & Co. KG
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

#include <rtems/score/threadimpl.h>
#include <rtems/score/schedulerimpl.h>

#if defined(RTEMS_SMP)
static void _Thread_Scheduler_withdraw_nodes(
  Thread_Control *the_thread,
  Scheduler_Node *scheduler_node
)
{
  while ( scheduler_node != NULL ) {
    Scheduler_Node          *next;
    const Scheduler_Control *scheduler;
    ISR_lock_Context         lock_context;

    next = scheduler_node->Thread.Scheduler_node.next;
#if defined(RTEMS_DEBUG)
    scheduler_node->Thread.Scheduler_node.next = NULL;
#endif

    scheduler = _Scheduler_Node_get_scheduler( scheduler_node );

    _Scheduler_Acquire_critical( scheduler, &lock_context );
    ( *scheduler->Operations.withdraw_node )(
      scheduler,
      the_thread,
      scheduler_node,
      THREAD_SCHEDULER_READY
    );
    _Scheduler_Release_critical( scheduler, &lock_context );

    scheduler_node = next;
  }
}

void _Thread_Scheduler_process_requests( Thread_Control *the_thread )
{
  ISR_lock_Context  lock_context;
  Scheduler_Node   *scheduler_node;

  _Thread_Scheduler_acquire_critical( the_thread, &lock_context );

  scheduler_node = the_thread->Scheduler.requests;

  if ( scheduler_node != NULL ) {
    Scheduler_Node *remove;

    the_thread->Scheduler.requests = NULL;
    remove = NULL;

    do {
      Scheduler_Node_request request;
      Scheduler_Node        *next;

      request = scheduler_node->Thread.request;
      scheduler_node->Thread.request = SCHEDULER_NODE_REQUEST_NOT_PENDING;

      next = scheduler_node->Thread.next_request;
#if defined(RTEMS_DEBUG)
      scheduler_node->Thread.next_request = NULL;
#endif

      if ( request == SCHEDULER_NODE_REQUEST_ADD ) {
        ++the_thread->Scheduler.helping_nodes;
        _Chain_Append_unprotected(
          &the_thread->Scheduler.Scheduler_nodes,
          &scheduler_node->Thread.Scheduler_node.Chain
        );
      } else if ( request == SCHEDULER_NODE_REQUEST_REMOVE ) {
        --the_thread->Scheduler.helping_nodes;
        _Chain_Extract_unprotected(
          &scheduler_node->Thread.Scheduler_node.Chain
        );
        scheduler_node->Thread.Scheduler_node.next = remove;
        remove = scheduler_node;
      } else {
        _Assert( request == SCHEDULER_NODE_REQUEST_NOTHING );
      }

      scheduler_node = next;
    } while ( scheduler_node != NULL );

    _Thread_Scheduler_release_critical( the_thread, &lock_context );
    _Thread_Scheduler_withdraw_nodes( the_thread, remove );
  } else {
    _Thread_Scheduler_release_critical( the_thread, &lock_context );
  }
}
#endif
