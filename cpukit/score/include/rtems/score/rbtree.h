/**
 *  @file  rtems/score/rbtree.h
 *
 *  @brief Constants and Structures Associated with the Red-Black Tree Handler
 *
 *  This include file contains all the constants and structures associated
 *  with the Red-Black Tree Handler.
 */

/*
 *  Copyright (c) 2010 Gedare Bloom.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_RBTREE_H
#define _RTEMS_SCORE_RBTREE_H

#include <stddef.h>

#include <rtems/score/address.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @defgroup ScoreRBTree Red-Black Tree Handler
 *
 *  @ingroup Score
 *
 *  The Red-Black Tree Handler is used to manage sets of entities.  This handler
 *  provides two data structures.  The rbtree Node data structure is included
 *  as the first part of every data structure that will be placed on
 *  a RBTree.  The second data structure is rbtree Control which is used
 *  to manage a set of rbtree Nodes.
 */
/**@{*/

/**
 *  @typedef RBTree_Node
 *
 *  This type definition promotes the name for the RBTree Node used by
 *  all RTEMS code.  It is a separate type definition because a forward
 *  reference is required to define it.  See @ref RBTree_Node_struct for
 *  detailed information.
 */
typedef struct RBTree_Node_struct RBTree_Node;

/**
 * This enum type defines the colors available for the RBTree Nodes
 */
typedef enum {
  RBT_BLACK,
  RBT_RED
} RBTree_Color;

/**
 *  @struct RBTree_Node_struct
 *
 *  This is used to manage each element (node) which is placed
 *  on a RBT.
 *
 *  @note Typically, a more complicated structure will use the
 *        rbtree package.  The more complicated structure will
 *        include a rbtree node as the first element in its
 *        control structure.  It will then call the rbtree package
 *        with a pointer to that node element.  The node pointer
 *        and the higher level structure start at the same address
 *        so the user can cast the pointers back and forth.
 *
 */
struct RBTree_Node_struct {
  /** This points to the node's parent */
  RBTree_Node *parent;
  /** child[0] points to the left child, child[1] points to the right child */
  RBTree_Node *child[2];
  /** The color of the node. Either red or black */
  RBTree_Color color;
};

/**
 *  This type indicates the direction.
 */
typedef enum {
  RBT_LEFT=0,
  RBT_RIGHT=1
} RBTree_Direction;

/**
 * @brief Integer type for compare results.
 *
 * The type is large enough to represent pointers and 32-bit signed integers.
 *
 * @see RBTree_Compare.
 */
typedef long RBTree_Compare_result;

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
typedef RBTree_Compare_result ( *RBTree_Compare )(
  const RBTree_Node *first,
  const RBTree_Node *second
);

/**
 *  @struct RBTree_Control
 *
 * This is used to manage a RBT.  A rbtree consists of a tree of zero or more
 * nodes.
 *
 * @note This implementation does not require special checks for
 *   manipulating the root element of the RBT.
 *   To accomplish this the @a RBTree_Control structure can be overlaid
 *   with a @ref RBTree_Node structure to act as a "dummy root",
 *   which has a NULL parent and its left child is the root.
 */

/* the RBTree_Control is actually part of the RBTree structure as an
 * RBTree_Node. The mapping of fields from RBTree_Control to RBTree_Node are:
 *   permanent_null == parent
 *   root == left
 *   first[0] == right
 */
typedef struct {
  /** This points to a NULL. Useful for finding the root. */
  RBTree_Node *permanent_null;
  /** This points to the root node of the RBT. */
  RBTree_Node *root;
  /** This points to the min and max nodes of this RBT. */
  RBTree_Node *first[2];
} RBTree_Control;

/**
 *  @brief RBTree initializer for an empty rbtree with designator @a name.
 */
#define RBTREE_INITIALIZER_EMPTY( name ) \
  { NULL, NULL, { NULL, NULL } }

/**
 *  @brief RBTree definition for an empty rbtree with designator @a name.
 */
#define RBTREE_DEFINE_EMPTY( name ) \
  RBTree_Control name = RBTREE_INITIALIZER_EMPTY( name )

/**
 *  @brief RBTree_Node initializer for an empty node with designator @a name.
 */
#define RBTREE_NODE_INITIALIZER_EMPTY( name ) \
  { NULL, { NULL, NULL }, RBT_RED }

/**
 *  @brief RBTree definition for an empty rbtree with designator @a name.
 */
#define RBTREE_NODE_DEFINE_EMPTY( name ) \
  RBTree_Node name = RBTREE_NODE_INITIALIZER_EMPTY( name )

/**
 *  @brief Initialize a RBTree Header.
 *
 *  This routine initializes @a the_rbtree structure to manage the
 *  contiguous array of @a number_nodes nodes which starts at
 *  @a starting_address.  Each node is of @a node_size bytes.
 *
 *  @param[in] the_rbtree is the pointer to rbtree header
 *  @param[in] compare The node compare function.
 *  @param[in] starting_address is the starting address of first node
 *  @param[in] number_nodes is the number of nodes in rbtree
 *  @param[in] node_size is the size of node in bytes
 *  @param[in] is_unique If true, then reject nodes with a duplicate key, else
 *    otherwise.
 */
void _RBTree_Initialize(
  RBTree_Control *the_rbtree,
  RBTree_Compare  compare,
  void           *starting_address,
  size_t          number_nodes,
  size_t          node_size,
  bool            is_unique
);

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
RBTree_Node *_RBTree_Find(
  const RBTree_Control *the_rbtree,
  const RBTree_Node    *the_node,
  RBTree_Compare        compare,
  bool                  is_unique
);

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
RBTree_Node *_RBTree_Insert(
  RBTree_Control *the_rbtree,
  RBTree_Node    *the_node,
  RBTree_Compare  compare,
  bool            is_unique
);

/**
 * @brief Extracts (removes) the node from the red-black tree.
 *
 * This function does not set the node off-tree.  In case this is desired, then
 * call _RBTree_Set_off_tree() after the extraction.
 *
 * In case the node to extract is not a node of the tree, then this function
 * yields unpredictable results.
 *
 * @param[in] the_rbtree The red-black tree control.
 * @param[in] the_node The node to extract.
 */
void _RBTree_Extract(
  RBTree_Control *the_rbtree,
  RBTree_Node *the_node
);

/**
 * @brief Returns the in-order next node of a node.
 *
 * @param[in] node The node.
 * @param[in] dir The direction.
 *
 * @retval NULL The in-order next node does not exist.
 * @retval otherwise The next node.
 */
RBTree_Node *_RBTree_Next(
  const RBTree_Node *node,
  RBTree_Direction dir
);

/**
 * @brief Sets a red-black tree node as off-tree.
 *
 * Do not use this function on nodes which are a part of a tree.
 *
 * @param[in] the_node The node to set off-tree.
 *
 * @see _RBTree_Is_node_off_tree().
 */
RTEMS_INLINE_ROUTINE void _RBTree_Set_off_tree( RBTree_Node *the_node )
{
  the_node->parent = NULL;
}

/**
 * @brief Returns true, if this red-black tree node is off-tree, and false
 * otherwise.
 *
 * @param[in] the_node The node to test.
 *
 * @retval true The node is not a part of a tree (off-tree).
 * @retval false Otherwise.
 *
 * @see _RBTree_Set_off_tree().
 */
RTEMS_INLINE_ROUTINE bool _RBTree_Is_node_off_tree(
  const RBTree_Node *the_node
)
{
  return the_node->parent == NULL;
}

/**
 * @brief Returns a pointer to root node of the red-black tree.
 *
 * The root node may change after insert or extract operations.
 *
 * @param[in] the_rbtree The red-black tree control.
 *
 * @retval NULL The tree is empty.
 * @retval root The root node.
 *
 * @see _RBTree_Is_root().
 */
RTEMS_INLINE_ROUTINE RBTree_Node *_RBTree_Root(
  const RBTree_Control *the_rbtree
)
{
  return the_rbtree->root;
}

/**
 * @brief Return pointer to RBTree's first node.
 *
 * This function returns a pointer to the first node on @a the_rbtree,
 * where @a dir specifies whether to return the minimum (0) or maximum (1).
 */
RTEMS_INLINE_ROUTINE RBTree_Node *_RBTree_First(
  const RBTree_Control *the_rbtree,
  RBTree_Direction dir
)
{
  return the_rbtree->first[dir];
}

/**
 * @brief Returns a pointer to the parent of this node.
 *
 * The node must have a parent, thus it is invalid to use this function for the
 * root node or a node that is not part of a tree.  To test for the root node
 * compare with _RBTree_Root() or use _RBTree_Is_root().
 *
 * @param[in] the_node The node of interest.
 *
 * @retval parent The parent of this node.
 * @retval undefined The node is the root node or not part of a tree.
 */
RTEMS_INLINE_ROUTINE RBTree_Node *_RBTree_Parent(
  const RBTree_Node *the_node
)
{
  return the_node->parent;
}

/**
 * @brief Return pointer to the left of this node.
 *
 * This function returns a pointer to the left node of this node.
 *
 * @param[in] the_node is the node to be operated upon.
 *
 * @return This method returns the left node on the rbtree.
 */
RTEMS_INLINE_ROUTINE RBTree_Node *_RBTree_Left(
  const RBTree_Node *the_node
)
{
  return the_node->child[RBT_LEFT];
}

/**
 * @brief Return pointer to the right of this node.
 *
 * This function returns a pointer to the right node of this node.
 *
 * @param[in] the_node is the node to be operated upon.
 *
 * @return This method returns the right node on the rbtree.
 */
RTEMS_INLINE_ROUTINE RBTree_Node *_RBTree_Right(
  const RBTree_Node *the_node
)
{
  return the_node->child[RBT_RIGHT];
}

/**
 * @brief Is the RBTree empty.
 *
 * This function returns true if there are no nodes on @a the_rbtree and
 * false otherwise.
 *
 * @param[in] the_rbtree is the rbtree to be operated upon.
 *
 * @retval true There are no nodes on @a the_rbtree.
 * @retval false There are nodes on @a the_rbtree.
 */
RTEMS_INLINE_ROUTINE bool _RBTree_Is_empty(
  const RBTree_Control *the_rbtree
)
{
  return (the_rbtree->root == NULL);
}

/**
 * @brief Is this the first node on the RBTree.
 *
 * This function returns true if @a the_node is the first node on
 * @a the_rbtree and false otherwise. @a dir specifies whether first means
 * minimum (0) or maximum (1).
 *
 * @retval true @a the_node is the first node on @a the_rbtree.
 * @retval false @a the_node is not the first node on @a the_rbtree.
 *
 */
RTEMS_INLINE_ROUTINE bool _RBTree_Is_first(
  const RBTree_Control *the_rbtree,
  const RBTree_Node *the_node,
  RBTree_Direction dir
)
{
  return (the_node == _RBTree_First(the_rbtree, dir));
}

/**
 * @brief Returns true if this node is the root node of a red-black tree, and
 * false otherwise.
 *
 * The root node may change after insert or extract operations.  In case the
 * node is not a node of a tree, then this function yields unpredictable
 * results.
 *
 * @param[in] the_node The node of interest.
 *
 * @retval true The node is the root node.
 * @retval false Otherwise.
 *
 * @see _RBTree_Root().
 */
RTEMS_INLINE_ROUTINE bool _RBTree_Is_root(
  const RBTree_Node *the_node
)
{
  return _RBTree_Parent( _RBTree_Parent( the_node ) ) == NULL;
}

/**
 * @brief Initialize this RBTree as empty.
 *
 * This routine initializes @a the_rbtree to contain zero nodes.
 */
RTEMS_INLINE_ROUTINE void _RBTree_Initialize_empty(
  RBTree_Control *the_rbtree
)
{
  the_rbtree->permanent_null   = NULL;
  the_rbtree->root             = NULL;
  the_rbtree->first[RBT_LEFT]  = NULL;
  the_rbtree->first[RBT_RIGHT] = NULL;
}

/**
 * @brief Returns the predecessor of a node.
 *
 * @param[in] node is the node.
 *
 * @retval NULL The predecessor does not exist.  Otherwise it returns
 *         the predecessor node.
 */
RTEMS_INLINE_ROUTINE RBTree_Node *_RBTree_Predecessor(
  const RBTree_Node *node
)
{
  return _RBTree_Next( node, RBT_LEFT );
}

/**
 * @brief Returns the successor of a node.
 *
 * @param[in] node is the node.
 *
 * @retval NULL The successor does not exist.  Otherwise the successor node.
 */
RTEMS_INLINE_ROUTINE RBTree_Node *_RBTree_Successor(
  const RBTree_Node *node
)
{
  return _RBTree_Next( node, RBT_RIGHT );
}

/**
 * @brief Gets a node with an extremal key value from the red-black tree.
 *
 * This function extracts a node with the minimum or maximum key value from
 * tree and returns a pointer to that node if it exists.  In case multiple
 * nodes with a minimum key value exist, then they are extracted in FIFO order.
 * In case multiple nodes with a maximum key value exist, then they are
 * extracted in LIFO order.
 *
 * @param[in] the_rbtree The red-black tree control.
 * @param[in] dir Specifies whether to get a node with the minimum (RBT_LEFT)
 *   or maximum (RBT_RIGHT) key value.
 *
 * @retval NULL The tree is empty.
 * @retval extremal_node A node with a minimal or maximal key value on the
 *   tree.
 */
RTEMS_INLINE_ROUTINE RBTree_Node *_RBTree_Get(
  RBTree_Control *the_rbtree,
  RBTree_Direction dir
)
{
  RBTree_Node *the_node = the_rbtree->first[ dir ];

  if ( the_node != NULL ) {
    _RBTree_Extract( the_rbtree, the_node );
  }

  return the_node;
}

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
