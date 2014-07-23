/**
 * @file
 *
 * @brief Inlined Routines Associated with Red-Black Trees
 *
 * This include file contains the bodies of the routines which are
 * associated with Red-Black Trees and inlined.
 *
 * @note  The routines in this file are ordered from simple
 *        to complex.  No other RBTree Handler routine is referenced
 *        unless it has already been defined.
 */

/*
 *  Copyright (c) 2010-2012 Gedare Bloom.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_RBTREEIMPL_H
#define _RTEMS_SCORE_RBTREEIMPL_H

#include <rtems/score/rbtree.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ScoreRBTree
 */
/**@{**/

/**
 * @brief Red-black tree visitor.
 *
 * @param[in] node The node.
 * @param[in] dir The direction.
 * @param[in] visitor_arg The visitor argument.
 *
 * @retval true Stop the iteration.
 * @retval false Continue the iteration.
 *
 * @see _RBTree_Iterate().
 */
typedef bool (*RBTree_Visitor)(
  const RBTree_Node *node,
  RBTree_Direction dir,
  void *visitor_arg
);

/**
 * @brief Red-black tree iteration.
 *
 * @param[in] rbtree The red-black tree.
 * @param[in] dir The direction.
 * @param[in] visitor The visitor.
 * @param[in] visitor_arg The visitor argument.
 */
void _RBTree_Iterate(
  const RBTree_Control *rbtree,
  RBTree_Direction dir,
  RBTree_Visitor visitor,
  void *visitor_arg
);

/**
 * @brief Get the direction opposite to @a the_dir.
 */
RTEMS_INLINE_ROUTINE RBTree_Direction _RBTree_Opposite_direction(
  RBTree_Direction the_dir
)
{
  return (RBTree_Direction) !((int) the_dir);
}

/**
 * @brief Returns the direction of the node.
 *
 * @param[in] the_node The node of interest.
 * @param[in] parent The parent of the node.  The parent must exist, thus it is
 * invalid to use this function for the root node.
 */
RTEMS_INLINE_ROUTINE RBTree_Direction _RBTree_Direction(
  const RBTree_Node *the_node,
  const RBTree_Node *parent
)
{
  return (RBTree_Direction) ( the_node != parent->child[ 0 ] );
}

/**
 * @brief Is this node red.
 *
 * This function returns true if @a the_node is red and false otherwise.
 *
 * @retval true @a the_node is red.
 * @retval false @a the_node in not red.
 */
RTEMS_INLINE_ROUTINE bool _RBTree_Is_red(
    const RBTree_Node *the_node
    )
{
  return (the_node && the_node->color == RBT_RED);
}

/**
 * @brief Returns the sibling of the node.
 *
 * @param[in] the_node The node of interest.
 * @param[in] parent The parent of the node.  The parent must exist, thus it is
 * invalid to use this function for the root node.
 *
 * @retval NULL No sibling exists.
 * @retval sibling The sibling of the node.
 */
RTEMS_INLINE_ROUTINE RBTree_Node *_RBTree_Sibling(
  const RBTree_Node *the_node,
  const RBTree_Node *parent
)
{
  RBTree_Node *left_child = parent->child[ RBT_LEFT ];

  return the_node == left_child ? parent->child[ RBT_RIGHT ] : left_child;
}

RTEMS_INLINE_ROUTINE bool _RBTree_Is_equal(
  RBTree_Compare_result compare_result
)
{
  return compare_result == 0;
}

RTEMS_INLINE_ROUTINE bool _RBTree_Is_greater(
  RBTree_Compare_result compare_result
)
{
  return compare_result > 0;
}

RTEMS_INLINE_ROUTINE bool _RBTree_Is_lesser(
  RBTree_Compare_result compare_result
)
{
  return compare_result < 0;
}

/**
 * @brief Rotates the node in the specified direction.
 *
 * The node is swapped with its child in the opposite direction if it exists.
 *
 * Sub-tree before rotation:
 * @dot
 * digraph state {
 *   parent -> the_node;
 *   the_node -> sibling [label="dir"];
 *   the_node -> child [label="opp_dir"];
 *   child -> grandchild [label="dir"];
 *   child -> grandchildsibling [label="opp_dir"];
 * }
 * @enddot
 *
 * Sub-tree after rotation:
 * @dot
 * digraph state {
 *   parent -> child;
 *   the_node -> sibling [label="dir"];
 *   the_node -> grandchild [label="opp_dir"];
 *   child -> the_node [label="dir"];
 *   child -> grandchildsibling [label="opp_dir"];
 * }
 * @enddot
 *
 * @param[in] the_node The node to rotate.
 * @param[in] dir The rotation direction.
 */
RTEMS_INLINE_ROUTINE void _RBTree_Rotate(
  RBTree_Node      *the_node,
  RBTree_Direction  dir
)
{
  RBTree_Direction  opp_dir = _RBTree_Opposite_direction( dir );
  RBTree_Node      *child = the_node->child[ opp_dir ];
  RBTree_Node      *grandchild;
  RBTree_Node      *parent;

  if ( child == NULL)
    return;

  grandchild = child->child[ dir ];
  the_node->child[ opp_dir ] = grandchild;

  if ( grandchild != NULL )
    grandchild->parent = the_node;

  child->child[ dir ] = the_node;

  parent = _RBTree_Parent( the_node );
  parent->child[ _RBTree_Direction( the_node, parent ) ] = child;

  child->parent = parent;
  the_node->parent = child;
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
