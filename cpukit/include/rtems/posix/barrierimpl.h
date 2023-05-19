/* SPDX-License-Identifier: BSD-2-Clause */

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
 *  Copyright (c) 2017 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
