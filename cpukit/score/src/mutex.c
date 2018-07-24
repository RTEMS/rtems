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

#include <sys/lock.h>
#include <errno.h>

#include <rtems/score/assert.h>
#include <rtems/score/muteximpl.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/todimpl.h>

#define MUTEX_TQ_OPERATIONS &_Thread_queue_Operations_priority_inherit

RTEMS_STATIC_ASSERT(
  offsetof( Mutex_Control, Queue )
    == offsetof( struct _Mutex_Control, _Queue ),
  MUTEX_CONTROL_QUEUE
);

RTEMS_STATIC_ASSERT(
  sizeof( Mutex_Control ) == sizeof( struct _Mutex_Control ),
  MUTEX_CONTROL_SIZE
);

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

static Thread_Control *_Mutex_Queue_acquire_critical(
  Mutex_Control        *mutex,
  Thread_queue_Context *queue_context
)
{
  Thread_Control *executing;

  executing = _Thread_Executing;
  _Thread_queue_Queue_acquire_critical(
    &mutex->Queue.Queue,
    &executing->Potpourri_stats,
    &queue_context->Lock_context.Lock_context
  );

  return executing;
}

static void _Mutex_Queue_release(
  Mutex_Control        *mutex,
  ISR_Level             level,
  Thread_queue_Context *queue_context
)
{
  _Thread_queue_Queue_release_critical(
    &mutex->Queue.Queue,
    &queue_context->Lock_context.Lock_context
  );
  _ISR_Local_enable( level );
}

static void _Mutex_Acquire_slow(
  Mutex_Control        *mutex,
  Thread_Control       *owner,
  Thread_Control       *executing,
  ISR_Level             level,
  Thread_queue_Context *queue_context
)
{
  _Thread_queue_Context_set_thread_state(
    queue_context,
    STATES_WAITING_FOR_MUTEX
  );
  _Thread_queue_Context_set_deadlock_callout(
    queue_context,
    _Thread_queue_Deadlock_fatal
  );
  _Thread_queue_Context_set_ISR_level( queue_context, level );
  _Thread_queue_Enqueue(
    &mutex->Queue.Queue,
    MUTEX_TQ_OPERATIONS,
    executing,
    queue_context
  );
}

static void _Mutex_Release_critical(
  Mutex_Control        *mutex,
  Thread_Control       *executing,
  ISR_Level             level,
  Thread_queue_Context *queue_context
)
{
  Thread_queue_Heads *heads;

  heads = mutex->Queue.Queue.heads;
  mutex->Queue.Queue.owner = NULL;
  _Thread_Resource_count_decrement( executing );

  if ( RTEMS_PREDICT_TRUE( heads == NULL ) ) {
    _Mutex_Queue_release( mutex, level, queue_context );
  } else {
    _Thread_queue_Context_set_ISR_level( queue_context, level );
    _Thread_queue_Surrender(
      &mutex->Queue.Queue,
      heads,
      executing,
      queue_context,
      MUTEX_TQ_OPERATIONS
    );
  }
}

void _Mutex_Acquire( struct _Mutex_Control *_mutex )
{
  Mutex_Control        *mutex;
  Thread_queue_Context  queue_context;
  ISR_Level             level;
  Thread_Control       *executing;
  Thread_Control       *owner;

  mutex = _Mutex_Get( _mutex );
  _Thread_queue_Context_initialize( &queue_context );
  _Thread_queue_Context_ISR_disable( &queue_context, level );
  executing = _Mutex_Queue_acquire_critical( mutex, &queue_context );

  owner = mutex->Queue.Queue.owner;

  if ( RTEMS_PREDICT_TRUE( owner == NULL ) ) {
    mutex->Queue.Queue.owner = executing;
    _Thread_Resource_count_increment( executing );
    _Mutex_Queue_release( mutex, level, &queue_context );
  } else {
    _Thread_queue_Context_set_enqueue_do_nothing_extra( &queue_context );
    _Mutex_Acquire_slow( mutex, owner, executing, level, &queue_context );
  }
}

int _Mutex_Acquire_timed(
  struct _Mutex_Control *_mutex,
  const struct timespec *abstime
)
{
  Mutex_Control        *mutex;
  Thread_queue_Context  queue_context;
  ISR_Level             level;
  Thread_Control       *executing;
  Thread_Control       *owner;

  mutex = _Mutex_Get( _mutex );
  _Thread_queue_Context_initialize( &queue_context );
  _Thread_queue_Context_ISR_disable( &queue_context, level );
  executing = _Mutex_Queue_acquire_critical( mutex, &queue_context );

  owner = mutex->Queue.Queue.owner;

  if ( RTEMS_PREDICT_TRUE( owner == NULL ) ) {
    mutex->Queue.Queue.owner = executing;
    _Thread_Resource_count_increment( executing );
    _Mutex_Queue_release( mutex, level, &queue_context );

    return 0;
  } else {
    _Thread_queue_Context_set_enqueue_timeout_realtime_timespec(
      &queue_context,
      abstime
    );
    _Mutex_Acquire_slow( mutex, owner, executing, level, &queue_context );

    return STATUS_GET_POSIX( _Thread_Wait_get_status( executing ) );
  }
}

int _Mutex_Try_acquire( struct _Mutex_Control *_mutex )
{
  Mutex_Control        *mutex;
  Thread_queue_Context  queue_context;
  ISR_Level             level;
  Thread_Control       *executing;
  Thread_Control       *owner;
  int                   eno;

  mutex = _Mutex_Get( _mutex );
  _Thread_queue_Context_initialize( &queue_context );
  _Thread_queue_Context_ISR_disable( &queue_context, level );
  executing = _Mutex_Queue_acquire_critical( mutex, &queue_context );

  owner = mutex->Queue.Queue.owner;

  if ( RTEMS_PREDICT_TRUE( owner == NULL ) ) {
    mutex->Queue.Queue.owner = executing;
    _Thread_Resource_count_increment( executing );
    eno = 0;
  } else {
    eno = EBUSY;
  }

  _Mutex_Queue_release( mutex, level, &queue_context );

  return eno;
}

void _Mutex_Release( struct _Mutex_Control *_mutex )
{
  Mutex_Control        *mutex;
  Thread_queue_Context  queue_context;
  ISR_Level             level;
  Thread_Control       *executing;

  mutex = _Mutex_Get( _mutex );
  _Thread_queue_Context_initialize( &queue_context );
  _Thread_queue_Context_ISR_disable( &queue_context, level );
  executing = _Mutex_Queue_acquire_critical( mutex, &queue_context );

  _Assert( mutex->Queue.Queue.owner == executing );

  _Mutex_Release_critical( mutex, executing, level, &queue_context );
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
  ISR_Level             level;
  Thread_Control          *executing;
  Thread_Control          *owner;

  mutex = _Mutex_recursive_Get( _mutex );
  _Thread_queue_Context_initialize( &queue_context );
  _Thread_queue_Context_ISR_disable( &queue_context, level );
  executing = _Mutex_Queue_acquire_critical( &mutex->Mutex, &queue_context );

  owner = mutex->Mutex.Queue.Queue.owner;

  if ( RTEMS_PREDICT_TRUE( owner == NULL ) ) {
    mutex->Mutex.Queue.Queue.owner = executing;
    _Thread_Resource_count_increment( executing );
    _Mutex_Queue_release( &mutex->Mutex, level, &queue_context );
  } else if ( owner == executing ) {
    ++mutex->nest_level;
    _Mutex_Queue_release( &mutex->Mutex, level, &queue_context );
  } else {
    _Thread_queue_Context_set_enqueue_do_nothing_extra( &queue_context );
    _Mutex_Acquire_slow( &mutex->Mutex, owner, executing, level, &queue_context );
  }
}

int _Mutex_recursive_Acquire_timed(
  struct _Mutex_recursive_Control *_mutex,
  const struct timespec           *abstime
)
{
  Mutex_recursive_Control *mutex;
  Thread_queue_Context     queue_context;
  ISR_Level                level;
  Thread_Control          *executing;
  Thread_Control          *owner;

  mutex = _Mutex_recursive_Get( _mutex );
  _Thread_queue_Context_initialize( &queue_context );
  _Thread_queue_Context_ISR_disable( &queue_context, level );
  executing = _Mutex_Queue_acquire_critical( &mutex->Mutex, &queue_context );

  owner = mutex->Mutex.Queue.Queue.owner;

  if ( RTEMS_PREDICT_TRUE( owner == NULL ) ) {
    mutex->Mutex.Queue.Queue.owner = executing;
    _Thread_Resource_count_increment( executing );
    _Mutex_Queue_release( &mutex->Mutex, level, &queue_context );

    return 0;
  } else if ( owner == executing ) {
    ++mutex->nest_level;
    _Mutex_Queue_release( &mutex->Mutex, level, &queue_context );

    return 0;
  } else {
    _Thread_queue_Context_set_enqueue_timeout_realtime_timespec(
      &queue_context,
      abstime
    );
    _Mutex_Acquire_slow( &mutex->Mutex, owner, executing, level, &queue_context );

    return STATUS_GET_POSIX( _Thread_Wait_get_status( executing ) );
  }
}

int _Mutex_recursive_Try_acquire( struct _Mutex_recursive_Control *_mutex )
{
  Mutex_recursive_Control *mutex;
  Thread_queue_Context     queue_context;
  ISR_Level                level;
  Thread_Control          *executing;
  Thread_Control          *owner;
  int                      eno;

  mutex = _Mutex_recursive_Get( _mutex );
  _Thread_queue_Context_initialize( &queue_context );
  _Thread_queue_Context_ISR_disable( &queue_context, level );
  executing = _Mutex_Queue_acquire_critical( &mutex->Mutex, &queue_context );

  owner = mutex->Mutex.Queue.Queue.owner;

  if ( RTEMS_PREDICT_TRUE( owner == NULL ) ) {
    mutex->Mutex.Queue.Queue.owner = executing;
    _Thread_Resource_count_increment( executing );
    eno = 0;
  } else if ( owner == executing ) {
    ++mutex->nest_level;
    eno = 0;
  } else {
    eno = EBUSY;
  }

  _Mutex_Queue_release( &mutex->Mutex, level, &queue_context );

  return eno;
}

void _Mutex_recursive_Release( struct _Mutex_recursive_Control *_mutex )
{
  Mutex_recursive_Control *mutex;
  Thread_queue_Context     queue_context;
  ISR_Level                level;
  Thread_Control          *executing;
  unsigned int             nest_level;

  mutex = _Mutex_recursive_Get( _mutex );
  _Thread_queue_Context_initialize( &queue_context );
  _Thread_queue_Context_ISR_disable( &queue_context, level );
  executing = _Mutex_Queue_acquire_critical( &mutex->Mutex, &queue_context );

  _Assert( mutex->Mutex.Queue.Queue.owner == executing );

  nest_level = mutex->nest_level;

  if ( RTEMS_PREDICT_TRUE( nest_level == 0 ) ) {
    _Mutex_Release_critical( &mutex->Mutex, executing, level, &queue_context );
  } else {
    mutex->nest_level = nest_level - 1;

    _Mutex_Queue_release( &mutex->Mutex, level, &queue_context );
  }
}
