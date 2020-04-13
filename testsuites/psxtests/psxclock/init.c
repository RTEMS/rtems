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

#include <time.h>
#include <errno.h>
#include <stdint.h>

#include "pmacros.h"
#include "pritime.h"

#include <rtems.h>
#include <rtems/score/todimpl.h>

const char rtems_test_name[] = "PSXCLOCK";

static void check_enosys(int status)
{
  if ( (status == -1) && (errno == ENOSYS) )
    return;
  puts( "ERROR -- did not return ENOSYS as expected" );
  rtems_test_exit(0);
}

typedef struct {
  int counter;
  struct timespec delta;
} nanosleep_contex;

static void task_nanosleep( rtems_task_argument arg )
{
  nanosleep_contex *ctx;

  ctx = (nanosleep_contex *) arg;
  ++ctx->counter;
  nanosleep( &ctx->delta, NULL );
}

static void test_far_future_nanosleep( void )
{
  rtems_status_code sc;
  rtems_id          id;
  nanosleep_contex  ctx;

  sc = rtems_task_create(
    rtems_build_name( 'N', 'A', 'N', 'O' ),
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  ctx.counter = 0;
  ctx.delta.tv_sec = INT64_MAX;
  ctx.delta.tv_nsec = 999999999;
  sc = rtems_task_start( id, task_nanosleep, (rtems_task_argument) &ctx );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  sc = rtems_task_wake_after( RTEMS_YIELD_PROCESSOR );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  rtems_test_assert( ctx.counter == 1 );

  sc = rtems_task_delete( id );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );
}

static rtems_task Init(
  rtems_task_argument argument
)
{
  struct timespec tv;
  struct timespec tr;
  int             sc;
  time_t          seconds;
  time_t          seconds1;
  unsigned int    remaining;
  struct tm       tm;
  struct timespec delay_request;

  TEST_BEGIN();

  tm_build_time( &tm, TM_FRIDAY, TM_MAY, 24, 96, 11, 5, 0 );

  /* error cases in clock_gettime and clock_settime */

  puts( "Init: clock_gettime - EINVAL (NULL timespec)" );
  errno = 0;
  sc = clock_gettime( CLOCK_REALTIME, NULL );
  rtems_test_assert( sc == -1 );
  rtems_test_assert( errno == EINVAL );

  puts( "Init: clock_gettime - EINVAL (invalid clockid)" );
  errno = 0;
  sc = clock_gettime( (clockid_t)-1, &tv );
  rtems_test_assert( sc == -1 );
  rtems_test_assert( errno == EINVAL );

  puts( "Init: clock_settime - EINVAL (invalid clockid)" );
  errno = 0;
  sc = clock_settime( (clockid_t)-1, &tv );
  rtems_test_assert( sc == -1 );
  rtems_test_assert( errno == EINVAL );

  /* way back near the dawn of time :D */
  tv.tv_sec = 1;
  tv.tv_nsec = 0;
  printf( ctime( &tv.tv_sec ) );
  puts( "Init: clock_settime - before 1988 EINVAL" );
  errno = 0;
  sc = clock_settime( CLOCK_REALTIME, &tv );
  rtems_test_assert( sc == -1 );
  rtems_test_assert( errno == EINVAL );

  puts( "Init: clock_settime - invalid nanoseconds EINVAL" );
  tv.tv_sec = 946681200;
  tv.tv_nsec = 2000000000;
  errno = 0;
  sc = clock_settime( CLOCK_REALTIME, &tv );
  rtems_test_assert( sc == -1 );
  rtems_test_assert( errno == EINVAL );

  puts( "Init: clock_settime - far future EINVAL" );
  tv.tv_sec = 17179869184;
  tv.tv_nsec = 0;
  errno = 0;
  sc = clock_settime( CLOCK_REALTIME, &tv );
  rtems_test_assert( sc == -1 );
  rtems_test_assert( errno == EINVAL );

  /* exercise clock_getres */

  puts( "Init: clock_getres - EINVAL (invalid clockid)" );
  errno = 0;
  sc = clock_getres( (clockid_t) -1, &tv );
  rtems_test_assert( sc == -1 );
  rtems_test_assert( errno == EINVAL );

  puts( "Init: clock_getres - EINVAL (NULL resolution)" );
  errno = 0;
  sc = clock_getres( CLOCK_REALTIME, NULL );
  rtems_test_assert( sc == -1 );
  rtems_test_assert( errno == EINVAL );

  puts( "Init: clock_getres - SUCCESSFUL" );
  sc = clock_getres( CLOCK_REALTIME, &tv );
  printf( "Init: resolution = sec (%" PRIdtime_t "), nsec (%ld)\n", tv.tv_sec, tv.tv_nsec );
  rtems_test_assert( !sc );

  /* set the time of day, and print our buffer in multiple ways */

  tv.tv_sec = mktime( &tm );
  rtems_test_assert( tv.tv_sec != -1 );

  tv.tv_nsec = 0;

  /* now set the time of day */

  empty_line();

  printf( asctime( &tm ) );
  puts( "Init: clock_settime - SUCCESSFUL" );
  sc = clock_settime( CLOCK_REALTIME, &tv );
  rtems_test_assert( !sc );

  printf( asctime( &tm ) );
  printf( ctime( &tv.tv_sec ) );

  /* use sleep to delay */

  remaining = sleep( 3 );
  rtems_test_assert( !remaining );

  /* print new times to make sure it has changed and we can get the realtime */
  sc = clock_gettime( CLOCK_PROCESS_CPUTIME_ID, &tv );
  rtems_test_assert( !sc );
  printf("Time since boot: (%" PRIdtime_t ", %ld)\n", tv.tv_sec,tv.tv_nsec );

  sc = clock_gettime( CLOCK_REALTIME, &tv );
  rtems_test_assert( !sc );

  printf( ctime( &tv.tv_sec ) );

  seconds = time( NULL );
  printf( ctime( &seconds ) );

  /*  just to have the value copied out through the parameter */

  seconds = time( &seconds1 );
  rtems_test_assert( seconds == seconds1 );

  /* check the time remaining */

  printf( "Init: seconds remaining (%d)\n", (int)remaining );
  rtems_test_assert( !remaining );

  test_far_future_nanosleep();

  /* error cases in nanosleep */

  empty_line();
  puts( "Init: nanosleep - EINVAL (NULL time)" );
  sc = nanosleep ( NULL, &tr );
  rtems_test_assert( sc == -1 );
  rtems_test_assert( errno == EINVAL );

  tv.tv_sec = 0;
  tv.tv_nsec = TOD_NANOSECONDS_PER_SECOND * 2;
  puts( "Init: nanosleep - EINVAL (too many nanoseconds)" );
  errno = 0;
  sc = nanosleep ( &tv, &tr );
  rtems_test_assert( sc == -1 );
  rtems_test_assert( errno == EINVAL );

  /* this is an error */
  tv.tv_sec = -1;
  tv.tv_nsec = 0;
  puts( "Init: nanosleep - negative seconds - EINVAL" );
  errno = 0;
  sc = nanosleep ( &tv, &tr );
  rtems_test_assert( sc == -1 );
  rtems_test_assert( errno == EINVAL );

  /* this is also an error */
  tv.tv_sec = 0;
  tv.tv_nsec = -1;
  puts( "Init: nanosleep - negative nanoseconds - EINVAL" );
  errno = 0;
  sc = nanosleep ( &tv, &tr );
  rtems_test_assert( sc == -1 );
  rtems_test_assert( errno == EINVAL );

  /* this is actually a small delay */
  tv.tv_sec = 0;
  tv.tv_nsec = 1;
  puts( "Init: nanosleep - delay so small results in one tick" );
  sc = nanosleep ( &tv, &tr );
  rtems_test_assert( !sc );
  rtems_test_assert( !tr.tv_sec );
  rtems_test_assert( !tr.tv_nsec );

  /* use nanosleep to yield */

  tv.tv_sec = 0;
  tv.tv_nsec = 0;

  puts( "Init: nanosleep - yield with remaining" );
  sc = nanosleep ( &tv, &tr );
  rtems_test_assert( !sc );
  rtems_test_assert( !tr.tv_sec );
  rtems_test_assert( !tr.tv_nsec );

  puts( "Init: nanosleep - yield with NULL time remaining" );
  sc = nanosleep ( &tv, NULL );
  rtems_test_assert( !sc );
  rtems_test_assert( !tr.tv_sec );
  rtems_test_assert( !tr.tv_nsec );

  /* use nanosleep to delay */

  tv.tv_sec = 3;
  tv.tv_nsec = 500000;

  puts( "Init: nanosleep - 1.05 seconds" );
  sc = nanosleep ( &tv, &tr );
  rtems_test_assert( !sc );

  /* print the current real time again */
  sc = clock_gettime( CLOCK_REALTIME, &tv );
  rtems_test_assert( !sc );
  printf( ctime( &tv.tv_sec ) );

  /* check the time remaining */

  printf( "Init: sec (%" PRIdtime_t "), nsec (%ld) remaining\n", tr.tv_sec, tr.tv_nsec );
  rtems_test_assert( !tr.tv_sec && !tr.tv_nsec );

  puts( "Init: nanosleep - 1.35 seconds" );
  delay_request.tv_sec = 1;
  delay_request.tv_nsec = 35000000;
  sc = nanosleep( &delay_request, NULL );
  rtems_test_assert( !sc );

  /* print the current real time again */
  sc = clock_gettime( CLOCK_REALTIME, &tv );
  rtems_test_assert( !sc );
  printf( ctime( &tv.tv_sec ) );

  empty_line();
  puts( "clock_gettime - CLOCK_THREAD_CPUTIME_ID -- ENOSYS" );
  #if defined(_POSIX_THREAD_CPUTIME)
    {
      struct timespec tp;
      sc = clock_gettime( CLOCK_THREAD_CPUTIME_ID, &tp );
      check_enosys( sc );
    }
  #endif

  puts( "clock_settime - CLOCK_PROCESS_CPUTIME_ID -- ENOSYS" );
  #if defined(_POSIX_CPUTIME)
    {
      struct timespec tp;
      sc = clock_settime( CLOCK_PROCESS_CPUTIME_ID, &tp );
      check_enosys( sc );
    }
  #endif

  puts( "clock_settime - CLOCK_THREAD_CPUTIME_ID -- ENOSYS" );
  #if defined(_POSIX_THREAD_CPUTIME)
    {
      struct timespec tp;
      sc = clock_settime( CLOCK_THREAD_CPUTIME_ID, &tp );
      check_enosys( sc );
    }
  #endif

  TEST_END();
  rtems_test_exit(0);
}


/* configuration information */
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_MAXIMUM_TASKS             2

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
