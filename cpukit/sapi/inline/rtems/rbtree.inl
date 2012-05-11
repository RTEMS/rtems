/**
 * @file rtems/rbtree.inl
 *
 *  This include file contains all the constants and structures associated
 *  with the RBTree API in RTEMS. The rbtree is a Red Black Tree that
 *  is part of the Super Core. This is the published interface to that
 *  code.
 */
 
/*
 *  Copyright (c) 2010-2012 Gedare Bloom.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_RBTREE_H
# error "Never use <rtems/rbtree.inl> directly; include <rtems/rbtree.h> instead."
#endif

#ifndef _RTEMS_RBTREE_INL
#define _RTEMS_RBTREE_INL

#include <rtems/score/rbtree.inl>

/**
 *  @brief Initialize a RBTree Header
 *
 *  This routine initializes @a the_rbtree structure to manage the
 *  contiguous array of @a number_nodes nodes which starts at
 *  @a starting_address.  Each node is of @a node_size bytes.
 */
RTEMS_INLINE_ROUTINE void rtems_rbtree_initialize(
  rtems_rbtree_control          *the_rbtree,
  rtems_rbtree_compare_function  compare_function,
  void                          *starting_address,
  size_t                         number_nodes,
  size_t                         node_size,
  bool                           is_unique
)
{
  _RBTree_Initialize( the_rbtree, compare_function, starting_address,
    number_nodes, node_size, is_unique);
}

/**
 *  @brief Initialize this RBTree as Empty
 *
 *  This routine initializes @a the_rbtree to contain zero nodes.
 */
RTEMS_INLINE_ROUTINE void rtems_rbtree_initialize_empty(
  rtems_rbtree_control          *the_rbtree,
  rtems_rbtree_compare_function  compare_function,
  bool                           is_unique
)
{
  _RBTree_Initialize_empty( the_rbtree, compare_function, is_unique );
}

/**
 *  @brief Set off rbtree
 *
 *  This function sets the next and previous fields of the @a node to NULL
 *  indicating the @a node is not part of any rbtree.
 */
RTEMS_INLINE_ROUTINE void rtems_rbtree_set_off_rbtree(
  rtems_rbtree_node *node
)
{
  _RBTree_Set_off_rbtree( node );
}

/**
 *  @brief Is the Node off RBTree
 *
 *  This function returns true if the @a node is not on a rbtree. A @a node is
 *  off rbtree if the next and previous fields are set to NULL.
 */
RTEMS_INLINE_ROUTINE bool rtems_rbtree_is_node_off_rbtree(
  const rtems_rbtree_node *node
)
{
  return _RBTree_Is_node_off_rbtree( node );
}

/**
 *  @brief Is the RBTree Node Pointer NULL
 *
 *  This function returns true if @a the_node is NULL and false otherwise.
 */
RTEMS_INLINE_ROUTINE bool rtems_rbtree_is_null_node(
  const rtems_rbtree_node *the_node
)
{
  return _RBTree_Is_null_node( the_node );
}

/**
 *  @brief Return pointer to RBTree Root
 *
 *  This function returns a pointer to the root node of @a the_rbtree.
 */
RTEMS_INLINE_ROUTINE rtems_rbtree_node *rtems_rbtree_root(
  const rtems_rbtree_control *the_rbtree
)
{
  return _RBTree_Root( the_rbtree );
}

/**
 *  @brief Return pointer to RBTree Minimum
 *
 *  This function returns a pointer to the minimum node of @a the_rbtree.
 */
RTEMS_INLINE_ROUTINE rtems_rbtree_node *rtems_rbtree_min(
  const rtems_rbtree_control *the_rbtree
)
{
  return _RBTree_First( the_rbtree, RBT_LEFT );
}

/**
 *  @brief Return pointer to RBTree Maximum
 *
 *  This function returns a pointer to the maximum node of @a the_rbtree.
 */
RTEMS_INLINE_ROUTINE rtems_rbtree_node *rtems_rbtree_max(
  const rtems_rbtree_control *the_rbtree
)
{
  return _RBTree_First( the_rbtree, RBT_RIGHT );
}

/**
 *  @brief Return pointer to the left child node from this node
 *
 *  This function returns a pointer to the left child node of @a the_node.
 */
RTEMS_INLINE_ROUTINE rtems_rbtree_node *rtems_rbtree_left(
  const rtems_rbtree_node *the_node
)
{
  return _RBTree_Left( the_node );
}

/**
 *  @brief Return pointer to the right child node from this node
 *
 *  This function returns a pointer to the right child node of @a the_node.
 */
RTEMS_INLINE_ROUTINE rtems_rbtree_node *rtems_rbtree_right(
  const rtems_rbtree_node *the_node
)
{
  return _RBTree_Right( the_node );
}

/**
 *  @brief Return pointer to the parent child node from this node
 *
 *  This function returns a pointer to the parent node of @a the_node.
 */
RTEMS_INLINE_ROUTINE rtems_rbtree_node *rtems_rbtree_parent(
  const rtems_rbtree_node *the_node
)
{
  return _RBTree_Parent( the_node );
}

/**
 *  @brief Are Two Nodes Equal
 *
 *  This function returns true if @a left and @a right are equal,
 *  and false otherwise.
 */
RTEMS_INLINE_ROUTINE bool rtems_rbtree_are_nodes_equal(
  const rtems_rbtree_node *left,
  const rtems_rbtree_node *right
)
{
  return _RBTree_Are_nodes_equal( left, right );
}

/**
 *  @brief Is the RBTree Empty
 *
 *  This function returns true if there a no nodes on @a the_rbtree and
 *  false otherwise.
 */
RTEMS_INLINE_ROUTINE bool rtems_rbtree_is_empty(
  const rtems_rbtree_control *the_rbtree
)
{
  return _RBTree_Is_empty( the_rbtree );
}

/**
 *  @brief Is this the Minimum Node on the RBTree
 *
 *  This function returns true if @a the_node is the min node on @a the_rbtree 
 *  and false otherwise.
 */
RTEMS_INLINE_ROUTINE bool rtems_rbtree_is_min(
  const rtems_rbtree_control *the_rbtree,
  const rtems_rbtree_node *the_node
)
{
  return _RBTree_Is_first( the_rbtree, the_node, RBT_LEFT );
}

/**
 *  @brief Is this the Maximum Node on the RBTree
 *
 *  This function returns true if @a the_node is the max node on @a the_rbtree 
 *  and false otherwise.
 */
RTEMS_INLINE_ROUTINE bool rtems_rbtree_is_max(
  const rtems_rbtree_control *the_rbtree,
  const rtems_rbtree_node *the_node
)
{
  return _RBTree_Is_first( the_rbtree, the_node, RBT_RIGHT );
}


/**
 *  @brief Does this RBTree have only One Node
 *
 *  This function returns true if there is only one node on @a the_rbtree and
 *  false otherwise.
 */
RTEMS_INLINE_ROUTINE bool rtems_rbtree_has_only_one_node(
  const rtems_rbtree_control *the_rbtree
)
{
  return _RBTree_Has_only_one_node( the_rbtree );
}

/**
 *  @brief Is this Node the RBTree Root
 *
 *  This function returns true if @a the_node is the root of @a the_rbtree and
 *  false otherwise.
 */
RTEMS_INLINE_ROUTINE bool rtems_rbtree_is_root(
  const rtems_rbtree_control *the_rbtree,
  const rtems_rbtree_node *the_node
)
{
  return _RBTree_Is_root( the_rbtree, the_node );
}

/**
 * @copydoc _RBTree_Find_unprotected()
 */
RTEMS_INLINE_ROUTINE rtems_rbtree_node* rtems_rbtree_find_unprotected(
    rtems_rbtree_control *the_rbtree,
    rtems_rbtree_node *the_node
)
{
  return _RBTree_Find_unprotected( the_rbtree, the_node );
}

/** @brief Find the node with given key in the tree
 *
 *  This function returns a pointer to the node having key equal to the key
 *  of @a the_node if it exists within @a the_rbtree, and NULL if not.
 *  @a the_node has to be made up before a search.
 *
 *  @note If the tree is not unique and contains duplicate keys, the set
 *        of duplicate keys acts as FIFO.
 */
RTEMS_INLINE_ROUTINE rtems_rbtree_node* rtems_rbtree_find(
  rtems_rbtree_control *the_rbtree,
  rtems_rbtree_node *the_node
)
{
  return _RBTree_Find( the_rbtree, the_node );
}

/**
 * @copydoc _RBTree_Predecessor_unprotected()
 */
RTEMS_INLINE_ROUTINE rtems_rbtree_node* rtems_rbtree_predecessor_unprotected(
  const rtems_rbtree_node *node
)
{
  return _RBTree_Predecessor_unprotected( node );
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
 * @copydoc _RBTree_Successor_unprotected()
 */
RTEMS_INLINE_ROUTINE rtems_rbtree_node* rtems_rbtree_successor_unprotected(
  const rtems_rbtree_node *node
)
{
  return _RBTree_Successor_unprotected( node );
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
 * @copydoc _RBTree_Extract_unprotected()
 */
RTEMS_INLINE_ROUTINE void rtems_rbtree_extract_unprotected(
  rtems_rbtree_control *the_rbtree,
  rtems_rbtree_node *the_node
)
{
  _RBTree_Extract_unprotected( the_rbtree, the_node );
}

/**
 *  @brief Extract the specified node from a rbtree
 *
 *  This routine extracts @a the_node from @a the_rbtree on which it resides.
 *  It disables interrupts to ensure the atomicity of the extract operation.
 */
RTEMS_INLINE_ROUTINE void rtems_rbtree_extract(
  rtems_rbtree_control *the_rbtree,
  rtems_rbtree_node *the_node
)
{
  _RBTree_Extract( the_rbtree, the_node );
}

/**
 *  @brief Obtain the min node on a rbtree
 *
 *  This function removes the min node from @a the_rbtree and returns
 *  a pointer to that node.  If @a the_rbtree is empty, then NULL is returned.
 */

RTEMS_INLINE_ROUTINE rtems_rbtree_node *rtems_rbtree_get_min_unprotected(
  rtems_rbtree_control *the_rbtree
)
{
  return _RBTree_Get_unprotected( the_rbtree, RBT_LEFT );
}

/**
 *  @brief Obtain the min node on a rbtree
 *
 *  This function removes the min node from @a the_rbtree and returns
 *  a pointer to that node.  If @a the_rbtree is empty, then NULL is returned.
 *  It disables interrupts to ensure the atomicity of the get operation.
 */
RTEMS_INLINE_ROUTINE rtems_rbtree_node *rtems_rbtree_get_min(
  rtems_rbtree_control *the_rbtree
)
{
  return _RBTree_Get( the_rbtree, RBT_LEFT );
}

/**
 *  @brief Obtain the max node on a rbtree
 *
 *  This function removes the max node from @a the_rbtree and returns
 *  a pointer to that node.  If @a the_rbtree is empty, then NULL is returned.
 */

RTEMS_INLINE_ROUTINE rtems_rbtree_node *rtems_rbtree_get_max_unprotected(
  rtems_rbtree_control *the_rbtree
)
{
  return _RBTree_Get_unprotected( the_rbtree, RBT_RIGHT );
}

/**
 *  @brief Obtain the max node on a rbtree
 *
 *  This function removes the max node from @a the_rbtree and returns
 *  a pointer to that node.  If @a the_rbtree is empty, then NULL is returned.
 *  It disables interrupts to ensure the atomicity of the get operation.
 */
RTEMS_INLINE_ROUTINE rtems_rbtree_node *rtems_rbtree_get_max(
  rtems_rbtree_control *the_rbtree
)
{
  return _RBTree_Get( the_rbtree, RBT_RIGHT );
}

/**
 *  @brief Peek at the min node on a rbtree
 *
 *  This function returns a pointer to the min node from @a the_rbtree 
 *  without changing the tree.  If @a the_rbtree is empty, 
 *  then NULL is returned.
 */
RTEMS_INLINE_ROUTINE rtems_rbtree_node *rtems_rbtree_peek_min(
  const rtems_rbtree_control *the_rbtree
)
{
  return _RBTree_First( the_rbtree, RBT_LEFT );
}

/**
 *  @brief Peek at the max node on a rbtree
 *
 *  This function returns a pointer to the max node from @a the_rbtree 
 *  without changing the tree.  If @a the_rbtree is empty, 
 *  then NULL is returned.
 */
RTEMS_INLINE_ROUTINE rtems_rbtree_node *rtems_rbtree_peek_max(
  const rtems_rbtree_control *the_rbtree
)
{
  return _RBTree_First( the_rbtree, RBT_RIGHT );
}

/**
 * @copydoc _RBTree_Find_header_unprotected()
 */
RTEMS_INLINE_ROUTINE rtems_rbtree_control *rtems_rbtree_find_header_unprotected(
  rtems_rbtree_node *the_node
)
{
  return _RBTree_Find_header_unprotected( the_node );
}

/**
 *  @brief Find the control header of the tree containing a given node.
 *
 *  This routine finds the rtems_rbtree_control structure of the tree 
 *  containing @a the_node.
 *  It disables interrupts to ensure the atomicity of the find operation.
 */
RTEMS_INLINE_ROUTINE rtems_rbtree_control *rtems_rbtree_find_header(
  rtems_rbtree_node *the_node
)
{
  return _RBTree_Find_header( the_node );
}

/**
 * @copydoc _RBTree_Insert_unprotected()
 */
RTEMS_INLINE_ROUTINE rtems_rbtree_node *rtems_rbtree_insert_unprotected(
  rtems_rbtree_control *the_rbtree,
  rtems_rbtree_node *the_node
)
{
  return _RBTree_Insert_unprotected( the_rbtree, the_node );
}

/**
 *  @brief Insert a node on a rbtree
 *
 *  This routine inserts @a the_node on @a the_rbtree.
 *  It disables interrupts to ensure the atomicity of the insert operation.
 *
 *  @retval 0 Successfully inserted.
 *  @retval -1 NULL @a the_node.
 *  @retval RBTree_Node* if one with equal key to the key of @a the_node exists
 *          in an unique @a the_rbtree.
 */
RTEMS_INLINE_ROUTINE rtems_rbtree_node *rtems_rbtree_insert(
  rtems_rbtree_control *the_rbtree,
  rtems_rbtree_node *the_node
)
{
  return _RBTree_Insert( the_rbtree, the_node );
}

/** @brief Determines whether the tree is unique
 */
RTEMS_INLINE_ROUTINE bool rtems_rbtree_is_unique(
  const rtems_rbtree_control *the_rbtree
)
{
  return _RBTree_Is_unique(the_rbtree);
}

#endif
/* end of include file */
