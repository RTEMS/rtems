/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreSchedulerEDF
 *
 * @brief This header file provides interfaces of the
 *   @ref RTEMSScoreSchedulerEDF which are only used by the implementation.
 */

/*
 *  Copryight (c) 2011 Petr Benes.
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

#ifndef _RTEMS_SCORE_SCHEDULEREDFIMPL_H
#define _RTEMS_SCORE_SCHEDULEREDFIMPL_H

#include <rtems/score/scheduleredf.h>
#include <rtems/score/scheduleruniimpl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup RTEMSScoreSchedulerEDF
 *
 * @{
 */

/**
 * This is just a most significant bit of Priority_Control type. It
 * distinguishes threads which are deadline driven (priority
 * represented by a lower number than @a SCHEDULER_EDF_PRIO_MSB) from those
 * ones who do not have any deadlines and thus are considered background
 * tasks.
 */
#define SCHEDULER_EDF_PRIO_MSB 0x8000000000000000

/**
 * @brief Gets the context of the scheduler.
 *
 * @param scheduler The scheduler instance.
 *
 * @return The scheduler context of @a scheduler.
 */
static inline Scheduler_EDF_Context *
  _Scheduler_EDF_Get_context( const Scheduler_Control *scheduler )
{
  return (Scheduler_EDF_Context *) _Scheduler_Get_context( scheduler );
}

/**
 * @brief Gets the scheduler EDF node of the thread.
 *
 * @param the_thread The thread to get the scheduler node of.
 *
 * @return The EDF scheduler node of @a the_thread.
 */
static inline Scheduler_EDF_Node *_Scheduler_EDF_Thread_get_node(
  Thread_Control *the_thread
)
{
  return (Scheduler_EDF_Node *) _Thread_Scheduler_get_home_node( the_thread );
}

/**
 * @brief Returns the scheduler EDF node for the scheduler node.
 *
 * @param node The scheduler node of which the scheduler EDF node is returned.
 *
 * @return The corresponding scheduler EDF node.
 */
static inline Scheduler_EDF_Node * _Scheduler_EDF_Node_downcast(
  Scheduler_Node *node
)
{
  return (Scheduler_EDF_Node *) node;
}

/**
 * @brief Checks if @a left is less than the priority of the node @a right.
 *
 * @param left The priority on the left hand side of the comparison.
 * @param right The node of which the priority is compared to left.
 *
 * @retval true @a left is less than the priority of @a right.
 * @retval false @a left is greater or equal than the priority of @a right.
 */
static inline bool _Scheduler_EDF_Less(
  const void        *left,
  const RBTree_Node *right
)
{
  const Priority_Control   *the_left;
  const Scheduler_EDF_Node *the_right;
  Priority_Control          prio_left;
  Priority_Control          prio_right;

  the_left = (const Priority_Control *) left;
  the_right = RTEMS_CONTAINER_OF( right, Scheduler_EDF_Node, Node );

  prio_left = *the_left;
  prio_right = the_right->priority;

  return prio_left < prio_right;
}

/**
 * @brief Checks if @a left is less or equal than the priority of the node @a right.
 *
 * @param left The priority on the left hand side of the comparison.
 * @param right The node of which the priority is compared to left.
 *
 * @retval true @a left is less or equal than the priority of @a right.
 * @retval false @a left is greater than the priority of @a right.
 */
static inline bool _Scheduler_EDF_Priority_less_equal(
  const void        *left,
  const RBTree_Node *right
)
{
  const Priority_Control   *the_left;
  const Scheduler_EDF_Node *the_right;
  Priority_Control          prio_left;
  Priority_Control          prio_right;

  the_left = (const Priority_Control *) left;
  the_right = RTEMS_CONTAINER_OF( right, Scheduler_EDF_Node, Node );

  prio_left = *the_left;
  prio_right = the_right->priority;

  return prio_left <= prio_right;
}

/**
 * @brief Inserts the scheduler node with the given priority in the ready
 *      queue of the context.
 *
 * @param[in, out] context The context to insert the node in.
 * @param node The node to be inserted.
 * @param insert_priority The priority with which the node will be inserted.
 */
static inline void _Scheduler_EDF_Enqueue(
  Scheduler_EDF_Context *context,
  Scheduler_EDF_Node    *node,
  Priority_Control       insert_priority
)
{
  _RBTree_Insert_inline(
    &context->Ready,
    &node->Node,
    &insert_priority,
    _Scheduler_EDF_Priority_less_equal
  );
}

/**
 * @brief Extracts the scheduler node from the ready queue of the context.
 *
 * @param[in, out] context The context to extract the node from.
 * @param[in, out] node The node to extract.
 */
static inline void _Scheduler_EDF_Extract(
  Scheduler_EDF_Context *context,
  Scheduler_EDF_Node    *node
)
{
  _RBTree_Extract( &context->Ready, &node->Node );
}

/**
 * @brief Extracts the node from the context of the given scheduler.
 *
 * @param scheduler The scheduler instance.
 * @param the_thread The thread is not used in this method.
 * @param[in, out] node The node to be extracted.
 */
static inline void _Scheduler_EDF_Extract_body(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
)
{
  Scheduler_EDF_Context *context;
  Scheduler_EDF_Node    *the_node;

  context = _Scheduler_EDF_Get_context( scheduler );
  the_node = _Scheduler_EDF_Node_downcast( node );

  _Scheduler_EDF_Extract( context, the_node );
}

/**
 * @brief Gets the highest priority ready thread of the scheduler.
 *
 * @param scheduler is the scheduler.
 */
static inline Thread_Control *_Scheduler_EDF_Get_highest_ready(
  const Scheduler_Control *scheduler
)
{
  Scheduler_EDF_Context *context;
  RBTree_Node           *first;
  Scheduler_EDF_Node    *node;

  context = _Scheduler_EDF_Get_context( scheduler );
  first = _RBTree_Minimum( &context->Ready );
  node = RTEMS_CONTAINER_OF( first, Scheduler_EDF_Node, Node );

  return node->Base.owner;
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
