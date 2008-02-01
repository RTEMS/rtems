/* spmonotonic -- sanity check the rate monotonic manager
 *
 * license and distribution terms for this file may be found in the file
 * LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE .
 *
 * $Id$
 */

#define CONFIGURE_INIT
#include <tmacros.h>  /* includes bsp.h, stdio, etc... */

/* prototype */
rtems_task Init (rtems_task_argument ignored);

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_MAXIMUM_TASKS     5
#define CONFIGURE_MAXIMUM_BARRIERS  1

#define CONFIGURE_INIT

rtems_id    Barrier;

int SuccessfulCase;
int DeletedCase;

rtems_task Waiter(
  rtems_task_argument number
)
{
  rtems_status_code status;

  printf( "Waiter %d waiting on barrier\n", number );
  status = rtems_barrier_wait( Barrier, RTEMS_NO_TIMEOUT );

  printf( "Waiter %d back from barrier\n", number );

  if ( SuccessfulCase == TRUE ) {
    directive_failed(status, "rtems_barrier_wait");
  } else if ( DeletedCase == TRUE ) {
    fatal_directive_status(
      status,
      RTEMS_OBJECT_WAS_DELETED,
      "rtems_barrier_wait did not get deleted"
    );
  }

  rtems_task_delete( RTEMS_SELF );
}

#include <rtems/confdefs.h>

rtems_task Init(
  rtems_task_argument ignored
)
{
  rtems_status_code status;
  rtems_name        name = rtems_build_name('B','A','R','1');
  uint32_t          released;
  rtems_id          testId;
  rtems_id          Tasks[CONFIGURE_MAXIMUM_TASKS-1];
  uint32_t          i;

  puts("\n\n*** TEST 33 ***");

  /* Check bad argument cases */
  puts( "Delete barrier with bad id" );
  status = rtems_barrier_delete( 100 );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_barrier_delete did not return RTEMS_INVALID_ID"
  );

  puts( "Release barrier with bad id" );
  status = rtems_barrier_release( 100, &released );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_barrier_release did not return RTEMS_INVALID_ID"
  );

  puts( "Wait on barrier with bad id" );
  status = rtems_barrier_wait( 100, 10 );
  fatal_directive_status(
    status,
    RTEMS_INVALID_ID,
    "rtems_barrier_wait did not return RTEMS_INVALID_ID"
  );

  /* Create barrier with automatic release and 0 maximum waiters */
  puts( "Create barrier with automatic release and 0 max waiters" );
  status = rtems_barrier_create(
    name, RTEMS_BARRIER_AUTOMATIC_RELEASE, 0, &Barrier
  );
  fatal_directive_status(
    status,
    RTEMS_INVALID_NUMBER,
    "rtems_barrier_create did not return RTEMS_INVALID_NUMBER"
  );

  /* Create barrier */
  puts( "Create barrier" );
  status = rtems_barrier_create( name, RTEMS_DEFAULT_ATTRIBUTES, 0, &Barrier );
  directive_failed(status, "rtems_barrier_create");

  /* Check for creating too many */
  puts( "Create too many barriers" );
  status = rtems_barrier_create( name, RTEMS_DEFAULT_ATTRIBUTES, 0, &Barrier );
  fatal_directive_status(
    status,
    RTEMS_TOO_MANY,
    "rtems_barrier_create did not return RTEMS_TOO_MANY"
  );

  puts( "Check barrier ident" );
  status = rtems_barrier_ident( name, &testId );
  directive_failed(status, "rtems_barrier_ident");
  if ( testId != Barrier ) {
    printf( "ERROR -- rtems_barrier_create -- did not get Id expected\n" );
    exit( 0 );
  }

  puts( "Wait on barrier w/timeout and TIMEOUT" );
  status = rtems_barrier_wait( Barrier, 25 );
  fatal_directive_status(
    status,
    RTEMS_TIMEOUT,
    "rtems_barrier_wait did not timeout"
  );

  /* Release no tasks */
  status = rtems_barrier_release( Barrier, &released );
  directive_failed(status, "rtems_barrier_release");
  if ( released != 0 ) {
    printf( "ERROR -- rtems_barrier_release -- released != 0, = %d", released);
    exit(0);
  }

  /*  Create some tasks to wait for the barrier */
  SuccessfulCase = TRUE;
  DeletedCase    = FALSE;
  puts( "\n*** Testing manual release of barrier ***" );
  for (i=0 ; i<(CONFIGURE_MAXIMUM_TASKS-1) ; i++) {
    status = rtems_task_create(
      rtems_build_name('W','A','I','T'),
      1,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &Tasks[ i ]
    );
    directive_failed( status, "rtems_task_create of Waiter" );

    status = rtems_task_start( Tasks[ i ], Waiter, i );
    directive_failed( status, "rtems_task_start of Waiter" );
  }

  puts( "Delay to let Waiters block" ); 
  status = rtems_task_wake_after( TICKS_PER_SECOND );
  directive_failed(status, "rtems_task_wake_after");

  /* Release tasks which were waiting */
  puts( "Releasing tasks" );
  status = rtems_barrier_release( Barrier, &released );
  directive_failed(status, "rtems_barrier_release");
  if ( released != (CONFIGURE_MAXIMUM_TASKS-1) ) {
    printf( "ERROR -- rtems_barrier_release -- released != %d, = %d",
         (CONFIGURE_MAXIMUM_TASKS-1), released);
    exit(0);
  }

  puts( "Delay to let Waiters print a message" ); 
  status = rtems_task_wake_after( TICKS_PER_SECOND );
  directive_failed(status, "rtems_task_wake_after");

  /*  Create some tasks to wait for the barrier */
  SuccessfulCase = FALSE;
  DeletedCase    = TRUE;
  puts( "\n*** Testing Deletion of barrier ***" );
  for (i=0 ; i<(CONFIGURE_MAXIMUM_TASKS-1) ; i++) {
    status = rtems_task_create(
      rtems_build_name('W','A','I','T'),
      1,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &Tasks[ i ]
    );
    directive_failed( status, "rtems_task_create of Waiter" );

    status = rtems_task_start( Tasks[ i ], Waiter, i );
    directive_failed( status, "rtems_task_start of Waiter" );
  }

  puts( "Delay to let Waiters block" ); 
  status = rtems_task_wake_after( TICKS_PER_SECOND );
  directive_failed(status, "rtems_task_wake_after");

  puts( "Delete barrier" );
  status = rtems_barrier_delete( Barrier );
  directive_failed(status, "rtems_barrier_delete");

  puts( "Delay to let Waiters print a message" ); 
  status = rtems_task_wake_after( TICKS_PER_SECOND );
  directive_failed(status, "rtems_task_wake_after");

  /* Create barrier with automatic release */
  puts( "Create barrier" );
  status = rtems_barrier_create(
    name, RTEMS_BARRIER_AUTOMATIC_RELEASE, CONFIGURE_MAXIMUM_TASKS-1, &Barrier
  );
  directive_failed(status, "rtems_barrier_create");

  /*  Create some tasks to wait for the barrier */
  SuccessfulCase = TRUE;
  DeletedCase    = FALSE;
  puts( "\n*** Testing automatic release of barrier ***" );
  for (i=0 ; i<(CONFIGURE_MAXIMUM_TASKS-1) ; i++) {
    status = rtems_task_create(
      rtems_build_name('W','A','I','T'),
      1,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &Tasks[ i ]
    );
    directive_failed( status, "rtems_task_create of Waiter" );

    status = rtems_task_start( Tasks[ i ], Waiter, i );
    directive_failed( status, "rtems_task_start of Waiter" );
  }

  puts( "Delay to let task wait on barrier" ); 
  status = rtems_task_wake_after( TICKS_PER_SECOND );
  directive_failed(status, "rtems_task_wake_after");

  /* the end */
  puts("*** END OF TEST 33 ***");
  rtems_test_exit(0);
}
