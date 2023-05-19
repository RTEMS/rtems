/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScore
 *
 * @brief This source file contains the implementation of
 *   _Condition_Wait(), _Condition_Wait_timed(), _Condition_Wait_recursive(),
 *   _Condition_Wait_recursive_timed(),
 *   _Condition_Wait_recursive_timed_ticks(), _Condition_Signal(), and
 *   _Condition_Broadcast().
 */

/*
 * Copyright (C) 2015, 2016 embedded brains GmbH & Co. KG
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

#include <sys/lock.h>
#include <errno.h>
#include <limits.h>

#include <rtems/score/atomic.h>
#include <rtems/score/chainimpl.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/threadqimpl.h>
#include <rtems/score/todimpl.h>
#include <rtems/score/watchdogimpl.h>

#define CONDITION_TQ_OPERATIONS &_Thread_queue_Operations_FIFO

typedef struct {
  Thread_queue_Syslock_queue Queue;
} Condition_Control;

RTEMS_STATIC_ASSERT(
  offsetof( Condition_Control, Queue )
    == offsetof( struct _Condition_Control, _Queue ),
  CONDITION_CONTROL_QUEUE
);

RTEMS_STATIC_ASSERT(
  sizeof( Condition_Control ) == sizeof( struct _Condition_Control ),
  CONDITION_CONTROL_SIZE
);

static Condition_Control *_Condition_Get(
  struct _Condition_Control *_condition
)
{
  return (Condition_Control *) _condition;
}

static Thread_Control *_Condition_Queue_acquire_critical(
  Condition_Control    *condition,
  Thread_queue_Context *queue_context
)
{
  Thread_Control *executing;

  executing = _Thread_Executing;
  _Thread_queue_Queue_acquire_critical(
    &condition->Queue.Queue,
    &executing->Potpourri_stats,
    &queue_context->Lock_context.Lock_context
  );

  return executing;
}

static void _Condition_Queue_release(
  Condition_Control    *condition,
  Thread_queue_Context *queue_context
)
{
  _Thread_queue_Queue_release(
    &condition->Queue.Queue,
    &queue_context->Lock_context.Lock_context
  );
}

typedef struct {
  Thread_queue_Context   Base;
  struct _Mutex_Control *mutex;
} Condition_Enqueue_context;

static void _Condition_Mutex_release( Thread_queue_Context *queue_context )
{
  Condition_Enqueue_context *context;

  context = (Condition_Enqueue_context *) queue_context;
  _Mutex_Release( context->mutex );
}

static void _Condition_Enqueue_no_timeout(
  Thread_queue_Queue   *queue,
  Thread_Control       *the_thread,
  Per_CPU_Control      *cpu_self,
  Thread_queue_Context *queue_context
)
{
  _Condition_Mutex_release( queue_context );
}

static void _Condition_Enqueue_with_timeout(
  Thread_queue_Queue   *queue,
  Thread_Control       *the_thread,
  Per_CPU_Control      *cpu_self,
  Thread_queue_Context *queue_context
)
{
  _Thread_queue_Add_timeout_realtime_timespec(
    queue,
    the_thread,
    cpu_self,
    queue_context
  );
  _Condition_Mutex_release( queue_context );
}

static Thread_Control *_Condition_Do_wait(
  struct _Condition_Control *_condition,
  struct _Mutex_Control     *_mutex,
  Condition_Enqueue_context *context
)
{
  Condition_Control *condition;
  Thread_Control    *executing;

  context->mutex = _mutex;
  condition = _Condition_Get( _condition );
  _ISR_lock_ISR_disable( &context->Base.Lock_context.Lock_context );
  executing = _Condition_Queue_acquire_critical( condition, &context->Base );
  _Thread_queue_Context_set_thread_state(
    &context->Base,
    STATES_WAITING_FOR_CONDITION_VARIABLE
  );
  _Thread_queue_Enqueue(
    &condition->Queue.Queue,
    CONDITION_TQ_OPERATIONS,
    executing,
    &context->Base
  );

  return executing;
}

void _Condition_Wait(
  struct _Condition_Control *_condition,
  struct _Mutex_Control     *_mutex
)
{
  Condition_Enqueue_context context;

  _Thread_queue_Context_initialize( &context.Base );
  _Thread_queue_Context_set_enqueue_callout(
    &context.Base,
    _Condition_Enqueue_no_timeout
  );
  _Condition_Do_wait( _condition, _mutex, &context );
  _Mutex_Acquire( _mutex );
}

int _Condition_Wait_timed(
  struct _Condition_Control *_condition,
  struct _Mutex_Control     *_mutex,
  const struct timespec     *abstime
)
{
  Condition_Enqueue_context  context;
  Thread_Control            *executing;
  int                        eno;

  _Thread_queue_Context_initialize( &context.Base );
  _Thread_queue_Context_set_enqueue_callout(
    &context.Base,
    _Condition_Enqueue_with_timeout
  );
  _Thread_queue_Context_set_timeout_argument( &context.Base, abstime, true );
  executing = _Condition_Do_wait( _condition, _mutex, &context );
  eno = STATUS_GET_POSIX( _Thread_Wait_get_status( executing ) );
  _Mutex_Acquire( _mutex );

  return eno;
}

static unsigned int _Condition_Unnest_mutex(
  struct _Mutex_recursive_Control *_mutex
)
{
  unsigned int nest_level;

  nest_level = _mutex->_nest_level;
  _mutex->_nest_level = 0;

  return nest_level;
}

void _Condition_Wait_recursive(
  struct _Condition_Control       *_condition,
  struct _Mutex_recursive_Control *_mutex
)
{
  Condition_Enqueue_context context;
  unsigned int              nest_level;

  _Thread_queue_Context_initialize( &context.Base );
  _Thread_queue_Context_set_enqueue_callout(
    &context.Base,
    _Condition_Enqueue_no_timeout
  );
  nest_level = _Condition_Unnest_mutex( _mutex );
  _Condition_Do_wait( _condition, &_mutex->_Mutex, &context );
  _Mutex_recursive_Acquire( _mutex );
  _mutex->_nest_level = nest_level;
}

int _Condition_Wait_recursive_timed(
  struct _Condition_Control       *_condition,
  struct _Mutex_recursive_Control *_mutex,
  const struct timespec           *abstime
)
{
  Condition_Enqueue_context  context;
  Thread_Control            *executing;
  int                        eno;
  unsigned int               nest_level;

  _Thread_queue_Context_initialize( &context.Base );
  _Thread_queue_Context_set_enqueue_callout(
    &context.Base,
    _Condition_Enqueue_with_timeout
  );
  _Thread_queue_Context_set_timeout_argument( &context.Base, abstime, true );
  nest_level = _Condition_Unnest_mutex( _mutex );
  executing = _Condition_Do_wait( _condition, &_mutex->_Mutex, &context );
  eno = STATUS_GET_POSIX( _Thread_Wait_get_status( executing ) );
  _Mutex_recursive_Acquire( _mutex );
  _mutex->_nest_level = nest_level;

  return eno;
}

typedef struct {
  Thread_queue_Context Base;
  int                  count;
} Condition_Flush_context;

static Thread_Control *_Condition_Flush_filter(
  Thread_Control       *the_thread,
  Thread_queue_Queue   *queue,
  Thread_queue_Context *queue_context
)
{
  Condition_Flush_context *context;

  context = (Condition_Flush_context *) queue_context;

  if ( context->count <= 0 ) {
    return NULL;
  }

  --context->count;

  return the_thread;
}

static void _Condition_Wake( struct _Condition_Control *_condition, int count )
{
  Condition_Control       *condition;
  Condition_Flush_context  context;

  condition = _Condition_Get( _condition );
  _Thread_queue_Context_initialize( &context.Base );
  _ISR_lock_ISR_disable( &context.Base.Lock_context.Lock_context );
  _Condition_Queue_acquire_critical( condition, &context.Base );

  /*
   * In common uses cases of condition variables there are normally no threads
   * on the queue, so check this condition early.
   */
  if (
    RTEMS_PREDICT_TRUE( _Thread_queue_Is_empty( &condition->Queue.Queue ) )
  ) {
    _Condition_Queue_release( condition, &context.Base );
    return;
  }

  context.count = count;
  _Thread_queue_Flush_critical(
    &condition->Queue.Queue,
    CONDITION_TQ_OPERATIONS,
    _Condition_Flush_filter,
    &context.Base
  );
}

void _Condition_Signal( struct _Condition_Control *_condition )
{
  _Condition_Wake( _condition, 1 );
}

void _Condition_Broadcast( struct _Condition_Control *_condition )
{
  _Condition_Wake( _condition, INT_MAX );
}
