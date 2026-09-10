/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreSyslockCondition
 *
 * @brief This header file provides the interfaces of the
 *   @ref RTEMSScoreSyslockCondition.
 */

/*
 * Copyright (C) 2026 embedded brains GmbH & Co. KG
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

#ifndef _RTEMS_SCORE_CONDIMPL_H
#define _RTEMS_SCORE_CONDIMPL_H

#include <sys/lock.h>

#include <rtems/score/threadqimpl.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup RTEMSScoreSyslockCondition System Lock Condition Support
 *
 * @ingroup RTEMSScore
 *
 * @brief The System Lock Condition Support helps to implement directives
 *   which use data structures compatible with the data structures defined by
 *   the Newlib provided <sys/lock.h> header file.
 *
 * A component which owns the thread queue of a condition variable can use the
 * lock of that thread queue as its own lock.  The lock is an ISR lock, so a
 * handler in interrupt context can take it.
 *
 * @{
 */

/**
 * @brief The condition variable control block.
 */
typedef struct {
  Thread_queue_Syslock_queue Queue;
} Condition_Control;

/**
 * @brief The thread queue operations of a condition variable.
 */
#define CONDITION_TQ_OPERATIONS &_Thread_queue_Operations_FIFO

/**
 * @brief Gets the thread queue of the condition variable.
 *
 * @param condition is the condition variable.
 *
 * @return Returns the thread queue of the condition variable.
 */
static inline Thread_queue_Queue *_Condition_Get_queue(
  struct _Condition_Control *condition
)
{
  return &( (Condition_Control *) condition )->Queue.Queue;
}

/**
 * @brief Acquires the thread queue lock of the condition variable.
 *
 * The caller shall disable interrupts before the call.
 *
 * @param condition is the condition variable.
 *
 * @param[in, out] queue_context is the thread queue context.
 */
static inline void _Condition_Acquire_critical(
  struct _Condition_Control *condition,
  Thread_queue_Context      *queue_context
)
{
  _Thread_queue_Queue_acquire_critical(
    _Condition_Get_queue( condition ),
    &_Thread_Executing->Potpourri_stats,
    &queue_context->Lock_context.Lock_context
  );
}

/**
 * @brief Releases the thread queue lock of the condition variable.
 *
 * The caller shall enable interrupts after the call.
 *
 * @param condition is the condition variable.
 *
 * @param[in, out] queue_context is the thread queue context.
 */
static inline void _Condition_Release_critical(
  struct _Condition_Control *condition,
  Thread_queue_Context      *queue_context
)
{
  _Thread_queue_Queue_release_critical(
    _Condition_Get_queue( condition ),
    &queue_context->Lock_context.Lock_context
  );
}

/**
 * @brief Disables interrupts and acquires the thread queue lock of the
 *   condition variable.
 *
 * @param condition is the condition variable.
 *
 * @param[in, out] queue_context is the thread queue context.
 */
static inline void _Condition_Acquire(
  struct _Condition_Control *condition,
  Thread_queue_Context      *queue_context
)
{
  _ISR_lock_ISR_disable( &queue_context->Lock_context.Lock_context );
  _Condition_Acquire_critical( condition, queue_context );
}

/**
 * @brief Releases the thread queue lock of the condition variable and enables
 *   interrupts.
 *
 * @param condition is the condition variable.
 *
 * @param[in, out] queue_context is the thread queue context.
 */
static inline void _Condition_Release(
  struct _Condition_Control *condition,
  Thread_queue_Context      *queue_context
)
{
  _Thread_queue_Queue_release(
    _Condition_Get_queue( condition ),
    &queue_context->Lock_context.Lock_context
  );
}

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_CONDIMPL_H */
