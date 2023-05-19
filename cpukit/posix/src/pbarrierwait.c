/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief Wait at a Barrier
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (c) 2017 embedded brains GmbH & Co. KG
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
