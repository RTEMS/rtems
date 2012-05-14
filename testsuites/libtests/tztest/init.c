/**
 *  @file
 *
 *  This routine is the initialization task of test to exercise some
 *  timezone functionality.
 */

/*
 *  COPYRIGHT (c) 2007-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define CONFIGURE_INIT

#include <tmacros.h>
#include <time.h>
#include <stdlib.h>
 
/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);
void tztester(void);

void tztester(void)
{
  struct tm *tm;
  time_t now;

  printf("TZ:\"%s\"\n", getenv("TZ"));

  time(&now);
  tm = localtime(&now);
  printf ("%4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d\n",
     1900+tm->tm_year, tm->tm_mon+1, tm->tm_mday,
     tm->tm_hour, tm->tm_min, tm->tm_sec);

  tm = gmtime(&now);
  printf ("%4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d\n",
     1900+tm->tm_year, tm->tm_mon+1, tm->tm_mday,
     tm->tm_hour, tm->tm_min, tm->tm_sec);
}

rtems_task Init(
  rtems_task_argument argument
)
{
  rtems_time_of_day time;
  rtems_status_code status;

  puts( "\n\n*** TIMEZONE TEST ***" );

  build_time( &time, 3, 14, 2007, 15, 9, 26, 5 );
  status = rtems_clock_set( &time );
  directive_failed( status, "rtems_clock_set" );

  setenv( "TZ", "CST6CDT,M 3.2.0,M 11.1.0", 1 );

  tzset();

  tztester();

  puts( "*** END OF TIMEZONE TEST ***" );
  exit(0);
}


#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_MAXIMUM_TASKS 1

#include <rtems/confdefs.h>
