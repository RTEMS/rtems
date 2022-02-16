/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup POSIX_SEMAPHORE POSIX Semaphores Support
 *
 * @brief Lock a Semaphore
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

#include <rtems/posix/semaphoreimpl.h>
#include <rtems/score/todimpl.h>

/*
 *  11.2.6 Lock a Semaphore, P1003.1b-1993, p.226
 *
 *  NOTE: P1003.4b/D8 adds sem_timedwait(), p. 27
 */

int sem_timedwait(
  sem_t                 *__restrict _sem,
  const struct timespec *__restrict abstime
)
{
  Sem_Control          *sem;
  Thread_queue_Context  queue_context;
  ISR_Level             level;
  Thread_Control       *executing;
  unsigned int          count;

  POSIX_SEMAPHORE_VALIDATE_OBJECT( _sem );

  sem = _Sem_Get( &_sem->_Semaphore );
  _Thread_queue_Context_initialize( &queue_context );
  _Thread_queue_Context_ISR_disable( &queue_context, level );
  executing = _Sem_Queue_acquire_critical( sem, &queue_context );

  count = sem->count;
  if ( RTEMS_PREDICT_TRUE( count > 0 ) ) {
    sem->count = count - 1;
    _Sem_Queue_release( sem, level, &queue_context );
    return 0;
  } else {
    Status_Control status;

    _Thread_queue_Context_set_thread_state(
      &queue_context,
      STATES_WAITING_FOR_SEMAPHORE
    );
    _Thread_queue_Context_set_enqueue_timeout_realtime_timespec(
      &queue_context,
      abstime,
      true
    );
    _Thread_queue_Context_set_ISR_level( &queue_context, level );
    _Thread_queue_Enqueue(
      &sem->Queue.Queue,
      SEMAPHORE_TQ_OPERATIONS,
      executing,
      &queue_context
    );
    status = _Thread_Wait_get_status( executing );
    return _POSIX_Zero_or_minus_one_plus_errno( status );
  }
}
