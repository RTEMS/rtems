/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreSchedulerSimple
 *
 * @brief This header file provides interfaces of the
 *   @ref RTEMSScoreSchedulerSimple which are only used by the implementation.
 */

/*
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
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

#ifndef _RTEMS_SCORE_SCHEDULERSIMPLEIMPL_H
#define _RTEMS_SCORE_SCHEDULERSIMPLEIMPL_H

#include <rtems/score/schedulersimple.h>
#include <rtems/score/chainimpl.h>
#include <rtems/score/scheduleruniimpl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup RTEMSScoreSchedulerSimple
 *
 * @{
 */

/**
 * @brief Gets context of the scheduler.
 *
 * @param scheduler The scheduler instance to get the context of.
 *
 * @return The context of @a scheduler.
 */
static inline Scheduler_simple_Context *
  _Scheduler_simple_Get_context( const Scheduler_Control *scheduler )
{
  return (Scheduler_simple_Context *) _Scheduler_Get_context( scheduler );
}

/**
 * @brief Checks if the priority is less or equal than the priority of the node.
 *
 * @param key is the priority to compare.
 *
 * @param to_insert is the chain node to insert.
 *
 * @param next is the chain node to compare the priority of.
 *
 * @retval true @a to_insert is smaller or equal than the priority of @a next.
 * @retval false @a to_insert is greater than the priority of @a next.
 */
static inline bool _Scheduler_simple_Priority_less_equal(
  const void       *key,
  const Chain_Node *to_insert,
  const Chain_Node *next
)
{
  const unsigned int   *priority_to_insert;
  const Thread_Control *thread_next;

  (void) to_insert;
  priority_to_insert = (const unsigned int *) key;
  thread_next = (const Thread_Control *) next;

  return *priority_to_insert <= _Thread_Get_priority( thread_next );
}

/**
 * @brief Inserts the thread control with the given priority into the chain.
 *
 * @param[in, out] chain The chain to insert @a to_insert in.
 * @param[in, out] to_insert The node to insert into @a chain.
 * @param insert_priority The priority to insert @a to_insert with.
 */
static inline void _Scheduler_simple_Insert(
  Chain_Control  *chain,
  Thread_Control *to_insert,
  unsigned int    insert_priority
)
{
  _Chain_Insert_ordered_unprotected(
    chain,
    &to_insert->Object.Node,
    &insert_priority,
    _Scheduler_simple_Priority_less_equal
  );
}

/**
 * @brief Extracts the threads node.
 *
 * @param scheduler This parameter is unused.
 * @param[in, out] the_thread The thread of which to extract the node out of its chain.
 * @param node This parameter is unused.
 */
static inline void _Scheduler_simple_Extract(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
)
{
  (void) scheduler;
  (void) node;

  _Chain_Extract_unprotected( &the_thread->Object.Node );
}

/**
 * @brief Gets the highest priority ready thread of the scheduler.
 *
 * @param scheduler is the scheduler.
 */
static inline Thread_Control *_Scheduler_simple_Get_highest_ready(
  const Scheduler_Control *scheduler
)
{
  Scheduler_simple_Context *context =
    _Scheduler_simple_Get_context( scheduler );

  return (Thread_Control *) _Chain_First( &context->Ready );
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
