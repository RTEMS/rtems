/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief
 * their priority changes.
 */

/*
 *  Based upon test code posted on the RTEMS User's Mailing List
 *  by Sergio Faustino <sergio.faustino@edisoft.pt>:
 *
 *    http://www.rtems.org/pipermail/rtems-users/2009-June/005540.html
 *
 * Copyright (C) 2008. On-Line Applications Research Corporation (OAR).
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

#include <tmacros.h>

#include <rtems/score/watchdogimpl.h>

const char rtems_test_name[] = "SP 67";

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);
rtems_timer_service_routine TIMER_service_routine(
  rtems_id  ignored_id,
  void     *user_data
);

volatile bool _timer_passage_1 = FALSE;
volatile bool _timer_passage_2 = FALSE;

/*timer Routine*/
rtems_timer_service_routine TIMER_service_routine(
  rtems_id  ignored_id,
  void     *user_data
)
{
  (void) ignored_id;

  bool *passed = (bool *)user_data;
  *passed = TRUE;
}

rtems_task Init(
  rtems_task_argument argument
)
{
  (void) argument;

  rtems_status_code status;
  rtems_id          timer1;
  rtems_id          timer2;

  TEST_BEGIN();

  /* build timer name*/

  /* create Timer */
  puts( "Init - create timer 1" );
  status = rtems_timer_create( rtems_build_name('T', 'M', '1', ' '), &timer1 );
  directive_failed( status, "rtems_timer_create #1" );

  puts( "Init - create timer 2" );
  status = rtems_timer_create( rtems_build_name('T', 'M', '2', ' '), &timer2 );
  directive_failed( status, "rtems_timer_create #1" );

  /* Manipulate the time */
  _Watchdog_Ticks_since_boot = (Watchdog_Interval) -15;

  /* initiate timer server */
  puts( "Init - Initiate the timer server" );
  status = rtems_timer_initiate_server(
    RTEMS_MINIMUM_PRIORITY,
    RTEMS_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_ATTRIBUTES
  );
  directive_failed( status, "rtems_timer_initiate_server" );

  /* Give the timer server some time to initialize */
  status = rtems_task_wake_after( 10 );
  directive_failed( status, "task wake_after" );

  status = rtems_timer_server_fire_after(
    timer1,
    10,
    TIMER_service_routine,
    (void*) &_timer_passage_1
  );
  directive_failed( status, "rtems_timer_server_fire_after" );

  status = rtems_timer_server_fire_after(
    timer2,
    20,
    TIMER_service_routine,
    (void*) &_timer_passage_2
  );
  directive_failed( status, "rtems_timer_server_fire_after" );

  status = rtems_task_wake_after( 15 );
  directive_failed( status, "task wake_after" );

  if (!_timer_passage_1) {
    puts( "Timer 1 FAILED to fire after wrapping time");
    rtems_test_exit(0);
  }
  puts( "Server Timer 1 fired after wrapping ticks since boot-- OK");

  if (_timer_passage_2) {
    puts( "Timer 2 fired and should not have after wrapping time");
    rtems_test_exit(0);
  }

  TEST_END();
  rtems_test_exit(0);
}

/* configuration stuff */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS              2
#define CONFIGURE_MAXIMUM_TIMERS             2

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
