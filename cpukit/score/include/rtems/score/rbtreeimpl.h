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
 *  http://www.rtems.com/license/LICENSE.
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
 * @see _RBTree_Iterate_unprotected().
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
void _RBTree_Iterate_unprotected(
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
 * @brief Is this RBTree control pointer NULL.
 *
 * This function returns true if @a the_rbtree is NULL and false otherwise.
 *
 * @retval true @a the_rbtree is @c NULL.
 * @retval false @a the_rbtree is not @c NULL.
 */
RTEMS_INLINE_ROUTINE bool _RBTree_Is_null(
    const RBTree_Control *the_rbtree
    )
{
  return (the_rbtree == NULL);
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
 * @brief Return a pointer to node's grandparent.
 *
 * This function returns a pointer to the grandparent of @a the_node if it
 * exists, and NULL if not.
 */
RTEMS_INLINE_ROUTINE RBTree_Node *_RBTree_Grandparent(
  const RBTree_Node *the_node
)
{
  if(!the_node) return NULL;
  if(!(the_node->parent)) return NULL;
  if(!(the_node->parent->parent)) return NULL;
  if(!(the_node->parent->parent->parent)) return NULL;
  return(the_node->parent->parent);
}

/**
 * @brief Return a pointer to node's sibling.
 *
 * This function returns a pointer to the sibling of @a the_node if it
 * exists, and NULL if not.
 */
RTEMS_INLINE_ROUTINE RBTree_Node *_RBTree_Sibling(
  const RBTree_Node *the_node
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

/**
 * @brief Return a pointer to node's parent's sibling.
 *
 * This function returns a pointer to the sibling of the parent of
 * @a the_node if it exists, and NULL if not.
 */
RTEMS_INLINE_ROUTINE RBTree_Node *_RBTree_Parent_sibling(
  const RBTree_Node *the_node
)
{
  if(!the_node) return NULL;
  if(_RBTree_Grandparent(the_node) == NULL) return NULL;

  return _RBTree_Sibling(the_node->parent);
}

RTEMS_INLINE_ROUTINE bool _RBTree_Is_equal( int compare_result )
{
  return compare_result == 0;
}

RTEMS_INLINE_ROUTINE bool _RBTree_Is_greater(
  int compare_result
)
{
  return compare_result > 0;
}

RTEMS_INLINE_ROUTINE bool _RBTree_Is_lesser(
  int compare_result
)
{
  return compare_result < 0;
}

/**
 * @brief Rotate the_node in the direction passed as second argument.
 *
 * This routine rotates @a the_node to the direction @a dir, swapping
 * @a the_node with its child\[@a dir\].
 */
RTEMS_INLINE_ROUTINE void _RBTree_Rotate(
    RBTree_Node *the_node,
    RBTree_Direction dir
    )
{
  RBTree_Node *c;
  if (the_node == NULL) return;
  if (the_node->child[_RBTree_Opposite_direction(dir)] == NULL) return;

  c = the_node->child[_RBTree_Opposite_direction(dir)];
  the_node->child[_RBTree_Opposite_direction(dir)] = c->child[dir];

  if (c->child[dir])
    c->child[dir]->parent = the_node;

  c->child[dir] = the_node;

  the_node->parent->child[the_node != the_node->parent->child[0]] = c;

  c->parent = the_node->parent;
  the_node->parent = c;
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
