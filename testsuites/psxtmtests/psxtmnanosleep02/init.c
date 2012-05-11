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

#include <timesys.h>
#include <rtems/timerdrv.h>
#include "test_support.h"

#include <pthread.h>

/* forward declarations to avoid warnings */
void *POSIX_Init(void *argument);
void *Middle(void *argument);
void *Low(void *argument);

void *Low(
  void *argument
)
{
  benchmark_timer_t end_time;

  end_time = benchmark_timer_read();

  put_time(
    "nanosleep - blocking",
    end_time,
    OPERATION_COUNT,
    0,
    0
  );

  puts( "*** END OF POSIX TIME TEST PSXTMNANOSLEEP02 ***" );

  rtems_test_exit( 0 );
  return NULL;
}

void *Middle(
  void *argument
)
{
  /* calling nanosleep */
  struct timespec sleepTime;
  sleepTime.tv_sec = 0;
  sleepTime.tv_nsec = 1;

  nanosleep(&sleepTime, (struct  timespec *) NULL);

  return NULL;
}

void *POSIX_Init(
  void *argument
)
{
  int        i;
  int        status;
  pthread_t  threadId;
  struct timespec sleepTime;
  struct timespec remainder;

  sleepTime.tv_sec = 0;
  sleepTime.tv_nsec = 1;
  remainder.tv_sec = 0;
  remainder.tv_nsec = 0;

  puts( "\n\n*** POSIX TIME TEST PSXTMNANOSLEEP02 ***" );

  for ( i=0 ; i < OPERATION_COUNT - 1 ; i++ ) {
    status = pthread_create( &threadId, NULL, Middle, NULL );
    rtems_test_assert( !status );
  }
  
  status = pthread_create( &threadId, NULL, Low, NULL );
  rtems_test_assert( !status );

  /* start the timer and switch through all the other tasks */
  benchmark_timer_initialize();
  /* calling nanosleep*/
  nanosleep(&sleepTime, &remainder);

  return NULL;
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER

#define CONFIGURE_MAXIMUM_POSIX_THREADS     OPERATION_COUNT + 2
#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
  /* end of file */
