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
 * @ingroup RTEMSAPIClassic
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
 * @brief Integer type for compare results.
 *
 * The type is large enough to represent pointers and 32-bit signed integers.
 *
 * @see rtems_rbtree_compare.
 */
typedef long rtems_rbtree_compare_result;

/**
 * @brief Compares two red-black tree nodes.
 *
 * @param[in] first The first node.
 * @param[in] second The second node.
 *
 * @retval positive The key value of the first node is greater than the one of
 *   the second node.
 * @retval 0 The key value of the first node is equal to the one of the second
 *   node.
 * @retval negative The key value of the first node is less than the one of the
 *   second node.
 */
typedef rtems_rbtree_compare_result ( *rtems_rbtree_compare )(
  const RBTree_Node *first,
  const RBTree_Node *second
);

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
 *
 * @param[in] the_rbtree is the pointer to rbtree header
 * @param[in] compare The node compare function.
 * @param[in] starting_address is the starting address of first node
 * @param[in] number_nodes is the number of nodes in rbtree
 * @param[in] node_size is the size of node in bytes
 * @param[in] is_unique If true, then reject nodes with a duplicate key, else
 *   otherwise.
 */
void rtems_rbtree_initialize(
  rtems_rbtree_control *the_rbtree,
  rtems_rbtree_compare  compare,
  void                 *starting_address,
  size_t                number_nodes,
  size_t                node_size,
  bool                  is_unique
);

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
 * @copydoc _RBTree_Minimum()
 */
RTEMS_INLINE_ROUTINE rtems_rbtree_node *rtems_rbtree_min(
  const rtems_rbtree_control *the_rbtree
)
{
  return _RBTree_Minimum( the_rbtree );
}

/**
 * @copydoc _RBTree_Maximum()
 */
RTEMS_INLINE_ROUTINE rtems_rbtree_node *rtems_rbtree_max(
  const rtems_rbtree_control *the_rbtree
)
{
  return _RBTree_Maximum( the_rbtree );
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
  return rtems_rbtree_min( the_rbtree ) == the_node;
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
  return rtems_rbtree_max( the_rbtree ) == the_node;
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

RTEMS_INLINE_ROUTINE bool rtems_rbtree_is_equal(
  rtems_rbtree_compare_result compare_result
)
{
  return compare_result == 0;
}

RTEMS_INLINE_ROUTINE bool rtems_rbtree_is_greater(
  rtems_rbtree_compare_result compare_result
)
{
  return compare_result > 0;
}

RTEMS_INLINE_ROUTINE bool rtems_rbtree_is_lesser(
  rtems_rbtree_compare_result compare_result
)
{
  return compare_result < 0;
}

/**
 * @brief Tries to find a node for the specified key in the tree.
 *
 * @param[in] the_rbtree The red-black tree control.
 * @param[in] the_node A node specifying the key.
 * @param[in] compare The node compare function.
 * @param[in] is_unique If true, then return the first node with a key equal to
 *   the one of the node specified if it exits, else return the last node if it
 *   exists.
 *
 * @retval node A node corresponding to the key.  If the tree is not unique
 * and contains duplicate keys, the set of duplicate keys acts as FIFO.
 * @retval NULL No node exists in the tree for the key.
 */
rtems_rbtree_node* rtems_rbtree_find(
  const rtems_rbtree_control *the_rbtree,
  const rtems_rbtree_node    *the_node,
  rtems_rbtree_compare              compare,
  bool                        is_unique
);

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
 * @brief Gets a node with the minimum key value from the red-black tree.
 *
 * This function extracts a node with the minimum key value from
 * tree and returns a pointer to that node if it exists.  In case multiple
 * nodes with a minimum key value exist, then they are extracted in FIFO order.
 *
 * @param[in] the_rbtree The red-black tree control.
 *
 * @retval NULL The tree is empty.
 * @retval node A node with the minimal key value on the tree.
 */
RTEMS_INLINE_ROUTINE rtems_rbtree_node *rtems_rbtree_get_min(
  rtems_rbtree_control *the_rbtree
)
{
  rtems_rbtree_node *the_node = rtems_rbtree_min( the_rbtree );

  if ( the_node != NULL ) {
    rtems_rbtree_extract( the_rbtree, the_node );
  }

  return the_node;
}

/**
 * @brief Gets a node with the maximal key value from the red-black tree.
 *
 * This function extracts a node with the maximum key value from tree and
 * returns a pointer to that node if it exists.  In case multiple nodes with a
 * maximum key value exist, then they are extracted in LIFO order.
 *
 * @param[in] the_rbtree The red-black tree control.
 *
 * @retval NULL The tree is empty.
 * @retval node A node with the maximal key value on the tree.
 */
RTEMS_INLINE_ROUTINE rtems_rbtree_node *rtems_rbtree_get_max(
  rtems_rbtree_control *the_rbtree
)
{
  rtems_rbtree_node *the_node = rtems_rbtree_max( the_rbtree );

  if ( the_node != NULL ) {
    rtems_rbtree_extract( the_rbtree, the_node );
  }

  return the_node;
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
  return rtems_rbtree_min( the_rbtree );
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
  return rtems_rbtree_max( the_rbtree );
}

/**
 * @brief Inserts the node into the red-black tree.
 *
 * In case the node is already a node of a tree, then this function yields
 * unpredictable results.
 *
 * @param[in] the_rbtree The red-black tree control.
 * @param[in] the_node The node to insert.
 * @param[in] compare The node compare function.
 * @param[in] is_unique If true, then reject nodes with a duplicate key, else
 *   insert nodes in FIFO order in case the key value is equal to existing nodes.
 *
 * @retval NULL Successfully inserted.
 * @retval existing_node This is a unique insert and there exists a node with
 *   an equal key in the tree already.
 */
rtems_rbtree_node *rtems_rbtree_insert(
  RBTree_Control *the_rbtree,
  RBTree_Node    *the_node,
  rtems_rbtree_compare  compare,
  bool            is_unique
);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
