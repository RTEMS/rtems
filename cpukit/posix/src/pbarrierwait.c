/**
 * @file
 *
 * @brief Wait at a Barrier
 * @ingroup POSIXAPI
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (c) 2017 embedded brains GmbH
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/posix/barrierimpl.h>
#include <rtems/posix/posixapi.h>

int pthread_barrier_wait( pthread_barrier_t *_barrier )
{
  POSIX_Barrier_Control *barrier;
  Thread_queue_Context   queue_context;
  Thread_Control        *executing;
  unsigned int           waiting_threads;

  POSIX_BARRIER_VALIDATE_OBJECT( _barrier );

  barrier = _POSIX_Barrier_Get( _barrier );

  executing = _POSIX_Barrier_Queue_acquire( barrier, &queue_context );
  waiting_threads = barrier->waiting_threads;
  ++waiting_threads;

  if ( waiting_threads == barrier->count ) {
    barrier->waiting_threads = 0;
    _Thread_queue_Flush_critical(
      &barrier->Queue.Queue,
      POSIX_BARRIER_TQ_OPERATIONS,
      _Thread_queue_Flush_default_filter,
      &queue_context
    );
    return PTHREAD_BARRIER_SERIAL_THREAD;
  } else {
    barrier->waiting_threads = waiting_threads;
    _Thread_queue_Context_set_thread_state(
      &queue_context,
      STATES_WAITING_FOR_BARRIER
    );
    _Thread_queue_Context_set_enqueue_do_nothing_extra( &queue_context );
    _Thread_queue_Enqueue(
      &barrier->Queue.Queue,
      POSIX_BARRIER_TQ_OPERATIONS,
      executing,
      &queue_context
    );
    return 0;
  }
}
