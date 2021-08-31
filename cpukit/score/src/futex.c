/**
 * @file
 *
 * @ingroup RTEMSScoreFutex
 *
 * @brief This source file contains the implementation of
 *   _Futex_Wait() and _Futex_Wake().
 */

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

/**
 * @defgroup RTEMSScoreFutex Futex Handler
 *
 * @ingroup RTEMSScore
 *
 * @brief This group contains the Futex Handler implementation.
 *
 * The behaviour of the futex operations is defined by Linux, see also:
 *
 * https://man7.org/linux/man-pages/man2/futex.2.html
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

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

static Thread_Control *_Futex_Queue_acquire_critical(
  Futex_Control        *futex,
  Thread_queue_Context *queue_context
)
{
  Thread_Control *executing;

  executing = _Thread_Executing;
  _Thread_queue_Queue_acquire_critical(
    &futex->Queue.Queue,
    &executing->Potpourri_stats,
    &queue_context->Lock_context.Lock_context
  );

  return executing;
}

static void _Futex_Queue_release(
  Futex_Control        *futex,
  ISR_Level             level,
  Thread_queue_Context *queue_context
)
{
  _Thread_queue_Queue_release_critical(
    &futex->Queue.Queue,
    &queue_context->Lock_context.Lock_context
  );
  _ISR_Local_enable( level );
}

/**
 * @brief Performs the ``FUTEX_WAIT`` operation.
 *
 * @param[in, out] _futex is the futex object.
 *
 * @param[in] uaddr is the address to the futex state.
 *
 * @param val is the expected futex state value.
 *
 * @retval 0 Returns zero if the futex state is equal to the expected value.
 *   In this case the calling thread is enqueued on the thread queue of the
 *   futex object.
 *
 * @retval EAGAIN Returns EAGAIN if the futex state is not equal to the
 *   expected value.
 */
int _Futex_Wait( struct _Futex_Control *_futex, int *uaddr, int val )
{
  Futex_Control        *futex;
  ISR_Level             level;
  Thread_queue_Context  queue_context;
  Thread_Control       *executing;
  int                   eno;

  futex = _Futex_Get( _futex );
  _Thread_queue_Context_initialize( &queue_context );
  _Thread_queue_Context_ISR_disable( &queue_context, level );
  executing = _Futex_Queue_acquire_critical( futex, &queue_context );

  if ( *uaddr == val ) {
    _Thread_queue_Context_set_thread_state(
      &queue_context,
      STATES_WAITING_FOR_FUTEX
    );
    _Thread_queue_Context_set_enqueue_do_nothing_extra( &queue_context );
    _Thread_queue_Context_set_ISR_level( &queue_context, level );
    _Thread_queue_Enqueue(
      &futex->Queue.Queue,
      FUTEX_TQ_OPERATIONS,
      executing,
      &queue_context
    );
    eno = 0;
  } else {
    _Futex_Queue_release( futex, level, &queue_context );
    eno = EAGAIN;
  }

  return eno;
}

typedef struct {
  Thread_queue_Context Base;
  int                  count;
} Futex_Context;

static Thread_Control *_Futex_Flush_filter(
  Thread_Control       *the_thread,
  Thread_queue_Queue   *queue,
  Thread_queue_Context *queue_context
)
{
  Futex_Context *context;

  context = (Futex_Context *) queue_context;

  if ( context->count <= 0 ) {
    return NULL;
  }

  --context->count;

  return the_thread;
}

/**
 * @brief Performs the ``FUTEX_WAKE`` operation.
 *
 * @param[in, out] _futex is the futex object.
 *
 * @param count is the maximum count of threads to wake up.
 *
 * @return Returns the count of woken up threads.
 */
int _Futex_Wake( struct _Futex_Control *_futex, int count )
{
  Futex_Control *futex;
  ISR_Level      level;
  Futex_Context  context;

  futex = _Futex_Get( _futex );
  _Thread_queue_Context_initialize( &context.Base );
  _Thread_queue_Context_ISR_disable( &context.Base, level );
  _Futex_Queue_acquire_critical( futex, &context.Base );

  /*
   * For some synchronization objects like barriers the _Futex_Wake() must be
   * called in the fast path.  Normally there are no threads on the queue, so
   * check this condition early.
   */
  if ( RTEMS_PREDICT_TRUE( _Thread_queue_Is_empty( &futex->Queue.Queue ) ) ) {
    _Futex_Queue_release( futex, level, &context.Base );
    return 0;
  }

  context.count = count;
  _Thread_queue_Context_set_ISR_level( &context.Base, level );
  return (int) _Thread_queue_Flush_critical(
    &futex->Queue.Queue,
    FUTEX_TQ_OPERATIONS,
    _Futex_Flush_filter,
    &context.Base
  );
}
