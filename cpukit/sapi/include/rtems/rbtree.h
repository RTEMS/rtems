/**
 * @file
 * 
 * @brief Constants and Structures Associated with the RBTree API in RTEMS
 * 
 * This include file contains all the constants and structures associated
 * with the RBTree API in RTEMS. The rbtree is a Red Black Tree that
 * is part of the Super Core. This is the published interface to that
 * code.
 */

/*
 *  Copyright (c) 2010 Gedare Bloom.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_RBTREE_H
#define _RTEMS_RBTREE_H

#include <rtems/score/rbtree.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup ClassicRBTrees Red-Black Trees
 *
 * @ingroup ClassicRTEMS
 *
 * @brief A Red-Black Tree container.
 *
 * The red-black tree container offers no internal protection against
 * concurrent access.  The user must ensure that at most one thread at once can
 * access a red-black tree instance.
 *
 * @{
 */

/**
 * @typedef rtems_rbtree_node
 *
 * A node that can be manipulated in the rbtree.
 */
typedef RBTree_Node rtems_rbtree_node;

/**
 * @typedef rtems_rbtree_control
 *
 * The rbtree's control anchors the rbtree.
 */
typedef RBTree_Control rtems_rbtree_control;

/**
 * @copydoc RBTree_Compare_result
 */
typedef RBTree_Compare_result rtems_rbtree_compare_result;

/**
 * @copydoc RBTree_Compare
 */
typedef RBTree_Compare rtems_rbtree_compare;

/**
 * @brief RBTree initializer for an empty rbtree with designator @a name.
 */
#define RTEMS_RBTREE_INITIALIZER_EMPTY(name) \
  RBTREE_INITIALIZER_EMPTY(name)

/**
 * @brief RBTree definition for an empty rbtree with designator @a name.
 */
#define RTEMS_RBTREE_DEFINE_EMPTY(name) \
  RBTREE_DEFINE_EMPTY(name)

/**
 * @brief Initialize a RBTree header.
 *
 * This routine initializes @a the_rbtree structure to manage the
 * contiguous array of @a number_nodes nodes which starts at
 * @a starting_address.  Each node is of @a node_size bytes.
 */
RTEMS_INLINE_ROUTINE void rtems_rbtree_initialize(
  rtems_rbtree_control *the_rbtree,
  rtems_rbtree_compare  compare,
  void                 *starting_address,
  size_t                number_nodes,
  size_t                node_size,
  bool                  is_unique
)
{
  _RBTree_Initialize( the_rbtree, compare, starting_address,
    number_nodes, node_size, is_unique);
}

/**
 * @brief Initialize this RBTree as Empty
 *
 * This routine initializes @a the_rbtree to contain zero nodes.
 */
RTEMS_INLINE_ROUTINE void rtems_rbtree_initialize_empty(
  rtems_rbtree_control *the_rbtree
)
{
  _RBTree_Initialize_empty( the_rbtree );
}

/**
 * @brief Set off RBtree.
 *
 * This function sets the next and previous fields of the @a node to NULL
 * indicating the @a node is not part of any rbtree.
 */
RTEMS_INLINE_ROUTINE void rtems_rbtree_set_off_tree(
  rtems_rbtree_node *node
)
{
  _RBTree_Set_off_tree( node );
}

/**
 * @brief Is the Node off RBTree.
 *
 * This function returns true if the @a node is not on a rbtree. A @a node is
 * off rbtree if the next and previous fields are set to NULL.
 */
RTEMS_INLINE_ROUTINE bool rtems_rbtree_is_node_off_tree(
  const rtems_rbtree_node *node
)
{
  return _RBTree_Is_node_off_tree( node );
}

/**
 * @brief Return pointer to RBTree root.
 *
 * This function returns a pointer to the root node of @a the_rbtree.
 */
RTEMS_INLINE_ROUTINE rtems_rbtree_node *rtems_rbtree_root(
  const rtems_rbtree_control *the_rbtree
)
{
  return _RBTree_Root( the_rbtree );
}

/**
 * @brief Return pointer to RBTree Minimum
 *
 * This function returns a pointer to the minimum node of @a the_rbtree.
 */
RTEMS_INLINE_ROUTINE rtems_rbtree_node *rtems_rbtree_min(
  const rtems_rbtree_control *the_rbtree
)
{
  return _RBTree_First( the_rbtree, RBT_LEFT );
}

/**
 * @brief Return pointer to RBTree maximum.
 *
 * This function returns a pointer to the maximum node of @a the_rbtree.
 */
RTEMS_INLINE_ROUTINE rtems_rbtree_node *rtems_rbtree_max(
  const rtems_rbtree_control *the_rbtree
)
{
  return _RBTree_First( the_rbtree, RBT_RIGHT );
}

/**
 * @brief Return pointer to the left child node from this node.
 *
 * This function returns a pointer to the left child node of @a the_node.
 */
RTEMS_INLINE_ROUTINE rtems_rbtree_node *rtems_rbtree_left(
  const rtems_rbtree_node *the_node
)
{
  return _RBTree_Left( the_node );
}

/**
 * @brief Return pointer to the right child node from this node.
 *
 * This function returns a pointer to the right child node of @a the_node.
 */
RTEMS_INLINE_ROUTINE rtems_rbtree_node *rtems_rbtree_right(
  const rtems_rbtree_node *the_node
)
{
  return _RBTree_Right( the_node );
}

/**
 * @copydoc _RBTree_Parent()
 */
RTEMS_INLINE_ROUTINE rtems_rbtree_node *rtems_rbtree_parent(
  const rtems_rbtree_node *the_node
)
{
  return _RBTree_Parent( the_node );
}

/**
 * @brief Is the RBTree empty.
 *
 * This function returns true if there a no nodes on @a the_rbtree and
 * false otherwise.
 */
RTEMS_INLINE_ROUTINE bool rtems_rbtree_is_empty(
  const rtems_rbtree_control *the_rbtree
)
{
  return _RBTree_Is_empty( the_rbtree );
}

/**
 * @brief Is this the minimum node on the RBTree.
 *
 * This function returns true if @a the_node is the min node on @a the_rbtree 
 * and false otherwise.
 */
RTEMS_INLINE_ROUTINE bool rtems_rbtree_is_min(
  const rtems_rbtree_control *the_rbtree,
  const rtems_rbtree_node *the_node
)
{
  return _RBTree_Is_first( the_rbtree, the_node, RBT_LEFT );
}

/**
 * @brief Is this the maximum node on the RBTree.
 *
 * This function returns true if @a the_node is the max node on @a the_rbtree 
 * and false otherwise.
 */
RTEMS_INLINE_ROUTINE bool rtems_rbtree_is_max(
  const rtems_rbtree_control *the_rbtree,
  const rtems_rbtree_node *the_node
)
{
  return _RBTree_Is_first( the_rbtree, the_node, RBT_RIGHT );
}

/**
 * @copydoc _RBTree_Is_root()
 */
RTEMS_INLINE_ROUTINE bool rtems_rbtree_is_root(
  const rtems_rbtree_node *the_node
)
{
  return _RBTree_Is_root( the_node );
}

/**
 * @copydoc _RBTree_Find()
 */
RTEMS_INLINE_ROUTINE rtems_rbtree_node* rtems_rbtree_find(
  const rtems_rbtree_control *the_rbtree,
  const rtems_rbtree_node    *the_node,
  rtems_rbtree_compare        compare,
  bool                        is_unique
)
{
  return _RBTree_Find( the_rbtree, the_node, compare, is_unique );
}

/**
 * @copydoc _RBTree_Predecessor()
 */
RTEMS_INLINE_ROUTINE rtems_rbtree_node* rtems_rbtree_predecessor(
  const rtems_rbtree_node *node
)
{
  return _RBTree_Predecessor( node );
}

/**
 * @copydoc _RBTree_Successor()
 */
RTEMS_INLINE_ROUTINE rtems_rbtree_node* rtems_rbtree_successor(
  const rtems_rbtree_node *node
)
{
  return _RBTree_Successor( node );
}

/**
 * @copydoc _RBTree_Extract()
 */
RTEMS_INLINE_ROUTINE void rtems_rbtree_extract(
  rtems_rbtree_control *the_rbtree,
  rtems_rbtree_node *the_node
)
{
  _RBTree_Extract( the_rbtree, the_node );
}

/**
 * @brief Obtain the min node on a rbtree.
 *
 * This function removes the min node from @a the_rbtree and returns
 * a pointer to that node.  If @a the_rbtree is empty, then NULL is returned.
 */

RTEMS_INLINE_ROUTINE rtems_rbtree_node *rtems_rbtree_get_min(
  rtems_rbtree_control *the_rbtree
)
{
  return _RBTree_Get( the_rbtree, RBT_LEFT );
}

/**
 * @brief Obtain the max node on a rbtree.
 *
 * This function removes the max node from @a the_rbtree and returns
 * a pointer to that node.  If @a the_rbtree is empty, then NULL is returned.
 */

RTEMS_INLINE_ROUTINE rtems_rbtree_node *rtems_rbtree_get_max(
  rtems_rbtree_control *the_rbtree
)
{
  return _RBTree_Get( the_rbtree, RBT_RIGHT );
}

/**
 * @brief Peek at the min node on a rbtree.
 *
 * This function returns a pointer to the min node from @a the_rbtree 
 * without changing the tree.  If @a the_rbtree is empty, 
 * then NULL is returned.
 */
RTEMS_INLINE_ROUTINE rtems_rbtree_node *rtems_rbtree_peek_min(
  const rtems_rbtree_control *the_rbtree
)
{
  return _RBTree_First( the_rbtree, RBT_LEFT );
}

/**
 * @brief Peek at the max node on a rbtree.
 *
 * This function returns a pointer to the max node from @a the_rbtree 
 * without changing the tree.  If @a the_rbtree is empty, 
 * then NULL is returned.
 */
RTEMS_INLINE_ROUTINE rtems_rbtree_node *rtems_rbtree_peek_max(
  const rtems_rbtree_control *the_rbtree
)
{
  return _RBTree_First( the_rbtree, RBT_RIGHT );
}

/**
 * @copydoc _RBTree_Insert()
 */
RTEMS_INLINE_ROUTINE rtems_rbtree_node *rtems_rbtree_insert(
  rtems_rbtree_control *the_rbtree,
  rtems_rbtree_node    *the_node,
  rtems_rbtree_compare  compare,
  bool                  is_unique
)
{
  return _RBTree_Insert( the_rbtree, the_node, compare, is_unique );
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
