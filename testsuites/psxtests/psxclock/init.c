/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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

static void wait_ticks( rtems_interval ticks )
{
  /*
   * Avoid any clock related sleep calls
   */
  rtems_test_assert( rtems_task_wake_after( ticks ) == RTEMS_SUCCESSFUL );
}

struct clock_context;
typedef struct clock_context clock_context;
typedef void (*clock_sleeper)(clock_context* ctx);
struct clock_context {
  const char* name;
  int instance;
  rtems_id tid;
  int counter;
  int result;
  rtems_interval ticks;
  struct timespec tspec;
  clock_sleeper sleeper;
};

static void clock_context_init(
  clock_context* ctx, const char* name, int instance,
  time_t secs, long nsecs, clock_sleeper sleeper)
{
  memset( ctx, 0, sizeof( *ctx ) );
  ctx->name = name;
  ctx->instance = instance;
  ctx->tspec.tv_sec = secs;
  ctx->tspec.tv_nsec = nsecs;
  ctx->sleeper = sleeper;
}

static void test_nanosleep( clock_context* ctx )
{
  if ( nanosleep ( &ctx->tspec, NULL ) < 0 )
  {
    ctx->result = errno;
  }
}

static void test_clock_nanosleep_realtime( clock_context* ctx )
{
  if ( clock_nanosleep ( CLOCK_REALTIME, 0, &ctx->tspec, NULL ) < 0 )
  {
    ctx->result = errno;
  }
}

static void test_clock_nanosleep_monotonic( clock_context* ctx )
{
  if ( clock_nanosleep ( CLOCK_MONOTONIC, 0, &ctx->tspec, NULL ) < 0 )
  {
    ctx->result = errno;
  }
}

static void test_clock_check( clock_context* ctx, const rtems_interval ticks_per_sec )
{
  const long tick_period_nsec = 1000000000LLU / ticks_per_sec;
  rtems_interval ticks =
    (((ctx->tspec.tv_sec * 1000000000LLU) + ctx->tspec.tv_nsec) / tick_period_nsec) + 1;
  rtems_test_assert( ctx->result == 0 );
  printf(
    "clock: %s: sec=%" PRIdtime_t" nsec=%li ticks=%u expected-ticks=%u\n",
    ctx->name, ctx->tspec.tv_sec, ctx->tspec.tv_nsec, ctx->ticks, ticks);
  rtems_test_assert( ctx->ticks == ticks );
}

static void task_clock( rtems_task_argument arg )
{
  clock_context* ctx = (clock_context*) arg;
  rtems_interval start = rtems_clock_get_ticks_since_boot();
  rtems_interval end;
  ctx->result = 0;
  ctx->sleeper( ctx );
  end = rtems_clock_get_ticks_since_boot();
  ctx->ticks = end - start;
  ++ctx->counter;
  rtems_task_delete( RTEMS_SELF );
}

static void test_start_task( clock_context* ctx )
{
  rtems_status_code sc;
  sc = rtems_task_create(
    rtems_build_name( 'C', 'R', 'T', '0' + ctx->instance ),
    1,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx->tid
  );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );
  sc = rtems_task_start( ctx->tid, task_clock, (rtems_task_argument) ctx );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );
}

static void test_settime_and_sleeping_step( int step )
{
  const rtems_interval ticks_per_sec = rtems_clock_get_ticks_per_second();
  struct timespec tv;

  clock_context ns_ctx;
  clock_context cnr_ctx;
  clock_context cnm_ctx;

  printf( "\nClock settime while sleeping: step=%i\n", step );

  clock_context_init(
    &ns_ctx, "nanosleep", 0, 0, 500000000,
    test_nanosleep );
  clock_context_init(
    &cnr_ctx, "clock_nanosleep(CLOCK_REALTIME)", 0, 0, 500000000,
    test_clock_nanosleep_realtime );
  clock_context_init(
    &cnm_ctx, "clock_nanosleep(CLOCK_MONOTONIC)", 0, 0, 500000000,
    test_clock_nanosleep_monotonic );

  /* Sat, 01 Jan 2000 00:00:00 GMT */
  tv.tv_sec = 946684800;
  tv.tv_nsec = 0;
  rtems_test_assert( clock_settime( CLOCK_REALTIME, &tv ) == 0 );

  wait_ticks( 1 );

  test_start_task( &ns_ctx );
  test_start_task( &cnr_ctx );
  test_start_task( &cnm_ctx );

  wait_ticks( 2 );

  /*
   * Jump forwards 1 second
   */
  if ( step != 0 )
  {
    tv.tv_sec = 946684800 + step;
    tv.tv_nsec = 0;
    rtems_test_assert( clock_settime( CLOCK_REALTIME, &tv ) == 0 );
  }

  while (true)
  {
    int counts = 0;
    wait_ticks( ticks_per_sec /  4 );
    counts += ns_ctx.counter;
    counts += cnr_ctx.counter;
    counts += cnm_ctx.counter;
    if (counts == 3)
    {
      break;
    }
  }

  test_clock_check( &ns_ctx, ticks_per_sec );
  test_clock_check( &cnr_ctx, ticks_per_sec );
  test_clock_check( &cnm_ctx, ticks_per_sec );
}

static void test_settime_and_sleeping( void )
{
  test_settime_and_sleeping_step( 0 );
  test_settime_and_sleeping_step( 1 );
  test_settime_and_sleeping_step( -1 );
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

  puts( "Init: clock_settime - 2400-01-01T00:00:00.999999999Z SUCCESSFUL" );
  tv.tv_sec = 13569465600;
  tv.tv_nsec = 999999999;
  sc = clock_settime( CLOCK_REALTIME, &tv );
  rtems_test_assert( sc == 0 );

  puts( "Init: clock_settime - 2400-01-01T00:00:01Z EINVAL" );
  tv.tv_sec = 13569465601;
  tv.tv_nsec = 0;
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

  test_settime_and_sleeping( );

  TEST_END();
  rtems_test_exit(0);
}


/* configuration information */
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_MAXIMUM_TASKS             4

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
