/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreRbtreeUnitRbtree
 */

/*
 * Copyright (C) 2021 embedded brains GmbH & Co. KG
 * Copyright (C) 2010 Gedare Bloom
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

/*
 * This file is part of the RTEMS quality process and was automatically
 * generated.  If you find something that needs to be fixed or
 * worded better please post a report or patch to an RTEMS mailing list
 * or raise a bug report:
 *
 * https://www.rtems.org/bugs.html
 *
 * For information on updating and regenerating please refer to the How-To
 * section in the Software Requirements Engineering chapter of the
 * RTEMS Software Engineering manual.  The manual is provided as a part of
 * a release.  For development sources please refer to the online
 * documentation at:
 *
 * https://docs.rtems.org
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <rtems/score/rbtreeimpl.h>

#include <rtems/test.h>

/**
 * @defgroup ScoreRbtreeUnitRbtree spec:/score/rbtree/unit/rbtree
 *
 * @ingroup TestsuitesUnitNoClock0
 *
 * @brief Unit tests for the red-black tree implementation.
 *
 * The red-black trees are used by various handlers for priority queues and the
 * timers.
 *
 * This test case performs the following actions:
 *
 * - Call _RBTree_Initialize_one() and check the tree properties.
 *
 *   - Check that the tree is not emtpy.
 *
 *   - Check that the tree root is is the only node.
 *
 *   - Check that the node is not off the tree.
 *
 *   - Check that the node has no left child.
 *
 *   - Check that the node has no right child.
 *
 *   - Check that the node has no parent.
 *
 *   - Check that the node has no successor.
 *
 *   - Check that the node has no predecessor.
 *
 *   - Check that the minimum node is the node.
 *
 *   - Check that the maximum node is the node.
 *
 *   - Check that the tree is emtpy after extraction of the node.
 *
 * - Call _RBTree_Insert_inline() and check the return status for a sample set
 *   of nodes.
 *
 *   - Insert the first node.  Check that it is the new minimum node.
 *
 *   - Insert the second node.  Check that it is not the new minimum node.
 *
 *   - Insert the third node.  Check that it is the new minimum node.
 *
 * - Call _RBTree_Insert_inline() and _RBTree_Extract() for a sample set of
 *   trees.
 *
 * @{
 */

typedef struct {
  int         id;
  int         key;
  RBTree_Node Node;
} TestNode;

static TestNode node_array[ 100 ];

static int Color( const RBTree_Node *n )
{
  return RB_COLOR( n, Node );
}

static bool Less( const void *left, const RBTree_Node *right )
{
  const int       *the_left;
  const TestNode *the_right;

  the_left = left;
  the_right = RTEMS_CONTAINER_OF( right, TestNode, Node );

  return *the_left < the_right->key;
}

/*
 * recursively checks tree. if the tree is built properly it should only
 * be a depth of 7 function calls for 100 entries in the tree.
 */
static int VerifyTree( RBTree_Node *root )
{
  RBTree_Node *ln;
  RBTree_Node *rn;
  TestNode    *tn;
  TestNode    *ltn;
  TestNode    *rtn;
  int          lh;
  int          rh;

  if ( root == NULL ) {
    return 1;
  }

  ln = _RBTree_Left( root );
  rn = _RBTree_Right( root );
  tn = RTEMS_CONTAINER_OF( root, TestNode, Node );
  ltn = RTEMS_CONTAINER_OF( ln, TestNode, Node );
  rtn = RTEMS_CONTAINER_OF( rn, TestNode, Node );

  /* Consecutive red links */
  if (
    Color( root ) == RB_RED &&
    ( ( ln != NULL && Color( ln ) == RB_RED ) ||
      ( rn != NULL && Color( rn ) == RB_RED ) )
  ) {
    return -1;
  }

  lh = VerifyTree ( ln );
  rh = VerifyTree ( rn );

  if ( lh == -1 || rh == -1 ) {
    return -1;
  }

  /* Black height mismatch */
  if ( lh != rh ) {
    return -1;
  }

  /* Invalid binary search tree */
  if (
    ( ln != NULL && tn->key != ltn->key && !Less( &ltn->key, root ) ) ||
    ( rn != NULL && tn->key != rtn->key && !Less( &tn->key, rn ) )
  ) {
    return -1;
  }

  /* Only count black links */
  return Color( root ) == RB_BLACK ? lh + 1 : lh;
}

#define TN( i ) &node_array[ i ].Node

typedef struct {
  int                key;
  const RBTree_Node *parent;
  const RBTree_Node *left;
  const RBTree_Node *right;
  int                color;
} TestNodeDescription;

typedef struct {
  int                        current;
  int                        count;
  const TestNodeDescription *tree;
} VisitorContext;

static bool VisitNodes(
  const RBTree_Node *node,
  void              *visitor_arg
)
{
  VisitorContext            *ctx;
  const TestNodeDescription *td;
  const TestNode            *tn;

  ctx = visitor_arg;
  td = &ctx->tree[ ctx->current ];
  tn = RTEMS_CONTAINER_OF( node, TestNode, Node );

  T_lt_int( ctx->current, ctx->count );

  T_eq_int( td->key, tn->key );

  if ( td->parent == NULL ) {
    T_true( _RBTree_Is_root( &tn->Node ) );
  } else {
    T_eq_ptr( td->parent, _RBTree_Parent( &tn->Node ) );
  }

  T_eq_ptr( td->left, _RBTree_Left( &tn->Node ) );
  T_eq_ptr( td->right, _RBTree_Right( &tn->Node ) );
  T_eq_int( td->color, Color( &tn->Node ) );

  ++ctx->current;

  return false;
}

static const TestNodeDescription random_ops_tree_unique_1[] = {
  { 0, NULL, NULL, NULL, RB_BLACK }
};

static const TestNodeDescription random_ops_tree_multiple_1[] = {
  { 0, NULL, NULL, NULL, RB_BLACK }
};

static const TestNodeDescription random_ops_tree_unique_2[] = {
};

static const TestNodeDescription random_ops_tree_multiple_2[] = {
};

static const TestNodeDescription random_ops_tree_unique_3[] = {
  { 2, NULL, NULL, NULL, RB_BLACK }
};

static const TestNodeDescription random_ops_tree_multiple_3[] = {
  { 1, NULL, NULL, NULL, RB_BLACK }
};

static const TestNodeDescription random_ops_tree_unique_4[] = {
  { 0, TN( 3 ), NULL, NULL, RB_RED },
  { 3, NULL, TN( 0 ), NULL, RB_BLACK }
};

static const TestNodeDescription random_ops_tree_multiple_4[] = {
  { 0, NULL, NULL, TN( 3 ), RB_BLACK },
  { 1, TN( 0 ), NULL, NULL, RB_RED }
};

static const TestNodeDescription random_ops_tree_unique_5[] = {
  { 0, TN( 1 ), NULL, NULL, RB_RED },
  { 1, NULL, TN( 0 ), TN( 4 ), RB_BLACK },
  { 4, TN( 1 ), NULL, NULL, RB_RED }
};

static const TestNodeDescription random_ops_tree_multiple_5[] = {
  { 0, TN( 1 ), NULL, NULL, RB_RED },
  { 0, NULL, TN( 0 ), TN( 4 ), RB_BLACK },
  { 2, TN( 1 ), NULL, NULL, RB_RED }
};

static const TestNodeDescription random_ops_tree_unique_6[] = {
  { 0, TN( 2 ), NULL, NULL, RB_RED },
  { 2, NULL, TN( 0 ), NULL, RB_BLACK }
};

static const TestNodeDescription random_ops_tree_multiple_6[] = {
  { 0, TN( 2 ), NULL, NULL, RB_RED },
  { 1, NULL, TN( 0 ), NULL, RB_BLACK }
};

static const TestNodeDescription random_ops_tree_unique_7[] = {
  { 0, TN( 2 ), NULL, TN( 1 ), RB_BLACK },
  { 1, TN( 0 ), NULL, NULL, RB_RED },
  { 2, NULL, TN( 0 ), TN( 5 ), RB_BLACK },
  { 4, TN( 5 ), NULL, NULL, RB_RED },
  { 5, TN( 2 ), TN( 4 ), NULL, RB_BLACK }
};

static const TestNodeDescription random_ops_tree_multiple_7[] = {
  { 0, TN( 2 ), NULL, TN( 1 ), RB_BLACK },
  { 0, TN( 0 ), NULL, NULL, RB_RED },
  { 1, NULL, TN( 0 ), TN( 4 ), RB_BLACK },
  { 2, TN( 4 ), NULL, NULL, RB_RED },
  { 2, TN( 2 ), TN( 5 ), NULL, RB_BLACK }
};

static const TestNodeDescription random_ops_tree_unique_8[] = {
  { 0, TN( 1 ), NULL, NULL, RB_BLACK },
  { 1, NULL, TN( 0 ), TN( 6 ), RB_BLACK },
  { 5, TN( 6 ), NULL, NULL, RB_RED },
  { 6, TN( 1 ), TN( 5 ), NULL, RB_BLACK }
};

static const TestNodeDescription random_ops_tree_multiple_8[] = {
  { 0, TN( 5 ), NULL, TN( 0 ), RB_BLACK },
  { 0, TN( 1 ), NULL, NULL, RB_RED },
  { 2, NULL, TN( 1 ), TN( 6 ), RB_BLACK },
  { 3, TN( 5 ), NULL, NULL, RB_BLACK }
};

static const TestNodeDescription random_ops_tree_unique_9[] = {
  { 1, TN( 2 ), NULL, NULL, RB_BLACK },
  { 2, TN( 6 ), TN( 1 ), TN( 4 ), RB_RED },
  { 4, TN( 2 ), NULL, TN( 5 ), RB_BLACK },
  { 5, TN( 4 ), NULL, NULL, RB_RED },
  { 6, NULL, TN( 2 ), TN( 7 ), RB_BLACK },
  { 7, TN( 6 ), NULL, TN( 8 ), RB_BLACK },
  { 8, TN( 7 ), NULL, NULL, RB_RED }
};

static const TestNodeDescription random_ops_tree_multiple_9[] = {
  { 0, TN( 2 ), NULL, NULL, RB_BLACK },
  { 1, TN( 6 ), TN( 1 ), TN( 4 ), RB_RED },
  { 2, TN( 2 ), NULL, TN( 5 ), RB_BLACK },
  { 2, TN( 4 ), NULL, NULL, RB_RED },
  { 3, NULL, TN( 2 ), TN( 7 ), RB_BLACK },
  { 3, TN( 6 ), NULL, TN( 8 ), RB_BLACK },
  { 4, TN( 7 ), NULL, NULL, RB_RED }
};

static const TestNodeDescription random_ops_tree_unique_10[] = {
  { 0, TN( 2 ), NULL, NULL, RB_BLACK },
  { 2, TN( 6 ), TN( 0 ), TN( 4 ), RB_RED },
  { 3, TN( 4 ), NULL, NULL, RB_RED },
  { 4, TN( 2 ), TN( 3 ), NULL, RB_BLACK },
  { 6, NULL, TN( 2 ), TN( 8 ), RB_BLACK },
  { 8, TN( 6 ), NULL, NULL, RB_BLACK }
};

static const TestNodeDescription random_ops_tree_multiple_10[] = {
  { 0, TN( 2 ), NULL, NULL, RB_BLACK },
  { 1, TN( 6 ), TN( 0 ), TN( 4 ), RB_RED },
  { 1, TN( 4 ), NULL, NULL, RB_RED },
  { 2, TN( 2 ), TN( 3 ), NULL, RB_BLACK },
  { 3, NULL, TN( 2 ), TN( 8 ), RB_BLACK },
  { 4, TN( 6 ), NULL, NULL, RB_BLACK }
};

static const TestNodeDescription random_ops_tree_unique_11[] = {
  { 2, TN( 6 ), NULL, NULL, RB_BLACK },
  { 6, NULL, TN( 2 ), TN( 8 ), RB_BLACK },
  { 7, TN( 8 ), NULL, NULL, RB_RED },
  { 8, TN( 6 ), TN( 7 ), TN( 9 ), RB_BLACK },
  { 9, TN( 8 ), NULL, NULL, RB_RED }
};

static const TestNodeDescription random_ops_tree_multiple_11[] = {
  { 1, TN( 6 ), NULL, NULL, RB_BLACK },
  { 3, NULL, TN( 2 ), TN( 8 ), RB_BLACK },
  { 3, TN( 8 ), NULL, NULL, RB_RED },
  { 4, TN( 6 ), TN( 7 ), TN( 9 ), RB_BLACK },
  { 4, TN( 8 ), NULL, NULL, RB_RED }
};

static const TestNodeDescription random_ops_tree_unique_12[] = {
  { 0, TN( 1 ), NULL, NULL, RB_RED },
  { 1, TN( 3 ), TN( 0 ), TN( 2 ), RB_BLACK },
  { 2, TN( 1 ), NULL, NULL, RB_RED },
  { 3, TN( 5 ), TN( 1 ), TN( 4 ), RB_RED },
  { 4, TN( 3 ), NULL, NULL, RB_BLACK },
  { 5, NULL, TN( 3 ), TN( 9 ), RB_BLACK },
  { 9, TN( 5 ), NULL, TN( 11 ), RB_BLACK },
  { 11, TN( 9 ), NULL, NULL, RB_RED }
};

static const TestNodeDescription random_ops_tree_multiple_12[] = {
  { 0, TN( 1 ), NULL, NULL, RB_BLACK },
  { 0, TN( 5 ), TN( 0 ), TN( 3 ), RB_RED },
  { 1, TN( 1 ), NULL, TN( 2 ), RB_BLACK },
  { 1, TN( 3 ), NULL, NULL, RB_RED },
  { 2, NULL, TN( 1 ), TN( 9 ), RB_BLACK },
  { 2, TN( 9 ), NULL, NULL, RB_BLACK },
  { 4, TN( 5 ), TN( 4 ), TN( 11 ), RB_RED },
  { 5, TN( 9 ), NULL, NULL, RB_BLACK }
};

static const TestNodeDescription random_ops_tree_unique_13[] = {
  { 0, TN( 1 ), NULL, NULL, RB_RED },
  { 1, TN( 3 ), TN( 0 ), NULL, RB_BLACK },
  { 3, TN( 8 ), TN( 1 ), TN( 5 ), RB_RED },
  { 4, TN( 5 ), NULL, NULL, RB_RED },
  { 5, TN( 3 ), TN( 4 ), TN( 6 ), RB_BLACK },
  { 6, TN( 5 ), NULL, NULL, RB_RED },
  { 8, NULL, TN( 3 ), TN( 11 ), RB_BLACK },
  { 10, TN( 11 ), NULL, NULL, RB_RED },
  { 11, TN( 8 ), TN( 10 ), NULL, RB_BLACK }
};

static const TestNodeDescription random_ops_tree_multiple_13[] = {
  { 0, TN( 0 ), NULL, NULL, RB_RED },
  { 0, TN( 3 ), TN( 1 ), NULL, RB_BLACK },
  { 1, TN( 6 ), TN( 0 ), TN( 4 ), RB_RED },
  { 2, TN( 3 ), NULL, TN( 5 ), RB_BLACK },
  { 2, TN( 4 ), NULL, NULL, RB_RED },
  { 3, NULL, TN( 3 ), TN( 11 ), RB_BLACK },
  { 4, TN( 11 ), NULL, NULL, RB_RED },
  { 5, TN( 6 ), TN( 8 ), TN( 10 ), RB_BLACK },
  { 5, TN( 11 ), NULL, NULL, RB_RED }
};

static const TestNodeDescription random_ops_tree_unique_14[] = {
  { 3, TN( 5 ), NULL, NULL, RB_RED },
  { 5, TN( 6 ), TN( 3 ), NULL, RB_BLACK },
  { 6, NULL, TN( 5 ), TN( 12 ), RB_BLACK },
  { 8, TN( 12 ), NULL, NULL, RB_BLACK },
  { 12, TN( 6 ), TN( 8 ), TN( 13 ), RB_RED },
  { 13, TN( 12 ), NULL, NULL, RB_BLACK }
};

static const TestNodeDescription random_ops_tree_multiple_14[] = {
  { 1, TN( 5 ), NULL, NULL, RB_RED },
  { 2, TN( 6 ), TN( 3 ), NULL, RB_BLACK },
  { 3, NULL, TN( 5 ), TN( 13 ), RB_BLACK },
  { 4, TN( 13 ), NULL, NULL, RB_BLACK },
  { 6, TN( 6 ), TN( 8 ), TN( 12 ), RB_RED },
  { 6, TN( 13 ), NULL, NULL, RB_BLACK }
};

static const TestNodeDescription random_ops_tree_unique_15[] = {
  { 0, TN( 2 ), NULL, NULL, RB_RED },
  { 2, TN( 8 ), TN( 0 ), TN( 7 ), RB_BLACK },
  { 7, TN( 2 ), NULL, NULL, RB_RED },
  { 8, NULL, TN( 2 ), TN( 12 ), RB_BLACK },
  { 9, TN( 12 ), NULL, TN( 10 ), RB_BLACK },
  { 10, TN( 9 ), NULL, NULL, RB_RED },
  { 12, TN( 8 ), TN( 9 ), TN( 13 ), RB_RED },
  { 13, TN( 12 ), NULL, TN( 14 ), RB_BLACK },
  { 14, TN( 13 ), NULL, NULL, RB_RED }
};

static const TestNodeDescription random_ops_tree_multiple_15[] = {
  { 0, TN( 2 ), NULL, NULL, RB_RED },
  { 1, TN( 9 ), TN( 0 ), TN( 7 ), RB_BLACK },
  { 3, TN( 2 ), NULL, NULL, RB_RED },
  { 4, NULL, TN( 2 ), TN( 10 ), RB_BLACK },
  { 4, TN( 10 ), NULL, NULL, RB_BLACK },
  { 5, TN( 9 ), TN( 8 ), TN( 12 ), RB_RED },
  { 6, TN( 12 ), NULL, NULL, RB_RED },
  { 6, TN( 10 ), TN( 13 ), TN( 14 ), RB_BLACK },
  { 7, TN( 12 ), NULL, NULL, RB_RED }
};

static const TestNodeDescription random_ops_tree_unique_16[] = {
  { 0, TN( 5 ), NULL, TN( 3 ), RB_BLACK },
  { 3, TN( 0 ), NULL, NULL, RB_RED },
  { 5, TN( 10 ), TN( 0 ), TN( 7 ), RB_RED },
  { 7, TN( 5 ), NULL, NULL, RB_BLACK },
  { 10, NULL, TN( 5 ), TN( 12 ), RB_BLACK },
  { 12, TN( 10 ), NULL, NULL, RB_BLACK }
};

static const TestNodeDescription random_ops_tree_multiple_16[] = {
  { 0, TN( 5 ), NULL, TN( 3 ), RB_BLACK },
  { 1, TN( 0 ), NULL, NULL, RB_RED },
  { 2, TN( 10 ), TN( 0 ), TN( 7 ), RB_RED },
  { 3, TN( 5 ), NULL, NULL, RB_BLACK },
  { 5, NULL, TN( 5 ), TN( 12 ), RB_BLACK },
  { 6, TN( 10 ), NULL, NULL, RB_BLACK }
};

static const TestNodeDescription random_ops_tree_unique_17[] = {
  { 0, TN( 1 ), NULL, NULL, RB_RED },
  { 1, TN( 3 ), TN( 0 ), NULL, RB_BLACK },
  { 3, TN( 7 ), TN( 1 ), TN( 5 ), RB_RED },
  { 4, TN( 5 ), NULL, NULL, RB_RED },
  { 5, TN( 3 ), TN( 4 ), NULL, RB_BLACK },
  { 7, NULL, TN( 3 ), TN( 9 ), RB_BLACK },
  { 8, TN( 9 ), NULL, NULL, RB_BLACK },
  { 9, TN( 7 ), TN( 8 ), TN( 16 ), RB_RED },
  { 16, TN( 9 ), NULL, NULL, RB_BLACK }
};

static const TestNodeDescription random_ops_tree_multiple_17[] = {
  { 0, TN( 0 ), NULL, NULL, RB_RED },
  { 0, TN( 3 ), TN( 1 ), NULL, RB_BLACK },
  { 1, TN( 7 ), TN( 0 ), TN( 5 ), RB_RED },
  { 2, TN( 3 ), NULL, TN( 4 ), RB_BLACK },
  { 2, TN( 5 ), NULL, NULL, RB_RED },
  { 3, NULL, TN( 3 ), TN( 8 ), RB_BLACK },
  { 4, TN( 8 ), NULL, NULL, RB_BLACK },
  { 4, TN( 7 ), TN( 9 ), TN( 16 ), RB_RED },
  { 8, TN( 8 ), NULL, NULL, RB_BLACK }
};

static const TestNodeDescription random_ops_tree_unique_18[] = {
  { 0, TN( 2 ), NULL, TN( 1 ), RB_BLACK },
  { 1, TN( 0 ), NULL, NULL, RB_RED },
  { 2, TN( 4 ), TN( 0 ), TN( 3 ), RB_BLACK },
  { 3, TN( 2 ), NULL, NULL, RB_BLACK },
  { 4, NULL, TN( 2 ), TN( 12 ), RB_BLACK },
  { 5, TN( 6 ), NULL, NULL, RB_RED },
  { 6, TN( 8 ), TN( 5 ), TN( 7 ), RB_BLACK },
  { 7, TN( 6 ), NULL, NULL, RB_RED },
  { 8, TN( 12 ), TN( 6 ), TN( 10 ), RB_RED },
  { 9, TN( 10 ), NULL, NULL, RB_RED },
  { 10, TN( 8 ), TN( 9 ), NULL, RB_BLACK },
  { 12, TN( 4 ), TN( 8 ), TN( 17 ), RB_BLACK },
  { 14, TN( 17 ), NULL, NULL, RB_RED },
  { 17, TN( 12 ), TN( 14 ), NULL, RB_BLACK }
};

static const TestNodeDescription random_ops_tree_multiple_18[] = {
  { 0, TN( 3 ), NULL, TN( 1 ), RB_BLACK },
  { 0, TN( 0 ), NULL, NULL, RB_RED },
  { 1, TN( 4 ), TN( 0 ), TN( 2 ), RB_BLACK },
  { 1, TN( 3 ), NULL, NULL, RB_BLACK },
  { 2, NULL, TN( 3 ), TN( 12 ), RB_BLACK },
  { 2, TN( 6 ), NULL, NULL, RB_RED },
  { 3, TN( 8 ), TN( 5 ), TN( 7 ), RB_BLACK },
  { 3, TN( 6 ), NULL, NULL, RB_RED },
  { 4, TN( 12 ), TN( 6 ), TN( 10 ), RB_RED },
  { 4, TN( 10 ), NULL, NULL, RB_RED },
  { 5, TN( 8 ), TN( 9 ), NULL, RB_BLACK },
  { 6, TN( 4 ), TN( 8 ), TN( 14 ), RB_BLACK },
  { 7, TN( 12 ), NULL, TN( 17 ), RB_BLACK },
  { 8, TN( 14 ), NULL, NULL, RB_RED }
};

static const TestNodeDescription random_ops_tree_unique_19[] = {
  { 1, TN( 2 ), NULL, NULL, RB_RED },
  { 2, TN( 6 ), TN( 1 ), NULL, RB_BLACK },
  { 6, TN( 11 ), TN( 2 ), TN( 8 ), RB_BLACK },
  { 8, TN( 6 ), NULL, TN( 9 ), RB_BLACK },
  { 9, TN( 8 ), NULL, NULL, RB_RED },
  { 11, NULL, TN( 6 ), TN( 14 ), RB_BLACK },
  { 12, TN( 14 ), NULL, NULL, RB_BLACK },
  { 14, TN( 11 ), TN( 12 ), TN( 16 ), RB_BLACK },
  { 16, TN( 14 ), NULL, NULL, RB_BLACK }
};

static const TestNodeDescription random_ops_tree_multiple_19[] = {
  { 0, TN( 2 ), NULL, NULL, RB_RED },
  { 1, TN( 6 ), TN( 1 ), NULL, RB_BLACK },
  { 3, TN( 11 ), TN( 2 ), TN( 9 ), RB_BLACK },
  { 4, TN( 6 ), NULL, TN( 8 ), RB_BLACK },
  { 4, TN( 9 ), NULL, NULL, RB_RED },
  { 5, NULL, TN( 6 ), TN( 14 ), RB_BLACK },
  { 6, TN( 14 ), NULL, NULL, RB_BLACK },
  { 7, TN( 11 ), TN( 12 ), TN( 16 ), RB_BLACK },
  { 8, TN( 14 ), NULL, NULL, RB_BLACK }
};

static const TestNodeDescription random_ops_tree_unique_20[] = {
  { 0, TN( 3 ), NULL, TN( 1 ), RB_BLACK },
  { 1, TN( 0 ), NULL, NULL, RB_RED },
  { 3, TN( 9 ), TN( 0 ), TN( 7 ), RB_BLACK },
  { 4, TN( 7 ), NULL, NULL, RB_RED },
  { 7, TN( 3 ), TN( 4 ), NULL, RB_BLACK },
  { 9, NULL, TN( 3 ), TN( 12 ), RB_BLACK },
  { 10, TN( 12 ), NULL, NULL, RB_BLACK },
  { 12, TN( 9 ), TN( 10 ), TN( 17 ), RB_BLACK },
  { 14, TN( 17 ), NULL, NULL, RB_BLACK },
  { 17, TN( 12 ), TN( 14 ), TN( 18 ), RB_RED },
  { 18, TN( 17 ), NULL, TN( 19 ), RB_BLACK },
  { 19, TN( 18 ), NULL, NULL, RB_RED }
};

static const TestNodeDescription random_ops_tree_multiple_20[] = {
  { 0, TN( 3 ), NULL, TN( 1 ), RB_BLACK },
  { 0, TN( 0 ), NULL, NULL, RB_RED },
  { 1, TN( 9 ), TN( 0 ), TN( 7 ), RB_BLACK },
  { 2, TN( 7 ), NULL, NULL, RB_RED },
  { 3, TN( 3 ), TN( 4 ), NULL, RB_BLACK },
  { 4, NULL, TN( 3 ), TN( 14 ), RB_BLACK },
  { 5, TN( 14 ), NULL, TN( 12 ), RB_BLACK },
  { 6, TN( 10 ), NULL, NULL, RB_RED },
  { 7, TN( 9 ), TN( 10 ), TN( 18 ), RB_BLACK },
  { 8, TN( 18 ), NULL, NULL, RB_RED },
  { 9, TN( 14 ), TN( 17 ), TN( 19 ), RB_BLACK },
  { 9, TN( 18 ), NULL, NULL, RB_RED }
};

static const TestNodeDescription random_ops_tree_unique_21[] = {
  { 0, TN( 3 ), NULL, TN( 1 ), RB_BLACK },
  { 1, TN( 0 ), NULL, NULL, RB_RED },
  { 3, TN( 11 ), TN( 0 ), TN( 5 ), RB_BLACK },
  { 4, TN( 5 ), NULL, NULL, RB_BLACK },
  { 5, TN( 3 ), TN( 4 ), TN( 8 ), RB_RED },
  { 8, TN( 5 ), NULL, NULL, RB_BLACK },
  { 11, NULL, TN( 3 ), TN( 15 ), RB_BLACK },
  { 13, TN( 15 ), NULL, NULL, RB_BLACK },
  { 15, TN( 11 ), TN( 13 ), TN( 17 ), RB_BLACK },
  { 16, TN( 17 ), NULL, NULL, RB_RED },
  { 17, TN( 15 ), TN( 16 ), NULL, RB_BLACK }
};

static const TestNodeDescription random_ops_tree_multiple_21[] = {
  { 0, TN( 3 ), NULL, TN( 1 ), RB_BLACK },
  { 0, TN( 0 ), NULL, NULL, RB_RED },
  { 1, TN( 8 ), TN( 0 ), TN( 4 ), RB_BLACK },
  { 2, TN( 3 ), NULL, TN( 5 ), RB_BLACK },
  { 2, TN( 4 ), NULL, NULL, RB_RED },
  { 4, NULL, TN( 3 ), TN( 13 ), RB_BLACK },
  { 5, TN( 13 ), NULL, NULL, RB_BLACK },
  { 6, TN( 8 ), TN( 11 ), TN( 17 ), RB_BLACK },
  { 7, TN( 17 ), NULL, NULL, RB_BLACK },
  { 8, TN( 13 ), TN( 15 ), TN( 16 ), RB_RED },
  { 8, TN( 17 ), NULL, NULL, RB_BLACK }
};

static const TestNodeDescription random_ops_tree_unique_22[] = {
  { 1, TN( 3 ), NULL, TN( 2 ), RB_BLACK },
  { 2, TN( 1 ), NULL, NULL, RB_RED },
  { 3, TN( 8 ), TN( 1 ), TN( 7 ), RB_BLACK },
  { 4, TN( 7 ), NULL, NULL, RB_RED },
  { 7, TN( 3 ), TN( 4 ), NULL, RB_BLACK },
  { 8, NULL, TN( 3 ), TN( 14 ), RB_BLACK },
  { 10, TN( 11 ), NULL, NULL, RB_RED },
  { 11, TN( 14 ), TN( 10 ), NULL, RB_BLACK },
  { 14, TN( 8 ), TN( 11 ), TN( 18 ), RB_BLACK },
  { 15, TN( 18 ), NULL, NULL, RB_BLACK },
  { 18, TN( 14 ), TN( 15 ), TN( 21 ), RB_RED },
  { 21, TN( 18 ), NULL, NULL, RB_BLACK }
};

static const TestNodeDescription random_ops_tree_multiple_22[] = {
  { 0, TN( 3 ), NULL, NULL, RB_BLACK },
  { 1, TN( 8 ), TN( 1 ), TN( 4 ), RB_BLACK },
  { 1, TN( 4 ), NULL, NULL, RB_BLACK },
  { 2, TN( 3 ), TN( 2 ), TN( 7 ), RB_RED },
  { 3, TN( 4 ), NULL, NULL, RB_BLACK },
  { 4, NULL, TN( 3 ), TN( 14 ), RB_BLACK },
  { 5, TN( 14 ), NULL, TN( 10 ), RB_BLACK },
  { 5, TN( 11 ), NULL, NULL, RB_RED },
  { 7, TN( 8 ), TN( 11 ), TN( 18 ), RB_BLACK },
  { 7, TN( 18 ), NULL, NULL, RB_BLACK },
  { 9, TN( 14 ), TN( 15 ), TN( 21 ), RB_RED },
  { 10, TN( 18 ), NULL, NULL, RB_BLACK }
};

static const TestNodeDescription random_ops_tree_unique_23[] = {
  { 0, TN( 2 ), NULL, NULL, RB_RED },
  { 2, TN( 8 ), TN( 0 ), TN( 7 ), RB_BLACK },
  { 7, TN( 2 ), NULL, NULL, RB_RED },
  { 8, TN( 12 ), TN( 2 ), TN( 11 ), RB_BLACK },
  { 11, TN( 8 ), NULL, NULL, RB_BLACK },
  { 12, NULL, TN( 8 ), TN( 17 ), RB_BLACK },
  { 13, TN( 15 ), NULL, TN( 14 ), RB_BLACK },
  { 14, TN( 13 ), NULL, NULL, RB_RED },
  { 15, TN( 17 ), TN( 13 ), TN( 16 ), RB_RED },
  { 16, TN( 15 ), NULL, NULL, RB_BLACK },
  { 17, TN( 12 ), TN( 15 ), TN( 20 ), RB_BLACK },
  { 20, TN( 17 ), NULL, TN( 21 ), RB_BLACK },
  { 21, TN( 20 ), NULL, NULL, RB_RED }
};

static const TestNodeDescription random_ops_tree_multiple_23[] = {
  { 0, TN( 2 ), NULL, NULL, RB_RED },
  { 1, TN( 8 ), TN( 0 ), TN( 7 ), RB_BLACK },
  { 3, TN( 2 ), NULL, NULL, RB_RED },
  { 4, TN( 12 ), TN( 2 ), TN( 11 ), RB_BLACK },
  { 5, TN( 8 ), NULL, NULL, RB_BLACK },
  { 6, NULL, TN( 8 ), TN( 17 ), RB_BLACK },
  { 6, TN( 15 ), NULL, NULL, RB_BLACK },
  { 7, TN( 17 ), TN( 13 ), TN( 16 ), RB_RED },
  { 7, TN( 16 ), NULL, NULL, RB_RED },
  { 8, TN( 15 ), TN( 14 ), NULL, RB_BLACK },
  { 8, TN( 12 ), TN( 15 ), TN( 20 ), RB_BLACK },
  { 10, TN( 17 ), NULL, TN( 21 ), RB_BLACK },
  { 10, TN( 20 ), NULL, NULL, RB_RED }
};

static const TestNodeDescription random_ops_tree_unique_24[] = {
  { 4, TN( 6 ), NULL, TN( 5 ), RB_BLACK },
  { 5, TN( 4 ), NULL, NULL, RB_RED },
  { 6, TN( 14 ), TN( 4 ), TN( 10 ), RB_BLACK },
  { 8, TN( 10 ), NULL, NULL, RB_RED },
  { 10, TN( 6 ), TN( 8 ), NULL, RB_BLACK },
  { 14, NULL, TN( 6 ), TN( 20 ), RB_BLACK },
  { 15, TN( 16 ), NULL, NULL, RB_RED },
  { 16, TN( 20 ), TN( 15 ), NULL, RB_BLACK },
  { 20, TN( 14 ), TN( 16 ), TN( 22 ), RB_BLACK },
  { 22, TN( 20 ), NULL, NULL, RB_BLACK }
};

static const TestNodeDescription random_ops_tree_multiple_24[] = {
  { 2, TN( 6 ), NULL, TN( 5 ), RB_BLACK },
  { 2, TN( 4 ), NULL, NULL, RB_RED },
  { 3, TN( 14 ), TN( 4 ), TN( 10 ), RB_BLACK },
  { 4, TN( 10 ), NULL, NULL, RB_RED },
  { 5, TN( 6 ), TN( 8 ), NULL, RB_BLACK },
  { 7, NULL, TN( 6 ), TN( 20 ), RB_BLACK },
  { 7, TN( 16 ), NULL, NULL, RB_RED },
  { 8, TN( 20 ), TN( 15 ), NULL, RB_BLACK },
  { 10, TN( 14 ), TN( 16 ), TN( 22 ), RB_BLACK },
  { 11, TN( 20 ), NULL, NULL, RB_BLACK }
};

static const TestNodeDescription random_ops_tree_unique_25[] = {
  { 0, TN( 1 ), NULL, NULL, RB_RED },
  { 1, TN( 3 ), TN( 0 ), NULL, RB_BLACK },
  { 3, TN( 13 ), TN( 1 ), TN( 5 ), RB_BLACK },
  { 4, TN( 5 ), NULL, NULL, RB_BLACK },
  { 5, TN( 3 ), TN( 4 ), TN( 6 ), RB_RED },
  { 6, TN( 5 ), NULL, TN( 9 ), RB_BLACK },
  { 9, TN( 6 ), NULL, NULL, RB_RED },
  { 13, NULL, TN( 3 ), TN( 19 ), RB_BLACK },
  { 14, TN( 15 ), NULL, NULL, RB_RED },
  { 15, TN( 16 ), TN( 14 ), NULL, RB_BLACK },
  { 16, TN( 19 ), TN( 15 ), TN( 17 ), RB_RED },
  { 17, TN( 16 ), NULL, NULL, RB_BLACK },
  { 19, TN( 13 ), TN( 16 ), TN( 23 ), RB_BLACK },
  { 23, TN( 19 ), NULL, TN( 24 ), RB_BLACK },
  { 24, TN( 23 ), NULL, NULL, RB_RED }
};

static const TestNodeDescription random_ops_tree_multiple_25[] = {
  { 0, TN( 3 ), NULL, TN( 1 ), RB_BLACK },
  { 0, TN( 0 ), NULL, NULL, RB_RED },
  { 1, TN( 13 ), TN( 0 ), TN( 4 ), RB_BLACK },
  { 2, TN( 4 ), NULL, NULL, RB_BLACK },
  { 2, TN( 3 ), TN( 5 ), TN( 6 ), RB_RED },
  { 3, TN( 4 ), NULL, TN( 9 ), RB_BLACK },
  { 4, TN( 6 ), NULL, NULL, RB_RED },
  { 6, NULL, TN( 3 ), TN( 19 ), RB_BLACK },
  { 7, TN( 17 ), NULL, TN( 14 ), RB_BLACK },
  { 7, TN( 15 ), NULL, NULL, RB_RED },
  { 8, TN( 19 ), TN( 15 ), TN( 16 ), RB_RED },
  { 8, TN( 17 ), NULL, NULL, RB_BLACK },
  { 9, TN( 13 ), TN( 17 ), TN( 23 ), RB_BLACK },
  { 11, TN( 19 ), NULL, TN( 24 ), RB_BLACK },
  { 12, TN( 23 ), NULL, NULL, RB_RED }
};

static const TestNodeDescription random_ops_tree_unique_26[] = {
  { 0, TN( 1 ), NULL, NULL, RB_RED },
  { 1, TN( 3 ), TN( 0 ), NULL, RB_BLACK },
  { 3, TN( 11 ), TN( 1 ), TN( 9 ), RB_BLACK },
  { 6, TN( 9 ), NULL, NULL, RB_RED },
  { 9, TN( 3 ), TN( 6 ), TN( 10 ), RB_BLACK },
  { 10, TN( 9 ), NULL, NULL, RB_RED },
  { 11, NULL, TN( 3 ), TN( 14 ), RB_BLACK },
  { 12, TN( 14 ), NULL, TN( 13 ), RB_BLACK },
  { 13, TN( 12 ), NULL, NULL, RB_RED },
  { 14, TN( 11 ), TN( 12 ), TN( 20 ), RB_BLACK },
  { 18, TN( 20 ), NULL, NULL, RB_BLACK },
  { 20, TN( 14 ), TN( 18 ), TN( 23 ), RB_RED },
  { 21, TN( 23 ), NULL, NULL, RB_RED },
  { 23, TN( 20 ), TN( 21 ), NULL, RB_BLACK }
};

static const TestNodeDescription random_ops_tree_multiple_26[] = {
  { 0, TN( 3 ), NULL, TN( 0 ), RB_BLACK },
  { 0, TN( 1 ), NULL, NULL, RB_RED },
  { 1, TN( 9 ), TN( 1 ), TN( 6 ), RB_BLACK },
  { 3, TN( 3 ), NULL, NULL, RB_BLACK },
  { 4, NULL, TN( 3 ), TN( 14 ), RB_BLACK },
  { 5, TN( 12 ), NULL, TN( 10 ), RB_BLACK },
  { 5, TN( 11 ), NULL, NULL, RB_RED },
  { 6, TN( 14 ), TN( 11 ), TN( 13 ), RB_RED },
  { 6, TN( 12 ), NULL, NULL, RB_BLACK },
  { 7, TN( 9 ), TN( 12 ), TN( 20 ), RB_BLACK },
  { 9, TN( 20 ), NULL, NULL, RB_BLACK },
  { 10, TN( 14 ), TN( 18 ), TN( 23 ), RB_RED },
  { 10, TN( 23 ), NULL, NULL, RB_RED },
  { 11, TN( 20 ), TN( 21 ), NULL, RB_BLACK }
};

static const TestNodeDescription random_ops_tree_unique_27[] = {
  { 3, TN( 8 ), NULL, NULL, RB_BLACK },
  { 8, TN( 19 ), TN( 3 ), TN( 17 ), RB_BLACK },
  { 12, TN( 17 ), NULL, NULL, RB_RED },
  { 17, TN( 8 ), TN( 12 ), NULL, RB_BLACK },
  { 19, NULL, TN( 8 ), TN( 24 ), RB_BLACK },
  { 20, TN( 21 ), NULL, NULL, RB_RED },
  { 21, TN( 24 ), TN( 20 ), TN( 23 ), RB_BLACK },
  { 23, TN( 21 ), NULL, NULL, RB_RED },
  { 24, TN( 19 ), TN( 21 ), TN( 25 ), RB_BLACK },
  { 25, TN( 24 ), NULL, TN( 26 ), RB_BLACK },
  { 26, TN( 25 ), NULL, NULL, RB_RED }
};

static const TestNodeDescription random_ops_tree_multiple_27[] = {
  { 1, TN( 8 ), NULL, NULL, RB_BLACK },
  { 4, TN( 19 ), TN( 3 ), TN( 17 ), RB_BLACK },
  { 6, TN( 17 ), NULL, NULL, RB_RED },
  { 8, TN( 8 ), TN( 12 ), NULL, RB_BLACK },
  { 9, NULL, TN( 8 ), TN( 25 ), RB_BLACK },
  { 10, TN( 21 ), NULL, NULL, RB_RED },
  { 10, TN( 25 ), TN( 20 ), TN( 23 ), RB_BLACK },
  { 11, TN( 21 ), NULL, NULL, RB_RED },
  { 12, TN( 19 ), TN( 21 ), TN( 24 ), RB_BLACK },
  { 12, TN( 25 ), NULL, TN( 26 ), RB_BLACK },
  { 13, TN( 24 ), NULL, NULL, RB_RED }
};

static const TestNodeDescription random_ops_tree_unique_28[] = {
  { 0, TN( 5 ), NULL, NULL, RB_BLACK },
  { 5, TN( 13 ), TN( 0 ), TN( 7 ), RB_RED },
  { 7, TN( 5 ), NULL, NULL, RB_BLACK },
  { 13, NULL, TN( 5 ), TN( 17 ), RB_BLACK },
  { 15, TN( 17 ), NULL, NULL, RB_BLACK },
  { 17, TN( 13 ), TN( 15 ), TN( 26 ), RB_RED },
  { 21, TN( 26 ), NULL, NULL, RB_RED },
  { 26, TN( 17 ), TN( 21 ), NULL, RB_BLACK }
};

static const TestNodeDescription random_ops_tree_multiple_28[] = {
  { 0, TN( 5 ), NULL, NULL, RB_BLACK },
  { 2, TN( 13 ), TN( 0 ), TN( 7 ), RB_RED },
  { 3, TN( 5 ), NULL, NULL, RB_BLACK },
  { 6, NULL, TN( 5 ), TN( 17 ), RB_BLACK },
  { 7, TN( 17 ), NULL, NULL, RB_BLACK },
  { 8, TN( 13 ), TN( 15 ), TN( 26 ), RB_RED },
  { 10, TN( 26 ), NULL, NULL, RB_RED },
  { 13, TN( 17 ), TN( 21 ), NULL, RB_BLACK }
};

static const TestNodeDescription random_ops_tree_unique_29[] = {
  { 0, TN( 3 ), NULL, TN( 1 ), RB_BLACK },
  { 1, TN( 0 ), NULL, NULL, RB_RED },
  { 3, TN( 12 ), TN( 0 ), TN( 6 ), RB_BLACK },
  { 4, TN( 6 ), NULL, NULL, RB_BLACK },
  { 6, TN( 3 ), TN( 4 ), TN( 8 ), RB_RED },
  { 7, TN( 8 ), NULL, NULL, RB_RED },
  { 8, TN( 6 ), TN( 7 ), TN( 11 ), RB_BLACK },
  { 11, TN( 8 ), NULL, NULL, RB_RED },
  { 12, NULL, TN( 3 ), TN( 17 ), RB_BLACK },
  { 13, TN( 17 ), NULL, TN( 14 ), RB_BLACK },
  { 14, TN( 13 ), NULL, NULL, RB_RED },
  { 17, TN( 12 ), TN( 13 ), TN( 25 ), RB_BLACK },
  { 22, TN( 25 ), NULL, NULL, RB_RED },
  { 25, TN( 17 ), TN( 22 ), TN( 27 ), RB_BLACK },
  { 27, TN( 25 ), NULL, NULL, RB_RED }
};

static const TestNodeDescription random_ops_tree_multiple_29[] = {
  { 0, TN( 3 ), NULL, TN( 1 ), RB_BLACK },
  { 0, TN( 0 ), NULL, NULL, RB_RED },
  { 1, TN( 11 ), TN( 0 ), TN( 6 ), RB_BLACK },
  { 2, TN( 6 ), NULL, NULL, RB_BLACK },
  { 3, TN( 3 ), TN( 4 ), TN( 7 ), RB_RED },
  { 3, TN( 6 ), NULL, TN( 8 ), RB_BLACK },
  { 4, TN( 7 ), NULL, NULL, RB_RED },
  { 5, NULL, TN( 3 ), TN( 22 ), RB_BLACK },
  { 6, TN( 12 ), NULL, NULL, RB_BLACK },
  { 6, TN( 22 ), TN( 13 ), TN( 17 ), RB_RED },
  { 7, TN( 17 ), NULL, NULL, RB_RED },
  { 8, TN( 12 ), TN( 14 ), NULL, RB_BLACK },
  { 11, TN( 11 ), TN( 12 ), TN( 25 ), RB_BLACK },
  { 12, TN( 22 ), NULL, TN( 27 ), RB_BLACK },
  { 13, TN( 25 ), NULL, NULL, RB_RED }
};

static const TestNodeDescription random_ops_tree_unique_30[] = {
  { 0, TN( 4 ), NULL, NULL, RB_RED },
  { 4, TN( 6 ), TN( 0 ), NULL, RB_BLACK },
  { 6, TN( 13 ), TN( 4 ), TN( 9 ), RB_RED },
  { 8, TN( 9 ), NULL, NULL, RB_RED },
  { 9, TN( 6 ), TN( 8 ), TN( 12 ), RB_BLACK },
  { 12, TN( 9 ), NULL, NULL, RB_RED },
  { 13, NULL, TN( 6 ), TN( 18 ), RB_BLACK },
  { 14, TN( 16 ), NULL, NULL, RB_RED },
  { 16, TN( 18 ), TN( 14 ), TN( 17 ), RB_BLACK },
  { 17, TN( 16 ), NULL, NULL, RB_RED },
  { 18, TN( 13 ), TN( 16 ), TN( 27 ), RB_RED },
  { 20, TN( 27 ), NULL, NULL, RB_RED },
  { 27, TN( 18 ), TN( 20 ), TN( 28 ), RB_BLACK },
  { 28, TN( 27 ), NULL, NULL, RB_RED }
};

static const TestNodeDescription random_ops_tree_multiple_30[] = {
  { 0, TN( 4 ), NULL, NULL, RB_BLACK },
  { 2, TN( 13 ), TN( 0 ), TN( 9 ), RB_RED },
  { 3, TN( 9 ), NULL, NULL, RB_RED },
  { 4, TN( 4 ), TN( 6 ), TN( 8 ), RB_BLACK },
  { 4, TN( 9 ), NULL, NULL, RB_RED },
  { 6, TN( 14 ), TN( 4 ), TN( 12 ), RB_BLACK },
  { 6, TN( 13 ), NULL, NULL, RB_BLACK },
  { 7, NULL, TN( 13 ), TN( 18 ), RB_BLACK },
  { 8, TN( 18 ), NULL, TN( 16 ), RB_BLACK },
  { 8, TN( 17 ), NULL, NULL, RB_RED },
  { 9, TN( 14 ), TN( 17 ), TN( 27 ), RB_BLACK },
  { 10, TN( 27 ), NULL, NULL, RB_RED },
  { 13, TN( 18 ), TN( 20 ), TN( 28 ), RB_BLACK },
  { 14, TN( 27 ), NULL, NULL, RB_RED }
};

static const TestNodeDescription random_ops_tree_unique_31[] = {
  { 0, TN( 2 ), NULL, NULL, RB_RED },
  { 2, TN( 5 ), TN( 0 ), NULL, RB_BLACK },
  { 5, TN( 11 ), TN( 2 ), TN( 9 ), RB_BLACK },
  { 7, TN( 9 ), NULL, NULL, RB_RED },
  { 9, TN( 5 ), TN( 7 ), NULL, RB_BLACK },
  { 11, NULL, TN( 5 ), TN( 21 ), RB_BLACK },
  { 14, TN( 16 ), NULL, NULL, RB_RED },
  { 16, TN( 21 ), TN( 14 ), TN( 18 ), RB_BLACK },
  { 18, TN( 16 ), NULL, NULL, RB_RED },
  { 21, TN( 11 ), TN( 16 ), TN( 30 ), RB_BLACK },
  { 30, TN( 21 ), NULL, NULL, RB_BLACK }
};

static const TestNodeDescription random_ops_tree_multiple_31[] = {
  { 0, TN( 2 ), NULL, NULL, RB_RED },
  { 1, TN( 5 ), TN( 0 ), NULL, RB_BLACK },
  { 2, TN( 11 ), TN( 2 ), TN( 9 ), RB_BLACK },
  { 3, TN( 9 ), NULL, NULL, RB_RED },
  { 4, TN( 5 ), TN( 7 ), NULL, RB_BLACK },
  { 5, NULL, TN( 5 ), TN( 21 ), RB_BLACK },
  { 7, TN( 16 ), NULL, NULL, RB_RED },
  { 8, TN( 21 ), TN( 14 ), TN( 18 ), RB_BLACK },
  { 9, TN( 16 ), NULL, NULL, RB_RED },
  { 10, TN( 11 ), TN( 16 ), TN( 30 ), RB_BLACK },
  { 15, TN( 21 ), NULL, NULL, RB_BLACK }
};

#define RANDOM_OPS_TREE( i ) \
  { &random_ops_tree_multiple_##i[ 0 ], &random_ops_tree_unique_##i[ 0 ] }

static const TestNodeDescription *const random_ops_trees[][2] = {
  RANDOM_OPS_TREE( 1 ),
  RANDOM_OPS_TREE( 2 ),
  RANDOM_OPS_TREE( 3 ),
  RANDOM_OPS_TREE( 4 ),
  RANDOM_OPS_TREE( 5 ),
  RANDOM_OPS_TREE( 6 ),
  RANDOM_OPS_TREE( 7 ),
  RANDOM_OPS_TREE( 8 ),
  RANDOM_OPS_TREE( 9 ),
  RANDOM_OPS_TREE( 10 ),
  RANDOM_OPS_TREE( 11 ),
  RANDOM_OPS_TREE( 12 ),
  RANDOM_OPS_TREE( 13 ),
  RANDOM_OPS_TREE( 14 ),
  RANDOM_OPS_TREE( 15 ),
  RANDOM_OPS_TREE( 16 ),
  RANDOM_OPS_TREE( 17 ),
  RANDOM_OPS_TREE( 18 ),
  RANDOM_OPS_TREE( 19 ),
  RANDOM_OPS_TREE( 20 ),
  RANDOM_OPS_TREE( 21 ),
  RANDOM_OPS_TREE( 22 ),
  RANDOM_OPS_TREE( 23 ),
  RANDOM_OPS_TREE( 24 ),
  RANDOM_OPS_TREE( 25 ),
  RANDOM_OPS_TREE( 26 ),
  RANDOM_OPS_TREE( 27 ),
  RANDOM_OPS_TREE( 28 ),
  RANDOM_OPS_TREE( 29 ),
  RANDOM_OPS_TREE( 30 ),
  RANDOM_OPS_TREE( 31 )
};

#define RANDOM_OPS_TREE_COUNT( i ) \
  { \
    RTEMS_ARRAY_SIZE( random_ops_tree_multiple_##i ), \
    RTEMS_ARRAY_SIZE( random_ops_tree_unique_##i ) \
  }

static const size_t random_ops_tree_counts[][2] = {
  RANDOM_OPS_TREE_COUNT( 1 ),
  RANDOM_OPS_TREE_COUNT( 2 ),
  RANDOM_OPS_TREE_COUNT( 3 ),
  RANDOM_OPS_TREE_COUNT( 4 ),
  RANDOM_OPS_TREE_COUNT( 5 ),
  RANDOM_OPS_TREE_COUNT( 6 ),
  RANDOM_OPS_TREE_COUNT( 7 ),
  RANDOM_OPS_TREE_COUNT( 8 ),
  RANDOM_OPS_TREE_COUNT( 9 ),
  RANDOM_OPS_TREE_COUNT( 10 ),
  RANDOM_OPS_TREE_COUNT( 11 ),
  RANDOM_OPS_TREE_COUNT( 12 ),
  RANDOM_OPS_TREE_COUNT( 13 ),
  RANDOM_OPS_TREE_COUNT( 14 ),
  RANDOM_OPS_TREE_COUNT( 15 ),
  RANDOM_OPS_TREE_COUNT( 16 ),
  RANDOM_OPS_TREE_COUNT( 17 ),
  RANDOM_OPS_TREE_COUNT( 18 ),
  RANDOM_OPS_TREE_COUNT( 19 ),
  RANDOM_OPS_TREE_COUNT( 20 ),
  RANDOM_OPS_TREE_COUNT( 21 ),
  RANDOM_OPS_TREE_COUNT( 22 ),
  RANDOM_OPS_TREE_COUNT( 23 ),
  RANDOM_OPS_TREE_COUNT( 24 ),
  RANDOM_OPS_TREE_COUNT( 25 ),
  RANDOM_OPS_TREE_COUNT( 26 ),
  RANDOM_OPS_TREE_COUNT( 27 ),
  RANDOM_OPS_TREE_COUNT( 28 ),
  RANDOM_OPS_TREE_COUNT( 29 ),
  RANDOM_OPS_TREE_COUNT( 30 ),
  RANDOM_OPS_TREE_COUNT( 31 )
};

static uint32_t SimpleRandom( uint32_t v )
{
  v *= 1664525;
  v += 1013904223;

  return v;
}

static void RandomOps( size_t n, bool unique )
{
  VisitorContext ctx = {
    .current = 0,
    .count = random_ops_tree_counts[ n - 1 ][ unique ],
    .tree = random_ops_trees[ n - 1 ][ unique ]
  };
  RBTree_Control tree;
  TestNode      *nodes;
  size_t         m;
  size_t         s;
  uint32_t       v;
  size_t         i;

  nodes = &node_array[ 0 ];
  m = n * n * n;
  s = unique ? 1 : 2;
  v = 0xdeadbeef;
  _RBTree_Initialize_empty( &tree );

  memset( nodes, 0, n * sizeof( *nodes ) );

  for ( i = 0; i < n; ++i ) {
    nodes[ i ].key = (int) ( i / s );
  }

  for ( i = 0; i < m; ++i ) {
    size_t j = ( v >> 13 ) % n;
    TestNode *tn = &nodes[ j ];

    if ( tn->id == 0 ) {
      tn->id = 1;
      _RBTree_Initialize_node( &tn->Node );
      _RBTree_Insert_inline( &tree, &tn->Node, &tn->key, Less );
    } else {
      tn->id = 0;
      _RBTree_Extract( &tree, &tn->Node );
    }

    T_ne_int( VerifyTree( _RBTree_Root( &tree ) ), -1 );

    v = SimpleRandom( v );
  }

  _RBTree_Iterate( &tree, VisitNodes, &ctx );
  T_true( ctx.current == ctx.count );
}

/**
 * @brief Call _RBTree_Initialize_one() and check the tree properties.
 */
static void ScoreRbtreeUnitRbtree_Action_0( void )
{
  RBTree_Control tree;
  RBTree_Node    node;

  _RBTree_Initialize_node( &node );
  _RBTree_Initialize_one( &tree, &node );

  /*
   * Check that the tree is not emtpy.
   */
  T_false( _RBTree_Is_empty( &tree ) );

  /*
   * Check that the tree root is is the only node.
   */
  T_true( _RBTree_Is_root( &node ) );

  /*
   * Check that the node is not off the tree.
   */
  T_false( _RBTree_Is_node_off_tree( &node ) );

  /*
   * Check that the node has no left child.
   */
  T_null( _RBTree_Left( &node ) );

  /*
   * Check that the node has no right child.
   */
  T_null( _RBTree_Right( &node ) );

  /*
   * Check that the node has no parent.
   */
  T_null( _RBTree_Parent( &node ) );

  /*
   * Check that the node has no successor.
   */
  T_null( _RBTree_Successor( &node ) );

  /*
   * Check that the node has no predecessor.
   */
  T_null( _RBTree_Predecessor( &node ) );

  /*
   * Check that the minimum node is the node.
   */
  T_eq_ptr( _RBTree_Minimum( &tree ), &node );

  /*
   * Check that the maximum node is the node.
   */
  T_eq_ptr( _RBTree_Maximum( &tree ), &node );

  /*
   * Check that the tree is emtpy after extraction of the node.
   */
  _RBTree_Extract( &tree, &node );
  T_true( _RBTree_Is_empty( &tree ) );
}

/**
 * @brief Call _RBTree_Insert_inline() and check the return status for a sample
 *   set of nodes.
 */
static void ScoreRbtreeUnitRbtree_Action_1( void )
{
  RBTree_Control tree;
  TestNode       a;
  TestNode       b;
  TestNode       c;
  bool           is_new_minimum;

  _RBTree_Initialize_empty( &tree );

  /*
   * Insert the first node.  Check that it is the new minimum node.
   */
  _RBTree_Initialize_node( &b.Node );
  b.key = 2;
  is_new_minimum = _RBTree_Insert_inline( &tree, &b.Node, &b.key, Less );
  T_true( is_new_minimum );

  /*
   * Insert the second node.  Check that it is not the new minimum node.
   */
  _RBTree_Initialize_node( &c.Node );
  c.key = 3;
  is_new_minimum = _RBTree_Insert_inline( &tree, &c.Node, &c.key, Less );
  T_false( is_new_minimum );

  /*
   * Insert the third node.  Check that it is the new minimum node.
   */
  _RBTree_Initialize_node( &a.Node );
  a.key = 1;
  is_new_minimum = _RBTree_Insert_inline( &tree, &a.Node, &a.key, Less );
  T_true( is_new_minimum );
}

/**
 * @brief Call _RBTree_Insert_inline() and _RBTree_Extract() for a sample set
 *   of trees.
 */
static void ScoreRbtreeUnitRbtree_Action_2( void )
{
  size_t n;

  for ( n = 0; n < RTEMS_ARRAY_SIZE( random_ops_trees ); ++n ) {
    RandomOps( n + 1, true );
    RandomOps( n + 1, false );
  }
}

/**
 * @fn void T_case_body_ScoreRbtreeUnitRbtree( void )
 */
T_TEST_CASE( ScoreRbtreeUnitRbtree )
{
  ScoreRbtreeUnitRbtree_Action_0();
  ScoreRbtreeUnitRbtree_Action_1();
  ScoreRbtreeUnitRbtree_Action_2();
}

/** @} */
