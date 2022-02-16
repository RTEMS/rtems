/* SPDX-License-Identifier: BSD-2-Clause */

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
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
