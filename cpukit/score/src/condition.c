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

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#if HAVE_STRUCT__THREAD_QUEUE_QUEUE

#include <sys/lock.h>
#include <errno.h>

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
    &queue_context->Lock_context
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
    &queue_context->Lock_context
  );
}

static Per_CPU_Control *_Condition_Do_wait(
  struct _Condition_Control *_condition,
  Watchdog_Interval          timeout,
  Thread_queue_Context      *queue_context
)
{
  Condition_Control *condition;
  Thread_Control    *executing;
  Per_CPU_Control   *cpu_self;

  condition = _Condition_Get( _condition );
  executing = _Condition_Queue_acquire_critical( condition, queue_context );
  cpu_self = _Thread_Dispatch_disable_critical( &queue_context->Lock_context );

  _Thread_queue_Context_set_expected_level( queue_context, 2 );
  _Thread_queue_Enqueue_critical(
    &condition->Queue.Queue,
    CONDITION_TQ_OPERATIONS,
    executing,
    STATES_WAITING_FOR_SYS_LOCK_CONDITION,
    timeout,
    queue_context
  );

  return cpu_self;
}

void _Condition_Wait(
  struct _Condition_Control *_condition,
  struct _Mutex_Control     *_mutex
)
{
  Thread_queue_Context  queue_context;
  Per_CPU_Control      *cpu_self;

  _Thread_queue_Context_initialize( &queue_context );
  _ISR_lock_ISR_disable( &queue_context.Lock_context );
  cpu_self = _Condition_Do_wait( _condition, 0, &queue_context );

  _Mutex_Release( _mutex );
  _Thread_Dispatch_enable( cpu_self );
  _Mutex_Acquire( _mutex );
}

int _Condition_Wait_timed(
  struct _Condition_Control *_condition,
  struct _Mutex_Control     *_mutex,
  const struct timespec     *abstime
)
{
  Thread_queue_Context  queue_context;
  Per_CPU_Control      *cpu_self;
  Thread_Control       *executing;
  int                   eno;
  Watchdog_Interval     ticks;

  _Thread_queue_Context_initialize( &queue_context );
  _ISR_lock_ISR_disable( &queue_context.Lock_context );

  switch ( _TOD_Absolute_timeout_to_ticks( abstime, &ticks ) ) {
    case TOD_ABSOLUTE_TIMEOUT_INVALID:
      _ISR_lock_ISR_enable( &queue_context.Lock_context );
      return EINVAL;
    case TOD_ABSOLUTE_TIMEOUT_IS_IN_PAST:
    case TOD_ABSOLUTE_TIMEOUT_IS_NOW:
      _ISR_lock_ISR_enable( &queue_context.Lock_context );
      return ETIMEDOUT;
    default:
      break;
  }

  cpu_self = _Condition_Do_wait( _condition, ticks, &queue_context );

  _Mutex_Release( _mutex );
  executing = cpu_self->executing;
  _Thread_Dispatch_enable( cpu_self );
  eno = STATUS_GET_POSIX( _Thread_Wait_get_status( executing ) );
  _Mutex_Acquire( _mutex );

  return eno;
}

void _Condition_Wait_recursive(
  struct _Condition_Control       *_condition,
  struct _Mutex_recursive_Control *_mutex
)
{
  Thread_queue_Context  queue_context;
  Per_CPU_Control      *cpu_self;
  unsigned int          nest_level;

  _Thread_queue_Context_initialize( &queue_context );
  _ISR_lock_ISR_disable( &queue_context.Lock_context );
  cpu_self = _Condition_Do_wait( _condition, 0, &queue_context );

  nest_level = _mutex->_nest_level;
  _mutex->_nest_level = 0;
  _Mutex_recursive_Release( _mutex );
  _Thread_Dispatch_enable( cpu_self );
  _Mutex_recursive_Acquire( _mutex );
  _mutex->_nest_level = nest_level;
}

int _Condition_Wait_recursive_timed(
  struct _Condition_Control       *_condition,
  struct _Mutex_recursive_Control *_mutex,
  const struct timespec           *abstime
)
{
  Thread_queue_Context   queue_context;
  Per_CPU_Control       *cpu_self;
  Thread_Control        *executing;
  int                    eno;
  unsigned int           nest_level;
  Watchdog_Interval      ticks;

  _Thread_queue_Context_initialize( &queue_context );
  _ISR_lock_ISR_disable( &queue_context.Lock_context );

  switch ( _TOD_Absolute_timeout_to_ticks( abstime, &ticks ) ) {
    case TOD_ABSOLUTE_TIMEOUT_INVALID:
      _ISR_lock_ISR_enable( &queue_context.Lock_context );
      return EINVAL;
    case TOD_ABSOLUTE_TIMEOUT_IS_IN_PAST:
    case TOD_ABSOLUTE_TIMEOUT_IS_NOW:
      _ISR_lock_ISR_enable( &queue_context.Lock_context );
      return ETIMEDOUT;
    default:
      break;
  }

  cpu_self = _Condition_Do_wait( _condition, ticks, &queue_context );

  nest_level = _mutex->_nest_level;
  _mutex->_nest_level = 0;
  _Mutex_recursive_Release( _mutex );
  executing = cpu_self->executing;
  _Thread_Dispatch_enable( cpu_self );
  eno = STATUS_GET_POSIX( _Thread_Wait_get_status( executing ) );
  _Mutex_recursive_Acquire( _mutex );
  _mutex->_nest_level = nest_level;

  return eno;
}

typedef struct {
  Thread_queue_Context Base;
  int                  count;
} Condition_Context;

static Thread_Control *_Condition_Flush_filter(
  Thread_Control       *the_thread,
  Thread_queue_Queue   *queue,
  Thread_queue_Context *queue_context
)
{
  Condition_Context *context;

  context = (Condition_Context *) queue_context;

  if ( context->count <= 0 ) {
    return NULL;
  }

  --context->count;

  return the_thread;
}

static void _Condition_Wake( struct _Condition_Control *_condition, int count )
{
  Condition_Control *condition;
  Condition_Context  context;

  condition = _Condition_Get( _condition );
  _Thread_queue_Context_initialize( &context.Base );
  _ISR_lock_ISR_disable( &context.Base.Lock_context );
  _Condition_Queue_acquire_critical( condition, &context.Base );

  /*
   * In common uses cases of condition variables there are normally no threads
   * on the queue, so check this condition early.
   */
  if ( __predict_true( _Thread_queue_Is_empty( &condition->Queue.Queue ) ) ) {
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

#endif /* HAVE_STRUCT__THREAD_QUEUE_QUEUE */
