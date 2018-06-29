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

#include <sys/tree.h>
#include <rtems/score/basedefs.h>
#include <rtems/score/assert.h>

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

struct RBTree_Control;

/**
 * @brief Red-black tree node.
 *
 * This is used to manage each node (element) which is placed on a red-black
 * tree.
 */
typedef struct RBTree_Node {
  RB_ENTRY(RBTree_Node) Node;
} RBTree_Node;

/**
 * @brief Red-black tree control.
 *
 * This is used to manage a red-black tree.  A red-black tree consists of a
 * tree of zero or more nodes.
 */
typedef RB_HEAD(RBTree_Control, RBTree_Node) RBTree_Control;

/**
 * @brief Initializer for an empty red-black tree with designator @a name.
 */
#define RBTREE_INITIALIZER_EMPTY( name ) \
  RB_INITIALIZER( name )

/**
 * @brief Definition for an empty red-black tree with designator @a name.
 */
#define RBTREE_DEFINE_EMPTY( name ) \
  RBTree_Control name = RBTREE_INITIALIZER_EMPTY( name )

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
  RB_COLOR( the_node, Node ) = -1;
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
  return RB_COLOR( the_node, Node ) == -1;
}

/**
 * @brief Rebalances the red-black tree after insertion of the node.
 *
 * @param[in] the_rbtree The red-black tree control.
 * @param[in] the_node The most recently inserted node.
 */
void _RBTree_Insert_color(
  RBTree_Control *the_rbtree,
  RBTree_Node    *the_node
);

/**
 * @brief Initializes a red-black tree node.
 *
 * In debug configurations, the node is set off tree.  In all other
 * configurations, this function does nothing.
 *
 * @param[in] the_node The red-black tree node to initialize.
 */
RTEMS_INLINE_ROUTINE void _RBTree_Initialize_node( RBTree_Node *the_node )
{
#if defined(RTEMS_DEBUG)
  _RBTree_Set_off_tree( the_node );
#else
  (void) the_node;
#endif
}

/**
 * @brief Adds a child node to a parent node.
 *
 * @param[in] child The child node.
 * @param[in] parent The parent node.
 * @param[in] link The child node link of the parent node.
 */
RTEMS_INLINE_ROUTINE void _RBTree_Add_child(
  RBTree_Node  *child,
  RBTree_Node  *parent,
  RBTree_Node **link
)
{
  _Assert( _RBTree_Is_node_off_tree( child ) );
  RB_SET( child, parent, Node );
  *link = child;
}

/**
 * @brief Inserts the node into the red-black tree using the specified parent
 * node and link.
 *
 * @param[in] the_rbtree The red-black tree control.
 * @param[in] the_node The node to insert.
 * @param[in] parent The parent node.
 * @param[in] link The child node link of the parent node.
 *
 * @code
 * #include <rtems/score/rbtree.h>
 *
 * typedef struct {
 *   int value;
 *   RBTree_Node Node;
 * } Some_Node;
 *
 * bool _Some_Less(
 *   const RBTree_Node *a,
 *   const RBTree_Node *b
 * )
 * {
 *   const Some_Node *aa = RTEMS_CONTAINER_OF( a, Some_Node, Node );
 *   const Some_Node *bb = RTEMS_CONTAINER_OF( b, Some_Node, Node );
 *
 *   return aa->value < bb->value;
 * }
 *
 * void _Some_Insert(
 *   RBTree_Control *the_rbtree,
 *   Some_Node      *the_node
 * )
 * {
 *   RBTree_Node **link = _RBTree_Root_reference( the_rbtree );
 *   RBTree_Node  *parent = NULL;
 *
 *   while ( *link != NULL ) {
 *     parent = *link;
 *
 *     if ( _Some_Less( &the_node->Node, parent ) ) {
 *       link = _RBTree_Left_reference( parent );
 *     } else {
 *       link = _RBTree_Right_reference( parent );
 *     }
 *   }
 *
 *   _RBTree_Insert_with_parent( the_rbtree, &the_node->Node, parent, link );
 * }
 * @endcode
 */
RTEMS_INLINE_ROUTINE void _RBTree_Insert_with_parent(
  RBTree_Control  *the_rbtree,
  RBTree_Node     *the_node,
  RBTree_Node     *parent,
  RBTree_Node    **link
)
{
  _RBTree_Add_child( the_node, parent, link );
  _RBTree_Insert_color( the_rbtree, the_node );
}

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
  return RB_ROOT( the_rbtree );
}

/**
 * @brief Returns a reference to the root pointer of the red-black tree.
 */
RTEMS_INLINE_ROUTINE RBTree_Node **_RBTree_Root_reference(
  RBTree_Control *the_rbtree
)
{
  return &RB_ROOT( the_rbtree );
}

/**
 * @brief Returns a constant reference to the root pointer of the red-black tree.
 */
RTEMS_INLINE_ROUTINE RBTree_Node * const *_RBTree_Root_const_reference(
  const RBTree_Control *the_rbtree
)
{
  return &RB_ROOT( the_rbtree );
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
  return RB_PARENT( the_node, Node );
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
  return RB_LEFT( the_node, Node );
}

/**
 * @brief Returns a reference to the left child pointer of the red-black tree
 * node.
 */
RTEMS_INLINE_ROUTINE RBTree_Node **_RBTree_Left_reference(
  RBTree_Node *the_node
)
{
  return &RB_LEFT( the_node, Node );
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
  return RB_RIGHT( the_node, Node );
}

/**
 * @brief Returns a reference to the right child pointer of the red-black tree
 * node.
 */
RTEMS_INLINE_ROUTINE RBTree_Node **_RBTree_Right_reference(
  RBTree_Node *the_node
)
{
  return &RB_RIGHT( the_node, Node );
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
  return RB_EMPTY( the_rbtree );
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
  return _RBTree_Parent( the_node ) == NULL;
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
  RB_INIT( the_rbtree );
}

/**
 * @brief Initializes this red-black tree to contain exactly the specified
 * node.
 *
 * @param[in] the_rbtree The red-black tree control.
 * @param[in] the_node The one and only node.
 */
RTEMS_INLINE_ROUTINE void _RBTree_Initialize_one(
  RBTree_Control *the_rbtree,
  RBTree_Node    *the_node
)
{
  _Assert( _RBTree_Is_node_off_tree( the_node ) );
  RB_ROOT( the_rbtree ) = the_node;
  RB_PARENT( the_node, Node ) = NULL;
  RB_LEFT( the_node, Node ) = NULL;
  RB_RIGHT( the_node, Node ) = NULL;
  RB_COLOR( the_node, Node ) = RB_BLACK;
}

/**
 * @brief Returns the minimum node of the red-black tree.
 *
 * @param[in] the_rbtree The red-black tree control.
 *
 * @retval NULL The red-black tree is empty.
 * @retval node The minimum node.
 */
RBTree_Node *_RBTree_Minimum( const RBTree_Control *the_rbtree );

/**
 * @brief Returns the maximum node of the red-black tree.
 *
 * @param[in] the_rbtree The red-black tree control.
 *
 * @retval NULL The red-black tree is empty.
 * @retval node The maximum node.
 */
RBTree_Node *_RBTree_Maximum( const RBTree_Control *the_rbtree );

/**
 * @brief Returns the predecessor of a node.
 *
 * @param[in] node is the node.
 *
 * @retval NULL The predecessor does not exist.  Otherwise it returns
 *         the predecessor node.
 */
RBTree_Node *_RBTree_Predecessor( const RBTree_Node *node );

/**
 * @brief Returns the successor of a node.
 *
 * @param[in] node is the node.
 *
 * @retval NULL The successor does not exist.  Otherwise the successor node.
 */
RBTree_Node *_RBTree_Successor( const RBTree_Node *node );

/**
 * @brief Replaces a node in the red-black tree without a rebalance.
 *
 * @param[in] the_rbtree The red-black tree control.
 * @param[in] victim The victim node.
 * @param[in] replacement The replacement node.
 */
void _RBTree_Replace_node(
  RBTree_Control *the_rbtree,
  RBTree_Node    *victim,
  RBTree_Node    *replacement
);

/**
 * @brief Inserts the node into the red-black tree.
 *
 * @param the_rbtree The red-black tree control.
 * @param the_node The node to insert.
 * @param key The key of the node to insert.  This key must be equal to the key
 *   stored in the node to insert.  The separate key parameter is provided for
 *   two reasons.  Firstly, it allows to share the less operator with
 *   _RBTree_Find_inline().  Secondly, the compiler may generate better code if
 *   the key is stored in a local variable.
 * @param less Must return true if the specified key is less than the key of
 *   the node, otherwise false.
 *
 * @retval true The inserted node is the new minimum node according to the
 *   specified less order function.
 * @retval false Otherwise.
 */
RTEMS_INLINE_ROUTINE bool _RBTree_Insert_inline(
  RBTree_Control *the_rbtree,
  RBTree_Node    *the_node,
  const void     *key,
  bool         ( *less )( const void *, const RBTree_Node * )
)
{
  RBTree_Node **link;
  RBTree_Node  *parent;
  bool          is_new_minimum;

  link = _RBTree_Root_reference( the_rbtree );
  parent = NULL;
  is_new_minimum = true;

  while ( *link != NULL ) {
    parent = *link;

    if ( ( *less )( key, parent ) ) {
      link = _RBTree_Left_reference( parent );
    } else {
      link = _RBTree_Right_reference( parent );
      is_new_minimum = false;
    }
  }

  _RBTree_Add_child( the_node, parent, link );
  _RBTree_Insert_color( the_rbtree, the_node );
  return is_new_minimum;
}

/**
 * @brief Finds an object in the red-black tree with the specified key.
 *
 * @param the_rbtree The red-black tree control.
 * @param key The key to look after.
 * @param equal Must return true if the specified key equals the key of the
 *   node, otherwise false.
 * @param less Must return true if the specified key is less than the key of
 *   the node, otherwise false.
 * @param map In case a node with the specified key is found, then this
 *   function is called to map the node to the object returned.  Usually it
 *   performs some offset operation via RTEMS_CONTAINER_OF() to map the node to
 *   its containing object.  Thus, the return type is a void pointer and not a
 *   red-black tree node.
 *
 * @retval object An object with the specified key.
 * @retval NULL No object with the specified key exists in the red-black tree.
 */
RTEMS_INLINE_ROUTINE void *_RBTree_Find_inline(
  const RBTree_Control *the_rbtree,
  const void           *key,
  bool               ( *equal )( const void *, const RBTree_Node * ),
  bool               ( *less )( const void *, const RBTree_Node * ),
  void              *( *map )( RBTree_Node * )
)
{
  RBTree_Node * const *link;
  RBTree_Node         *parent;

  link = _RBTree_Root_const_reference( the_rbtree );
  parent = NULL;

  while ( *link != NULL ) {
    parent = *link;

    if ( ( *equal )( key, parent ) ) {
      return ( *map )( parent );
    } else if ( ( *less )( key, parent ) ) {
      link = _RBTree_Left_reference( parent );
    } else {
      link = _RBTree_Right_reference( parent );
    }
  }

  return NULL;
}

/**
 * @brief Returns the container of the first node of the specified red-black
 * tree in postorder.
 *
 * Postorder traversal may be used to delete all nodes of a red-black tree.
 *
 * @param the_rbtree The red-black tree control.
 * @param offset The offset to the red-black tree node in the container structure.
 *
 * @retval NULL The red-black tree is empty.
 * @retval container The container of the first node of the specified red-black
 *   tree in postorder.
 *
 * @see _RBTree_Postorder_next().
 *
 * @code
 * #include <rtems/score/rbtree.h>
 *
 * typedef struct {
 *   int         data;
 *   RBTree_Node Node;
 * } Container_Control;
 *
 * void visit( Container_Control *the_container );
 *
 * void postorder_traversal( RBTree_Control *the_rbtree )
 * {
 *   Container_Control *the_container;
 *
 *   the_container = _RBTree_Postorder_first(
 *     the_rbtree,
 *     offsetof( Container_Control, Node )
 *   );
 *
 *   while ( the_container != NULL ) {
 *     visit( the_container );
 *
 *     the_container = _RBTree_Postorder_next(
 *       &the_container->Node,
 *       offsetof( Container_Control, Node )
 *     );
 *   }
 * }
 * @endcode
 */
void *_RBTree_Postorder_first(
  const RBTree_Control *the_rbtree,
  size_t                offset
);

/**
 * @brief Returns the container of the next node in postorder.
 *
 * @param the_node The red-black tree node.  The node must not be NULL.
 * @param offset The offset to the red-black tree node in the container structure.
 *
 * @retval NULL The node is NULL or there is no next node in postorder.
 * @retval container The container of the next node in postorder.
 *
 * @see _RBTree_Postorder_first().
 */
void *_RBTree_Postorder_next(
  const RBTree_Node *the_node,
  size_t             offset
);

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
