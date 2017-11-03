/*
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

#include <sys/time.h>
#include <errno.h>
#include <inttypes.h>
#include <sched.h>
#include <stdint.h>
#include <unistd.h>

#include <pmacros.h>

const char rtems_test_name[] = "PSX 12";

#define SS_REPL_PERIOD_NS 200000000

#define SS_INIT_BUDGET_NS 100000000

#define SS_REPL_PERIOD_MS ( SS_REPL_PERIOD_NS / 1000000 )

#define SS_PRIO_LOW 1

#define SS_PRIO_HIGH 2

#define SS_SAMPLE_PERIODS 3

typedef struct {
  uint64_t start;
  struct {
    uint32_t high;
    uint32_t low;
  } samples[ SS_SAMPLE_PERIODS ];
} test_context;

static test_context test_instance;

static int get_current_prio( pthread_t thread )
{
  rtems_status_code sc;
  rtems_task_priority prio;
  int max;

  sc = rtems_task_set_priority( thread, RTEMS_CURRENT_PRIORITY, &prio );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );

  max = sched_get_priority_max( SCHED_FIFO );

  return max + 1 - (int) prio;
}

static void wait_for_prio( int prio )
{
  while ( prio != get_current_prio( pthread_self() ) ) {
    /* Wait */
  }
}

static uint64_t timeval_to_us( const struct timeval *tv )
{
  uint64_t t;

  t = tv->tv_sec;
  t *= 1000000;
  t += tv->tv_usec;

  return t;
}

static uint64_t now( void )
{
  struct timeval now;

  gettimeofday( &now, NULL );

  return timeval_to_us( &now );
}

static uint32_t delta_in_ms( test_context *ctx )
{
  uint32_t d;

  d = (uint32_t) ( now() - ctx->start );
  return ( d + 499 ) / 1000;
}

static void *sporadic_server( void *argument )
{
  test_context *ctx;
  size_t        i;

  ctx = argument;

  for ( i = 0 ; i < SS_SAMPLE_PERIODS ; ++i ) {
    wait_for_prio( SS_PRIO_LOW );
    ctx->samples[ i ].high = delta_in_ms( ctx );
    wait_for_prio( SS_PRIO_HIGH );
    ctx->samples[ i ].low = delta_in_ms( ctx );
  }

  puts( "Sporadic Server: exitting" );

  return NULL;
}

static void *POSIX_Init( void *argument )
{
  test_context       *ctx;
  int                 status;
  pthread_attr_t      attr;
  pthread_t           thread;
  struct sched_param  schedparam;
  size_t              i;

  TEST_BEGIN();

  ctx = &test_instance;

  /* set the time of day, and print our buffer in multiple ways */

  set_time( TM_FRIDAY, TM_MAY, 24, 96, 11, 5, 0 );

  /* get id of this thread */

  printf( "Init's ID is 0x%08" PRIxpthread_t "\n", pthread_self() );

  /* invalid scheduling policy error */

  puts( "Init: pthread_attr_init - SUCCESSFUL" );
  status = pthread_attr_init( &attr );
  rtems_test_assert( !status );

  status = pthread_attr_setinheritsched( &attr, PTHREAD_EXPLICIT_SCHED );
  rtems_test_assert( !status );
  attr.schedpolicy = -1;

  puts( "Init: pthread_create - EINVAL (invalid scheduling policy)" );
  status = pthread_create( &thread, &attr, sporadic_server, NULL );
  rtems_test_assert( status == EINVAL );

  /* replenish period < budget error */

  puts( "Init: pthread_attr_init - SUCCESSFUL" );
  status = pthread_attr_init( &attr );
  rtems_test_assert( !status );

  puts( "Init: set scheduling parameter attributes for sporadic server" );
  status = pthread_attr_setschedpolicy( &attr, SCHED_SPORADIC );
  rtems_test_assert( !status );

  schedparam.sched_ss_repl_period.tv_sec = 1;
  schedparam.sched_ss_repl_period.tv_nsec = 0;
  schedparam.sched_ss_init_budget.tv_sec = 2;
  schedparam.sched_ss_init_budget.tv_nsec = 0;

  schedparam.sched_priority = 200;
  schedparam.sched_ss_low_priority = 100;

  status = pthread_attr_setschedparam( &attr, &schedparam );
  rtems_test_assert( !status );

  status = pthread_attr_setinheritsched( &attr, PTHREAD_EXPLICIT_SCHED );
  rtems_test_assert( !status );

  puts( "Init: pthread_create - EINVAL (replenish < budget)" );
  status = pthread_create( &thread, &attr, sporadic_server, NULL );
  rtems_test_assert( status == EINVAL );

  /* invalid sched_ss_low_priority error */

  schedparam.sched_ss_repl_period.tv_sec = 0;
  schedparam.sched_ss_repl_period.tv_nsec = SS_REPL_PERIOD_NS;
  schedparam.sched_ss_init_budget.tv_sec = 0;
  schedparam.sched_ss_init_budget.tv_nsec = SS_INIT_BUDGET_NS;

  schedparam.sched_priority = SS_PRIO_HIGH;
  schedparam.sched_ss_low_priority = -1;

  status = pthread_attr_setschedparam( &attr, &schedparam );
  rtems_test_assert( !status );

  puts( "Init: pthread_create - EINVAL (invalid sched_ss_low_priority)" );
  status = pthread_create( &thread, &attr, sporadic_server, NULL );
  rtems_test_assert( status == EINVAL );

  /* create a thread as a sporadic server */

  schedparam.sched_ss_repl_period.tv_sec = 0;
  schedparam.sched_ss_repl_period.tv_nsec = SS_REPL_PERIOD_NS;
  schedparam.sched_ss_init_budget.tv_sec = 0;
  schedparam.sched_ss_init_budget.tv_nsec = SS_INIT_BUDGET_NS;

  schedparam.sched_priority = SS_PRIO_HIGH;
  schedparam.sched_ss_low_priority = SS_PRIO_LOW;

  status = pthread_attr_setschedparam( &attr, &schedparam );
  rtems_test_assert( !status );

  puts( "Init: pthread_create - SUCCESSFUL" );

  /* Align with clock tick */
  sleep( 1 );

  ctx->start = now();

  status = pthread_create( &thread, &attr, sporadic_server, ctx );
  rtems_test_assert( !status );

  status = pthread_join( thread, NULL );
  rtems_test_assert( !status );

  for ( i = 0 ; i < SS_SAMPLE_PERIODS ; ++i ) {
    printf( "[%zu] H %3" PRIu32 "ms\n", i, ctx->samples[ i ].high );
    printf( "[%zu] L %3" PRIu32 "ms\n", i, ctx->samples[ i ].low );
    rtems_test_assert( ctx->samples[ i ].low / SS_REPL_PERIOD_MS == i + 1 );
  }

  TEST_END();
  rtems_test_exit( 0 );

  return NULL; /* just so the compiler thinks we returned something */
}

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_MAXIMUM_POSIX_THREADS 2

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
