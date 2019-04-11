/**
 * @file
 *
 * @ingroup RTEMSScorePriority
 *
 * @brief Priority Handler API Implementation.
 */

/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
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

#ifndef _RTEMS_SCORE_PRIORITYIMPL_H
#define _RTEMS_SCORE_PRIORITYIMPL_H

#include <rtems/score/priority.h>
#include <rtems/score/scheduler.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @addtogroup RTEMSScorePriority
 *
 * @{
 */

/**
 * @brief Initializes the priority actions empty.
 *
 * @param[out] actions The actions to be initialized empty.
 */
RTEMS_INLINE_ROUTINE void _Priority_Actions_initialize_empty(
  Priority_Actions *actions
)
{
  actions->actions = NULL;
}

/**
 * @brief Initializes the priority actions with the given information.
 *
 * @param[out] actions The actions to be initialized.
 * @param aggregation The aggregation for the @a actions to be initialized.
 * @param node The action node for the @a actions to be initialized.
 * @param type The action type for the @a actions to be initialized.
 */
RTEMS_INLINE_ROUTINE void _Priority_Actions_initialize_one(
  Priority_Actions     *actions,
  Priority_Aggregation *aggregation,
  Priority_Node        *node,
  Priority_Action_type  type
)
{
#if defined(RTEMS_SMP)
  aggregation->Action.next = NULL;
#endif
  aggregation->Action.node = node;
  aggregation->Action.type = type;

  actions->actions = aggregation;
}

/**
 * @brief Checks if the priority actions is empty.
 *
 * @param actions The priority actions to check if it is empty.
 *
 * @retval true The priority actions @a actions is empty.
 * @retval false The priority actions @a actions is empty.
 */
RTEMS_INLINE_ROUTINE bool _Priority_Actions_is_empty(
  const Priority_Actions *actions
)
{
  return actions->actions == NULL;
}

/**
 * @brief Checks if the priority actions is valid.
 *
 * @param aggregation The aggregation of the priority action.
 *
 * @retval true The @a aggregation is valid.
 * @retval false The @a aggregation is not valid.
 */
RTEMS_INLINE_ROUTINE bool _Priority_Actions_is_valid(
  const Priority_Aggregation *aggregation
)
{
#if defined(RTEMS_SMP)
  return aggregation != NULL;
#else
  (void) aggregation;
  return false;
#endif
}

/**
 * @brief Moves the priority actions' actions.
 *
 * @param[in, out] actions The priority actions to move the actions away from.
 *
 * @return The former actions of @a actions that were moved.
 */
RTEMS_INLINE_ROUTINE Priority_Aggregation *_Priority_Actions_move(
  Priority_Actions *actions
)
{
  Priority_Aggregation *aggregation;

  aggregation = actions->actions;
  actions->actions = NULL;

  return aggregation;
}

/**
 * @brief Adds actions to the priority actions' actions.
 *
 * @param[in, out] actions The priority actions to add actions to.
 * @param[out] aggregation The actions to add to @a actions.
 */
RTEMS_INLINE_ROUTINE void _Priority_Actions_add(
  Priority_Actions     *actions,
  Priority_Aggregation *aggregation
)
{
#if defined(RTEMS_SMP)
  /*
   * Priority aggregations are only added to action lists, so do not care about
   * the current next pointer value.
   */
  aggregation->Action.next = actions->actions;
#endif
  actions->actions = aggregation;
}

/**
 * @brief Initializes the priority node to the given priority.
 *
 * @param[out] node The priority node to be initialized.
 * @param priority The priority to initialize @a node to.
 */
RTEMS_INLINE_ROUTINE void _Priority_Node_initialize(
  Priority_Node    *node,
  Priority_Control  priority
)
{
  node->priority = priority;
  _RBTree_Initialize_node( &node->Node.RBTree );
}

/**
 * @brief Sets the priority of the priority node to the given priority.
 *
 * @param[out] node The priority node to set the priority of.
 * @param priority The new priority for @a node.
 */
RTEMS_INLINE_ROUTINE void _Priority_Node_set_priority(
  Priority_Node    *node,
  Priority_Control  priority
)
{
  node->priority = priority;
}

/**
 * @brief Sets the priority node inactive.
 *
 * @param[in, out] node The priority node to set inactive.
 */
RTEMS_INLINE_ROUTINE void _Priority_Node_set_inactive(
  Priority_Node *node
)
{
  _RBTree_Set_off_tree( &node->Node.RBTree );
}

/**
 * @brief Checks if the priority node is active.
 *
 * @param node The priority node that shall be verified if it is active.
 *
 * @retval true The priority node is active.
 * @retval false The priority node is inactive.
 */
RTEMS_INLINE_ROUTINE bool _Priority_Node_is_active(
  const Priority_Node *node
)
{
  return !_RBTree_Is_node_off_tree( &node->Node.RBTree );
}

/**
 * @brief Initializes the priority aggregation empty.
 *
 * @param[out] aggregation The priority aggregaton to initialize empty.
 */
RTEMS_INLINE_ROUTINE void _Priority_Initialize_empty(
  Priority_Aggregation *aggregation
)
{
#if defined(RTEMS_DEBUG)
#if defined(RTEMS_SMP)
  aggregation->Action.next = NULL;
#endif
  aggregation->Action.node = NULL;
  aggregation->Action.type = PRIORITY_ACTION_INVALID;
#endif
  _RBTree_Initialize_node( &aggregation->Node.Node.RBTree );
  _RBTree_Initialize_empty( &aggregation->Contributors );
}

/**
 * @brief Initializes the priority aggregation with the given information.
 *
 * @param[out] aggregation The priority aggregaton to initialize.
 * @param node The priority node to initialize @a aggregation with.
 */
RTEMS_INLINE_ROUTINE void _Priority_Initialize_one(
  Priority_Aggregation *aggregation,
  Priority_Node        *node
)
{
#if defined(RTEMS_DEBUG)
#if defined(RTEMS_SMP)
  aggregation->Action.next = NULL;
#endif
  aggregation->Action.node = NULL;
  aggregation->Action.type = PRIORITY_ACTION_INVALID;
#endif
  _Priority_Node_initialize( &aggregation->Node, node->priority );
  _RBTree_Initialize_one( &aggregation->Contributors, &node->Node.RBTree );
}

/**
 * @brief Checks if the priority aggregation is empty.
 *
 * @param aggregation The priority aggregation that shall be verified if it is empty.
 *
 * @retval true The priority aggregation is empty.
 * @retval false The priority aggregation is not empty.
 */
RTEMS_INLINE_ROUTINE bool _Priority_Is_empty(
  const Priority_Aggregation *aggregation
)
{
  return _RBTree_Is_empty( &aggregation->Contributors );
}

/**
 * @brief Gets the priority aggregation's priority.
 *
 * @param aggregation The priority aggregation to get the priority from.
 *
 * @return The priority of @a aggregation.
 */
RTEMS_INLINE_ROUTINE Priority_Control _Priority_Get_priority(
  const Priority_Aggregation *aggregation
)
{
  return aggregation->Node.priority;
}

/**
 * @brief Gets the priority aggregation's scheduler.
 *
 * @param aggregation The priority aggregation to get the scheduler from.
 *
 * @return The scheduler of @a aggregation.
 */
RTEMS_INLINE_ROUTINE const Scheduler_Control *_Priority_Get_scheduler(
  const Priority_Aggregation *aggregation
)
{
#if defined(RTEMS_SMP)
  return aggregation->scheduler;
#else
  return &_Scheduler_Table[ 0 ];
#endif
}

/**
 * @brief Gets the minimum node of the priority aggregation
 *
 * @param aggregation The priority aggregation to get the minimum node from.
 *
 * @return The minimum node of @a aggregation
 */
RTEMS_INLINE_ROUTINE Priority_Node *_Priority_Get_minimum_node(
  const Priority_Aggregation *aggregation
)
{
  return (Priority_Node *) _RBTree_Minimum( &aggregation->Contributors );
}

/**
 * @brief Sets the action node of the priority aggregation.
 *
 * @param[out] aggregation The priority aggregation to set the action node of.
 * @param node The new priority node for @a aggregation.
 */
RTEMS_INLINE_ROUTINE void _Priority_Set_action_node(
  Priority_Aggregation *aggregation,
  Priority_Node        *node
)
{
  aggregation->Action.node = node;
}

/**
 * @brief Sets the action type of the priority aggregation.
 *
 * @param[out] aggregation The priority aggregation to set the action type of.
 * @param type The new action type for @a aggregation.
 */
RTEMS_INLINE_ROUTINE void _Priority_Set_action_type(
  Priority_Aggregation *aggregation,
  Priority_Action_type  type
)
{
  aggregation->Action.type = type;
}

/**
 * @brief Sets the action type and action node of the priority aggregation.
 *
 * @param[out] aggregation The priority aggregation to set the action type
 *      and action node of.
 * @param node The new action node for @a aggregation.
 * @param type The new action type for @a aggregation.
 */
RTEMS_INLINE_ROUTINE void _Priority_Set_action(
  Priority_Aggregation *aggregation,
  Priority_Node        *node,
  Priority_Action_type  type
)
{
  aggregation->Action.node = node;
  aggregation->Action.type = type;
}

/**
 * @brief Gets the next action of the priority aggregation.
 *
 * @param aggregation The priority aggregation to get the next action of.
 *
 * @retval next_action The next action of @a aggregation if RTEMS_SMP is defined.
 * @retval NULL RTEMS_SMP is not defined.
 */
RTEMS_INLINE_ROUTINE Priority_Aggregation *_Priority_Get_next_action(
  const Priority_Aggregation *aggregation
)
{
#if defined(RTEMS_SMP)
  return aggregation->Action.next;
#else
  (void) aggregation;
  return NULL;
#endif
}

/**
 * @brief Compares two priorities.
 *
 * @param left The priority control on the left hand side of the comparison.
 * @param right THe RBTree_Node to get the priority for the comparison from.
 *
 * @retval true The priority on the left hand side of the comparison is smaller.
 * @retval false The priority on the left hand side of the comparison is greater of equal.
 */
RTEMS_INLINE_ROUTINE bool _Priority_Less(
  const void        *left,
  const RBTree_Node *right
)
{
  const Priority_Control *the_left;
  const Priority_Node    *the_right;

  the_left = left;
  the_right = RTEMS_CONTAINER_OF( right, Priority_Node, Node.RBTree );

  return *the_left < the_right->priority;
}

/**
 * @brief Inserts the node with the given priority into the
 *      priority aggregation's contributors.
 *
 * This method does not handle the case that @a node was the minimal node.
 *
 * @param[in, out] aggregation The aggregation to insert the node in its contributors.
 * @param[in] node The node to insert in the contributors.
 * @param priority The priority control for the inserted node.
 *
 * @retval true The inserted node with its priority is the minimum of the RBTree.
 * @retval false The inserted node with its priority is not the minimum of the RBTree.
 */
RTEMS_INLINE_ROUTINE bool _Priority_Plain_insert(
  Priority_Aggregation *aggregation,
  Priority_Node        *node,
  Priority_Control      priority
)
{
  return _RBTree_Insert_inline(
    &aggregation->Contributors,
    &node->Node.RBTree,
    &priority,
    _Priority_Less
  );
}

/**
 * @brief Extracts the priority node from the aggregation.
 *
 * This method does not handle the case that @a node was the minimal node.
 *
 * @param[in,  out] aggregation The aggregation to extract the node from.
 * @param node The node to be extracted.
 */
RTEMS_INLINE_ROUTINE void _Priority_Plain_extract(
  Priority_Aggregation *aggregation,
  Priority_Node        *node
)
{
  _RBTree_Extract( &aggregation->Contributors, &node->Node.RBTree );
}

/**
 * @brief Updates the priority of the node in the aggregation.
 *
 * It first extracts the node and inserts it again, with the new @a node priority as key.
 *   This method does not handle the case that @a node was the minimal node.
 *
 * @param[in, out] aggregation The aggregation to change the node in.
 * @param node The node that has a new priority and will be reinserted in the aggregation.
 */
RTEMS_INLINE_ROUTINE void _Priority_Plain_changed(
  Priority_Aggregation *aggregation,
  Priority_Node        *node
)
{
  _Priority_Plain_extract( aggregation, node );
  _Priority_Plain_insert( aggregation, node, node->priority );
}

typedef void ( *Priority_Add_handler )(
  Priority_Aggregation *aggregation,
  Priority_Actions     *actions,
  void                 *arg
);

typedef void ( *Priority_Change_handler )(
  Priority_Aggregation *aggregation,
  bool                  prepend_it,
  Priority_Actions     *actions,
  void                 *arg
);

typedef void ( *Priority_Remove_handler )(
  Priority_Aggregation *aggregation,
  Priority_Actions     *actions,
  void                 *arg
);

/**
 * @brief Does nothing.
 *
 * This method does nothing.
 *
 * @param aggregation Is ignored by the method.
 * @param prepend_it Is ignored by the method.
 * @param actions Is ignored by the method.
 * @param arg Is ignored by the method.
 */
RTEMS_INLINE_ROUTINE void _Priority_Change_nothing(
  Priority_Aggregation *aggregation,
  bool                  prepend_it,
  Priority_Actions     *actions,
  void                 *arg
)
{
  (void) aggregation;
  (void) prepend_it;
  (void) actions;
  (void) arg;
}

/**
 * @brief Does nothing.
 *
 * This method does nothing.
 *
 * @param aggregation Is ignored by the method.
 * @param actions Is ignored by the method.
 * @param arg Is ignored by the method.
 */
RTEMS_INLINE_ROUTINE void _Priority_Remove_nothing(
  Priority_Aggregation *aggregation,
  Priority_Actions     *actions,
  void                 *arg
)
{
  (void) aggregation;
  (void) actions;
  (void) arg;
}

/**
 * @brief Inserts the node in a nonempty aggregation and handles change if
 *      the node is the new minimum.
 *
 * @param[in, out] aggregation The aggregation to insert the node into.
 * @param node The node to be inserted.  The node's priority is used as a key.
 * @param actions Parameter for @a change that is used if the node is the new
 *      minimum.
 * @param change The priority change handler that is called in the case that
 *      the new node is the minimum of the aggregation.
 * @param arg Arguments for @a change that is used if the node is the new
 *      minimum.
 */
RTEMS_INLINE_ROUTINE void _Priority_Non_empty_insert(
  Priority_Aggregation    *aggregation,
  Priority_Node           *node,
  Priority_Actions        *actions,
  Priority_Change_handler  change,
  void                    *arg
)
{
  bool is_new_minimum;

  _Assert( !_Priority_Is_empty( aggregation ) );
  is_new_minimum = _Priority_Plain_insert( aggregation, node, node->priority );

  if ( is_new_minimum ) {
    aggregation->Node.priority = node->priority;
    ( *change )( aggregation, false, actions, arg );
  }
}

/**
 * @brief
 *
 * @param[in, out] aggregation The aggregation to insert the node in.
 * @param node The node to be inserted.  The node's priority is used as key.
 * @param actions Priority actions for the case that the aggregation was empty before
 *      the insert or the node is the new minimum.
 * @param add Is called in the case that @a aggregation was empty before the insert.
 * @param change Is called in the case that @a aggregation was nonempty before the
 *      insert and @a node is the new minimum of the aggregation.
 * @param arg The arguments for @a change.
 */
RTEMS_INLINE_ROUTINE void _Priority_Insert(
  Priority_Aggregation    *aggregation,
  Priority_Node           *node,
  Priority_Actions        *actions,
  Priority_Add_handler     add,
  Priority_Change_handler  change,
  void                    *arg
)
{
  if ( _Priority_Is_empty( aggregation ) ) {
    _Priority_Initialize_one( aggregation, node );
    ( *add )( aggregation, actions, arg );
  } else {
    _Priority_Non_empty_insert( aggregation, node, actions, change, arg );
  }
}

/**
 * @brief Extracts the node from the aggregation.
 *
 * This method extracts the node from the aggregation and handles the
 * cases that @a aggregation is empty after the extraction, or that
 * @a node was the minimal node in @a aggregation by calling remove (if
 * empty) or change (if @a node was the minimal node).
 *
 * @param[in, out] aggregation The aggregation to extract the @a node from.
 * @param node The node to extract from @a aggregation.
 * @param actions The actions for the cases that the aggregation is empty
 *      after the extraction, or the minimal node was extracted.
 * @param remove Is called in the case that the aggregation is empty after
 *      the extraction.
 * @param change Is called in the case that the minimal node was extracted.
 * @param arg The arguments for @a remove and @a change.
 */
RTEMS_INLINE_ROUTINE void _Priority_Extract(
  Priority_Aggregation    *aggregation,
  Priority_Node           *node,
  Priority_Actions        *actions,
  Priority_Remove_handler  remove,
  Priority_Change_handler  change,
  void                    *arg
)
{
  _Priority_Plain_extract( aggregation, node );

  if ( _Priority_Is_empty( aggregation ) ) {
    ( *remove )( aggregation, actions, arg );
  } else {
    Priority_Node *min;

    min = _Priority_Get_minimum_node( aggregation );

    if ( node->priority < min->priority ) {
      aggregation->Node.priority = min->priority;
      ( *change )( aggregation, true, actions, arg );
    }
  }
}

/**
 * @brief Extracts the node from the aggregation.
 *
 * This method extracts the node from the aggregation and handles the
 * case that @a node was the minimal node in @a aggregation by calling
 * change (if @a node was the minimal node).
 *
 * @param[in, out] aggregation The aggregation to extract the @a node from.
 * @param node The node to extract from @a aggregation.
 * @param actions The actions for the case that the minimal node was extracted.
 * @param change Is called in the case that the minimal node was extracted.
 * @param arg The arguments for @a change.
 */
RTEMS_INLINE_ROUTINE void _Priority_Extract_non_empty(
  Priority_Aggregation    *aggregation,
  Priority_Node           *node,
  Priority_Actions        *actions,
  Priority_Change_handler  change,
  void                    *arg
)
{
  Priority_Node *min;

  _Priority_Plain_extract( aggregation, node );
  _Assert( !_Priority_Is_empty( aggregation ) );

  min = _Priority_Get_minimum_node( aggregation );

  if ( node->priority < min->priority ) {
    aggregation->Node.priority = min->priority;
    ( *change )( aggregation, true, actions, arg );
  }
}

/**
 * @brief Updates the priority of the node in the aggregation.
 *
 * This method handles the case that @a node was the minimal node in
 * @a aggregation.
 *
 * @param[in, out] aggregation The aggregation to change the node in.
 * @param node The node that has a new priority and will be reinserted in the aggregation.
 * @param prepend_it Indicates whether @a change should prepend if the minimal priority is
 *      incorrectly set after the change.
 * @param actions The actions for the case that the minimal priority is incorrectly set
 *      after the change.
 * @param change Is called if the minimal priority is incorrectly set after the change.
 * @param arg The arguments for @a change.
 */
RTEMS_INLINE_ROUTINE void _Priority_Changed(
  Priority_Aggregation    *aggregation,
  Priority_Node           *node,
  bool                     prepend_it,
  Priority_Actions        *actions,
  Priority_Change_handler  change,
  void                    *arg
)
{
  Priority_Node *min;

  _Priority_Plain_changed( aggregation, node );

  min = _Priority_Get_minimum_node( aggregation );

  if ( min->priority != aggregation->Node.priority ) {
    aggregation->Node.priority = min->priority;
    ( *change )( aggregation, prepend_it, actions, arg );
  }
}

/**
 * @brief Replaces one node by another.
 *
 * The new node has the priority of the old node.
 *
 * @param[in, out] aggregation The aggregation to replace @a victim by @a replacement in.
 * @param victim The node that will be replaced.
 * @param[out] replacement The node that replaces @a victim.  It obtains its priority
 *      from @a victim.
 */
RTEMS_INLINE_ROUTINE void _Priority_Replace(
  Priority_Aggregation *aggregation,
  Priority_Node        *victim,
  Priority_Node        *replacement
)
{
  replacement->priority = victim->priority;
  _RBTree_Replace_node(
    &aggregation->Contributors,
    &victim->Node.RBTree,
    &replacement->Node.RBTree
  );
}

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_PRIORITYIMPL_H */
