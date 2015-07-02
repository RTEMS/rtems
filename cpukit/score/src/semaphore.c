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

#include <rtems/score/assert.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/threadqimpl.h>

#define SEMAPHORE_TQ_OPERATIONS &_Thread_queue_Operations_priority

typedef struct {
  Thread_queue_Syslock_queue Queue;
  unsigned int count;
} Semaphore_Control;

RTEMS_STATIC_ASSERT(
  offsetof( Semaphore_Control, Queue )
    == offsetof( struct _Semaphore_Control, _Queue ),
  SEMAPHORE_CONTROL_QUEUE
);

RTEMS_STATIC_ASSERT(
  offsetof( Semaphore_Control, count )
    == offsetof( struct _Semaphore_Control, _count ),
  SEMAPHORE_CONTROL_COUNT
);

RTEMS_STATIC_ASSERT(
  sizeof( Semaphore_Control ) == sizeof( struct _Semaphore_Control ),
  SEMAPHORE_CONTROL_SIZE
);

static Semaphore_Control *_Semaphore_Get(
  struct _Semaphore_Control *_sem
)
{
  return (Semaphore_Control *) _sem;
}

static Thread_Control *_Semaphore_Queue_acquire(
  Semaphore_Control *sem,
  ISR_lock_Context  *lock_context
)
{
  Thread_Control *executing;

  _ISR_lock_ISR_disable( lock_context );
  executing = _Thread_Executing;
  _Thread_queue_Queue_acquire_critical(
    &sem->Queue.Queue,
    &executing->Potpourri_stats,
    lock_context
  );

  return executing;
}

static void _Semaphore_Queue_release(
  Semaphore_Control *sem,
  ISR_lock_Context  *lock_context
)
{
  _Thread_queue_Queue_release( &sem->Queue.Queue, lock_context );
}

void _Semaphore_Wait( struct _Semaphore_Control *_sem )
{
  Semaphore_Control *sem ;
  ISR_lock_Context   lock_context;
  Thread_Control    *executing;
  unsigned int       count;

  sem = _Semaphore_Get( _sem );
  executing = _Semaphore_Queue_acquire( sem, &lock_context );

  count = sem->count;
  if ( count > 0 ) {
    sem->count = count - 1;
    _Semaphore_Queue_release( sem, &lock_context );
  } else {
    _Thread_queue_Enqueue_critical(
      &sem->Queue.Queue,
      SEMAPHORE_TQ_OPERATIONS,
      executing,
      STATES_WAITING_FOR_SYS_LOCK_SEMAPHORE,
      0,
      0,
      &lock_context
    );
  }
}

void _Semaphore_Post( struct _Semaphore_Control *_sem )
{
  Semaphore_Control  *sem;
  ISR_lock_Context    lock_context;
  Thread_queue_Heads *heads;

  sem = _Semaphore_Get( _sem );
  _Semaphore_Queue_acquire( sem, &lock_context );

  heads = sem->Queue.Queue.heads;
  if ( heads == NULL ) {
    _Assert( sem->count < UINT_MAX );
    ++sem->count;
    _Semaphore_Queue_release( sem, &lock_context );
  } else {
    const Thread_queue_Operations *operations;
    Thread_Control *first;

    operations = SEMAPHORE_TQ_OPERATIONS;
    first = ( *operations->first )( heads );

    _Thread_queue_Extract_critical(
      &sem->Queue.Queue,
      operations,
      first,
      &lock_context
    );
  }
}

#endif /* HAVE_STRUCT__THREAD_QUEUE_QUEUE */
