/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreScheduler
 *
 * @brief This header file provides interfaces of the
 *   @ref RTEMSScoreScheduler related to scheduler nodes which are only used by
 *   the implementation.
 */

/*
 * Copyright (C) 2014, 2017 embedded brains GmbH & Co. KG
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

#ifndef _RTEMS_SCORE_SCHEDULERNODEIMPL_H
#define _RTEMS_SCORE_SCHEDULERNODEIMPL_H

#include <rtems/score/schedulernode.h>
#include <rtems/score/priorityimpl.h>

/**
 * @addtogroup RTEMSScoreScheduler
 *
 * @{
 */

struct _Scheduler_Control;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SCHEDULER_NODE_OF_WAIT_PRIORITY_NODE( node ) \
  RTEMS_CONTAINER_OF( node, Scheduler_Node, Wait.Priority.Node.Node.Chain )

#define SCHEDULER_NODE_OF_WAIT_PRIORITY( node ) \
  RTEMS_CONTAINER_OF( node, Scheduler_Node, Wait.Priority )

/**
 * @brief Maps a priority value to support the append indicator.
 */
#define SCHEDULER_PRIORITY_MAP( priority ) ( ( priority ) << 1 )

/**
 * @brief Returns the plain priority value.
 */
#define SCHEDULER_PRIORITY_UNMAP( priority ) ( ( priority ) >> 1 )

/**
 * @brief Clears the priority append indicator bit.
 */
#define SCHEDULER_PRIORITY_PURIFY( priority )  \
  ( ( priority ) & ~( (Priority_Control) PRIORITY_GROUP_LAST ) )

/**
 * @brief Returns the priority control with the append indicator bit set.
 */
#define SCHEDULER_PRIORITY_APPEND( priority )  \
  ( ( priority ) | ( (Priority_Control) PRIORITY_GROUP_LAST ) )

/**
 * @brief Returns true, if the item should be appended to its priority group,
 * otherwise returns false and the item should be prepended to its priority
 * group.
 */
#define SCHEDULER_PRIORITY_IS_APPEND( priority ) \
  ( ( ( priority ) & ( (Priority_Control) PRIORITY_GROUP_LAST ) ) != 0 )

/**
 * @brief Initializes the node.
 *
 * @param scheduler is the scheduler of the node.
 *
 * @param[out] node is the node to initialize.
 *
 * @param[in, out] the_thread is the thread of the node.
 *
 * @param priority is the initial priority of the node.
 */
static inline void _Scheduler_Node_do_initialize(
  const struct _Scheduler_Control *scheduler,
  Scheduler_Node                  *node,
  Thread_Control                  *the_thread,
  Priority_Control                 priority
)
{
  node->owner = the_thread;

  node->Priority.value = priority;

#if defined(RTEMS_SMP)
  _Chain_Initialize_node( &node->Thread.Wait_node );
  node->Wait.Priority.scheduler = scheduler;
  node->user = the_thread;
  node->idle = NULL;
#if CPU_SIZEOF_POINTER != 8
  _ISR_lock_Initialize( &node->Priority.Lock, "Scheduler Node Priority" );
#endif
#else
  (void) scheduler;
  (void) the_thread;
#endif
}

/**
 * @brief Destroys the node.
 *
 * @param scheduler is the scheduler of the node.
 *
 * @param[in, out] node is the node to destroy.
 */
static inline void _Scheduler_Node_do_destroy(
  const struct _Scheduler_Control *scheduler,
  Scheduler_Node                  *node
)
{
  (void) scheduler;

#if defined(RTEMS_SMP) && CPU_SIZEOF_POINTER != 8
  _ISR_lock_Destroy( &node->Priority.Lock );
#else
  (void) node;
#endif
}

/**
 * @brief Gets the scheduler of the node.
 *
 * @param node The node to get the scheduler of.
 *
 * @return The scheduler of the node.
 */
static inline const Scheduler_Control *_Scheduler_Node_get_scheduler(
  const Scheduler_Node *node
)
{
  return _Priority_Get_scheduler( &node->Wait.Priority );
}

/**
 * @brief Gets the owner of the node.
 *
 * @param node The node to get the owner of.
 *
 * @return The owner of the node.
 */
static inline Thread_Control *_Scheduler_Node_get_owner(
  const Scheduler_Node *node
)
{
  return node->owner;
}

/**
 * @brief Gets the priority of the node.
 *
 * @param node The node to get the priority of.
 *
 * @return The priority of the node.
 */
static inline Priority_Control _Scheduler_Node_get_priority(
  Scheduler_Node *node
)
{
  Priority_Control priority;

#if defined(RTEMS_SMP) && CPU_SIZEOF_POINTER == 8
  priority = _Atomic_Fetch_add_ulong(
    &node->Priority.value,
    0,
    ATOMIC_ORDER_RELAXED
  );
#else
  ISR_lock_Context lock_context;

  _ISR_lock_Acquire( &node->Priority.Lock, &lock_context );
  priority = node->Priority.value;
  _ISR_lock_Release( &node->Priority.Lock, &lock_context );
#endif

  return priority;
}

/**
 * @brief Sets the priority of the node.
 *
 * @param[in, out] node is the scheduler node.
 *
 * @param new_priority is the priority to set.
 *
 * @param group_order is the priority group order, see #PRIORITY_GROUP_FIRST
 *   and #PRIORITY_GROUP_LAST.
 */
static inline void _Scheduler_Node_set_priority(
  Scheduler_Node      *node,
  Priority_Control     new_priority,
  Priority_Group_order group_order
)
{
#if defined(RTEMS_SMP) && CPU_SIZEOF_POINTER == 8
  _Atomic_Store_ulong(
    &node->Priority.value,
    new_priority | (Priority_Control) group_order,
    ATOMIC_ORDER_RELAXED
  );
#else
  ISR_lock_Context lock_context;

  _ISR_lock_Acquire( &node->Priority.Lock, &lock_context );
  node->Priority.value = new_priority | ( (Priority_Control) group_order );
  _ISR_lock_Release( &node->Priority.Lock, &lock_context );
#endif
}

#if defined(RTEMS_SMP)
/**
 * @brief Gets the user of the node.
 *
 * @param node The node to get the user of.
 *
 * @return The user of the node.
 */
static inline Thread_Control *_Scheduler_Node_get_user(
  const Scheduler_Node *node
)
{
  return node->user;
}

/**
 * @brief Sets the user of the node.
 *
 * @param[out] node The node to set the user of.
 * @param user The new user for @a node.
 */
static inline void _Scheduler_Node_set_user(
  Scheduler_Node *node,
  Thread_Control *user
)
{
  node->user = user;
}

/**
 * @brief Gets the idle thread of the node.
 *
 * @param node The node to get the idle thread of.
 *
 * @return The idle thread of @a node.
 */
static inline Thread_Control *_Scheduler_Node_get_idle(
  const Scheduler_Node *node
)
{
  return node->idle;
}

/**
 * @brief Sets the scheduler node's user to the idle thread.
 *
 * @param[in, out] node is the node to receive an idle thread.
 *
 * @param idle is the idle thread to use.
 */
static inline void _Scheduler_Node_set_idle_user(
  Scheduler_Node *node,
  Thread_Control *idle
)
{
  _Assert( _Scheduler_Node_get_idle( node ) == NULL );
  _Assert(
    _Scheduler_Node_get_owner( node ) == _Scheduler_Node_get_user( node )
  );

  _Scheduler_Node_set_user( node, idle );
  node->idle = idle;
}
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

/** @} */

#endif /* _RTEMS_SCORE_SCHEDULERNODEIMPL_H */
