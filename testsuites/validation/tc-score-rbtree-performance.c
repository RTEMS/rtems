/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreRbtreeValPerf
 */

/*
 * Copyright (C) 2026 embedded brains GmbH & Co. KG
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/rbtreeimpl.h>

#include <rtems/test.h>

/**
 * @defgroup ScoreRbtreeValPerf spec:/score/rbtree/val/perf
 *
 * @ingroup TestsuitesPerformanceNoClock0
 *
 * @brief This test case provides a context to run red-black tree performance
 *   tests.
 *
 * @{
 */

#define NODE_COUNT_MAX 233

#define TREE_COUNT 16

typedef struct {
  RBTree_Node node;
  uint32_t    key;
} Node;

typedef struct {
  RBTree_Control tree;
  Node           nodes[ NODE_COUNT_MAX + 1 ];
  Node           spare;
  Node          *node;
  RBTree_Node   *result;
} Tree;

/**
 * @brief Test context for spec:/score/rbtree/val/perf test case.
 */
typedef struct {
  /**
   * @brief This member provides the trees.
   */
  Tree trees[ TREE_COUNT ];

  /**
   * @brief This member contains the node count of each tree.
   */
  uint32_t node_count;

  /**
   * @brief This member contains the index of the next pseudo-random position.
   */
  uint32_t sample;

  /**
   * @brief This member references the measure runtime context.
   */
  T_measure_runtime_context *context;

  /**
   * @brief This member provides the measure runtime request.
   */
  T_measure_runtime_request request;

  /**
   * @brief This member provides an optional measurement begin time point.
   */
  T_ticks begin;

  /**
   * @brief This member provides an optional measurement end time point.
   */
  T_ticks end;
} ScoreRbtreeValPerf_Context;

static ScoreRbtreeValPerf_Context ScoreRbtreeValPerf_Instance;

typedef ScoreRbtreeValPerf_Context Context;

/* clang-format off */

static const uint8_t random_index[] = {
  147, 21, 128, 74, 3, 222, 137, 76, 165, 10, 94, 192, 184, 72, 153, 42, 4,
  120, 246, 152, 204, 232, 124, 104, 215, 217, 143, 187, 111, 27, 90, 240, 23,
  247, 179, 211, 78, 18, 6, 125, 43, 133, 35, 169, 142, 191, 171, 185, 189,
  167, 162, 116, 227, 199, 31, 81, 175, 244, 126, 230, 201, 20, 209, 203, 188,
  15, 206, 214, 0, 89, 151, 106, 59, 85, 9, 205, 196, 29, 101, 16, 118, 200,
  181, 19, 223, 8, 56, 251, 148, 62, 195, 33, 87, 86, 41, 220, 176, 113, 119,
  26, 52, 53, 58, 156, 55, 177, 61, 17, 157, 237, 68, 93, 44, 149, 69, 60, 134,
  122, 168, 131, 24, 117, 180, 109, 63, 11, 234, 135, 70, 110, 40, 79, 155,
  202, 212, 25, 213, 238, 127, 159, 95, 47, 242, 145, 51, 112, 164, 186, 115,
  132, 121, 178, 207, 174, 92, 84, 193, 236, 7, 160, 241, 239, 114, 28, 66,
  182, 170, 190, 150, 219, 14, 48, 80, 83, 161, 154, 22, 98, 249, 252, 38, 105,
  226, 130, 146, 103, 245, 36, 123, 194, 65, 97, 248, 73, 64, 231, 34, 5, 255,
  102, 37, 221, 140, 77, 108, 136, 218, 46, 254, 208, 91, 144, 30, 100, 163,
  96, 107, 210, 12, 139, 45, 39, 229, 32, 198, 99, 50, 183, 88, 158, 216, 172,
  228, 235, 82, 233, 49, 129, 1, 75, 13, 166, 141, 67, 71, 54, 173, 2, 224,
  253, 225, 138, 250, 197, 57, 243,
};

/* clang-format on */

static bool Less( const void *left, const RBTree_Node *right )
{
  const uint32_t *key;
  const Node     *node;

  key = left;
  node = RTEMS_CONTAINER_OF( right, Node, node );

  return *key < node->key;
}

static void Insert( Tree *tree, Node *node )
{
  (void) _RBTree_Insert_inline( &tree->tree, &node->node, &node->key, Less );
}

static void Extract( Tree *tree, Node *node )
{
  _RBTree_Extract( &tree->tree, &node->node );
}

static uint32_t NextIndex( Context *ctx )
{
  uint32_t index;

  index = random_index[ ctx->sample % RTEMS_ARRAY_SIZE( random_index ) ];
  ++ctx->sample;

  return index;
}

/*
 * Node i of each tree has the key 2 * i + 2.  A key with the value 2 * j + 1
 * for 0 <= j <= n lies at position j of a tree of n nodes.  All trees have the
 * same shape.
 */
static void PrepareTree( Context *ctx, uint32_t node_count, bool ascending )
{
  size_t k;

  ctx->node_count = node_count;
  ctx->sample = 0;

  for ( k = 0; k < TREE_COUNT; ++k ) {
    Tree    *tree;
    uint32_t i;

    tree = &ctx->trees[ k ];
    _RBTree_Initialize_empty( &tree->tree );

    for ( i = 0; i <= NODE_COUNT_MAX; ++i ) {
      _RBTree_Initialize_node( &tree->nodes[ i ].node );
      tree->nodes[ i ].key = 2 * i + 2;
    }

    _RBTree_Initialize_node( &tree->spare.node );

    if ( ascending ) {
      for ( i = 0; i < node_count; ++i ) {
        Insert( tree, &tree->nodes[ i ] );
      }
    } else {
      for ( i = 0; i < RTEMS_ARRAY_SIZE( random_index ); ++i ) {
        uint32_t j;

        j = random_index[ i ];

        if ( j < node_count ) {
          Insert( tree, &tree->nodes[ j ] );
        }
      }
    }
  }
}

static void SetupInsert( Context *ctx, bool ascending )
{
  size_t k;

  for ( k = 0; k < TREE_COUNT; ++k ) {
    Tree *tree;
    Node *node;

    tree = &ctx->trees[ k ];
    node = &tree->nodes[ ctx->node_count ];
    tree->node = node;

    if ( ascending ) {
      node->key = 2 * ctx->node_count + 3;
    } else {
      node->key = 2 * ( NextIndex( ctx ) % ( ctx->node_count + 1 ) ) + 1;
    }
  }
}

static void SetupRandomNode( Context *ctx )
{
  size_t k;

  for ( k = 0; k < TREE_COUNT; ++k ) {
    Tree *tree;

    tree = &ctx->trees[ k ];
    tree->node = &tree->nodes[ NextIndex( ctx ) % ctx->node_count ];
  }
}

static void SetupMinimumNode( Context *ctx )
{
  size_t k;

  for ( k = 0; k < TREE_COUNT; ++k ) {
    Tree *tree;

    tree = &ctx->trees[ k ];
    tree->node = RTEMS_CONTAINER_OF(
      _RBTree_Minimum( &tree->tree ),
      Node,
      node
    );
  }
}

#if defined( RTEMS_SMP )
static void SetupReplace( Context *ctx )
{
  size_t k;

  SetupRandomNode( ctx );

  for ( k = 0; k < TREE_COUNT; ++k ) {
    Tree *tree;

    tree = &ctx->trees[ k ];
    tree->spare.key = tree->node->key;
  }
}
#endif

static void InsertAll( Context *ctx )
{
  size_t k;

  for ( k = 0; k < TREE_COUNT; ++k ) {
    Insert( &ctx->trees[ k ], ctx->trees[ k ].node );
  }
}

static void ExtractAll( Context *ctx )
{
  size_t k;

  for ( k = 0; k < TREE_COUNT; ++k ) {
    Extract( &ctx->trees[ k ], ctx->trees[ k ].node );
  }
}

static void MinimumAll( Context *ctx )
{
  size_t k;

  for ( k = 0; k < TREE_COUNT; ++k ) {
    ctx->trees[ k ].result = _RBTree_Minimum( &ctx->trees[ k ].tree );
  }
}

static void SuccessorAll( Context *ctx )
{
  size_t k;

  for ( k = 0; k < TREE_COUNT; ++k ) {
    ctx->trees[ k ].result = _RBTree_Successor( &ctx->trees[ k ].node->node );
  }
}

#if defined( RTEMS_SMP )
static void ReplaceAll( Context *ctx, bool spare )
{
  size_t k;

  for ( k = 0; k < TREE_COUNT; ++k ) {
    Tree *tree;

    tree = &ctx->trees[ k ];

    if ( spare ) {
      _RBTree_Replace_node(
        &tree->tree,
        &tree->node->node,
        &tree->spare.node
      );
    } else {
      _RBTree_Replace_node(
        &tree->tree,
        &tree->spare.node,
        &tree->node->node
      );
    }
  }
}
#endif

static void ScoreRbtreeValPerf_Setup_Context( ScoreRbtreeValPerf_Context *ctx )
{
  T_measure_runtime_config config;

  memset( &config, 0, sizeof( config ) );
  config.sample_count = 100;
  ctx->request.arg = ctx;
  ctx->request.flags = T_MEASURE_RUNTIME_REPORT_SAMPLES;
  ctx->context = T_measure_runtime_create( &config );
  T_assert_not_null( ctx->context );
}

static void ScoreRbtreeValPerf_Setup_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeValPerf_Setup_Context( ctx );
}

static T_fixture ScoreRbtreeValPerf_Fixture = {
  .setup = ScoreRbtreeValPerf_Setup_Wrap,
  .stop = NULL,
  .teardown = NULL,
  .scope = NULL,
  .initial_context = &ScoreRbtreeValPerf_Instance
};

/**
 * @defgroup ScoreRbtreeReqPerfExtractMinimum1 \
 *   spec:/score/rbtree/req/perf-extract-minimum-1
 *
 * @{
 */

/**
 * @brief Prepare each tree with 1 nodes.
 */
static void ScoreRbtreeReqPerfExtractMinimum1_Prepare(
  ScoreRbtreeValPerf_Context *ctx
)
{
  PrepareTree( ctx, 1, false );
}

/**
 * @brief Select the node to extract in each tree.
 */
static void ScoreRbtreeReqPerfExtractMinimum1_Setup(
  ScoreRbtreeValPerf_Context *ctx
)
{
  SetupMinimumNode( ctx );
}

static void ScoreRbtreeReqPerfExtractMinimum1_Setup_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfExtractMinimum1_Setup( ctx );
}

/**
 * @brief Extract the node from each tree.
 */
static void ScoreRbtreeReqPerfExtractMinimum1_Body(
  ScoreRbtreeValPerf_Context *ctx
)
{
  ExtractAll( ctx );
}

static void ScoreRbtreeReqPerfExtractMinimum1_Body_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfExtractMinimum1_Body( ctx );
}

/**
 * @brief Insert the nodes again. Discard samples interrupted by a clock tick.
 */
static bool ScoreRbtreeReqPerfExtractMinimum1_Teardown(
  ScoreRbtreeValPerf_Context *ctx,
  uint32_t                    tic,
  uint32_t                    toc
)
{
  InsertAll( ctx );

  return tic == toc;
}

static bool ScoreRbtreeReqPerfExtractMinimum1_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  (void) delta;
  (void) retry;
  return ScoreRbtreeReqPerfExtractMinimum1_Teardown( ctx, tic, toc );
}

/** @} */

/**
 * @defgroup ScoreRbtreeReqPerfExtractMinimum13 \
 *   spec:/score/rbtree/req/perf-extract-minimum-13
 *
 * @{
 */

/**
 * @brief Prepare each tree with 13 nodes.
 */
static void ScoreRbtreeReqPerfExtractMinimum13_Prepare(
  ScoreRbtreeValPerf_Context *ctx
)
{
  PrepareTree( ctx, 13, false );
}

/**
 * @brief Select the node to extract in each tree.
 */
static void ScoreRbtreeReqPerfExtractMinimum13_Setup(
  ScoreRbtreeValPerf_Context *ctx
)
{
  SetupMinimumNode( ctx );
}

static void ScoreRbtreeReqPerfExtractMinimum13_Setup_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfExtractMinimum13_Setup( ctx );
}

/**
 * @brief Extract the node from each tree.
 */
static void ScoreRbtreeReqPerfExtractMinimum13_Body(
  ScoreRbtreeValPerf_Context *ctx
)
{
  ExtractAll( ctx );
}

static void ScoreRbtreeReqPerfExtractMinimum13_Body_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfExtractMinimum13_Body( ctx );
}

/**
 * @brief Insert the nodes again. Discard samples interrupted by a clock tick.
 */
static bool ScoreRbtreeReqPerfExtractMinimum13_Teardown(
  ScoreRbtreeValPerf_Context *ctx,
  uint32_t                    tic,
  uint32_t                    toc
)
{
  InsertAll( ctx );

  return tic == toc;
}

static bool ScoreRbtreeReqPerfExtractMinimum13_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  (void) delta;
  (void) retry;
  return ScoreRbtreeReqPerfExtractMinimum13_Teardown( ctx, tic, toc );
}

/** @} */

/**
 * @defgroup ScoreRbtreeReqPerfExtractMinimum2 \
 *   spec:/score/rbtree/req/perf-extract-minimum-2
 *
 * @{
 */

/**
 * @brief Prepare each tree with 2 nodes.
 */
static void ScoreRbtreeReqPerfExtractMinimum2_Prepare(
  ScoreRbtreeValPerf_Context *ctx
)
{
  PrepareTree( ctx, 2, false );
}

/**
 * @brief Select the node to extract in each tree.
 */
static void ScoreRbtreeReqPerfExtractMinimum2_Setup(
  ScoreRbtreeValPerf_Context *ctx
)
{
  SetupMinimumNode( ctx );
}

static void ScoreRbtreeReqPerfExtractMinimum2_Setup_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfExtractMinimum2_Setup( ctx );
}

/**
 * @brief Extract the node from each tree.
 */
static void ScoreRbtreeReqPerfExtractMinimum2_Body(
  ScoreRbtreeValPerf_Context *ctx
)
{
  ExtractAll( ctx );
}

static void ScoreRbtreeReqPerfExtractMinimum2_Body_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfExtractMinimum2_Body( ctx );
}

/**
 * @brief Insert the nodes again. Discard samples interrupted by a clock tick.
 */
static bool ScoreRbtreeReqPerfExtractMinimum2_Teardown(
  ScoreRbtreeValPerf_Context *ctx,
  uint32_t                    tic,
  uint32_t                    toc
)
{
  InsertAll( ctx );

  return tic == toc;
}

static bool ScoreRbtreeReqPerfExtractMinimum2_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  (void) delta;
  (void) retry;
  return ScoreRbtreeReqPerfExtractMinimum2_Teardown( ctx, tic, toc );
}

/** @} */

/**
 * @defgroup ScoreRbtreeReqPerfExtractMinimum233 \
 *   spec:/score/rbtree/req/perf-extract-minimum-233
 *
 * @{
 */

/**
 * @brief Prepare each tree with 233 nodes.
 */
static void ScoreRbtreeReqPerfExtractMinimum233_Prepare(
  ScoreRbtreeValPerf_Context *ctx
)
{
  PrepareTree( ctx, 233, false );
}

/**
 * @brief Select the node to extract in each tree.
 */
static void ScoreRbtreeReqPerfExtractMinimum233_Setup(
  ScoreRbtreeValPerf_Context *ctx
)
{
  SetupMinimumNode( ctx );
}

static void ScoreRbtreeReqPerfExtractMinimum233_Setup_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfExtractMinimum233_Setup( ctx );
}

/**
 * @brief Extract the node from each tree.
 */
static void ScoreRbtreeReqPerfExtractMinimum233_Body(
  ScoreRbtreeValPerf_Context *ctx
)
{
  ExtractAll( ctx );
}

static void ScoreRbtreeReqPerfExtractMinimum233_Body_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfExtractMinimum233_Body( ctx );
}

/**
 * @brief Insert the nodes again. Discard samples interrupted by a clock tick.
 */
static bool ScoreRbtreeReqPerfExtractMinimum233_Teardown(
  ScoreRbtreeValPerf_Context *ctx,
  uint32_t                    tic,
  uint32_t                    toc
)
{
  InsertAll( ctx );

  return tic == toc;
}

static bool ScoreRbtreeReqPerfExtractMinimum233_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  (void) delta;
  (void) retry;
  return ScoreRbtreeReqPerfExtractMinimum233_Teardown( ctx, tic, toc );
}

/** @} */

/**
 * @defgroup ScoreRbtreeReqPerfExtractMinimum3 \
 *   spec:/score/rbtree/req/perf-extract-minimum-3
 *
 * @{
 */

/**
 * @brief Prepare each tree with 3 nodes.
 */
static void ScoreRbtreeReqPerfExtractMinimum3_Prepare(
  ScoreRbtreeValPerf_Context *ctx
)
{
  PrepareTree( ctx, 3, false );
}

/**
 * @brief Select the node to extract in each tree.
 */
static void ScoreRbtreeReqPerfExtractMinimum3_Setup(
  ScoreRbtreeValPerf_Context *ctx
)
{
  SetupMinimumNode( ctx );
}

static void ScoreRbtreeReqPerfExtractMinimum3_Setup_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfExtractMinimum3_Setup( ctx );
}

/**
 * @brief Extract the node from each tree.
 */
static void ScoreRbtreeReqPerfExtractMinimum3_Body(
  ScoreRbtreeValPerf_Context *ctx
)
{
  ExtractAll( ctx );
}

static void ScoreRbtreeReqPerfExtractMinimum3_Body_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfExtractMinimum3_Body( ctx );
}

/**
 * @brief Insert the nodes again. Discard samples interrupted by a clock tick.
 */
static bool ScoreRbtreeReqPerfExtractMinimum3_Teardown(
  ScoreRbtreeValPerf_Context *ctx,
  uint32_t                    tic,
  uint32_t                    toc
)
{
  InsertAll( ctx );

  return tic == toc;
}

static bool ScoreRbtreeReqPerfExtractMinimum3_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  (void) delta;
  (void) retry;
  return ScoreRbtreeReqPerfExtractMinimum3_Teardown( ctx, tic, toc );
}

/** @} */

/**
 * @defgroup ScoreRbtreeReqPerfExtractMinimum34 \
 *   spec:/score/rbtree/req/perf-extract-minimum-34
 *
 * @{
 */

/**
 * @brief Prepare each tree with 34 nodes.
 */
static void ScoreRbtreeReqPerfExtractMinimum34_Prepare(
  ScoreRbtreeValPerf_Context *ctx
)
{
  PrepareTree( ctx, 34, false );
}

/**
 * @brief Select the node to extract in each tree.
 */
static void ScoreRbtreeReqPerfExtractMinimum34_Setup(
  ScoreRbtreeValPerf_Context *ctx
)
{
  SetupMinimumNode( ctx );
}

static void ScoreRbtreeReqPerfExtractMinimum34_Setup_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfExtractMinimum34_Setup( ctx );
}

/**
 * @brief Extract the node from each tree.
 */
static void ScoreRbtreeReqPerfExtractMinimum34_Body(
  ScoreRbtreeValPerf_Context *ctx
)
{
  ExtractAll( ctx );
}

static void ScoreRbtreeReqPerfExtractMinimum34_Body_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfExtractMinimum34_Body( ctx );
}

/**
 * @brief Insert the nodes again. Discard samples interrupted by a clock tick.
 */
static bool ScoreRbtreeReqPerfExtractMinimum34_Teardown(
  ScoreRbtreeValPerf_Context *ctx,
  uint32_t                    tic,
  uint32_t                    toc
)
{
  InsertAll( ctx );

  return tic == toc;
}

static bool ScoreRbtreeReqPerfExtractMinimum34_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  (void) delta;
  (void) retry;
  return ScoreRbtreeReqPerfExtractMinimum34_Teardown( ctx, tic, toc );
}

/** @} */

/**
 * @defgroup ScoreRbtreeReqPerfExtractMinimum5 \
 *   spec:/score/rbtree/req/perf-extract-minimum-5
 *
 * @{
 */

/**
 * @brief Prepare each tree with 5 nodes.
 */
static void ScoreRbtreeReqPerfExtractMinimum5_Prepare(
  ScoreRbtreeValPerf_Context *ctx
)
{
  PrepareTree( ctx, 5, false );
}

/**
 * @brief Select the node to extract in each tree.
 */
static void ScoreRbtreeReqPerfExtractMinimum5_Setup(
  ScoreRbtreeValPerf_Context *ctx
)
{
  SetupMinimumNode( ctx );
}

static void ScoreRbtreeReqPerfExtractMinimum5_Setup_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfExtractMinimum5_Setup( ctx );
}

/**
 * @brief Extract the node from each tree.
 */
static void ScoreRbtreeReqPerfExtractMinimum5_Body(
  ScoreRbtreeValPerf_Context *ctx
)
{
  ExtractAll( ctx );
}

static void ScoreRbtreeReqPerfExtractMinimum5_Body_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfExtractMinimum5_Body( ctx );
}

/**
 * @brief Insert the nodes again. Discard samples interrupted by a clock tick.
 */
static bool ScoreRbtreeReqPerfExtractMinimum5_Teardown(
  ScoreRbtreeValPerf_Context *ctx,
  uint32_t                    tic,
  uint32_t                    toc
)
{
  InsertAll( ctx );

  return tic == toc;
}

static bool ScoreRbtreeReqPerfExtractMinimum5_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  (void) delta;
  (void) retry;
  return ScoreRbtreeReqPerfExtractMinimum5_Teardown( ctx, tic, toc );
}

/** @} */

/**
 * @defgroup ScoreRbtreeReqPerfExtractMinimum89 \
 *   spec:/score/rbtree/req/perf-extract-minimum-89
 *
 * @{
 */

/**
 * @brief Prepare each tree with 89 nodes.
 */
static void ScoreRbtreeReqPerfExtractMinimum89_Prepare(
  ScoreRbtreeValPerf_Context *ctx
)
{
  PrepareTree( ctx, 89, false );
}

/**
 * @brief Select the node to extract in each tree.
 */
static void ScoreRbtreeReqPerfExtractMinimum89_Setup(
  ScoreRbtreeValPerf_Context *ctx
)
{
  SetupMinimumNode( ctx );
}

static void ScoreRbtreeReqPerfExtractMinimum89_Setup_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfExtractMinimum89_Setup( ctx );
}

/**
 * @brief Extract the node from each tree.
 */
static void ScoreRbtreeReqPerfExtractMinimum89_Body(
  ScoreRbtreeValPerf_Context *ctx
)
{
  ExtractAll( ctx );
}

static void ScoreRbtreeReqPerfExtractMinimum89_Body_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfExtractMinimum89_Body( ctx );
}

/**
 * @brief Insert the nodes again. Discard samples interrupted by a clock tick.
 */
static bool ScoreRbtreeReqPerfExtractMinimum89_Teardown(
  ScoreRbtreeValPerf_Context *ctx,
  uint32_t                    tic,
  uint32_t                    toc
)
{
  InsertAll( ctx );

  return tic == toc;
}

static bool ScoreRbtreeReqPerfExtractMinimum89_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  (void) delta;
  (void) retry;
  return ScoreRbtreeReqPerfExtractMinimum89_Teardown( ctx, tic, toc );
}

/** @} */

/**
 * @defgroup ScoreRbtreeReqPerfExtractRandom1 \
 *   spec:/score/rbtree/req/perf-extract-random-1
 *
 * @{
 */

/**
 * @brief Prepare each tree with 1 nodes.
 */
static void ScoreRbtreeReqPerfExtractRandom1_Prepare(
  ScoreRbtreeValPerf_Context *ctx
)
{
  PrepareTree( ctx, 1, false );
}

/**
 * @brief Select the node to extract in each tree.
 */
static void ScoreRbtreeReqPerfExtractRandom1_Setup(
  ScoreRbtreeValPerf_Context *ctx
)
{
  SetupRandomNode( ctx );
}

static void ScoreRbtreeReqPerfExtractRandom1_Setup_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfExtractRandom1_Setup( ctx );
}

/**
 * @brief Extract the node from each tree.
 */
static void ScoreRbtreeReqPerfExtractRandom1_Body(
  ScoreRbtreeValPerf_Context *ctx
)
{
  ExtractAll( ctx );
}

static void ScoreRbtreeReqPerfExtractRandom1_Body_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfExtractRandom1_Body( ctx );
}

/**
 * @brief Insert the nodes again. Discard samples interrupted by a clock tick.
 */
static bool ScoreRbtreeReqPerfExtractRandom1_Teardown(
  ScoreRbtreeValPerf_Context *ctx,
  uint32_t                    tic,
  uint32_t                    toc
)
{
  InsertAll( ctx );

  return tic == toc;
}

static bool ScoreRbtreeReqPerfExtractRandom1_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  (void) delta;
  (void) retry;
  return ScoreRbtreeReqPerfExtractRandom1_Teardown( ctx, tic, toc );
}

/** @} */

/**
 * @defgroup ScoreRbtreeReqPerfExtractRandom13 \
 *   spec:/score/rbtree/req/perf-extract-random-13
 *
 * @{
 */

/**
 * @brief Prepare each tree with 13 nodes.
 */
static void ScoreRbtreeReqPerfExtractRandom13_Prepare(
  ScoreRbtreeValPerf_Context *ctx
)
{
  PrepareTree( ctx, 13, false );
}

/**
 * @brief Select the node to extract in each tree.
 */
static void ScoreRbtreeReqPerfExtractRandom13_Setup(
  ScoreRbtreeValPerf_Context *ctx
)
{
  SetupRandomNode( ctx );
}

static void ScoreRbtreeReqPerfExtractRandom13_Setup_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfExtractRandom13_Setup( ctx );
}

/**
 * @brief Extract the node from each tree.
 */
static void ScoreRbtreeReqPerfExtractRandom13_Body(
  ScoreRbtreeValPerf_Context *ctx
)
{
  ExtractAll( ctx );
}

static void ScoreRbtreeReqPerfExtractRandom13_Body_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfExtractRandom13_Body( ctx );
}

/**
 * @brief Insert the nodes again. Discard samples interrupted by a clock tick.
 */
static bool ScoreRbtreeReqPerfExtractRandom13_Teardown(
  ScoreRbtreeValPerf_Context *ctx,
  uint32_t                    tic,
  uint32_t                    toc
)
{
  InsertAll( ctx );

  return tic == toc;
}

static bool ScoreRbtreeReqPerfExtractRandom13_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  (void) delta;
  (void) retry;
  return ScoreRbtreeReqPerfExtractRandom13_Teardown( ctx, tic, toc );
}

/** @} */

/**
 * @defgroup ScoreRbtreeReqPerfExtractRandom2 \
 *   spec:/score/rbtree/req/perf-extract-random-2
 *
 * @{
 */

/**
 * @brief Prepare each tree with 2 nodes.
 */
static void ScoreRbtreeReqPerfExtractRandom2_Prepare(
  ScoreRbtreeValPerf_Context *ctx
)
{
  PrepareTree( ctx, 2, false );
}

/**
 * @brief Select the node to extract in each tree.
 */
static void ScoreRbtreeReqPerfExtractRandom2_Setup(
  ScoreRbtreeValPerf_Context *ctx
)
{
  SetupRandomNode( ctx );
}

static void ScoreRbtreeReqPerfExtractRandom2_Setup_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfExtractRandom2_Setup( ctx );
}

/**
 * @brief Extract the node from each tree.
 */
static void ScoreRbtreeReqPerfExtractRandom2_Body(
  ScoreRbtreeValPerf_Context *ctx
)
{
  ExtractAll( ctx );
}

static void ScoreRbtreeReqPerfExtractRandom2_Body_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfExtractRandom2_Body( ctx );
}

/**
 * @brief Insert the nodes again. Discard samples interrupted by a clock tick.
 */
static bool ScoreRbtreeReqPerfExtractRandom2_Teardown(
  ScoreRbtreeValPerf_Context *ctx,
  uint32_t                    tic,
  uint32_t                    toc
)
{
  InsertAll( ctx );

  return tic == toc;
}

static bool ScoreRbtreeReqPerfExtractRandom2_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  (void) delta;
  (void) retry;
  return ScoreRbtreeReqPerfExtractRandom2_Teardown( ctx, tic, toc );
}

/** @} */

/**
 * @defgroup ScoreRbtreeReqPerfExtractRandom233 \
 *   spec:/score/rbtree/req/perf-extract-random-233
 *
 * @{
 */

/**
 * @brief Prepare each tree with 233 nodes.
 */
static void ScoreRbtreeReqPerfExtractRandom233_Prepare(
  ScoreRbtreeValPerf_Context *ctx
)
{
  PrepareTree( ctx, 233, false );
}

/**
 * @brief Select the node to extract in each tree.
 */
static void ScoreRbtreeReqPerfExtractRandom233_Setup(
  ScoreRbtreeValPerf_Context *ctx
)
{
  SetupRandomNode( ctx );
}

static void ScoreRbtreeReqPerfExtractRandom233_Setup_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfExtractRandom233_Setup( ctx );
}

/**
 * @brief Extract the node from each tree.
 */
static void ScoreRbtreeReqPerfExtractRandom233_Body(
  ScoreRbtreeValPerf_Context *ctx
)
{
  ExtractAll( ctx );
}

static void ScoreRbtreeReqPerfExtractRandom233_Body_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfExtractRandom233_Body( ctx );
}

/**
 * @brief Insert the nodes again. Discard samples interrupted by a clock tick.
 */
static bool ScoreRbtreeReqPerfExtractRandom233_Teardown(
  ScoreRbtreeValPerf_Context *ctx,
  uint32_t                    tic,
  uint32_t                    toc
)
{
  InsertAll( ctx );

  return tic == toc;
}

static bool ScoreRbtreeReqPerfExtractRandom233_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  (void) delta;
  (void) retry;
  return ScoreRbtreeReqPerfExtractRandom233_Teardown( ctx, tic, toc );
}

/** @} */

/**
 * @defgroup ScoreRbtreeReqPerfExtractRandom3 \
 *   spec:/score/rbtree/req/perf-extract-random-3
 *
 * @{
 */

/**
 * @brief Prepare each tree with 3 nodes.
 */
static void ScoreRbtreeReqPerfExtractRandom3_Prepare(
  ScoreRbtreeValPerf_Context *ctx
)
{
  PrepareTree( ctx, 3, false );
}

/**
 * @brief Select the node to extract in each tree.
 */
static void ScoreRbtreeReqPerfExtractRandom3_Setup(
  ScoreRbtreeValPerf_Context *ctx
)
{
  SetupRandomNode( ctx );
}

static void ScoreRbtreeReqPerfExtractRandom3_Setup_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfExtractRandom3_Setup( ctx );
}

/**
 * @brief Extract the node from each tree.
 */
static void ScoreRbtreeReqPerfExtractRandom3_Body(
  ScoreRbtreeValPerf_Context *ctx
)
{
  ExtractAll( ctx );
}

static void ScoreRbtreeReqPerfExtractRandom3_Body_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfExtractRandom3_Body( ctx );
}

/**
 * @brief Insert the nodes again. Discard samples interrupted by a clock tick.
 */
static bool ScoreRbtreeReqPerfExtractRandom3_Teardown(
  ScoreRbtreeValPerf_Context *ctx,
  uint32_t                    tic,
  uint32_t                    toc
)
{
  InsertAll( ctx );

  return tic == toc;
}

static bool ScoreRbtreeReqPerfExtractRandom3_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  (void) delta;
  (void) retry;
  return ScoreRbtreeReqPerfExtractRandom3_Teardown( ctx, tic, toc );
}

/** @} */

/**
 * @defgroup ScoreRbtreeReqPerfExtractRandom34 \
 *   spec:/score/rbtree/req/perf-extract-random-34
 *
 * @{
 */

/**
 * @brief Prepare each tree with 34 nodes.
 */
static void ScoreRbtreeReqPerfExtractRandom34_Prepare(
  ScoreRbtreeValPerf_Context *ctx
)
{
  PrepareTree( ctx, 34, false );
}

/**
 * @brief Select the node to extract in each tree.
 */
static void ScoreRbtreeReqPerfExtractRandom34_Setup(
  ScoreRbtreeValPerf_Context *ctx
)
{
  SetupRandomNode( ctx );
}

static void ScoreRbtreeReqPerfExtractRandom34_Setup_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfExtractRandom34_Setup( ctx );
}

/**
 * @brief Extract the node from each tree.
 */
static void ScoreRbtreeReqPerfExtractRandom34_Body(
  ScoreRbtreeValPerf_Context *ctx
)
{
  ExtractAll( ctx );
}

static void ScoreRbtreeReqPerfExtractRandom34_Body_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfExtractRandom34_Body( ctx );
}

/**
 * @brief Insert the nodes again. Discard samples interrupted by a clock tick.
 */
static bool ScoreRbtreeReqPerfExtractRandom34_Teardown(
  ScoreRbtreeValPerf_Context *ctx,
  uint32_t                    tic,
  uint32_t                    toc
)
{
  InsertAll( ctx );

  return tic == toc;
}

static bool ScoreRbtreeReqPerfExtractRandom34_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  (void) delta;
  (void) retry;
  return ScoreRbtreeReqPerfExtractRandom34_Teardown( ctx, tic, toc );
}

/** @} */

/**
 * @defgroup ScoreRbtreeReqPerfExtractRandom5 \
 *   spec:/score/rbtree/req/perf-extract-random-5
 *
 * @{
 */

/**
 * @brief Prepare each tree with 5 nodes.
 */
static void ScoreRbtreeReqPerfExtractRandom5_Prepare(
  ScoreRbtreeValPerf_Context *ctx
)
{
  PrepareTree( ctx, 5, false );
}

/**
 * @brief Select the node to extract in each tree.
 */
static void ScoreRbtreeReqPerfExtractRandom5_Setup(
  ScoreRbtreeValPerf_Context *ctx
)
{
  SetupRandomNode( ctx );
}

static void ScoreRbtreeReqPerfExtractRandom5_Setup_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfExtractRandom5_Setup( ctx );
}

/**
 * @brief Extract the node from each tree.
 */
static void ScoreRbtreeReqPerfExtractRandom5_Body(
  ScoreRbtreeValPerf_Context *ctx
)
{
  ExtractAll( ctx );
}

static void ScoreRbtreeReqPerfExtractRandom5_Body_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfExtractRandom5_Body( ctx );
}

/**
 * @brief Insert the nodes again. Discard samples interrupted by a clock tick.
 */
static bool ScoreRbtreeReqPerfExtractRandom5_Teardown(
  ScoreRbtreeValPerf_Context *ctx,
  uint32_t                    tic,
  uint32_t                    toc
)
{
  InsertAll( ctx );

  return tic == toc;
}

static bool ScoreRbtreeReqPerfExtractRandom5_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  (void) delta;
  (void) retry;
  return ScoreRbtreeReqPerfExtractRandom5_Teardown( ctx, tic, toc );
}

/** @} */

/**
 * @defgroup ScoreRbtreeReqPerfExtractRandom89 \
 *   spec:/score/rbtree/req/perf-extract-random-89
 *
 * @{
 */

/**
 * @brief Prepare each tree with 89 nodes.
 */
static void ScoreRbtreeReqPerfExtractRandom89_Prepare(
  ScoreRbtreeValPerf_Context *ctx
)
{
  PrepareTree( ctx, 89, false );
}

/**
 * @brief Select the node to extract in each tree.
 */
static void ScoreRbtreeReqPerfExtractRandom89_Setup(
  ScoreRbtreeValPerf_Context *ctx
)
{
  SetupRandomNode( ctx );
}

static void ScoreRbtreeReqPerfExtractRandom89_Setup_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfExtractRandom89_Setup( ctx );
}

/**
 * @brief Extract the node from each tree.
 */
static void ScoreRbtreeReqPerfExtractRandom89_Body(
  ScoreRbtreeValPerf_Context *ctx
)
{
  ExtractAll( ctx );
}

static void ScoreRbtreeReqPerfExtractRandom89_Body_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfExtractRandom89_Body( ctx );
}

/**
 * @brief Insert the nodes again. Discard samples interrupted by a clock tick.
 */
static bool ScoreRbtreeReqPerfExtractRandom89_Teardown(
  ScoreRbtreeValPerf_Context *ctx,
  uint32_t                    tic,
  uint32_t                    toc
)
{
  InsertAll( ctx );

  return tic == toc;
}

static bool ScoreRbtreeReqPerfExtractRandom89_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  (void) delta;
  (void) retry;
  return ScoreRbtreeReqPerfExtractRandom89_Teardown( ctx, tic, toc );
}

/** @} */

/**
 * @defgroup ScoreRbtreeReqPerfInsertAscending1 \
 *   spec:/score/rbtree/req/perf-insert-ascending-1
 *
 * @{
 */

/**
 * @brief Prepare each tree with 1 nodes.
 */
static void ScoreRbtreeReqPerfInsertAscending1_Prepare(
  ScoreRbtreeValPerf_Context *ctx
)
{
  PrepareTree( ctx, 1, true );
}

/**
 * @brief Select the node to insert and its key in each tree.
 */
static void ScoreRbtreeReqPerfInsertAscending1_Setup(
  ScoreRbtreeValPerf_Context *ctx
)
{
  SetupInsert( ctx, true );
}

static void ScoreRbtreeReqPerfInsertAscending1_Setup_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfInsertAscending1_Setup( ctx );
}

/**
 * @brief Insert the node into each tree.
 */
static void ScoreRbtreeReqPerfInsertAscending1_Body(
  ScoreRbtreeValPerf_Context *ctx
)
{
  InsertAll( ctx );
}

static void ScoreRbtreeReqPerfInsertAscending1_Body_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfInsertAscending1_Body( ctx );
}

/**
 * @brief Extract the nodes. Discard samples interrupted by a clock tick.
 */
static bool ScoreRbtreeReqPerfInsertAscending1_Teardown(
  ScoreRbtreeValPerf_Context *ctx,
  uint32_t                    tic,
  uint32_t                    toc
)
{
  ExtractAll( ctx );

  return tic == toc;
}

static bool ScoreRbtreeReqPerfInsertAscending1_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  (void) delta;
  (void) retry;
  return ScoreRbtreeReqPerfInsertAscending1_Teardown( ctx, tic, toc );
}

/** @} */

/**
 * @defgroup ScoreRbtreeReqPerfInsertAscending13 \
 *   spec:/score/rbtree/req/perf-insert-ascending-13
 *
 * @{
 */

/**
 * @brief Prepare each tree with 13 nodes.
 */
static void ScoreRbtreeReqPerfInsertAscending13_Prepare(
  ScoreRbtreeValPerf_Context *ctx
)
{
  PrepareTree( ctx, 13, true );
}

/**
 * @brief Select the node to insert and its key in each tree.
 */
static void ScoreRbtreeReqPerfInsertAscending13_Setup(
  ScoreRbtreeValPerf_Context *ctx
)
{
  SetupInsert( ctx, true );
}

static void ScoreRbtreeReqPerfInsertAscending13_Setup_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfInsertAscending13_Setup( ctx );
}

/**
 * @brief Insert the node into each tree.
 */
static void ScoreRbtreeReqPerfInsertAscending13_Body(
  ScoreRbtreeValPerf_Context *ctx
)
{
  InsertAll( ctx );
}

static void ScoreRbtreeReqPerfInsertAscending13_Body_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfInsertAscending13_Body( ctx );
}

/**
 * @brief Extract the nodes. Discard samples interrupted by a clock tick.
 */
static bool ScoreRbtreeReqPerfInsertAscending13_Teardown(
  ScoreRbtreeValPerf_Context *ctx,
  uint32_t                    tic,
  uint32_t                    toc
)
{
  ExtractAll( ctx );

  return tic == toc;
}

static bool ScoreRbtreeReqPerfInsertAscending13_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  (void) delta;
  (void) retry;
  return ScoreRbtreeReqPerfInsertAscending13_Teardown( ctx, tic, toc );
}

/** @} */

/**
 * @defgroup ScoreRbtreeReqPerfInsertAscending2 \
 *   spec:/score/rbtree/req/perf-insert-ascending-2
 *
 * @{
 */

/**
 * @brief Prepare each tree with 2 nodes.
 */
static void ScoreRbtreeReqPerfInsertAscending2_Prepare(
  ScoreRbtreeValPerf_Context *ctx
)
{
  PrepareTree( ctx, 2, true );
}

/**
 * @brief Select the node to insert and its key in each tree.
 */
static void ScoreRbtreeReqPerfInsertAscending2_Setup(
  ScoreRbtreeValPerf_Context *ctx
)
{
  SetupInsert( ctx, true );
}

static void ScoreRbtreeReqPerfInsertAscending2_Setup_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfInsertAscending2_Setup( ctx );
}

/**
 * @brief Insert the node into each tree.
 */
static void ScoreRbtreeReqPerfInsertAscending2_Body(
  ScoreRbtreeValPerf_Context *ctx
)
{
  InsertAll( ctx );
}

static void ScoreRbtreeReqPerfInsertAscending2_Body_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfInsertAscending2_Body( ctx );
}

/**
 * @brief Extract the nodes. Discard samples interrupted by a clock tick.
 */
static bool ScoreRbtreeReqPerfInsertAscending2_Teardown(
  ScoreRbtreeValPerf_Context *ctx,
  uint32_t                    tic,
  uint32_t                    toc
)
{
  ExtractAll( ctx );

  return tic == toc;
}

static bool ScoreRbtreeReqPerfInsertAscending2_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  (void) delta;
  (void) retry;
  return ScoreRbtreeReqPerfInsertAscending2_Teardown( ctx, tic, toc );
}

/** @} */

/**
 * @defgroup ScoreRbtreeReqPerfInsertAscending233 \
 *   spec:/score/rbtree/req/perf-insert-ascending-233
 *
 * @{
 */

/**
 * @brief Prepare each tree with 233 nodes.
 */
static void ScoreRbtreeReqPerfInsertAscending233_Prepare(
  ScoreRbtreeValPerf_Context *ctx
)
{
  PrepareTree( ctx, 233, true );
}

/**
 * @brief Select the node to insert and its key in each tree.
 */
static void ScoreRbtreeReqPerfInsertAscending233_Setup(
  ScoreRbtreeValPerf_Context *ctx
)
{
  SetupInsert( ctx, true );
}

static void ScoreRbtreeReqPerfInsertAscending233_Setup_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfInsertAscending233_Setup( ctx );
}

/**
 * @brief Insert the node into each tree.
 */
static void ScoreRbtreeReqPerfInsertAscending233_Body(
  ScoreRbtreeValPerf_Context *ctx
)
{
  InsertAll( ctx );
}

static void ScoreRbtreeReqPerfInsertAscending233_Body_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfInsertAscending233_Body( ctx );
}

/**
 * @brief Extract the nodes. Discard samples interrupted by a clock tick.
 */
static bool ScoreRbtreeReqPerfInsertAscending233_Teardown(
  ScoreRbtreeValPerf_Context *ctx,
  uint32_t                    tic,
  uint32_t                    toc
)
{
  ExtractAll( ctx );

  return tic == toc;
}

static bool ScoreRbtreeReqPerfInsertAscending233_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  (void) delta;
  (void) retry;
  return ScoreRbtreeReqPerfInsertAscending233_Teardown( ctx, tic, toc );
}

/** @} */

/**
 * @defgroup ScoreRbtreeReqPerfInsertAscending3 \
 *   spec:/score/rbtree/req/perf-insert-ascending-3
 *
 * @{
 */

/**
 * @brief Prepare each tree with 3 nodes.
 */
static void ScoreRbtreeReqPerfInsertAscending3_Prepare(
  ScoreRbtreeValPerf_Context *ctx
)
{
  PrepareTree( ctx, 3, true );
}

/**
 * @brief Select the node to insert and its key in each tree.
 */
static void ScoreRbtreeReqPerfInsertAscending3_Setup(
  ScoreRbtreeValPerf_Context *ctx
)
{
  SetupInsert( ctx, true );
}

static void ScoreRbtreeReqPerfInsertAscending3_Setup_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfInsertAscending3_Setup( ctx );
}

/**
 * @brief Insert the node into each tree.
 */
static void ScoreRbtreeReqPerfInsertAscending3_Body(
  ScoreRbtreeValPerf_Context *ctx
)
{
  InsertAll( ctx );
}

static void ScoreRbtreeReqPerfInsertAscending3_Body_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfInsertAscending3_Body( ctx );
}

/**
 * @brief Extract the nodes. Discard samples interrupted by a clock tick.
 */
static bool ScoreRbtreeReqPerfInsertAscending3_Teardown(
  ScoreRbtreeValPerf_Context *ctx,
  uint32_t                    tic,
  uint32_t                    toc
)
{
  ExtractAll( ctx );

  return tic == toc;
}

static bool ScoreRbtreeReqPerfInsertAscending3_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  (void) delta;
  (void) retry;
  return ScoreRbtreeReqPerfInsertAscending3_Teardown( ctx, tic, toc );
}

/** @} */

/**
 * @defgroup ScoreRbtreeReqPerfInsertAscending34 \
 *   spec:/score/rbtree/req/perf-insert-ascending-34
 *
 * @{
 */

/**
 * @brief Prepare each tree with 34 nodes.
 */
static void ScoreRbtreeReqPerfInsertAscending34_Prepare(
  ScoreRbtreeValPerf_Context *ctx
)
{
  PrepareTree( ctx, 34, true );
}

/**
 * @brief Select the node to insert and its key in each tree.
 */
static void ScoreRbtreeReqPerfInsertAscending34_Setup(
  ScoreRbtreeValPerf_Context *ctx
)
{
  SetupInsert( ctx, true );
}

static void ScoreRbtreeReqPerfInsertAscending34_Setup_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfInsertAscending34_Setup( ctx );
}

/**
 * @brief Insert the node into each tree.
 */
static void ScoreRbtreeReqPerfInsertAscending34_Body(
  ScoreRbtreeValPerf_Context *ctx
)
{
  InsertAll( ctx );
}

static void ScoreRbtreeReqPerfInsertAscending34_Body_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfInsertAscending34_Body( ctx );
}

/**
 * @brief Extract the nodes. Discard samples interrupted by a clock tick.
 */
static bool ScoreRbtreeReqPerfInsertAscending34_Teardown(
  ScoreRbtreeValPerf_Context *ctx,
  uint32_t                    tic,
  uint32_t                    toc
)
{
  ExtractAll( ctx );

  return tic == toc;
}

static bool ScoreRbtreeReqPerfInsertAscending34_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  (void) delta;
  (void) retry;
  return ScoreRbtreeReqPerfInsertAscending34_Teardown( ctx, tic, toc );
}

/** @} */

/**
 * @defgroup ScoreRbtreeReqPerfInsertAscending5 \
 *   spec:/score/rbtree/req/perf-insert-ascending-5
 *
 * @{
 */

/**
 * @brief Prepare each tree with 5 nodes.
 */
static void ScoreRbtreeReqPerfInsertAscending5_Prepare(
  ScoreRbtreeValPerf_Context *ctx
)
{
  PrepareTree( ctx, 5, true );
}

/**
 * @brief Select the node to insert and its key in each tree.
 */
static void ScoreRbtreeReqPerfInsertAscending5_Setup(
  ScoreRbtreeValPerf_Context *ctx
)
{
  SetupInsert( ctx, true );
}

static void ScoreRbtreeReqPerfInsertAscending5_Setup_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfInsertAscending5_Setup( ctx );
}

/**
 * @brief Insert the node into each tree.
 */
static void ScoreRbtreeReqPerfInsertAscending5_Body(
  ScoreRbtreeValPerf_Context *ctx
)
{
  InsertAll( ctx );
}

static void ScoreRbtreeReqPerfInsertAscending5_Body_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfInsertAscending5_Body( ctx );
}

/**
 * @brief Extract the nodes. Discard samples interrupted by a clock tick.
 */
static bool ScoreRbtreeReqPerfInsertAscending5_Teardown(
  ScoreRbtreeValPerf_Context *ctx,
  uint32_t                    tic,
  uint32_t                    toc
)
{
  ExtractAll( ctx );

  return tic == toc;
}

static bool ScoreRbtreeReqPerfInsertAscending5_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  (void) delta;
  (void) retry;
  return ScoreRbtreeReqPerfInsertAscending5_Teardown( ctx, tic, toc );
}

/** @} */

/**
 * @defgroup ScoreRbtreeReqPerfInsertAscending89 \
 *   spec:/score/rbtree/req/perf-insert-ascending-89
 *
 * @{
 */

/**
 * @brief Prepare each tree with 89 nodes.
 */
static void ScoreRbtreeReqPerfInsertAscending89_Prepare(
  ScoreRbtreeValPerf_Context *ctx
)
{
  PrepareTree( ctx, 89, true );
}

/**
 * @brief Select the node to insert and its key in each tree.
 */
static void ScoreRbtreeReqPerfInsertAscending89_Setup(
  ScoreRbtreeValPerf_Context *ctx
)
{
  SetupInsert( ctx, true );
}

static void ScoreRbtreeReqPerfInsertAscending89_Setup_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfInsertAscending89_Setup( ctx );
}

/**
 * @brief Insert the node into each tree.
 */
static void ScoreRbtreeReqPerfInsertAscending89_Body(
  ScoreRbtreeValPerf_Context *ctx
)
{
  InsertAll( ctx );
}

static void ScoreRbtreeReqPerfInsertAscending89_Body_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfInsertAscending89_Body( ctx );
}

/**
 * @brief Extract the nodes. Discard samples interrupted by a clock tick.
 */
static bool ScoreRbtreeReqPerfInsertAscending89_Teardown(
  ScoreRbtreeValPerf_Context *ctx,
  uint32_t                    tic,
  uint32_t                    toc
)
{
  ExtractAll( ctx );

  return tic == toc;
}

static bool ScoreRbtreeReqPerfInsertAscending89_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  (void) delta;
  (void) retry;
  return ScoreRbtreeReqPerfInsertAscending89_Teardown( ctx, tic, toc );
}

/** @} */

/**
 * @defgroup ScoreRbtreeReqPerfInsertRandom1 \
 *   spec:/score/rbtree/req/perf-insert-random-1
 *
 * @{
 */

/**
 * @brief Prepare each tree with 1 nodes.
 */
static void ScoreRbtreeReqPerfInsertRandom1_Prepare(
  ScoreRbtreeValPerf_Context *ctx
)
{
  PrepareTree( ctx, 1, false );
}

/**
 * @brief Select the node to insert and its key in each tree.
 */
static void ScoreRbtreeReqPerfInsertRandom1_Setup(
  ScoreRbtreeValPerf_Context *ctx
)
{
  SetupInsert( ctx, false );
}

static void ScoreRbtreeReqPerfInsertRandom1_Setup_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfInsertRandom1_Setup( ctx );
}

/**
 * @brief Insert the node into each tree.
 */
static void ScoreRbtreeReqPerfInsertRandom1_Body(
  ScoreRbtreeValPerf_Context *ctx
)
{
  InsertAll( ctx );
}

static void ScoreRbtreeReqPerfInsertRandom1_Body_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfInsertRandom1_Body( ctx );
}

/**
 * @brief Extract the nodes. Discard samples interrupted by a clock tick.
 */
static bool ScoreRbtreeReqPerfInsertRandom1_Teardown(
  ScoreRbtreeValPerf_Context *ctx,
  uint32_t                    tic,
  uint32_t                    toc
)
{
  ExtractAll( ctx );

  return tic == toc;
}

static bool ScoreRbtreeReqPerfInsertRandom1_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  (void) delta;
  (void) retry;
  return ScoreRbtreeReqPerfInsertRandom1_Teardown( ctx, tic, toc );
}

/** @} */

/**
 * @defgroup ScoreRbtreeReqPerfInsertRandom13 \
 *   spec:/score/rbtree/req/perf-insert-random-13
 *
 * @{
 */

/**
 * @brief Prepare each tree with 13 nodes.
 */
static void ScoreRbtreeReqPerfInsertRandom13_Prepare(
  ScoreRbtreeValPerf_Context *ctx
)
{
  PrepareTree( ctx, 13, false );
}

/**
 * @brief Select the node to insert and its key in each tree.
 */
static void ScoreRbtreeReqPerfInsertRandom13_Setup(
  ScoreRbtreeValPerf_Context *ctx
)
{
  SetupInsert( ctx, false );
}

static void ScoreRbtreeReqPerfInsertRandom13_Setup_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfInsertRandom13_Setup( ctx );
}

/**
 * @brief Insert the node into each tree.
 */
static void ScoreRbtreeReqPerfInsertRandom13_Body(
  ScoreRbtreeValPerf_Context *ctx
)
{
  InsertAll( ctx );
}

static void ScoreRbtreeReqPerfInsertRandom13_Body_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfInsertRandom13_Body( ctx );
}

/**
 * @brief Extract the nodes. Discard samples interrupted by a clock tick.
 */
static bool ScoreRbtreeReqPerfInsertRandom13_Teardown(
  ScoreRbtreeValPerf_Context *ctx,
  uint32_t                    tic,
  uint32_t                    toc
)
{
  ExtractAll( ctx );

  return tic == toc;
}

static bool ScoreRbtreeReqPerfInsertRandom13_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  (void) delta;
  (void) retry;
  return ScoreRbtreeReqPerfInsertRandom13_Teardown( ctx, tic, toc );
}

/** @} */

/**
 * @defgroup ScoreRbtreeReqPerfInsertRandom2 \
 *   spec:/score/rbtree/req/perf-insert-random-2
 *
 * @{
 */

/**
 * @brief Prepare each tree with 2 nodes.
 */
static void ScoreRbtreeReqPerfInsertRandom2_Prepare(
  ScoreRbtreeValPerf_Context *ctx
)
{
  PrepareTree( ctx, 2, false );
}

/**
 * @brief Select the node to insert and its key in each tree.
 */
static void ScoreRbtreeReqPerfInsertRandom2_Setup(
  ScoreRbtreeValPerf_Context *ctx
)
{
  SetupInsert( ctx, false );
}

static void ScoreRbtreeReqPerfInsertRandom2_Setup_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfInsertRandom2_Setup( ctx );
}

/**
 * @brief Insert the node into each tree.
 */
static void ScoreRbtreeReqPerfInsertRandom2_Body(
  ScoreRbtreeValPerf_Context *ctx
)
{
  InsertAll( ctx );
}

static void ScoreRbtreeReqPerfInsertRandom2_Body_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfInsertRandom2_Body( ctx );
}

/**
 * @brief Extract the nodes. Discard samples interrupted by a clock tick.
 */
static bool ScoreRbtreeReqPerfInsertRandom2_Teardown(
  ScoreRbtreeValPerf_Context *ctx,
  uint32_t                    tic,
  uint32_t                    toc
)
{
  ExtractAll( ctx );

  return tic == toc;
}

static bool ScoreRbtreeReqPerfInsertRandom2_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  (void) delta;
  (void) retry;
  return ScoreRbtreeReqPerfInsertRandom2_Teardown( ctx, tic, toc );
}

/** @} */

/**
 * @defgroup ScoreRbtreeReqPerfInsertRandom233 \
 *   spec:/score/rbtree/req/perf-insert-random-233
 *
 * @{
 */

/**
 * @brief Prepare each tree with 233 nodes.
 */
static void ScoreRbtreeReqPerfInsertRandom233_Prepare(
  ScoreRbtreeValPerf_Context *ctx
)
{
  PrepareTree( ctx, 233, false );
}

/**
 * @brief Select the node to insert and its key in each tree.
 */
static void ScoreRbtreeReqPerfInsertRandom233_Setup(
  ScoreRbtreeValPerf_Context *ctx
)
{
  SetupInsert( ctx, false );
}

static void ScoreRbtreeReqPerfInsertRandom233_Setup_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfInsertRandom233_Setup( ctx );
}

/**
 * @brief Insert the node into each tree.
 */
static void ScoreRbtreeReqPerfInsertRandom233_Body(
  ScoreRbtreeValPerf_Context *ctx
)
{
  InsertAll( ctx );
}

static void ScoreRbtreeReqPerfInsertRandom233_Body_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfInsertRandom233_Body( ctx );
}

/**
 * @brief Extract the nodes. Discard samples interrupted by a clock tick.
 */
static bool ScoreRbtreeReqPerfInsertRandom233_Teardown(
  ScoreRbtreeValPerf_Context *ctx,
  uint32_t                    tic,
  uint32_t                    toc
)
{
  ExtractAll( ctx );

  return tic == toc;
}

static bool ScoreRbtreeReqPerfInsertRandom233_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  (void) delta;
  (void) retry;
  return ScoreRbtreeReqPerfInsertRandom233_Teardown( ctx, tic, toc );
}

/** @} */

/**
 * @defgroup ScoreRbtreeReqPerfInsertRandom3 \
 *   spec:/score/rbtree/req/perf-insert-random-3
 *
 * @{
 */

/**
 * @brief Prepare each tree with 3 nodes.
 */
static void ScoreRbtreeReqPerfInsertRandom3_Prepare(
  ScoreRbtreeValPerf_Context *ctx
)
{
  PrepareTree( ctx, 3, false );
}

/**
 * @brief Select the node to insert and its key in each tree.
 */
static void ScoreRbtreeReqPerfInsertRandom3_Setup(
  ScoreRbtreeValPerf_Context *ctx
)
{
  SetupInsert( ctx, false );
}

static void ScoreRbtreeReqPerfInsertRandom3_Setup_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfInsertRandom3_Setup( ctx );
}

/**
 * @brief Insert the node into each tree.
 */
static void ScoreRbtreeReqPerfInsertRandom3_Body(
  ScoreRbtreeValPerf_Context *ctx
)
{
  InsertAll( ctx );
}

static void ScoreRbtreeReqPerfInsertRandom3_Body_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfInsertRandom3_Body( ctx );
}

/**
 * @brief Extract the nodes. Discard samples interrupted by a clock tick.
 */
static bool ScoreRbtreeReqPerfInsertRandom3_Teardown(
  ScoreRbtreeValPerf_Context *ctx,
  uint32_t                    tic,
  uint32_t                    toc
)
{
  ExtractAll( ctx );

  return tic == toc;
}

static bool ScoreRbtreeReqPerfInsertRandom3_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  (void) delta;
  (void) retry;
  return ScoreRbtreeReqPerfInsertRandom3_Teardown( ctx, tic, toc );
}

/** @} */

/**
 * @defgroup ScoreRbtreeReqPerfInsertRandom34 \
 *   spec:/score/rbtree/req/perf-insert-random-34
 *
 * @{
 */

/**
 * @brief Prepare each tree with 34 nodes.
 */
static void ScoreRbtreeReqPerfInsertRandom34_Prepare(
  ScoreRbtreeValPerf_Context *ctx
)
{
  PrepareTree( ctx, 34, false );
}

/**
 * @brief Select the node to insert and its key in each tree.
 */
static void ScoreRbtreeReqPerfInsertRandom34_Setup(
  ScoreRbtreeValPerf_Context *ctx
)
{
  SetupInsert( ctx, false );
}

static void ScoreRbtreeReqPerfInsertRandom34_Setup_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfInsertRandom34_Setup( ctx );
}

/**
 * @brief Insert the node into each tree.
 */
static void ScoreRbtreeReqPerfInsertRandom34_Body(
  ScoreRbtreeValPerf_Context *ctx
)
{
  InsertAll( ctx );
}

static void ScoreRbtreeReqPerfInsertRandom34_Body_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfInsertRandom34_Body( ctx );
}

/**
 * @brief Extract the nodes. Discard samples interrupted by a clock tick.
 */
static bool ScoreRbtreeReqPerfInsertRandom34_Teardown(
  ScoreRbtreeValPerf_Context *ctx,
  uint32_t                    tic,
  uint32_t                    toc
)
{
  ExtractAll( ctx );

  return tic == toc;
}

static bool ScoreRbtreeReqPerfInsertRandom34_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  (void) delta;
  (void) retry;
  return ScoreRbtreeReqPerfInsertRandom34_Teardown( ctx, tic, toc );
}

/** @} */

/**
 * @defgroup ScoreRbtreeReqPerfInsertRandom5 \
 *   spec:/score/rbtree/req/perf-insert-random-5
 *
 * @{
 */

/**
 * @brief Prepare each tree with 5 nodes.
 */
static void ScoreRbtreeReqPerfInsertRandom5_Prepare(
  ScoreRbtreeValPerf_Context *ctx
)
{
  PrepareTree( ctx, 5, false );
}

/**
 * @brief Select the node to insert and its key in each tree.
 */
static void ScoreRbtreeReqPerfInsertRandom5_Setup(
  ScoreRbtreeValPerf_Context *ctx
)
{
  SetupInsert( ctx, false );
}

static void ScoreRbtreeReqPerfInsertRandom5_Setup_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfInsertRandom5_Setup( ctx );
}

/**
 * @brief Insert the node into each tree.
 */
static void ScoreRbtreeReqPerfInsertRandom5_Body(
  ScoreRbtreeValPerf_Context *ctx
)
{
  InsertAll( ctx );
}

static void ScoreRbtreeReqPerfInsertRandom5_Body_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfInsertRandom5_Body( ctx );
}

/**
 * @brief Extract the nodes. Discard samples interrupted by a clock tick.
 */
static bool ScoreRbtreeReqPerfInsertRandom5_Teardown(
  ScoreRbtreeValPerf_Context *ctx,
  uint32_t                    tic,
  uint32_t                    toc
)
{
  ExtractAll( ctx );

  return tic == toc;
}

static bool ScoreRbtreeReqPerfInsertRandom5_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  (void) delta;
  (void) retry;
  return ScoreRbtreeReqPerfInsertRandom5_Teardown( ctx, tic, toc );
}

/** @} */

/**
 * @defgroup ScoreRbtreeReqPerfInsertRandom89 \
 *   spec:/score/rbtree/req/perf-insert-random-89
 *
 * @{
 */

/**
 * @brief Prepare each tree with 89 nodes.
 */
static void ScoreRbtreeReqPerfInsertRandom89_Prepare(
  ScoreRbtreeValPerf_Context *ctx
)
{
  PrepareTree( ctx, 89, false );
}

/**
 * @brief Select the node to insert and its key in each tree.
 */
static void ScoreRbtreeReqPerfInsertRandom89_Setup(
  ScoreRbtreeValPerf_Context *ctx
)
{
  SetupInsert( ctx, false );
}

static void ScoreRbtreeReqPerfInsertRandom89_Setup_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfInsertRandom89_Setup( ctx );
}

/**
 * @brief Insert the node into each tree.
 */
static void ScoreRbtreeReqPerfInsertRandom89_Body(
  ScoreRbtreeValPerf_Context *ctx
)
{
  InsertAll( ctx );
}

static void ScoreRbtreeReqPerfInsertRandom89_Body_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfInsertRandom89_Body( ctx );
}

/**
 * @brief Extract the nodes. Discard samples interrupted by a clock tick.
 */
static bool ScoreRbtreeReqPerfInsertRandom89_Teardown(
  ScoreRbtreeValPerf_Context *ctx,
  uint32_t                    tic,
  uint32_t                    toc
)
{
  ExtractAll( ctx );

  return tic == toc;
}

static bool ScoreRbtreeReqPerfInsertRandom89_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  (void) delta;
  (void) retry;
  return ScoreRbtreeReqPerfInsertRandom89_Teardown( ctx, tic, toc );
}

/** @} */

/**
 * @defgroup ScoreRbtreeReqPerfMinimum1 spec:/score/rbtree/req/perf-minimum-1
 *
 * @{
 */

/**
 * @brief Prepare each tree with 1 nodes.
 */
static void ScoreRbtreeReqPerfMinimum1_Prepare(
  ScoreRbtreeValPerf_Context *ctx
)
{
  PrepareTree( ctx, 1, false );
}

/**
 * @brief Get the minimum node of each tree.
 */
static void ScoreRbtreeReqPerfMinimum1_Body( ScoreRbtreeValPerf_Context *ctx )
{
  MinimumAll( ctx );
}

static void ScoreRbtreeReqPerfMinimum1_Body_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfMinimum1_Body( ctx );
}

/**
 * @brief Discard samples interrupted by a clock tick.
 */
static bool ScoreRbtreeReqPerfMinimum1_Teardown( uint32_t tic, uint32_t toc )
{
  return tic == toc;
}

static bool ScoreRbtreeReqPerfMinimum1_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  (void) arg;
  (void) delta;
  (void) retry;
  return ScoreRbtreeReqPerfMinimum1_Teardown( tic, toc );
}

/** @} */

/**
 * @defgroup ScoreRbtreeReqPerfMinimum13 spec:/score/rbtree/req/perf-minimum-13
 *
 * @{
 */

/**
 * @brief Prepare each tree with 13 nodes.
 */
static void ScoreRbtreeReqPerfMinimum13_Prepare(
  ScoreRbtreeValPerf_Context *ctx
)
{
  PrepareTree( ctx, 13, false );
}

/**
 * @brief Get the minimum node of each tree.
 */
static void ScoreRbtreeReqPerfMinimum13_Body( ScoreRbtreeValPerf_Context *ctx )
{
  MinimumAll( ctx );
}

static void ScoreRbtreeReqPerfMinimum13_Body_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfMinimum13_Body( ctx );
}

/**
 * @brief Discard samples interrupted by a clock tick.
 */
static bool ScoreRbtreeReqPerfMinimum13_Teardown( uint32_t tic, uint32_t toc )
{
  return tic == toc;
}

static bool ScoreRbtreeReqPerfMinimum13_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  (void) arg;
  (void) delta;
  (void) retry;
  return ScoreRbtreeReqPerfMinimum13_Teardown( tic, toc );
}

/** @} */

/**
 * @defgroup ScoreRbtreeReqPerfMinimum2 spec:/score/rbtree/req/perf-minimum-2
 *
 * @{
 */

/**
 * @brief Prepare each tree with 2 nodes.
 */
static void ScoreRbtreeReqPerfMinimum2_Prepare(
  ScoreRbtreeValPerf_Context *ctx
)
{
  PrepareTree( ctx, 2, false );
}

/**
 * @brief Get the minimum node of each tree.
 */
static void ScoreRbtreeReqPerfMinimum2_Body( ScoreRbtreeValPerf_Context *ctx )
{
  MinimumAll( ctx );
}

static void ScoreRbtreeReqPerfMinimum2_Body_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfMinimum2_Body( ctx );
}

/**
 * @brief Discard samples interrupted by a clock tick.
 */
static bool ScoreRbtreeReqPerfMinimum2_Teardown( uint32_t tic, uint32_t toc )
{
  return tic == toc;
}

static bool ScoreRbtreeReqPerfMinimum2_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  (void) arg;
  (void) delta;
  (void) retry;
  return ScoreRbtreeReqPerfMinimum2_Teardown( tic, toc );
}

/** @} */

/**
 * @defgroup ScoreRbtreeReqPerfMinimum233 \
 *   spec:/score/rbtree/req/perf-minimum-233
 *
 * @{
 */

/**
 * @brief Prepare each tree with 233 nodes.
 */
static void ScoreRbtreeReqPerfMinimum233_Prepare(
  ScoreRbtreeValPerf_Context *ctx
)
{
  PrepareTree( ctx, 233, false );
}

/**
 * @brief Get the minimum node of each tree.
 */
static void ScoreRbtreeReqPerfMinimum233_Body(
  ScoreRbtreeValPerf_Context *ctx
)
{
  MinimumAll( ctx );
}

static void ScoreRbtreeReqPerfMinimum233_Body_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfMinimum233_Body( ctx );
}

/**
 * @brief Discard samples interrupted by a clock tick.
 */
static bool ScoreRbtreeReqPerfMinimum233_Teardown( uint32_t tic, uint32_t toc )
{
  return tic == toc;
}

static bool ScoreRbtreeReqPerfMinimum233_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  (void) arg;
  (void) delta;
  (void) retry;
  return ScoreRbtreeReqPerfMinimum233_Teardown( tic, toc );
}

/** @} */

/**
 * @defgroup ScoreRbtreeReqPerfMinimum3 spec:/score/rbtree/req/perf-minimum-3
 *
 * @{
 */

/**
 * @brief Prepare each tree with 3 nodes.
 */
static void ScoreRbtreeReqPerfMinimum3_Prepare(
  ScoreRbtreeValPerf_Context *ctx
)
{
  PrepareTree( ctx, 3, false );
}

/**
 * @brief Get the minimum node of each tree.
 */
static void ScoreRbtreeReqPerfMinimum3_Body( ScoreRbtreeValPerf_Context *ctx )
{
  MinimumAll( ctx );
}

static void ScoreRbtreeReqPerfMinimum3_Body_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfMinimum3_Body( ctx );
}

/**
 * @brief Discard samples interrupted by a clock tick.
 */
static bool ScoreRbtreeReqPerfMinimum3_Teardown( uint32_t tic, uint32_t toc )
{
  return tic == toc;
}

static bool ScoreRbtreeReqPerfMinimum3_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  (void) arg;
  (void) delta;
  (void) retry;
  return ScoreRbtreeReqPerfMinimum3_Teardown( tic, toc );
}

/** @} */

/**
 * @defgroup ScoreRbtreeReqPerfMinimum34 spec:/score/rbtree/req/perf-minimum-34
 *
 * @{
 */

/**
 * @brief Prepare each tree with 34 nodes.
 */
static void ScoreRbtreeReqPerfMinimum34_Prepare(
  ScoreRbtreeValPerf_Context *ctx
)
{
  PrepareTree( ctx, 34, false );
}

/**
 * @brief Get the minimum node of each tree.
 */
static void ScoreRbtreeReqPerfMinimum34_Body( ScoreRbtreeValPerf_Context *ctx )
{
  MinimumAll( ctx );
}

static void ScoreRbtreeReqPerfMinimum34_Body_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfMinimum34_Body( ctx );
}

/**
 * @brief Discard samples interrupted by a clock tick.
 */
static bool ScoreRbtreeReqPerfMinimum34_Teardown( uint32_t tic, uint32_t toc )
{
  return tic == toc;
}

static bool ScoreRbtreeReqPerfMinimum34_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  (void) arg;
  (void) delta;
  (void) retry;
  return ScoreRbtreeReqPerfMinimum34_Teardown( tic, toc );
}

/** @} */

/**
 * @defgroup ScoreRbtreeReqPerfMinimum5 spec:/score/rbtree/req/perf-minimum-5
 *
 * @{
 */

/**
 * @brief Prepare each tree with 5 nodes.
 */
static void ScoreRbtreeReqPerfMinimum5_Prepare(
  ScoreRbtreeValPerf_Context *ctx
)
{
  PrepareTree( ctx, 5, false );
}

/**
 * @brief Get the minimum node of each tree.
 */
static void ScoreRbtreeReqPerfMinimum5_Body( ScoreRbtreeValPerf_Context *ctx )
{
  MinimumAll( ctx );
}

static void ScoreRbtreeReqPerfMinimum5_Body_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfMinimum5_Body( ctx );
}

/**
 * @brief Discard samples interrupted by a clock tick.
 */
static bool ScoreRbtreeReqPerfMinimum5_Teardown( uint32_t tic, uint32_t toc )
{
  return tic == toc;
}

static bool ScoreRbtreeReqPerfMinimum5_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  (void) arg;
  (void) delta;
  (void) retry;
  return ScoreRbtreeReqPerfMinimum5_Teardown( tic, toc );
}

/** @} */

/**
 * @defgroup ScoreRbtreeReqPerfMinimum89 spec:/score/rbtree/req/perf-minimum-89
 *
 * @{
 */

/**
 * @brief Prepare each tree with 89 nodes.
 */
static void ScoreRbtreeReqPerfMinimum89_Prepare(
  ScoreRbtreeValPerf_Context *ctx
)
{
  PrepareTree( ctx, 89, false );
}

/**
 * @brief Get the minimum node of each tree.
 */
static void ScoreRbtreeReqPerfMinimum89_Body( ScoreRbtreeValPerf_Context *ctx )
{
  MinimumAll( ctx );
}

static void ScoreRbtreeReqPerfMinimum89_Body_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfMinimum89_Body( ctx );
}

/**
 * @brief Discard samples interrupted by a clock tick.
 */
static bool ScoreRbtreeReqPerfMinimum89_Teardown( uint32_t tic, uint32_t toc )
{
  return tic == toc;
}

static bool ScoreRbtreeReqPerfMinimum89_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  (void) arg;
  (void) delta;
  (void) retry;
  return ScoreRbtreeReqPerfMinimum89_Teardown( tic, toc );
}

/** @} */

#if defined( RTEMS_SMP )
/**
 * @defgroup ScoreRbtreeReqPerfReplace233 \
 *   spec:/score/rbtree/req/perf-replace-233
 *
 * @{
 */

/**
 * @brief Prepare each tree with 233 nodes.
 */
static void ScoreRbtreeReqPerfReplace233_Prepare(
  ScoreRbtreeValPerf_Context *ctx
)
{
  PrepareTree( ctx, 233, false );
}

/**
 * @brief Select the node to replace in each tree.
 */
static void ScoreRbtreeReqPerfReplace233_Setup(
  ScoreRbtreeValPerf_Context *ctx
)
{
  SetupReplace( ctx );
}

static void ScoreRbtreeReqPerfReplace233_Setup_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfReplace233_Setup( ctx );
}

/**
 * @brief Replace the node in each tree.
 */
static void ScoreRbtreeReqPerfReplace233_Body(
  ScoreRbtreeValPerf_Context *ctx
)
{
  ReplaceAll( ctx, true );
}

static void ScoreRbtreeReqPerfReplace233_Body_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfReplace233_Body( ctx );
}

/**
 * @brief Replace the spare node by the node in each tree. Discard samples
 *   interrupted by a clock tick.
 */
static bool ScoreRbtreeReqPerfReplace233_Teardown(
  ScoreRbtreeValPerf_Context *ctx,
  uint32_t                    tic,
  uint32_t                    toc
)
{
  ReplaceAll( ctx, false );

  return tic == toc;
}

static bool ScoreRbtreeReqPerfReplace233_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  (void) delta;
  (void) retry;
  return ScoreRbtreeReqPerfReplace233_Teardown( ctx, tic, toc );
}

/** @} */
#endif

/**
 * @defgroup ScoreRbtreeReqPerfSuccessor233 \
 *   spec:/score/rbtree/req/perf-successor-233
 *
 * @{
 */

/**
 * @brief Prepare each tree with 233 nodes.
 */
static void ScoreRbtreeReqPerfSuccessor233_Prepare(
  ScoreRbtreeValPerf_Context *ctx
)
{
  PrepareTree( ctx, 233, false );
}

/**
 * @brief Select the node of which to get the successor in each tree.
 */
static void ScoreRbtreeReqPerfSuccessor233_Setup(
  ScoreRbtreeValPerf_Context *ctx
)
{
  SetupRandomNode( ctx );
}

static void ScoreRbtreeReqPerfSuccessor233_Setup_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfSuccessor233_Setup( ctx );
}

/**
 * @brief Get the successor of the node in each tree.
 */
static void ScoreRbtreeReqPerfSuccessor233_Body(
  ScoreRbtreeValPerf_Context *ctx
)
{
  SuccessorAll( ctx );
}

static void ScoreRbtreeReqPerfSuccessor233_Body_Wrap( void *arg )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = arg;
  ScoreRbtreeReqPerfSuccessor233_Body( ctx );
}

/**
 * @brief Discard samples interrupted by a clock tick.
 */
static bool ScoreRbtreeReqPerfSuccessor233_Teardown(
  uint32_t tic,
  uint32_t toc
)
{
  return tic == toc;
}

static bool ScoreRbtreeReqPerfSuccessor233_Teardown_Wrap(
  void        *arg,
  T_ticks     *delta,
  uint32_t     tic,
  uint32_t     toc,
  unsigned int retry
)
{
  (void) arg;
  (void) delta;
  (void) retry;
  return ScoreRbtreeReqPerfSuccessor233_Teardown( tic, toc );
}

/** @} */

/**
 * @fn void T_case_body_ScoreRbtreeValPerf( void )
 */
T_TEST_CASE_FIXTURE( ScoreRbtreeValPerf, &ScoreRbtreeValPerf_Fixture )
{
  ScoreRbtreeValPerf_Context *ctx;

  ctx = T_fixture_context();

  ScoreRbtreeReqPerfExtractMinimum1_Prepare( ctx );
  ctx->request.name = "ScoreRbtreeReqPerfExtractMinimum1";
  ctx->request.setup = ScoreRbtreeReqPerfExtractMinimum1_Setup_Wrap;
  ctx->request.body = ScoreRbtreeReqPerfExtractMinimum1_Body_Wrap;
  ctx->request.teardown = ScoreRbtreeReqPerfExtractMinimum1_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  ScoreRbtreeReqPerfExtractMinimum13_Prepare( ctx );
  ctx->request.name = "ScoreRbtreeReqPerfExtractMinimum13";
  ctx->request.setup = ScoreRbtreeReqPerfExtractMinimum13_Setup_Wrap;
  ctx->request.body = ScoreRbtreeReqPerfExtractMinimum13_Body_Wrap;
  ctx->request.teardown = ScoreRbtreeReqPerfExtractMinimum13_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  ScoreRbtreeReqPerfExtractMinimum2_Prepare( ctx );
  ctx->request.name = "ScoreRbtreeReqPerfExtractMinimum2";
  ctx->request.setup = ScoreRbtreeReqPerfExtractMinimum2_Setup_Wrap;
  ctx->request.body = ScoreRbtreeReqPerfExtractMinimum2_Body_Wrap;
  ctx->request.teardown = ScoreRbtreeReqPerfExtractMinimum2_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  ScoreRbtreeReqPerfExtractMinimum233_Prepare( ctx );
  ctx->request.name = "ScoreRbtreeReqPerfExtractMinimum233";
  ctx->request.setup = ScoreRbtreeReqPerfExtractMinimum233_Setup_Wrap;
  ctx->request.body = ScoreRbtreeReqPerfExtractMinimum233_Body_Wrap;
  ctx->request.teardown = ScoreRbtreeReqPerfExtractMinimum233_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  ScoreRbtreeReqPerfExtractMinimum3_Prepare( ctx );
  ctx->request.name = "ScoreRbtreeReqPerfExtractMinimum3";
  ctx->request.setup = ScoreRbtreeReqPerfExtractMinimum3_Setup_Wrap;
  ctx->request.body = ScoreRbtreeReqPerfExtractMinimum3_Body_Wrap;
  ctx->request.teardown = ScoreRbtreeReqPerfExtractMinimum3_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  ScoreRbtreeReqPerfExtractMinimum34_Prepare( ctx );
  ctx->request.name = "ScoreRbtreeReqPerfExtractMinimum34";
  ctx->request.setup = ScoreRbtreeReqPerfExtractMinimum34_Setup_Wrap;
  ctx->request.body = ScoreRbtreeReqPerfExtractMinimum34_Body_Wrap;
  ctx->request.teardown = ScoreRbtreeReqPerfExtractMinimum34_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  ScoreRbtreeReqPerfExtractMinimum5_Prepare( ctx );
  ctx->request.name = "ScoreRbtreeReqPerfExtractMinimum5";
  ctx->request.setup = ScoreRbtreeReqPerfExtractMinimum5_Setup_Wrap;
  ctx->request.body = ScoreRbtreeReqPerfExtractMinimum5_Body_Wrap;
  ctx->request.teardown = ScoreRbtreeReqPerfExtractMinimum5_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  ScoreRbtreeReqPerfExtractMinimum89_Prepare( ctx );
  ctx->request.name = "ScoreRbtreeReqPerfExtractMinimum89";
  ctx->request.setup = ScoreRbtreeReqPerfExtractMinimum89_Setup_Wrap;
  ctx->request.body = ScoreRbtreeReqPerfExtractMinimum89_Body_Wrap;
  ctx->request.teardown = ScoreRbtreeReqPerfExtractMinimum89_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  ScoreRbtreeReqPerfExtractRandom1_Prepare( ctx );
  ctx->request.name = "ScoreRbtreeReqPerfExtractRandom1";
  ctx->request.setup = ScoreRbtreeReqPerfExtractRandom1_Setup_Wrap;
  ctx->request.body = ScoreRbtreeReqPerfExtractRandom1_Body_Wrap;
  ctx->request.teardown = ScoreRbtreeReqPerfExtractRandom1_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  ScoreRbtreeReqPerfExtractRandom13_Prepare( ctx );
  ctx->request.name = "ScoreRbtreeReqPerfExtractRandom13";
  ctx->request.setup = ScoreRbtreeReqPerfExtractRandom13_Setup_Wrap;
  ctx->request.body = ScoreRbtreeReqPerfExtractRandom13_Body_Wrap;
  ctx->request.teardown = ScoreRbtreeReqPerfExtractRandom13_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  ScoreRbtreeReqPerfExtractRandom2_Prepare( ctx );
  ctx->request.name = "ScoreRbtreeReqPerfExtractRandom2";
  ctx->request.setup = ScoreRbtreeReqPerfExtractRandom2_Setup_Wrap;
  ctx->request.body = ScoreRbtreeReqPerfExtractRandom2_Body_Wrap;
  ctx->request.teardown = ScoreRbtreeReqPerfExtractRandom2_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  ScoreRbtreeReqPerfExtractRandom233_Prepare( ctx );
  ctx->request.name = "ScoreRbtreeReqPerfExtractRandom233";
  ctx->request.setup = ScoreRbtreeReqPerfExtractRandom233_Setup_Wrap;
  ctx->request.body = ScoreRbtreeReqPerfExtractRandom233_Body_Wrap;
  ctx->request.teardown = ScoreRbtreeReqPerfExtractRandom233_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  ScoreRbtreeReqPerfExtractRandom3_Prepare( ctx );
  ctx->request.name = "ScoreRbtreeReqPerfExtractRandom3";
  ctx->request.setup = ScoreRbtreeReqPerfExtractRandom3_Setup_Wrap;
  ctx->request.body = ScoreRbtreeReqPerfExtractRandom3_Body_Wrap;
  ctx->request.teardown = ScoreRbtreeReqPerfExtractRandom3_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  ScoreRbtreeReqPerfExtractRandom34_Prepare( ctx );
  ctx->request.name = "ScoreRbtreeReqPerfExtractRandom34";
  ctx->request.setup = ScoreRbtreeReqPerfExtractRandom34_Setup_Wrap;
  ctx->request.body = ScoreRbtreeReqPerfExtractRandom34_Body_Wrap;
  ctx->request.teardown = ScoreRbtreeReqPerfExtractRandom34_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  ScoreRbtreeReqPerfExtractRandom5_Prepare( ctx );
  ctx->request.name = "ScoreRbtreeReqPerfExtractRandom5";
  ctx->request.setup = ScoreRbtreeReqPerfExtractRandom5_Setup_Wrap;
  ctx->request.body = ScoreRbtreeReqPerfExtractRandom5_Body_Wrap;
  ctx->request.teardown = ScoreRbtreeReqPerfExtractRandom5_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  ScoreRbtreeReqPerfExtractRandom89_Prepare( ctx );
  ctx->request.name = "ScoreRbtreeReqPerfExtractRandom89";
  ctx->request.setup = ScoreRbtreeReqPerfExtractRandom89_Setup_Wrap;
  ctx->request.body = ScoreRbtreeReqPerfExtractRandom89_Body_Wrap;
  ctx->request.teardown = ScoreRbtreeReqPerfExtractRandom89_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  ScoreRbtreeReqPerfInsertAscending1_Prepare( ctx );
  ctx->request.name = "ScoreRbtreeReqPerfInsertAscending1";
  ctx->request.setup = ScoreRbtreeReqPerfInsertAscending1_Setup_Wrap;
  ctx->request.body = ScoreRbtreeReqPerfInsertAscending1_Body_Wrap;
  ctx->request.teardown = ScoreRbtreeReqPerfInsertAscending1_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  ScoreRbtreeReqPerfInsertAscending13_Prepare( ctx );
  ctx->request.name = "ScoreRbtreeReqPerfInsertAscending13";
  ctx->request.setup = ScoreRbtreeReqPerfInsertAscending13_Setup_Wrap;
  ctx->request.body = ScoreRbtreeReqPerfInsertAscending13_Body_Wrap;
  ctx->request.teardown = ScoreRbtreeReqPerfInsertAscending13_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  ScoreRbtreeReqPerfInsertAscending2_Prepare( ctx );
  ctx->request.name = "ScoreRbtreeReqPerfInsertAscending2";
  ctx->request.setup = ScoreRbtreeReqPerfInsertAscending2_Setup_Wrap;
  ctx->request.body = ScoreRbtreeReqPerfInsertAscending2_Body_Wrap;
  ctx->request.teardown = ScoreRbtreeReqPerfInsertAscending2_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  ScoreRbtreeReqPerfInsertAscending233_Prepare( ctx );
  ctx->request.name = "ScoreRbtreeReqPerfInsertAscending233";
  ctx->request.setup = ScoreRbtreeReqPerfInsertAscending233_Setup_Wrap;
  ctx->request.body = ScoreRbtreeReqPerfInsertAscending233_Body_Wrap;
  ctx->request.teardown = ScoreRbtreeReqPerfInsertAscending233_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  ScoreRbtreeReqPerfInsertAscending3_Prepare( ctx );
  ctx->request.name = "ScoreRbtreeReqPerfInsertAscending3";
  ctx->request.setup = ScoreRbtreeReqPerfInsertAscending3_Setup_Wrap;
  ctx->request.body = ScoreRbtreeReqPerfInsertAscending3_Body_Wrap;
  ctx->request.teardown = ScoreRbtreeReqPerfInsertAscending3_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  ScoreRbtreeReqPerfInsertAscending34_Prepare( ctx );
  ctx->request.name = "ScoreRbtreeReqPerfInsertAscending34";
  ctx->request.setup = ScoreRbtreeReqPerfInsertAscending34_Setup_Wrap;
  ctx->request.body = ScoreRbtreeReqPerfInsertAscending34_Body_Wrap;
  ctx->request.teardown = ScoreRbtreeReqPerfInsertAscending34_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  ScoreRbtreeReqPerfInsertAscending5_Prepare( ctx );
  ctx->request.name = "ScoreRbtreeReqPerfInsertAscending5";
  ctx->request.setup = ScoreRbtreeReqPerfInsertAscending5_Setup_Wrap;
  ctx->request.body = ScoreRbtreeReqPerfInsertAscending5_Body_Wrap;
  ctx->request.teardown = ScoreRbtreeReqPerfInsertAscending5_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  ScoreRbtreeReqPerfInsertAscending89_Prepare( ctx );
  ctx->request.name = "ScoreRbtreeReqPerfInsertAscending89";
  ctx->request.setup = ScoreRbtreeReqPerfInsertAscending89_Setup_Wrap;
  ctx->request.body = ScoreRbtreeReqPerfInsertAscending89_Body_Wrap;
  ctx->request.teardown = ScoreRbtreeReqPerfInsertAscending89_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  ScoreRbtreeReqPerfInsertRandom1_Prepare( ctx );
  ctx->request.name = "ScoreRbtreeReqPerfInsertRandom1";
  ctx->request.setup = ScoreRbtreeReqPerfInsertRandom1_Setup_Wrap;
  ctx->request.body = ScoreRbtreeReqPerfInsertRandom1_Body_Wrap;
  ctx->request.teardown = ScoreRbtreeReqPerfInsertRandom1_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  ScoreRbtreeReqPerfInsertRandom13_Prepare( ctx );
  ctx->request.name = "ScoreRbtreeReqPerfInsertRandom13";
  ctx->request.setup = ScoreRbtreeReqPerfInsertRandom13_Setup_Wrap;
  ctx->request.body = ScoreRbtreeReqPerfInsertRandom13_Body_Wrap;
  ctx->request.teardown = ScoreRbtreeReqPerfInsertRandom13_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  ScoreRbtreeReqPerfInsertRandom2_Prepare( ctx );
  ctx->request.name = "ScoreRbtreeReqPerfInsertRandom2";
  ctx->request.setup = ScoreRbtreeReqPerfInsertRandom2_Setup_Wrap;
  ctx->request.body = ScoreRbtreeReqPerfInsertRandom2_Body_Wrap;
  ctx->request.teardown = ScoreRbtreeReqPerfInsertRandom2_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  ScoreRbtreeReqPerfInsertRandom233_Prepare( ctx );
  ctx->request.name = "ScoreRbtreeReqPerfInsertRandom233";
  ctx->request.setup = ScoreRbtreeReqPerfInsertRandom233_Setup_Wrap;
  ctx->request.body = ScoreRbtreeReqPerfInsertRandom233_Body_Wrap;
  ctx->request.teardown = ScoreRbtreeReqPerfInsertRandom233_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  ScoreRbtreeReqPerfInsertRandom3_Prepare( ctx );
  ctx->request.name = "ScoreRbtreeReqPerfInsertRandom3";
  ctx->request.setup = ScoreRbtreeReqPerfInsertRandom3_Setup_Wrap;
  ctx->request.body = ScoreRbtreeReqPerfInsertRandom3_Body_Wrap;
  ctx->request.teardown = ScoreRbtreeReqPerfInsertRandom3_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  ScoreRbtreeReqPerfInsertRandom34_Prepare( ctx );
  ctx->request.name = "ScoreRbtreeReqPerfInsertRandom34";
  ctx->request.setup = ScoreRbtreeReqPerfInsertRandom34_Setup_Wrap;
  ctx->request.body = ScoreRbtreeReqPerfInsertRandom34_Body_Wrap;
  ctx->request.teardown = ScoreRbtreeReqPerfInsertRandom34_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  ScoreRbtreeReqPerfInsertRandom5_Prepare( ctx );
  ctx->request.name = "ScoreRbtreeReqPerfInsertRandom5";
  ctx->request.setup = ScoreRbtreeReqPerfInsertRandom5_Setup_Wrap;
  ctx->request.body = ScoreRbtreeReqPerfInsertRandom5_Body_Wrap;
  ctx->request.teardown = ScoreRbtreeReqPerfInsertRandom5_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  ScoreRbtreeReqPerfInsertRandom89_Prepare( ctx );
  ctx->request.name = "ScoreRbtreeReqPerfInsertRandom89";
  ctx->request.setup = ScoreRbtreeReqPerfInsertRandom89_Setup_Wrap;
  ctx->request.body = ScoreRbtreeReqPerfInsertRandom89_Body_Wrap;
  ctx->request.teardown = ScoreRbtreeReqPerfInsertRandom89_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  ScoreRbtreeReqPerfMinimum1_Prepare( ctx );
  ctx->request.name = "ScoreRbtreeReqPerfMinimum1";
  ctx->request.setup = NULL;
  ctx->request.body = ScoreRbtreeReqPerfMinimum1_Body_Wrap;
  ctx->request.teardown = ScoreRbtreeReqPerfMinimum1_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  ScoreRbtreeReqPerfMinimum13_Prepare( ctx );
  ctx->request.name = "ScoreRbtreeReqPerfMinimum13";
  ctx->request.setup = NULL;
  ctx->request.body = ScoreRbtreeReqPerfMinimum13_Body_Wrap;
  ctx->request.teardown = ScoreRbtreeReqPerfMinimum13_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  ScoreRbtreeReqPerfMinimum2_Prepare( ctx );
  ctx->request.name = "ScoreRbtreeReqPerfMinimum2";
  ctx->request.setup = NULL;
  ctx->request.body = ScoreRbtreeReqPerfMinimum2_Body_Wrap;
  ctx->request.teardown = ScoreRbtreeReqPerfMinimum2_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  ScoreRbtreeReqPerfMinimum233_Prepare( ctx );
  ctx->request.name = "ScoreRbtreeReqPerfMinimum233";
  ctx->request.setup = NULL;
  ctx->request.body = ScoreRbtreeReqPerfMinimum233_Body_Wrap;
  ctx->request.teardown = ScoreRbtreeReqPerfMinimum233_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  ScoreRbtreeReqPerfMinimum3_Prepare( ctx );
  ctx->request.name = "ScoreRbtreeReqPerfMinimum3";
  ctx->request.setup = NULL;
  ctx->request.body = ScoreRbtreeReqPerfMinimum3_Body_Wrap;
  ctx->request.teardown = ScoreRbtreeReqPerfMinimum3_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  ScoreRbtreeReqPerfMinimum34_Prepare( ctx );
  ctx->request.name = "ScoreRbtreeReqPerfMinimum34";
  ctx->request.setup = NULL;
  ctx->request.body = ScoreRbtreeReqPerfMinimum34_Body_Wrap;
  ctx->request.teardown = ScoreRbtreeReqPerfMinimum34_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  ScoreRbtreeReqPerfMinimum5_Prepare( ctx );
  ctx->request.name = "ScoreRbtreeReqPerfMinimum5";
  ctx->request.setup = NULL;
  ctx->request.body = ScoreRbtreeReqPerfMinimum5_Body_Wrap;
  ctx->request.teardown = ScoreRbtreeReqPerfMinimum5_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  ScoreRbtreeReqPerfMinimum89_Prepare( ctx );
  ctx->request.name = "ScoreRbtreeReqPerfMinimum89";
  ctx->request.setup = NULL;
  ctx->request.body = ScoreRbtreeReqPerfMinimum89_Body_Wrap;
  ctx->request.teardown = ScoreRbtreeReqPerfMinimum89_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );

  #if defined( RTEMS_SMP )
  ScoreRbtreeReqPerfReplace233_Prepare( ctx );
  ctx->request.name = "ScoreRbtreeReqPerfReplace233";
  ctx->request.setup = ScoreRbtreeReqPerfReplace233_Setup_Wrap;
  ctx->request.body = ScoreRbtreeReqPerfReplace233_Body_Wrap;
  ctx->request.teardown = ScoreRbtreeReqPerfReplace233_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );
  #endif

  ScoreRbtreeReqPerfSuccessor233_Prepare( ctx );
  ctx->request.name = "ScoreRbtreeReqPerfSuccessor233";
  ctx->request.setup = ScoreRbtreeReqPerfSuccessor233_Setup_Wrap;
  ctx->request.body = ScoreRbtreeReqPerfSuccessor233_Body_Wrap;
  ctx->request.teardown = ScoreRbtreeReqPerfSuccessor233_Teardown_Wrap;
  T_measure_runtime( ctx->context, &ctx->request );
}

/** @} */
