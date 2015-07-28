/*
 * Copyright (c) 2015 embedded brains GmbH.  All rights reserved.
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
  Condition_Control *condition,
  ISR_lock_Context  *lock_context
)
{
  Thread_Control *executing;

  executing = _Thread_Executing;
  _Thread_queue_Queue_acquire_critical(
    &condition->Queue.Queue,
    &executing->Potpourri_stats,
    lock_context
  );

  return executing;
}

static void _Condition_Queue_release(
  Condition_Control *condition,
  ISR_lock_Context  *lock_context
)
{
  _Thread_queue_Queue_release( &condition->Queue.Queue, lock_context );
}

static Per_CPU_Control *_Condition_Do_wait(
  struct _Condition_Control *_condition,
  Watchdog_Interval          timeout,
  ISR_lock_Context          *lock_context
)
{
  Condition_Control *condition;
  Thread_Control    *executing;
  Per_CPU_Control   *cpu_self;

  condition = _Condition_Get( _condition );
  executing = _Condition_Queue_acquire_critical( condition, lock_context );
  cpu_self = _Thread_Dispatch_disable_critical( lock_context );

  executing->Wait.return_code = 0;
  _Thread_queue_Enqueue_critical(
    &condition->Queue.Queue,
    CONDITION_TQ_OPERATIONS,
    executing,
    STATES_WAITING_FOR_SYS_LOCK_CONDITION,
    timeout,
    ETIMEDOUT,
    lock_context
  );

  return cpu_self;
}

void _Condition_Wait(
  struct _Condition_Control *_condition,
  struct _Mutex_Control     *_mutex
)
{
  ISR_lock_Context  lock_context;
  Per_CPU_Control  *cpu_self;

  _ISR_lock_ISR_disable( &lock_context );
  cpu_self = _Condition_Do_wait( _condition, 0, &lock_context );

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
  ISR_lock_Context   lock_context;
  Per_CPU_Control   *cpu_self;
  Thread_Control    *executing;
  int                eno;
  Watchdog_Interval  ticks;

  _ISR_lock_ISR_disable( &lock_context );

  switch ( _TOD_Absolute_timeout_to_ticks( abstime, &ticks ) ) {
    case TOD_ABSOLUTE_TIMEOUT_INVALID:
      _ISR_lock_ISR_enable( &lock_context );
      return EINVAL;
    case TOD_ABSOLUTE_TIMEOUT_IS_IN_PAST:
    case TOD_ABSOLUTE_TIMEOUT_IS_NOW:
      _ISR_lock_ISR_enable( &lock_context );
      return ETIMEDOUT;
    default:
      break;
  }

  cpu_self = _Condition_Do_wait( _condition, ticks, &lock_context );

  _Mutex_Release( _mutex );
  executing = cpu_self->executing;
  _Thread_Dispatch_enable( cpu_self );
  eno = (int) executing->Wait.return_code;
  _Mutex_Acquire( _mutex );

  return eno;
}

void _Condition_Wait_recursive(
  struct _Condition_Control       *_condition,
  struct _Mutex_recursive_Control *_mutex
)
{
  ISR_lock_Context  lock_context;
  Per_CPU_Control  *cpu_self;
  unsigned int      nest_level;

  _ISR_lock_ISR_disable( &lock_context );
  cpu_self = _Condition_Do_wait( _condition, 0, &lock_context );

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
  ISR_lock_Context   lock_context;
  Per_CPU_Control   *cpu_self;
  Thread_Control    *executing;
  int                eno;
  unsigned int       nest_level;
  Watchdog_Interval  ticks;

  _ISR_lock_ISR_disable( &lock_context );

  switch ( _TOD_Absolute_timeout_to_ticks( abstime, &ticks ) ) {
    case TOD_ABSOLUTE_TIMEOUT_INVALID:
      _ISR_lock_ISR_enable( &lock_context );
      return EINVAL;
    case TOD_ABSOLUTE_TIMEOUT_IS_IN_PAST:
    case TOD_ABSOLUTE_TIMEOUT_IS_NOW:
      _ISR_lock_ISR_enable( &lock_context );
      return ETIMEDOUT;
    default:
      break;
  }

  cpu_self = _Condition_Do_wait( _condition, ticks, &lock_context );

  nest_level = _mutex->_nest_level;
  _mutex->_nest_level = 0;
  _Mutex_recursive_Release( _mutex );
  executing = cpu_self->executing;
  _Thread_Dispatch_enable( cpu_self );
  eno = (int) executing->Wait.return_code;
  _Mutex_recursive_Acquire( _mutex );
  _mutex->_nest_level = nest_level;

  return eno;
}

static int _Condition_Wake( struct _Condition_Control *_condition, int count )
{
  Condition_Control  *condition;
  ISR_lock_Context    lock_context;
  Thread_queue_Heads *heads;
  Chain_Control       unblock;
  Chain_Node         *node;
  Chain_Node         *tail;
  int                 woken;

  condition = _Condition_Get( _condition );
  _ISR_lock_ISR_disable( &lock_context );
  _Condition_Queue_acquire_critical( condition, &lock_context );

  /*
   * In common uses cases of condition variables there are normally no threads
   * on the queue, so check this condition early.
   */
  heads = condition->Queue.Queue.heads;
  if ( __predict_true( heads == NULL ) ) {
    _Condition_Queue_release( condition, &lock_context );

    return 0;
  }

  woken = 0;
  _Chain_Initialize_empty( &unblock );
  while ( count > 0 && heads != NULL ) {
    const Thread_queue_Operations *operations;
    Thread_Control                *first;
    bool                           do_unblock;

    operations = CONDITION_TQ_OPERATIONS;
    first = ( *operations->first )( heads );

    do_unblock = _Thread_queue_Extract_locked(
      &condition->Queue.Queue,
      operations,
      first
    );
    if (do_unblock) {
      _Chain_Append_unprotected( &unblock, &first->Wait.Node.Chain );
    }

    ++woken;
    --count;
    heads = condition->Queue.Queue.heads;
  }

  node = _Chain_First( &unblock );
  tail = _Chain_Tail( &unblock );
  if ( node != tail ) {
    Per_CPU_Control *cpu_self;

    cpu_self = _Thread_Dispatch_disable_critical( &lock_context );
    _Condition_Queue_release( condition, &lock_context );

    do {
      Thread_Control *thread;
      Chain_Node     *next;

      next = _Chain_Next( node );
      thread = THREAD_CHAIN_NODE_TO_THREAD( node );
      _Watchdog_Remove_ticks( &thread->Timer );
      _Thread_Unblock( thread );

      node = next;
    } while ( node != tail );

    _Thread_Dispatch_enable( cpu_self );
  } else {
    _Condition_Queue_release( condition, &lock_context );
  }

  return woken;
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
