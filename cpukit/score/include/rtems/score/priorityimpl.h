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

RTEMS_INLINE_ROUTINE void _Priority_Actions_initialize_empty(
  Priority_Actions *actions
)
{
  actions->actions = NULL;
}

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

RTEMS_INLINE_ROUTINE bool _Priority_Actions_is_empty(
  const Priority_Actions *actions
)
{
  return actions->actions == NULL;
}

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

RTEMS_INLINE_ROUTINE Priority_Aggregation *_Priority_Actions_move(
  Priority_Actions *actions
)
{
  Priority_Aggregation *aggregation;

  aggregation = actions->actions;
  actions->actions = NULL;

  return aggregation;
}

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

RTEMS_INLINE_ROUTINE void _Priority_Node_initialize(
  Priority_Node    *node,
  Priority_Control  priority
)
{
  node->priority = priority;
  _RBTree_Initialize_node( &node->Node.RBTree );
}

RTEMS_INLINE_ROUTINE void _Priority_Node_set_priority(
  Priority_Node    *node,
  Priority_Control  priority
)
{
  node->priority = priority;
}

RTEMS_INLINE_ROUTINE void _Priority_Node_set_inactive(
  Priority_Node *node
)
{
  _RBTree_Set_off_tree( &node->Node.RBTree );
}

RTEMS_INLINE_ROUTINE bool _Priority_Node_is_active(
  const Priority_Node *node
)
{
  return !_RBTree_Is_node_off_tree( &node->Node.RBTree );
}

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

RTEMS_INLINE_ROUTINE bool _Priority_Is_empty(
  const Priority_Aggregation *aggregation
)
{
  return _RBTree_Is_empty( &aggregation->Contributors );
}

RTEMS_INLINE_ROUTINE Priority_Control _Priority_Get_priority(
  const Priority_Aggregation *aggregation
)
{
  return aggregation->Node.priority;
}

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

RTEMS_INLINE_ROUTINE Priority_Node *_Priority_Get_minimum_node(
  const Priority_Aggregation *aggregation
)
{
  return (Priority_Node *) _RBTree_Minimum( &aggregation->Contributors );
}

RTEMS_INLINE_ROUTINE void _Priority_Set_action_node(
  Priority_Aggregation *aggregation,
  Priority_Node        *node
)
{
  aggregation->Action.node = node;
}

RTEMS_INLINE_ROUTINE void _Priority_Set_action_type(
  Priority_Aggregation *aggregation,
  Priority_Action_type  type
)
{
  aggregation->Action.type = type;
}

RTEMS_INLINE_ROUTINE void _Priority_Set_action(
  Priority_Aggregation *aggregation,
  Priority_Node        *node,
  Priority_Action_type  type
)
{
  aggregation->Action.node = node;
  aggregation->Action.type = type;
}

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

RTEMS_INLINE_ROUTINE void _Priority_Plain_extract(
  Priority_Aggregation *aggregation,
  Priority_Node        *node
)
{
  _RBTree_Extract( &aggregation->Contributors, &node->Node.RBTree );
}

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

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_PRIORITYIMPL_H */
