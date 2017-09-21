/**
 * @file
 * 
 * @brief Inlined Routines from the POSIX Barrier Manager
 *
 * This file contains the static inlin implementation of the inlined
 * routines from the POSIX Barrier Manager.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (c) 2017 embedded brains GmbH
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_POSIX_BARRIERIMPL_H
#define _RTEMS_POSIX_BARRIERIMPL_H

#include <errno.h>
#include <pthread.h>

#include <rtems/score/percpu.h>
#include <rtems/score/threadqimpl.h>

#ifdef __cplusplus
extern "C" {
#endif

#define POSIX_BARRIER_MAGIC 0x1cf03773UL

#define POSIX_BARRIER_TQ_OPERATIONS &_Thread_queue_Operations_FIFO

typedef struct {
  unsigned long flags;
  Thread_queue_Syslock_queue Queue;
  unsigned int count;
  unsigned int waiting_threads;
} POSIX_Barrier_Control;

static inline POSIX_Barrier_Control *_POSIX_Barrier_Get(
  pthread_barrier_t *_barrier
)
{
  return (POSIX_Barrier_Control *) _barrier;
}

static inline Thread_Control *_POSIX_Barrier_Queue_acquire(
  POSIX_Barrier_Control *barrier,
  Thread_queue_Context  *queue_context
)
{
  ISR_Level       level;
  Thread_Control *executing;

  _Thread_queue_Context_initialize( queue_context );
  _Thread_queue_Context_ISR_disable( queue_context, level );
  _Thread_queue_Context_set_ISR_level( queue_context, level );
  executing = _Thread_Executing;
  _Thread_queue_Queue_acquire_critical(
    &barrier->Queue.Queue,
    &executing->Potpourri_stats,
    &queue_context->Lock_context.Lock_context
  );

  return executing;
}

static inline void _POSIX_Barrier_Queue_release(
  POSIX_Barrier_Control *barrier,
  Thread_queue_Context  *queue_context
)
{
  _Thread_queue_Queue_release(
    &barrier->Queue.Queue,
    &queue_context->Lock_context.Lock_context
  );
}

#define POSIX_BARRIER_VALIDATE_OBJECT( bar ) \
  do { \
    if ( \
      ( bar ) == NULL \
        || ( (uintptr_t) ( bar ) ^ POSIX_BARRIER_MAGIC ) != ( bar )->_flags \
    ) { \
      return EINVAL; \
    } \
  } while ( 0 )

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
