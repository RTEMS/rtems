/*
 *  COPYRIGHT (c) 2016.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "tmacros.h"
#include <pthread.h>
#include <time.h>
#include <errno.h>

const char rtems_test_name[] = "PSXCOND 2";

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);

static void test_exercise_clock_attribute_errors(void)
{
  int                 sc;
  pthread_condattr_t  attr;
  clockid_t           clock;

  /* NULL attribute pointer errors */
  puts( "pthread_condattr_getclock (NULL attribute) - EINVAL" );
  sc = pthread_condattr_getclock( NULL, &clock );
  fatal_posix_service_status( sc, EINVAL, "condattr getclock" );

  puts( "pthread_condattr_getclock (NULL clock) - EINVAL" );
  sc = pthread_condattr_getclock( &attr, NULL );
  fatal_posix_service_status( sc, EINVAL, "condattr getclock" );

  puts( "pthread_condattr_setclock (NULL attribute) - EINVAL" );
  sc = pthread_condattr_setclock( NULL, CLOCK_REALTIME );
  fatal_posix_service_status( sc, EINVAL, "condattr setclock" );

  /* invalid clock errors */
  puts( "pthread_condattr_init - OK" );
  sc = pthread_condattr_init( &attr );
  fatal_posix_service_status( sc, 0, "condattr init" );

  puts( "pthread_condattr_setclock (bad clock)- EINVAL" );
  sc = pthread_condattr_setclock( &attr, -1 );
  fatal_posix_service_status( sc, EINVAL, "condattr setclock" );

  puts( "pthread_condattr_setclock (CLOCK_PROCESS_CPUTIME_ID)- EINVAL" );
  sc = pthread_condattr_setclock( &attr, CLOCK_PROCESS_CPUTIME_ID );
  fatal_posix_service_status( sc, EINVAL, "condattr setclock" );

  puts( "pthread_condattr_setclock (CLOCK_THREAD_CPUTIME_ID)- EINVAL" );
  sc = pthread_condattr_setclock( &attr, CLOCK_THREAD_CPUTIME_ID );
  fatal_posix_service_status( sc, EINVAL, "condattr setclock" );
}

static void test_exercise_clock_attribute(void)
{
  int                 sc;
  pthread_condattr_t  attr;

  puts( "pthread_condattr_init - OK" );
  sc = pthread_condattr_init( &attr );
  fatal_posix_service_status( sc, 0, "condattr init" );

  puts( "pthread_condattr_setclock (CLOCK_REALTIME)- OK" );
  sc = pthread_condattr_setclock( &attr, CLOCK_REALTIME );
  fatal_posix_service_status( sc, 0, "condattr setclock" );

  puts( "pthread_condattr_setclock (CLOCK_MONOTONIC)- OK" );
  sc = pthread_condattr_setclock( &attr, CLOCK_MONOTONIC );
  fatal_posix_service_status( sc, 0, "condattr setclock" );
}

void *POSIX_Init(
  void *argument
)
{
  TEST_BEGIN();

  test_exercise_clock_attribute_errors();

  test_exercise_clock_attribute();

  TEST_END();
  rtems_test_exit( 0 );

  return NULL; /* just so the compiler thinks we returned something */
}

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_MAXIMUM_POSIX_THREADS 1

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
