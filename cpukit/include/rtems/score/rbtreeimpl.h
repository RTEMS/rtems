/**
 * @file
 *
 * @ingroup RTEMSScoreRBTree
 *
 * @brief This header file provides interfaces of the
 *   @ref RTEMSScoreRBTree which are only used by the implementation.
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
 * @addtogroup RTEMSScoreRBTree
 *
 * @{
 */

/**
 * @brief Appends the node to the red-black tree.
 *
 * The appended node is the new maximum node of the tree.  The caller shall
 * ensure that the appended node is indeed the maximum node with respect to the
 * tree order.
 *
 * @param[in, out] the_rbtree is the red-black tree control.
 *
 * @param the_node[out] is the node to append.
 */
void _RBTree_Append( RBTree_Control *the_rbtree, RBTree_Node *the_node );

/**
 * @brief Prepends the node to the red-black tree.
 *
 * The prepended node is the new minimum node of the tree.  The caller shall
 * ensure that the prepended node is indeed the minimum node with respect to the
 * tree order.
 *
 * @param[in, out] the_rbtree is the red-black tree control.
 *
 * @param the_node[out] is the node to prepend.
 */
void _RBTree_Prepend( RBTree_Control *the_rbtree, RBTree_Node *the_node );

/**
 * @brief Red-black tree visitor.
 *
 * @param[in] node The node.
 * @param[in] visitor_arg The visitor argument.
 *
 * @retval true Stop the iteration.
 * @retval false Continue the iteration.
 *
 * @see _RBTree_Iterate().
 */
typedef bool (*RBTree_Visitor)(
  const RBTree_Node *node,
  void *visitor_arg
);

/**
 * @brief Red-black tree iteration.
 *
 * @param rbtree The red-black tree.
 * @param visitor The visitor.
 * @param visitor_arg The visitor argument.
 */
void _RBTree_Iterate(
  const RBTree_Control *rbtree,
  RBTree_Visitor visitor,
  void *visitor_arg
);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
