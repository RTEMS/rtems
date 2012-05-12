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

static void test_chain_control_initializer(void)
{
  rtems_chain_control chain = RTEMS_CHAIN_INITIALIZER_EMPTY( chain );
  puts( "INIT - Verify rtems_chain_control initializer" );
  rtems_test_assert( rtems_chain_is_empty( &chain ) );
}

static void test_chain_control_layout(void)
{
  rtems_chain_control chain;
  puts( "INIT - Verify rtems_chain_control layout" );
  rtems_test_assert(
    sizeof(rtems_chain_control)
      == sizeof(rtems_chain_node) + sizeof(rtems_chain_node *)
  );
  rtems_test_assert(
    sizeof(rtems_chain_control)
      == 3 * sizeof(rtems_chain_node *)
  );
  rtems_test_assert( &chain.Head.Node.previous == &chain.Tail.Node.next );
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
  rtems_chain_insert( &node1, &node2 );

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

  puts( "INIT - Verify rtems_chain_prepend_with_notification" );
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

  test_chain_first_and_last();
  test_chain_with_empty_check();
  test_chain_with_notification();
  test_chain_get_with_wait();
  test_chain_control_layout();
  test_chain_control_initializer();

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
