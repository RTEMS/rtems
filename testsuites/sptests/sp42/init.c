/*
 *  Exercise thread queue enqueue and dequeue priority
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>

#include <bsp.h>

#include "tmacros.h"

const char rtems_test_name[] = "SP 42";

#define MAX_TASKS 20

/*
 * Carefully chosen to exercise threadq enqueue/dequeue priority logic.
 * Somewhat randomly sorted to ensure than if discipline is FIFO, run-time
 * behavior won't be the same when released.
 */
static const rtems_task_priority Priorities_High[MAX_TASKS] = {
  37, 37, 37, 37,       /* backward - more 2-n */
  2, 2, 2, 2,           /* forward - multiple are on 2-n chain */
  4, 3,                 /* forward - search forward arbitrary */
  3, 3, 3, 3,           /* forward - more 2-n */
  38, 37,               /* backward - search backward arbitrary */
  34, 34, 34, 34,       /* backward - multple on 2-n chain */
};

static const rtems_task_priority Priorities_Low[MAX_TASKS] = {
  13, 13, 13, 13,       /* backward - more 2-n */
  2, 2, 2, 2,           /* forward - multiple are on 2-n chain */
  4, 3,                 /* forward - search forward arbitrary */
  3, 3, 3, 3,           /* forward - more 2-n */
  14, 13,               /* backward - search backward arbitrary */
  12, 12, 12, 12,       /* backward - multple on 2-n chain */
};

static const int Obtain_order[2][MAX_TASKS] = {
  { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19 },
  { 4, 5, 6, 7, 9, 10, 11, 12, 13, 8, 16, 17, 18, 19, 0, 1, 2, 3, 15, 14 }
};

static const rtems_task_priority *Priorities;

static rtems_id   Semaphore;
static rtems_id   Master;
static rtems_id   Task_id[ MAX_TASKS ];
static rtems_name Task_name[ MAX_TASKS ];

static rtems_task_argument Obtain_counter;

static enum {
 FIFO,
 PRIORITY
} Variant;

static rtems_task Locker_task(
  rtems_task_argument task_index
)
{
  rtems_id            tid;
  rtems_status_code   status;
  rtems_task_argument my_obtain_counter;

  status = rtems_task_ident( RTEMS_SELF, RTEMS_SEARCH_ALL_NODES, &tid );
  directive_failed( status, "rtems_task_ident" );

  rtems_test_assert( task_index == task_number( tid ) - 1 );

  status = rtems_semaphore_obtain( Semaphore, RTEMS_DEFAULT_OPTIONS, 0 );
  directive_failed( status, "rtems_semaphore_obtain" );

  put_name( Task_name[ task_index ], FALSE );
  puts( " - unblocked - OK" );

  status = rtems_task_wake_after( 10 );
  directive_failed( status, "rtems_task_wake_after" );

  my_obtain_counter = Obtain_counter;
  rtems_test_assert( task_index == Obtain_order[ Variant ][ Obtain_counter ] );
  ++Obtain_counter;

  status = rtems_semaphore_release( Semaphore );
  directive_failed( status, "rtems_semaphore_release" );

  if ( my_obtain_counter == MAX_TASKS - 1 ) {
    status = rtems_event_transient_send( Master );
    directive_failed( status, "rtems_event_transient_send" );
  }

  rtems_task_exit();
}

static void do_test(
  rtems_attribute attr,
  bool            extract  /* TRUE if extract, not release */
)
{
  rtems_status_code   status;
  rtems_task_argument i;

  Variant = ( ( attr & RTEMS_PRIORITY ) != 0 ? PRIORITY : FIFO );
  Obtain_counter = 0;

  status = rtems_semaphore_create(
    rtems_build_name( 'S', 'E', 'M', '0' ),  /* name = SEM0 */
    0,                                       /* locked */
    RTEMS_BINARY_SEMAPHORE | attr,           /* mutex w/desired discipline */
    0,                                       /* IGNORED */
    &Semaphore
  );
  directive_failed( status, "rtems_semaphore_create" );

  for (i = 0 ; i < MAX_TASKS ; i++ ) {
    Task_name[ i ] = rtems_build_name(
       'T',
       'A',
       '0' + (char)(i/10),
       '0' + (char)(i%10)
    );

    status = rtems_task_create(
      Task_name[ i ],
      Priorities[ i ],
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &Task_id[ i ]
    );
    directive_failed( status, "rtems_task_create" );

    status = rtems_task_start( Task_id[ i ], Locker_task, i );
    directive_failed( status, "rtems_task_start" );
  }

  if ( extract ) {
    for (i = 0 ; i< MAX_TASKS ; i++ ) {
      status = rtems_task_delete( Task_id[ i ]  );
      directive_failed( status, "rtems_task_delete" );
    }
  }

  /* do the initial release */
  status = rtems_semaphore_release( Semaphore );
  directive_failed( status, "rtems_semaphore_release" );

  if ( !extract ) {
    status = rtems_event_transient_receive( RTEMS_WAIT, RTEMS_NO_TIMEOUT );
    directive_failed( status, "rtems_event_transient_receive" );
  }

  /* now delete the semaphore since no one is waiting and it is unlocked */
  status = rtems_semaphore_delete( Semaphore );
  directive_failed( status, "rtems_semaphore_delete" );
}

static rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_task_priority prio;
  rtems_status_code status;

  TEST_BEGIN();

  Master = rtems_task_self();

  if (RTEMS_MAXIMUM_PRIORITY >= 255)
    Priorities = Priorities_High;
  else if (RTEMS_MAXIMUM_PRIORITY >= 15)
    Priorities = Priorities_Low;
  else {
    puts( "Test needs at least 16 configured priority levels" );
    rtems_test_exit( 0 );
  }

  prio = RTEMS_MAXIMUM_PRIORITY - 1;
  status = rtems_task_set_priority(RTEMS_SELF, prio, &prio);
  directive_failed( status, "rtems_task_set_priority" );

  if ( sizeof(Priorities_Low) / sizeof(rtems_task_priority) != MAX_TASKS ) {
    puts( "Priorities_Low table does not have right number of entries" );
    rtems_test_exit( 0 );
  }

  if ( sizeof(Priorities_High) / sizeof(rtems_task_priority) != MAX_TASKS ) {
    puts( "Priorities_High table does not have right number of entries" );
    rtems_test_exit( 0 );
  }

  puts( "Exercising blocking discipline w/extract in FIFO order " );
  do_test( RTEMS_FIFO, TRUE );

  puts( "Exercising blocking discipline w/unblock in FIFO order" );
  do_test( RTEMS_FIFO, FALSE );

  puts( "Exercising blocking discipline w/extract in priority order " );
  do_test( RTEMS_PRIORITY, TRUE );

  puts( "Exercising blocking discipline w/unblock in priority order" );
  do_test( RTEMS_PRIORITY, FALSE );

  TEST_END();
  rtems_test_exit(0);
}

/**************** START OF CONFIGURATION INFORMATION ****************/

#define CONFIGURE_INIT

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS             MAX_TASKS+1
#define CONFIGURE_MAXIMUM_SEMAPHORES        1

#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_DEFAULT_MODES

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#include <rtems/confdefs.h>

/****************  END OF CONFIGURATION INFORMATION  ****************/

