/**
 * @file
 *
 * @brief Thread Queue Flush
 * @ingroup ScoreThreadQ
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/threadimpl.h>

Thread_Control *_Thread_queue_Flush_default_filter(
  Thread_Control     *the_thread,
  Thread_queue_Queue *queue,
  ISR_lock_Context   *lock_context
)
{
  (void) queue;
  (void) lock_context;
  return the_thread;
}

size_t _Thread_queue_Do_flush_critical(
  Thread_queue_Queue            *queue,
  const Thread_queue_Operations *operations,
  Thread_queue_Flush_filter      filter,
#if defined(RTEMS_MULTIPROCESSING)
  Thread_queue_MP_callout        mp_callout,
  Objects_Id                     mp_id,
#endif
  ISR_lock_Context              *lock_context
)
{
  size_t         flushed;
  Chain_Control  unblock;
  Chain_Node    *node;
  Chain_Node    *tail;

  flushed = 0;
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
    first = ( *filter )( first, queue, lock_context );
    if ( first == NULL ) {
      break;
    }

    do_unblock = _Thread_queue_Extract_locked(
      queue,
      operations,
      first,
      mp_callout,
      mp_id
    );
    if ( do_unblock ) {
      _Chain_Append_unprotected( &unblock, &first->Wait.Node.Chain );
    }

    ++flushed;
  }

  node = _Chain_First( &unblock );
  tail = _Chain_Tail( &unblock );

  if ( node != tail ) {
    Per_CPU_Control *cpu_self;

    cpu_self = _Thread_Dispatch_disable_critical( lock_context );
    _Thread_queue_Queue_release( queue, lock_context );

    do {
      Thread_Control *the_thread;
      Chain_Node     *next;

      next = _Chain_Next( node );
      the_thread = THREAD_CHAIN_NODE_TO_THREAD( node );
      _Thread_Remove_timer_and_unblock( the_thread, queue );

      node = next;
    } while ( node != tail );

    _Thread_Dispatch_enable( cpu_self );
  } else {
    _Thread_queue_Queue_release( queue, lock_context );
  }

  return flushed;
}
