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

#include <linux/rbtree.h>

const char rtems_test_name[] = "SPRBTREE 1";

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);

static void test_rbtree_init_one(void)
{
  RBTree_Control tree;
  RBTree_Node    node;

  puts( "INIT - Initialize one" );

  _RBTree_Initialize_node( &node );
  _RBTree_Initialize_one( &tree, &node );
  rtems_test_assert( !_RBTree_Is_empty( &tree ) );
  rtems_test_assert( _RBTree_Is_root( &node ) );
  rtems_test_assert( !_RBTree_Is_node_off_tree( &node ) );
  rtems_test_assert( _RBTree_Left( &node ) == NULL );
  rtems_test_assert( _RBTree_Right( &node ) == NULL );
  rtems_test_assert( _RBTree_Parent( &node ) == NULL );
  rtems_test_assert( _RBTree_Successor( &node ) == NULL );
  rtems_test_assert( _RBTree_Predecessor( &node ) == NULL );
  rtems_test_assert( _RBTree_Minimum( &tree ) == &node );
  rtems_test_assert( _RBTree_Maximum( &tree ) == &node );

  _RBTree_Extract( &tree, &node );
  rtems_test_assert( _RBTree_Is_empty( &tree ) );
}

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

#define RED RB_RED

#define BLACK RB_BLACK

static int rb_color( const rtems_rbtree_node *n )
{
  return RB_COLOR( n, Node );
}

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
    if ( rb_color( root ) == RED ) {
      if (
        ( ln && rb_color( ln ) == RED )
          || ( rn && rb_color( rn ) == RED )
      ) {
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
      return ( rb_color( root ) == RED ) ? lh : lh + 1;
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
  rtems_test_assert( rb_assert( rtems_rbtree_root( tree ) ) != -1 );
  rtems_test_assert( rtems_rbtree_min( tree ) == min );
  rtems_test_assert( rtems_rbtree_max( tree ) == max );
}

static void min_max_extract(
  rtems_rbtree_control *tree,
  rtems_rbtree_node    *node,
  rtems_rbtree_node    *min,
  rtems_rbtree_node    *max
)
{
  rtems_rbtree_extract( tree, node );
  rtems_test_assert( rb_assert( rtems_rbtree_root( tree ) ) != -1 );
  rtems_test_assert( rtems_rbtree_min( tree ) == min );
  rtems_test_assert( rtems_rbtree_max( tree ) == max );
}

static void test_rbtree_min_max(void)
{
  rtems_rbtree_control  tree;
  rtems_rbtree_node    *node;
  rtems_rbtree_node    *min;
  rtems_rbtree_node    *max;

  puts( "INIT - Verify min/max node updates" );

  rtems_rbtree_initialize_empty( &tree );
  rtems_test_assert( rb_assert( rtems_rbtree_root( &tree ) ) == 1 );

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

static bool test_less(
  const void        *left,
  const RBTree_Node *right
)
{
  const int       *the_left;
  const test_node *the_right;

  the_left = left;
  the_right = RTEMS_CONTAINER_OF( right, test_node, Node );

  return *the_left < the_right->key;
}

static void test_rbtree_insert_inline( void )
{
  RBTree_Control tree;
  test_node      a;
  test_node      b;
  test_node      c;
  int            key;
  bool           is_new_minimum;

  puts( "INIT - Verify _RBTree_Insert_inline" );

  a.key = 1;
  b.key = 2;
  c.key = 3;

  _RBTree_Initialize_empty( &tree );
  _RBTree_Initialize_node( &a.Node );
  _RBTree_Initialize_node( &b.Node );
  _RBTree_Initialize_node( &c.Node );

  key = b.key;
  is_new_minimum = _RBTree_Insert_inline(
    &tree,
    &b.Node,
    &key,
    test_less
  );
  rtems_test_assert( is_new_minimum );

  key = c.key;
  is_new_minimum = _RBTree_Insert_inline(
    &tree,
    &c.Node,
    &key,
    test_less
  );
  rtems_test_assert( !is_new_minimum );

  key = a.key;
  is_new_minimum = _RBTree_Insert_inline(
    &tree,
    &a.Node,
    &key,
    test_less
  );
  rtems_test_assert( is_new_minimum );
}

#define TN( i ) &node_array[ i ].Node

typedef struct {
  int key;
  const rtems_rbtree_node *parent;
  const rtems_rbtree_node *left;
  const rtems_rbtree_node *right;
  int color;
} test_node_description;

static const test_node_description test_insert_tree_0[] = {
  { 52, NULL, NULL, NULL , BLACK }
};

static const test_node_description test_insert_tree_1[] = {
  { 52, NULL, NULL, TN( 1 ) , BLACK },
  { 99, TN( 0 ), NULL, NULL , RED }
};

static const test_node_description test_insert_tree_2[] = {
  { 0, TN( 0 ), NULL, NULL , RED },
  { 52, NULL, TN( 2 ), TN( 1 ) , BLACK },
  { 99, TN( 0 ), NULL, NULL , RED }
};

static const test_node_description test_insert_tree_3[] = {
  { 0, TN( 0 ), NULL, NULL , BLACK },
  { 52, NULL, TN( 2 ), TN( 1 ) , BLACK },
  { 85, TN( 1 ), NULL, NULL , RED },
  { 99, TN( 0 ), TN( 3 ), NULL , BLACK }
};

static const test_node_description test_insert_tree_4[] = {
  { 0, TN( 0 ), NULL, TN( 4 ) , BLACK },
  { 43, TN( 2 ), NULL, NULL , RED },
  { 52, NULL, TN( 2 ), TN( 1 ) , BLACK },
  { 85, TN( 1 ), NULL, NULL , RED },
  { 99, TN( 0 ), TN( 3 ), NULL , BLACK }
};

static const test_node_description test_insert_tree_5[] = {
  { 0, TN( 4 ), NULL, NULL , RED },
  { 43, TN( 0 ), TN( 2 ), TN( 5 ) , BLACK },
  { 44, TN( 4 ), NULL, NULL , RED },
  { 52, NULL, TN( 4 ), TN( 1 ) , BLACK },
  { 85, TN( 1 ), NULL, NULL , RED },
  { 99, TN( 0 ), TN( 3 ), NULL , BLACK }
};

static const test_node_description test_insert_tree_6[] = {
  { 0, TN( 4 ), NULL, TN( 6 ) , BLACK },
  { 10, TN( 2 ), NULL, NULL , RED },
  { 43, TN( 0 ), TN( 2 ), TN( 5 ) , RED },
  { 44, TN( 4 ), NULL, NULL , BLACK },
  { 52, NULL, TN( 4 ), TN( 1 ) , BLACK },
  { 85, TN( 1 ), NULL, NULL , RED },
  { 99, TN( 0 ), TN( 3 ), NULL , BLACK }
};

static const test_node_description test_insert_tree_7[] = {
  { 0, TN( 4 ), NULL, TN( 6 ) , BLACK },
  { 10, TN( 2 ), NULL, NULL , RED },
  { 43, TN( 0 ), TN( 2 ), TN( 5 ) , RED },
  { 44, TN( 4 ), NULL, NULL , BLACK },
  { 52, NULL, TN( 4 ), TN( 3 ) , BLACK },
  { 60, TN( 3 ), NULL, NULL , RED },
  { 85, TN( 0 ), TN( 7 ), TN( 1 ) , BLACK },
  { 99, TN( 3 ), NULL, NULL , RED }
};

static const test_node_description test_insert_tree_8[] = {
  { 0, TN( 4 ), NULL, TN( 6 ) , BLACK },
  { 10, TN( 2 ), NULL, NULL , RED },
  { 43, TN( 0 ), TN( 2 ), TN( 5 ) , RED },
  { 44, TN( 4 ), NULL, TN( 8 ) , BLACK },
  { 50, TN( 5 ), NULL, NULL , RED },
  { 52, NULL, TN( 4 ), TN( 3 ) , BLACK },
  { 60, TN( 3 ), NULL, NULL , RED },
  { 85, TN( 0 ), TN( 7 ), TN( 1 ) , BLACK },
  { 99, TN( 3 ), NULL, NULL , RED }
};

static const test_node_description test_insert_tree_9[] = {
  { 0, TN( 6 ), NULL, NULL , RED },
  { 10, TN( 4 ), TN( 2 ), TN( 9 ) , BLACK },
  { 19, TN( 6 ), NULL, NULL , RED },
  { 43, TN( 0 ), TN( 6 ), TN( 5 ) , RED },
  { 44, TN( 4 ), NULL, TN( 8 ) , BLACK },
  { 50, TN( 5 ), NULL, NULL , RED },
  { 52, NULL, TN( 4 ), TN( 3 ) , BLACK },
  { 60, TN( 3 ), NULL, NULL , RED },
  { 85, TN( 0 ), TN( 7 ), TN( 1 ) , BLACK },
  { 99, TN( 3 ), NULL, NULL , RED }
};

static const test_node_description test_insert_tree_10[] = {
  { 0, TN( 6 ), NULL, TN( 10 ) , BLACK },
  { 8, TN( 2 ), NULL, NULL , RED },
  { 10, TN( 4 ), TN( 2 ), TN( 9 ) , RED },
  { 19, TN( 6 ), NULL, NULL , BLACK },
  { 43, NULL, TN( 6 ), TN( 0 ) , BLACK },
  { 44, TN( 0 ), NULL, TN( 8 ) , BLACK },
  { 50, TN( 5 ), NULL, NULL , RED },
  { 52, TN( 4 ), TN( 5 ), TN( 3 ) , RED },
  { 60, TN( 3 ), NULL, NULL , RED },
  { 85, TN( 0 ), TN( 7 ), TN( 1 ) , BLACK },
  { 99, TN( 3 ), NULL, NULL , RED }
};

static const test_node_description test_insert_tree_11[] = {
  { 0, TN( 6 ), NULL, TN( 10 ) , BLACK },
  { 8, TN( 2 ), NULL, NULL , RED },
  { 10, TN( 4 ), TN( 2 ), TN( 9 ) , BLACK },
  { 19, TN( 6 ), NULL, NULL , BLACK },
  { 43, NULL, TN( 6 ), TN( 0 ) , BLACK },
  { 44, TN( 0 ), NULL, TN( 8 ) , BLACK },
  { 50, TN( 5 ), NULL, NULL , RED },
  { 52, TN( 4 ), TN( 5 ), TN( 3 ) , BLACK },
  { 60, TN( 3 ), NULL, TN( 11 ) , BLACK },
  { 68, TN( 7 ), NULL, NULL , RED },
  { 85, TN( 0 ), TN( 7 ), TN( 1 ) , RED },
  { 99, TN( 3 ), NULL, NULL , BLACK }
};

static const test_node_description test_insert_tree_12[] = {
  { 0, TN( 6 ), NULL, TN( 10 ) , BLACK },
  { 8, TN( 2 ), NULL, NULL , RED },
  { 10, TN( 4 ), TN( 2 ), TN( 9 ) , BLACK },
  { 19, TN( 6 ), NULL, NULL , BLACK },
  { 43, NULL, TN( 6 ), TN( 0 ) , BLACK },
  { 44, TN( 12 ), NULL, NULL , RED },
  { 48, TN( 0 ), TN( 5 ), TN( 8 ) , BLACK },
  { 50, TN( 12 ), NULL, NULL , RED },
  { 52, TN( 4 ), TN( 12 ), TN( 3 ) , BLACK },
  { 60, TN( 3 ), NULL, TN( 11 ) , BLACK },
  { 68, TN( 7 ), NULL, NULL , RED },
  { 85, TN( 0 ), TN( 7 ), TN( 1 ) , RED },
  { 99, TN( 3 ), NULL, NULL , BLACK }
};

static const test_node_description test_insert_tree_13[] = {
  { 0, TN( 6 ), NULL, TN( 10 ) , BLACK },
  { 8, TN( 2 ), NULL, NULL , RED },
  { 10, TN( 4 ), TN( 2 ), TN( 9 ) , BLACK },
  { 19, TN( 6 ), NULL, NULL , BLACK },
  { 43, NULL, TN( 6 ), TN( 0 ) , BLACK },
  { 44, TN( 12 ), NULL, NULL , RED },
  { 48, TN( 0 ), TN( 5 ), TN( 8 ) , BLACK },
  { 50, TN( 12 ), NULL, NULL , RED },
  { 52, TN( 4 ), TN( 12 ), TN( 3 ) , BLACK },
  { 57, TN( 7 ), NULL, NULL , RED },
  { 60, TN( 3 ), TN( 13 ), TN( 11 ) , BLACK },
  { 68, TN( 7 ), NULL, NULL , RED },
  { 85, TN( 0 ), TN( 7 ), TN( 1 ) , RED },
  { 99, TN( 3 ), NULL, NULL , BLACK }
};

static const test_node_description test_insert_tree_14[] = {
  { 0, TN( 6 ), NULL, TN( 10 ) , BLACK },
  { 8, TN( 2 ), NULL, NULL , RED },
  { 10, TN( 4 ), TN( 2 ), TN( 9 ) , BLACK },
  { 17, TN( 9 ), NULL, NULL , RED },
  { 19, TN( 6 ), TN( 14 ), NULL , BLACK },
  { 43, NULL, TN( 6 ), TN( 0 ) , BLACK },
  { 44, TN( 12 ), NULL, NULL , RED },
  { 48, TN( 0 ), TN( 5 ), TN( 8 ) , BLACK },
  { 50, TN( 12 ), NULL, NULL , RED },
  { 52, TN( 4 ), TN( 12 ), TN( 3 ) , BLACK },
  { 57, TN( 7 ), NULL, NULL , RED },
  { 60, TN( 3 ), TN( 13 ), TN( 11 ) , BLACK },
  { 68, TN( 7 ), NULL, NULL , RED },
  { 85, TN( 0 ), TN( 7 ), TN( 1 ) , RED },
  { 99, TN( 3 ), NULL, NULL , BLACK }
};

static const test_node_description test_insert_tree_15[] = {
  { 0, TN( 6 ), NULL, TN( 10 ) , BLACK },
  { 8, TN( 2 ), NULL, NULL , RED },
  { 10, TN( 4 ), TN( 2 ), TN( 9 ) , BLACK },
  { 17, TN( 9 ), NULL, NULL , RED },
  { 19, TN( 6 ), TN( 14 ), NULL , BLACK },
  { 43, NULL, TN( 6 ), TN( 7 ) , BLACK },
  { 44, TN( 12 ), NULL, NULL , RED },
  { 48, TN( 0 ), TN( 5 ), TN( 8 ) , BLACK },
  { 50, TN( 12 ), NULL, NULL , RED },
  { 52, TN( 7 ), TN( 12 ), TN( 13 ) , RED },
  { 57, TN( 0 ), NULL, NULL , BLACK },
  { 60, TN( 4 ), TN( 0 ), TN( 3 ) , BLACK },
  { 67, TN( 11 ), NULL, NULL , RED },
  { 68, TN( 3 ), TN( 15 ), NULL , BLACK },
  { 85, TN( 7 ), TN( 11 ), TN( 1 ) , RED },
  { 99, TN( 3 ), NULL, NULL , BLACK }
};

static const test_node_description test_insert_tree_16[] = {
  { 0, TN( 6 ), NULL, TN( 10 ) , BLACK },
  { 8, TN( 2 ), NULL, NULL , RED },
  { 10, TN( 4 ), TN( 2 ), TN( 9 ) , BLACK },
  { 17, TN( 9 ), NULL, NULL , RED },
  { 19, TN( 6 ), TN( 14 ), NULL , BLACK },
  { 43, NULL, TN( 6 ), TN( 7 ) , BLACK },
  { 44, TN( 12 ), NULL, NULL , RED },
  { 48, TN( 0 ), TN( 5 ), TN( 8 ) , BLACK },
  { 50, TN( 12 ), NULL, NULL , RED },
  { 52, TN( 7 ), TN( 12 ), TN( 13 ) , RED },
  { 57, TN( 0 ), NULL, NULL , BLACK },
  { 60, TN( 4 ), TN( 0 ), TN( 3 ) , BLACK },
  { 67, TN( 11 ), NULL, NULL , RED },
  { 68, TN( 3 ), TN( 15 ), NULL , BLACK },
  { 85, TN( 7 ), TN( 11 ), TN( 1 ) , RED },
  { 90, TN( 1 ), NULL, NULL , RED },
  { 99, TN( 3 ), TN( 16 ), NULL , BLACK }
};

static const test_node_description test_insert_tree_17[] = {
  { 0, TN( 6 ), NULL, TN( 10 ) , BLACK },
  { 8, TN( 2 ), NULL, NULL , RED },
  { 10, TN( 4 ), TN( 2 ), TN( 14 ) , BLACK },
  { 12, TN( 14 ), NULL, NULL , RED },
  { 17, TN( 6 ), TN( 17 ), TN( 9 ) , BLACK },
  { 19, TN( 14 ), NULL, NULL , RED },
  { 43, NULL, TN( 6 ), TN( 7 ) , BLACK },
  { 44, TN( 12 ), NULL, NULL , RED },
  { 48, TN( 0 ), TN( 5 ), TN( 8 ) , BLACK },
  { 50, TN( 12 ), NULL, NULL , RED },
  { 52, TN( 7 ), TN( 12 ), TN( 13 ) , RED },
  { 57, TN( 0 ), NULL, NULL , BLACK },
  { 60, TN( 4 ), TN( 0 ), TN( 3 ) , BLACK },
  { 67, TN( 11 ), NULL, NULL , RED },
  { 68, TN( 3 ), TN( 15 ), NULL , BLACK },
  { 85, TN( 7 ), TN( 11 ), TN( 1 ) , RED },
  { 90, TN( 1 ), NULL, NULL , RED },
  { 99, TN( 3 ), TN( 16 ), NULL , BLACK }
};

static const test_node_description test_insert_tree_18[] = {
  { 0, TN( 6 ), NULL, TN( 10 ) , BLACK },
  { 8, TN( 2 ), NULL, NULL , RED },
  { 10, TN( 4 ), TN( 2 ), TN( 14 ) , BLACK },
  { 12, TN( 14 ), NULL, NULL , RED },
  { 17, TN( 6 ), TN( 17 ), TN( 9 ) , BLACK },
  { 19, TN( 14 ), NULL, NULL , RED },
  { 43, NULL, TN( 6 ), TN( 7 ) , BLACK },
  { 44, TN( 12 ), NULL, NULL , RED },
  { 48, TN( 0 ), TN( 5 ), TN( 8 ) , BLACK },
  { 50, TN( 12 ), NULL, NULL , RED },
  { 52, TN( 7 ), TN( 12 ), TN( 13 ) , RED },
  { 57, TN( 0 ), NULL, NULL , BLACK },
  { 60, TN( 4 ), TN( 0 ), TN( 3 ) , BLACK },
  { 67, TN( 11 ), NULL, NULL , RED },
  { 68, TN( 3 ), TN( 15 ), TN( 18 ) , BLACK },
  { 77, TN( 11 ), NULL, NULL , RED },
  { 85, TN( 7 ), TN( 11 ), TN( 1 ) , RED },
  { 90, TN( 1 ), NULL, NULL , RED },
  { 99, TN( 3 ), TN( 16 ), NULL , BLACK }
};

static const test_node_description test_insert_tree_19[] = {
  { 0, TN( 6 ), NULL, TN( 10 ) , BLACK },
  { 8, TN( 2 ), NULL, NULL , RED },
  { 10, TN( 4 ), TN( 2 ), TN( 14 ) , BLACK },
  { 12, TN( 14 ), NULL, NULL , RED },
  { 17, TN( 6 ), TN( 17 ), TN( 9 ) , BLACK },
  { 19, TN( 14 ), NULL, NULL , RED },
  { 43, NULL, TN( 6 ), TN( 7 ) , BLACK },
  { 44, TN( 12 ), NULL, NULL , RED },
  { 48, TN( 0 ), TN( 5 ), TN( 8 ) , BLACK },
  { 50, TN( 12 ), NULL, NULL , RED },
  { 52, TN( 7 ), TN( 12 ), TN( 13 ) , BLACK },
  { 57, TN( 0 ), NULL, NULL , BLACK },
  { 60, TN( 4 ), TN( 0 ), TN( 3 ) , RED },
  { 67, TN( 11 ), NULL, NULL , BLACK },
  { 68, TN( 3 ), TN( 15 ), TN( 18 ) , RED },
  { 71, TN( 18 ), NULL, NULL , RED },
  { 77, TN( 11 ), TN( 19 ), NULL , BLACK },
  { 85, TN( 7 ), TN( 11 ), TN( 1 ) , BLACK },
  { 90, TN( 1 ), NULL, NULL , RED },
  { 99, TN( 3 ), TN( 16 ), NULL , BLACK }
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
  { 8, TN( 6 ), NULL, NULL , BLACK },
  { 10, TN( 4 ), TN( 10 ), TN( 14 ) , BLACK },
  { 12, TN( 14 ), NULL, NULL , RED },
  { 17, TN( 6 ), TN( 17 ), TN( 9 ) , BLACK },
  { 19, TN( 14 ), NULL, NULL , RED },
  { 43, NULL, TN( 6 ), TN( 7 ) , BLACK },
  { 44, TN( 12 ), NULL, NULL , RED },
  { 48, TN( 0 ), TN( 5 ), TN( 8 ) , BLACK },
  { 50, TN( 12 ), NULL, NULL , RED },
  { 52, TN( 7 ), TN( 12 ), TN( 13 ) , BLACK },
  { 57, TN( 0 ), NULL, NULL , BLACK },
  { 60, TN( 4 ), TN( 0 ), TN( 3 ) , RED },
  { 67, TN( 11 ), NULL, NULL , BLACK },
  { 68, TN( 3 ), TN( 15 ), TN( 18 ) , RED },
  { 71, TN( 18 ), NULL, NULL , RED },
  { 77, TN( 11 ), TN( 19 ), NULL , BLACK },
  { 85, TN( 7 ), TN( 11 ), TN( 1 ) , BLACK },
  { 90, TN( 1 ), NULL, NULL , RED },
  { 99, TN( 3 ), TN( 16 ), NULL , BLACK }
};

static const test_node_description test_remove_tree_1[] = {
  { 10, TN( 14 ), NULL, TN( 17 ) , BLACK },
  { 12, TN( 6 ), NULL, NULL , RED },
  { 17, TN( 4 ), TN( 6 ), TN( 9 ) , BLACK },
  { 19, TN( 14 ), NULL, NULL , BLACK },
  { 43, NULL, TN( 14 ), TN( 7 ) , BLACK },
  { 44, TN( 12 ), NULL, NULL , RED },
  { 48, TN( 0 ), TN( 5 ), TN( 8 ) , BLACK },
  { 50, TN( 12 ), NULL, NULL , RED },
  { 52, TN( 7 ), TN( 12 ), TN( 13 ) , BLACK },
  { 57, TN( 0 ), NULL, NULL , BLACK },
  { 60, TN( 4 ), TN( 0 ), TN( 3 ) , RED },
  { 67, TN( 11 ), NULL, NULL , BLACK },
  { 68, TN( 3 ), TN( 15 ), TN( 18 ) , RED },
  { 71, TN( 18 ), NULL, NULL , RED },
  { 77, TN( 11 ), TN( 19 ), NULL , BLACK },
  { 85, TN( 7 ), TN( 11 ), TN( 1 ) , BLACK },
  { 90, TN( 1 ), NULL, NULL , RED },
  { 99, TN( 3 ), TN( 16 ), NULL , BLACK }
};

static const test_node_description test_remove_tree_2[] = {
  { 12, TN( 14 ), NULL, NULL , BLACK },
  { 17, TN( 4 ), TN( 17 ), TN( 9 ) , BLACK },
  { 19, TN( 14 ), NULL, NULL , BLACK },
  { 43, NULL, TN( 14 ), TN( 7 ) , BLACK },
  { 44, TN( 12 ), NULL, NULL , RED },
  { 48, TN( 0 ), TN( 5 ), TN( 8 ) , BLACK },
  { 50, TN( 12 ), NULL, NULL , RED },
  { 52, TN( 7 ), TN( 12 ), TN( 13 ) , BLACK },
  { 57, TN( 0 ), NULL, NULL , BLACK },
  { 60, TN( 4 ), TN( 0 ), TN( 3 ) , RED },
  { 67, TN( 11 ), NULL, NULL , BLACK },
  { 68, TN( 3 ), TN( 15 ), TN( 18 ) , RED },
  { 71, TN( 18 ), NULL, NULL , RED },
  { 77, TN( 11 ), TN( 19 ), NULL , BLACK },
  { 85, TN( 7 ), TN( 11 ), TN( 1 ) , BLACK },
  { 90, TN( 1 ), NULL, NULL , RED },
  { 99, TN( 3 ), TN( 16 ), NULL , BLACK }
};

static const test_node_description test_remove_tree_3[] = {
  { 17, TN( 4 ), NULL, TN( 9 ) , BLACK },
  { 19, TN( 14 ), NULL, NULL , RED },
  { 43, TN( 7 ), TN( 14 ), TN( 0 ) , BLACK },
  { 44, TN( 12 ), NULL, NULL , RED },
  { 48, TN( 0 ), TN( 5 ), TN( 8 ) , BLACK },
  { 50, TN( 12 ), NULL, NULL , RED },
  { 52, TN( 4 ), TN( 12 ), TN( 13 ) , RED },
  { 57, TN( 0 ), NULL, NULL , BLACK },
  { 60, NULL, TN( 4 ), TN( 3 ) , BLACK },
  { 67, TN( 11 ), NULL, NULL , BLACK },
  { 68, TN( 3 ), TN( 15 ), TN( 18 ) , RED },
  { 71, TN( 18 ), NULL, NULL , RED },
  { 77, TN( 11 ), TN( 19 ), NULL , BLACK },
  { 85, TN( 7 ), TN( 11 ), TN( 1 ) , BLACK },
  { 90, TN( 1 ), NULL, NULL , RED },
  { 99, TN( 3 ), TN( 16 ), NULL , BLACK }
};

static const test_node_description test_remove_tree_4[] = {
  { 19, TN( 4 ), NULL, NULL , BLACK },
  { 43, TN( 7 ), TN( 9 ), TN( 0 ) , BLACK },
  { 44, TN( 12 ), NULL, NULL , RED },
  { 48, TN( 0 ), TN( 5 ), TN( 8 ) , BLACK },
  { 50, TN( 12 ), NULL, NULL , RED },
  { 52, TN( 4 ), TN( 12 ), TN( 13 ) , RED },
  { 57, TN( 0 ), NULL, NULL , BLACK },
  { 60, NULL, TN( 4 ), TN( 3 ) , BLACK },
  { 67, TN( 11 ), NULL, NULL , BLACK },
  { 68, TN( 3 ), TN( 15 ), TN( 18 ) , RED },
  { 71, TN( 18 ), NULL, NULL , RED },
  { 77, TN( 11 ), TN( 19 ), NULL , BLACK },
  { 85, TN( 7 ), TN( 11 ), TN( 1 ) , BLACK },
  { 90, TN( 1 ), NULL, NULL , RED },
  { 99, TN( 3 ), TN( 16 ), NULL , BLACK }
};

static const test_node_description test_remove_tree_5[] = {
  { 43, TN( 12 ), NULL, TN( 5 ) , BLACK },
  { 44, TN( 4 ), NULL, NULL , RED },
  { 48, TN( 0 ), TN( 4 ), TN( 8 ) , RED },
  { 50, TN( 12 ), NULL, NULL , BLACK },
  { 52, TN( 7 ), TN( 12 ), TN( 13 ) , BLACK },
  { 57, TN( 0 ), NULL, NULL , BLACK },
  { 60, NULL, TN( 0 ), TN( 3 ) , BLACK },
  { 67, TN( 11 ), NULL, NULL , BLACK },
  { 68, TN( 3 ), TN( 15 ), TN( 18 ) , RED },
  { 71, TN( 18 ), NULL, NULL , RED },
  { 77, TN( 11 ), TN( 19 ), NULL , BLACK },
  { 85, TN( 7 ), TN( 11 ), TN( 1 ) , BLACK },
  { 90, TN( 1 ), NULL, NULL , RED },
  { 99, TN( 3 ), TN( 16 ), NULL , BLACK }
};

static const test_node_description test_remove_tree_6[] = {
  { 44, TN( 12 ), NULL, NULL , BLACK },
  { 48, TN( 0 ), TN( 5 ), TN( 8 ) , RED },
  { 50, TN( 12 ), NULL, NULL , BLACK },
  { 52, TN( 7 ), TN( 12 ), TN( 13 ) , BLACK },
  { 57, TN( 0 ), NULL, NULL , BLACK },
  { 60, NULL, TN( 0 ), TN( 3 ) , BLACK },
  { 67, TN( 11 ), NULL, NULL , BLACK },
  { 68, TN( 3 ), TN( 15 ), TN( 18 ) , RED },
  { 71, TN( 18 ), NULL, NULL , RED },
  { 77, TN( 11 ), TN( 19 ), NULL , BLACK },
  { 85, TN( 7 ), TN( 11 ), TN( 1 ) , BLACK },
  { 90, TN( 1 ), NULL, NULL , RED },
  { 99, TN( 3 ), TN( 16 ), NULL , BLACK }
};

static const test_node_description test_remove_tree_7[] = {
  { 48, TN( 0 ), NULL, TN( 8 ) , BLACK },
  { 50, TN( 12 ), NULL, NULL , RED },
  { 52, TN( 7 ), TN( 12 ), TN( 13 ) , BLACK },
  { 57, TN( 0 ), NULL, NULL , BLACK },
  { 60, NULL, TN( 0 ), TN( 3 ) , BLACK },
  { 67, TN( 11 ), NULL, NULL , BLACK },
  { 68, TN( 3 ), TN( 15 ), TN( 18 ) , RED },
  { 71, TN( 18 ), NULL, NULL , RED },
  { 77, TN( 11 ), TN( 19 ), NULL , BLACK },
  { 85, TN( 7 ), TN( 11 ), TN( 1 ) , BLACK },
  { 90, TN( 1 ), NULL, NULL , RED },
  { 99, TN( 3 ), TN( 16 ), NULL , BLACK }
};

static const test_node_description test_remove_tree_8[] = {
  { 50, TN( 0 ), NULL, NULL , BLACK },
  { 52, TN( 7 ), TN( 8 ), TN( 13 ) , BLACK },
  { 57, TN( 0 ), NULL, NULL , BLACK },
  { 60, NULL, TN( 0 ), TN( 3 ) , BLACK },
  { 67, TN( 11 ), NULL, NULL , BLACK },
  { 68, TN( 3 ), TN( 15 ), TN( 18 ) , RED },
  { 71, TN( 18 ), NULL, NULL , RED },
  { 77, TN( 11 ), TN( 19 ), NULL , BLACK },
  { 85, TN( 7 ), TN( 11 ), TN( 1 ) , BLACK },
  { 90, TN( 1 ), NULL, NULL , RED },
  { 99, TN( 3 ), TN( 16 ), NULL , BLACK }
};

static const test_node_description test_remove_tree_9[] = {
  { 52, TN( 7 ), NULL, TN( 13 ) , BLACK },
  { 57, TN( 0 ), NULL, NULL , RED },
  { 60, TN( 11 ), TN( 0 ), TN( 15 ) , BLACK },
  { 67, TN( 7 ), NULL, NULL , BLACK },
  { 68, NULL, TN( 7 ), TN( 3 ) , BLACK },
  { 71, TN( 18 ), NULL, NULL , RED },
  { 77, TN( 3 ), TN( 19 ), NULL , BLACK },
  { 85, TN( 11 ), TN( 18 ), TN( 1 ) , BLACK },
  { 90, TN( 1 ), NULL, NULL , RED },
  { 99, TN( 3 ), TN( 16 ), NULL , BLACK }
};

static const test_node_description test_remove_tree_10[] = {
  { 57, TN( 7 ), NULL, NULL , BLACK },
  { 60, TN( 11 ), TN( 13 ), TN( 15 ) , BLACK },
  { 67, TN( 7 ), NULL, NULL , BLACK },
  { 68, NULL, TN( 7 ), TN( 3 ) , BLACK },
  { 71, TN( 18 ), NULL, NULL , RED },
  { 77, TN( 3 ), TN( 19 ), NULL , BLACK },
  { 85, TN( 11 ), TN( 18 ), TN( 1 ) , BLACK },
  { 90, TN( 1 ), NULL, NULL , RED },
  { 99, TN( 3 ), TN( 16 ), NULL , BLACK }
};

static const test_node_description test_remove_tree_11[] = {
  { 60, TN( 11 ), NULL, TN( 15 ) , BLACK },
  { 67, TN( 7 ), NULL, NULL , RED },
  { 68, NULL, TN( 7 ), TN( 3 ) , BLACK },
  { 71, TN( 18 ), NULL, NULL , RED },
  { 77, TN( 3 ), TN( 19 ), NULL , BLACK },
  { 85, TN( 11 ), TN( 18 ), TN( 1 ) , RED },
  { 90, TN( 1 ), NULL, NULL , RED },
  { 99, TN( 3 ), TN( 16 ), NULL , BLACK }
};

static const test_node_description test_remove_tree_12[] = {
  { 67, TN( 11 ), NULL, NULL , BLACK },
  { 68, NULL, TN( 15 ), TN( 3 ) , BLACK },
  { 71, TN( 18 ), NULL, NULL , RED },
  { 77, TN( 3 ), TN( 19 ), NULL , BLACK },
  { 85, TN( 11 ), TN( 18 ), TN( 1 ) , RED },
  { 90, TN( 1 ), NULL, NULL , RED },
  { 99, TN( 3 ), TN( 16 ), NULL , BLACK }
};

static const test_node_description test_remove_tree_13[] = {
  { 68, TN( 19 ), NULL, NULL , BLACK },
  { 71, TN( 3 ), TN( 11 ), TN( 18 ) , RED },
  { 77, TN( 19 ), NULL, NULL , BLACK },
  { 85, NULL, TN( 19 ), TN( 1 ) , BLACK },
  { 90, TN( 1 ), NULL, NULL , RED },
  { 99, TN( 3 ), TN( 16 ), NULL , BLACK }
};

static const test_node_description test_remove_tree_14[] = {
  { 71, TN( 3 ), NULL, TN( 18 ) , BLACK },
  { 77, TN( 19 ), NULL, NULL , RED },
  { 85, NULL, TN( 19 ), TN( 1 ) , BLACK },
  { 90, TN( 1 ), NULL, NULL , RED },
  { 99, TN( 3 ), TN( 16 ), NULL , BLACK }
};

static const test_node_description test_remove_tree_15[] = {
  { 77, TN( 3 ), NULL, NULL , BLACK },
  { 85, NULL, TN( 18 ), TN( 1 ) , BLACK },
  { 90, TN( 1 ), NULL, NULL , RED },
  { 99, TN( 3 ), TN( 16 ), NULL , BLACK }
};

static const test_node_description test_remove_tree_16[] = {
  { 85, TN( 16 ), NULL, NULL , BLACK },
  { 90, NULL, TN( 3 ), TN( 1 ) , BLACK },
  { 99, TN( 16 ), NULL, NULL , BLACK }
};

static const test_node_description test_remove_tree_17[] = {
  { 90, NULL, NULL, TN( 1 ) , BLACK },
  { 99, TN( 16 ), NULL, NULL , RED }
};

static const test_node_description test_remove_tree_18[] = {
  { 99, NULL, NULL, NULL , BLACK }
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
  void              *visitor_arg
)
{
  visitor_context *ctx = visitor_arg;
  const test_node_description *td = &ctx->tree[ ctx->current ];
  const test_node *tn = RTEMS_CONTAINER_OF( node, test_node, Node );

  rtems_test_assert( ctx->current < ctx->count );

  rtems_test_assert( td->key == tn->key );

  if ( td->parent == NULL ) {
    rtems_test_assert( rtems_rbtree_is_root( &tn->Node ) );
  } else {
    rtems_test_assert( td->parent == rtems_rbtree_parent( &tn->Node ) );
  }

  rtems_test_assert( td->left == rtems_rbtree_left( &tn->Node ) );
  rtems_test_assert( td->right == rtems_rbtree_right( &tn->Node ) );
  rtems_test_assert( td->color == rb_color( &tn->Node ) );

  ++ctx->current;

  return false;
}

static const test_node_description random_ops_tree_unique_1[] = {
  { 0, NULL, NULL, NULL, BLACK }
};

static const test_node_description random_ops_tree_multiple_1[] = {
  { 0, NULL, NULL, NULL, BLACK }
};

static const test_node_description random_ops_tree_unique_2[] = {
};

static const test_node_description random_ops_tree_multiple_2[] = {
};

static const test_node_description random_ops_tree_unique_3[] = {
  { 2, NULL, NULL, NULL, BLACK }
};

static const test_node_description random_ops_tree_multiple_3[] = {
  { 1, NULL, NULL, NULL, BLACK }
};

static const test_node_description random_ops_tree_unique_4[] = {
  { 0, TN(3), NULL, NULL, RED },
  { 3, NULL, TN(0), NULL, BLACK }
};

static const test_node_description random_ops_tree_multiple_4[] = {
  { 0, NULL, NULL, TN( 3 ), BLACK },
  { 1, TN( 0 ), NULL, NULL, RED }
};

static const test_node_description random_ops_tree_unique_5[] = {
  { 0, TN( 1 ), NULL, NULL, RED },
  { 1, NULL, TN( 0 ), TN( 4 ), BLACK },
  { 4, TN( 1 ), NULL, NULL, RED }
};

static const test_node_description random_ops_tree_multiple_5[] = {
  { 0, TN( 1 ), NULL, NULL, RED },
  { 0, NULL, TN( 0 ), TN( 4 ), BLACK },
  { 2, TN( 1 ), NULL, NULL, RED }
};

static const test_node_description random_ops_tree_unique_6[] = {
  { 0, TN( 2 ), NULL, NULL, RED },
  { 2, NULL, TN( 0 ), NULL, BLACK }
};

static const test_node_description random_ops_tree_multiple_6[] = {
  { 0, TN( 2 ), NULL, NULL, RED },
  { 1, NULL, TN( 0 ), NULL, BLACK }
};

static const test_node_description random_ops_tree_unique_7[] = {
  { 0, TN( 2 ), NULL, TN( 1 ), BLACK },
  { 1, TN( 0 ), NULL, NULL, RED },
  { 2, NULL, TN( 0 ), TN( 5 ), BLACK },
  { 4, TN( 5 ), NULL, NULL, RED },
  { 5, TN( 2 ), TN( 4 ), NULL, BLACK }
};

static const test_node_description random_ops_tree_multiple_7[] = {
  { 0, TN( 2 ), NULL, TN( 1 ), BLACK },
  { 0, TN( 0 ), NULL, NULL, RED },
  { 1, NULL, TN( 0 ), TN( 4 ), BLACK },
  { 2, TN( 4 ), NULL, NULL, RED },
  { 2, TN( 2 ), TN( 5 ), NULL, BLACK }
};

static const test_node_description random_ops_tree_unique_8[] = {
  { 0, TN(1), NULL, NULL, BLACK },
  { 1, NULL, TN(0), TN(6), BLACK },
  { 5, TN(6), NULL, NULL, RED },
  { 6, TN(1), TN(5), NULL, BLACK }
};

static const test_node_description random_ops_tree_multiple_8[] = {
  { 0, TN( 5 ), NULL, TN( 0 ), BLACK },
  { 0, TN( 1 ), NULL, NULL, RED },
  { 2, NULL, TN( 1 ), TN( 6 ), BLACK },
  { 3, TN( 5 ), NULL, NULL, BLACK }
};

static const test_node_description random_ops_tree_unique_9[] = {
  { 1, TN( 2 ), NULL, NULL, BLACK },
  { 2, TN( 6 ), TN( 1 ), TN( 4 ), RED },
  { 4, TN( 2 ), NULL, TN( 5 ), BLACK },
  { 5, TN( 4 ), NULL, NULL, RED },
  { 6, NULL, TN( 2 ), TN( 7 ), BLACK },
  { 7, TN( 6 ), NULL, TN( 8 ), BLACK },
  { 8, TN( 7 ), NULL, NULL, RED }
};

static const test_node_description random_ops_tree_multiple_9[] = {
  { 0, TN( 2 ), NULL, NULL, BLACK },
  { 1, TN( 6 ), TN( 1 ), TN( 4 ), RED },
  { 2, TN( 2 ), NULL, TN( 5 ), BLACK },
  { 2, TN( 4 ), NULL, NULL, RED },
  { 3, NULL, TN( 2 ), TN( 7 ), BLACK },
  { 3, TN( 6 ), NULL, TN( 8 ), BLACK },
  { 4, TN( 7 ), NULL, NULL, RED }
};

static const test_node_description random_ops_tree_unique_10[] = {
  { 0, TN( 2 ), NULL, NULL, BLACK },
  { 2, TN( 6 ), TN( 0 ), TN( 4 ), RED },
  { 3, TN( 4 ), NULL, NULL, RED },
  { 4, TN( 2 ), TN( 3 ), NULL, BLACK },
  { 6, NULL, TN( 2 ), TN( 8 ), BLACK },
  { 8, TN( 6 ), NULL, NULL, BLACK }
};

static const test_node_description random_ops_tree_multiple_10[] = {
  { 0, TN( 2 ), NULL, NULL, BLACK },
  { 1, TN( 6 ), TN( 0 ), TN( 4 ), RED },
  { 1, TN( 4 ), NULL, NULL, RED },
  { 2, TN( 2 ), TN( 3 ), NULL, BLACK },
  { 3, NULL, TN( 2 ), TN( 8 ), BLACK },
  { 4, TN( 6 ), NULL, NULL, BLACK }
};

static const test_node_description random_ops_tree_unique_11[] = {
  { 2, TN( 6 ), NULL, NULL, BLACK },
  { 6, NULL, TN( 2 ), TN( 8 ), BLACK },
  { 7, TN( 8 ), NULL, NULL, RED },
  { 8, TN( 6 ), TN( 7 ), TN( 9 ), BLACK },
  { 9, TN( 8 ), NULL, NULL, RED }
};

static const test_node_description random_ops_tree_multiple_11[] = {
  { 1, TN( 6 ), NULL, NULL, BLACK },
  { 3, NULL, TN( 2 ), TN( 8 ), BLACK },
  { 3, TN( 8 ), NULL, NULL, RED },
  { 4, TN( 6 ), TN( 7 ), TN( 9 ), BLACK },
  { 4, TN( 8 ), NULL, NULL, RED }
};

static const test_node_description random_ops_tree_unique_12[] = {
  { 0, TN( 1 ), NULL, NULL, RED },
  { 1, TN( 3 ), TN( 0 ), TN( 2 ), BLACK },
  { 2, TN( 1 ), NULL, NULL, RED },
  { 3, TN( 5 ), TN( 1 ), TN( 4 ), RED },
  { 4, TN( 3 ), NULL, NULL, BLACK },
  { 5, NULL, TN( 3 ), TN( 9 ), BLACK },
  { 9, TN( 5 ), NULL, TN( 11 ), BLACK },
  { 11, TN( 9 ), NULL, NULL, RED }
};

static const test_node_description random_ops_tree_multiple_12[] = {
  { 0, TN( 1 ), NULL, NULL, BLACK },
  { 0, TN( 5 ), TN( 0 ), TN( 3 ), RED },
  { 1, TN( 1 ), NULL, TN( 2 ), BLACK },
  { 1, TN( 3 ), NULL, NULL, RED },
  { 2, NULL, TN( 1 ), TN( 9 ), BLACK },
  { 2, TN( 9 ), NULL, NULL, BLACK },
  { 4, TN( 5 ), TN( 4 ), TN( 11 ), RED },
  { 5, TN( 9 ), NULL, NULL, BLACK }
};

static const test_node_description random_ops_tree_unique_13[] = {
  { 0, TN(1), NULL, NULL, RED },
  { 1, TN(3), TN(0), NULL, BLACK },
  { 3, TN(8), TN(1), TN(5), RED },
  { 4, TN(5), NULL, NULL, RED },
  { 5, TN(3), TN(4), TN(6), BLACK },
  { 6, TN(5), NULL, NULL, RED },
  { 8, NULL, TN(3), TN(11), BLACK },
  { 10, TN(11), NULL, NULL, RED },
  { 11, TN(8), TN(10), NULL, BLACK }
};

static const test_node_description random_ops_tree_multiple_13[] = {
  { 0, TN(0), NULL, NULL, RED },
  { 0, TN(3), TN(1), NULL, BLACK },
  { 1, TN(6), TN(0), TN(4), RED },
  { 2, TN(3), NULL, TN(5), BLACK },
  { 2, TN(4), NULL, NULL, RED },
  { 3, NULL, TN(3), TN(11), BLACK },
  { 4, TN(11), NULL, NULL, RED },
  { 5, TN(6), TN(8), TN(10), BLACK },
  { 5, TN(11), NULL, NULL, RED }
};

static const test_node_description random_ops_tree_unique_14[] = {
  { 3, TN(5), NULL, NULL, RED },
  { 5, TN(6), TN(3), NULL, BLACK },
  { 6, NULL, TN(5), TN(12), BLACK },
  { 8, TN(12), NULL, NULL, BLACK },
  { 12, TN(6), TN(8), TN(13), RED },
  { 13, TN(12), NULL, NULL, BLACK }
};

static const test_node_description random_ops_tree_multiple_14[] = {
  { 1, TN( 5 ), NULL, NULL, RED },
  { 2, TN( 6 ), TN( 3 ), NULL, BLACK },
  { 3, NULL, TN( 5 ), TN( 13 ), BLACK },
  { 4, TN( 13 ), NULL, NULL, BLACK },
  { 6, TN( 6 ), TN( 8 ), TN( 12 ), RED },
  { 6, TN( 13 ), NULL, NULL, BLACK }
};

static const test_node_description random_ops_tree_unique_15[] = {
  { 0, TN(2), NULL, NULL, RED },
  { 2, TN(8), TN(0), TN(7), BLACK },
  { 7, TN(2), NULL, NULL, RED },
  { 8, NULL, TN(2), TN(12), BLACK },
  { 9, TN(12), NULL, TN(10), BLACK },
  { 10, TN(9), NULL, NULL, RED },
  { 12, TN(8), TN(9), TN(13), RED },
  { 13, TN(12), NULL, TN(14), BLACK },
  { 14, TN(13), NULL, NULL, RED }
};

static const test_node_description random_ops_tree_multiple_15[] = {
  { 0, TN(2), NULL, NULL, RED },
  { 1, TN(9), TN(0), TN(7), BLACK },
  { 3, TN(2), NULL, NULL, RED },
  { 4, NULL, TN(2), TN(10), BLACK },
  { 4, TN(10), NULL, NULL, BLACK },
  { 5, TN(9), TN(8), TN(12), RED },
  { 6, TN(12), NULL, NULL, RED },
  { 6, TN(10), TN(13), TN(14), BLACK },
  { 7, TN(12), NULL, NULL, RED }
};

static const test_node_description random_ops_tree_unique_16[] = {
  { 0, TN(5), NULL, TN(3), BLACK },
  { 3, TN(0), NULL, NULL, RED },
  { 5, TN(10), TN(0), TN(7), RED },
  { 7, TN(5), NULL, NULL, BLACK },
  { 10, NULL, TN(5), TN(12), BLACK },
  { 12, TN(10), NULL, NULL, BLACK }
};

static const test_node_description random_ops_tree_multiple_16[] = {
  { 0, TN(5), NULL, TN(3), BLACK },
  { 1, TN(0), NULL, NULL, RED },
  { 2, TN(10), TN(0), TN(7), RED },
  { 3, TN(5), NULL, NULL, BLACK },
  { 5, NULL, TN(5), TN(12), BLACK },
  { 6, TN(10), NULL, NULL, BLACK }
};

static const test_node_description random_ops_tree_unique_17[] = {
  { 0, TN(1), NULL, NULL, RED },
  { 1, TN(3), TN(0), NULL, BLACK },
  { 3, TN(7), TN(1), TN(5), RED },
  { 4, TN(5), NULL, NULL, RED },
  { 5, TN(3), TN(4), NULL, BLACK },
  { 7, NULL, TN(3), TN(9), BLACK },
  { 8, TN(9), NULL, NULL, BLACK },
  { 9, TN(7), TN(8), TN(16), RED },
  { 16, TN(9), NULL, NULL, BLACK }
};

static const test_node_description random_ops_tree_multiple_17[] = {
  { 0, TN(0), NULL, NULL, RED },
  { 0, TN(3), TN(1), NULL, BLACK },
  { 1, TN(7), TN(0), TN(5), RED },
  { 2, TN(3), NULL, TN(4), BLACK },
  { 2, TN(5), NULL, NULL, RED },
  { 3, NULL, TN(3), TN(8), BLACK },
  { 4, TN(8), NULL, NULL, BLACK },
  { 4, TN(7), TN(9), TN(16), RED },
  { 8, TN(8), NULL, NULL, BLACK }
};

static const test_node_description random_ops_tree_unique_18[] = {
  { 0, TN(2), NULL, TN(1), BLACK },
  { 1, TN(0), NULL, NULL, RED },
  { 2, TN(4), TN(0), TN(3), BLACK },
  { 3, TN(2), NULL, NULL, BLACK },
  { 4, NULL, TN(2), TN(12), BLACK },
  { 5, TN(6), NULL, NULL, RED },
  { 6, TN(8), TN(5), TN(7), BLACK },
  { 7, TN(6), NULL, NULL, RED },
  { 8, TN(12), TN(6), TN(10), RED },
  { 9, TN(10), NULL, NULL, RED },
  { 10, TN(8), TN(9), NULL, BLACK },
  { 12, TN(4), TN(8), TN(17), BLACK },
  { 14, TN(17), NULL, NULL, RED },
  { 17, TN(12), TN(14), NULL, BLACK }
};

static const test_node_description random_ops_tree_multiple_18[] = {
  { 0, TN(3), NULL, TN(1), BLACK },
  { 0, TN(0), NULL, NULL, RED },
  { 1, TN(4), TN(0), TN(2), BLACK },
  { 1, TN(3), NULL, NULL, BLACK },
  { 2, NULL, TN(3), TN(12), BLACK },
  { 2, TN(6), NULL, NULL, RED },
  { 3, TN(8), TN(5), TN(7), BLACK },
  { 3, TN(6), NULL, NULL, RED },
  { 4, TN(12), TN(6), TN(10), RED },
  { 4, TN(10), NULL, NULL, RED },
  { 5, TN(8), TN(9), NULL, BLACK },
  { 6, TN(4), TN(8), TN(14), BLACK },
  { 7, TN(12), NULL, TN(17), BLACK },
  { 8, TN(14), NULL, NULL, RED }
};

static const test_node_description random_ops_tree_unique_19[] = {
  { 1, TN(2), NULL, NULL, RED },
  { 2, TN(6), TN(1), NULL, BLACK },
  { 6, TN(11), TN(2), TN(8), BLACK },
  { 8, TN(6), NULL, TN(9), BLACK },
  { 9, TN(8), NULL, NULL, RED },
  { 11, NULL, TN(6), TN(14), BLACK },
  { 12, TN(14), NULL, NULL, BLACK },
  { 14, TN(11), TN(12), TN(16), BLACK },
  { 16, TN(14), NULL, NULL, BLACK }
};

static const test_node_description random_ops_tree_multiple_19[] = {
  { 0, TN(2), NULL, NULL, RED },
  { 1, TN(6), TN(1), NULL, BLACK },
  { 3, TN(11), TN(2), TN(9), BLACK },
  { 4, TN(6), NULL, TN(8), BLACK },
  { 4, TN(9), NULL, NULL, RED },
  { 5, NULL, TN(6), TN(14), BLACK },
  { 6, TN(14), NULL, NULL, BLACK },
  { 7, TN(11), TN(12), TN(16), BLACK },
  { 8, TN(14), NULL, NULL, BLACK }
};

static const test_node_description random_ops_tree_unique_20[] = {
  { 0, TN(3), NULL, TN(1), BLACK },
  { 1, TN(0), NULL, NULL, RED },
  { 3, TN(9), TN(0), TN(7), BLACK },
  { 4, TN(7), NULL, NULL, RED },
  { 7, TN(3), TN(4), NULL, BLACK },
  { 9, NULL, TN(3), TN(12), BLACK },
  { 10, TN(12), NULL, NULL, BLACK },
  { 12, TN(9), TN(10), TN(17), BLACK },
  { 14, TN(17), NULL, NULL, BLACK },
  { 17, TN(12), TN(14), TN(18), RED },
  { 18, TN(17), NULL, TN(19), BLACK },
  { 19, TN(18), NULL, NULL, RED }
};

static const test_node_description random_ops_tree_multiple_20[] = {
  { 0, TN(3), NULL, TN(1), BLACK },
  { 0, TN(0), NULL, NULL, RED },
  { 1, TN(9), TN(0), TN(7), BLACK },
  { 2, TN(7), NULL, NULL, RED },
  { 3, TN(3), TN(4), NULL, BLACK },
  { 4, NULL, TN(3), TN(14), BLACK },
  { 5, TN(14), NULL, TN(12), BLACK },
  { 6, TN(10), NULL, NULL, RED },
  { 7, TN(9), TN(10), TN(18), BLACK },
  { 8, TN(18), NULL, NULL, RED },
  { 9, TN(14), TN(17), TN(19), BLACK },
  { 9, TN(18), NULL, NULL, RED }
};

static const test_node_description random_ops_tree_unique_21[] = {
  { 0, TN(3), NULL, TN(1), BLACK },
  { 1, TN(0), NULL, NULL, RED },
  { 3, TN(11), TN(0), TN(5), BLACK },
  { 4, TN(5), NULL, NULL, BLACK },
  { 5, TN(3), TN(4), TN(8), RED },
  { 8, TN(5), NULL, NULL, BLACK },
  { 11, NULL, TN(3), TN(15), BLACK },
  { 13, TN(15), NULL, NULL, BLACK },
  { 15, TN(11), TN(13), TN(17), BLACK },
  { 16, TN(17), NULL, NULL, RED },
  { 17, TN(15), TN(16), NULL, BLACK }
};

static const test_node_description random_ops_tree_multiple_21[] = {
  { 0, TN(3), NULL, TN(1), BLACK },
  { 0, TN(0), NULL, NULL, RED },
  { 1, TN(8), TN(0), TN(4), BLACK },
  { 2, TN(3), NULL, TN(5), BLACK },
  { 2, TN(4), NULL, NULL, RED },
  { 4, NULL, TN(3), TN(13), BLACK },
  { 5, TN(13), NULL, NULL, BLACK },
  { 6, TN(8), TN(11), TN(17), BLACK },
  { 7, TN(17), NULL, NULL, BLACK },
  { 8, TN(13), TN(15), TN(16), RED },
  { 8, TN(17), NULL, NULL, BLACK }
};

static const test_node_description random_ops_tree_unique_22[] = {
  { 1, TN(3), NULL, TN(2), BLACK },
  { 2, TN(1), NULL, NULL, RED },
  { 3, TN(8), TN(1), TN(7), BLACK },
  { 4, TN(7), NULL, NULL, RED },
  { 7, TN(3), TN(4), NULL, BLACK },
  { 8, NULL, TN(3), TN(14), BLACK },
  { 10, TN(11), NULL, NULL, RED },
  { 11, TN(14), TN(10), NULL, BLACK },
  { 14, TN(8), TN(11), TN(18), BLACK },
  { 15, TN(18), NULL, NULL, BLACK },
  { 18, TN(14), TN(15), TN(21), RED },
  { 21, TN(18), NULL, NULL, BLACK }
};

static const test_node_description random_ops_tree_multiple_22[] = {
  { 0, TN(3), NULL, NULL, BLACK },
  { 1, TN(8), TN(1), TN(4), BLACK },
  { 1, TN(4), NULL, NULL, BLACK },
  { 2, TN(3), TN(2), TN(7), RED },
  { 3, TN(4), NULL, NULL, BLACK },
  { 4, NULL, TN(3), TN(14), BLACK },
  { 5, TN(14), NULL, TN(10), BLACK },
  { 5, TN(11), NULL, NULL, RED },
  { 7, TN(8), TN(11), TN(18), BLACK },
  { 7, TN(18), NULL, NULL, BLACK },
  { 9, TN(14), TN(15), TN(21), RED },
  { 10, TN(18), NULL, NULL, BLACK }
};

static const test_node_description random_ops_tree_unique_23[] = {
  { 0, TN(2), NULL, NULL, RED },
  { 2, TN(8), TN(0), TN(7), BLACK },
  { 7, TN(2), NULL, NULL, RED },
  { 8, TN(12), TN(2), TN(11), BLACK },
  { 11, TN(8), NULL, NULL, BLACK },
  { 12, NULL, TN(8), TN(17), BLACK },
  { 13, TN(15), NULL, TN(14), BLACK },
  { 14, TN(13), NULL, NULL, RED },
  { 15, TN(17), TN(13), TN(16), RED },
  { 16, TN(15), NULL, NULL, BLACK },
  { 17, TN(12), TN(15), TN(20), BLACK },
  { 20, TN(17), NULL, TN(21), BLACK },
  { 21, TN(20), NULL, NULL, RED }
};

static const test_node_description random_ops_tree_multiple_23[] = {
  { 0, TN(2), NULL, NULL, RED },
  { 1, TN(8), TN(0), TN(7), BLACK },
  { 3, TN(2), NULL, NULL, RED },
  { 4, TN(12), TN(2), TN(11), BLACK },
  { 5, TN(8), NULL, NULL, BLACK },
  { 6, NULL, TN(8), TN(17), BLACK },
  { 6, TN(15), NULL, NULL, BLACK },
  { 7, TN(17), TN(13), TN(16), RED },
  { 7, TN(16), NULL, NULL, RED },
  { 8, TN(15), TN(14), NULL, BLACK },
  { 8, TN(12), TN(15), TN(20), BLACK },
  { 10, TN(17), NULL, TN(21), BLACK },
  { 10, TN(20), NULL, NULL, RED }
};

static const test_node_description random_ops_tree_unique_24[] = {
  { 4, TN(6), NULL, TN(5), BLACK },
  { 5, TN(4), NULL, NULL, RED },
  { 6, TN(14), TN(4), TN(10), BLACK },
  { 8, TN(10), NULL, NULL, RED },
  { 10, TN(6), TN(8), NULL, BLACK },
  { 14, NULL, TN(6), TN(20), BLACK },
  { 15, TN(16), NULL, NULL, RED },
  { 16, TN(20), TN(15), NULL, BLACK },
  { 20, TN(14), TN(16), TN(22), BLACK },
  { 22, TN(20), NULL, NULL, BLACK }
};

static const test_node_description random_ops_tree_multiple_24[] = {
  { 2, TN(6), NULL, TN(5), BLACK },
  { 2, TN(4), NULL, NULL, RED },
  { 3, TN(14), TN(4), TN(10), BLACK },
  { 4, TN(10), NULL, NULL, RED },
  { 5, TN(6), TN(8), NULL, BLACK },
  { 7, NULL, TN(6), TN(20), BLACK },
  { 7, TN(16), NULL, NULL, RED },
  { 8, TN(20), TN(15), NULL, BLACK },
  { 10, TN(14), TN(16), TN(22), BLACK },
  { 11, TN(20), NULL, NULL, BLACK }
};

static const test_node_description random_ops_tree_unique_25[] = {
  { 0, TN(1), NULL, NULL, RED },
  { 1, TN(3), TN(0), NULL, BLACK },
  { 3, TN(13), TN(1), TN(5), BLACK },
  { 4, TN(5), NULL, NULL, BLACK },
  { 5, TN(3), TN(4), TN(6), RED },
  { 6, TN(5), NULL, TN(9), BLACK },
  { 9, TN(6), NULL, NULL, RED },
  { 13, NULL, TN(3), TN(19), BLACK },
  { 14, TN(15), NULL, NULL, RED },
  { 15, TN(16), TN(14), NULL, BLACK },
  { 16, TN(19), TN(15), TN(17), RED },
  { 17, TN(16), NULL, NULL, BLACK },
  { 19, TN(13), TN(16), TN(23), BLACK },
  { 23, TN(19), NULL, TN(24), BLACK },
  { 24, TN(23), NULL, NULL, RED }
};

static const test_node_description random_ops_tree_multiple_25[] = {
  { 0, TN(3), NULL, TN(1), BLACK },
  { 0, TN(0), NULL, NULL, RED },
  { 1, TN(13), TN(0), TN(4), BLACK },
  { 2, TN(4), NULL, NULL, BLACK },
  { 2, TN(3), TN(5), TN(6), RED },
  { 3, TN(4), NULL, TN(9), BLACK },
  { 4, TN(6), NULL, NULL, RED },
  { 6, NULL, TN(3), TN(19), BLACK },
  { 7, TN(17), NULL, TN(14), BLACK },
  { 7, TN(15), NULL, NULL, RED },
  { 8, TN(19), TN(15), TN(16), RED },
  { 8, TN(17), NULL, NULL, BLACK },
  { 9, TN(13), TN(17), TN(23), BLACK },
  { 11, TN(19), NULL, TN(24), BLACK },
  { 12, TN(23), NULL, NULL, RED }
};

static const test_node_description random_ops_tree_unique_26[] = {
  { 0, TN(1), NULL, NULL, RED },
  { 1, TN(3), TN(0), NULL, BLACK },
  { 3, TN(11), TN(1), TN(9), BLACK },
  { 6, TN(9), NULL, NULL, RED },
  { 9, TN(3), TN(6), TN(10), BLACK },
  { 10, TN(9), NULL, NULL, RED },
  { 11, NULL, TN(3), TN(14), BLACK },
  { 12, TN(14), NULL, TN(13), BLACK },
  { 13, TN(12), NULL, NULL, RED },
  { 14, TN(11), TN(12), TN(20), BLACK },
  { 18, TN(20), NULL, NULL, BLACK },
  { 20, TN(14), TN(18), TN(23), RED },
  { 21, TN(23), NULL, NULL, RED },
  { 23, TN(20), TN(21), NULL, BLACK }
};

static const test_node_description random_ops_tree_multiple_26[] = {
  { 0, TN(3), NULL, TN(0), BLACK },
  { 0, TN(1), NULL, NULL, RED },
  { 1, TN(9), TN(1), TN(6), BLACK },
  { 3, TN(3), NULL, NULL, BLACK },
  { 4, NULL, TN(3), TN(14), BLACK },
  { 5, TN(12), NULL, TN(10), BLACK },
  { 5, TN(11), NULL, NULL, RED },
  { 6, TN(14), TN(11), TN(13), RED },
  { 6, TN(12), NULL, NULL, BLACK },
  { 7, TN(9), TN(12), TN(20), BLACK },
  { 9, TN(20), NULL, NULL, BLACK },
  { 10, TN(14), TN(18), TN(23), RED },
  { 10, TN(23), NULL, NULL, RED },
  { 11, TN(20), TN(21), NULL, BLACK }
};

static const test_node_description random_ops_tree_unique_27[] = {
  { 3, TN(8), NULL, NULL, BLACK },
  { 8, TN(19), TN(3), TN(17), BLACK },
  { 12, TN(17), NULL, NULL, RED },
  { 17, TN(8), TN(12), NULL, BLACK },
  { 19, NULL, TN(8), TN(24), BLACK },
  { 20, TN(21), NULL, NULL, RED },
  { 21, TN(24), TN(20), TN(23), BLACK },
  { 23, TN(21), NULL, NULL, RED },
  { 24, TN(19), TN(21), TN(25), BLACK },
  { 25, TN(24), NULL, TN(26), BLACK },
  { 26, TN(25), NULL, NULL, RED }
};

static const test_node_description random_ops_tree_multiple_27[] = {
  { 1, TN(8), NULL, NULL, BLACK },
  { 4, TN(19), TN(3), TN(17), BLACK },
  { 6, TN(17), NULL, NULL, RED },
  { 8, TN(8), TN(12), NULL, BLACK },
  { 9, NULL, TN(8), TN(25), BLACK },
  { 10, TN(21), NULL, NULL, RED },
  { 10, TN(25), TN(20), TN(23), BLACK },
  { 11, TN(21), NULL, NULL, RED },
  { 12, TN(19), TN(21), TN(24), BLACK },
  { 12, TN(25), NULL, TN(26), BLACK },
  { 13, TN(24), NULL, NULL, RED }
};

static const test_node_description random_ops_tree_unique_28[] = {
  { 0, TN(5), NULL, NULL, BLACK },
  { 5, TN(13), TN(0), TN(7), RED },
  { 7, TN(5), NULL, NULL, BLACK },
  { 13, NULL, TN(5), TN(17), BLACK },
  { 15, TN(17), NULL, NULL, BLACK },
  { 17, TN(13), TN(15), TN(26), RED },
  { 21, TN(26), NULL, NULL, RED },
  { 26, TN(17), TN(21), NULL, BLACK }
};

static const test_node_description random_ops_tree_multiple_28[] = {
  { 0, TN(5), NULL, NULL, BLACK },
  { 2, TN(13), TN(0), TN(7), RED },
  { 3, TN(5), NULL, NULL, BLACK },
  { 6, NULL, TN(5), TN(17), BLACK },
  { 7, TN(17), NULL, NULL, BLACK },
  { 8, TN(13), TN(15), TN(26), RED },
  { 10, TN(26), NULL, NULL, RED },
  { 13, TN(17), TN(21), NULL, BLACK }
};

static const test_node_description random_ops_tree_unique_29[] = {
  { 0, TN(3), NULL, TN(1), BLACK },
  { 1, TN(0), NULL, NULL, RED },
  { 3, TN(12), TN(0), TN(6), BLACK },
  { 4, TN(6), NULL, NULL, BLACK },
  { 6, TN(3), TN(4), TN(8), RED },
  { 7, TN(8), NULL, NULL, RED },
  { 8, TN(6), TN(7), TN(11), BLACK },
  { 11, TN(8), NULL, NULL, RED },
  { 12, NULL, TN(3), TN(17), BLACK },
  { 13, TN(17), NULL, TN(14), BLACK },
  { 14, TN(13), NULL, NULL, RED },
  { 17, TN(12), TN(13), TN(25), BLACK },
  { 22, TN(25), NULL, NULL, RED },
  { 25, TN(17), TN(22), TN(27), BLACK },
  { 27, TN(25), NULL, NULL, RED }
};

static const test_node_description random_ops_tree_multiple_29[] = {
  { 0, TN(3), NULL, TN(1), BLACK },
  { 0, TN(0), NULL, NULL, RED },
  { 1, TN(11), TN(0), TN(6), BLACK },
  { 2, TN(6), NULL, NULL, BLACK },
  { 3, TN(3), TN(4), TN(7), RED },
  { 3, TN(6), NULL, TN(8), BLACK },
  { 4, TN(7), NULL, NULL, RED },
  { 5, NULL, TN(3), TN(22), BLACK },
  { 6, TN(12), NULL, NULL, BLACK },
  { 6, TN(22), TN(13), TN(17), RED },
  { 7, TN(17), NULL, NULL, RED },
  { 8, TN(12), TN(14), NULL, BLACK },
  { 11, TN(11), TN(12), TN(25), BLACK },
  { 12, TN(22), NULL, TN(27), BLACK },
  { 13, TN(25), NULL, NULL, RED }
};

static const test_node_description random_ops_tree_unique_30[] = {
  { 0, TN(4), NULL, NULL, RED },
  { 4, TN(6), TN(0), NULL, BLACK },
  { 6, TN(13), TN(4), TN(9), RED },
  { 8, TN(9), NULL, NULL, RED },
  { 9, TN(6), TN(8), TN(12), BLACK },
  { 12, TN(9), NULL, NULL, RED },
  { 13, NULL, TN(6), TN(18), BLACK },
  { 14, TN(16), NULL, NULL, RED },
  { 16, TN(18), TN(14), TN(17), BLACK },
  { 17, TN(16), NULL, NULL, RED },
  { 18, TN(13), TN(16), TN(27), RED },
  { 20, TN(27), NULL, NULL, RED },
  { 27, TN(18), TN(20), TN(28), BLACK },
  { 28, TN(27), NULL, NULL, RED }
};

static const test_node_description random_ops_tree_multiple_30[] = {
  { 0, TN(4), NULL, NULL, BLACK },
  { 2, TN(13), TN(0), TN(9), RED },
  { 3, TN(9), NULL, NULL, RED },
  { 4, TN(4), TN(6), TN(8), BLACK },
  { 4, TN(9), NULL, NULL, RED },
  { 6, TN(14), TN(4), TN(12), BLACK },
  { 6, TN(13), NULL, NULL, BLACK },
  { 7, NULL, TN(13), TN(18), BLACK },
  { 8, TN(18), NULL, TN(16), BLACK },
  { 8, TN(17), NULL, NULL, RED },
  { 9, TN(14), TN(17), TN(27), BLACK },
  { 10, TN(27), NULL, NULL, RED },
  { 13, TN(18), TN(20), TN(28), BLACK },
  { 14, TN(27), NULL, NULL, RED }
};

static const test_node_description random_ops_tree_unique_31[] = {
  { 0, TN(2), NULL, NULL, RED },
  { 2, TN(5), TN(0), NULL, BLACK },
  { 5, TN(11), TN(2), TN(9), BLACK },
  { 7, TN(9), NULL, NULL, RED },
  { 9, TN(5), TN(7), NULL, BLACK },
  { 11, NULL, TN(5), TN(21), BLACK },
  { 14, TN(16), NULL, NULL, RED },
  { 16, TN(21), TN(14), TN(18), BLACK },
  { 18, TN(16), NULL, NULL, RED },
  { 21, TN(11), TN(16), TN(30), BLACK },
  { 30, TN(21), NULL, NULL, BLACK }
};

static const test_node_description random_ops_tree_multiple_31[] = {
  { 0, TN(2), NULL, NULL, RED },
  { 1, TN(5), TN(0), NULL, BLACK },
  { 2, TN(11), TN(2), TN(9), BLACK },
  { 3, TN(9), NULL, NULL, RED },
  { 4, TN(5), TN(7), NULL, BLACK },
  { 5, NULL, TN(5), TN(21), BLACK },
  { 7, TN(16), NULL, NULL, RED },
  { 8, TN(21), TN(14), TN(18), BLACK },
  { 9, TN(16), NULL, NULL, RED },
  { 10, TN(11), TN(16), TN(30), BLACK },
  { 15, TN(21), NULL, NULL, BLACK }
};

#define RANDOM_OPS_TREE( i ) \
  { &random_ops_tree_multiple_ ## i[ 0 ], &random_ops_tree_unique_ ## i[ 0 ] }

static const test_node_description *const random_ops_trees[][2] = {
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
    RTEMS_ARRAY_SIZE( random_ops_tree_multiple_ ## i ), \
    RTEMS_ARRAY_SIZE( random_ops_tree_unique_ ## i ) \
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

static uint32_t simple_random( uint32_t v )
{
  v *= 1664525;
  v += 1013904223;

  return v;
}

static void random_ops( size_t n, bool unique )
{
  visitor_context ctx = {
    0,
    random_ops_tree_counts[ n - 1 ][ unique ],
    random_ops_trees[ n - 1 ][ unique ]
  };
  rtems_rbtree_control tree;
  test_node *nodes = &node_array[ 0 ];
  size_t m = n * n * n;
  size_t s = unique ? 1 : 2;
  uint32_t v = 0xdeadbeef;
  size_t i;

  rtems_rbtree_initialize_empty( &tree );

  memset( nodes, 0, n * sizeof( *nodes ) );

  for ( i = 0; i < n; ++i ) {
    nodes[ i ].key = (int) ( i / s );
  }

  for ( i = 0; i < m; ++i ) {
    size_t j = ( v >> 13 ) % n;
    test_node *tn = &nodes[ j ];

    if ( tn->id == 0 ) {
      tn->id = 1;
      rtems_rbtree_insert( &tree, &tn->Node, test_compare_function, unique );
    } else {
      tn->id = 0;
      rtems_rbtree_extract( &tree, &tn->Node );
    }

    rtems_test_assert( rb_assert( rtems_rbtree_root( &tree ) ) != -1 );

    v = simple_random( v );
  }

  _RBTree_Iterate( &tree, visit_nodes, &ctx );
  rtems_test_assert( ctx.current == ctx.count );
}

static void test_rbtree_random_ops( void )
{
  size_t n;

  puts( "INIT - Random operations" );

  for ( n = 0; n < RTEMS_ARRAY_SIZE( random_ops_trees ); ++n ) {
    random_ops( n + 1, true );
    random_ops( n + 1, false );
  }
}

typedef struct {
  rtems_rbtree_node *parent;
  rtems_rbtree_node *left;
  rtems_rbtree_node *right;
} postorder_node_description;

static const postorder_node_description postorder_tree_1[] = {
  { NULL, NULL, NULL }
};

/*
 *   TN_1
 *   /
 * TN_0
 */
static const postorder_node_description postorder_tree_2[] = {
  { TN( 1 ), NULL, NULL },
  { NULL, TN( 0 ), NULL }
};

/*
 * TN_1
 *     \
 *    TN_0
 */
static const postorder_node_description postorder_tree_3[] = {
  { TN( 1 ), NULL, NULL },
  { NULL, NULL, TN( 0 ) }
};

/*
 *    TN_2
 *   /    \
 * TN_0  TN_1
 */
static const postorder_node_description postorder_tree_4[] = {
  { TN( 2 ), NULL, NULL },
  { TN( 2 ), NULL, NULL },
  { NULL, TN( 0 ), TN( 1 ) }
};

/*
 *       TN_3
 *      /
 *    TN_2
 *   /    \
 * TN_0  TN_1
 */
static const postorder_node_description postorder_tree_5[] = {
  { TN( 2 ), NULL, NULL },
  { TN( 2 ), NULL, NULL },
  { TN( 3 ), TN( 0 ), TN( 1 ) },
  { NULL, TN( 2 ), NULL }
};

/*
 *      TN_10
 *     /      \
 *    TN_6     TN_9
 *   /    \        \
 * TN_0  TN_5       TN_8
 *      /    \      /
 *    TN_2   TN_4  TN_7
 *   /      /
 * TN_1   TN_3
 */
static const postorder_node_description postorder_tree_6[] = {
  { TN( 6 ), NULL, NULL },
  { TN( 2 ), NULL, NULL },
  { TN( 5 ), TN( 1 ), NULL },
  { TN( 4 ), NULL, NULL },
  { TN( 5 ), TN( 3 ), NULL },
  { TN( 6 ), TN( 2 ), TN( 4 ) },
  { TN( 10 ), TN( 0 ), TN( 5 ) },
  { TN( 8 ), NULL, NULL },
  { TN( 9 ), TN( 7 ), NULL },
  { TN( 10 ), NULL, TN( 8 ) },
  { NULL, TN( 6 ), TN( 9 ) }
};

/*
 *      TN_5
 *     /
 *    TN_4
 *   /
 * TN_3
 *     \
 *    TN_2
 *        \
 *       TN_1
 *      /
 *    TN_0
 */
static const postorder_node_description postorder_tree_7[] = {
  { TN( 1 ), NULL, NULL },
  { TN( 2 ), TN( 0 ), NULL },
  { TN( 3 ), NULL, TN( 1 ) },
  { TN( 4 ), NULL, TN( 2 ) },
  { TN( 5 ), TN( 3 ), NULL },
  { NULL, TN( 0 ), NULL }
};

typedef struct {
  const postorder_node_description *tree;
  size_t                            node_count;
} postorder_tree;

#define POSTORDER_TREE( idx ) \
  { postorder_tree_##idx, RTEMS_ARRAY_SIZE( postorder_tree_##idx ) }

static const postorder_tree postorder_trees[] = {
  { NULL, 0 },
  POSTORDER_TREE( 1 ),
  POSTORDER_TREE( 2 ),
  POSTORDER_TREE( 3 ),
  POSTORDER_TREE( 4 ),
  POSTORDER_TREE( 5 ),
  POSTORDER_TREE( 6 ),
  POSTORDER_TREE( 7 )
};

static void postorder_tree_init(
  RBTree_Control       *tree,
  const postorder_tree *pt
)
{
  size_t i;

  memset( node_array, 0, pt->node_count * sizeof( node_array[ 0 ] ) );

  if ( pt->node_count > 0 ) {
    _RBTree_Initialize_node( TN( 0 ) );
    _RBTree_Initialize_one( tree, TN( 0 ) );
  } else {
    _RBTree_Initialize_empty( tree );
  }

  for ( i = 0; i < pt->node_count; ++i ) {
    const postorder_node_description *pnd;

    pnd = &pt->tree[ i ];
    RB_PARENT( TN( i ), Node) = pnd->parent;
    RB_LEFT( TN( i ), Node) = pnd->left;
    RB_RIGHT( TN( i ), Node) = pnd->right;
  }
}

static void postorder_tree_check(
  RBTree_Control       *tree,
  const postorder_tree *pt
)
{
  test_node *node;
  test_node *next;
  size_t i;

  node = _RBTree_Postorder_first( tree, offsetof( test_node, Node ) );

  for ( i = 0; i < pt->node_count; ++i ) {
    rtems_test_assert( node == &node_array[ i ] );
    node = _RBTree_Postorder_next( &node->Node, offsetof( test_node, Node ) );
  }

  rtems_test_assert( node == NULL );

  i = 0;
  next = NULL;

  rbtree_postorder_for_each_entry_safe( node, next, tree, Node ) {
    rtems_test_assert( node == &node_array[ i ] );

    if ( i < pt->node_count - 1 ) {
      rtems_test_assert( next == &node_array[ i + 1 ] );
    } else {
      rtems_test_assert( next == NULL );
    }

    ++i;
  }

  rtems_test_assert( i == pt->node_count );
  rtems_test_assert( node == NULL );
  rtems_test_assert( next == NULL );
}

static void test_rbtree_postorder( void )
{
  RBTree_Control tree;
  size_t         i;

  puts( "INIT - Postorder operations" );

  for ( i = 0; i < RTEMS_ARRAY_SIZE( postorder_trees ); ++i ) {
    const postorder_tree *pt;

    pt = &postorder_trees[ i ];
    postorder_tree_init( &tree, pt );
    postorder_tree_check( &tree, pt );
  }
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

  if (!rb_assert(rtems_rbtree_root(&rbtree1)) )
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

    if (!rb_assert(rtems_rbtree_root(&rbtree1)) )
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

    if (!rb_assert(rtems_rbtree_root(&rbtree1)) )
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

    if (!rb_assert(rtems_rbtree_root(&rbtree1)) )
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

    if (!rb_assert(rtems_rbtree_root(&rbtree1)) )
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

    if (!rb_assert(rtems_rbtree_root(&rbtree1)) )
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

    if (!rb_assert(rtems_rbtree_root(&rbtree1)) )
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

    if (!rb_assert(rtems_rbtree_root(&rbtree1)) )
      puts( "INIT - FAILED TREE CHECK" );
  }

  puts( "INIT - Extracting 20 random nodes" );

  for (i = 0; i < 20; i++) {
    id = numbers[i];
    rtems_rbtree_extract( &rbtree1, &node_array[id].Node );
    if (!rb_assert(rtems_rbtree_root(&rbtree1)) )
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

    if (!rb_assert(rtems_rbtree_root(&rbtree1)) )
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
  if ( !rtems_rbtree_left( &node_array[1].Node ) ||
        rtems_rbtree_right( &node_array[1].Node ) )
     puts( "INIT - LEFT CHILD ONLY NOT FOUND" );
  rtems_rbtree_extract( &rbtree1, &node_array[3].Node );
  while( (p = rtems_rbtree_get_max(&rbtree1)) );

  puts( "INIT - Verify rtems_rbtree_get_max with 100 nodes value [99,0]" );
  for (i = 0; i < 100; i++) {
    node_array[i].id = 99-i;
    node_array[i].key = 99-i;
    rb_insert_unique( &rbtree1, &node_array[i].Node );

    if (!rb_assert(rtems_rbtree_root(&rbtree1)) )
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

    if (!rb_assert(rtems_rbtree_root(&rbtree1)) )
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

    if (!rb_assert(rtems_rbtree_root(&rbtree1)) )
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

  if ( rb_color( rtems_rbtree_root(&rbtree1) ) != BLACK )
    puts ( "INIT - ERROR ON RBTREE ROOT IS BLACK MISMATCH" );

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

    if (!rb_assert(rtems_rbtree_root(&rbtree1)) )
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

    _RBTree_Iterate( &rbtree1, visit_nodes, &ctx );
    rtems_test_assert( ctx.current == ctx.count );

    if (!rb_assert(rtems_rbtree_root(&rbtree1)) )
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

    if (!rb_assert(rtems_rbtree_root(&rbtree1)) )
      puts( "INIT - FAILED TREE CHECK" );

    if ( id < 19 ) {
      visitor_context ctx = { 0, 20 - id - 1, test_remove_trees[ id ] };

      _RBTree_Iterate( &rbtree1, visit_nodes, &ctx );
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

    if (!rb_assert(rtems_rbtree_root(&rbtree1)) )
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

    if (!rb_assert(rtems_rbtree_root(&rbtree1)) )
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

    if (!rb_assert(rtems_rbtree_root(&rbtree1)) )
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

    if (!rb_assert(rtems_rbtree_root(&rbtree1)) )
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

    if (!rb_assert(rtems_rbtree_root(&rbtree1)) )
      puts( "INIT - FAILED TREE CHECK" );
  }

  if(!rtems_rbtree_is_empty(&rbtree1)) {
    puts( "INIT - TREE NOT EMPTY" );
    rtems_test_exit(0);
  }

  test_rbtree_postorder();
  test_rbtree_init_one();
  test_rbtree_min_max();
  test_rbtree_insert_inline();
  test_rbtree_random_ops();

  TEST_END();
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
