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

#if defined(USE_TIMER_SERVER)
   #define TEST_NUMBER      "53"
   #define TSR_MODE         "Server"
   #define FIRE_WHEN        rtems_timer_server_fire_when
   #define FIRE_WHEN_STRING "rtems_timer_server_fire_when"
#else
   #define TEST_NUMBER      "52"
   #define TSR_MODE         "Interrupt"
   #define FIRE_WHEN        rtems_timer_fire_when
   #define FIRE_WHEN_STRING "rtems_timer_fire_when"
#endif

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <tmacros.h>
#include <string.h>

const char rtems_test_name[] = "SP " TEST_NUMBER;

#define INITIAL_YEAR 2009

static bool _timer_passage;

static rtems_time_of_day time_to_fire;

/*timer Routine*/
static rtems_timer_service_routine TIMER_service_routine(
  rtems_id  ignored_id,
  void     *user_data
)
{
  (void) ignored_id;
  (void) user_data;

  rtems_status_code status;
  rtems_time_of_day now;

  _timer_passage = true;

  memset( &now, 0, sizeof( now ) );

  status = rtems_clock_get_tod( &now );
  rtems_test_assert( status == RTEMS_SUCCESSFUL );
  rtems_test_assert( memcmp( &now, &time_to_fire, sizeof( now ) ) == 0 );
}

static rtems_task Init(
  rtems_task_argument argument
)
{
  (void) argument;

  rtems_status_code status;
  rtems_id          timer_id;
  rtems_name        timer_name;

  rtems_time_of_day global_time;

  TEST_BEGIN();

  /* build timer name*/
  timer_name = rtems_build_name('T', 'M', '1', ' ');

  /* create Timer */
  status = rtems_timer_create(timer_name, &timer_id);
  directive_failed( status, "rtems_timer_create" );

  #if defined(USE_TIMER_SERVER)
    /* initiate timer server */
    status = rtems_timer_initiate_server(
      RTEMS_MINIMUM_PRIORITY,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_ATTRIBUTES
    );
    directive_failed( status, "rtems_timer_initiate_server" );
  #endif

  /* Set system clock  */
  build_time(&global_time, 6, 8, INITIAL_YEAR, 16, 5, 13, 0);
  status = rtems_clock_set(&global_time);
  directive_failed( status, "rtems_clock_set" );

  /* Set Timer to Fire */
  /* build fire times */
  time_to_fire = global_time;

  /* only diferent second */
  time_to_fire.year = INITIAL_YEAR + 5;

  status = FIRE_WHEN(
    timer_id,
    &time_to_fire,
    TIMER_service_routine,
    (void*) NULL
  );
  directive_failed( status, FIRE_WHEN_STRING );

  /* Set system clock FORWARD */
  global_time.year = time_to_fire.year;
  status = rtems_clock_set(&global_time);

  if (!_timer_passage) {
    puts( TSR_MODE " Timer FAILED to fire after setting time forward");
    rtems_test_exit(0);
  }

  puts( TSR_MODE " Timer fired after setting time forward -- OK");

  TEST_END();
  rtems_test_exit(0);
}

/* configuration stuff */

#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MICROSECONDS_PER_TICK 50000

#define CONFIGURE_MAXIMUM_TASKS              2
#define CONFIGURE_MAXIMUM_TIMERS             1

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_PRIORITY (RTEMS_MINIMUM_PRIORITY + 1)
#define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_DEFAULT_MODES

#define CONFIGURE_INIT
#include <rtems/confdefs.h>
