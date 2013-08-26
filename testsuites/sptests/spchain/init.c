/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tmacros.h>
#include <rtems/chain.h>

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
  rtems_chain_append( &chain, &node1 );
  rtems_chain_explicit_insert( &chain, &node1, &node2 );

  puts( "INIT - Verify rtems_chain_is_first" );
  cnode = rtems_chain_first(&chain);  
  rtems_test_assert( rtems_chain_is_first( cnode ) );

  puts( "INIT - Verify rtems_chain_is_last" );
  cnode = rtems_chain_last(&chain);
  rtems_test_assert( rtems_chain_is_last( cnode ) );
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
  rtems_chain_node *p;
  bool empty;

  puts( "INIT - Verify rtems_chain_append_with_empty_check" );
  rtems_chain_initialize_empty( &chain );
  empty = rtems_chain_append_with_empty_check( &chain, &a );
  rtems_test_assert( empty );
  empty = rtems_chain_append_with_empty_check( &chain, &a );
  rtems_test_assert( !empty );

  puts( "INIT - Verify rtems_chain_prepend_with_empty_check" );
  rtems_chain_initialize_empty( &chain );
  empty = rtems_chain_prepend_with_empty_check( &chain, &a );
  rtems_test_assert( empty );
  empty = rtems_chain_prepend_with_empty_check( &chain, &a );
  rtems_test_assert( !empty );
  empty = rtems_chain_prepend_with_empty_check( &chain, &b );
  rtems_test_assert( !empty );

  puts( "INIT - Verify rtems_chain_get_with_empty_check" );
  rtems_chain_initialize_empty( &chain );
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
    rtems_chain_append_unprotected( &chain, &nodes[i] );
    count = rtems_chain_node_count_unprotected( &chain );
    rtems_test_assert( count == i + 1 );
  }
}

static bool test_order( const Chain_Node *left, const Chain_Node *right )
{
  return left < right;
}

static void test_chain_insert_ordered( void )
{
  Chain_Control chain = CHAIN_INITIALIZER_EMPTY(chain);
  Chain_Node nodes[5];
  const Chain_Node *tail;
  const Chain_Node *node;
  size_t n = RTEMS_ARRAY_SIZE( nodes );
  size_t i = 0;

  puts( "INIT - Verify _Chain_Insert_ordered_unprotected" );

  _Chain_Insert_ordered_unprotected( &chain, &nodes[4], test_order );
  _Chain_Insert_ordered_unprotected( &chain, &nodes[2], test_order );
  _Chain_Insert_ordered_unprotected( &chain, &nodes[0], test_order );
  _Chain_Insert_ordered_unprotected( &chain, &nodes[3], test_order );
  _Chain_Insert_ordered_unprotected( &chain, &nodes[1], test_order );

  tail = _Chain_Immutable_tail( &chain );
  node = _Chain_Immutable_first( &chain );
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

  puts( "\n\n*** TEST OF RTEMS CHAIN API ***" );

  puts( "Init - Initialize chain empty" );
  rtems_chain_initialize_empty( &chain1 );

  /* verify that the chain append and insert work */
  puts( "INIT - Verify rtems_chain_insert" );
  node1.id = 1;
  node2.id = 2;
  rtems_chain_append( &chain1, &node1.Node );
  rtems_chain_explicit_insert( &chain1, &node1.Node, &node2.Node );

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

  test_chain_first_and_last();
  test_chain_with_empty_check();
  test_chain_with_notification();
  test_chain_get_with_wait();
  test_chain_control_layout();
  test_chain_control_initializer();
  test_chain_node_count();
  test_chain_insert_ordered();

  puts( "*** END OF RTEMS CHAIN API TEST ***" );
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
