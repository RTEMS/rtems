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

#include <rtems/score/assert.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/threadqimpl.h>
#include <rtems/score/todimpl.h>

#define MUTEX_TQ_OPERATIONS &_Thread_queue_Operations_priority

typedef struct {
  Thread_queue_Syslock_queue Queue;
  Thread_Control *owner;
} Mutex_Control;

RTEMS_STATIC_ASSERT(
  offsetof( Mutex_Control, Queue )
    == offsetof( struct _Mutex_Control, _Queue ),
  MUTEX_CONTROL_QUEUE
);

RTEMS_STATIC_ASSERT(
  offsetof( Mutex_Control, owner )
    == offsetof( struct _Mutex_Control, _owner ),
  MUTEX_CONTROL_OWNER
);

RTEMS_STATIC_ASSERT(
  sizeof( Mutex_Control ) == sizeof( struct _Mutex_Control ),
  MUTEX_CONTROL_SIZE
);

typedef struct {
  Mutex_Control Mutex;
  unsigned int nest_level;
} Mutex_recursive_Control;

RTEMS_STATIC_ASSERT(
  offsetof( Mutex_recursive_Control, Mutex )
    == offsetof( struct _Mutex_recursive_Control, _Mutex ),
  MUTEX_RECURSIVE_CONTROL_MUTEX
);

RTEMS_STATIC_ASSERT(
  offsetof( Mutex_recursive_Control, nest_level )
    == offsetof( struct _Mutex_recursive_Control, _nest_level ),
  MUTEX_RECURSIVE_CONTROL_NEST_LEVEL
);

RTEMS_STATIC_ASSERT(
  sizeof( Mutex_recursive_Control )
    == sizeof( struct _Mutex_recursive_Control ),
  MUTEX_RECURSIVE_CONTROL_SIZE
);

static Mutex_Control *_Mutex_Get( struct _Mutex_Control *_mutex )
{
  return (Mutex_Control *) _mutex;
}

static Thread_Control *_Mutex_Queue_acquire(
  Mutex_Control    *mutex,
  ISR_lock_Context *lock_context
)
{
  Thread_Control *executing;

  _ISR_lock_ISR_disable( lock_context );
  executing = _Thread_Executing;
  _Thread_queue_Queue_acquire_critical(
    &mutex->Queue.Queue,
    &executing->Potpourri_stats,
    lock_context
  );

  return executing;
}

static void _Mutex_Queue_release(
  Mutex_Control    *mutex,
  ISR_lock_Context *lock_context
)
{
  _Thread_queue_Queue_release( &mutex->Queue.Queue, lock_context );
}

static void _Mutex_Acquire_slow(
  Mutex_Control     *mutex,
  Thread_Control    *owner,
  Thread_Control    *executing,
  Watchdog_Interval  timeout,
  ISR_lock_Context  *lock_context
)
{
  /* Priority inheritance */
  _Thread_Raise_priority( owner, executing->current_priority );

  _Thread_queue_Enqueue_critical(
    &mutex->Queue.Queue,
    MUTEX_TQ_OPERATIONS,
    executing,
    STATES_WAITING_FOR_SYS_LOCK_MUTEX,
    timeout,
    ETIMEDOUT,
    lock_context
  );
}

static void _Mutex_Release_slow(
  Mutex_Control      *mutex,
  Thread_Control     *executing,
  Thread_queue_Heads *heads,
  bool                keep_priority,
  ISR_lock_Context   *lock_context
)
{
  if (heads != NULL) {
    const Thread_queue_Operations *operations;
    Thread_Control *first;

    operations = MUTEX_TQ_OPERATIONS;
    first = ( *operations->first )( heads );

    mutex->owner = first;
    _Thread_queue_Extract_critical(
      &mutex->Queue.Queue,
      operations,
      first,
      lock_context
    );
  } else {
    _Mutex_Queue_release( mutex, lock_context);
  }

  if ( !keep_priority ) {
    Per_CPU_Control *cpu_self;

    cpu_self = _Thread_Dispatch_disable();
    _Thread_Restore_priority( executing );
    _Thread_Dispatch_enable( cpu_self );
  }
}

static void _Mutex_Release_critical(
  Mutex_Control *mutex,
  Thread_Control *executing,
  ISR_lock_Context *lock_context
)
{
  Thread_queue_Heads *heads;
  bool keep_priority;

  mutex->owner = NULL;

  --executing->resource_count;

  /*
   * Ensure that the owner resource count is visible to all other
   * processors and that we read the latest priority restore
   * hint.
   */
  _Atomic_Fence( ATOMIC_ORDER_ACQ_REL );

  heads = mutex->Queue.Queue.heads;
  keep_priority = _Thread_Owns_resources( executing )
    || !executing->priority_restore_hint;

  if ( __predict_true( heads == NULL && keep_priority ) ) {
    _Mutex_Queue_release( mutex, lock_context );
  } else {
    _Mutex_Release_slow(
      mutex,
      executing,
      heads,
      keep_priority,
      lock_context
    );
  }
}

void _Mutex_Acquire( struct _Mutex_Control *_mutex )
{
  Mutex_Control    *mutex;
  ISR_lock_Context  lock_context;
  Thread_Control   *executing;
  Thread_Control   *owner;

  mutex = _Mutex_Get( _mutex );
  executing = _Mutex_Queue_acquire( mutex, &lock_context );

  owner = mutex->owner;
  ++executing->resource_count;

  if ( __predict_true( owner == NULL ) ) {
    mutex->owner = executing;
    _Mutex_Queue_release( mutex, &lock_context );
  } else {
    _Mutex_Acquire_slow( mutex, owner, executing, 0, &lock_context );
  }
}

int _Mutex_Acquire_timed(
  struct _Mutex_Control *_mutex,
  const struct timespec *abstime
)
{
  Mutex_Control    *mutex;
  ISR_lock_Context  lock_context;
  Thread_Control   *executing;
  Thread_Control   *owner;

  mutex = _Mutex_Get( _mutex );
  executing = _Mutex_Queue_acquire( mutex, &lock_context );

  owner = mutex->owner;
  ++executing->resource_count;

  if ( __predict_true( owner == NULL ) ) {
    mutex->owner = executing;
    _Mutex_Queue_release( mutex, &lock_context );

    return 0;
  } else {
    Watchdog_Interval ticks;

    switch ( _TOD_Absolute_timeout_to_ticks( abstime, &ticks ) ) {
      case TOD_ABSOLUTE_TIMEOUT_INVALID:
        _Mutex_Queue_release( mutex, &lock_context );
        return EINVAL;
      case TOD_ABSOLUTE_TIMEOUT_IS_IN_PAST:
      case TOD_ABSOLUTE_TIMEOUT_IS_NOW:
        _Mutex_Queue_release( mutex, &lock_context );
        return ETIMEDOUT;
      default:
        break;
    }

    executing->Wait.return_code = 0;
    _Mutex_Acquire_slow( mutex, owner, executing, ticks, &lock_context );

    return (int) executing->Wait.return_code;
  }
}

int _Mutex_Try_acquire( struct _Mutex_Control *_mutex )
{
  Mutex_Control    *mutex;
  ISR_lock_Context  lock_context;
  Thread_Control   *executing;
  Thread_Control   *owner;
  int               success;

  mutex = _Mutex_Get( _mutex );
  executing = _Mutex_Queue_acquire( mutex, &lock_context );

  owner = mutex->owner;

  if ( __predict_true( owner == NULL ) ) {
    mutex->owner = executing;
    ++executing->resource_count;
    success = 1;
  } else {
    success = 0;
  }

  _Mutex_Queue_release( mutex, &lock_context );

  return success;
}

void _Mutex_Release( struct _Mutex_Control *_mutex )
{
  Mutex_Control    *mutex;
  ISR_lock_Context  lock_context;
  Thread_Control   *executing;

  mutex = _Mutex_Get( _mutex );
  executing = _Mutex_Queue_acquire( mutex, &lock_context );

  _Assert( mutex->owner == executing );

  _Mutex_Release_critical( mutex, executing, &lock_context );
}

static Mutex_recursive_Control *_Mutex_recursive_Get(
  struct _Mutex_recursive_Control *_mutex
)
{
  return (Mutex_recursive_Control *) _mutex;
}

void _Mutex_recursive_Acquire( struct _Mutex_recursive_Control *_mutex )
{
  Mutex_recursive_Control *mutex;
  ISR_lock_Context         lock_context;
  Thread_Control          *executing;
  Thread_Control          *owner;

  mutex = _Mutex_recursive_Get( _mutex );
  executing = _Mutex_Queue_acquire( &mutex->Mutex, &lock_context );

  owner = mutex->Mutex.owner;

  if ( __predict_true( owner == NULL ) ) {
    mutex->Mutex.owner = executing;
    ++executing->resource_count;
    _Mutex_Queue_release( &mutex->Mutex, &lock_context );
  } else if ( owner == executing ) {
    ++mutex->nest_level;
    _Mutex_Queue_release( &mutex->Mutex, &lock_context );
  } else {
    _Mutex_Acquire_slow( &mutex->Mutex, owner, executing, 0, &lock_context );
  }
}

int _Mutex_recursive_Acquire_timed(
  struct _Mutex_recursive_Control *_mutex,
  const struct timespec           *abstime
)
{
  Mutex_recursive_Control *mutex;
  ISR_lock_Context         lock_context;
  Thread_Control          *executing;
  Thread_Control          *owner;

  mutex = _Mutex_recursive_Get( _mutex );
  executing = _Mutex_Queue_acquire( &mutex->Mutex, &lock_context );

  owner = mutex->Mutex.owner;

  if ( __predict_true( owner == NULL ) ) {
    mutex->Mutex.owner = executing;
    ++executing->resource_count;
    _Mutex_Queue_release( &mutex->Mutex, &lock_context );

    return 0;
  } else if ( owner == executing ) {
    ++mutex->nest_level;
    _Mutex_Queue_release( &mutex->Mutex, &lock_context );

    return 0;
  } else {
    Watchdog_Interval ticks;

    switch ( _TOD_Absolute_timeout_to_ticks( abstime, &ticks ) ) {
      case TOD_ABSOLUTE_TIMEOUT_INVALID:
        _Mutex_Queue_release( &mutex->Mutex, &lock_context );
        return EINVAL;
      case TOD_ABSOLUTE_TIMEOUT_IS_IN_PAST:
      case TOD_ABSOLUTE_TIMEOUT_IS_NOW:
        _Mutex_Queue_release( &mutex->Mutex, &lock_context );
        return ETIMEDOUT;
      default:
        break;
    }

    executing->Wait.return_code = 0;
    _Mutex_Acquire_slow(
      &mutex->Mutex,
      owner,
      executing,
      ticks,
      &lock_context
    );

    return (int) executing->Wait.return_code;
  }
}

int _Mutex_recursive_Try_acquire( struct _Mutex_recursive_Control *_mutex )
{
  Mutex_recursive_Control *mutex;
  ISR_lock_Context         lock_context;
  Thread_Control          *executing;
  Thread_Control          *owner;
  int success;

  mutex = _Mutex_recursive_Get( _mutex );
  executing = _Mutex_Queue_acquire( &mutex->Mutex, &lock_context );

  owner = mutex->Mutex.owner;

  if ( __predict_true( owner == NULL ) ) {
    mutex->Mutex.owner = executing;
    ++executing->resource_count;
    success = 1;
  } else if ( owner == executing ) {
    ++mutex->nest_level;
    success = 1;
  } else {
    success = 0;
  }

  _Mutex_Queue_release( &mutex->Mutex, &lock_context );

  return success;
}

void _Mutex_recursive_Release( struct _Mutex_recursive_Control *_mutex )
{
  Mutex_recursive_Control *mutex;
  ISR_lock_Context         lock_context;
  Thread_Control          *executing;
  unsigned int             nest_level;

  mutex = _Mutex_recursive_Get( _mutex );
  executing = _Mutex_Queue_acquire( &mutex->Mutex, &lock_context );

  _Assert( mutex->Mutex.owner == executing );

  nest_level = mutex->nest_level;

  if ( __predict_true( nest_level == 0 ) ) {
    _Mutex_Release_critical( &mutex->Mutex, executing, &lock_context );
  } else {
    mutex->nest_level = nest_level - 1;

    _Mutex_Queue_release( &mutex->Mutex, &lock_context );
  }
}

#endif /* HAVE_STRUCT__THREAD_QUEUE_QUEUE */
