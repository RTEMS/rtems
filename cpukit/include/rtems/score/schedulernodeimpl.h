/**
 * @file
 *
 * @ingroup RTEMSScoreScheduler
 *
 * @brief Scheduler Node Implementation.
 */

/*
 * Copyright (c) 2014, 2017 embedded brains GmbH.  All rights reserved.
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
 * @brief Priority append indicator for the priority control used for the
 * scheduler node priority.
 */
#define SCHEDULER_PRIORITY_APPEND_FLAG 1

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
  ( ( priority ) & ~( (Priority_Control) SCHEDULER_PRIORITY_APPEND_FLAG ) )

/**
 * @brief Returns the priority control with the append indicator bit set.
 */
#define SCHEDULER_PRIORITY_APPEND( priority )  \
  ( ( priority ) | SCHEDULER_PRIORITY_APPEND_FLAG )

/**
 * @brief Returns true, if the item should be appended to its priority group,
 * otherwise returns false and the item should be prepended to its priority
 * group.
 */
#define SCHEDULER_PRIORITY_IS_APPEND( priority ) \
  ( ( ( priority ) & SCHEDULER_PRIORITY_APPEND_FLAG ) != 0 )

/**
 * @brief Initializes a node.
 *
 * @param scheduler The scheduler for the initialization of @a node.
 * @param[out] node The node to initialize.
 * @param the_thread The thread for the initialization of @a node.
 * @param priority The priority value for @a node.
 */
RTEMS_INLINE_ROUTINE void _Scheduler_Node_do_initialize(
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
  _SMP_sequence_lock_Initialize( &node->Priority.Lock );
#else
  (void) scheduler;
  (void) the_thread;
#endif
}

/**
 * @brief Gets the scheduler of the node.
 *
 * @param node The node to get the scheduler of.
 *
 * @return The scheduler of the node.
 */
RTEMS_INLINE_ROUTINE const Scheduler_Control *_Scheduler_Node_get_scheduler(
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
RTEMS_INLINE_ROUTINE Thread_Control *_Scheduler_Node_get_owner(
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
RTEMS_INLINE_ROUTINE Priority_Control _Scheduler_Node_get_priority(
  Scheduler_Node *node
)
{
  Priority_Control priority;

#if defined(RTEMS_SMP)
  unsigned int     seq;

  do {
    seq = _SMP_sequence_lock_Read_begin( &node->Priority.Lock );
#endif

    priority = node->Priority.value;

#if defined(RTEMS_SMP)
  } while ( _SMP_sequence_lock_Read_retry( &node->Priority.Lock, seq ) );
#endif

  return priority;
}

/**
 * @brief Sets the priority of the node.
 *
 * @param[in, out] node The node to set the priority of.
 * @param new_priority The new priority for @a node.
 * @param prepend_it Indicates whether the new priority should be prepended.
 */
RTEMS_INLINE_ROUTINE void _Scheduler_Node_set_priority(
  Scheduler_Node   *node,
  Priority_Control  new_priority,
  bool              prepend_it
)
{
#if defined(RTEMS_SMP)
  unsigned int seq;

  seq = _SMP_sequence_lock_Write_begin( &node->Priority.Lock );
#endif

  new_priority |= ( prepend_it ? 0 : SCHEDULER_PRIORITY_APPEND_FLAG );
  node->Priority.value = new_priority;

#if defined(RTEMS_SMP)
  _SMP_sequence_lock_Write_end( &node->Priority.Lock, seq );
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
RTEMS_INLINE_ROUTINE Thread_Control *_Scheduler_Node_get_user(
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
RTEMS_INLINE_ROUTINE void _Scheduler_Node_set_user(
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
RTEMS_INLINE_ROUTINE Thread_Control *_Scheduler_Node_get_idle(
  const Scheduler_Node *node
)
{
  return node->idle;
}
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

/** @} */

#endif /* _RTEMS_SCORE_SCHEDULERNODEIMPL_H */
