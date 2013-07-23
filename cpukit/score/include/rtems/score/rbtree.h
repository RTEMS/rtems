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
 *  http://www.rtems.com/license/LICENSE.
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
 * @brief Macro to return the structure containing the @a node.
 *
 * This macro returns a pointer of type @a container_type that points
 * to the structure containing @a node, where @a node_field_name is the
 * field name of the RBTree_Node structure in @a container_type.
 *
 */
#define _RBTree_Container_of(node, container_type, node_field_name) \
( \
  (container_type*) \
    ( (uintptr_t)(node) - offsetof(container_type, node_field_name) ) \
)

/**
 *  This type indicates the direction.
 */
typedef enum {
  RBT_LEFT=0,
  RBT_RIGHT=1
} RBTree_Direction;

/**
 * This type defines function pointers for user-provided comparison
 * function. The function compares two nodes in order to determine
 * the order in a red-black tree.
 */
typedef int (*RBTree_Compare_function)(
  const RBTree_Node *node1,
  const RBTree_Node *node2
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
  /**
   * Comparison function pointer. Keys to compare have to be found
   * inside to maintain generality. It has to return 1 if first node
   * has higher key than second, -1 if lower, 0 if equal.
   */
  RBTree_Compare_function compare_function;
  /** Determines whether the tree accepts duplicate keys. */
  bool is_unique;
} RBTree_Control;

/**
 *  @brief RBTree initializer for an empty rbtree with designator @a name.
 */
#define RBTREE_INITIALIZER_EMPTY(name) \
{ \
  .permanent_null = NULL, \
  .root = NULL, \
  .first[0] = NULL, \
  .first[1] = NULL, \
  .compare_function = NULL, \
  .is_unique = 0 \
}

/**
 *  @brief RBTree definition for an empty rbtree with designator @a name.
 */
#define RBTREE_DEFINE_EMPTY(name) \
RBTree_Control name = RBTREE_INITIALIZER_EMPTY(name)

/**
 *  @brief RBTree_Node initializer for an empty node with designator @a name.
 */
#define RBTREE_NODE_INITIALIZER_EMPTY(name) \
{ \
  .parent = NULL, \
  .child[0] = NULL, \
  .child[1] = NULL, \
  RBT_RED \
}

/**
 *  @brief RBTree definition for an empty rbtree with designator @a name.
 */
#define RBTREE_NODE_DEFINE_EMPTY(name) \
RBTree_Node name = RBTREE_NODE_INITIALIZER_EMPTY(name)

/**
 *  @brief Initialize a RBTree Header.
 *
 *  This routine initializes @a the_rbtree structure to manage the
 *  contiguous array of @a number_nodes nodes which starts at
 *  @a starting_address.  Each node is of @a node_size bytes.
 *
 *  @param[in] the_rbtree is the pointer to rbtree header
 *  @param[in] starting_address is the starting address of first node
 *  @param[in] number_nodes is the number of nodes in rbtree
 *  @param[in] node_size is the size of node in bytes
 */
void _RBTree_Initialize(
  RBTree_Control          *the_rbtree,
  RBTree_Compare_function  compare_function,
  void                    *starting_address,
  size_t                   number_nodes,
  size_t                   node_size,
  bool                     is_unique
);

/**
 *  @brief Obtain the min or max node of a rbtree.
 *
 *  This function removes the min or max node from @a the_rbtree and returns
 *  a pointer to that node.  If @a the_rbtree is empty, then NULL is returned.
 *  @a dir specifies whether to return the min (0) or max (1).
 *
 *  @retval This method returns a pointer to a node.  If a node was removed,
 *          then a pointer to that node is returned.  If @a the_rbtree was
 *          empty, then NULL is returned.
 *
 *  @note It disables interrupts to ensure the atomicity of the get operation.
 */
RBTree_Node *_RBTree_Get(
  RBTree_Control *the_rbtree,
  RBTree_Direction dir
);

/** @brief Find the node with given key in the tree
 *
 *  This function returns a pointer to the node in @a the_rbtree
 *  having key equal to key of  @a the_node if it exists,
 *  and NULL if not. @a the_node has to be made up before a search.
 *
 *  @note If the tree is not unique and contains duplicate keys, the set
 *        of duplicate keys acts as FIFO.
 */
RBTree_Node *_RBTree_Find_unprotected(
  const RBTree_Control *the_rbtree,
  const RBTree_Node *the_node
);

/**
 *  @brief Find the node with given key in the tree.
 *
 *  This function returns a pointer to the node with key equal to a key
 *  of @a the_node if it exists in the Red-Black Tree @a the_rbtree,
 *  and NULL if not.
 *
 *  @param[in] the_rbtree pointer to rbtree control
 *  @param[in] the_node node with the key to search for
 *  @retval This method returns pointer to control header of rbtree. *
 *          If there is no control header available (the node is not part
 *          of a tree), then NULL is returned. *
 *
 *  - INTERRUPT LATENCY:
 *    + single case
 */
RBTree_Node *_RBTree_Find(
  const RBTree_Control *the_rbtree,
  const RBTree_Node *the_node
);

/**
 *  @brief Find the control structure of the tree containing the given node.
 *
 *  This function returns a pointer called @a return_header to the
 *  control structure of the tree containing @a the_node, if it exists,
 *   and @a NULL if not.
 *
 *  @param[in] the_node is the pointer to the rbtree node.
 *
 *  -INTERRUPT LATENCY:
 *    + single case
 */
RBTree_Control *_RBTree_Find_header(
  RBTree_Node *the_node
);

/**
 *  @brief Insert @a the_node on the Red-Black Tree @a the_rbtree (unprotected).
 *
 *  This routine inserts @a the_node on the Red-Black Tree @a the_rbtree.
 *
 *  @retval 0 Successfully inserted.
 *  @retval -1 NULL @a the_node.
 *  @retval RBTree_Node* if one with equal value to @a the_node 's key exists
 *          in an unique @a the_rbtree.
 *
 *  @note It does NOT disable interrupts to ensure the atomicity
 *        of the extract operation.
 */
RBTree_Node *_RBTree_Insert_unprotected(
  RBTree_Control *the_rbtree,
  RBTree_Node *the_node
);

/**
 *  @brief Insert a node on a rbtree.
 *
 *  This routine inserts @a the_node on the tree @a the_rbtree.
 *
 *  @retval 0 Successfully inserted.
 *  @retval -1 NULL @a the_node.
 *  @retval RBTree_Node* if one with equal value to @a the_node 's key exists
 *          in an unique @a the_rbtree.
 *
 *  @note It disables interrupts to ensure the atomicity
 *  of the extract operation.
 */
RBTree_Node *_RBTree_Insert(
  RBTree_Control *the_rbtree,
  RBTree_Node *the_node
);


/**
 *  @brief Extracts (removes) @a the_node from @a the_rbtree (unprotected).
 *
 *  This routine extracts (removes) @a the_node from @a the_rbtree.
 *
 *  @note It does NOT disable interrupts to ensure the atomicity
 *        of the extract operation.
 */
void _RBTree_Extract_unprotected(
  RBTree_Control *the_rbtree,
  RBTree_Node *the_node
);

/**
 *  @brief Delete a node from the rbtree.
 *
 *  This routine deletes @a the_node from @a the_rbtree.
 *
 *  @note It disables interrupts to ensure the atomicity of the
 *  append operation.
 */
void _RBTree_Extract(
  RBTree_Control *the_rbtree,
  RBTree_Node    *the_node
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
RBTree_Node *_RBTree_Next_unprotected(
  const RBTree_Node *node,
  RBTree_Direction dir
);

/**
 * @copydoc _RBTree_Next_unprotected()
 *
 * The function disables the interrupts protect the operation.
 */
RBTree_Node *_RBTree_Next(
  const RBTree_Node *node,
  RBTree_Direction dir
);

/**
 * @brief Set off RBtree.
 *
 * This function sets the parent and child fields of the @a node to NULL
 * indicating the @a node is not part of a rbtree.
 *
 */
RTEMS_INLINE_ROUTINE void _RBTree_Set_off_rbtree(
    RBTree_Node *node
    )
{
  node->parent = node->child[RBT_LEFT] = node->child[RBT_RIGHT] = NULL;
}

/**
 * @brief Is the node off RBTree.
 *
 * This function returns true if the @a node is not on a rbtree. A @a node is
 * off rbtree if the parent and child fields are set to NULL.
 */
RTEMS_INLINE_ROUTINE bool _RBTree_Is_node_off_rbtree(
    const RBTree_Node *node
    )
{
  return (node->parent == NULL) &&
         (node->child[RBT_LEFT] == NULL) &&
         (node->child[RBT_RIGHT] == NULL);
}

/**
 * @brief Are two Nodes equal.
 *
 * This function returns true if @a left and @a right are equal,
 * and false otherwise.
 *
 * @retval true @a left and @a right are equal.
 * @retval false @a left and @a right are not equal.
 */
RTEMS_INLINE_ROUTINE bool _RBTree_Are_nodes_equal(
    const RBTree_Node *left,
    const RBTree_Node *right
    )
{
  return left == right;
}

/**
 * @brief Is the RBTree node pointer NUL.
 *
 * This function returns true if @a the_node is NULL and false otherwise.
 *
 * @retval true @a the_node is @c NULL.
 * @retval false @a the_node is not @c NULL.
 */
RTEMS_INLINE_ROUTINE bool _RBTree_Is_null_node(
    const RBTree_Node *the_node
    )
{
  return (the_node == NULL);
}

/**
 * @brief Return pointer to RBTree's root node.
 *
 * This function returns a pointer to the root node of @a the_rbtree.
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
 * @brief Return pointer to the parent of this node.
 *
 * This function returns a pointer to the parent node of @a the_node.
 */
RTEMS_INLINE_ROUTINE RBTree_Node *_RBTree_Parent(
  const RBTree_Node *the_node
)
{
  if (!the_node->parent->parent) return NULL;
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
 * @brief Does this RBTree have only one node.
 *
 * This function returns true if there is only one node on @a the_rbtree and
 * false otherwise.
 *
 * @retval true @a the_rbtree has only one node.
 * @retval false @a the_rbtree has more than one nodes.
 */
RTEMS_INLINE_ROUTINE bool _RBTree_Has_only_one_node(
    const RBTree_Control *the_rbtree
    )
{
  if(!the_rbtree) return false; /* TODO: expected behavior? */
  return (the_rbtree->root->child[RBT_LEFT] == NULL &&
          the_rbtree->root->child[RBT_RIGHT] == NULL);
}

/**
 * @brief Is this node the RBTree root.
 *
 * This function returns true if @a the_node is the root of @a the_rbtree and
 * false otherwise.
 *
 * @retval true @a the_node is the root of @a the_rbtree.
 * @retval false @a the_node is not the root of @a the_rbtree.
 */
RTEMS_INLINE_ROUTINE bool _RBTree_Is_root(
  const RBTree_Control *the_rbtree,
  const RBTree_Node    *the_node
)
{
  return (the_node == _RBTree_Root(the_rbtree));
}

/**
 * @brief Find the RBTree_Control header given a node in the tree.
 *
 * This function returns a pointer to the header of the Red Black
 * Tree containing @a the_node if it exists, and NULL if not.
 */
RTEMS_INLINE_ROUTINE RBTree_Control *_RBTree_Find_header_unprotected(
    RBTree_Node *the_node
    )
{
  if(!the_node) return NULL;
  if(!(the_node->parent)) return NULL;
  while(the_node->parent) the_node = the_node->parent;
  return (RBTree_Control*)the_node;
}

/**
 * @brief Initialize this RBTree as empty.
 *
 * This routine initializes @a the_rbtree to contain zero nodes.
 */
RTEMS_INLINE_ROUTINE void _RBTree_Initialize_empty(
    RBTree_Control          *the_rbtree,
    RBTree_Compare_function  compare_function,
    bool                     is_unique
    )
{
  the_rbtree->permanent_null   = NULL;
  the_rbtree->root             = NULL;
  the_rbtree->first[0]         = NULL;
  the_rbtree->first[1]         = NULL;
  the_rbtree->compare_function = compare_function;
  the_rbtree->is_unique        = is_unique;
}

/**
 * @brief Returns the predecessor of a node.
 *
 * @param[in] node is the node.
 *
 * @retval NULL The predecessor does not exist.  Otherwise it returns
 *         the predecessor node.
 */
RTEMS_INLINE_ROUTINE RBTree_Node *_RBTree_Predecessor_unprotected(
  const RBTree_Node *node
)
{
  return _RBTree_Next_unprotected( node, RBT_LEFT );
}

/**
 * @copydoc _RBTree_Predecessor_unprotected()
 *
 * The function disables the interrupts protect the operation.
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
RTEMS_INLINE_ROUTINE RBTree_Node *_RBTree_Successor_unprotected(
  const RBTree_Node *node
)
{
  return _RBTree_Next_unprotected( node, RBT_RIGHT );
}

/**
 * @copydoc _RBTree_Successor_unprotected()
 *
 * The function disables the interrupts protect the operation.
 */
RTEMS_INLINE_ROUTINE RBTree_Node *_RBTree_Successor(
  const RBTree_Node *node
)
{
  return _RBTree_Next( node, RBT_RIGHT );
}

/**
 * @brief Get the first node (unprotected).
 *
 * This function removes the minimum or maximum node from the_rbtree and
 * returns a pointer to that node.  It does NOT disable interrupts to ensure
 * the atomicity of the get operation.
 *
 * @param[in] the_rbtree is the rbtree to attempt to get the min node from.
 * @param[in] dir specifies whether to get minimum (0) or maximum (1)
 *
 * @return This method returns the min or max node on the rbtree, or NULL.
 *
 * @note This routine may return NULL if the RBTree is empty.
 */
RTEMS_INLINE_ROUTINE RBTree_Node *_RBTree_Get_unprotected(
    RBTree_Control *the_rbtree,
    RBTree_Direction dir
    )
{
  RBTree_Node *the_node = the_rbtree->first[dir];
  _RBTree_Extract_unprotected(the_rbtree, the_node);
  return the_node;
}

/**
 * @brief Determines whether the tree is unique.
 */
RTEMS_INLINE_ROUTINE bool _RBTree_Is_unique(
  const RBTree_Control *the_rbtree
)
{
  return( the_rbtree && the_rbtree->is_unique );
}

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
