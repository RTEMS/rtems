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

#include <pmacros.h>
#include <errno.h>

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);
rtems_timer_service_routine Cancel_duringISR_TSR(
  rtems_id  ignored_id,
  void     *ignored_address
);
rtems_timer_service_routine SetState_duringISR_TSR(
  rtems_id  ignored_id,
  void     *ignored_address
);
rtems_timer_service_routine SetType_duringISR_TSR(
  rtems_id  ignored_id,
  void     *ignored_address
);
void doit(
  rtems_timer_service_routine (*TSR)(rtems_id, void *),
  const char                   *method
);

volatile int TSR_occurred;
volatile int TSR_status;

rtems_id  timer_id;

rtems_timer_service_routine Cancel_duringISR_TSR(
  rtems_id  ignored_id,
  void     *ignored_address
)
{
  TSR_status = pthread_cancel( pthread_self() );
  TSR_occurred = 1;
}

rtems_timer_service_routine SetState_duringISR_TSR(
  rtems_id  ignored_id,
  void     *ignored_address
)
{
  int oldstate;

  TSR_status = pthread_setcancelstate( 0, &oldstate );
  TSR_occurred = 1;
}

rtems_timer_service_routine SetType_duringISR_TSR(
  rtems_id  ignored_id,
  void     *ignored_address
)
{
  int oldtype;

  TSR_status = pthread_setcanceltype( 0, &oldtype );
  TSR_occurred = 1;
}

void doit(
  rtems_timer_service_routine (*TSR)(rtems_id, void *),
  const char                   *method
)
{
  rtems_interval    start;
  rtems_interval    end;
  rtems_status_code status;

  printf( "Init: schedule %s from a TSR\n", method );

  TSR_occurred = 0;
  TSR_status   = 0;

  status = rtems_timer_fire_after( timer_id, 10, TSR, NULL );
  rtems_test_assert( !status );

  start = rtems_clock_get_ticks_since_boot();
  do {
    end = rtems_clock_get_ticks_since_boot();
  } while ( !TSR_occurred && ((end - start) <= 800));

  if ( !TSR_occurred ) {
    printf( "%s did not occur\n", method );
    rtems_test_exit(0);
  }
  if ( TSR_status != EPROTO ) {
    printf( "%s returned %s\n", method, strerror(TSR_status) );
    rtems_test_exit(0);
  }
  printf( "%s - from ISR returns EPROTO - OK\n", method );

}

void *POSIX_Init(
  void *argument
)
{
  rtems_status_code status;

  puts( "\n\n*** POSIX TEST CANCEL 01 ***" );

  status = rtems_timer_create(
    rtems_build_name( 'T', 'M', '1', ' ' ),
    &timer_id
  );
  rtems_test_assert( !status );

  doit( Cancel_duringISR_TSR, "pthread_cancel" );
  doit( SetState_duringISR_TSR, "pthread_setcancelstate" );
  doit( SetType_duringISR_TSR, "pthread_setcanceltype" );

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
