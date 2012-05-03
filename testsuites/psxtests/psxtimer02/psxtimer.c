/*
 *
 *  This is a simple real-time applications which contains 3 periodic tasks.
 *
 *  Task A is an independent task.
 *
 *  Task B and C share a data.
 *
 *  Tasks are implemented as POSIX threads.
 *
 *  The share data is protected with a POSIX mutex.
 *
 *  Other POSIX facilities such as timers, condition, .. is also used
 *
 *  COPYRIGHT (c) 1989-2009.
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
#include "system.h"
#include "tmacros.h"
#include <signal.h>   /* signal facilities */
#include <unistd.h>   /* sleep facilities */
#include <time.h>     /* time facilities */
#include <stdio.h>    /* console facilities */

void *POSIX_Init (
  void *argument
)

{
  struct timespec   now;
  struct sigevent   event;
  int               status;
  timer_t           timer;
  timer_t           timer1;
  struct itimerspec itimer;

  /*
   *  If these are not filled in correctly, we don't pass its error checking.
   */
  event.sigev_notify = SIGEV_SIGNAL;
  event.sigev_signo = SIGUSR1;

  puts( "\n\n*** POSIX Timers Test 02 ***" );

  puts( "timer_create - bad clock id - EINVAL" );
  status = timer_create( (timer_t) -1, &event, &timer );
  fatal_posix_service_status_errno( status, EINVAL, "bad clock id" );

  puts( "timer_create - bad timer id pointer - EINVAL" );
  status = timer_create( CLOCK_REALTIME, &event, NULL );
  fatal_posix_service_status_errno( status, EINVAL, "bad timer id" );

  puts( "timer_create - OK" );
  status = timer_create( CLOCK_REALTIME, NULL, &timer );
  posix_service_failed( status, "timer_create OK" );

  puts( "timer_create - too many - EAGAIN" );
  status = timer_create( CLOCK_REALTIME, NULL, &timer1 );
  fatal_posix_service_status_errno( status, EAGAIN, "too many" );

  puts( "timer_delete - bad id - EINVAL" );
  status = timer_delete( timer1 + 1 );
  fatal_posix_service_status_errno( status, EINVAL, "bad id" );

  puts( "timer_getoverrun - bad id - EINVAL" );
  status = timer_getoverrun( timer1 + 1 );
  fatal_posix_service_status_errno( status, EINVAL, "bad id" );

  puts( "timer_gettime - bad itimer - EINVAL" );
  status = timer_gettime( timer1, NULL );
  fatal_posix_service_status_errno( status, EINVAL, "bad id" );

  puts( "timer_gettime - bad id - EINVAL" );
  status = timer_gettime( timer1 + 1, &itimer );
  fatal_posix_service_status_errno( status, EINVAL, "bad id" );

  puts( "timer_settime - bad itimer pointer - EINVAL" );
  status = timer_settime( timer, TIMER_ABSTIME, &itimer, NULL );
  fatal_posix_service_status_errno( status, EINVAL, "bad itimer pointer" );

  itimer.it_value.tv_nsec = 2000000000;
  puts( "timer_settime - bad itimer value - too many nanosecond - EINVAL" );
  status = timer_settime( timer, TIMER_ABSTIME, &itimer, NULL );
  fatal_posix_service_status_errno( status, EINVAL, "bad itimer value #1" );

  itimer.it_value.tv_nsec = -1;
  puts( "timer_settime - bad itimer value - negative nanosecond - EINVAL" );
  status = timer_settime( timer, TIMER_ABSTIME, &itimer, NULL );
  fatal_posix_service_status_errno( status, EINVAL, "bad itimer value #2" );

  clock_gettime( CLOCK_REALTIME, &now );
  itimer.it_value = now;
  itimer.it_value.tv_sec = itimer.it_value.tv_sec - 1;
  puts( "timer_settime - bad itimer value - previous time - EINVAL" );
  status = timer_settime( timer, TIMER_ABSTIME, &itimer, NULL );
  fatal_posix_service_status_errno( status, EINVAL, "bad itimer value #3" );

  clock_gettime( CLOCK_REALTIME, &now );
  itimer.it_value = now;
  itimer.it_value.tv_sec = itimer.it_value.tv_sec + 1;
  puts( "timer_settime - bad id - EINVAL" );
  status = timer_settime( timer1, TIMER_ABSTIME, &itimer, NULL );
  fatal_posix_service_status_errno( status, EINVAL, "bad id" );

  itimer.it_value.tv_nsec = 0;
  puts( "timer_settime - bad clock value - EINVAL" );
  status = timer_settime( timer, 0x80, &itimer, NULL );
  fatal_posix_service_status_errno( status, EINVAL, "bad clock value" );

  puts( "timer_delete - OK" );
  status = timer_delete( timer );
  posix_service_failed( status, "timer_delete OK" );

  puts( "timer_delete - bad id - EINVAL" );
  status = timer_delete( timer );
  fatal_posix_service_status_errno( status, EINVAL, "bad id" );

  puts( "*** END OF POSIX Timers Test 02 ***" );
  rtems_test_exit (0);
}
