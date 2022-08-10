/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file POSIX Timer Test of FACE Behavior
 */

/*
 *  COPYRIGHT (c) 2022. On-Line Applications Research Corporation (OAR).
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

#include <pmacros.h>
#include "tmacros.h"

#include <unistd.h>
#include <errno.h>
#include <sched.h>
#include <time.h>     /* time facilities */
#include <stdio.h>    /* console facilities */

const char rtems_test_name[] = "PSXTIMER FACE 1";

static void *POSIX_Init (
  void *argument
)

{
  struct sigevent   event;
  int               status;
  timer_t           timer;

  /*
   *  If these are not filled in correctly, we do not execute pass the
   *  error checking for a NULL event pointer.
   */
  event.sigev_notify = SIGEV_SIGNAL;
  event.sigev_signo = SIGUSR1;

  TEST_BEGIN();

  /*
   * When FACE timer behavior is configured, creating a POSIX timer
   * using CLOCK_REALTIME is not allowed.
   */
  puts( "timer_create - CLOCK_REALTIME forbidden - EPERM" );
  status = timer_create( CLOCK_REALTIME, &event, &timer );
  fatal_posix_service_status_errno( status, EPERM, "not allowed" );

  /*
   * When FACE timer behavior is configured, creating a POSIX timer
   * on a value other than CLOCK_REALTIME or CLOCK_MONOTONIC is not allowed.
   */
  puts( "timer_create - CLOCK_PROCESS_CPUTIME_ID not allowed - EINVAL" );
  status = timer_create( CLOCK_PROCESS_CPUTIME_ID, &event, &timer );
  fatal_posix_service_status_errno( status, EINVAL, "invalid clock" );

  /*
   * When FACE timer behavior is configured, creating a POSIX timer
   * on CLOCK_MONOTONIC allowed.
   */
  puts( "timer_create - OK" );
  status = timer_create( CLOCK_MONOTONIC, &event, &timer );
  posix_service_failed( status, "timer_create OK" );

  /*
   * Delete the previously created timer.
   */
  puts( "timer_delete - OK" );
  status = timer_delete(  timer );
  posix_service_failed( status, "timer_delete ok" );

  /*
   */
  puts( "timer_create - CLOCK_MONOTONIC is allowed - OK" );
  status = timer_create( CLOCK_MONOTONIC, &event, &timer );
  posix_service_failed( status, "timer_create ok" );

  TEST_END();
  rtems_test_exit (0);
}

/* configuration information */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_POSIX_INIT_THREAD_TABLE

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_MAXIMUM_POSIX_THREADS             1
#define CONFIGURE_MAXIMUM_POSIX_TIMERS              1

#define CONFIGURE_POSIX_TIMERS_FACE_BEHAVIOR

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/* end of include file */
