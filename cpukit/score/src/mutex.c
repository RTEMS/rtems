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
} Mutex_Control;

RTEMS_STATIC_ASSERT(
  offsetof( Mutex_Control, Queue )
    == offsetof( struct _Mutex_Control, _Queue ),
  MUTEX_CONTROL_QUEUE
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
  Mutex_Control        *mutex,
  Thread_queue_Context *queue_context
)
{
  Thread_Control *executing;

  _ISR_lock_ISR_disable( &queue_context->Lock_context );
  executing = _Thread_Executing;
  _Thread_queue_Queue_acquire_critical(
    &mutex->Queue.Queue,
    &executing->Potpourri_stats,
    &queue_context->Lock_context
  );

  return executing;
}

static void _Mutex_Queue_release(
  Mutex_Control        *mutex,
  Thread_queue_Context *queue_context
)
{
  _Thread_queue_Queue_release(
    &mutex->Queue.Queue,
    &queue_context->Lock_context
  );
}

static void _Mutex_Acquire_slow(
  Mutex_Control        *mutex,
  Thread_Control       *owner,
  Thread_Control       *executing,
  Watchdog_Interval     timeout,
  Thread_queue_Context *queue_context
)
{
  _Thread_Inherit_priority( owner, executing );
  _Thread_queue_Context_set_expected_level( queue_context, 1 );
  _Thread_queue_Enqueue_critical(
    &mutex->Queue.Queue,
    MUTEX_TQ_OPERATIONS,
    executing,
    STATES_WAITING_FOR_SYS_LOCK_MUTEX,
    timeout,
    queue_context
  );
}

static void _Mutex_Release_slow(
  Mutex_Control        *mutex,
  Thread_Control       *executing,
  Thread_queue_Heads   *heads,
  bool                  keep_priority,
  Thread_queue_Context *queue_context
)
{
  if (heads != NULL) {
    const Thread_queue_Operations *operations;
    Thread_Control                *first;
    bool                           unblock;

    operations = MUTEX_TQ_OPERATIONS;
    first = ( *operations->first )( heads );

    mutex->Queue.Queue.owner = first;
    ++first->resource_count;
    unblock = _Thread_queue_Extract_locked(
      &mutex->Queue.Queue,
      operations,
      first,
      queue_context
    );
    _Thread_queue_Boost_priority( &mutex->Queue.Queue, first );
    _Thread_queue_Unblock_critical(
      unblock,
      &mutex->Queue.Queue,
      first,
      &queue_context->Lock_context
    );
  } else {
    _Mutex_Queue_release( mutex, queue_context );
  }

  if ( !keep_priority ) {
    Per_CPU_Control *cpu_self;

    cpu_self = _Thread_Dispatch_disable();
    _Thread_Restore_priority( executing );
    _Thread_Dispatch_enable( cpu_self );
  }
}

static void _Mutex_Release_critical(
  Mutex_Control        *mutex,
  Thread_Control       *executing,
  Thread_queue_Context *queue_context
)
{
  Thread_queue_Heads *heads;
  bool keep_priority;

  mutex->Queue.Queue.owner = NULL;

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
    _Mutex_Queue_release( mutex, queue_context );
  } else {
    _Mutex_Release_slow(
      mutex,
      executing,
      heads,
      keep_priority,
      queue_context
    );
  }
}

void _Mutex_Acquire( struct _Mutex_Control *_mutex )
{
  Mutex_Control        *mutex;
  Thread_queue_Context  queue_context;
  Thread_Control       *executing;
  Thread_Control       *owner;

  mutex = _Mutex_Get( _mutex );
  _Thread_queue_Context_initialize( &queue_context );
  executing = _Mutex_Queue_acquire( mutex, &queue_context );

  owner = mutex->Queue.Queue.owner;

  if ( __predict_true( owner == NULL ) ) {
    mutex->Queue.Queue.owner = executing;
    ++executing->resource_count;
    _Mutex_Queue_release( mutex, &queue_context );
  } else {
    _Mutex_Acquire_slow( mutex, owner, executing, 0, &queue_context );
  }
}

int _Mutex_Acquire_timed(
  struct _Mutex_Control *_mutex,
  const struct timespec *abstime
)
{
  Mutex_Control        *mutex;
  Thread_queue_Context  queue_context;
  Thread_Control       *executing;
  Thread_Control       *owner;

  mutex = _Mutex_Get( _mutex );
  _Thread_queue_Context_initialize( &queue_context );
  executing = _Mutex_Queue_acquire( mutex, &queue_context );

  owner = mutex->Queue.Queue.owner;

  if ( __predict_true( owner == NULL ) ) {
    mutex->Queue.Queue.owner = executing;
    ++executing->resource_count;
    _Mutex_Queue_release( mutex, &queue_context );

    return 0;
  } else {
    Watchdog_Interval ticks;

    switch ( _TOD_Absolute_timeout_to_ticks( abstime, &ticks ) ) {
      case TOD_ABSOLUTE_TIMEOUT_INVALID:
        _Mutex_Queue_release( mutex, &queue_context );
        return EINVAL;
      case TOD_ABSOLUTE_TIMEOUT_IS_IN_PAST:
      case TOD_ABSOLUTE_TIMEOUT_IS_NOW:
        _Mutex_Queue_release( mutex, &queue_context );
        return ETIMEDOUT;
      default:
        break;
    }

    _Mutex_Acquire_slow( mutex, owner, executing, ticks, &queue_context );

    return STATUS_GET_POSIX( _Thread_Wait_get_status( executing ) );
  }
}

int _Mutex_Try_acquire( struct _Mutex_Control *_mutex )
{
  Mutex_Control        *mutex;
  Thread_queue_Context  queue_context;
  Thread_Control       *executing;
  Thread_Control       *owner;
  int                   eno;

  mutex = _Mutex_Get( _mutex );
  _Thread_queue_Context_initialize( &queue_context );
  executing = _Mutex_Queue_acquire( mutex, &queue_context );

  owner = mutex->Queue.Queue.owner;

  if ( __predict_true( owner == NULL ) ) {
    mutex->Queue.Queue.owner = executing;
    ++executing->resource_count;
    eno = 0;
  } else {
    eno = EBUSY;
  }

  _Mutex_Queue_release( mutex, &queue_context );

  return eno;
}

void _Mutex_Release( struct _Mutex_Control *_mutex )
{
  Mutex_Control        *mutex;
  Thread_queue_Context  queue_context;
  Thread_Control       *executing;

  mutex = _Mutex_Get( _mutex );
  _Thread_queue_Context_initialize( &queue_context );
  executing = _Mutex_Queue_acquire( mutex, &queue_context );

  _Assert( mutex->Queue.Queue.owner == executing );

  _Mutex_Release_critical( mutex, executing, &queue_context );
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
  Thread_queue_Context     queue_context;
  Thread_Control          *executing;
  Thread_Control          *owner;

  mutex = _Mutex_recursive_Get( _mutex );
  _Thread_queue_Context_initialize( &queue_context );
  executing = _Mutex_Queue_acquire( &mutex->Mutex, &queue_context );

  owner = mutex->Mutex.Queue.Queue.owner;

  if ( __predict_true( owner == NULL ) ) {
    mutex->Mutex.Queue.Queue.owner = executing;
    ++executing->resource_count;
    _Mutex_Queue_release( &mutex->Mutex, &queue_context );
  } else if ( owner == executing ) {
    ++mutex->nest_level;
    _Mutex_Queue_release( &mutex->Mutex, &queue_context );
  } else {
    _Mutex_Acquire_slow( &mutex->Mutex, owner, executing, 0, &queue_context );
  }
}

int _Mutex_recursive_Acquire_timed(
  struct _Mutex_recursive_Control *_mutex,
  const struct timespec           *abstime
)
{
  Mutex_recursive_Control *mutex;
  Thread_queue_Context     queue_context;
  Thread_Control          *executing;
  Thread_Control          *owner;

  mutex = _Mutex_recursive_Get( _mutex );
  _Thread_queue_Context_initialize( &queue_context );
  executing = _Mutex_Queue_acquire( &mutex->Mutex, &queue_context );

  owner = mutex->Mutex.Queue.Queue.owner;

  if ( __predict_true( owner == NULL ) ) {
    mutex->Mutex.Queue.Queue.owner = executing;
    ++executing->resource_count;
    _Mutex_Queue_release( &mutex->Mutex, &queue_context );

    return 0;
  } else if ( owner == executing ) {
    ++mutex->nest_level;
    _Mutex_Queue_release( &mutex->Mutex, &queue_context );

    return 0;
  } else {
    Watchdog_Interval ticks;

    switch ( _TOD_Absolute_timeout_to_ticks( abstime, &ticks ) ) {
      case TOD_ABSOLUTE_TIMEOUT_INVALID:
        _Mutex_Queue_release( &mutex->Mutex, &queue_context );
        return EINVAL;
      case TOD_ABSOLUTE_TIMEOUT_IS_IN_PAST:
      case TOD_ABSOLUTE_TIMEOUT_IS_NOW:
        _Mutex_Queue_release( &mutex->Mutex, &queue_context );
        return ETIMEDOUT;
      default:
        break;
    }

    _Mutex_Acquire_slow(
      &mutex->Mutex,
      owner,
      executing,
      ticks,
      &queue_context
    );

    return STATUS_GET_POSIX( _Thread_Wait_get_status( executing ) );
  }
}

int _Mutex_recursive_Try_acquire( struct _Mutex_recursive_Control *_mutex )
{
  Mutex_recursive_Control *mutex;
  Thread_queue_Context     queue_context;
  Thread_Control          *executing;
  Thread_Control          *owner;
  int                      eno;

  mutex = _Mutex_recursive_Get( _mutex );
  _Thread_queue_Context_initialize( &queue_context );
  executing = _Mutex_Queue_acquire( &mutex->Mutex, &queue_context );

  owner = mutex->Mutex.Queue.Queue.owner;

  if ( __predict_true( owner == NULL ) ) {
    mutex->Mutex.Queue.Queue.owner = executing;
    ++executing->resource_count;
    eno = 0;
  } else if ( owner == executing ) {
    ++mutex->nest_level;
    eno = 0;
  } else {
    eno = EBUSY;
  }

  _Mutex_Queue_release( &mutex->Mutex, &queue_context );

  return eno;
}

void _Mutex_recursive_Release( struct _Mutex_recursive_Control *_mutex )
{
  Mutex_recursive_Control *mutex;
  Thread_queue_Context     queue_context;
  Thread_Control          *executing;
  unsigned int             nest_level;

  mutex = _Mutex_recursive_Get( _mutex );
  _Thread_queue_Context_initialize( &queue_context );
  executing = _Mutex_Queue_acquire( &mutex->Mutex, &queue_context );

  _Assert( mutex->Mutex.Queue.Queue.owner == executing );

  nest_level = mutex->nest_level;

  if ( __predict_true( nest_level == 0 ) ) {
    _Mutex_Release_critical( &mutex->Mutex, executing, &queue_context );
  } else {
    mutex->nest_level = nest_level - 1;

    _Mutex_Queue_release( &mutex->Mutex, &queue_context );
  }
}

#endif /* HAVE_STRUCT__THREAD_QUEUE_QUEUE */
