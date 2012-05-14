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
#include <time.h>
#include <errno.h>

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);

rtems_task Init(
  rtems_task_argument argument
)
{
  struct tm       tm;
  struct timespec tv;
  useconds_t      remaining;
  int             sc;

  puts( "\n\n*** POSIX USLEEP TEST ***" );

  tm_build_time( &tm, TM_FRIDAY, TM_MAY, 24, 96, 11, 5, 0 );

  /* set the time of day, and print our buffer in multiple ways */

  tv.tv_nsec = 0;
  tv.tv_sec = mktime( &tm );
  rtems_test_assert( tv.tv_sec != -1 );

  /* now set the time of day */

  printf( asctime( &tm ) );
  puts( "Init: clock_settime - SUCCESSFUL" );
  sc = clock_settime( CLOCK_REALTIME, &tv );
  rtems_test_assert( !sc );

  printf( asctime( &tm ) );
  printf( ctime( &tv.tv_sec ) );

  /* use sleep to delay */

  remaining = usleep( 3 * 1000000 );
  rtems_test_assert( !remaining );

  sc = clock_gettime( CLOCK_REALTIME, &tv );
  rtems_test_assert( !sc );

  printf( ctime( &tv.tv_sec ) );

  puts( "*** END OF POSIX USLEEP TEST ***" );
  rtems_test_exit(0);
}


/* configuration information */
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_MAXIMUM_TASKS             1

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
