/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreThreadQueue
 *
 * @brief This source file contains the implementation of
 *   _Thread_queue_Flush_default_filter(),
 *   _Thread_queue_Flush_status_object_was_deleted(),
 *   _Thread_queue_Flush_status_unavailable(), and
 *   _Thread_queue_Flush_critical().
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
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

#include <rtems/score/schedulerimpl.h>
#include <rtems/score/status.h>
#include <rtems/score/threadimpl.h>

Thread_Control *_Thread_queue_Flush_default_filter(
  Thread_Control       *the_thread,
  Thread_queue_Queue   *queue,
  Thread_queue_Context *queue_context
)
{
  (void) queue;
  (void) queue_context;
  return the_thread;
}

Thread_Control *_Thread_queue_Flush_status_object_was_deleted(
  Thread_Control       *the_thread,
  Thread_queue_Queue   *queue,
  Thread_queue_Context *queue_context
)
{
  the_thread->Wait.return_code = STATUS_OBJECT_WAS_DELETED;

  (void) queue;
  (void) queue_context;
  return the_thread;
}

Thread_Control *_Thread_queue_Flush_status_unavailable(
  Thread_Control       *the_thread,
  Thread_queue_Queue   *queue,
  Thread_queue_Context *queue_context
)
{
  the_thread->Wait.return_code = STATUS_UNAVAILABLE;

  (void) queue;
  (void) queue_context;
  return the_thread;
}

size_t _Thread_queue_Flush_critical(
  Thread_queue_Queue            *queue,
  const Thread_queue_Operations *operations,
  Thread_queue_Flush_filter      filter,
  Thread_queue_Context          *queue_context
)
{
  size_t        flushed;
  size_t        priority_updates;
  Chain_Control unblock;
  Chain_Node   *node;
  Chain_Node   *tail;

  flushed = 0;
  priority_updates = 0;
  _Chain_Initialize_empty( &unblock );

  while ( true ) {
    Thread_queue_Heads *heads;
    Thread_Control     *first;
    bool                do_unblock;

    heads = queue->heads;
    if ( heads == NULL ) {
      break;
    }

    first = ( *operations->first )( heads );
    first = ( *filter )( first, queue, queue_context );
    if ( first == NULL ) {
      break;
    }

    /*
     * We do not have enough space in the queue context to collect all priority
     * updates, so clear it each time and accumulate the priority updates.
     */
    _Thread_queue_Context_clear_priority_updates( queue_context );

    do_unblock = _Thread_queue_Extract_locked(
      queue,
      operations,
      first,
      queue_context
    );
    if ( do_unblock ) {
      Scheduler_Node *scheduler_node;

      scheduler_node = _Thread_Scheduler_get_home_node( first );
      _Chain_Append_unprotected(
        &unblock,
        &scheduler_node->Wait.Priority.Node.Node.Chain
      );
    }

    priority_updates += _Thread_queue_Context_get_priority_updates(
      queue_context
    );
    ++flushed;
  }

  node = _Chain_First( &unblock );
  tail = _Chain_Tail( &unblock );

  if ( node != tail ) {
    Per_CPU_Control *cpu_self;

    cpu_self = _Thread_queue_Dispatch_disable( queue_context );
    _Thread_queue_Queue_release(
      queue,
      &queue_context->Lock_context.Lock_context
    );

    do {
      Scheduler_Node *scheduler_node;
      Thread_Control *the_thread;
      Chain_Node     *next;

      next = _Chain_Next( node );
      scheduler_node = SCHEDULER_NODE_OF_WAIT_PRIORITY_NODE( node );
      the_thread = _Scheduler_Node_get_owner( scheduler_node );
      _Thread_Remove_timer_and_unblock( the_thread, queue );

      node = next;
    } while ( node != tail );

    if ( priority_updates != 0 ) {
      Thread_Control  *owner;
      ISR_lock_Context lock_context;

      owner = queue->owner;
      _Assert( owner != NULL );
      _Thread_State_acquire( owner, &lock_context );
      _Scheduler_Update_priority( owner );
      _Thread_State_release( owner, &lock_context );
    }

    _Thread_Dispatch_enable( cpu_self );
  } else {
    _Thread_queue_Queue_release(
      queue,
      &queue_context->Lock_context.Lock_context
    );
  }

  return flushed;
}
