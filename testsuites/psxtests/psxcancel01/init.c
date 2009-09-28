/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <pmacros.h>
#include <errno.h>

volatile int Cancel_occurred;
volatile int Cancel_status;

rtems_timer_service_routine Cancel_duringISR_TSR(
  rtems_id  ignored_id,
  void     *ignored_address
)
{
  Cancel_status = pthread_cancel( pthread_self() );
  Cancel_occurred = 1;
}


void *POSIX_Init(
  void *argument
)
{
  int             status;
  rtems_interval  start;
  rtems_interval  end;
  rtems_id        timer_id;

  puts( "\n\n*** POSIX TEST CANCEL 01 ***" );

  status = rtems_timer_create(
    rtems_build_name( 'T', 'M', '1', ' ' ),
    &timer_id
  );
  assert( !status );

  Cancel_occurred = 0;
  Cancel_status   = 0;

  puts( "Init: schedule pthread_cancel from a TSR" );
  status = rtems_timer_fire_after( timer_id, 10, Cancel_duringISR_TSR, NULL );
  assert( !status );

  /* cancel occurs during sleep */

  do {
    end = rtems_clock_get_ticks_since_boot();
  } while ( !Cancel_occurred && ((end - start) <= 800));

  if ( !Cancel_occurred ) {
    puts( "Cancel did not occur" );
    rtems_test_exit(0);
  }
  if ( Cancel_status != EPROTO ) {
    printf( "Cancel returned %s\n", strerror(Cancel_status) );
    rtems_test_exit(0);
  }
  puts( "pthread_cancel - from ISR returns EPROTO - OK" );


  puts( "*** END OF POSIX TEST CANCEL 01 ***" );
  rtems_test_exit(0);
  return NULL; /* just so the compiler thinks we returned something */
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TIMERS        1

#define CONFIGURE_MAXIMUM_POSIX_THREADS        1
#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
