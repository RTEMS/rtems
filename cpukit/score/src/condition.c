/*
 * Copyright (c) 2015, 2016 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
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
  _Thread_queue_Context_set_timeout_argument( &context.Base, abstime );
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
  _Thread_queue_Context_set_timeout_argument( &context.Base, abstime );
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
