/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreThreadQueue
 *
 * @brief This header file provides interfaces related to thread queue
 *   operations.
 */

/*
 * Copyright (C) 2021 embedded brains GmbH (http://www.embedded-brains.de)
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

#ifndef _RTEMS_SCORE_THREADQOPS_H
#define _RTEMS_SCORE_THREADQOPS_H

#include <rtems/score/threadq.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup RTEMSScoreThreadQueue
 *
 * @{
 */

/**
 * @brief Initializes the priority actions so that no actions are performed.
 *
 * @param queue is unused.
 *
 * @param[out] priority_actions is initialized so that no actions are
 *   performed.
 */
void _Thread_queue_Do_nothing_priority_actions(
  Thread_queue_Queue *queue,
  Priority_Actions   *priority_actions
);

/**
 * @brief Enqueues the thread to the FIFO thread queue.
 *
 * @param[in, out] queue is the thread queue.
 *
 * @param[in, out] the_thread is the thread to enqueue.
 *
 * @param[in, out] queue_context is the thread queue context.
 */
void _Thread_queue_FIFO_enqueue(
  Thread_queue_Queue   *queue,
  Thread_Control       *the_thread,
  Thread_queue_Context *queue_context
);

/**
 * @brief Extracts the thread from the FIFO thread queue.
 *
 * @param[in, out] queue is the thread queue.
 *
 * @param[in, out] the_thread is the thread to extract.
 *
 * @param[in, out] queue_context is the thread queue context.
 */
void _Thread_queue_FIFO_extract(
  Thread_queue_Queue   *queue,
  Thread_Control       *the_thread,
  Thread_queue_Context *queue_context
);

/**
 * @brief Surrenders the thread queue to the first thread on the FIFO thread
 *   queue.
 *
 * @param[in, out] queue is the thread queue.
 *
 * @param[in, out] heads are heads of the thread queue.
 *
 * @param previous_owner is unused.
 *
 * @param[in, out] queue_context is the thread queue context.
 *
 * @return Returns the first thread on the thread queue according to the queue
 *   order.
 */
Thread_Control *_Thread_queue_FIFO_surrender(
  Thread_queue_Queue   *queue,
  Thread_queue_Heads   *heads,
  Thread_Control       *previous_owner,
  Thread_queue_Context *queue_context
);

/**
 * @brief Gets the first thread on the FIFO thread queue.
 *
 * @param[in, out] queue is the thread queue.
 *
 * @param[in, out] the_thread is the thread to extract.
 *
 * @param[in, out] queue_context is the thread queue context.
 */
Thread_Control *_Thread_queue_FIFO_first( const Thread_queue_Heads *heads );

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_SCORE_THREADQOPS_H */
