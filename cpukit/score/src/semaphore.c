/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScore
 *
 * @brief This source file contains the implementation of
 *   _Semaphore_Wait(), _Semaphore_Wait_timed_ticks(), _Semaphore_Try_wait(),
 *   _Semaphore_Post(), and _Semaphore_Post_binary().
 */

/*
 * Copyright (C) 2015, 2017 embedded brains GmbH & Co. KG
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

#include <rtems/score/semaphoreimpl.h>
#include <rtems/score/statesimpl.h>
#include <rtems/score/threadimpl.h>

#include <errno.h>

RTEMS_STATIC_ASSERT(
  offsetof( Sem_Control, Queue )
    == offsetof( struct _Semaphore_Control, _Queue ),
  SEMAPHORE_CONTROL_QUEUE
);

RTEMS_STATIC_ASSERT(
  offsetof( Sem_Control, count )
    == offsetof( struct _Semaphore_Control, _count ),
  SEMAPHORE_CONTROL_COUNT
);

RTEMS_STATIC_ASSERT(
  sizeof( Sem_Control ) == sizeof( struct _Semaphore_Control ),
  SEMAPHORE_CONTROL_SIZE
);

void _Semaphore_Wait( struct _Semaphore_Control *_sem )
{
  Sem_Control          *sem;
  ISR_Level             level;
  Thread_queue_Context  queue_context;
  Thread_Control       *executing;
  unsigned int          count;

  sem = _Sem_Get( _sem );
  _Thread_queue_Context_initialize( &queue_context );
  _Thread_queue_Context_ISR_disable( &queue_context, level );
  executing = _Sem_Queue_acquire_critical( sem, &queue_context );

  count = sem->count;
  if ( RTEMS_PREDICT_TRUE( count > 0 ) ) {
    sem->count = count - 1;
    _Sem_Queue_release( sem, level, &queue_context );
  } else {
    _Thread_queue_Context_set_thread_state(
      &queue_context,
      STATES_WAITING_FOR_SEMAPHORE
    );
    _Thread_queue_Context_set_enqueue_do_nothing_extra( &queue_context );
    _Thread_queue_Context_set_ISR_level( &queue_context, level );
    _Thread_queue_Enqueue(
      &sem->Queue.Queue,
      SEMAPHORE_TQ_OPERATIONS,
      executing,
      &queue_context
    );
  }
}

int _Semaphore_Wait_timed_ticks( struct _Semaphore_Control *_sem, uint32_t ticks )
{
  Sem_Control          *sem;
  ISR_Level             level;
  Thread_queue_Context  queue_context;
  Thread_Control       *executing;
  unsigned int          count;

  sem = _Sem_Get( _sem );
  _Thread_queue_Context_initialize( &queue_context );
  _Thread_queue_Context_ISR_disable( &queue_context, level );
  executing = _Sem_Queue_acquire_critical( sem, &queue_context );

  count = sem->count;
  if ( RTEMS_PREDICT_TRUE( count > 0 ) ) {
    sem->count = count - 1;
    _Sem_Queue_release( sem, level, &queue_context );
    return 0;
  } else {
    _Thread_queue_Context_set_thread_state(
      &queue_context,
      STATES_WAITING_FOR_SEMAPHORE
    );
    _Thread_queue_Context_set_enqueue_timeout_ticks( &queue_context, ticks );
    _Thread_queue_Context_set_ISR_level( &queue_context, level );
    _Thread_queue_Enqueue(
      &sem->Queue.Queue,
      SEMAPHORE_TQ_OPERATIONS,
      executing,
      &queue_context
    );
    return STATUS_GET_POSIX( _Thread_Wait_get_status( executing ) );
  }
}

int _Semaphore_Try_wait( struct _Semaphore_Control *_sem )
{
  Sem_Control          *sem;
  ISR_Level             level;
  Thread_queue_Context  queue_context;
  unsigned int          count;
  int                   eno;

  sem = _Sem_Get( _sem );
  _Thread_queue_Context_initialize( &queue_context );
  _Thread_queue_Context_ISR_disable( &queue_context, level );
  _Sem_Queue_acquire_critical( sem, &queue_context );

  count = sem->count;
  if ( RTEMS_PREDICT_TRUE( count > 0 ) ) {
    sem->count = count - 1;
    eno = 0;
  } else {
    eno = EAGAIN;
  }

  _Sem_Queue_release( sem, level, &queue_context );
  return eno;
}

void _Semaphore_Post( struct _Semaphore_Control *_sem )
{
  Sem_Control          *sem;
  ISR_Level             level;
  Thread_queue_Context  queue_context;
  Thread_queue_Heads   *heads;

  sem = _Sem_Get( _sem );
  _Thread_queue_Context_initialize( &queue_context );
  _Thread_queue_Context_ISR_disable( &queue_context, level );
  _Sem_Queue_acquire_critical( sem, &queue_context );

  heads = sem->Queue.Queue.heads;
  if ( RTEMS_PREDICT_TRUE( heads == NULL ) ) {
    ++sem->count;
    _Sem_Queue_release( sem, level, &queue_context );
  } else {
    _Thread_queue_Context_set_ISR_level( &queue_context, level );
    _Thread_queue_Surrender_no_priority(
      &sem->Queue.Queue,
      heads,
      &queue_context,
      SEMAPHORE_TQ_OPERATIONS
    );
  }
}

void _Semaphore_Post_binary( struct _Semaphore_Control *_sem )
{
  Sem_Control          *sem;
  ISR_Level             level;
  Thread_queue_Context  queue_context;
  Thread_queue_Heads   *heads;

  sem = _Sem_Get( _sem );
  _Thread_queue_Context_initialize( &queue_context );
  _Thread_queue_Context_ISR_disable( &queue_context, level );
  _Sem_Queue_acquire_critical( sem, &queue_context );

  heads = sem->Queue.Queue.heads;
  if ( RTEMS_PREDICT_TRUE( heads == NULL ) ) {
    sem->count = 1;
    _Sem_Queue_release( sem, level, &queue_context );
  } else {
    _Thread_queue_Context_set_ISR_level( &queue_context, level );
    _Thread_queue_Surrender_no_priority(
      &sem->Queue.Queue,
      heads,
      &queue_context,
      SEMAPHORE_TQ_OPERATIONS
    );
  }
}
