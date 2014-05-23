/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
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

#ifndef _RTEMS_SCORE_RESOURCEIMPL_H
#define _RTEMS_SCORE_RESOURCEIMPL_H

#include <rtems/score/resource.h>
#include <rtems/score/chainimpl.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @addtogroup ScoreResource
 *
 * @{
 */

/**
 * @brief Visitor function for resource node iteration.
 *
 * The visitor is allowed to extract the node.
 *
 * @param[in] node The current resource node.
 * @param[in] arg The argument passed to _Resource_Iterate().
 *
 * @retval true Stop the iteration.
 * @retval false Continue the iteration.
 */
typedef bool (*Resource_Node_visitor)( Resource_Node *node, void *arg );

/**
 * @brief Iterates over all nodes of a resource dependency tree.
 *
 * @param[in] top The top node to start the iteration.  The visitor function is
 * not invoked for the top node.
 * @param[in] visitor The visitor function.
 * @param[in] arg The argument for the visitor function.
 */
void _Resource_Iterate(
  Resource_Node         *top,
  Resource_Node_visitor  visitor,
  void                  *arg
);

RTEMS_INLINE_ROUTINE void _Resource_Node_initialize( Resource_Node *node )
{
  node->dependency = NULL;
  node->root = node;
  _Chain_Initialize_empty( &node->Resources );
}

RTEMS_INLINE_ROUTINE void _Resource_Node_set_dependency(
  Resource_Node    *node,
  Resource_Control *dependency
)
{
  node->dependency = dependency;
}

RTEMS_INLINE_ROUTINE Resource_Node *_Resource_Node_get_root(
  const Resource_Node *node
)
{
  return node->root;
}

RTEMS_INLINE_ROUTINE void _Resource_Node_set_root(
  Resource_Node *node,
  Resource_Node *root
)
{
  node->root = root;
}

RTEMS_INLINE_ROUTINE bool _Resource_Node_owns_resources( const Resource_Node *node )
{
  return !_Chain_Is_empty( &node->Resources );
}

RTEMS_INLINE_ROUTINE void _Resource_Node_add_resource(
  Resource_Node    *node,
  Resource_Control *resource
)
{
  _Chain_Prepend_unprotected( &node->Resources, &resource->Node );
}

RTEMS_INLINE_ROUTINE void _Resource_Node_extract( Resource_Node *node )
{
  _Chain_Extract_unprotected( &node->Node );
}

RTEMS_INLINE_ROUTINE void _Resource_Initialize( Resource_Control *resource )
{
  resource->owner = NULL;
  _Chain_Initialize_empty( &resource->Rivals );
}

RTEMS_INLINE_ROUTINE void _Resource_Add_rival(
  Resource_Control *resource,
  Resource_Node    *node
)
{
  _Chain_Append_unprotected( &resource->Rivals, &node->Node );
}

RTEMS_INLINE_ROUTINE void _Resource_Extract( Resource_Control *resource )
{
  _Chain_Extract_unprotected( &resource->Node );
}

RTEMS_INLINE_ROUTINE Resource_Node *_Resource_Get_owner(
  const Resource_Control *resource
)
{
  return resource->owner;
}

RTEMS_INLINE_ROUTINE void _Resource_Set_owner(
  Resource_Control *resource,
  Resource_Node    *owner
)
{
  resource->owner = owner;
}

/**
 * @brief Returns true if this is the most recently obtained resource of the
 * node, and false otherwise.
 *
 * Resources are organized in last in first out order (LIFO).
 *
 * @param[in] resource The resource in question.
 * @param[in] node The node that obtained the resource.
 */
RTEMS_INLINE_ROUTINE bool _Resource_Is_most_recently_obtained(
  const Resource_Control *resource,
  const Resource_Node    *node
)
{
  return &resource->Node == _Chain_Immutable_first( &node->Resources );
}

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_RESOURCEIMPL_H */
