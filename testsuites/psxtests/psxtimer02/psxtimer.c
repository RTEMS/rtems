/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file POSIX Timer Test #2
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
 *  Other POSIX facilities such as timers, condition, .. are also used.
 */

/*
 *  COPYRIGHT (c) 1989-2009.
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

#define CONFIGURE_INIT
#include "system.h"
#include "tmacros.h"
#include <signal.h>   /* signal facilities */
#include <unistd.h>   /* sleep facilities */
#include <time.h>     /* time facilities */
#include <stdio.h>    /* console facilities */

const char rtems_test_name[] = "PSXTIMER 2";

void *POSIX_Init (
  void *argument
)
{
  (void) argument;

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

  TEST_BEGIN();

  puts( "timer_create - bad clock id - EINVAL" );
  status = timer_create( (clockid_t) -1, &event, &timer );
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

  puts( "timer_create (monotonic) - bad timer id pointer - EINVAL" );
  status = timer_create( CLOCK_MONOTONIC, &event, NULL );
  fatal_posix_service_status_errno( status, EINVAL, "bad timer id" );

  puts( "timer_create (monotonic) - OK" );
  status = timer_create( CLOCK_MONOTONIC, NULL, &timer );
  posix_service_failed( status, "timer_create OK" );

  puts( "timer_create (monotonic) - too many - EAGAIN" );
  status = timer_create( CLOCK_MONOTONIC, NULL, &timer1 );
  fatal_posix_service_status_errno( status, EAGAIN, "too many" );

  clock_gettime( CLOCK_MONOTONIC, &now );
  itimer.it_value = now;
  itimer.it_value.tv_sec = itimer.it_value.tv_sec - 1;
  puts( "timer_settime (monotonic) - bad itimer value - previous time - EINVAL" );
  status = timer_settime( timer, TIMER_ABSTIME, &itimer, NULL );
  fatal_posix_service_status_errno( status, EINVAL, "bad itimer value #3" );

  clock_gettime( CLOCK_MONOTONIC, &now );
  itimer.it_value = now;
  itimer.it_value.tv_sec = itimer.it_value.tv_sec + 1;
  puts( "timer_settime (monotonic) - bad id - EINVAL" );
  status = timer_settime( timer1, TIMER_ABSTIME, &itimer, NULL );
  fatal_posix_service_status_errno( status, EINVAL, "bad id" );

  TEST_END();
  rtems_test_exit (0);
}
