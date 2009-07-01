/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <tmacros.h>
#include <rtems/chain.h>

typedef struct {
  rtems_chain_node Node;
  int              id;
} test_node;

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

  for ( p = chain1.first, id = 1 ;
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

  puts( "*** TEST OF RTEMS CHAIN API ***" );
  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

/*
 *  In this application, the initialization task performs the system
 *  initialization and then transforms itself into the idle task.
 */
#define CONFIGURE_IDLE_TASK_BODY Init
#define CONFIGURE_IDLE_TASK_INITIALIZES_APPLICATION

/*
 *  Another odd case to hit.  Since we use the Init task is Idle task
 *  configuration, we can dummy up the initialization task configuration
 *  to have a non-NULL pointer and 0 tasks.
 */

#define CONFIGURE_HAS_OWN_INIT_TASK_TABLE 1

rtems_initialization_tasks_table Initialization_tasks[1] =
{ { 0, }};

#define CONFIGURE_INIT_TASK_TABLE      Initialization_tasks
#define CONFIGURE_INIT_TASK_TABLE_SIZE 0
#define CONFIGURE_INIT_TASK_STACK_SIZE 0

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* global variables */
