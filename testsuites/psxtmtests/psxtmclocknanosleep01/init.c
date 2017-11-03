/*
 *  COPYRIGHT (c) 1989-2013.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (c) 2016. Gedare Bloom.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <timesys.h>
#include <rtems/btimer.h>
#include "test_support.h"

const char rtems_test_name[] = "PSXTMCLOCKNANOSLEEP 01";

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);

void *POSIX_Init(
  void *argument
)
{
  benchmark_timer_t end_time;
  struct timespec sleepTime;

  sleepTime.tv_sec = 0;
  sleepTime.tv_nsec = 0;

  TEST_BEGIN();

  benchmark_timer_initialize();
    clock_nanosleep( CLOCK_REALTIME, 0, &sleepTime, (struct  timespec *) NULL );
  end_time = benchmark_timer_read();

  put_time( "nanosleep: yield", end_time, 1, 0, 0 );

  TEST_END();

  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER

#define CONFIGURE_MAXIMUM_POSIX_THREADS     1
#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
