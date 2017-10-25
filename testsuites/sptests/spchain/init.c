/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tmacros.h>
#include <rtems/chain.h>

const char rtems_test_name[] = "SPCHAIN";

static void test_chain_init_one(void)
{
  Chain_Control chain;
  Chain_Node    node;

  puts( "INIT - Verify _Chain_Initialize_one" );

  _Chain_Initialize_node( &node );
  _Chain_Initialize_one( &chain, &node );
  rtems_test_assert( !_Chain_Is_empty( &chain ) );
  rtems_test_assert( !_Chain_Is_node_off_chain( &node ) );
  rtems_test_assert( _Chain_Is_first( &node ) );
  rtems_test_assert( _Chain_Is_last( &node ) );
  rtems_test_assert( _Chain_First( &chain ) == &node );
  rtems_test_assert( _Chain_Last( &chain ) == &node );
  rtems_test_assert( _Chain_Next( &node ) == _Chain_Tail( &chain ) );
  rtems_test_assert( _Chain_Previous( &node ) == _Chain_Head( &chain ) );

  _Chain_Extract_unprotected( &node );
  rtems_test_assert( _Chain_Is_empty( &chain ) );
}

static void update_registry_and_extract(
  Chain_Iterator_registry *reg,
  Chain_Node *n
)
{
  _Chain_Iterator_registry_update( reg, n );
  _Chain_Extract_unprotected( n );
}

static Chain_Iterator_registry static_reg =
  CHAIN_ITERATOR_REGISTRY_INITIALIZER( static_reg );

static void test_chain_iterator( void )
{
  Chain_Control chain;
  Chain_Iterator_registry reg;
  Chain_Iterator fit;
  Chain_Iterator bit;
  Chain_Node a;
  Chain_Node b;
  Chain_Node c;

  puts( "INIT - Verify Chain_Iterator" );

  rtems_test_assert( _Chain_Is_empty( &static_reg.Iterators ));

  _Chain_Initialize_empty( &chain );
  _Chain_Initialize_node( &a );
  _Chain_Initialize_node( &b );
  _Chain_Initialize_node( &c );
  _Chain_Iterator_registry_initialize( &reg );
  _Chain_Iterator_initialize( &chain, &reg, &fit, CHAIN_ITERATOR_FORWARD );
  _Chain_Iterator_initialize( &chain, &reg, &bit, CHAIN_ITERATOR_BACKWARD );

  rtems_test_assert( _Chain_Iterator_next( &fit ) == _Chain_Tail( &chain ));
  rtems_test_assert( _Chain_Iterator_next( &bit ) == _Chain_Head( &chain ));

  _Chain_Iterator_set_position( &fit, _Chain_Head( &chain ) );
  _Chain_Iterator_set_position( &bit, _Chain_Tail( &chain ) );
  rtems_test_assert( _Chain_Iterator_next( &fit ) == _Chain_Tail( &chain ));
  rtems_test_assert( _Chain_Iterator_next( &bit ) == _Chain_Head( &chain ));

  _Chain_Append_unprotected( &chain, &a );
  rtems_test_assert( _Chain_Iterator_next( &fit ) == &a );
  rtems_test_assert( _Chain_Iterator_next( &bit ) == &a );

  _Chain_Append_unprotected( &chain, &b );
  rtems_test_assert( _Chain_Iterator_next( &fit ) == &a );
  rtems_test_assert( _Chain_Iterator_next( &bit ) == &b );

  _Chain_Append_unprotected( &chain, &c );
  rtems_test_assert( _Chain_Iterator_next( &fit ) == &a );
  rtems_test_assert( _Chain_Iterator_next( &bit ) == &c );

  update_registry_and_extract( &reg, &b );
  rtems_test_assert( _Chain_Iterator_next( &fit ) == &a );
  rtems_test_assert( _Chain_Iterator_next( &bit ) == &c );

  _Chain_Insert_unprotected( &a, &b );
  rtems_test_assert( _Chain_Iterator_next( &fit ) == &a );
  rtems_test_assert( _Chain_Iterator_next( &bit ) == &c );

  update_registry_and_extract( &reg, &c );
  rtems_test_assert( _Chain_Iterator_next( &fit ) == &a );
  rtems_test_assert( _Chain_Iterator_next( &bit ) == &b );

  _Chain_Append_unprotected( &chain, &c );
  rtems_test_assert( _Chain_Iterator_next( &fit ) == &a );
  rtems_test_assert( _Chain_Iterator_next( &bit ) == &c );

  update_registry_and_extract( &reg, &a );
  rtems_test_assert( _Chain_Iterator_next( &fit ) == &b );
  rtems_test_assert( _Chain_Iterator_next( &bit ) == &c );

  _Chain_Prepend_unprotected( &chain, &a );
  rtems_test_assert( _Chain_Iterator_next( &fit ) == &a );
  rtems_test_assert( _Chain_Iterator_next( &bit ) == &c );

  update_registry_and_extract( &reg, &a );
  rtems_test_assert( _Chain_Iterator_next( &fit ) == &b );
  rtems_test_assert( _Chain_Iterator_next( &bit ) == &c );

  update_registry_and_extract( &reg, &b );
  rtems_test_assert( _Chain_Iterator_next( &fit ) == &c );
  rtems_test_assert( _Chain_Iterator_next( &bit ) == &c );

  update_registry_and_extract( &reg, &c );
  rtems_test_assert( _Chain_Iterator_next( &fit ) == _Chain_Tail( &chain ));
  rtems_test_assert( _Chain_Iterator_next( &bit ) == _Chain_Head( &chain ));

  _Chain_Append_unprotected( &chain, &a );
  rtems_test_assert( _Chain_Iterator_next( &fit ) == &a );
  rtems_test_assert( _Chain_Iterator_next( &bit ) == &a );

  _Chain_Append_unprotected( &chain, &b );
  rtems_test_assert( _Chain_Iterator_next( &fit ) == &a );
  rtems_test_assert( _Chain_Iterator_next( &bit ) == &b );

  _Chain_Append_unprotected( &chain, &c );
  rtems_test_assert( _Chain_Iterator_next( &fit ) == &a );
  rtems_test_assert( _Chain_Iterator_next( &bit ) == &c );

  update_registry_and_extract( &reg, &c );
  rtems_test_assert( _Chain_Iterator_next( &fit ) == &a );
  rtems_test_assert( _Chain_Iterator_next( &bit ) == &b );

  update_registry_and_extract( &reg, &b );
  rtems_test_assert( _Chain_Iterator_next( &fit ) == &a );
  rtems_test_assert( _Chain_Iterator_next( &bit ) == &a );

  update_registry_and_extract( &reg, &a );
  rtems_test_assert( _Chain_Iterator_next( &fit ) == _Chain_Tail( &chain ));
  rtems_test_assert( _Chain_Iterator_next( &bit ) == _Chain_Head( &chain ));

  rtems_test_assert( !_Chain_Is_empty( &reg.Iterators ));
  _Chain_Iterator_destroy( &fit );
  rtems_test_assert( !_Chain_Is_empty( &reg.Iterators ));
  _Chain_Iterator_destroy( &bit );
  rtems_test_assert( _Chain_Is_empty( &reg.Iterators ));
}

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);

#define EVENT RTEMS_EVENT_13
#define TIMEOUT 1

typedef struct {
  rtems_chain_node Node;
  int              id;
} test_node;

static rtems_chain_control one_node_chain;

static rtems_chain_node node_of_one_node_chain =
  RTEMS_CHAIN_NODE_INITIALIZER_ONE_NODE_CHAIN( &one_node_chain );

static rtems_chain_control one_node_chain =
  RTEMS_CHAIN_INITIALIZER_ONE_NODE( &node_of_one_node_chain );

static void test_chain_control_initializer(void)
{
  rtems_chain_control chain = RTEMS_CHAIN_INITIALIZER_EMPTY( chain );

  puts( "INIT - Verify rtems_chain_control initializer" );

  rtems_test_assert( rtems_chain_is_empty( &chain ) );

  rtems_test_assert( rtems_chain_has_only_one_node( &one_node_chain ) );
  rtems_test_assert(
    rtems_chain_immutable_first( &one_node_chain ) == &node_of_one_node_chain
  );
  rtems_test_assert(
    rtems_chain_immutable_last( &one_node_chain ) == &node_of_one_node_chain
  );
  rtems_test_assert(
    rtems_chain_immutable_head( &one_node_chain )
      == rtems_chain_immutable_previous( &node_of_one_node_chain )
  );
  rtems_test_assert(
    rtems_chain_immutable_tail( &one_node_chain )
      == rtems_chain_immutable_next( &node_of_one_node_chain )
  );
}

static void test_chain_control_layout(void)
{
  Chain_Control chain;

  puts( "INIT - Verify rtems_chain_control layout" );

  rtems_test_assert(
    sizeof(Chain_Control)
      == sizeof(Chain_Node) + sizeof(Chain_Node *)
  );
  rtems_test_assert(
    sizeof(Chain_Control)
      == 3 * sizeof(Chain_Node *)
  );
  rtems_test_assert(
    _Chain_Previous( _Chain_Head( &chain ) )
      == _Chain_Next( _Chain_Tail( &chain ) )
  );

#if !defined( RTEMS_SMP )
  rtems_test_assert(
    sizeof(Chain_Control)
      == sizeof(rtems_chain_control)
  );
#endif
}

static void test_chain_get_with_wait(void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_chain_control chain;
  rtems_chain_node *p = (rtems_chain_node *) 1;

  puts( "INIT - Verify rtems_chain_get_with_wait" );
  rtems_chain_initialize_empty( &chain );
  sc = rtems_chain_get_with_wait( &chain, EVENT, TIMEOUT, &p );
  rtems_test_assert( sc == RTEMS_TIMEOUT );
  rtems_test_assert( p == NULL );
}

static void test_chain_first_and_last(void)
{
  rtems_chain_control   chain;
  rtems_chain_node      node1, node2;
  rtems_chain_node     *cnode;

  rtems_chain_initialize_empty( &chain );
  rtems_chain_initialize_node( &node1 );
  rtems_chain_initialize_node( &node2 );
  rtems_chain_append( &chain, &node1 );
  rtems_chain_insert( &node1, &node2 );

  puts( "INIT - Verify rtems_chain_is_first" );
  cnode = rtems_chain_first(&chain);  
  rtems_test_assert( rtems_chain_is_first( cnode ) );

  puts( "INIT - Verify rtems_chain_is_last" );
  cnode = rtems_chain_last(&chain);
  rtems_test_assert( rtems_chain_is_last( cnode ) );

  cnode = rtems_chain_get_first_unprotected( &chain );
  rtems_test_assert( cnode == &node1 );
  cnode = rtems_chain_first( &chain );
  rtems_test_assert( cnode == &node2 );
  cnode = rtems_chain_last( &chain );
  rtems_test_assert( cnode == &node2 );
}

static void test_chain_with_notification(void)
{
  rtems_status_code sc = RTEMS_SUCCESSFUL;
  rtems_chain_control chain;
  rtems_chain_node a;
  rtems_chain_node b;
  rtems_chain_node *p = (rtems_chain_node *) 1;
  rtems_event_set out = 0;

  puts( "INIT - Verify rtems_chain_append_with_notification" );
  rtems_chain_initialize_empty( &chain );
  rtems_chain_initialize_node( &a );
  sc = rtems_chain_append_with_notification(
    &chain,
    &a,
    rtems_task_self(),
    EVENT
  );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );
  sc = rtems_chain_get_with_wait( &chain, EVENT, TIMEOUT, &p );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );
  rtems_test_assert( p == &a );

  rtems_chain_initialize_empty( &chain );
  rtems_chain_initialize_node( &a );
  rtems_chain_initialize_node( &b );

  rtems_chain_append( &chain, &b );
  sc = rtems_chain_append_with_notification(
    &chain,
    &a,
    rtems_task_self(),
    EVENT
  );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );
  rtems_test_assert( p == &a );

  puts( "INIT - Verify rtems_chain_prepend_with_notification" );
  rtems_chain_initialize_empty( &chain );
  rtems_chain_initialize_node( &a );
  rtems_chain_initialize_node( &b );
  sc = rtems_chain_prepend_with_notification(
    &chain,
    &a,
    rtems_task_self(),
    EVENT
  );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );
  sc = rtems_chain_get_with_wait( &chain, EVENT, TIMEOUT, &p );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );
  rtems_test_assert( p == &a );

  rtems_chain_prepend( &chain, &b );
  sc = rtems_chain_prepend_with_notification(
    &chain,
    &a,
    rtems_task_self(),
    EVENT
  );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );
  rtems_test_assert( p == &a );

  puts( "INIT - Verify rtems_chain_get_with_notification" );
  rtems_chain_initialize_empty( &chain );
  rtems_chain_initialize_node( &a );
  rtems_chain_initialize_node( &b );

  rtems_chain_append( &chain, &b );
  rtems_chain_append( &chain, &a );

  sc = rtems_chain_get_with_notification(&chain, rtems_task_self(), EVENT, &p);
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );
  rtems_test_assert( p == &b );

  sc = rtems_chain_get_with_notification(&chain, rtems_task_self(), EVENT, &p);
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );
  rtems_test_assert( p == &a );
  sc = rtems_event_receive(
    EVENT,
    RTEMS_EVENT_ALL | RTEMS_WAIT,
    TIMEOUT,
    &out
  );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );
  rtems_test_assert( out == EVENT );
}

static void test_chain_with_empty_check(void)
{
  rtems_chain_control chain;
  rtems_chain_node a;
  rtems_chain_node b;
  rtems_chain_node c;
  rtems_chain_node *p;
  bool empty;

  puts( "INIT - Verify rtems_chain_append_with_empty_check" );
  rtems_chain_initialize_empty( &chain );
  rtems_chain_initialize_node( &a );
  rtems_chain_initialize_node( &b );
  empty = rtems_chain_append_with_empty_check( &chain, &a );
  rtems_test_assert( empty );
  empty = rtems_chain_append_with_empty_check( &chain, &b );
  rtems_test_assert( !empty );

  puts( "INIT - Verify rtems_chain_prepend_with_empty_check" );
  rtems_chain_initialize_empty( &chain );
  rtems_chain_initialize_node( &a );
  rtems_chain_initialize_node( &b );
  rtems_chain_initialize_node( &c );
  empty = rtems_chain_prepend_with_empty_check( &chain, &a );
  rtems_test_assert( empty );
  empty = rtems_chain_prepend_with_empty_check( &chain, &b );
  rtems_test_assert( !empty );
  empty = rtems_chain_prepend_with_empty_check( &chain, &c );
  rtems_test_assert( !empty );

  puts( "INIT - Verify rtems_chain_get_with_empty_check" );
  rtems_chain_initialize_empty( &chain );
  rtems_chain_initialize_node( &a );
  rtems_chain_initialize_node( &b );
  empty = rtems_chain_get_with_empty_check( &chain, &p );
  rtems_test_assert( empty );

  rtems_chain_append( &chain, &a );
  rtems_chain_append( &chain, &b );
  empty = rtems_chain_get_with_empty_check( &chain, &p );
  rtems_test_assert( !empty );
  rtems_test_assert( p == &a );
  empty = rtems_chain_get_with_empty_check( &chain, &p );
  rtems_test_assert( empty );
  rtems_test_assert( p == &b );
}

static void test_chain_node_count(void)
{
  rtems_chain_control chain;
  rtems_chain_node nodes[3];
  size_t count;
  size_t i;

  puts( "INIT - Verify rtems_chain_node_count_unprotected" );

  rtems_chain_initialize_empty( &chain );
  count = rtems_chain_node_count_unprotected( &chain );
  rtems_test_assert( count == 0 );

  for (i = 0; i < RTEMS_ARRAY_SIZE( nodes ); ++i) {
    rtems_chain_initialize_node( &nodes[ i ] );
    rtems_chain_append_unprotected( &chain, &nodes[i] );
    count = rtems_chain_node_count_unprotected( &chain );
    rtems_test_assert( count == i + 1 );
  }
}

static bool test_order( const void *left, const Chain_Node *right )
{
  return (uintptr_t) left < (uintptr_t) right;
}

static void insert_ordered( Chain_Control *chain, Chain_Node *node )
{
  _Chain_Insert_ordered_unprotected( chain, node, node, test_order );
}

static void test_chain_insert_ordered( void )
{
  Chain_Control chain = CHAIN_INITIALIZER_EMPTY(chain);
  Chain_Node nodes[5];
  const Chain_Node *tail;
  const Chain_Node *node;
  size_t n = RTEMS_ARRAY_SIZE( nodes );
  size_t i;

  puts( "INIT - Verify _Chain_Insert_ordered_unprotected" );

  for ( i = 0; i < n; ++i ) {
    _Chain_Initialize_node( &nodes[ i ] );
  }

  insert_ordered( &chain, &nodes[4] );
  insert_ordered( &chain, &nodes[2] );
  insert_ordered( &chain, &nodes[0] );
  insert_ordered( &chain, &nodes[3] );
  insert_ordered( &chain, &nodes[1] );

  tail = _Chain_Immutable_tail( &chain );
  node = _Chain_Immutable_first( &chain );
  i = 0;
  while ( node != tail && i < n ) {
    rtems_test_assert( node == &nodes[ i ] );
    ++i;
    node = _Chain_Immutable_next( node );
  }

  rtems_test_assert( i == n );
}

rtems_task Init(
  rtems_task_argument ignored
)
{
  rtems_chain_control  chain1;
  rtems_chain_node    *p;
  test_node            node1, node2;
  int                  id;

  TEST_BEGIN();

  puts( "Init - Initialize chain empty" );
  rtems_chain_initialize_empty( &chain1 );
  rtems_chain_initialize_node( &node1.Node );
  rtems_chain_initialize_node( &node2.Node );

  /* verify that the chain append and insert work */
  puts( "INIT - Verify rtems_chain_insert" );
  node1.id = 1;
  node2.id = 2;
  rtems_chain_append( &chain1, &node1.Node );
  rtems_chain_insert( &node1.Node, &node2.Node );

  for ( p = rtems_chain_first(&chain1), id = 1 ;
        !rtems_chain_is_tail(&chain1, p) ;
        p = p->next , id++ ) {
     test_node *t = (test_node *)p;
     if ( id > 2 ) {
       puts( "INIT - TOO MANY NODES ON CHAIN" );
       rtems_test_exit(0);
     }
     if ( t->id != id ) {
       puts( "INIT - ERROR ON CHAIN ID MISMATCH" );
       rtems_test_exit(0);
     }
  }

  test_chain_init_one();
  test_chain_first_and_last();
  test_chain_with_empty_check();
  test_chain_with_notification();
  test_chain_get_with_wait();
  test_chain_control_layout();
  test_chain_control_initializer();
  test_chain_node_count();
  test_chain_insert_ordered();
  test_chain_iterator();

  TEST_END();
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
