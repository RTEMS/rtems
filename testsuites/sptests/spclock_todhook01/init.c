/**
 * @file
 *
 * @brief Test TOD Set Hook
 *
 * @ingroup sptests
 */

/*
 *  SPDX-License-Identifier: BSD-2-Clause
 *
 *  COPYRIGHT (c) 2019.
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


#include <rtems.h>
#include <rtems/score/todimpl.h>

#include <errno.h>
#include <stdio.h>
#include <time.h>
#include "tmacros.h"

/* #define TEST_DEBUG */

const char rtems_test_name[] = "SPCLOCK TOD HOOK 1";

typedef struct test_case {
  bool       do_settime; 
  bool       use_posix; 
  bool       do_hook1; 
  bool       do_hook2; 
  struct tm  tm;
} testcase_t;

testcase_t Cases[] = {
  /* should not trigger hooks when time not set */
  { false, false, false, false, { 0, 0, 9, 31, 11, 88 } },
  { false, false, true,  true,  { 0, 0, 9, 24, 5,  95 }  },
  /* should trigger hook when time is set with Classic API rtems_clock_set */
  { true,  false, false, false, { 0, 0, 9, 24, 5,   95 }  },
  { true,  false, false, false, { 0, 0, 9, 31, 11,  88 }  },
  { true,  false, true,  false, { 0, 0, 9, 31, 11,  88 }  },
  { true,  false, true,  true,  { 0, 0, 9, 24, 5,  105 }  },
  /* should trigger hook when time is set with POSIX API clock_settime */
  { true,  true,  false, false, { 0, 0, 9, 24, 5,   95 }  },
  { true,  true,  false, false, { 0, 9, 6, 14, 2,  114 }  },
  { true,  true,  true,  false, { 0, 0, 9, 31, 11,  88 }  },
  { true,  true,  true,  true,  { 0, 0, 9, 24, 5,  105 }  },
};

#define NUM_CASES (sizeof(Cases)/sizeof(testcase_t))

static struct timespec tod_set;

static bool hook1_executed;
static bool hook2_executed;

static Status_Control tod_hook1(
  TOD_Action             action,
  const struct timespec *tod
)
{
  rtems_test_assert( action == TOD_ACTION_SET_CLOCK );

  rtems_test_assert( tod->tv_sec == tod_set.tv_sec );
  rtems_test_assert( tod->tv_nsec == tod_set.tv_nsec );

  hook1_executed = true;

  return STATUS_SUCCESSFUL;
}

static Status_Control tod_hook2(
  TOD_Action             action,
  const struct timespec *tod
)
{
  rtems_test_assert( action == TOD_ACTION_SET_CLOCK );

  rtems_test_assert( tod->tv_sec == tod_set.tv_sec );
  rtems_test_assert( tod->tv_nsec == tod_set.tv_nsec );

  hook2_executed = true;

  return STATUS_SUCCESSFUL;
}

/*
 * Execute one positive test case.
 *
 * Assume no hooks registered at begining. Unregister if needed at the end.
 */
static void do_positive_case(int i)
{
  testcase_t  *testcase = &Cases[i];
  TOD_Hook     hook1;
  TOD_Hook     hook2;

  #ifdef TEST_DEBUG
    printf(
      "%d: do_settime=%d use_posix=%d do_hook1=%d do_hook2=%d\n",
      i,
      testcase->do_settime,
      testcase->use_posix,
      testcase->do_hook1,
      testcase->do_hook2
    );
  #endif

  _Chain_Initialize_node( &hook1.Node );
  hook1.handler = tod_hook1;

  _Chain_Initialize_node( &hook2.Node );
  hook2.handler = tod_hook2;

  hook1_executed = false;
  hook2_executed = false;

  /*
   * Register the TOD Hooks
   */
  if ( testcase->do_hook1 == true ) {
    _TOD_Hook_Register( &hook1 );
  }

  if ( testcase->do_hook2 == true ) {
    _TOD_Hook_Register( &hook2 );
  }

  /*
   * Now set the time and if registered, let the handlers fire
   */
  if ( testcase->do_settime == true ) {
    rtems_time_of_day   time;
    rtems_status_code   status;
    struct tm          *tm = &testcase->tm;

    tod_set.tv_sec = mktime( tm );
    tod_set.tv_nsec = 0;

    if ( testcase->use_posix == false ) {
      build_time(
        &time,
        tm->tm_mon + 1,
        tm->tm_mday,
        tm->tm_year + 1900,
        tm->tm_hour,
        tm->tm_min,
        0,
        0
      );
      status = rtems_clock_set( &time );
      directive_failed( status, "rtems_clock_set" );
    } else {
      int rc;

      rc = clock_settime( CLOCK_REALTIME, &tod_set );
      rtems_test_assert( rc == 0 ); 
    }
  }

  /*
   * Unregister the TOD hooks 
   */
  if ( testcase->do_hook1 == true ) {
    _TOD_Hook_Unregister( &hook1 );
  }

  if ( testcase->do_hook2 == true ) {
    _TOD_Hook_Unregister( &hook2 );
  }

  #ifdef TEST_DEBUG
    printf(
      "    hook1_executed=%d hook2_executed=%d\n",
      hook1_executed,
      hook2_executed
    );
  #endif

  /*
   * Check expected results
   */
  if ( testcase->do_hook1 == true ) {
    rtems_test_assert( testcase->do_settime == hook1_executed );
  } else {
    rtems_test_assert( hook1_executed == false );
  }

  if ( testcase->do_hook2 == true ) {
    rtems_test_assert( testcase->do_settime == hook2_executed );
  } else {
    rtems_test_assert( hook2_executed == false );
  }
}

static bool hook_error_executed;

static Status_Control tod_hook_error(
  TOD_Action             action,
  const struct timespec *tod
)
{
  rtems_test_assert( action == TOD_ACTION_SET_CLOCK );

  rtems_test_assert( tod->tv_sec == tod_set.tv_sec );
  rtems_test_assert( tod->tv_nsec == tod_set.tv_nsec );

  hook_error_executed = true;

  return STATUS_NOT_OWNER;
}
/*
 * Execute one negative test case.
 *
 * Assume no hooks registered at begining. Unregister if needed at the end.
 */
static void do_negative_case(bool use_posix)
{
  TOD_Hook            hook_error;
  rtems_time_of_day   time;
  rtems_status_code   status;
  struct tm          *tm = &Cases[0].tm;


  _Chain_Initialize_node( &hook_error.Node );
  hook_error.handler = tod_hook_error;

  hook_error_executed = false;

  /*
   * Register the TOD Hooks
   */
  _TOD_Hook_Register( &hook_error );

  /*
   * Now set the time and if registered, let the handlers fire
   */
  tod_set.tv_sec = mktime( tm );
  tod_set.tv_nsec = 0;

  if ( use_posix == false ) {
    build_time(
      &time,
      tm->tm_mon + 1,
      tm->tm_mday,
      tm->tm_year + 1900,
      tm->tm_hour,
      tm->tm_min,
      0,
      0
    );
    status = rtems_clock_set( &time );
    rtems_test_assert( status == RTEMS_NOT_OWNER_OF_RESOURCE );
  } else {
    int rc;

    rc = clock_settime( CLOCK_REALTIME, &tod_set );
    rtems_test_assert( rc == -1 ); 
    rtems_test_assert( errno == EPERM ); 
  }

  /*
   * Unregister the TOD hooks 
   */
  _TOD_Hook_Unregister( &hook_error );

  /*
   * Check expected results
   */
  rtems_test_assert( hook_error_executed == true );
}


static rtems_task Init(rtems_task_argument ignored)
{
  // rtems_status_code status;
  int               i;

  TEST_BEGIN();

  // test positive cases
  for (i=0 ; i < NUM_CASES ; i++) {
    do_positive_case( i );
  }

  // test error cases
  do_negative_case(false);
  do_negative_case(true);

  TEST_END();

  rtems_test_exit(0);
}

/* configuration information */
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE
#define CONFIGURE_MAXIMUM_TASKS 1

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

