/**
 *  @file  rtems/score/rbtree.inl
 *
 *  This include file contains the bodies of the routines which are
 *  associated with Red-Black Trees and inlined.
 *
 *  @note  The routines in this file are ordered from simple
 *         to complex.  No other RBTree Handler routine is referenced
 *         unless it has already been defined.
 */

/*
 *  Copyright (c) 2010 Gedare Bloom.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _RTEMS_SCORE_RBTREE_H
# error "Never use <rtems/score/rbtree.inl> directly; include <rtems/score/rbtree.h> instead."
#endif

#ifndef _RTEMS_SCORE_RBTREE_INL
#define _RTEMS_SCORE_RBTREE_INL

/**
 *  @addtogroup ScoreRBTree 
 *  @{
 */

/** @brief Set off rbtree
 *
 *  This function sets the parent and child fields of the @a node to NULL
 *  indicating the @a node is not part of a rbtree.
 *
 */
RTEMS_INLINE_ROUTINE void _RBTree_Set_off_rbtree(
    RBTree_Node *node
    )
{
  node->parent = node->child[RBT_LEFT] = node->child[RBT_RIGHT] = NULL;
}

/** @brief Is the Node off RBTree
 *
 *  This function returns true if the @a node is not on a rbtree. A @a node is
 *  off rbtree if the parent and child fields are set to NULL.
 */
RTEMS_INLINE_ROUTINE bool _RBTree_Is_node_off_rbtree(
    const RBTree_Node *node
    )
{
  return (node->parent == NULL) && (node->child[RBT_LEFT] == NULL) && (node->child[RBT_RIGHT] == NULL);
}

/** @brief Are Two Nodes Equal
 *
 *  This function returns true if @a left and @a right are equal,
 *  and false otherwise.
 */
RTEMS_INLINE_ROUTINE bool _RBTree_Are_nodes_equal(
    const RBTree_Node *left,
    const RBTree_Node *right
    )
{
  return left == right;
}

/** @brief Is this RBTree Control Pointer Null
 *
 *  This function returns true if @a the_rbtree is NULL and false otherwise.
 */
RTEMS_INLINE_ROUTINE bool _RBTree_Is_null(
    const RBTree_Control *the_rbtree
    )
{
  return (the_rbtree == NULL);
}

/** @brief Is the RBTree Node Pointer NULL
 *
 *  This function returns true if @a the_node is NULL and false otherwise.
 */
RTEMS_INLINE_ROUTINE bool _RBTree_Is_null_node(
    const RBTree_Node *the_node
    )
{
  return (the_node == NULL);
}


/** @brief Return pointer to RBTree's root node
 *
 *  This function returns a pointer to the root node of @a the_rbtree.
 */
RTEMS_INLINE_ROUTINE RBTree_Node *_RBTree_Root(
    RBTree_Control *the_rbtree
    )
{
  return the_rbtree->root;
}

/** @brief Return pointer to RBTree's First node
 *
 *  This function returns a pointer to the first node on @a the_rbtree, 
 *  where @a dir specifies whether to return the minimum (0) or maximum (1).
 */
RTEMS_INLINE_ROUTINE RBTree_Node *_RBTree_First(
    RBTree_Control *the_rbtree,
    RBTree_Direction dir
    )
{
  return the_rbtree->first[dir];
}

/** @brief Return pointer to the parent of this node
 *
 *  This function returns a pointer to the parent node of @a the_node.
 */
RTEMS_INLINE_ROUTINE RBTree_Node *_RBTree_Parent(
    RBTree_Node *the_node
    )
{
  if (!the_node->parent->parent) return NULL;
  return the_node->parent;
}

/** @brief Return pointer to the left of this node
 *
 *  This function returns a pointer to the left node of this node.
 *
 *  @param[in] the_node is the node to be operated upon.
 *
 *  @return This method returns the left node on the rbtree.
 */
RTEMS_INLINE_ROUTINE RBTree_Node *_RBTree_Left(
    RBTree_Node *the_node
    )
{
  return the_node->child[RBT_LEFT];
}

/** @brief Return pointer to the right of this node
 *
 *  This function returns a pointer to the right node of this node.
 *
 *  @param[in] the_node is the node to be operated upon.
 *
 *  @return This method returns the right node on the rbtree.
 */
RTEMS_INLINE_ROUTINE RBTree_Node *_RBTree_Right(
    RBTree_Node *the_node
    )
{
  return the_node->child[RBT_RIGHT];
}

/** @brief Is the RBTree Empty
 *
 *  This function returns true if there are no nodes on @a the_rbtree and
 *  false otherwise.
 *
 *  @param[in] the_rbtree is the rbtree to be operated upon.
 *
 *  @return This function returns true if there are no nodes on 
 *  @a the_rbtree and false otherwise.
 */
RTEMS_INLINE_ROUTINE bool _RBTree_Is_empty(
    RBTree_Control *the_rbtree
    )
{
  return (the_rbtree->root == NULL);
}

/** @brief Is this the First Node on the RBTree
 *
 *  This function returns true if @a the_node is the first node on 
 *  @a the_rbtree and false otherwise. @a dir specifies whether first means 
 *  minimum (0) or maximum (1).
 *
 */
RTEMS_INLINE_ROUTINE bool _RBTree_Is_first(
    RBTree_Control *the_rbtree,
    const RBTree_Node *the_node,
    RBTree_Direction dir
    )
{
  return (the_node == _RBTree_First(the_rbtree, dir));
}

/** @brief Is this node red?
 *
 *  This function returns true if @a the_node is red and false otherwise.
 */
RTEMS_INLINE_ROUTINE bool _RBTree_Is_red(
    const RBTree_Node *the_node
    )
{
  return (the_node && the_node->color == RBT_RED);
}


/** @brief Does this RBTree have only One Node
 *
 *  This function returns true if there is only one node on @a the_rbtree and
 *  false otherwise.
 *
 */
RTEMS_INLINE_ROUTINE bool _RBTree_Has_only_one_node(
    const RBTree_Control *the_rbtree
    )
{
  if(!the_rbtree) return NULL; /* TODO: expected behavior? */
  return (the_rbtree->root->child[RBT_LEFT] == NULL && the_rbtree->root->child[RBT_RIGHT] == NULL);
}

/** @brief Is this Node the RBTree root
 *
 *  This function returns true if @a the_node is the root of @a the_rbtree and
 *  false otherwise.
 */
RTEMS_INLINE_ROUTINE bool _RBTree_Is_root(
    RBTree_Control *the_rbtree,
    const RBTree_Node    *the_node
    )
{
  return (the_node == _RBTree_Root(the_rbtree));
}

/** @brief Initialize this RBTree as Empty
 *
 *  This routine initializes @a the_rbtree to contain zero nodes.
 */
RTEMS_INLINE_ROUTINE void _RBTree_Initialize_empty(
    RBTree_Control *the_rbtree
    )
{
  the_rbtree->permanent_null = NULL;
  the_rbtree->root           = NULL;
  the_rbtree->first[0]       = NULL;
  the_rbtree->first[1]       = NULL;
}

/** @brief Return a pointer to node's grandparent
 *
 *  This function returns a pointer to the grandparent of @a the_node if it 
 *  exists, and NULL if not. 
 *  
 */
RTEMS_INLINE_ROUTINE RBTree_Node *_RBTree_Grandparent(
    RBTree_Node *the_node
    )
{
  if(!the_node) return NULL;
  if(!(the_node->parent)) return NULL;
  if(!(the_node->parent->parent)) return NULL;
  if(!(the_node->parent->parent->parent)) return NULL;
  return(the_node->parent->parent);
}

/** @brief Return a pointer to node's sibling
 *
 *  This function returns a pointer to the sibling of @a the_node if it 
 *  exists, and NULL if not. 
 */
RTEMS_INLINE_ROUTINE RBTree_Node *_RBTree_Sibling(
    RBTree_Node *the_node
    )
{
  if(!the_node) return NULL;
  if(!(the_node->parent)) return NULL;
  if(!(the_node->parent->parent)) return NULL;

  if(the_node == the_node->parent->child[RBT_LEFT])
    return the_node->parent->child[RBT_RIGHT];
  else
    return the_node->parent->child[RBT_LEFT];
}

/** @brief Return a pointer to node's parent's sibling
 *
 *  This function returns a pointer to the sibling of the parent of 
 *  @a the_node if it exists, and NULL if not. 
 */
RTEMS_INLINE_ROUTINE RBTree_Node *_RBTree_Parent_sibling(
    RBTree_Node *the_node
    )
{
  if(!the_node) return NULL;
  if(_RBTree_Grandparent(the_node) == NULL) return NULL;

  return _RBTree_Sibling(the_node->parent);
}

/** @brief Find the RBTree_Control header given a node in the tree
 *
 *  This function returns a pointer to the header of the Red Black 
 *  Tree containing @a the_node if it exists, and NULL if not. 
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

/** @brief Find the node with given value in the tree
 *
 *  This function returns a pointer to the node in @a the_rbtree 
 *  having value equal to @a the_value if it exists, and NULL if not. 
 */
RTEMS_INLINE_ROUTINE RBTree_Node *_RBTree_Find_unprotected(
    RBTree_Control *the_rbtree,
    unsigned int the_value
    )
{
  RBTree_Node* iter_node = the_rbtree->root;
  while (iter_node) {
    if (the_value == iter_node->value) return(iter_node);

    RBTree_Direction dir = the_value > iter_node->value;
    iter_node = iter_node->child[dir];
  } /* while(iter_node) */

  return 0;
}

/** @brief Find the nodes in-order predecessor
 *
 *  This function returns a pointer to the in-order predecessor 
 *  of @a the_node if it exists, and NULL if not. 
 */
RTEMS_INLINE_ROUTINE RBTree_Node *_RBTree_Predecessor(
    RBTree_Node *the_node
    )
{
  RBTree_Node* iter_node;
  if (!the_node) return NULL;
  iter_node = the_node->child[RBT_LEFT];
  if (!iter_node) return NULL;
  while (iter_node->child[RBT_RIGHT]) {
    iter_node = iter_node->child[RBT_RIGHT];
  } 
  return iter_node;
}

/** @brief Find the nodes in-order successor
 *
 *  This function returns a pointer to the in-order successor  
 *  of @a the_node if it exists, and NULL if not. 
 */
RTEMS_INLINE_ROUTINE RBTree_Node *_RBTree_Successor(
    RBTree_Node *the_node
    )
{
  RBTree_Node* iter_node;
  if (!the_node) return NULL;
  iter_node = the_node->child[RBT_RIGHT];
  if (!iter_node) return NULL;
  while (iter_node->child[RBT_LEFT]) {
    iter_node = iter_node->child[RBT_LEFT];
  } 
  return iter_node;
}

/** @brief Get the First Node (unprotected)
 *
 *  This function removes the minimum or maximum node from the_rbtree and 
 *  returns a pointer to that node.  It does NOT disable interrupts to ensure
 *  the atomicity of the get operation.
 *
 *  @param[in] the_rbtree is the rbtree to attempt to get the min node from.
 *  @param[in] dir specifies whether to get minimum (0) or maximum (1)
 *
 *  @return This method returns the min or max node on the rbtree, or NULL.
 *
 *  @note This routine may return NULL if the RBTree is empty.
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

/** @brief Peek at the First Node (unprotected)
 *
 *  This function returns a pointer to the first node, minimum if @a dir is 0 
 *  or maximum if @a dir is 1, from @a the_rbtree without extracting it.  
 *  It does NOT disable interrupts to ensure the atomicity of the peek.
 *
 *  @retval NULL if @a the_rbtree is empty.
 */
RTEMS_INLINE_ROUTINE RBTree_Node *_RBTree_Peek_unprotected(
    RBTree_Control *the_rbtree,
    RBTree_Direction dir
    )
{
  return(the_rbtree->first[dir]);
}

/** @brief Rotate the_node in the direction passed as second argument
 *  
 *  This routine rotates @a the_node to the direction @a dir, swapping
 *  @a the_node with its child\[@a dir\].
 */
RTEMS_INLINE_ROUTINE void _RBTree_Rotate(
    RBTree_Node *the_node,
    RBTree_Direction dir
    )
{
  RBTree_Node *c;
  if (the_node == NULL) return;
  if (the_node->child[(1-dir)] == NULL) return;
  

  c = the_node->child[(1-dir)];
  the_node->child[(1-dir)] = c->child[dir];

  if (c->child[dir])
    c->child[dir]->parent = the_node;

  c->child[dir] = the_node;

  the_node->parent->child[the_node != the_node->parent->child[0]] = c;

  c->parent = the_node->parent;
  the_node->parent = c;
}
/**@}*/

#endif
/* end of include file */
