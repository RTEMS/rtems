/*
 *  COPYRIGHT (c) 1989-2013.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if !defined(OPERATION_COUNT)
#define OPERATION_COUNT 100
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tmacros.h>
#include <timesys.h>
#include "test_support.h"

#include <pthread.h>

const char rtems_test_name[] = "PSXTMTHREAD 01";

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);
void *TestThread(void *argument);

pthread_t ThreadId;

void *TestThread(
  void *argument
)
{
  return NULL;
}


static void benchmark_pthread_create(
  int    iteration,
  void  *argument
)
{
  int status;

  status = pthread_create( &ThreadId, NULL, TestThread, NULL );
  rtems_test_assert( !status );
}

void *POSIX_Init(
  void *argument
)
{
  TEST_BEGIN();

  rtems_time_test_measure_operation(
    "pthread_create: no preempt",
    benchmark_pthread_create,
    NULL,
    1,
    0
  );


  TEST_END();

  rtems_test_exit(0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER

#define CONFIGURE_MAXIMUM_POSIX_THREADS     OPERATION_COUNT + 2
#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
/* end of file */
