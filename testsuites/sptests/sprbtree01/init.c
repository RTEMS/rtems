/*
 * Copyright (c) 2010 Gedare Bloom.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tmacros.h>
#include <rtems/rbtree.h>
#include <rtems/score/rbtreeimpl.h>

const char rtems_test_name[] = "SPRBTREE 1";

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);

static const int numbers[20] = {
  52, 99, 0, 85, 43, 44, 10, 60, 50, 19, 8, 68, 48, 57, 17, 67, 90, 12, 77, 71
};

static const int numbers_sorted[20] = {
  0, 8, 10, 12, 17, 19, 43, 44, 48, 50, 52, 57, 60, 67, 68, 71, 77, 85, 90, 99
};

typedef struct {
  int              id;
  int              key;
  rtems_rbtree_node Node;
} test_node;

static test_node node_array[100];

static rtems_rbtree_compare_result test_compare_function (
  const rtems_rbtree_node *n1,
  const rtems_rbtree_node *n2
)
{
  int key1 = RTEMS_CONTAINER_OF( n1, test_node, Node )->key;
  int key2 = RTEMS_CONTAINER_OF( n2, test_node, Node )->key;

  return key1 - key2;
}

static rtems_rbtree_node *rb_insert_unique(
  rtems_rbtree_control *rbtree,
  rtems_rbtree_node    *node
)
{
  return rtems_rbtree_insert( rbtree, node, test_compare_function, true );
}

static rtems_rbtree_node *rb_insert_multi(
  rtems_rbtree_control *rbtree,
  rtems_rbtree_node    *node
)
{
  return rtems_rbtree_insert( rbtree, node, test_compare_function, false );
}

static rtems_rbtree_node *rb_find_unique(
  rtems_rbtree_control *rbtree,
  rtems_rbtree_node    *node
)
{
  return rtems_rbtree_find( rbtree, node, test_compare_function, true );
}

static rtems_rbtree_node *rb_find_multi(
  rtems_rbtree_control *rbtree,
  rtems_rbtree_node    *node
)
{
  return rtems_rbtree_find( rbtree, node, test_compare_function, false );
}

/*
 * recursively checks tree. if the tree is built properly it should only
 * be a depth of 7 function calls for 100 entries in the tree.
 */
static int rb_assert ( rtems_rbtree_node *root )
{
  int lh, rh;

  if ( root == NULL )
    return 1;
  else {
    rtems_rbtree_node *ln = rtems_rbtree_left(root);
    rtems_rbtree_node *rn = rtems_rbtree_right(root);

    /* Consecutive red links */
    if ( root->color == RBT_RED ) {
      if ((ln && ln->color == RBT_RED)  || (rn && rn->color == RBT_RED)) {
        puts ( "Red violation" );
        return -1;
      }
    }

      lh = rb_assert ( ln );
      rh = rb_assert ( rn );

    /* Invalid binary search tree */
    if ( ( ln != NULL && test_compare_function(ln, root) > 0 )
        || ( rn != NULL && test_compare_function(rn, root) < 0 ) )
    {
      puts ( "Binary tree violation" );
      return -1;
    }

    /* Black height mismatch */
    if ( lh != -1 && rh != -1 && lh != rh ) {
      puts ( "Black violation" );
      return -1;
    }

    /* Only count black links */
    if ( lh != -1 && rh != -1 )
      return ( root->color == RBT_RED ) ? lh : lh + 1;
    else
      return -1;
  }
}

static test_node some_nodes[] = {
  { .key = 1 },
  { .key = 1 },
  { .key = 1 },
  { .key = 2 },
  { .key = 2 },
  { .key = 2 }
};

static void min_max_insert(
  rtems_rbtree_control *tree,
  rtems_rbtree_node    *node,
  rtems_rbtree_node    *min,
  rtems_rbtree_node    *max
)
{
  rb_insert_multi( tree, node );
  rtems_test_assert( rb_assert( tree->root ) != -1 );
  rtems_test_assert( tree->first[ 0 ] == min );
  rtems_test_assert( tree->first[ 1 ] == max );
}

static void min_max_extract(
  rtems_rbtree_control *tree,
  rtems_rbtree_node    *node,
  rtems_rbtree_node    *min,
  rtems_rbtree_node    *max
)
{
  rtems_rbtree_extract( tree, node );
  rtems_test_assert( rb_assert( tree->root ) != -1 );
  rtems_test_assert( tree->first[ 0 ] == min );
  rtems_test_assert( tree->first[ 1 ] == max );
}

static void test_rbtree_min_max(void)
{
  rtems_rbtree_control  tree;
  rtems_rbtree_node    *node;
  rtems_rbtree_node    *min;
  rtems_rbtree_node    *max;

  puts( "INIT - Verify min/max node updates" );

  rtems_rbtree_initialize_empty( &tree );
  rtems_test_assert( rb_assert( tree.root ) == 1 );

  node = &some_nodes[ 0 ].Node;
  min = node;
  max = node;
  min_max_insert( &tree, node, min, max );

  node = &some_nodes[ 1 ].Node;
  max = node;
  min_max_insert( &tree, node, min, max );

  node = &some_nodes[ 2 ].Node;
  max = node;
  min_max_insert( &tree, node, min, max );

  node = &some_nodes[ 3 ].Node;
  max = node;
  min_max_insert( &tree, node, min, max );

  node = &some_nodes[ 4 ].Node;
  max = node;
  min_max_insert( &tree, node, min, max );

  node = &some_nodes[ 5 ].Node;
  max = node;
  min_max_insert( &tree, node, min, max );

  node = &some_nodes[ 1 ].Node;
  min_max_extract( &tree, node, min, max );

  node = &some_nodes[ 4 ].Node;
  min_max_extract( &tree, node, min, max );

  node = &some_nodes[ 0 ].Node;
  min = &some_nodes[ 2 ].Node;
  min_max_extract( &tree, node, min, max );

  node = &some_nodes[ 5 ].Node;
  max = &some_nodes[ 3 ].Node;
  min_max_extract( &tree, node, min, max );

  node = &some_nodes[ 2 ].Node;
  min = &some_nodes[ 3 ].Node;
  min_max_extract( &tree, node, min, max );

  node = &some_nodes[ 3 ].Node;
  min = NULL;
  max = NULL;
  min_max_extract( &tree, node, min, max );
  rtems_test_assert( rtems_rbtree_is_empty( &tree ) );
}

#define TN( i ) &node_array[ i ].Node

typedef struct {
  int key;
  const rtems_rbtree_node *parent;
  const rtems_rbtree_node *left;
  const rtems_rbtree_node *right;
  RBTree_Color color;
} test_node_description;

static const test_node_description test_insert_tree_0[] = {
  { 52, NULL, NULL, NULL , RBT_BLACK }
};

static const test_node_description test_insert_tree_1[] = {
  { 52, NULL, NULL, TN( 1 ) , RBT_BLACK },
  { 99, TN( 0 ), NULL, NULL , RBT_RED }
};

static const test_node_description test_insert_tree_2[] = {
  { 0, TN( 0 ), NULL, NULL , RBT_RED },
  { 52, NULL, TN( 2 ), TN( 1 ) , RBT_BLACK },
  { 99, TN( 0 ), NULL, NULL , RBT_RED }
};

static const test_node_description test_insert_tree_3[] = {
  { 0, TN( 0 ), NULL, NULL , RBT_BLACK },
  { 52, NULL, TN( 2 ), TN( 1 ) , RBT_BLACK },
  { 85, TN( 1 ), NULL, NULL , RBT_RED },
  { 99, TN( 0 ), TN( 3 ), NULL , RBT_BLACK }
};

static const test_node_description test_insert_tree_4[] = {
  { 0, TN( 0 ), NULL, TN( 4 ) , RBT_BLACK },
  { 43, TN( 2 ), NULL, NULL , RBT_RED },
  { 52, NULL, TN( 2 ), TN( 1 ) , RBT_BLACK },
  { 85, TN( 1 ), NULL, NULL , RBT_RED },
  { 99, TN( 0 ), TN( 3 ), NULL , RBT_BLACK }
};

static const test_node_description test_insert_tree_5[] = {
  { 0, TN( 4 ), NULL, NULL , RBT_RED },
  { 43, TN( 0 ), TN( 2 ), TN( 5 ) , RBT_BLACK },
  { 44, TN( 4 ), NULL, NULL , RBT_RED },
  { 52, NULL, TN( 4 ), TN( 1 ) , RBT_BLACK },
  { 85, TN( 1 ), NULL, NULL , RBT_RED },
  { 99, TN( 0 ), TN( 3 ), NULL , RBT_BLACK }
};

static const test_node_description test_insert_tree_6[] = {
  { 0, TN( 4 ), NULL, TN( 6 ) , RBT_BLACK },
  { 10, TN( 2 ), NULL, NULL , RBT_RED },
  { 43, TN( 0 ), TN( 2 ), TN( 5 ) , RBT_RED },
  { 44, TN( 4 ), NULL, NULL , RBT_BLACK },
  { 52, NULL, TN( 4 ), TN( 1 ) , RBT_BLACK },
  { 85, TN( 1 ), NULL, NULL , RBT_RED },
  { 99, TN( 0 ), TN( 3 ), NULL , RBT_BLACK }
};

static const test_node_description test_insert_tree_7[] = {
  { 0, TN( 4 ), NULL, TN( 6 ) , RBT_BLACK },
  { 10, TN( 2 ), NULL, NULL , RBT_RED },
  { 43, TN( 0 ), TN( 2 ), TN( 5 ) , RBT_RED },
  { 44, TN( 4 ), NULL, NULL , RBT_BLACK },
  { 52, NULL, TN( 4 ), TN( 3 ) , RBT_BLACK },
  { 60, TN( 3 ), NULL, NULL , RBT_RED },
  { 85, TN( 0 ), TN( 7 ), TN( 1 ) , RBT_BLACK },
  { 99, TN( 3 ), NULL, NULL , RBT_RED }
};

static const test_node_description test_insert_tree_8[] = {
  { 0, TN( 4 ), NULL, TN( 6 ) , RBT_BLACK },
  { 10, TN( 2 ), NULL, NULL , RBT_RED },
  { 43, TN( 0 ), TN( 2 ), TN( 5 ) , RBT_RED },
  { 44, TN( 4 ), NULL, TN( 8 ) , RBT_BLACK },
  { 50, TN( 5 ), NULL, NULL , RBT_RED },
  { 52, NULL, TN( 4 ), TN( 3 ) , RBT_BLACK },
  { 60, TN( 3 ), NULL, NULL , RBT_RED },
  { 85, TN( 0 ), TN( 7 ), TN( 1 ) , RBT_BLACK },
  { 99, TN( 3 ), NULL, NULL , RBT_RED }
};

static const test_node_description test_insert_tree_9[] = {
  { 0, TN( 6 ), NULL, NULL , RBT_RED },
  { 10, TN( 4 ), TN( 2 ), TN( 9 ) , RBT_BLACK },
  { 19, TN( 6 ), NULL, NULL , RBT_RED },
  { 43, TN( 0 ), TN( 6 ), TN( 5 ) , RBT_RED },
  { 44, TN( 4 ), NULL, TN( 8 ) , RBT_BLACK },
  { 50, TN( 5 ), NULL, NULL , RBT_RED },
  { 52, NULL, TN( 4 ), TN( 3 ) , RBT_BLACK },
  { 60, TN( 3 ), NULL, NULL , RBT_RED },
  { 85, TN( 0 ), TN( 7 ), TN( 1 ) , RBT_BLACK },
  { 99, TN( 3 ), NULL, NULL , RBT_RED }
};

static const test_node_description test_insert_tree_10[] = {
  { 0, TN( 6 ), NULL, TN( 10 ) , RBT_BLACK },
  { 8, TN( 2 ), NULL, NULL , RBT_RED },
  { 10, TN( 4 ), TN( 2 ), TN( 9 ) , RBT_RED },
  { 19, TN( 6 ), NULL, NULL , RBT_BLACK },
  { 43, NULL, TN( 6 ), TN( 0 ) , RBT_BLACK },
  { 44, TN( 0 ), NULL, TN( 8 ) , RBT_BLACK },
  { 50, TN( 5 ), NULL, NULL , RBT_RED },
  { 52, TN( 4 ), TN( 5 ), TN( 3 ) , RBT_RED },
  { 60, TN( 3 ), NULL, NULL , RBT_RED },
  { 85, TN( 0 ), TN( 7 ), TN( 1 ) , RBT_BLACK },
  { 99, TN( 3 ), NULL, NULL , RBT_RED }
};

static const test_node_description test_insert_tree_11[] = {
  { 0, TN( 6 ), NULL, TN( 10 ) , RBT_BLACK },
  { 8, TN( 2 ), NULL, NULL , RBT_RED },
  { 10, TN( 4 ), TN( 2 ), TN( 9 ) , RBT_BLACK },
  { 19, TN( 6 ), NULL, NULL , RBT_BLACK },
  { 43, NULL, TN( 6 ), TN( 0 ) , RBT_BLACK },
  { 44, TN( 0 ), NULL, TN( 8 ) , RBT_BLACK },
  { 50, TN( 5 ), NULL, NULL , RBT_RED },
  { 52, TN( 4 ), TN( 5 ), TN( 3 ) , RBT_BLACK },
  { 60, TN( 3 ), NULL, TN( 11 ) , RBT_BLACK },
  { 68, TN( 7 ), NULL, NULL , RBT_RED },
  { 85, TN( 0 ), TN( 7 ), TN( 1 ) , RBT_RED },
  { 99, TN( 3 ), NULL, NULL , RBT_BLACK }
};

static const test_node_description test_insert_tree_12[] = {
  { 0, TN( 6 ), NULL, TN( 10 ) , RBT_BLACK },
  { 8, TN( 2 ), NULL, NULL , RBT_RED },
  { 10, TN( 4 ), TN( 2 ), TN( 9 ) , RBT_BLACK },
  { 19, TN( 6 ), NULL, NULL , RBT_BLACK },
  { 43, NULL, TN( 6 ), TN( 0 ) , RBT_BLACK },
  { 44, TN( 12 ), NULL, NULL , RBT_RED },
  { 48, TN( 0 ), TN( 5 ), TN( 8 ) , RBT_BLACK },
  { 50, TN( 12 ), NULL, NULL , RBT_RED },
  { 52, TN( 4 ), TN( 12 ), TN( 3 ) , RBT_BLACK },
  { 60, TN( 3 ), NULL, TN( 11 ) , RBT_BLACK },
  { 68, TN( 7 ), NULL, NULL , RBT_RED },
  { 85, TN( 0 ), TN( 7 ), TN( 1 ) , RBT_RED },
  { 99, TN( 3 ), NULL, NULL , RBT_BLACK }
};

static const test_node_description test_insert_tree_13[] = {
  { 0, TN( 6 ), NULL, TN( 10 ) , RBT_BLACK },
  { 8, TN( 2 ), NULL, NULL , RBT_RED },
  { 10, TN( 4 ), TN( 2 ), TN( 9 ) , RBT_BLACK },
  { 19, TN( 6 ), NULL, NULL , RBT_BLACK },
  { 43, NULL, TN( 6 ), TN( 0 ) , RBT_BLACK },
  { 44, TN( 12 ), NULL, NULL , RBT_RED },
  { 48, TN( 0 ), TN( 5 ), TN( 8 ) , RBT_BLACK },
  { 50, TN( 12 ), NULL, NULL , RBT_RED },
  { 52, TN( 4 ), TN( 12 ), TN( 3 ) , RBT_BLACK },
  { 57, TN( 7 ), NULL, NULL , RBT_RED },
  { 60, TN( 3 ), TN( 13 ), TN( 11 ) , RBT_BLACK },
  { 68, TN( 7 ), NULL, NULL , RBT_RED },
  { 85, TN( 0 ), TN( 7 ), TN( 1 ) , RBT_RED },
  { 99, TN( 3 ), NULL, NULL , RBT_BLACK }
};

static const test_node_description test_insert_tree_14[] = {
  { 0, TN( 6 ), NULL, TN( 10 ) , RBT_BLACK },
  { 8, TN( 2 ), NULL, NULL , RBT_RED },
  { 10, TN( 4 ), TN( 2 ), TN( 9 ) , RBT_BLACK },
  { 17, TN( 9 ), NULL, NULL , RBT_RED },
  { 19, TN( 6 ), TN( 14 ), NULL , RBT_BLACK },
  { 43, NULL, TN( 6 ), TN( 0 ) , RBT_BLACK },
  { 44, TN( 12 ), NULL, NULL , RBT_RED },
  { 48, TN( 0 ), TN( 5 ), TN( 8 ) , RBT_BLACK },
  { 50, TN( 12 ), NULL, NULL , RBT_RED },
  { 52, TN( 4 ), TN( 12 ), TN( 3 ) , RBT_BLACK },
  { 57, TN( 7 ), NULL, NULL , RBT_RED },
  { 60, TN( 3 ), TN( 13 ), TN( 11 ) , RBT_BLACK },
  { 68, TN( 7 ), NULL, NULL , RBT_RED },
  { 85, TN( 0 ), TN( 7 ), TN( 1 ) , RBT_RED },
  { 99, TN( 3 ), NULL, NULL , RBT_BLACK }
};

static const test_node_description test_insert_tree_15[] = {
  { 0, TN( 6 ), NULL, TN( 10 ) , RBT_BLACK },
  { 8, TN( 2 ), NULL, NULL , RBT_RED },
  { 10, TN( 4 ), TN( 2 ), TN( 9 ) , RBT_BLACK },
  { 17, TN( 9 ), NULL, NULL , RBT_RED },
  { 19, TN( 6 ), TN( 14 ), NULL , RBT_BLACK },
  { 43, NULL, TN( 6 ), TN( 7 ) , RBT_BLACK },
  { 44, TN( 12 ), NULL, NULL , RBT_RED },
  { 48, TN( 0 ), TN( 5 ), TN( 8 ) , RBT_BLACK },
  { 50, TN( 12 ), NULL, NULL , RBT_RED },
  { 52, TN( 7 ), TN( 12 ), TN( 13 ) , RBT_RED },
  { 57, TN( 0 ), NULL, NULL , RBT_BLACK },
  { 60, TN( 4 ), TN( 0 ), TN( 3 ) , RBT_BLACK },
  { 67, TN( 11 ), NULL, NULL , RBT_RED },
  { 68, TN( 3 ), TN( 15 ), NULL , RBT_BLACK },
  { 85, TN( 7 ), TN( 11 ), TN( 1 ) , RBT_RED },
  { 99, TN( 3 ), NULL, NULL , RBT_BLACK }
};

static const test_node_description test_insert_tree_16[] = {
  { 0, TN( 6 ), NULL, TN( 10 ) , RBT_BLACK },
  { 8, TN( 2 ), NULL, NULL , RBT_RED },
  { 10, TN( 4 ), TN( 2 ), TN( 9 ) , RBT_BLACK },
  { 17, TN( 9 ), NULL, NULL , RBT_RED },
  { 19, TN( 6 ), TN( 14 ), NULL , RBT_BLACK },
  { 43, NULL, TN( 6 ), TN( 7 ) , RBT_BLACK },
  { 44, TN( 12 ), NULL, NULL , RBT_RED },
  { 48, TN( 0 ), TN( 5 ), TN( 8 ) , RBT_BLACK },
  { 50, TN( 12 ), NULL, NULL , RBT_RED },
  { 52, TN( 7 ), TN( 12 ), TN( 13 ) , RBT_RED },
  { 57, TN( 0 ), NULL, NULL , RBT_BLACK },
  { 60, TN( 4 ), TN( 0 ), TN( 3 ) , RBT_BLACK },
  { 67, TN( 11 ), NULL, NULL , RBT_RED },
  { 68, TN( 3 ), TN( 15 ), NULL , RBT_BLACK },
  { 85, TN( 7 ), TN( 11 ), TN( 1 ) , RBT_RED },
  { 90, TN( 1 ), NULL, NULL , RBT_RED },
  { 99, TN( 3 ), TN( 16 ), NULL , RBT_BLACK }
};

static const test_node_description test_insert_tree_17[] = {
  { 0, TN( 6 ), NULL, TN( 10 ) , RBT_BLACK },
  { 8, TN( 2 ), NULL, NULL , RBT_RED },
  { 10, TN( 4 ), TN( 2 ), TN( 14 ) , RBT_BLACK },
  { 12, TN( 14 ), NULL, NULL , RBT_RED },
  { 17, TN( 6 ), TN( 17 ), TN( 9 ) , RBT_BLACK },
  { 19, TN( 14 ), NULL, NULL , RBT_RED },
  { 43, NULL, TN( 6 ), TN( 7 ) , RBT_BLACK },
  { 44, TN( 12 ), NULL, NULL , RBT_RED },
  { 48, TN( 0 ), TN( 5 ), TN( 8 ) , RBT_BLACK },
  { 50, TN( 12 ), NULL, NULL , RBT_RED },
  { 52, TN( 7 ), TN( 12 ), TN( 13 ) , RBT_RED },
  { 57, TN( 0 ), NULL, NULL , RBT_BLACK },
  { 60, TN( 4 ), TN( 0 ), TN( 3 ) , RBT_BLACK },
  { 67, TN( 11 ), NULL, NULL , RBT_RED },
  { 68, TN( 3 ), TN( 15 ), NULL , RBT_BLACK },
  { 85, TN( 7 ), TN( 11 ), TN( 1 ) , RBT_RED },
  { 90, TN( 1 ), NULL, NULL , RBT_RED },
  { 99, TN( 3 ), TN( 16 ), NULL , RBT_BLACK }
};

static const test_node_description test_insert_tree_18[] = {
  { 0, TN( 6 ), NULL, TN( 10 ) , RBT_BLACK },
  { 8, TN( 2 ), NULL, NULL , RBT_RED },
  { 10, TN( 4 ), TN( 2 ), TN( 14 ) , RBT_BLACK },
  { 12, TN( 14 ), NULL, NULL , RBT_RED },
  { 17, TN( 6 ), TN( 17 ), TN( 9 ) , RBT_BLACK },
  { 19, TN( 14 ), NULL, NULL , RBT_RED },
  { 43, NULL, TN( 6 ), TN( 7 ) , RBT_BLACK },
  { 44, TN( 12 ), NULL, NULL , RBT_RED },
  { 48, TN( 0 ), TN( 5 ), TN( 8 ) , RBT_BLACK },
  { 50, TN( 12 ), NULL, NULL , RBT_RED },
  { 52, TN( 7 ), TN( 12 ), TN( 13 ) , RBT_RED },
  { 57, TN( 0 ), NULL, NULL , RBT_BLACK },
  { 60, TN( 4 ), TN( 0 ), TN( 3 ) , RBT_BLACK },
  { 67, TN( 11 ), NULL, NULL , RBT_RED },
  { 68, TN( 3 ), TN( 15 ), TN( 18 ) , RBT_BLACK },
  { 77, TN( 11 ), NULL, NULL , RBT_RED },
  { 85, TN( 7 ), TN( 11 ), TN( 1 ) , RBT_RED },
  { 90, TN( 1 ), NULL, NULL , RBT_RED },
  { 99, TN( 3 ), TN( 16 ), NULL , RBT_BLACK }
};

static const test_node_description test_insert_tree_19[] = {
  { 0, TN( 6 ), NULL, TN( 10 ) , RBT_BLACK },
  { 8, TN( 2 ), NULL, NULL , RBT_RED },
  { 10, TN( 4 ), TN( 2 ), TN( 14 ) , RBT_BLACK },
  { 12, TN( 14 ), NULL, NULL , RBT_RED },
  { 17, TN( 6 ), TN( 17 ), TN( 9 ) , RBT_BLACK },
  { 19, TN( 14 ), NULL, NULL , RBT_RED },
  { 43, NULL, TN( 6 ), TN( 7 ) , RBT_BLACK },
  { 44, TN( 12 ), NULL, NULL , RBT_RED },
  { 48, TN( 0 ), TN( 5 ), TN( 8 ) , RBT_BLACK },
  { 50, TN( 12 ), NULL, NULL , RBT_RED },
  { 52, TN( 7 ), TN( 12 ), TN( 13 ) , RBT_BLACK },
  { 57, TN( 0 ), NULL, NULL , RBT_BLACK },
  { 60, TN( 4 ), TN( 0 ), TN( 3 ) , RBT_RED },
  { 67, TN( 11 ), NULL, NULL , RBT_BLACK },
  { 68, TN( 3 ), TN( 15 ), TN( 18 ) , RBT_RED },
  { 71, TN( 18 ), NULL, NULL , RBT_RED },
  { 77, TN( 11 ), TN( 19 ), NULL , RBT_BLACK },
  { 85, TN( 7 ), TN( 11 ), TN( 1 ) , RBT_BLACK },
  { 90, TN( 1 ), NULL, NULL , RBT_RED },
  { 99, TN( 3 ), TN( 16 ), NULL , RBT_BLACK }
};

static const test_node_description *const test_insert_trees[] = {
  &test_insert_tree_0[ 0 ],
  &test_insert_tree_1[ 0 ],
  &test_insert_tree_2[ 0 ],
  &test_insert_tree_3[ 0 ],
  &test_insert_tree_4[ 0 ],
  &test_insert_tree_5[ 0 ],
  &test_insert_tree_6[ 0 ],
  &test_insert_tree_7[ 0 ],
  &test_insert_tree_8[ 0 ],
  &test_insert_tree_9[ 0 ],
  &test_insert_tree_10[ 0 ],
  &test_insert_tree_11[ 0 ],
  &test_insert_tree_12[ 0 ],
  &test_insert_tree_13[ 0 ],
  &test_insert_tree_14[ 0 ],
  &test_insert_tree_15[ 0 ],
  &test_insert_tree_16[ 0 ],
  &test_insert_tree_17[ 0 ],
  &test_insert_tree_18[ 0 ],
  &test_insert_tree_19[ 0 ]
};

static const test_node_description test_remove_tree_0[] = {
  { 8, TN( 6 ), NULL, NULL , RBT_BLACK },
  { 10, TN( 4 ), TN( 10 ), TN( 14 ) , RBT_BLACK },
  { 12, TN( 14 ), NULL, NULL , RBT_RED },
  { 17, TN( 6 ), TN( 17 ), TN( 9 ) , RBT_BLACK },
  { 19, TN( 14 ), NULL, NULL , RBT_RED },
  { 43, NULL, TN( 6 ), TN( 7 ) , RBT_BLACK },
  { 44, TN( 12 ), NULL, NULL , RBT_RED },
  { 48, TN( 0 ), TN( 5 ), TN( 8 ) , RBT_BLACK },
  { 50, TN( 12 ), NULL, NULL , RBT_RED },
  { 52, TN( 7 ), TN( 12 ), TN( 13 ) , RBT_BLACK },
  { 57, TN( 0 ), NULL, NULL , RBT_BLACK },
  { 60, TN( 4 ), TN( 0 ), TN( 3 ) , RBT_RED },
  { 67, TN( 11 ), NULL, NULL , RBT_BLACK },
  { 68, TN( 3 ), TN( 15 ), TN( 18 ) , RBT_RED },
  { 71, TN( 18 ), NULL, NULL , RBT_RED },
  { 77, TN( 11 ), TN( 19 ), NULL , RBT_BLACK },
  { 85, TN( 7 ), TN( 11 ), TN( 1 ) , RBT_BLACK },
  { 90, TN( 1 ), NULL, NULL , RBT_RED },
  { 99, TN( 3 ), TN( 16 ), NULL , RBT_BLACK }
};

static const test_node_description test_remove_tree_1[] = {
  { 10, TN( 14 ), NULL, TN( 17 ) , RBT_BLACK },
  { 12, TN( 6 ), NULL, NULL , RBT_RED },
  { 17, TN( 4 ), TN( 6 ), TN( 9 ) , RBT_BLACK },
  { 19, TN( 14 ), NULL, NULL , RBT_BLACK },
  { 43, NULL, TN( 14 ), TN( 7 ) , RBT_BLACK },
  { 44, TN( 12 ), NULL, NULL , RBT_RED },
  { 48, TN( 0 ), TN( 5 ), TN( 8 ) , RBT_BLACK },
  { 50, TN( 12 ), NULL, NULL , RBT_RED },
  { 52, TN( 7 ), TN( 12 ), TN( 13 ) , RBT_BLACK },
  { 57, TN( 0 ), NULL, NULL , RBT_BLACK },
  { 60, TN( 4 ), TN( 0 ), TN( 3 ) , RBT_RED },
  { 67, TN( 11 ), NULL, NULL , RBT_BLACK },
  { 68, TN( 3 ), TN( 15 ), TN( 18 ) , RBT_RED },
  { 71, TN( 18 ), NULL, NULL , RBT_RED },
  { 77, TN( 11 ), TN( 19 ), NULL , RBT_BLACK },
  { 85, TN( 7 ), TN( 11 ), TN( 1 ) , RBT_BLACK },
  { 90, TN( 1 ), NULL, NULL , RBT_RED },
  { 99, TN( 3 ), TN( 16 ), NULL , RBT_BLACK }
};

static const test_node_description test_remove_tree_2[] = {
  { 12, TN( 14 ), NULL, NULL , RBT_BLACK },
  { 17, TN( 4 ), TN( 17 ), TN( 9 ) , RBT_BLACK },
  { 19, TN( 14 ), NULL, NULL , RBT_BLACK },
  { 43, NULL, TN( 14 ), TN( 7 ) , RBT_BLACK },
  { 44, TN( 12 ), NULL, NULL , RBT_RED },
  { 48, TN( 0 ), TN( 5 ), TN( 8 ) , RBT_BLACK },
  { 50, TN( 12 ), NULL, NULL , RBT_RED },
  { 52, TN( 7 ), TN( 12 ), TN( 13 ) , RBT_BLACK },
  { 57, TN( 0 ), NULL, NULL , RBT_BLACK },
  { 60, TN( 4 ), TN( 0 ), TN( 3 ) , RBT_RED },
  { 67, TN( 11 ), NULL, NULL , RBT_BLACK },
  { 68, TN( 3 ), TN( 15 ), TN( 18 ) , RBT_RED },
  { 71, TN( 18 ), NULL, NULL , RBT_RED },
  { 77, TN( 11 ), TN( 19 ), NULL , RBT_BLACK },
  { 85, TN( 7 ), TN( 11 ), TN( 1 ) , RBT_BLACK },
  { 90, TN( 1 ), NULL, NULL , RBT_RED },
  { 99, TN( 3 ), TN( 16 ), NULL , RBT_BLACK }
};

static const test_node_description test_remove_tree_3[] = {
  { 17, TN( 4 ), NULL, TN( 9 ) , RBT_BLACK },
  { 19, TN( 14 ), NULL, NULL , RBT_RED },
  { 43, TN( 7 ), TN( 14 ), TN( 0 ) , RBT_BLACK },
  { 44, TN( 12 ), NULL, NULL , RBT_RED },
  { 48, TN( 0 ), TN( 5 ), TN( 8 ) , RBT_BLACK },
  { 50, TN( 12 ), NULL, NULL , RBT_RED },
  { 52, TN( 4 ), TN( 12 ), TN( 13 ) , RBT_RED },
  { 57, TN( 0 ), NULL, NULL , RBT_BLACK },
  { 60, NULL, TN( 4 ), TN( 3 ) , RBT_BLACK },
  { 67, TN( 11 ), NULL, NULL , RBT_BLACK },
  { 68, TN( 3 ), TN( 15 ), TN( 18 ) , RBT_RED },
  { 71, TN( 18 ), NULL, NULL , RBT_RED },
  { 77, TN( 11 ), TN( 19 ), NULL , RBT_BLACK },
  { 85, TN( 7 ), TN( 11 ), TN( 1 ) , RBT_BLACK },
  { 90, TN( 1 ), NULL, NULL , RBT_RED },
  { 99, TN( 3 ), TN( 16 ), NULL , RBT_BLACK }
};

static const test_node_description test_remove_tree_4[] = {
  { 19, TN( 4 ), NULL, NULL , RBT_BLACK },
  { 43, TN( 7 ), TN( 9 ), TN( 0 ) , RBT_BLACK },
  { 44, TN( 12 ), NULL, NULL , RBT_RED },
  { 48, TN( 0 ), TN( 5 ), TN( 8 ) , RBT_BLACK },
  { 50, TN( 12 ), NULL, NULL , RBT_RED },
  { 52, TN( 4 ), TN( 12 ), TN( 13 ) , RBT_RED },
  { 57, TN( 0 ), NULL, NULL , RBT_BLACK },
  { 60, NULL, TN( 4 ), TN( 3 ) , RBT_BLACK },
  { 67, TN( 11 ), NULL, NULL , RBT_BLACK },
  { 68, TN( 3 ), TN( 15 ), TN( 18 ) , RBT_RED },
  { 71, TN( 18 ), NULL, NULL , RBT_RED },
  { 77, TN( 11 ), TN( 19 ), NULL , RBT_BLACK },
  { 85, TN( 7 ), TN( 11 ), TN( 1 ) , RBT_BLACK },
  { 90, TN( 1 ), NULL, NULL , RBT_RED },
  { 99, TN( 3 ), TN( 16 ), NULL , RBT_BLACK }
};

static const test_node_description test_remove_tree_5[] = {
  { 43, TN( 12 ), NULL, TN( 5 ) , RBT_BLACK },
  { 44, TN( 4 ), NULL, NULL , RBT_RED },
  { 48, TN( 0 ), TN( 4 ), TN( 8 ) , RBT_RED },
  { 50, TN( 12 ), NULL, NULL , RBT_BLACK },
  { 52, TN( 7 ), TN( 12 ), TN( 13 ) , RBT_BLACK },
  { 57, TN( 0 ), NULL, NULL , RBT_BLACK },
  { 60, NULL, TN( 0 ), TN( 3 ) , RBT_BLACK },
  { 67, TN( 11 ), NULL, NULL , RBT_BLACK },
  { 68, TN( 3 ), TN( 15 ), TN( 18 ) , RBT_RED },
  { 71, TN( 18 ), NULL, NULL , RBT_RED },
  { 77, TN( 11 ), TN( 19 ), NULL , RBT_BLACK },
  { 85, TN( 7 ), TN( 11 ), TN( 1 ) , RBT_BLACK },
  { 90, TN( 1 ), NULL, NULL , RBT_RED },
  { 99, TN( 3 ), TN( 16 ), NULL , RBT_BLACK }
};

static const test_node_description test_remove_tree_6[] = {
  { 44, TN( 12 ), NULL, NULL , RBT_BLACK },
  { 48, TN( 0 ), TN( 5 ), TN( 8 ) , RBT_RED },
  { 50, TN( 12 ), NULL, NULL , RBT_BLACK },
  { 52, TN( 7 ), TN( 12 ), TN( 13 ) , RBT_BLACK },
  { 57, TN( 0 ), NULL, NULL , RBT_BLACK },
  { 60, NULL, TN( 0 ), TN( 3 ) , RBT_BLACK },
  { 67, TN( 11 ), NULL, NULL , RBT_BLACK },
  { 68, TN( 3 ), TN( 15 ), TN( 18 ) , RBT_RED },
  { 71, TN( 18 ), NULL, NULL , RBT_RED },
  { 77, TN( 11 ), TN( 19 ), NULL , RBT_BLACK },
  { 85, TN( 7 ), TN( 11 ), TN( 1 ) , RBT_BLACK },
  { 90, TN( 1 ), NULL, NULL , RBT_RED },
  { 99, TN( 3 ), TN( 16 ), NULL , RBT_BLACK }
};

static const test_node_description test_remove_tree_7[] = {
  { 48, TN( 0 ), NULL, TN( 8 ) , RBT_BLACK },
  { 50, TN( 12 ), NULL, NULL , RBT_RED },
  { 52, TN( 7 ), TN( 12 ), TN( 13 ) , RBT_BLACK },
  { 57, TN( 0 ), NULL, NULL , RBT_BLACK },
  { 60, NULL, TN( 0 ), TN( 3 ) , RBT_BLACK },
  { 67, TN( 11 ), NULL, NULL , RBT_BLACK },
  { 68, TN( 3 ), TN( 15 ), TN( 18 ) , RBT_RED },
  { 71, TN( 18 ), NULL, NULL , RBT_RED },
  { 77, TN( 11 ), TN( 19 ), NULL , RBT_BLACK },
  { 85, TN( 7 ), TN( 11 ), TN( 1 ) , RBT_BLACK },
  { 90, TN( 1 ), NULL, NULL , RBT_RED },
  { 99, TN( 3 ), TN( 16 ), NULL , RBT_BLACK }
};

static const test_node_description test_remove_tree_8[] = {
  { 50, TN( 0 ), NULL, NULL , RBT_BLACK },
  { 52, TN( 7 ), TN( 8 ), TN( 13 ) , RBT_BLACK },
  { 57, TN( 0 ), NULL, NULL , RBT_BLACK },
  { 60, NULL, TN( 0 ), TN( 3 ) , RBT_BLACK },
  { 67, TN( 11 ), NULL, NULL , RBT_BLACK },
  { 68, TN( 3 ), TN( 15 ), TN( 18 ) , RBT_RED },
  { 71, TN( 18 ), NULL, NULL , RBT_RED },
  { 77, TN( 11 ), TN( 19 ), NULL , RBT_BLACK },
  { 85, TN( 7 ), TN( 11 ), TN( 1 ) , RBT_BLACK },
  { 90, TN( 1 ), NULL, NULL , RBT_RED },
  { 99, TN( 3 ), TN( 16 ), NULL , RBT_BLACK }
};

static const test_node_description test_remove_tree_9[] = {
  { 52, TN( 7 ), NULL, TN( 13 ) , RBT_BLACK },
  { 57, TN( 0 ), NULL, NULL , RBT_RED },
  { 60, TN( 11 ), TN( 0 ), TN( 15 ) , RBT_BLACK },
  { 67, TN( 7 ), NULL, NULL , RBT_BLACK },
  { 68, NULL, TN( 7 ), TN( 3 ) , RBT_BLACK },
  { 71, TN( 18 ), NULL, NULL , RBT_RED },
  { 77, TN( 3 ), TN( 19 ), NULL , RBT_BLACK },
  { 85, TN( 11 ), TN( 18 ), TN( 1 ) , RBT_BLACK },
  { 90, TN( 1 ), NULL, NULL , RBT_RED },
  { 99, TN( 3 ), TN( 16 ), NULL , RBT_BLACK }
};

static const test_node_description test_remove_tree_10[] = {
  { 57, TN( 7 ), NULL, NULL , RBT_BLACK },
  { 60, TN( 11 ), TN( 13 ), TN( 15 ) , RBT_BLACK },
  { 67, TN( 7 ), NULL, NULL , RBT_BLACK },
  { 68, NULL, TN( 7 ), TN( 3 ) , RBT_BLACK },
  { 71, TN( 18 ), NULL, NULL , RBT_RED },
  { 77, TN( 3 ), TN( 19 ), NULL , RBT_BLACK },
  { 85, TN( 11 ), TN( 18 ), TN( 1 ) , RBT_BLACK },
  { 90, TN( 1 ), NULL, NULL , RBT_RED },
  { 99, TN( 3 ), TN( 16 ), NULL , RBT_BLACK }
};

static const test_node_description test_remove_tree_11[] = {
  { 60, TN( 11 ), NULL, TN( 15 ) , RBT_BLACK },
  { 67, TN( 7 ), NULL, NULL , RBT_RED },
  { 68, NULL, TN( 7 ), TN( 3 ) , RBT_BLACK },
  { 71, TN( 18 ), NULL, NULL , RBT_RED },
  { 77, TN( 3 ), TN( 19 ), NULL , RBT_BLACK },
  { 85, TN( 11 ), TN( 18 ), TN( 1 ) , RBT_RED },
  { 90, TN( 1 ), NULL, NULL , RBT_RED },
  { 99, TN( 3 ), TN( 16 ), NULL , RBT_BLACK }
};

static const test_node_description test_remove_tree_12[] = {
  { 67, TN( 11 ), NULL, NULL , RBT_BLACK },
  { 68, NULL, TN( 15 ), TN( 3 ) , RBT_BLACK },
  { 71, TN( 18 ), NULL, NULL , RBT_RED },
  { 77, TN( 3 ), TN( 19 ), NULL , RBT_BLACK },
  { 85, TN( 11 ), TN( 18 ), TN( 1 ) , RBT_RED },
  { 90, TN( 1 ), NULL, NULL , RBT_RED },
  { 99, TN( 3 ), TN( 16 ), NULL , RBT_BLACK }
};

static const test_node_description test_remove_tree_13[] = {
  { 68, TN( 19 ), NULL, NULL , RBT_BLACK },
  { 71, TN( 3 ), TN( 11 ), TN( 18 ) , RBT_RED },
  { 77, TN( 19 ), NULL, NULL , RBT_BLACK },
  { 85, NULL, TN( 19 ), TN( 1 ) , RBT_BLACK },
  { 90, TN( 1 ), NULL, NULL , RBT_RED },
  { 99, TN( 3 ), TN( 16 ), NULL , RBT_BLACK }
};

static const test_node_description test_remove_tree_14[] = {
  { 71, TN( 3 ), NULL, TN( 18 ) , RBT_BLACK },
  { 77, TN( 19 ), NULL, NULL , RBT_RED },
  { 85, NULL, TN( 19 ), TN( 1 ) , RBT_BLACK },
  { 90, TN( 1 ), NULL, NULL , RBT_RED },
  { 99, TN( 3 ), TN( 16 ), NULL , RBT_BLACK }
};

static const test_node_description test_remove_tree_15[] = {
  { 77, TN( 3 ), NULL, NULL , RBT_BLACK },
  { 85, NULL, TN( 18 ), TN( 1 ) , RBT_BLACK },
  { 90, TN( 1 ), NULL, NULL , RBT_RED },
  { 99, TN( 3 ), TN( 16 ), NULL , RBT_BLACK }
};

static const test_node_description test_remove_tree_16[] = {
  { 85, TN( 16 ), NULL, NULL , RBT_BLACK },
  { 90, NULL, TN( 3 ), TN( 1 ) , RBT_BLACK },
  { 99, TN( 16 ), NULL, NULL , RBT_BLACK }
};

static const test_node_description test_remove_tree_17[] = {
  { 90, NULL, NULL, TN( 1 ) , RBT_BLACK },
  { 99, TN( 16 ), NULL, NULL , RBT_RED }
};

static const test_node_description test_remove_tree_18[] = {
  { 99, NULL, NULL, NULL , RBT_BLACK }
};

static const test_node_description *const test_remove_trees[] = {
  &test_remove_tree_0[ 0 ],
  &test_remove_tree_1[ 0 ],
  &test_remove_tree_2[ 0 ],
  &test_remove_tree_3[ 0 ],
  &test_remove_tree_4[ 0 ],
  &test_remove_tree_5[ 0 ],
  &test_remove_tree_6[ 0 ],
  &test_remove_tree_7[ 0 ],
  &test_remove_tree_8[ 0 ],
  &test_remove_tree_9[ 0 ],
  &test_remove_tree_10[ 0 ],
  &test_remove_tree_11[ 0 ],
  &test_remove_tree_12[ 0 ],
  &test_remove_tree_13[ 0 ],
  &test_remove_tree_14[ 0 ],
  &test_remove_tree_15[ 0 ],
  &test_remove_tree_16[ 0 ],
  &test_remove_tree_17[ 0 ],
  &test_remove_tree_18[ 0 ]
};

typedef struct {
  int current;
  int count;
  const test_node_description *tree;
} visitor_context;

static bool visit_nodes(
  const RBTree_Node *node,
  RBTree_Direction   dir,
  void              *visitor_arg
)
{
  visitor_context *ctx = visitor_arg;
  const test_node_description *td = &ctx->tree[ ctx->current ];
  const test_node *tn = RTEMS_CONTAINER_OF( node, test_node, Node );

  rtems_test_assert( ctx->current < ctx->count );

  rtems_test_assert( td->key == tn->id );
  rtems_test_assert( td->key == tn->key );

  if ( td->parent == NULL ) {
    rtems_test_assert( td->parent == tn->Node.parent->parent );
  } else {
    rtems_test_assert( td->parent == tn->Node.parent );
  }

  rtems_test_assert( td->left == tn->Node.child[ RBT_LEFT ] );
  rtems_test_assert( td->right == tn->Node.child[ RBT_RIGHT ] );
  rtems_test_assert( td->color == tn->Node.color );

  ++ctx->current;

  return false;
}

rtems_task Init( rtems_task_argument ignored )
{
  rtems_rbtree_control  rbtree1;
  rtems_rbtree_node    *p;
  test_node            node1, node2;
  test_node            search_node;
  int                  id;
  int i;

  TEST_BEGIN();

  puts( "Init - Initialize rbtree empty" );
  rtems_rbtree_initialize_empty( &rbtree1 );

  rtems_rbtree_set_off_tree( &node1.Node );
  rtems_test_assert( rtems_rbtree_is_node_off_tree( &node1.Node ) );

  /* verify that the rbtree insert work */
  puts( "INIT - Verify rtems_rbtree_insert with two nodes" );
  node1.id = 1;
  node1.key = 1;
  node2.id = 2;
  node2.key = 2;
  rb_insert_unique( &rbtree1, &node1.Node );
  rb_insert_unique( &rbtree1, &node2.Node );

  rtems_test_assert( !rtems_rbtree_is_node_off_tree( &node1.Node ) );

  _RBTree_Rotate(NULL, RBT_LEFT);
  i = (node1.Node.parent == &node2.Node);
  _RBTree_Rotate( &node1.Node,
                  !node1.Node.child[RBT_LEFT] ? RBT_RIGHT : RBT_LEFT
                );
  if ( (node1.Node.parent == &node2.Node) != i )
    puts( "INIT - FAILED FALSE ROTATION" );

  if (!rb_assert(rbtree1.root) )
    puts( "INIT - FAILED TREE CHECK" );

  for ( p = rtems_rbtree_get_min(&rbtree1), id = 1 ; p ;
      p = rtems_rbtree_get_min(&rbtree1) , id++ ) {
    test_node *t = RTEMS_CONTAINER_OF(p,test_node,Node);
    if ( id > 2 ) {
      puts( "INIT - TOO MANY NODES ON RBTREE" );
      rtems_test_exit(0);
    }
    if ( t->id != id ) {
      puts( "INIT - ERROR ON RBTREE ID MISMATCH" );
      rtems_test_exit(0);
    }

    if (!rb_assert(rbtree1.root) )
      puts( "INIT - FAILED TREE CHECK" );
  }
  if (id < 2) {
    puts("INIT - NOT ENOUGH NODES ON RBTREE");
    rtems_test_exit(0);
  }

  puts("INIT - Verify rtems_rbtree_insert with the same value twice");
  node2.key = node1.key;
  rb_insert_unique(&rbtree1, &node1.Node);
  p = rb_insert_unique(&rbtree1, &node2.Node);

  if (p != &node1.Node)
    puts( "INIT - FAILED DUPLICATE INSERT" );

  for ( p = rtems_rbtree_get_min(&rbtree1), id = 1 ; p ;
      p = rtems_rbtree_get_min(&rbtree1) , id++ ) {
    test_node *t = RTEMS_CONTAINER_OF(p,test_node,Node);
    if ( id > 1 ) {
      puts( "INIT - TOO MANY NODES ON RBTREE" );
      rtems_test_exit(0);
    }
    if ( t->id != id ) {
      puts( "INIT - ERROR ON RBTREE ID MISMATCH" );
      rtems_test_exit(0);
    }

    if (!rb_assert(rbtree1.root) )
      puts( "INIT - FAILED TREE CHECK" );
  }
  if (id < 1) {
    puts("INIT - NOT ENOUGH NODES ON RBTREE");
    rtems_test_exit(0);
  }
  node2.key = 2;

  /* verify that the rbtree is empty */
  puts( "INIT - Verify rtems_rbtree_is_empty" );
  if(!rtems_rbtree_is_empty(&rbtree1)) {
    puts( "INIT - TREE NOT EMPTY" );
    rtems_test_exit(0);
  }

  puts( "INIT - Verify rtems_XXX on an empty tree" );
  if(rtems_rbtree_get_min(&rbtree1)) {
    puts("INIT - get_min on empty returned non-NULL");
    rtems_test_exit(0);
  }
  if(rtems_rbtree_get_max(&rbtree1)) {
    puts("INIT - get_max on empty returned non-NULL");
    rtems_test_exit(0);
  }
  if(rtems_rbtree_peek_min(&rbtree1)) {
    puts("INIT - peek_min on empty returned non-NULL");
    rtems_test_exit(0);
  }
  if(rtems_rbtree_peek_max(&rbtree1)) {
    puts("INIT - peek_max on empty returned non-NULL");
    rtems_test_exit(0);
  }


  /* verify that the rbtree insert works after a tree is emptied */
  puts( "INIT - Verify rtems_rbtree_insert after empty tree" );
  node1.id = 2;
  node1.key = 2;
  node2.id = 1;
  node2.key = 1;
  rb_insert_unique( &rbtree1, &node1.Node );
  rb_insert_unique( &rbtree1, &node2.Node );

  puts( "INIT - Verify rtems_rbtree_peek_max/min, rtems_rbtree_extract" );
  test_node *t1 = RTEMS_CONTAINER_OF(rtems_rbtree_peek_max(&rbtree1),
         test_node,Node);
  test_node *t2 = RTEMS_CONTAINER_OF(rtems_rbtree_peek_min(&rbtree1),
         test_node,Node);
  if (t1->key - t2->key != 1) {
    puts( "INIT - Peek Min - Max failed" );
    rtems_test_exit(0);
  }
  p = rtems_rbtree_peek_max(&rbtree1);
  rtems_rbtree_extract(&rbtree1, p);
  t1 = RTEMS_CONTAINER_OF(p,test_node,Node);
  if (t1->key != 2) {
    puts( "INIT - rtems_rbtree_extract failed");
    rtems_test_exit(0);
  }
  rtems_test_assert( !rtems_rbtree_is_node_off_tree( p ) );
  rb_insert_unique(&rbtree1, p);

  for ( p = rtems_rbtree_get_min(&rbtree1), id = 1 ; p ;
      p = rtems_rbtree_get_min(&rbtree1) , id++ ) {
    test_node *t = RTEMS_CONTAINER_OF(p,test_node,Node);
    if ( id > 2 ) {
      puts( "INIT - TOO MANY NODES ON RBTREE" );
      rtems_test_exit(0);
    }
    if ( t->id != id ) {
      puts( "INIT - ERROR ON RBTREE ID MISMATCH" );
      rtems_test_exit(0);
    }
  }
  
  puts( "INIT - Verify rtems_rbtree_insert with 100 nodes value [0,99]" );
  for (i = 0; i < 100; i++) {
    node_array[i].id = i;
    node_array[i].key = i;
    rb_insert_unique( &rbtree1, &node_array[i].Node );

    if (!rb_assert(rbtree1.root) )
      puts( "INIT - FAILED TREE CHECK" );
  }

  puts( "INIT - Removing 100 nodes" );

  for ( p = rtems_rbtree_get_min(&rbtree1), id = 0 ; p ;
      p = rtems_rbtree_get_min(&rbtree1) , id++ ) {
    test_node *t = RTEMS_CONTAINER_OF(p,test_node,Node);
    if ( id > 99 ) {
      puts( "INIT - TOO MANY NODES ON RBTREE" );
      rtems_test_exit(0);
    }
    if ( t->id != id ) {
      puts( "INIT - ERROR ON RBTREE ID MISMATCH" );
      rtems_test_exit(0);
    }

    if (!rb_assert(rbtree1.root) )
      puts( "INIT - FAILED TREE CHECK" );
  }
  
  if(!rtems_rbtree_is_empty(&rbtree1)) {
    puts( "INIT - TREE NOT EMPTY" );
    rtems_test_exit(0);
  }

  puts( "INIT - Verify rtems_rbtree_insert with 100 nodes value [99,0]" );
  for (i = 0; i < 100; i++) {
    node_array[i].id = 99-i;
    node_array[i].key = 99-i;
    rb_insert_unique( &rbtree1, &node_array[i].Node );

    if (!rb_assert(rbtree1.root) )
      puts( "INIT - FAILED TREE CHECK" );
  }

  puts( "INIT - Removing 100 nodes" );

  for ( p = rtems_rbtree_get_min(&rbtree1), id = 0 ; p ;
      p = rtems_rbtree_get_min(&rbtree1) , id++ ) {
    test_node *t = RTEMS_CONTAINER_OF(p,test_node,Node);
    if ( id > 99 ) {
      puts( "INIT - TOO MANY NODES ON RBTREE" );
      rtems_test_exit(0);
    }
    if ( t->id != id ) {
      puts( "INIT - ERROR ON RBTREE ID MISMATCH" );
      rtems_test_exit(0);
    }

    if (!rb_assert(rbtree1.root) )
      puts( "INIT - FAILED TREE CHECK" );
  }

  if(!rtems_rbtree_is_empty(&rbtree1)) {
    puts( "INIT - TREE NOT EMPTY" );
    rtems_test_exit(0);
  }

  /* testing rbtree_extract by adding 100 nodes then removing the 20 with
   * keys specified by the numbers array, then removing the rest */
  puts( "INIT - Verify rtems_rbtree_extract with 100 nodes value [0,99]" );
  for (i = 0; i < 100; i++) {
    node_array[i].id = i;
    node_array[i].key = i;
    rb_insert_unique( &rbtree1, &node_array[i].Node );

    if (!rb_assert(rbtree1.root) )
      puts( "INIT - FAILED TREE CHECK" );
  }

  puts( "INIT - Extracting 20 random nodes" );

  for (i = 0; i < 20; i++) {
    id = numbers[i];
    rtems_rbtree_extract( &rbtree1, &node_array[id].Node );
    if (!rb_assert(rbtree1.root) )
      puts( "INIT - FAILED TREE CHECK" );
  }

  puts( "INIT - Removing 80 nodes" );

  for ( p = rtems_rbtree_get_min(&rbtree1), id = 0, i = 0 ; p ;
      p = rtems_rbtree_get_min(&rbtree1) , id++ ) {
    test_node *t = RTEMS_CONTAINER_OF(p, test_node, Node);

    while ( id == numbers_sorted[i] ) {
      /* skip if expected minimum (id) is in the set of extracted numbers */
      id++;
      i++;
    }

    if ( id > 99 ) {
      puts( "INIT - TOO MANY NODES ON RBTREE" );
      rtems_test_exit(0);
    }

    if ( t->id != id ) {
      puts( "INIT - ERROR ON RBTREE ID MISMATCH" );
      rtems_test_exit(0);
    }

    if (!rb_assert(rbtree1.root) )
      puts( "INIT - FAILED TREE CHECK" );
  }

  if(!rtems_rbtree_is_empty(&rbtree1)) {
    puts( "INIT - TREE NOT EMPTY" );
    rtems_test_exit(0);
  }

  /* Additional rtems_rbtree_extract test which removes a node
   * with two children while target node has a left child only. */
  for ( i = 0; i < 7; i++ ) {
    node_array[i].id = i;
    node_array[i].key = i;
  }
  rb_insert_unique( &rbtree1, &node_array[3].Node );
  rb_insert_unique( &rbtree1, &node_array[1].Node );
  rb_insert_unique( &rbtree1, &node_array[5].Node );
  rb_insert_unique( &rbtree1, &node_array[0].Node );
  rb_insert_unique( &rbtree1, &node_array[2].Node );
  rb_insert_unique( &rbtree1, &node_array[4].Node );
  rb_insert_unique( &rbtree1, &node_array[6].Node );
  rtems_rbtree_extract( &rbtree1, &node_array[2].Node );
  /* node_array[1] has now only a left child. */
  if ( !node_array[1].Node.child[RBT_LEFT] ||
        node_array[1].Node.child[RBT_RIGHT] )
     puts( "INIT - LEFT CHILD ONLY NOT FOUND" );
  rtems_rbtree_extract( &rbtree1, &node_array[3].Node );
  while( (p = rtems_rbtree_get_max(&rbtree1)) );

  puts( "INIT - Verify rtems_rbtree_get_max with 100 nodes value [99,0]" );
  for (i = 0; i < 100; i++) {
    node_array[i].id = 99-i;
    node_array[i].key = 99-i;
    rb_insert_unique( &rbtree1, &node_array[i].Node );

    if (!rb_assert(rbtree1.root) )
      puts( "INIT - FAILED TREE CHECK" );
  }

  puts( "INIT - Removing 100 nodes" );

  for ( p = rtems_rbtree_get_max(&rbtree1), id = 0 ; p ;
      p = rtems_rbtree_get_max(&rbtree1) , id++ ) {
    test_node *t = RTEMS_CONTAINER_OF(p,test_node,Node);
    if ( id > 99 ) {
      puts( "INIT - TOO MANY NODES ON RBTREE" );
      rtems_test_exit(0);
    }
    if ( t->id != 99-id ) {
      puts( "INIT - ERROR ON RBTREE ID MISMATCH" );
      rtems_test_exit(0);
    }

    if (!rb_assert(rbtree1.root) )
      puts( "INIT - FAILED TREE CHECK" );
  }

  if(!rtems_rbtree_is_empty(&rbtree1)) {
    puts( "INIT - TREE NOT EMPTY" );
    rtems_test_exit(0);
  }

  puts( "INIT - Verify rtems_rbtree_get_max with 100 nodes value [0,99]" );
  for (i = 0; i < 100; i++) {
    node_array[i].id = i;
    node_array[i].key = i;
    rb_insert_unique( &rbtree1, &node_array[i].Node );

    if (!rb_assert(rbtree1.root) )
      puts( "INIT - FAILED TREE CHECK" );
  }

  puts( "INIT - Verify rtems_rbtree_find" );
  search_node.key = 30;
  p = rb_find_unique(&rbtree1, &search_node.Node);
  if(RTEMS_CONTAINER_OF(p,test_node,Node)->id != 30) {
    puts ("INIT - ERROR ON RBTREE ID MISMATCH");
    rtems_test_exit(0);
  }

  puts( "INIT - Verify rtems_rbtree_predecessor/successor");
  p = rtems_rbtree_predecessor(p);
  if(p && RTEMS_CONTAINER_OF(p,test_node,Node)->id != 29) {
    puts ("INIT - ERROR ON RBTREE ID MISMATCH");
    rtems_test_exit(0);
  }
  p = rb_find_unique(&rbtree1, &search_node.Node);
  p = rtems_rbtree_successor(p);
  if(p && RTEMS_CONTAINER_OF(p,test_node,Node)->id != 31) {
    puts ("INIT - ERROR ON RBTREE ID MISMATCH");
    rtems_test_exit(0);
  }

  p = rb_find_unique(&rbtree1, &search_node.Node);
  puts( "INIT - Verify rtems_rbtree_find_control" );
  if (rtems_rbtree_find_control(p) != &rbtree1) {
    puts ("INIT - ERROR ON RBTREE HEADER MISMATCH");
    rtems_test_exit(0);
  }

  if ( _RBTree_Sibling( NULL ) != NULL )
    puts ( "INIT - ERROR ON RBTREE NULL SIBLING MISMATCH" );
  if ( _RBTree_Sibling( rbtree1.root ) != NULL )
    puts ( "INIT - ERROR ON RBTREE NULL SIBLING MISMATCH" );
  if ( _RBTree_Grandparent( NULL ) != NULL )
    puts ( "INIT - ERROR ON RBTREE NULL GRANDPARENT MISMATCH" );
  if ( _RBTree_Is_red( NULL ) != 0 )
    puts ( "INIT - ERROR ON RBTREE NULL IS RED MISMATCH" );
  if ( _RBTree_Is_red( rbtree1.root ) != 0 )
    puts ( "INIT - ERROR ON RBTREE NULL IS RED MISMATCH" );

  puts( "INIT - Removing 100 nodes" );

  for ( p = rtems_rbtree_get_max(&rbtree1), id = 99 ; p ;
      p = rtems_rbtree_get_max(&rbtree1) , id-- ) {
    test_node *t = RTEMS_CONTAINER_OF(p,test_node,Node);
    if ( id < 0 ) {
      puts( "INIT - TOO MANY NODES ON RBTREE" );
      rtems_test_exit(0);
    }
    if ( t->id != id ) {
      puts( "INIT - ERROR ON RBTREE ID MISMATCH" );
      rtems_test_exit(0);
    }

    if (!rb_assert(rbtree1.root) )
      puts( "INIT - FAILED TREE CHECK" );
  }

  if(!rtems_rbtree_is_empty(&rbtree1)) {
    puts( "INIT - TREE NOT EMPTY" );
    rtems_test_exit(0);
  }

  puts("INIT - Insert 20 random numbers");
  for (i = 0; i < 20; i++) {
    visitor_context ctx = { 0, i + 1, test_insert_trees[ i ] };

    node_array[i].id = numbers[i];
    node_array[i].key = numbers[i];
    rb_insert_unique( &rbtree1, &node_array[i].Node );

    _RBTree_Iterate( &rbtree1, RBT_RIGHT, visit_nodes, &ctx );
    rtems_test_assert( ctx.current == ctx.count );

    if (!rb_assert(rbtree1.root) )
      puts( "INIT - FAILED TREE CHECK" );
  }

  puts( "INIT - Removing 20 nodes" );

  for ( p = rtems_rbtree_get_min(&rbtree1), id = 0 ; p ;
      p = rtems_rbtree_get_min(&rbtree1) , id++ ) {
    test_node *t = RTEMS_CONTAINER_OF(p,test_node,Node);
    if ( id > 19 ) {
      puts( "INIT - TOO MANY NODES ON RBTREE" );
      rtems_test_exit(0);
    }
    if ( t->id != numbers_sorted[id] ) {
      puts( "INIT - ERROR ON RBTREE ID MISMATCH" );
      rtems_test_exit(0);
    }

    if (!rb_assert(rbtree1.root) )
      puts( "INIT - FAILED TREE CHECK" );

    if ( id < 19 ) {
      visitor_context ctx = { 0, 20 - id - 1, test_remove_trees[ id ] };

      _RBTree_Iterate( &rbtree1, RBT_RIGHT, visit_nodes, &ctx );
      rtems_test_assert( ctx.current == ctx.count );
    }
  }

  if(!rtems_rbtree_is_empty(&rbtree1)) {
    puts( "INIT - TREE NOT EMPTY" );
    rtems_test_exit(0);
  }

  puts( "INIT - Verify rtems_rbtree_initialize with 100 nodes value [0,99]" );
  for (i = 0; i < 100; i++) {
    node_array[i].id = i;
    node_array[i].key = i;
  }
  rtems_rbtree_initialize( &rbtree1, &test_compare_function,
                           &node_array[0].Node, 100,
                           sizeof(test_node), true );

  puts( "INIT - Removing 100 nodes" );

  for ( p = rtems_rbtree_get_min(&rbtree1), id = 0 ; p ;
      p = rtems_rbtree_get_min(&rbtree1) , id++ ) {
    test_node *t = RTEMS_CONTAINER_OF(p,test_node,Node);
    if ( id > 99 ) {
      puts( "INIT - TOO MANY NODES ON RBTREE" );
      rtems_test_exit(0);
    }

    if ( t->id != id ) {
      puts( "INIT - ERROR ON RBTREE ID MISMATCH" );
      rtems_test_exit(0);
    }

    if (!rb_assert(rbtree1.root) )
      puts( "INIT - FAILED TREE CHECK" );
  }

  if(!rtems_rbtree_is_empty(&rbtree1)) {
    puts( "INIT - TREE NOT EMPTY" );
    rtems_test_exit(0);
  }

  /* Initialize the tree for duplicate keys */
  puts( "Init - Initialize duplicate rbtree empty" );
  rtems_rbtree_initialize_empty( &rbtree1 );

  puts( "INIT - Verify rtems_rbtree_insert with 100 nodes value [0,99]" );
  for (i = 0; i < 100; i++) {
    node_array[i].id = i;
    node_array[i].key = i%5;
    rb_insert_multi( &rbtree1, &node_array[i].Node );

    if (!rb_assert(rbtree1.root) )
      puts( "INIT - FAILED TREE CHECK" );
  }

  puts( "INIT - Verify rtems_rbtree_find in a duplicate tree" );
  search_node.key = 2;
  p = rb_find_multi(&rbtree1, &search_node.Node);
  if(RTEMS_CONTAINER_OF(p,test_node,Node)->id != 2) {
    puts ("INIT - ERROR ON RBTREE ID MISMATCH");
    rtems_test_exit(0);
  }

  puts( "INIT - Removing 100 nodes" );

  for ( p = rtems_rbtree_get_min(&rbtree1), id = 0 ; p ;
      p = rtems_rbtree_get_min(&rbtree1) , id++ ) {
    test_node *t = RTEMS_CONTAINER_OF(p,test_node,Node);
    if ( id > 99 ) {
      puts( "INIT - TOO MANY NODES ON RBTREE" );
      rtems_test_exit(0);
    }
    if ( t->id != ( ((id*5)%100) + (id/20) ) ) {
      puts( "INIT - ERROR ON RBTREE ID MISMATCH" );
      rtems_test_exit(0);
    }

    if (!rb_assert(rbtree1.root) )
      puts( "INIT - FAILED TREE CHECK" );
  }

  if(!rtems_rbtree_is_empty(&rbtree1)) {
    puts( "INIT - TREE NOT EMPTY" );
    rtems_test_exit(0);
  }

  puts( "INIT - Verify rtems_rbtree_insert with 100 nodes value [99,0]" );
  for (i = 0; i < 100; i++) {
    node_array[i].id = 99-i;
    node_array[i].key = (99-i)%5;
    rb_insert_multi( &rbtree1, &node_array[i].Node );

    if (!rb_assert(rbtree1.root) )
      puts( "INIT - FAILED TREE CHECK" );
  }

  puts( "INIT - Verify rtems_rbtree_find in a duplicate tree" );
  search_node.key = 2;
  p = rb_find_multi(&rbtree1, &search_node.Node);
  if(RTEMS_CONTAINER_OF(p,test_node,Node)->id != 97) {
    puts ("INIT - ERROR ON RBTREE ID MISMATCH");
    rtems_test_exit(0);
  }

  puts( "INIT - Removing 100 nodes" );

  for ( p = rtems_rbtree_get_min(&rbtree1), id = 0 ; p ;
      p = rtems_rbtree_get_min(&rbtree1) , id++ ) {
    test_node *t = RTEMS_CONTAINER_OF(p,test_node,Node);
    if ( id > 99 ) {
      puts( "INIT - TOO MANY NODES ON RBTREE" );
      rtems_test_exit(0);
    }
    if ( t->id != ( (((99-id)*5)%100) + (id/20) ) ) {
      puts( "INIT - ERROR ON RBTREE ID MISMATCH" );
      rtems_test_exit(0);
    }

    if (!rb_assert(rbtree1.root) )
      puts( "INIT - FAILED TREE CHECK" );
  }

  if(!rtems_rbtree_is_empty(&rbtree1)) {
    puts( "INIT - TREE NOT EMPTY" );
    rtems_test_exit(0);
  }

  test_rbtree_min_max();

  TEST_END();
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
