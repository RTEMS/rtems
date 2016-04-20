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

#define FUTEX_TQ_OPERATIONS &_Thread_queue_Operations_FIFO

typedef struct {
  Thread_queue_Syslock_queue Queue;
} Futex_Control;

RTEMS_STATIC_ASSERT(
  offsetof( Futex_Control, Queue )
    == offsetof( struct _Futex_Control, _Queue ),
  FUTEX_CONTROL_QUEUE
);

RTEMS_STATIC_ASSERT(
  sizeof( Futex_Control ) == sizeof( struct _Futex_Control ),
  FUTEX_CONTROL_SIZE
);

static Futex_Control *_Futex_Get( struct _Futex_Control *_futex )
{
  return (Futex_Control *) _futex;
}

static Thread_Control *_Futex_Queue_acquire(
  Futex_Control    *futex,
  ISR_lock_Context *lock_context
)
{
  Thread_Control *executing;

  _ISR_lock_ISR_disable( lock_context );
  executing = _Thread_Executing;
  _Thread_queue_Queue_acquire_critical(
    &futex->Queue.Queue,
    &executing->Potpourri_stats,
    lock_context
  );

  return executing;
}

static void _Futex_Queue_release(
  Futex_Control    *futex,
  ISR_lock_Context *lock_context
)
{
  _Thread_queue_Queue_release( &futex->Queue.Queue, lock_context );
}

int _Futex_Wait( struct _Futex_Control *_futex, int *uaddr, int val )
{
  Futex_Control    *futex;
  ISR_lock_Context  lock_context;
  Thread_Control   *executing;
  int               eno;

  futex = _Futex_Get( _futex );
  executing = _Futex_Queue_acquire( futex, &lock_context );

  if ( *uaddr == val ) {
    _Thread_queue_Enqueue_critical(
      &futex->Queue.Queue,
      FUTEX_TQ_OPERATIONS,
      executing,
      STATES_WAITING_FOR_SYS_LOCK_FUTEX,
      0,
      0,
      &lock_context
    );
    eno = 0;
  } else {
    _Futex_Queue_release( futex, &lock_context );
    eno = EWOULDBLOCK;
  }

  return eno;
}

typedef struct {
  ISR_lock_Context Base;
  int              count;
} Futex_Lock_context;

static Thread_Control *_Futex_Flush_filter(
  Thread_Control     *the_thread,
  Thread_queue_Queue *queue,
  ISR_lock_Context   *lock_context
)
{
  Futex_Lock_context *futex_lock_context;

  futex_lock_context = (Futex_Lock_context *) lock_context;

  if ( futex_lock_context->count <= 0 ) {
    return NULL;
  }

  --futex_lock_context->count;

  return the_thread;
}

int _Futex_Wake( struct _Futex_Control *_futex, int count )
{
  Futex_Control      *futex;
  Futex_Lock_context  lock_context;

  futex = _Futex_Get( _futex );
  _Futex_Queue_acquire( futex, &lock_context.Base );

  /*
   * For some synchronization objects like barriers the _Futex_Wake() must be
   * called in the fast path.  Normally there are no threads on the queue, so
   * check this condition early.
   */
  if ( __predict_true( _Thread_queue_Is_empty( &futex->Queue.Queue ) ) ) {
    _Futex_Queue_release( futex, &lock_context.Base );
    return 0;
  }

  lock_context.count = count;
  return (int) _Thread_queue_Flush_critical(
    &futex->Queue.Queue,
    FUTEX_TQ_OPERATIONS,
    _Futex_Flush_filter,
    NULL,
    0,
    &lock_context.Base
  );
}

#endif /* HAVE_STRUCT__THREAD_QUEUE_QUEUE */
