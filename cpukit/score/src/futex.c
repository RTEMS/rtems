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

/*
 * Use a noinline function to force the compiler to set up and tear down the
 * large stack frame only in the slow case.
 */
static __attribute__((noinline)) int _Futex_Wake_slow(
  Futex_Control      *futex,
  int                 count,
  Thread_queue_Heads *heads,
  ISR_lock_Context   *lock_context
)
{
  Chain_Control  unblock;
  Chain_Node    *node;
  Chain_Node    *tail;
  int            woken;

  woken = 0;
  _Chain_Initialize_empty( &unblock );

  while ( count > 0 && heads != NULL ) {
    const Thread_queue_Operations *operations;
    Thread_Control                *first;
    bool                           do_unblock;

    operations = FUTEX_TQ_OPERATIONS;
    first = ( *operations->first )( heads );

    do_unblock = _Thread_queue_Extract_locked(
      &futex->Queue.Queue,
      operations,
      first
    );
    if (do_unblock) {
      _Chain_Append_unprotected( &unblock, &first->Wait.Node.Chain );
    }

    ++woken;
    --count;
    heads = futex->Queue.Queue.heads;
  }

  node = _Chain_First( &unblock );
  tail = _Chain_Tail( &unblock );

  if ( node != tail ) {
    Per_CPU_Control *cpu_self;

    cpu_self = _Thread_Dispatch_disable_critical( lock_context );
    _Futex_Queue_release( futex, lock_context );

    do {
      Thread_Control *thread;
      Chain_Node     *next;

      next = _Chain_Next( node );
      thread = THREAD_CHAIN_NODE_TO_THREAD( node );
      _Thread_Unblock( thread );

      node = next;
    } while ( node != tail );

    _Thread_Dispatch_enable( cpu_self );
  } else {
    _Futex_Queue_release( futex, lock_context );
  }

  return woken;
}

int _Futex_Wake( struct _Futex_Control *_futex, int count )
{
  Futex_Control      *futex;
  ISR_lock_Context    lock_context;
  Thread_queue_Heads *heads;

  futex = _Futex_Get( _futex );
  _Futex_Queue_acquire( futex, &lock_context );

  /*
   * For some synchronization objects like barriers the _Futex_Wake() must be
   * called in the fast path.  Normally there are no threads on the queue, so
   * check this condition early.
   */
  heads = futex->Queue.Queue.heads;
  if ( __predict_true( heads == NULL ) ) {
    _Futex_Queue_release( futex, &lock_context );

    return 0;
  }

  return _Futex_Wake_slow( futex, count, heads, &lock_context );
}

#endif /* HAVE_STRUCT__THREAD_QUEUE_QUEUE */
